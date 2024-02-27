#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Prototypes */
bool compare_orders(order_event_t *order, order_event_t *order2);
void enqueue_order(order_queue_t *queue, order_event_t *order);
void dequeue_order(order_queue_t *queue, order_event_t *order);


typedef enum order_types_t {
    UP_FROM = 0,
    DOWN_FROM = 1,
    GO_TO = 2
}order_types_t;

typedef enum order_status_t{
    RECIVED = 0,
    ACTIVE = 1,
    COMPLETED = 2
}order_status_t;

typedef struct order_event_t {
    int order_id;
    uint8_t elevator_id;
    order_types_t order_type;
    order_status_t order_status;
    uint64_t timestamp;
    uint8_t controller_id;
}order_event_t;

typedef struct order_queue_t {
    order_event_t *orders;
    int size;
    int capacity;
}order_queue_t;

order_queue_t * create_order_queue(int capacity){
    order_queue_t *queue = (order_queue_t *)malloc(sizeof(order_queue_t));
    queue->capacity = capacity;
    queue->size = 0;
    queue->orders = (order_event_t *)malloc(queue->capacity * sizeof(order_event_t));
    return queue;
}

