#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "../order_queue/orderQueue.h"
#include "../elevator_hardware/elevator_hardware.h"

#define N_ELEVATORS 1
#define N_FLOORS 1
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
  elevator_hardware_info_t elevator;
  bool alive;
  int floor;
  elevator_state_t elevator_state;
  bool obstruction;
  bool emergency_stop;
  bool door_open;
}elevator_status_t;

//Public functions
int elevator_status_init(order_queue_t* order_queue);
int elevator_status_kill();
elevator_status_t* get_elevator_status();



//Private functions



/*
typedef enum
{
  IDLE = 0,
  ACTIVE = 1
}elevator_state_t;


typedef struct {
    elevator_status_t* elevator;
    order_queue_t* order_queue;
    pthread_mutex_t* mutex;
}elevator_system_t;

*/


/* 
typedef struct elevator_status_t{
  uint8_t prev_floor; // last floor visited, current floor if at floor
  bool at_floor; // true if at floor, false if between floors
  elevator_direction_t current_direction;
  bool responding; // true if elevator is responding to orders, false if network is down
  int used_by; // controller id, -1 if not used
}elevator_status_t; */

/* 
typedef struct all_elevator_status_t{
  elevator_status_t elevator[N_ELEVATORS];
}all_elevator_status_t;

 */

/* typedef struct order_que_t{
  order_event_t order[N_ORDERS];
}order_que_t;


typedef struct order_event_t{
  int order_id; // Randomly generated number (big enough to make duplicates unlikely)
  uint8_t elevator_id;
  order_types_t order;
  uint64_t timestamp; // modified timestamp?
  order_status_t status;
  uint8_t controller_id; // Controller serving if active
}order_event_t;

 */

/* typedef enum order_types_t{
  UP_FROM = 0,
  DOWN_FROM = 1,
  GO_TO = 2
}order_types_t;

typedef enum order_status_t{
  RECIVED = 0,
  ACTIVE = 1,
  COMPLETED = 2
}order_status_t; */



/* 
typedef struct{
  int prev_floor; // last floor visited, current floor if at floor
  int current_floor;
  elevator_state_t state;
  elevator_hardware_motor_direction_t current_direction;
  elevator_hardware_info_t* hardware_info;
}elevator_status_t;

 */