#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../inc/order_queue/orderQueue.h"

order_queue_t * rxqueue;

void messageReceived(char * data, int datalength){

    printf("Recived %d bytes\n", datalength);
  
    memcpy(&rxqueue->size, data, sizeof(rxqueue->size));
    memcpy(&rxqueue->capacity, data+sizeof(rxqueue->size), sizeof(rxqueue->capacity));
    //memcpy(rxqueue->orders, data+sizeof(rxqueue->size)+sizeof(rxqueue->capacity), sizeof(rxqueue->orders)); 
    memcpy(rxqueue->orders, data+sizeof(order_queue_t), sizeof(order_event_t)*rxqueue->size);

    printf("Queue size: %d\n", rxqueue->size);
    printf("Queue capacity: %d\n", rxqueue->capacity);
    for(int i = 0; i < rxqueue->size; i++){
        printf("Order %d ID: %d\n", i, rxqueue->orders[i].order_id);
    }
}

int main(){

  rxqueue = create_order_queue(10);
  order_queue_t* txqueue = create_order_queue(10);

  /* Generate different orders */

    order_event_t order;
    order.elevator_id = 1;
    order.floor = 2;
    order.order_type = GO_TO;
    order.order_id = GenerateOrderID(&order);
    printf("Order 1 ID: %d\n", order.order_id);

    order_event_t order2;
    order2.elevator_id = 1;
    order2.floor = 2;
    order2.order_type = DOWN_FROM;
    order2.order_id = GenerateOrderID(&order2);
    printf("Order 2 ID: %d\n", order2.order_id);

    order_event_t order3;
    order3.elevator_id = 2;
    order3.floor = 3;
    order3.order_type = DOWN_FROM;
    order3.order_id = GenerateOrderID(&order3);
    printf("Order 3 ID: %d\n", order3.order_id);

    order_event_t order4;
    order4.elevator_id = 3;
    order4.floor = 1;
    order4.order_type = UP_FROM;
    order4.order_id = GenerateOrderID(&order4);
    printf("Order 4 ID: %d\n", order4.order_id);

    order_event_t order5;
    order5.elevator_id = 3;
    order5.floor = 1;
    order5.order_type = UP_FROM;
    order5.order_id = GenerateOrderID(&order5);
    printf("Order 5 ID: %d\n", order5.order_id);
    /*  */

        /* Testing enqueueing */
    printf("Testing enqueueing\n");
    enqueue_order(txqueue, &order);
    enqueue_order(txqueue, &order2);
    enqueue_order(txqueue, &order3);
    enqueue_order(txqueue, &order4);
    printf("Enqueueing identical order\n");
    enqueue_order(txqueue, &order5);

    char * buffer = malloc(sizeof(order_queue_t) + sizeof(order_event_t) * txqueue->capacity);
    memcpy(buffer, txqueue, sizeof(order_queue_t));
    memcpy(buffer + sizeof(order_queue_t), txqueue->orders, sizeof(order_event_t) * txqueue->capacity);

    messageReceived(buffer, sizeof(order_queue_t) + sizeof(order_event_t) * txqueue->capacity);

  return 0;
}