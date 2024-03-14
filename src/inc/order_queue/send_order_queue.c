/**
 * @file send_order_queue.c
 * @brief Functions for sending order queue to other controllers.
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "send_order_queue.h"

/**
 * @brief Send order queue init
 * 
 * @param messageCallback 
 * @param connectionCallback 
 */
void send_order_queue_init(TMessageCallback messageCallback, TTcpConnectionCallback connectionCallback){
    tcp_init(messageCallback, connectionCallback);
}

/**
 * @brief Connect to controller
 * 
 * @param ip 
 * @param port 
 */
void send_order_queue_connect(char * ip, int port){
    tcp_openConnection(ip, port);
}

/**
 * @brief Listen for incoming connections
 * 
 * @param port 
 */
void send_order_queue_listen(int port){
    tcp_startConnectionListening(port);
}

/**
 * @brief Close connection to controller
 * 
 * @param ip 
 */
void send_order_queue_close_connection(char * ip){
    shutdown(conn_lookup(ip), SHUT_RDWR);
    close(conn_lookup(ip));
}

/**
 * @brief Send order queue to controller
 * 
 * @param ip 
 * @param queue 
 * @return int 
 */
int send_order_queue_send_order(char * ip, order_queue_t *queue){

    /* Serialising data*/
    char * buffer = malloc(sizeof(order_queue_t) + sizeof(order_event_t) * queue->capacity);
    memcpy(buffer, queue, sizeof(order_queue_t));
    memcpy(buffer + sizeof(order_queue_t), queue->orders, sizeof(order_event_t) * queue->capacity);

    int res = tcp_send(ip, buffer, sizeof(order_queue_t) + sizeof(order_event_t) * queue->size);

    free(buffer);
    return res;
}

/**
 * @brief Deserialise order queue
 * 
 * @param ip 
 * @param queue 
 * @return int 
 */
void send_order_queue_deserialize(char * data, order_queue_t *queue){
    memcpy(&queue->size, data, sizeof(queue->size));
    memcpy(&queue->capacity, data+sizeof(queue->size), sizeof(queue->capacity));
    memcpy(queue->orders, data+sizeof(order_queue_t), sizeof(order_event_t)*queue->size);
}