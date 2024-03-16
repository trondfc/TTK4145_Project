/**
 * @file elevator_control.c
 * @brief functions for controlling elevator input and output
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "elevator_control.h"

/**
 * @brief initializes a button_lights_history_t struct with malloc 
 * 
 * @return button_lights_history_t* 
 */
button_lights_history_t* button_light_struct_init(){
  button_lights_history_t* _button_lights = (button_lights_history_t*)malloc(sizeof(button_lights_history_t));
  _button_lights->old = (button_lights_t*)malloc(sizeof(button_lights_t));
  _button_lights->old->up = (floor_buttons_t*)malloc(NO_FLOORS * sizeof(bool));
  _button_lights->old->down = (floor_buttons_t*)malloc(NO_FLOORS * sizeof(bool));
  for(int i = 0; i < MAX_IP_NODES; i++){
    _button_lights->old->cab[i] = (floor_buttons_t*)malloc(NO_FLOORS * sizeof(bool));
  }
  _button_lights->old->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));

  _button_lights->new = (button_lights_t*)malloc(sizeof(button_lights_t));
  _button_lights->new->up = (floor_buttons_t*)malloc(NO_FLOORS * sizeof(bool));
  _button_lights->new->down = (floor_buttons_t*)malloc(NO_FLOORS * sizeof(bool));
  for(int i = 0; i < MAX_IP_NODES; i++){
    _button_lights->new->cab[i] = (floor_buttons_t*)malloc(NO_FLOORS * sizeof(bool));
  }
  _button_lights->new->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));

  return _button_lights;
}

/**
 * @brief poll all elevators for stop signal
 *        if stop signal is detected, the elevator is set to stop
 *        if elevator is stopped, and stop signal is held for a certain amount of time, the elevator is restarted
 * 
 *        if the elevator is not responding, the elevator is set to not alive
 * 
 * @param elevator the elevator_status_t struct to be polled
 */
void poll_stopped_elevators(elevator_status_t* elevator){
  for(uint8_t i = 0; i < MAX_IP_NODES; i++){
    if(elevator[i].alive){
      int temp = elevator_hardware_get_stop_signal(&elevator[i].elevator);
      //printf("elevator %d %s is alive: %d\n",i, elevator[i].elevator.ip, elevator[i].alive);
      if(temp == 1){
        if(elevator[i].emergency_stop == false && elevator[i].number_of_stop_readings == 0){
          printf("Elevator %s has stopped\n", elevator[i].elevator.ip);
          pthread_mutex_lock(&elevator[i].mutex);
          elevator[i].emergency_stop = true;
          pthread_mutex_unlock(&elevator[i].mutex);
        }
         else if(elevator[i].number_of_stop_readings >= NUMBER_OF_STOP_READINGS && elevator[i].emergency_stop == true){
          printf("Restarting elevator %s\n", elevator[i].elevator.ip);
          pthread_mutex_lock(&elevator[i].mutex);
          elevator[i].emergency_stop = false;
          pthread_mutex_unlock(&elevator[i].mutex);
        }
        pthread_mutex_lock(&elevator[i].mutex);
        elevator[i].number_of_stop_readings++;
        pthread_mutex_unlock(&elevator[i].mutex);
      } 
      else if(temp == 0){
        pthread_mutex_lock(&elevator[i].mutex);
        if(elevator[i].number_of_stop_readings > 0){
          elevator[i].number_of_stop_readings--;
        }
        pthread_mutex_unlock(&elevator[i].mutex);
      }
      else if(temp == -1){ // Retunrs -1 if the elevator is not responding
        printf("Elevator %s is not responding\n", elevator[i].elevator.ip);
        pthread_mutex_lock(&elevator[i].mutex);
        elevator[i].alive = false;
        pthread_mutex_unlock(&elevator[i].mutex);
      } 
    }
  }
  usleep(ORDER_POLL_DELAY);
}

/**
 * @brief poll all elevators for obstruction signal
 * 
 * @param elevator the elevator_status_t struct to be polled
 */
void poll_obstructed_elevators(elevator_status_t* elevator){
  for(uint8_t i = 0; i < MAX_IP_NODES; i++){
    if(elevator[i].alive){
      
      if(elevator_hardware_get_obstruction_signal(&elevator[i].elevator)){
        //printf("Elevator %s is obstructed\n", elevator[i].elevator.ip);
        pthread_mutex_lock(&elevator[i].mutex);
        elevator[i].obstruction = true;
        pthread_mutex_unlock(&elevator[i].mutex);

      }
      else{
        //printf("Elevator %s is not obstructed\n", elevator[i].elevator.ip);
        pthread_mutex_lock(&elevator[i].mutex);
        elevator[i].obstruction = false;
        pthread_mutex_unlock(&elevator[i].mutex);
      }
    }
  }
  usleep(ORDER_POLL_DELAY);
}

/**
 * @brief poll all elevators for floor sensor signal
 * 
 * @param elevator the elevator_status_t struct to be polled
 */
void poll_elevator_floor(elevator_status_t* elevator){
  for(uint8_t i = 0; i < MAX_IP_NODES; i++){
    if(elevator[i].alive){
      int temp = elevator_hardware_get_floor_sensor_signal(&elevator[i].elevator);
      if(temp != -1){
        pthread_mutex_lock(&elevator[i].mutex);
        elevator[i].floor = temp;
        elevator[i].at_floor = true;
        pthread_mutex_unlock(&elevator[i].mutex);
      }
      else{
        pthread_mutex_lock(&elevator[i].mutex);
        elevator[i].at_floor = false;
        pthread_mutex_unlock(&elevator[i].mutex);
      }
    }
  }
}

/**
 * @brief update all elevators floor lights based on the floor sensor signal
 * 
 * @param elevator the elevator_status_t struct to be updated
 */
void update_elevator_floor_lights(elevator_status_t* elevator){
  for(uint8_t i = 0; i < MAX_IP_NODES; i++){
    if(elevator[i].alive){
      elevator_hardware_set_floor_indicator(elevator[i].floor, &elevator[i].elevator);
    }
  }
  usleep(ORDER_POLL_DELAY);
}

/**
 * @brief update all elevators stop lights based on the stop signal
 * 
 * @param elevator the elevator_status_t struct to be updated
 */
void update_elevator_stop_light(elevator_status_t* elevator){
  for(uint8_t i = 0; i < MAX_IP_NODES; i++){
    if(elevator[i].alive){
      elevator_hardware_set_stop_lamp(elevator[i].emergency_stop, &elevator[i].elevator);
    }
  }
  usleep(ORDER_POLL_DELAY);
}

/**
 * @brief update all elevators door lights based on the door open signal
 * 
 * @param elevator the elevator_status_t struct to be updated
 */
void update_elevator_door_light(elevator_status_t* elevator){
  for(uint8_t i = 0; i < MAX_IP_NODES; i++){
    if(elevator[i].alive){
      elevator_hardware_set_door_open_lamp(elevator[i].door_open, &elevator[i].elevator);
    }
  }
  usleep(ORDER_POLL_DELAY);
}

/**
 * @brief set all new button lights in button_lights_history_t struct based on the order_queue_t struct
 * 
 * @param button_lights struct to be updated
 * @param queue  struct to be read
 * @param elevator struct to be read
 */
void add_elevator_button_lights(button_lights_history_t* button_lights, order_queue_t* queue, elevator_status_t* elevator){
  for(int j = 0; j < NO_FLOORS; j++){
    button_lights->new->up->floors[j] = false;
    button_lights->new->down->floors[j] = false;
    for(int i = 0; i < MAX_IP_NODES; i++){
      button_lights->new->cab[i]->floors[j] = false;
    }
  }

  keep_alive_node_list_t* node_list = get_node_list();

  for(int i = 0; i < queue->size; i++){

    if(queue->orders[i].order_status >= SYNCED ||node_list->single_master){
      //printf("Adding order %ld to button lights\n", queue->orders[i].order_id);
      if(queue->orders[i].order_type == 0){
        button_lights->new->up->floors[queue->orders[i].floor] = true;
      }
      else if(queue->orders[i].order_type == 1){
        button_lights->new->down->floors[queue->orders[i].floor] = true;
      }
      else if(queue->orders[i].order_type == 2){
        for(int j = 0; j < MAX_IP_NODES; j++){
          if(strcmp(queue->orders[i].elevator_id, elevator[j].elevator.ip) == 0){
            button_lights->new->cab[j]->floors[queue->orders[i].floor] = true;
          }
        }
      }
    }
  }
}

/**
 * @brief Set the changed button lights, comparing the new and old button lights in the button_lights_history_t struct
 * 
 * @param button_lights struct with new and old button lights
 * @param elevator struct of elevators to be updated
 */
void set_changed_button_lights(button_lights_history_t* button_lights, elevator_status_t* elevator){
    for(int i = 0; i < NO_FLOORS; i++){
      //printf("floor up %d: new: %d old: %d\n", i, button_lights->new->up->floors[i], button_lights->old->up->floors[i]);
      if(button_lights->new->up->floors[i] != button_lights->old->up->floors[i]){
        //printf("Button light up %d has changed\n", i);
        for(int j = 0; j < MAX_IP_NODES; j++){
          if(elevator[j].alive){
            elevator_hardware_set_button_lamp(0, i, button_lights->new->up->floors[i], &elevator[j].elevator);
          }
        }
      }
      if(button_lights->new->down->floors[i] != button_lights->old->down->floors[i]){
        //printf("Button light down %d has changed\n", i);
        for(int j = 0; j < MAX_IP_NODES; j++){
          if(elevator[j].alive){
            elevator_hardware_set_button_lamp(1, i, button_lights->new->down->floors[i], &elevator[j].elevator);
          }
        }
      }
      for(int j = 0; j < MAX_IP_NODES; j++){
        if(button_lights->new->cab[j]->floors[i] != button_lights->old->cab[j]->floors[i]){
          //printf("Button light cab %d %d has changed\n", j, i);
          if(elevator[j].alive){
            elevator_hardware_set_button_lamp(2, i, button_lights->new->cab[j]->floors[i], &elevator[j].elevator);
          }
        }
      }
    }
    memcpy(button_lights->old->up, button_lights->new->up, sizeof(floor_buttons_t));
    memcpy(button_lights->old->down, button_lights->new->down, sizeof(floor_buttons_t));
    for(int i = 0; i < MAX_IP_NODES; i++){
      memcpy(button_lights->old->cab[i], button_lights->new->cab[i], sizeof(floor_buttons_t));
    }
}

/**
 * @brief Set all button lights, based on the new button lights in the button_lights_history_t struct
 *        needed to set non changed button lights to resently added elevators
 * 
 * @param button_lights struct button light values
 * @param elevator struct of elevators to be updated
 */
void sett_all_button_lights(button_lights_history_t* button_lights, elevator_status_t* elevator){
    for(int i = 0; i < NO_FLOORS; i++){
        for(int j = 0; j < MAX_IP_NODES; j++){
          if(elevator[j].alive){
            elevator_hardware_set_button_lamp(0, i, button_lights->new->up->floors[i], &elevator[j].elevator);
          }
        }
        for(int j = 0; j < MAX_IP_NODES; j++){
          if(elevator[j].alive){
            elevator_hardware_set_button_lamp(1, i, button_lights->new->down->floors[i], &elevator[j].elevator);
          }
        }
      for(int j = 0; j < MAX_IP_NODES; j++){
          if(elevator[j].alive){
            elevator_hardware_set_button_lamp(2, i, button_lights->new->cab[j]->floors[i], &elevator[j].elevator);
          }
      }
    }
    memcpy(button_lights->old->up, button_lights->new->up, sizeof(floor_buttons_t));
    memcpy(button_lights->old->down, button_lights->new->down, sizeof(floor_buttons_t));
    for(int i = 0; i < MAX_IP_NODES; i++){
      memcpy(button_lights->old->cab[i], button_lights->new->cab[i], sizeof(floor_buttons_t));
    }
}

void set_motor_direction(elevator_status_t* elevator){
  for(int i = 0; i < MAX_IP_NODES; i++){
    if(elevator[i].alive){

      if(elevator[i].door_open){
        elevator_hardware_set_motor_direction(DIRN_STOP, &elevator[i].elevator);
      }
      else if(elevator[i].elevator_state == STOP){
        if(!elevator[i].at_floor){
          if(elevator[i].floor < NO_FLOORS - 1){
            elevator_hardware_set_motor_direction(DIRN_UP, &elevator[i].elevator);
          }
          else if(elevator[i].floor > 0){
            elevator_hardware_set_motor_direction(DIRN_DOWN, &elevator[i].elevator);
          }
        }
        else{
          elevator_hardware_set_motor_direction(DIRN_STOP, &elevator[i].elevator);
        }
      }
      else if(elevator[i].elevator_state == UP || elevator[i].elevator_state == TRANSPORT_UP){
        elevator_hardware_set_motor_direction(DIRN_UP, &elevator[i].elevator);
      }
      else if(elevator[i].elevator_state == DOWN || elevator[i].elevator_state == TRANSPORT_DOWN){
        elevator_hardware_set_motor_direction(DIRN_DOWN, &elevator[i].elevator);
      }
    }
  }
}