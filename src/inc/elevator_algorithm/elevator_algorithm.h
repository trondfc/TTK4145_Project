#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "../order_queue/orderQueue.h"
#include "../elevator_hardware/elevator_hardware.h"

#define LOG_LEVEL LOG_LEVEL_INFO

#define N_ELEVATORS 4
#define N_FLOORS 4
#define N_ORDERS 100

#define DEFAULT_FLOOR 0

#define ELEVATOR_DOOR_OPEN_TIME 3000 //ms


/// Convert seconds to microseconds
#define SEC_TO_US(sec) ((sec)*1000000)
/// Convert milliseconds to microseconds
#define MS_TO_US(ms)    ((ms)*1000)
/// Convert nanoseconds to microseconds
#define NS_TO_US(ns)    ((ns)/1000)

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


typedef struct{
  int elevator_no;
  elevator_status_t* elevator_status;
  order_queue_t* order_queue;
}elevator_arg_t;

//Public functions
int elevator_status_init(elevator_status_t* elevator_status, order_queue_t* order_queue);
int elevator_status_kill();


//Private functions

