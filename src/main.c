
#include<unistd.h>
#include<stdio.h> 

#include "inc/keep_alive/keep_alive.h"
#include "inc/process_pair/process_pair.h"


host_config_t host_config;

int main_init(){
  printf("main_init\n");
  sysQueInit(5);
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
    if (last_host_state != host_config->host_state){
      printf("host_state changed to %s\n", host_config->host_state == MASTER ? "MASTER" : "SLAVE");
    }
    sleep(1);
    printf("keep_alive_control\n");
  }
return NULL;
}


int main()
{
  int err = main_init();
  if(err != 0){
    printf("Error in main_init\n");
    exit(-1);
  }

  pthread_t keep_alive_thread;

  pthread_create(&keep_alive_thread, NULL, keep_alive_control, (void*)&host_config);

  
  pthread_join(keep_alive_thread, NULL);
  exit(-1);
}

