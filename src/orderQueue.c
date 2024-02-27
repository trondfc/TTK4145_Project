#include "orderQueue.h"

bool compare_orders(order_event_t *order, order_event_t *order2){
    if(order->order_id == order2->order_id){
        return 1;
    }
    else{
        return 0;
    }
}

void enqueue_order(order_queue_t *queue, order_event_t *order){
    if(queue->size == queue->capacity){
        printf("Queue is full\n");
        return;
    }
    for(int i = 0; i < queue->size; i++){
        if(compare_orders(&queue->orders[i], order)){
            printf("Order already in queue\n");
            return;
        }
        else{
            queue->orders[queue->size] = *order;
            queue->size++;
        }
    }
}

void dequeue_order(order_queue_t *queue, order_event_t *order){
    if(queue->size == 0){
        printf("Queue is empty\n");
        return;
    }
    for(int i = 0; i < queue->size; i++){
        if(compare_orders(&queue->orders[i], order)){
            for(int j = i; j < queue->size - 1; j++){
                queue->orders[j] = queue->orders[j + 1];
            }
            queue->size--;
            return;
        }
    }
    printf("Order not in queue\n");
}