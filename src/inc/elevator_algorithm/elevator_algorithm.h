#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

//#include "../types.h"
#include "../order_queue/orderQueue.h"
#include "../elevator_hardware/elevator_hardware.h"
#include "../elevator_control/elevator_control.h"

#define LOG_LEVEL LOG_LEVEL_INFO

#define N_ELEVATORS 1
#define N_FLOORS 4
#define N_ORDERS 100

//#define DEFAULT_FLOOR 0




/// Convert seconds to microseconds
#define SEC_TO_US(sec) ((sec)*1000000)
/// Convert milliseconds to microseconds
#define MS_TO_US(ms)    ((ms)*1000)
/// Convert nanoseconds to microseconds
#define NS_TO_US(ns)    ((ns)/1000)

#define US_TO_SEC(us)   ((us)/1000000)

#define ELEVATOR_DOOR_OPEN_TIME SEC_TO_US(0)

typedef struct{
  int elevator_no;
  elevator_status_t* elevator_status;
  order_queue_t* order_queue;
}elevator_arg_t;

//Public functions
int elevator_algorithm_init(elevator_status_t* elevator_status, order_queue_t* order_queue);
int elevator_algorithm_kill();


//Private functions
uint64_t get_timestamp_us();

/*
typedef enum order_types_t {
    UP_FROM = 0,
    DOWN_FROM = 1,
    GO_TO = 2
}order_types_t;

typedef enum order_status_t{
    RECIVED = 0,
    SYNCED = 1,
    ACTIVE = 2,
    COMPLETED = 3
}order_status_t;

typedef struct order_event_t {
    long order_id;
    char elevator_id[16];
    uint8_t floor;
    order_types_t order_type;
    order_status_t order_status;
    time_t timestamp;
    uint8_t controller_id;
}order_event_t;*/