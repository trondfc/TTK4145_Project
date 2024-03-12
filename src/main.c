/* Include libraries */
#include "inc/elevator_control/elevator_button_inputs.h"
#include "inc/elevator_hardware/elevator_hardware.h"
#include "inc/order_queue/orderQueue.h"
#include "inc/process_pair/process_pair.h"
#include "inc/order_queue/send_order_queue.h"
#include "inc/keep_alive/keep_alive.h"
#include "inc/logger.h"
#include "inc/elevator_algorithm/elevator_algorithm.h"
#define LOG_LEVEL LOG_LEVEL_DEBUG // Set log level to debug

#include<unistd.h>
#include<stdio.h> 
#define QUEUE_SIZE 20
#define ORDER_POLL_DELAY 100 * 1000 // 100 ms
#define ORDER_SYNC_DELAY 500 * 1000 // 500 ms

//elevator_hardware_info_t elevator[KEEP_ALIVE_NODE_AMOUNT];
typedef enum elevator_direction{
  UP,
  DOWN,
  STOP
}elevator_direction_t;

typedef struct elevator_status{
  elevator_hardware_info_t elevator;
  bool alive;
  int floor;
  elevator_direction_t direction;
  bool obstruction;
  bool stop;
  bool door_open;
  bool in_use;
}elevator_status_t;

/* Define global variables */
order_queue_t *queue;
elevator_status_t elevator[KEEP_ALIVE_NODE_AMOUNT];


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

void elevator_init(){
  keep_alive_node_list_t* node_list = get_node_list();
  for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
    bool found = false;
    for(int j = 0; j < KEEP_ALIVE_NODE_AMOUNT; j++){
      if(strcmp(node_list->nodes[i].ip, elevator[j].elevator.ip) == 0){
        found = true;
        if(!elevator[j].alive){
          if(elevator_hardware_init(&elevator[j])){
            elevator[j].alive = true;
            printf("Elevator %s is alive\n", elevator[j].elevator.ip);
            break;
          }
        }
      }
    }
    if(!found){
      for(int j = 0; j < KEEP_ALIVE_NODE_AMOUNT; j++){
        if(!elevator[j].alive){
          strcpy(elevator[j].elevator.ip, node_list->nodes[i].ip);
          strcpy(elevator[j].elevator.port, "15657");
          if(elevator_hardware_init(&elevator[j])){
            elevator[j].alive = true;
            printf("Elevator %s is alive\n", elevator[j].elevator.ip);
            break;
          }
        }
      }
    }
  }
}

int main_init(){
  printf("main_init\n");
  sysQueInit(5);
  send_order_queue_init(messageReceived, connectionStatus);
  printf("sysQueInit\n");

  queue = create_order_queue(QUEUE_SIZE);

  return 0;
}

void poll_stopped_elevators(){
  for(uint8_t i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
    if(elevator[i].alive){
      int temp = elevator_hardware_get_stop_signal(&elevator[i]);
      if(temp == 1){
        printf("Elevator %s has stopped\n", elevator[i].elevator.ip);
      }
      else if (temp == 0){
        printf("Elevator %s has started\n", elevator[i].elevator.ip);
      }
      else{
        printf("Elevator %s is not responding\n", elevator[i].elevator.ip);
        elevator[i].alive = false;
      }
    }
  }
  return NULL;
}

void poll_obstructed_elevators(){
  for(uint8_t i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
    if(elevator[i].alive){
      if(elevator_hardware_get_obstruction_signal(&elevator[i])){
        printf("Elevator %s is obstructed\n", elevator[i].elevator.ip);

      }
      else{
        printf("Elevator %s is not obstructed\n", elevator[i].elevator.ip);
      }
    }
  }
  return NULL;
}

void* main_button_input(void* arg){
  printf("button_input\n");
    
    while(1){
        if (poll_new_orders(&elevator[0], queue)){
            for(int i = 0; i < queue->size; i++){
                printf("%d \t Order ID: %ld\n", i , queue->orders[i].order_id);
            }
            printf("\n");
        }
        if (poll_new_orders(&elevator[1], queue)){
            for(int i = 0; i < queue->size; i++){
                printf("%d \t Order ID: %ld\n", i , queue->orders[i].order_id);
            }
            printf("\n");
        }
        /*
        if (poll_new_orders(&elevator[2], queue)){
            for(int i = 0; i < queue->size; i++){
                printf("%d \t Order ID: %ld\n", i , queue->orders[i].order_id);
            }
            printf("\n");
        }*/
        
        usleep(ORDER_POLL_DELAY);
    }

  return NULL;
}

void* main_elevator_control(void* arg){
  printf("elevator_control\n");
  return NULL;
}

void* main_send(void* arg){
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

  //pthread_create(&keep_alive_thread, NULL, keep_alive_control, (void*)&host_config);
  keep_alive_init(5000, SLAVE);
  while(1){
    keep_alive_node_list_t* node_list = get_node_list();
    if(node_list->self->node_mode == SLAVE){
      printf("Slave\n");


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
      //pthread_cancel(button_thread);
      //pthread_cancel(elevator_thread);
      //pthread_create(&recv_thread, NULL, &main_recv, (void*)&host_config);

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

      //pthread_create(&button_thread, NULL, &main_button_input, (void*)&host_config);
      //pthread_create(&elevator_thread, NULL, &main_elevator_control, (void*)&host_config);
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

