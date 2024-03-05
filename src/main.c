/* Include libraries */
#include "inc/elevator_control/elevator_button_inputs.h"
#include "inc/elevator_hardware/elevator_hardware.h"
#include "inc/order_queue/orderQueue.h"
#include "inc/process_pair/process_pair.h"

#define QUEUE_SIZE 20
#define ORDER_POLL_DELAY 100 * 1000 // 100 ms

/* Define global variables */
order_queue_t *queue;



/* Make pthread functions */
void *orderQueue(void *arg){
  create_order_queue(QUEUE_SIZE);
  /* HERE WE NEED TO CHECK IF WE'RE MASTER */
  while(Hoststate == Master){
    /* HERE WE NEED TO RE-WRITE TO POLL ALL ELEVATORS */
    if (poll_new_orders(&elevator_1, queue)){
      for(int i = 0; i < queue->size; i++){
        printf("%d \t Order ID: %d\n", i , queue->orders[i].order_id);
      }
      printf("\n");
    }
    usleep(ORDER_POLL_DELAY);
  }
}

void *elevatorAlgorithm(void *arg){
  
}


int main(int argc, char * argv[]){
  

}

