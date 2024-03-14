/**
 * @file orderQueue.h
 * @brief Functions for creating, and maintaining an order queue.
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ORDERQUEUE_H
#define ORDERQUEUE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>


typedef enum order_types_t {
    UP_FROM = 0,
    DOWN_FROM = 1,
    GO_TO = 2
}order_types_t;

typedef enum order_status_t{
    RECIVED = 0,
    SYNCED = 1,
    NOTIFIED = 2,
    ACTIVE = 3,
    COMPLETED = 4
}order_status_t;

typedef struct order_event_t {
    long order_id;
    char elevator_id[16];
    uint8_t floor;
    order_types_t order_type;
    order_status_t order_status;
    time_t timestamp;
    uint8_t controller_id;
}order_event_t;

typedef struct order_queue_t {
    int size;
    int capacity;
    order_event_t *orders;
    pthread_mutex_t *queue_mutex;
}order_queue_t;

/* Prototypes */
long GenerateOrderID(order_event_t *order);
void enqueue_order(order_queue_t *queue, order_event_t *order);
void dequeue_order(order_queue_t *queue, order_event_t *order);
order_queue_t * create_order_queue(int capacity);

#endif