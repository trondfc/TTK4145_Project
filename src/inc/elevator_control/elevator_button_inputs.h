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