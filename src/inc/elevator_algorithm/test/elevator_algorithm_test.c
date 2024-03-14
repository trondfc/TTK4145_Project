#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>

#include "../../order_queue/orderQueue.h"
#include "../../config.h"

#include "../../logger.h"
#define LOG_LEVEL LOG_LEVEL_INFO

#define N_ELEVATORS 1

#include "../elevator_algorithm.h"

void print_order_data(order_event_t *order);
void print_elevator_status_data(elevator_status_t *elevator_status);

elevator_status_t* elevator_status;
order_queue_t* queue; 


int main(){

elevator_status = (elevator_status_t*)malloc(N_ELEVATORS * sizeof(elevator_status_t));
queue = create_order_queue(10);
elevator_algorithm_init(elevator_status, queue);
char elevator_id[N_ELEVATORS][16];
for(int i = 0; i < N_ELEVATORS; i++){
        sprintf(elevator_id[i], "192.168.0.%d", i+1);
        strcpy(elevator_status[i].elevator.ip, elevator_id[i]);
        elevator_status[i].alive = true;
        elevator_status[i].floor = 0;
        printf("%s\n", elevator_id[i]);
    }

    //-----------------Test of elevator_algorithm.h-----------------



    //--------------------------Queue ------------------------------
    sleep(1);
    order_event_t order;
    order.order_id = 1;
    order.order_type = GO_TO;
    order.floor = 2;
    order.order_status = RECIVED;
    strcpy(order.elevator_id, elevator_id[0]);
    enqueue_order(queue, &order);
    usleep(100000);
    
    assert(elevator_status[0].elevator_state == ELEVATOR_IDLE);

    queue->orders[0].order_status = SYNCED;
    usleep(100000);
    assert(queue->orders[0].order_status == ACTIVE);
    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_UP_AND_MOVING);




    printf("------------------\n");
    //print_order_data(&queue->orders[0]); 
    printf("\n\n");
    //print_elevator_status_data(&elevator_status[0]);


    //print_order_data(queue->orders[0]);
    

    elevator_status[0].floor = 2;
    sleep(6);

    
    printf("done\n");
    elevator_algorithm_kill();
}




void print_order_data(order_event_t *order){
    printf("Order ID: %ld\n", order->order_id);
    printf("Elevator ID: %s\n", order->elevator_id);
    printf("Floor: %d\n", order->floor);
    printf("Order type: %d\n", order->order_type);
    printf("Order status: %d\n", order->order_status);
    printf("Timestamp: %ld\n", order->timestamp);
    printf("Controller ID: %d\n", order->controller_id);
}

void print_elevator_status_data(elevator_status_t *elevator_status){
    printf("Elevator ID: %s\n", elevator_status->elevator.ip);
    printf("Elevator state: %d\n", elevator_status->elevator_state);
    printf("Floor: %d\n", elevator_status->floor);
    printf("Door Obstruction: %d\n", elevator_status->obstruction);
    printf("Door Open: %d\n", elevator_status->door_open);
    printf("E-STOP: %d\n", elevator_status->emergency_stop);
}