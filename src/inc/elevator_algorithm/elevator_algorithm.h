#pragma once

#include <stdint.h>
#include <stdbool.h>

#define N_ELEVATORS 4
#define N_FLOORS 4
#define N_ORDERS 100

typedef enum order_types_t{
  UP_FROM = 0,
  DOWN_FROM = 1,
  GO_TO = 2
}order_types_t;

typedef enum order_status_t{
  RECIVED = 0,
  ACTIVE = 1,
  COMPLETED = 2
}order_status_t;

typedef enum elevator_direction_t
{
    DIRN_DOWN = -1,
    DIRN_STOP = 0,
    DIRN_UP = 1
}elevator_direction_t;


typedef struct order_event_t{
  int order_id; // Randomly generated number (big enough to make duplicates unlikely)
  uint8_t elevator_id;
  order_types_t order;
  uint64_t timestamp; // modified timestamp?
  order_status_t status;
  uint8_t controller_id; // Controller serving if active
}order_event_t;

typedef struct order_que_t{
  order_event_t order[N_ORDERS];
}order_que_t;

typedef struct elevator_status_t{
  uint8_t prev_floor; // last floor visited, current floor if at floor
  bool at_floor; // true if at floor, false if between floors
  elevator_direction_t current_direction;
  bool responding; // true if elevator is responding to orders, false if network is down
  int used_by; // controller id, -1 if not used
}elevator_status_t;

typedef struct all_elevator_status_t{
  elevator_status_t elevator[N_ELEVATORS];
}all_elevator_status_t;

typedef struct {
    all_elevator_status_t* all_elevator_status;
    order_que_t* order_que;
    pthread_mutex_t lock;
}elevator_system_t;