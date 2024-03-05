#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "../inc/elevator_control/elevator_button_inputs.h"
#include "../inc/elevator_hardware/elevator_hardware.h"
#include "../inc/order_queue/orderQueue.h"
#include "../inc/order_queue/send_order_queue.h"

#define UDELAY 100 * 1000 // 2000 ms

order_queue_t* queue;

/* Callback function for tcp connupdate*/
void connectionStatus(const char * ip, int status){

  printf("A connection got updated %s: %d\n",ip,status);
}

void *PollOrders(){
    elevator_hardware_info_t elevator_1;

    elevator_hardware_read_config("elv1_ip", "elv1_port", &elevator_1);

    elevator_hardware_init(&elevator_1);


    
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

void* SyncOrders(){
    send_order_queue_init(NULL,connectionStatus);
    send_order_queue_connect("127.0.0.1",9000);
    while(1){
        pthread_mutex_lock(queue->queue_mutex);
        send_order_queue_send_order("127.0.0.1",queue);
        pthread_mutex_unlock(queue->queue_mutex);
        usleep(500*1000);
    }
    return NULL;
}

int main(){
    queue = create_order_queue(10);

    

    pthread_t pollOrders;
    pthread_t syncOrders;

    pthread_create(&pollOrders, NULL, PollOrders, NULL);
    pthread_create(&syncOrders, NULL, SyncOrders, NULL);

    pthread_join(pollOrders, NULL);
    pthread_join(syncOrders, NULL);

    return 0;

}