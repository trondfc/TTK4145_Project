#include <stdio.h>
#include <unistd.h>
#include "../inc/order_queue/orderQueue.h"
#include "../inc/sverresnetwork/sverresnetwork.h"

void 
messageReceived(const char * ip, char * data, int datalength){

    order_event_t* order = (order_event_t*)data;

  printf("Received message from %s: '%s'\n",ip,data);
    printf("Order id: %d\n", order->order_id);
    printf("Elevator id: %d\n", order->elevator_id);
    printf("Floor: %d\n", order->floor);
    printf("Order type: %d\n", order->order_type);
    printf("Order status: %d\n", order->order_status);
    printf("Timestamp: %ld\n", order->timestamp);
    printf("Controller id: %d\n", order->controller_id);
    
}

void 
connectionStatus(const char * ip, int status){

  printf("A connection got updated %s: %d\n",ip,status);
}



int main(){

  tcp_init(messageReceived,connectionStatus);
  tcp_startConnectionListening(9000);

  sleep(100);
  return 0;
}
