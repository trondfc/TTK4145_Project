#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "../elevator_control/elevator_control.h"
#include "../elevator_hardware/elevator_hardware.h"
#include "../order_queue/orderQueue.h"
#include "../config.h"

/* Prototypes */
bool order_in_elevator_path(order_event_t* order, elevator_status_t* elevator);
bool elevator_has_reserved_orders(order_queue_t* queue, elevator_status_t* elevator);
bool reserved_order_at_position(order_queue_t* queue, elevator_status_t* elevator);
long find_closest_order(order_queue_t* queue, elevator_status_t* elevator);
elevator_state_t direction_to_order(order_event_t* order, elevator_status_t* elevator);

