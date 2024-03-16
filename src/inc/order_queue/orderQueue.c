/**
 * @file orderQueue.c
 * @brief Functions for creating, and maintaining an order queue.
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "orderQueue.h"

/**
 * @brief Create a order queue object
 * 
 * @param capacity 
 * @return order_queue_t* 
 */
order_queue_t * create_order_queue(int capacity){
    order_queue_t *queue = (order_queue_t *)malloc(sizeof(order_queue_t));
    queue->capacity = capacity;
    queue->size = 0;
    queue->orders = (order_event_t *)malloc(queue->capacity * sizeof(order_event_t));
    queue->queue_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(queue->queue_mutex, NULL);
    return queue;
}

/**
 * @brief Concatenate the numbers from the order event to create a unique order id
 * 
 * @param a - IP-String
 * @param b 
 * @param c 
 * @return long 
 */
long ConcatenateNumbers(char * a, int b, int c){
    char aClean[16] = "";
    char str[30];

    int j=0;
    for (int i = 0; a[i] != '\0'; i++) {
        if (isdigit(a[i])) {
            aClean[j++] = a[i];
        }
    }

    sprintf(str, "%s%d%d", aClean, b, c);
    printf("Order ID: %s\n", str);
    return atol(str);
}

/**
 * @brief Generate a unique order id
 * 
 * @param order 
 * @return long 
 */
long GenerateOrderID(order_event_t *order){

    return ConcatenateNumbers(order->elevator_id, order->floor, order->order_type);
}

/**
 * @brief Add an order to the queue
 * 
 * @param queue 
 * @param order 
 */
void enqueue_order(order_queue_t *queue, order_event_t *order){
    if(queue->size == queue->capacity){
        printf("Queue is full\n");
        return;
    }
    //pthread_mutex_lock(queue->queue_mutex);
    for (int i = 0; i < queue->size; i++){
        if(queue->orders[i].order_id == order->order_id){
            printf("Order already in queue\n");
            return;
        }
    }
    queue->orders[queue->size] = *order;
    queue->size++;
    //pthread_mutex_unlock(queue->queue_mutex);
}   

/**
 * @brief Remove an order from the queue
 * 
 * @param queue 
 * @param order 
 */
void dequeue_order(order_queue_t *queue, order_event_t *order){
    if(queue->size == 0){
        printf("Queue is empty\n");
        return;
    }
    //memset(order, -1, sizeof(order_event_t));
    //pthread_mutex_lock(queue->queue_mutex);
    for(int i = 0; i < queue->size; i++){
        if(queue->orders[i].order_id == order->order_id){
            for(int j = i; j < queue->size - 1; j++){
                queue->orders[j] = queue->orders[j + 1];
            }
            queue->size--;
            return;
        }
        else{
            //printf("Order not in queue\n");
            return;
        }
    }
    //pthread_mutex_unlock(queue->queue_mutex); 
}

