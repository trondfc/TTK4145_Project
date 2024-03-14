#pragma once

#include "elevator_hardware/elevator_hardware.h"
/* 
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
  pthread_mutex_t mutex;
}elevator_status_t;

 */
typedef enum{
  ELEVATOR_IDLE,
  ELEVATOR_DIR_UP_AND_MOVING,
  ELEVATOR_DIR_UP_AND_STOPPED,
  ELEVATOR_DIR_DOWN_AND_MOVING, 
  ELEVATOR_DIR_DOWN_AND_STOPPED,
  ELEVATOR_EMERGENCY_STOP
}elevator_state_t;



typedef struct{
  pthread_mutex_t* mutex;                 //Controlled Externally
  elevator_hardware_info_t elevator;      //Controlled Externally
  bool alive;                             //Controlled Externally
  int floor;                              //Controlled Externally
  bool obstruction;                       //Controlled Externally   
  bool emergency_stop;                    //Controlled Externally  

  bool door_open;                         //Controlled Locally
  elevator_state_t elevator_state;        //Controlled Locally

}elevator_status_t;