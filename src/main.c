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
#include "inc/elevator_control/elevator_control.h"
#define LOG_LEVEL LOG_LEVEL_DEBUG // Set log level to debug


#define QUEUE_SIZE MAX_QUEUE_SIZE

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
  button_lights = button_light_struct_init();
  queue = create_order_queue(QUEUE_SIZE);
  
  printf("sysQueInit\n");


  return 0;
}

void* controll_elevator_button_lights(void* arg){
  elevator_status_t* elevator = g_elevator;
  while(1){
    for(int i = 0; i < 10; i++){
      set_changed_button_lights(button_lights, elevator);
      usleep(10*ORDER_POLL_DELAY);
    }
    sett_all_button_lights(button_lights, elevator);
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
    add_elevator_button_lights(button_lights, queue, g_elevator);
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

