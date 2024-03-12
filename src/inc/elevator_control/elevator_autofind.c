#include "elevator_autofind.h"

elevator_status_t* elevator_struct_init(){
  elevator_status_t* elevator = (elevator_status_t*)malloc(MAX_ELEVATORS * sizeof(elevator_status_t));
  for(int i = 0; i < MAX_ELEVATORS; i++){
    strcpy(elevator[i].elevator.ip, "\0");
    elevator[i].alive = false;
    elevator[i].floor = 0;
    elevator[i].direction = STOP;
    elevator[i].obstruction = false;
    elevator[i].stop = false;
    elevator[i].door_open = false;
    elevator[i].in_use = false;
    pthread_mutex_init(&elevator[i].mutex, NULL);
  }
  return elevator;
}

int compare_ips(char* a, char* b){
  if(strcmp(a, b) == 0 && strcmp(a, "\0") != 0){
    return 1;
  } 
  return 0;
}

void elevator_init_ip(elevator_status_t* elevator, char* ip){
  //printf("checking elevator %s\n", ip);
  bool node_alive = false;
  for(int i = 0; i < MAX_ELEVATORS; i++){
    if(compare_ips(ip, elevator[i].elevator.ip)){
      if(elevator[i].alive){
        //printf("Elevator %s is already alive\n", elevator[i].elevator.ip);
        node_alive = true;
        break;
      }
    }
  }
  if(!node_alive){
    for(int i = 0; i < MAX_ELEVATORS; i++){
      if(!elevator[i].alive){
        printf("Elevator %s is not alive\n", elevator[i].elevator.ip);
        pthread_mutex_lock(&elevator[i].mutex);
        strcpy(elevator[i].elevator.ip, ip);
        strcpy(elevator[i].elevator.port, "15657");
        if(elevator_hardware_init(&elevator[i].elevator)){
          elevator[i].alive = true;
          printf("Elevator %s is alive\n", elevator[i].elevator.ip);
        }
        pthread_mutex_unlock(&elevator[i].mutex);
        break;
      }
    }
  }
}

void elevator_init(elevator_status_t* elevator){
keep_alive_node_list_t* node_list = get_node_list();
  elevator_init_ip(elevator, node_list->self->ip);
  for(int i = 0; i < MAX_ELEVATORS; i++){
    if(strcmp(node_list->nodes[i].ip, "\0") == 0){
      continue;
    }
    elevator_init_ip(elevator, node_list->nodes[i].ip);
  }
}