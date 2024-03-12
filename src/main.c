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
#define ORDER_POLL_DELAY 10 * 1000 // 100 ms
#define ORDER_SYNC_DELAY 500 * 1000 // 500 ms

/* Define global variables */
order_queue_t *queue;
elevator_status_t *g_elevator;


void messageReceived(const char * ip, char * data, int datalength){

  printf("Received message from %s\n",ip);

  send_order_queue_deserialize(data, queue);

  printf("Queue size: %d\n", queue->size);
  printf("Queue capacity: %d\n", queue->capacity);
  for(int i = 0; i < queue->size; i++){
      printf("Order %d ID: %ld\n", i, queue->orders[i].order_id);
      printf("Order %d elevator_id: %s\n", i, queue->orders[i].elevator_id);
      printf("Order %d floor: %d\n", i, queue->orders[i].floor);
      printf("Order %d order_type: %d\n", i, queue->orders[i].order_type);
      printf("\n\n");
  }
    
}


/* Callback function for tcp connupdate*/
void connectionStatus(const char * ip, int status){
  printf("A connection got updated %s: %d\n",ip,status);
  keep_alive_node_list_t* node_list = get_node_list();
  for(uint8_t i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
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
  printf("sysQueInit\n");

  queue = create_order_queue(QUEUE_SIZE);

  return 0;
}

void poll_stopped_elevators(elevator_status_t* elevator){
  for(uint8_t i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
    if(elevator[i].alive){
      int temp = elevator_hardware_get_stop_signal(&elevator[i].elevator);
      //printf("elevator %d %s is alive: %d\n",i, elevator[i].elevator.ip, elevator[i].alive);
      if(temp == 1){
        //printf("Elevator %s has stopped\n", elevator[i].elevator.ip);
      }
      else if (temp == 0){
        //printf("Elevator %s has started\n", elevator[i].elevator.ip);
      }
      else{ // Retunrs -1 if the elevator is not responding
        printf("Elevator %s is not responding\n", elevator[i].elevator.ip);
        pthread_mutex_lock(&elevator[i].mutex);
        elevator[i].alive = false;
        pthread_mutex_unlock(&elevator[i].mutex);
      }
    }
    usleep(ORDER_POLL_DELAY);
  }
}

void poll_obstructed_elevators(elevator_status_t* elevator){
  for(uint8_t i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
    if(elevator[i].alive){
      /*
      if(elevator_hardware_get_obstruction_signal(&elevator[i].elevator)){
        //printf("Elevator %s is obstructed\n", elevator[i].elevator.ip);

      }
      else{
        //printf("Elevator %s is not obstructed\n", elevator[i].elevator.ip);
      }*/
    }
  }
}

void* main_button_input(void* arg){
  printf("button_input\n");

  while(1){
    for(int i = 0; i <KEEP_ALIVE_NODE_AMOUNT; i++){
      if(g_elevator[i].alive){
        //printf("Polling elevator %s\n", elevator[i].elevator.ip);
        if(poll_new_orders(&g_elevator[i].elevator, queue)){
          for(int j = 0; j < queue->size; j++){
            printf("%d \t Order ID: %ld\n", j , queue->orders[j].order_id);
          }
        } else{
          //printf("Elevator %s has no new orders\n", elevator[i].elevator.ip);
        }
      }
      usleep(ORDER_POLL_DELAY);
    }
  }
  return NULL;
}

void* main_elevator_control(void* arg){
  printf("elevator_control\n");
  while(1){
    elevator_init(g_elevator);
    poll_stopped_elevators(g_elevator);
    //poll_obstructed_elevators();
    usleep(10*ORDER_POLL_DELAY);
  }
  return NULL;
}

void* main_send(void* arg){
  return NULL;
  printf("send\n");
    printf("send_order_queue_init\n");
    sleep(1);
    while(1){
      keep_alive_node_list_t* node_list = get_node_list();
      for(uint8_t i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
        if(node_list->nodes[i].status == ALIVE){
          printf("found node at %s\n", node_list->nodes[i].ip);
          printf("Connection status: %d\n", node_list->nodes[i].connection);
          if(node_list->nodes[i].connection == DISCONNECTED){
            printf("Connecting to %s\n", node_list->nodes[i].ip);
            node_list->nodes[i].connection = CONNECTED;
            send_order_queue_connect(node_list->nodes[i].ip, 9000);
            sleep(1);
          }
          printf("Sending order to %s\n", node_list->nodes[i].ip);
          pthread_mutex_lock(queue->queue_mutex);
          send_order_queue_send_order(node_list->nodes[i].ip ,queue);
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
  } running_threads;

  int err = main_init();
  if(err != 0){
    printf("Error in main_init\n");
    exit(-1);
  }

  pthread_t keep_alive_thread, recv_thread, send_thread, button_thread, elevator_thread;

  keep_alive_init(5000, SLAVE);
  while(1){
    keep_alive_node_list_t* node_list = get_node_list();
    if(node_list->self->node_mode == SLAVE){
      //printf("Slave\n");


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
        running_threads.button_input = false;
      }

      if(running_threads.elevator_control == true){
        pthread_cancel(elevator_thread);
        pthread_join(elevator_thread, NULL);
        running_threads.elevator_control = false;
      }

    }else if(node_list->self->node_mode == MASTER){
      //printf("Master\n");
      if(running_threads.recv == true){
        for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
          if(node_list->nodes[i].connection == CONNECTED){
            send_order_queue_close_connection(node_list->nodes[i].ip);
          }
        }
        running_threads.recv = false;
      }

      if(running_threads.elevator_control == false){
        pthread_create(&elevator_thread, NULL, &main_elevator_control, NULL);
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

