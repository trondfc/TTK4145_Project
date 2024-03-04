#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "../inc/elevator_control/elevator_button_inputs.h"
#include "../inc/elevator_hardware/elevator_hardware.h"

#define UDELAY 100 * 1000 // 2000 ms

void *PollOrders(){
    elevator_hardware_info_t elevator_1;

    elevator_hardware_read_config("elv1_ip", "elv1_port", &elevator_1);

    elevator_hardware_init(&elevator_1);


    order_queue_t *queue = create_order_queue(10);
    while(1){
        if (poll_new_orders(&elevator_1, queue)){
            for(int i = 0; i < queue->size; i++){
                printf("%d \t Order ID: %d\n", i , queue->orders[i].order_id);
            }
            printf("\n");
        }
        usleep(UDELAY);
    }

    return NULL;
}

int main(){

    pthread_t pollOrders;
    pthread_create(&pollOrders, NULL, PollOrders, NULL);

    pthread_join(pollOrders, NULL);

    return 0;

}