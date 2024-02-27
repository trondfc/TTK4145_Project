#include "orderQueue.h"

order_queue_t * create_order_queue(int capacity){
    order_queue_t *queue = (order_queue_t *)malloc(sizeof(order_queue_t));
    queue->capacity = capacity;
    queue->size = 0;
    queue->orders = (order_event_t *)malloc(queue->capacity * sizeof(order_event_t));
    return queue;
}

int ConcatenateNumbers(char * a, int b, int c){
    char str[30];
    sprintf(str, "%s%d%d", a, b, c);
    return atoi(str);
}

int GenerateOrderID(order_event_t *order){

    return ConcatenateNumbers(order->elevator_id, order->floor, order->order_type);
}

void enqueue_order(order_queue_t *queue, order_event_t *order){
    if(queue->size == queue->capacity){
        printf("Queue is full\n");
        return;
    }
    for (int i = 0; i < queue->size; i++){
        if(queue->orders[i].order_id == order->order_id){
            printf("Order already in queue\n");
            return;
        }
    }
    queue->orders[queue->size] = *order;
    queue->size++;
}   

void dequeue_order(order_queue_t *queue, order_event_t *order){
    if(queue->size == 0){
        printf("Queue is empty\n");
        return;
    }
    for(int i = 0; i < queue->size; i++){
        if(queue->orders[i].order_id == order->order_id){
            for(int j = i; j < queue->size - 1; j++){
                queue->orders[j] = queue->orders[j + 1];
            }
            queue->size--;
            return;
        }
        else{
            printf("Order not in queue\n");
            return;
        }
    }
}

