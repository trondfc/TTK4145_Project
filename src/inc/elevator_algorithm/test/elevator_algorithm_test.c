#include<pthread.h>
#include<stdlib.h>


#include "../elevator_algorithm.h"
#include "../../order_queue/orderQueue.h"
#include "../../config.h"


elevator_status_t* elevator_status;
order_queue_t* queue; 

int main(){

    elevator_status = (elevator_status_t*)malloc(N_ELEVATORS * sizeof(elevator_status_t));
    queue = create_order_queue(10);

    elevator_status_init(elevator_status, queue);
    printf("main\n");
    elevator_status_kill();
}