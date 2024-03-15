#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "../elevator_control/elevator_control.h"
#include "../elevator_hardware/elevator_hardware.h"
#include "../order_queue/orderQueue.h"
#include "../config.h"

typedef struct {
    elevator_status_t* elevator;
    order_queue_t* queue;
}elevator_arg_t;

/* Prototypes */
bool order_in_elevator_path(order_event_t* order, elevator_status_t* elevator);
bool elevator_has_reserved_orders(order_queue_t* queue, elevator_status_t* elevator);
bool reserved_order_at_position(order_queue_t* queue, elevator_status_t* elevator);
elevator_state_t direction_to_order(order_event_t* order, elevator_status_t* elevator);
void reserve_elevator(order_queue_t* queue, order_event_t* order, elevator_status_t* elevator);

order_event_t* return_reserved_order_at_floor(order_queue_t* queue,elevator_status_t* elevator, int floor);
long return_closest_order(order_queue_t* queue, elevator_status_t* elevator);
order_event_t* return_oldes_order(order_queue_t* queue, elevator_status_t* elevator);
void order_completion_timedout(order_queue_t* queue);
void unreserve_elevators_orders(order_queue_t* queue, elevator_status_t* elevator);


void set_completed_order(order_queue_t* queue, elevator_status_t* elevator);
void remove_completed_order(order_queue_t* queue, elevator_status_t* elevator);


void* thr_handle_orders(void* args);