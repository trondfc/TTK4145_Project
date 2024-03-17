/**
 * @file elevator_button_inputs.h
 * @brief Functions to poll an elevator for new orders.
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ELEVATOR_BUTTON_INPUTS_H
#define ELEVATOR_BUTTON_INPUTS_H

/* Includes */
#include <time.h>
#include <string.h>

#include "../elevator_hardware/elevator_hardware.h"
#include "../order_queue/orderQueue.h"


/* Prototypes */
int poll_new_orders(elevator_hardware_info_t *elevator, order_queue_t *queue);

#endif // ELEVATOR_BUTTON_INPUTS_H