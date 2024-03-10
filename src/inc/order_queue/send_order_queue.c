#include "send_order_queue.h"

void send_order_queue_init(TMessageCallback messageCallback, TTcpConnectionCallback connectionCallback){
    tcp_init(messageCallback, connectionCallback);
}

void send_order_queue_connect(char * ip, int port){
    tcp_openConnection(ip, port);
}

void send_order_queue_listen(int port){
    tcp_startConnectionListening(port);
}

void send_order_queue_close_connection(char * ip){
    tcp_closeConnection(ip);
}

void send_order_queue_send_order(char * ip, order_queue_t *queue){

    /* Serialising data*/
    char * buffer = malloc(sizeof(order_queue_t) + sizeof(order_event_t) * queue->capacity);
    memcpy(buffer, queue, sizeof(order_queue_t));
    memcpy(buffer + sizeof(order_queue_t), queue->orders, sizeof(order_event_t) * queue->capacity);

    tcp_send(ip, buffer, sizeof(order_queue_t) + sizeof(order_event_t) * queue->size);

    free(buffer);
}

void send_order_queue_deserialize(char * data, order_queue_t *queue){
    memcpy(&queue->size, data, sizeof(queue->size));
    memcpy(&queue->capacity, data+sizeof(queue->size), sizeof(queue->capacity));
    memcpy(queue->orders, data+sizeof(order_queue_t), sizeof(order_event_t)*queue->size);
}