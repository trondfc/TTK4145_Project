#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../inc/order_queue/orderQueue.h"
#include "../inc/sverresnetwork/sverresnetwork.h"

order_queue_t * queue;

void messageReceived(const char * ip, char * data, int datalength){

  printf("Received message from %s: '%s'\n",ip,data);
  
  memcpy(&queue->size, data, sizeof(queue->size));
  memcpy(&queue->capacity, data+sizeof(queue->size), sizeof(queue->capacity));
  //memcpy(queue->orders, data+sizeof(queue->size)+sizeof(queue->capacity), sizeof(queue->orders)); 
  memcpy(queue->orders, data+sizeof(order_queue_t), sizeof(order_event_t)*queue->size);

  printf("Queue size: %d\n", queue->size);
  printf("Queue capacity: %d\n", queue->capacity);
  for(int i = 0; i < queue->size; i++){
      printf("Order %d ID: %d\n", i, queue->orders[i].order_id);
      printf("Order %d elevator_id: %d\n", i, queue->orders[i].elevator_id);
      printf("Order %d floor: %d\n", i, queue->orders[i].floor);
      printf("Order %d order_type: %d\n", i, queue->orders[i].order_type);
      printf("\n\n");
  }

    
}

void connectionStatus(const char * ip, int status){

  printf("A connection got updated %s: %d\n",ip,status);
}



int main(){

  queue = create_order_queue(10);

  tcp_init(messageReceived,connectionStatus);
  tcp_startConnectionListening(9000);

  sleep(100);
  return 0;
}
