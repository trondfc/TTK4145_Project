/**
 * @file elevator_autofind.c
 * @brief functions for finding and initializing elevators
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "elevator_autofind.h"

/**
 * @brief mallocs memory for elevator_status_t struct and initializes it
 *        with default values to 0 and initializes mutexes
 * 
 * @return elevator_status_t* 
 */
elevator_status_t* elevator_struct_init(){
  elevator_status_t* elevator = (elevator_status_t*)malloc(MAX_ELEVATORS * sizeof(elevator_status_t));
  for(int i = 0; i < MAX_ELEVATORS; i++){
    strcpy(elevator[i].elevator.ip, "\0");
    elevator[i].alive = false;
    elevator[i].floor = 0;
    elevator[i].elevator_state = STOP;
    elevator[i].obstruction = false;
    elevator[i].emergency_stop = false;
    elevator[i].number_of_stop_readings = 0;
    elevator[i].door_open = false;
    pthread_mutex_init(&elevator[i].mutex, NULL);
  }
  return elevator;
}

/**
 * @brief compares two strings and returns 1 if they are equal and not empty
 * 
 * @param a ip string
 * @param b ip string
 * @return int 
 */
int compare_ips(char* a, char* b){
  if(strcmp(a, b) == 0 && strcmp(a, "\0") != 0){
    return 1;
  } 
  return 0;
}

/**
 * @brief initializes an elevator with a given ip
 * 
 * @param elevator elevator_status_t struct to be initialized
 * @param ip 
 */
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
    keep_alive_node_list_t* node_list = get_node_list();
    for(int i = 0; i < MAX_ELEVATORS; i++){
      if(!elevator[i].alive){
        printf("Elevator %s is not alive\n", elevator[i].elevator.ip);
        pthread_mutex_lock(&elevator[i].mutex);
        strcpy(elevator[i].elevator.ip, ip);
        //char port[10];
        //sprintf(port, "%d", ELEVATOR_PORT);
        //strcpy(elevator[i].elevator.port, port);
        sprintf(elevator[i].elevator.port, "%d", ELEVATOR_PORT);
        if(elevator_hardware_init(&elevator[i].elevator)){
          elevator[i].alive = true;
          printf("Elevator %s is alive\n", elevator[i].elevator.ip);
        }
        if(node_list->single_master){
          strcpy(elevator[i].elevator.ip, node_list->self->ip);
        }
        pthread_mutex_unlock(&elevator[i].mutex);
        break;
      }
    }
  }
}

/**
 * @brief initializes all elevators in the elevator_status_t struct
 * 
 * @param elevator elevator_status_t struct to be initialized
 */
void elevator_init(elevator_status_t* elevator){
keep_alive_node_list_t* node_list = get_node_list();
  elevator_init_ip(elevator, node_list->host_ip);
  for(int i = 0; i < MAX_ELEVATORS; i++){
    if(strcmp(node_list->nodes[i].ip, "\0") == 0){
      continue;
    }
    elevator_init_ip(elevator, node_list->nodes[i].ip);
  }
}