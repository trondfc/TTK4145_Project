#include<unistd.h>
#include<stdio.h> 

/* Include libraries */
#include "inc/elevator_control/elevator_button_inputs.h"
#include "inc/elevator_hardware/elevator_hardware.h"
#include "inc/order_queue/orderQueue.h"
#include "inc/process_pair/process_pair.h"
#include "inc/order_queue/send_order_queue.h"
#include "inc/keep_alive/keep_alive.h"
#include "inc/logger.h"
#include "inc/types.h"
#include "inc/config.h"
#include "inc/elevator_control/elevator_autofind.h"
#define LOG_LEVEL LOG_LEVEL_DEBUG // Set log level to debug


#define QUEUE_SIZE MAX_QUEUE_SIZE
#define ORDER_POLL_DELAY 50 * 1000 // 10 ms
#define ORDER_SYNC_DELAY 500 * 1000 // 500 ms
#define NUMBER_OF_STOP_READINGS 5

/* Define global variables */
order_queue_t *queue;
elevator_status_t *g_elevator;
button_lights_history_t *button_lights;


void messageReceived(const char * ip, char * data, int datalength){

  //printf("Received message from %s\n",ip);

  send_order_queue_deserialize(data, queue);

  printf("Queue size: %d\n", queue->size);
  //printf("Queue capacity: %d\n", queue->capacity);
  for(int i = 0; i < queue->size; i++){
    printf("\tOrder %d: %ld from flor %d going %d. Status: %s \n", 
          i, 
          queue->orders[i].order_id, 
          queue->orders[i].floor, 
          queue->orders[i].order_type, 
          queue->orders[i].order_status == RECIVED ? "RECIVED" : "SYNCED");
  }
    
}


/* Callback function for tcp connupdate*/
void connectionStatus(const char * ip, int status){
  printf("A connection got updated %s: %d\n",ip,status);
  keep_alive_node_list_t* node_list = get_node_list();
  for(uint8_t i = 0; i < MAX_IP_NODES; i++){
    if(strcmp(node_list->nodes[i].ip, ip) == 0){
      if(status == 1){
        node_list->nodes[i].connection = CONNECTED;
        printf("Connection is now %d\n", node_list->nodes[i].connection);
      }else{
        node_list->nodes[i].connection = DISCONNECTED;
        printf("Connection is now %d\n", node_list->nodes[i].connection);
      }
    }
  }
}

int main_init(){
  printf("main_init\n");
  sysQueInit(5);
  send_order_queue_init(messageReceived, connectionStatus);
  g_elevator =  elevator_struct_init();
/*
  button_lights = (button_lights_t*)malloc(sizeof(button_lights_t));
  button_lights->up = (bool*)malloc(NO_FLOORS * sizeof(bool));
  button_lights->down = (bool*)malloc(NO_FLOORS * sizeof(bool));
  for(int i = 0; i < MAX_IP_NODES; i++){
    button_lights->cab[i] = (bool*)malloc(NO_FLOORS * sizeof(bool));
  }
*/
  button_lights = (button_lights_history_t*)malloc(sizeof(button_lights_history_t));
  button_lights->old = (button_lights_t*)malloc(sizeof(button_lights_t));
  button_lights->old->up = (floor_buttons_t*)malloc(NO_FLOORS * sizeof(bool));
  button_lights->old->down = (floor_buttons_t*)malloc(NO_FLOORS * sizeof(bool));
  for(int i = 0; i < MAX_IP_NODES; i++){
    button_lights->old->cab[i] = (floor_buttons_t*)malloc(NO_FLOORS * sizeof(bool));
  }
  button_lights->old->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));

  button_lights->new = (button_lights_t*)malloc(sizeof(button_lights_t));
  button_lights->new->up = (floor_buttons_t*)malloc(NO_FLOORS * sizeof(bool));
  button_lights->new->down = (floor_buttons_t*)malloc(NO_FLOORS * sizeof(bool));
  for(int i = 0; i < MAX_IP_NODES; i++){
    button_lights->new->cab[i] = (floor_buttons_t*)malloc(NO_FLOORS * sizeof(bool));
  }
  button_lights->new->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));

  printf("sysQueInit\n");

  queue = create_order_queue(QUEUE_SIZE);

  return 0;
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
/*
void update_elevator_button_lights(elevator_status_t* elevator, order_queue_t* queue){
  for(uint8_t i = 0; i < MAX_IP_NODES; i++){
    if(elevator[i].alive){
      //printf("Updating elevator %s\n", elevator[i].elevator.ip);
      for(int j = 0; j < queue->size; j++){
        if(queue->orders[j].order_status == SYNCED){
          printf("Order %ld is synced\n", queue->orders[j].order_id);
          if(queue->orders[j].order_type == 0 || queue->orders[j].order_type == 1){
            if(elevator_hardware_set_button_lamp(queue->orders[j].order_type, queue->orders[j].floor, 1, &elevator[i].elevator)){
              printf("\t\t\t\tsetting button lamp %d %d\n", queue->orders[j].order_type, queue->orders[j].floor);
              queue->orders[j].order_status = NOTIFIED;
            }
          }
          else if(queue->orders[j].order_type == 2){
            //if(queue->orders[j].elevator_id == elevator[i].elevator.ip){
            if(strcmp(queue->orders[j].elevator_id, elevator[i].elevator.ip) == 0){
              printf("setting button lamp %d %d on %s\n", queue->orders[j].order_type, queue->orders[j].floor, elevator[i].elevator.ip);
              elevator_hardware_set_button_lamp(queue->orders[j].order_type, queue->orders[j].floor, 1, &elevator[i].elevator);
            }
          }
        }
        usleep(ORDER_POLL_DELAY/2);
      }
    }
  }
}*/

void add_elevator_button_lights(){
  for(int j = 0; j < NO_FLOORS; j++){
    button_lights->new->up->floors[j] = false;
    button_lights->new->down->floors[j] = false;
    for(int i = 0; i < MAX_IP_NODES; i++){
      button_lights->new->cab[i]->floors[j] = false;
    }
  }

  for(int i = 0; i < queue->size; i++){
    if(queue->orders[i].order_status == SYNCED){
      printf("Adding order %ld to button lights\n", queue->orders[i].order_id);
      if(queue->orders[i].order_type == 0){
        button_lights->new->up->floors[queue->orders[i].floor] = true;
      }
      else if(queue->orders[i].order_type == 1){
        button_lights->new->down->floors[queue->orders[i].floor] = true;
      }
      else if(queue->orders[i].order_type == 2){
        for(int j = 0; j < MAX_IP_NODES; j++){
          if(strcmp(queue->orders[i].elevator_id, g_elevator[j].elevator.ip) == 0){
            button_lights->new->cab[j]->floors[queue->orders[i].floor] = true;
          }
        }
      }
    }
  }
}

void* controll_elevator_button_lights(void* arg){
  elevator_status_t* elevator = g_elevator;
  while(1){
    for(int i = 0; i < NO_FLOORS; i++){
      printf("floor up %d: new: %d old: %d\n", i, button_lights->new->up->floors[i], button_lights->old->up->floors[i]);
      if(button_lights->new->up->floors[i] != button_lights->old->up->floors[i]){
        printf("Button light up %d has changed\n", i);
        for(int j = 0; j < MAX_IP_NODES; j++){
          if(elevator[j].alive){
            elevator_hardware_set_button_lamp(0, i, button_lights->new->up->floors[i], &elevator[j].elevator);
          }
        }
      }
      if(button_lights->new->down->floors[i] != button_lights->old->down->floors[i]){
        printf("Button light down %d has changed\n", i);
        for(int j = 0; j < MAX_IP_NODES; j++){
          if(elevator[j].alive){
            elevator_hardware_set_button_lamp(1, i, button_lights->new->down->floors[i], &elevator[j].elevator);
          }
        }
      }
      for(int j = 0; j < MAX_IP_NODES; j++){
        if(button_lights->new->cab[j]->floors[i] != button_lights->old->cab[j]->floors[i]){
          printf("Button light cab %d %d has changed\n", j, i);
          if(elevator[j].alive){
            elevator_hardware_set_button_lamp(2, i, button_lights->new->cab[j]->floors[i], &elevator[j].elevator);
          }
        }
      }
    }
    //button_lights->old = button_lights->new;
    //memcpy(button_lights->old, button_lights->new, sizeof(button_lights_t));
    memcpy(button_lights->old->up, button_lights->new->up, sizeof(floor_buttons_t));
    memcpy(button_lights->old->down, button_lights->new->down, sizeof(floor_buttons_t));
    for(int i = 0; i < MAX_IP_NODES; i++){
      memcpy(button_lights->old->cab[i], button_lights->new->cab[i], sizeof(floor_buttons_t));
    }
  usleep(10*ORDER_POLL_DELAY);
  }
  return NULL;
}



void* main_button_input(void* arg){
  printf("button_input\n");

  while(1){
    for(int i = 0; i <KEEP_ALIVE_NODE_AMOUNT; i++){
      if(g_elevator[i].alive){
        //printf("Polling elevator %s\n", g_elevator[i].elevator.ip);
        if(poll_new_orders(&g_elevator[i].elevator, queue)){
          for(int j = 0; j < queue->size; j++){
            printf("%d \t Order ID: %ld\n", j , queue->orders[j].order_id);
          }
        } else{
          //printf("Elevator %s has no new orders\n", elevator[i].elevator.ip);
        }
      usleep(ORDER_POLL_DELAY);
      }
    }
  }
  return NULL;
}

void* main_elevator_inputs(void* arg){
  printf("elevator_inputs\n");
  while(1){
    elevator_init(g_elevator);
    poll_stopped_elevators(g_elevator);
    poll_obstructed_elevators(g_elevator);
    poll_elevator_floor(g_elevator);
  }
  return NULL;
}

void* main_elevator_output(void* arg){
  printf("elevator_outputs");
  while(1){
    update_elevator_floor_lights(g_elevator);
    update_elevator_stop_light(g_elevator);
    update_elevator_door_light(g_elevator);
    add_elevator_button_lights();
    usleep(5*ORDER_POLL_DELAY);
  }
}

void* print_elevator_status(void* arg){
  printf("print_elevator_status\n");
  while(1){
    printf("Elevator status\n");
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
      if(g_elevator[i].alive){
        printf("\tElevator %d: %s is %s, %s and %s at floor %d with door %s\n",i, g_elevator[i].elevator.ip,
                  g_elevator[i].in_use ? "not_used" : "in_use", 
                  g_elevator[i].stop ? "stopped" : "running", 
                  g_elevator[i].obstruction ? "obstructed" : "not obstructed",
                  g_elevator[i].floor,
                  g_elevator[i].door_open ? "open" : "closed"
                  );
      }
    }
    usleep(SEC_TO_US(1));
  }
  return NULL;
}

void* main_send(void* arg){
  printf("send\n");
    sleep(1);
    while(1){
      keep_alive_node_list_t* node_list = get_node_list();
      for(uint8_t i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
        if(node_list->nodes[i].status == ALIVE){
          //printf("found node at %s\n", node_list->nodes[i].ip);
          //printf("Connection status: %d\n", node_list->nodes[i].connection);
          if(node_list->nodes[i].connection == DISCONNECTED){
            printf("Connecting to %s\n", node_list->nodes[i].ip);
            node_list->nodes[i].connection = CONNECTED;
            send_order_queue_connect(node_list->nodes[i].ip, 9000);
            sleep(1);
          }
          //printf("Sending order to %s\n", node_list->nodes[i].ip);
          pthread_mutex_lock(queue->queue_mutex);
          if(send_order_queue_send_order(node_list->nodes[i].ip ,queue)){
            //printf("Order sent to %s\n", node_list->nodes[i].ip);
            for(int i = 0; i < queue->size; i++){
              if(queue->orders[i].order_status == RECIVED){
                queue->orders[i].order_status = SYNCED;
                printf("Order %ld is now synced\n", queue->orders[i].order_id);
              }
            }
          }
          pthread_mutex_unlock(queue->queue_mutex);
          usleep(ORDER_SYNC_DELAY);
        }
      }
    }
  return NULL;
}

void* main_recv(void* arg){
  printf("recv\n");
  return NULL;
}


int main()
{
  struct running_threads_s{
    bool keep_alive;
    bool button_input;
    bool elevator_control;
    bool send;
    bool recv;
    bool print_elevator_status;
  } running_threads;
  running_threads.keep_alive = false;
  running_threads.button_input = false;
  running_threads.elevator_control = false;
  running_threads.send = false;
  running_threads.recv = false;
  running_threads.print_elevator_status = false;


  int err = main_init();
  if(err != 0){
    printf("Error in main_init\n");
    exit(-1);
  }

  pthread_t keep_alive_thread, recv_thread, send_thread, button_thread, elevator_output_thread, elevator_input_thread , print_elevator_status_thread, elevator_light_thread;

  keep_alive_init(5000, SLAVE);
  while(1){
    keep_alive_node_list_t* node_list = get_node_list();
    if(node_list->self->node_mode == SLAVE){
      printf("Slave\n");

      if(running_threads.print_elevator_status == true){
        pthread_cancel(print_elevator_status_thread);
        pthread_join(print_elevator_status_thread, NULL);
        running_threads.print_elevator_status = false;
      }

      if(running_threads.send == true){
        pthread_cancel(send_thread);
        pthread_join(send_thread, NULL);
        for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
          if(node_list->nodes[i].connection == CONNECTED){
            send_order_queue_close_connection(node_list->nodes[i].ip);
          }
        }
        running_threads.send = false;
      }

      if(running_threads.recv == false){
        send_order_queue_listen(9000);
        running_threads.recv = true;
      }
      if(running_threads.button_input == true){
        pthread_cancel(button_thread);
        pthread_join(button_thread, NULL);
        for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
          if(g_elevator[i].alive){
            printf("========================================================================");
            printf("Destroying elevator %s\n", g_elevator[i].elevator.ip);
            elevator_hardware_destroy(&g_elevator[i].elevator);
          }
        }
        running_threads.button_input = false;
      }

      if(running_threads.elevator_control == true){
        pthread_cancel(elevator_input_thread);
        pthread_cancel(elevator_output_thread);
        pthread_cancel(elevator_light_thread);
        pthread_join(elevator_output_thread, NULL);
        pthread_join(elevator_input_thread, NULL);
        pthread_join(elevator_light_thread, NULL);
        running_threads.elevator_control = false;
      }

    }else if(node_list->self->node_mode == MASTER){
      printf("Master\n");
      if(running_threads.recv == true){
        for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
          if(node_list->nodes[i].connection == CONNECTED){
            send_order_queue_close_connection(node_list->nodes[i].ip);
          }
        }
        running_threads.recv = false;
      }

      if(running_threads.print_elevator_status == false){
        pthread_create(&print_elevator_status_thread, NULL, &print_elevator_status, NULL);
        running_threads.print_elevator_status = true;
      }

      if(running_threads.elevator_control == false){
        pthread_create(&elevator_input_thread, NULL, &main_elevator_inputs, NULL);
        pthread_create(&elevator_output_thread, NULL, &main_elevator_output, NULL);
        pthread_create(&elevator_light_thread, NULL, &controll_elevator_button_lights, NULL);
        running_threads.elevator_control = true;
      }
      if(running_threads.send == false){
        pthread_create(&send_thread, NULL, &main_send, NULL);
        running_threads.send = true;
      }

      if(running_threads.button_input == false){
        pthread_create(&button_thread, NULL, &main_button_input, NULL);
        running_threads.button_input = true;
      }
    }
    sleep(1);
  }

  exit(-1);
}

