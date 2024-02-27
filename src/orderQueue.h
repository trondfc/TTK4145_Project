#ifndef ORDERQUEUE_H
#define ORDERQUEUE_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


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

/* Prototypes */
void enqueue_order(order_queue_t *queue, order_event_t *order);
void dequeue_order(order_queue_t *queue, order_event_t *order);
order_queue_t * create_order_queue(int capacity);

#endif