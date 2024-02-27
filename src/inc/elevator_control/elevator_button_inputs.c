#include "elevator_button_inputs.h"


void poll_new_orders(elevator_hardware_info_t *elevator, order_queue_t *queue){
    for(uint8_t i = 0; i < N_FLOORS; i++){
        for(uint8_t j = 0; j < N_BUTTONS; j++){
            if(elevator_hardware_get_button_signal(j, i, elevator)){
                order_event_t * order = (order_event_t *)malloc(sizeof(order_event_t));
                strcpy(order->elevator_id, elevator->ip);
                order->floor = i;
                order->order_type = j;
                order->order_status = RECIVED;
                time_t systime;
                $time(systime);
                order->timestamp = systime;
                order->order_id = GenerateOrderID(order);
                enqueue_order(queue, order);
                free(order);
            }
        }
    }
}