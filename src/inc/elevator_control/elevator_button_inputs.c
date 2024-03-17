/**
 * @file elevator_button_inputs.c
 * @brief Functions to poll an elevator for new orders.
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "elevator_button_inputs.h"

/**
 * @brief Polls an elevator for orders, and adds it to a queue.
 * 
 * @param elevator 
 * @param queue 
 * @return int 
 */
int poll_new_orders(elevator_hardware_info_t *elevator, order_queue_t *queue){
    for(uint8_t i = 0; i < N_FLOORS; i++){
        for(uint8_t j = 0; j < N_BUTTONS; j++){
            if(elevator_hardware_get_button_signal(j, i, elevator)){
                order_event_t * order = (order_event_t *)malloc(sizeof(order_event_t));
                order->floor = i;
                order->order_type = j;
                order->order_status = RECIVED;
                time_t systime;
                time(&systime);
                order->timestamp = systime;

                if(order->order_type == GO_TO){
                    strcpy(order->elevator_id, elevator->ip);
                }
                else{
                    strcpy(order->elevator_id, "0.0.0.0");
                }

                order->order_id = GenerateOrderID(order);
                pthread_mutex_lock(queue->queue_mutex);
                enqueue_order(queue, order);
                pthread_mutex_unlock(queue->queue_mutex);
                free(order);

                return 1;
            }
        }
    }
    return 0;
}