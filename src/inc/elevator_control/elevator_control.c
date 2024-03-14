#include "elevator_control.h"

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

void poll_stopped_elevators(elevator_status_t* elevator){
  for(uint8_t i = 0; i < MAX_IP_NODES; i++){
    if(elevator[i].alive){
      int temp = elevator_hardware_get_stop_signal(&elevator[i].elevator);
      //printf("elevator %d %s is alive: %d\n",i, elevator[i].elevator.ip, elevator[i].alive);
      if(temp == 1){
        if(elevator[i].stop == false && elevator[i].number_of_stop_readings == 0){
          printf("Elevator %s has stopped\n", elevator[i].elevator.ip);
          pthread_mutex_lock(&elevator[i].mutex);
          elevator[i].stop = true;
          pthread_mutex_unlock(&elevator[i].mutex);
        }
         else if(elevator[i].number_of_stop_readings >= NUMBER_OF_STOP_READINGS && elevator[i].stop == true){
          printf("Restarting elevator %s\n", elevator[i].elevator.ip);
          pthread_mutex_lock(&elevator[i].mutex);
          elevator[i].stop = false;
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
  usleep(ORDER_POLL_DELAY);
}

void update_elevator_floor_lights(elevator_status_t* elevator){
  for(uint8_t i = 0; i < MAX_IP_NODES; i++){
    if(elevator[i].alive){
      elevator_hardware_set_floor_indicator(elevator[i].floor, &elevator[i].elevator);
    }
  }
  usleep(ORDER_POLL_DELAY);
}

void update_elevator_stop_light(elevator_status_t* elevator){
  for(uint8_t i = 0; i < MAX_IP_NODES; i++){
    if(elevator[i].alive){
      elevator_hardware_set_stop_lamp(elevator[i].stop, &elevator[i].elevator);
    }
  }
  usleep(ORDER_POLL_DELAY);
}

void update_elevator_door_light(elevator_status_t* elevator){
  for(uint8_t i = 0; i < MAX_IP_NODES; i++){
    if(elevator[i].alive){
      elevator_hardware_set_door_open_lamp(elevator[i].door_open, &elevator[i].elevator);
    }
  }
  usleep(ORDER_POLL_DELAY);
}

void add_elevator_button_lights(button_lights_history_t* button_lights, order_queue_t* queue, elevator_status_t* elevator){
  for(int j = 0; j < NO_FLOORS; j++){
    button_lights->new->up->floors[j] = false;
    button_lights->new->down->floors[j] = false;
    for(int i = 0; i < MAX_IP_NODES; i++){
      button_lights->new->cab[i]->floors[j] = false;
    }
  }

  for(int i = 0; i < queue->size; i++){
    if(queue->orders[i].order_status == SYNCED){
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