/* Include libraries */
#include "inc/elevator_control/elevator_button_inputs.h"
#include "inc/elevator_hardware/elevator_hardware.h"
#include "inc/order_queue/orderQueue.h"
#include "inc/process_pair/process_pair.h"
#include "inc/logger.h"
#define LOG_LEVEL LOG_LEVEL_DEBUG // Set log level to debug

#include<unistd.h>
#include<stdio.h> 
#define QUEUE_SIZE 20
#define ORDER_POLL_DELAY 100 * 1000 // 100 ms

#include "inc/keep_alive/keep_alive.h"
#include "inc/process_pair/process_pair.h"
/* Define global variables */
order_queue_t *queue;

host_config_t host_config;



int main_init(){
  printf("main_init\n");
  sysQueInit(5);
  printf("sysQueInit\n");
  return 0;
}

void* keep_alive_control(void* arg){
  host_config_t* host_config = (host_config_t*)arg;
  printf("keep_alive_init\n");
  
  keep_alive_config_t* keep_alive_config = keep_alive_start();
  pthread_mutex_lock(&host_config->mutex);
  host_config->node_list = get_alive_node_list(keep_alive_config);
  keep_alive_type_t last_host_state = host_config->host_state;
  pthread_mutex_unlock(&host_config->mutex);

  
  while(1){
    pthread_mutex_lock(&host_config->mutex);
    host_config->host_state = get_host_state(keep_alive_config);
    pthread_mutex_unlock(&host_config->mutex);
    if (host_config->host_state == RESET){
      printd(LOG_LEVEL_DEBUG, "Process Reset\n");
      //exit(-1);
    }
    if (last_host_state != host_config->host_state){
      printd(LOG_LEVEL_DEBUG, "host_state changed \n");
    }
    sleep(1);
    printf("keep_alive_control\n");
  }
return NULL;
}

void* main_button_input(void* arg){
  printf("button_input\n");
  return NULL;
}

void* main_elevator_control(void* arg){
  printf("elevator_control\n");
  return NULL;
}

void* main_send(void* arg){
  printf("send\n");
  return NULL;
}

void* main_recv(void* arg){
  printf("recv\n");
  return NULL;
}


int main()
{
  int err = main_init();
  if(err != 0){
    printf("Error in main_init\n");
    exit(-1);
  }

  pthread_t keep_alive_thread, recv_thread, send_thread, button_thread, elevator_thread;

  pthread_create(&keep_alive_thread, NULL, keep_alive_control, (void*)&host_config);

  while(1){
    if(host_config.host_state == SLAVE){
      printf("Slave\n");
      pthread_cancel(button_thread);
      pthread_cancel(elevator_thread);
      pthread_cancel(send_thread);
      pthread_create(&recv_thread, NULL, &main_recv, (void*)&host_config);

    }else if(host_config.host_state == MASTER){
      printf("Master\n");
      pthread_cancel(recv_thread);

      pthread_create(&button_thread, NULL, &main_button_input, (void*)&host_config);
      pthread_create(&elevator_thread, NULL, &main_elevator_control, (void*)&host_config);
      pthread_create(&send_thread, NULL, &main_send, (void*)&host_config);
    }
    sleep(1);
  }


  exit(-1);
}

