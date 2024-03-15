#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<unistd.h>

#include "../../order_queue/orderQueue.h"
#include "../../config.h"

#include "../../logger.h"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "../elevator_algorithm.h"

void print_order_data(order_event_t *order);
void print_elevator_status_data(elevator_status_t *elevator_status);
int set_order(order_event_t *order, int floor, order_types_t order_type, order_status_t order_status, char* elevator_id);
int set_elevator_status(elevator_status_t *elevator_status, int floor, int elevator_state, int at_floor);

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
    sleep(1);   //Wait for init
    order_event_t order;
    order.order_id = 1;
    order_event_t order2;
    order.order_id = 2;



//---------------------------------------------------------------
//               Set Elevator in correct dir from IDLE
//---------------------------------------------------------------

    //Test: GOTO order is recieved and elevator is idle
    set_order(&order, 2, GO_TO, RECIVED, elevator_id[0]);
    usleep(100000);
    assert(queue->orders[0].order_status == RECIVED);
    assert(elevator_status[0].elevator_state == ELEVATOR_IDLE);
    dequeue_order(queue, &order);

    //Test: GOTO order is synched and elevator is idle at floor 0
    set_elevator_status(&elevator_status[0], 0, ELEVATOR_IDLE, true);
    set_order(&order, 2, GO_TO, SYNCED, elevator_id[0]);
    queue->orders[0].order_status = SYNCED;
    usleep(100000);
    assert(queue->orders[0].order_status == ACTIVE);
    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_UP_AND_MOVING);
    dequeue_order(queue, &order);

    //TEST: UP_FROM order is recieved and elevator is idle at floor 0
    set_elevator_status(&elevator_status[0], 0, ELEVATOR_IDLE, true);
    set_order(&order, 2, UP_FROM, SYNCED, elevator_id[0]);
    queue->orders[0].order_status = SYNCED;
    usleep(100000);
    assert(queue->orders[0].order_status == ACTIVE);
    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_UP_AND_MOVING);
    dequeue_order(queue, &order);
    

    //TEST: UP_FROM order is recieved and elevator is idle at floor 3
    set_elevator_status(&elevator_status[0], 3, ELEVATOR_IDLE, true);
    set_order(&order, 2, UP_FROM, SYNCED, elevator_id[0]);
    queue->orders[0].order_status = SYNCED;
    usleep(100000);
    assert(queue->orders[0].order_status == ACTIVE);
    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_DOWN_AND_MOVING);
    dequeue_order(queue, &order);


    //Test: DOWN_FROM order is recieved and elevator is idle at floor 3
    set_elevator_status(&elevator_status[0], 3, ELEVATOR_IDLE, true);
    set_order(&order, 2, DOWN_FROM, SYNCED, elevator_id[0]);
    queue->orders[0].order_status = SYNCED;
    usleep(100000);
    assert(queue->orders[0].order_status == ACTIVE);
    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_DOWN_AND_MOVING);
    dequeue_order(queue, &order);


    //TEST: DOWN_FROM order is recieved and elevator is idle at floor 0
    set_elevator_status(&elevator_status[0], 0, ELEVATOR_IDLE, true);
    set_order(&order, 2, DOWN_FROM, SYNCED, elevator_id[0]);
    queue->orders[0].order_status = SYNCED;
    usleep(100000);
    assert(queue->orders[0].order_status == ACTIVE);
    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_UP_AND_MOVING);
    dequeue_order(queue, &order);

//---------------------------------------------------------------
//               Stop Elevator at correct floor
//---------------------------------------------------------------
    
    //Test: Elevator stops and compleates GOTO at floor 2 and floor 3
    set_elevator_status(&elevator_status[0], 0, ELEVATOR_DIR_UP_AND_MOVING, true);
    set_order(&order,  2, GO_TO, ACTIVE, elevator_id[0]);
    set_order(&order2, 3, GO_TO, ACTIVE, elevator_id[0]);
    usleep(100000);

    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_UP_AND_MOVING);
    assert(queue->size == 2);
    usleep(100000);

    set_elevator_status(&elevator_status[0], 1, ELEVATOR_DIR_UP_AND_MOVING, true);
    usleep(100000);

    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_UP_AND_MOVING);
    assert(queue->size == 2);
    set_elevator_status(&elevator_status[0], 2, ELEVATOR_DIR_UP_AND_MOVING, true);
    usleep(200000);

    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_UP_AND_MOVING);
    assert(queue->size == 1);
    set_elevator_status(&elevator_status[0], 3, ELEVATOR_DIR_UP_AND_MOVING, true);
    usleep(100000);

    assert(elevator_status[0].elevator_state == ELEVATOR_IDLE);
    assert(queue->size == 0);


    //Test: Elevator stops and compleates UP_FROM at floor 2 and floor 3
    set_elevator_status(&elevator_status[0], 0, ELEVATOR_DIR_UP_AND_MOVING, true);
    set_order(&order,  2, UP_FROM, ACTIVE, elevator_id[0]);
    set_order(&order2, 3, UP_FROM, ACTIVE, elevator_id[0]);
    usleep(200000);

    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_UP_AND_MOVING);
    assert(queue->size == 2);
    usleep(200000);

    set_elevator_status(&elevator_status[0], 1, ELEVATOR_DIR_UP_AND_MOVING, true);
    usleep(200000);

    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_UP_AND_MOVING);
    assert(queue->size == 2);
    set_elevator_status(&elevator_status[0], 2, ELEVATOR_DIR_UP_AND_MOVING, true);
    usleep(200000);

    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_UP_AND_MOVING);
    assert(queue->size == 1);
    set_elevator_status(&elevator_status[0], 3, ELEVATOR_DIR_UP_AND_MOVING, true);
    usleep(200000);

    assert(elevator_status[0].elevator_state == ELEVATOR_IDLE);
    assert(queue->size == 0);

    printf("-------------------Test------------------------------\n");
    //Test: Elevator passes and compleates DOWN_FROM at floor 2, stoppes at UP_FROM floor 3, changed to DOWN_FROM and stops at floor 2

    set_elevator_status(&elevator_status[0], 0, ELEVATOR_DIR_UP_AND_MOVING, true);
    set_order(&order,  2, DOWN_FROM, ACTIVE, elevator_id[0]);
    set_order(&order2, 3, UP_FROM, ACTIVE, elevator_id[0]);

    usleep(200000);
    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_UP_AND_MOVING);
    assert(queue->size == 2);

    set_elevator_status(&elevator_status[0], 1, elevator_status[0].elevator_state, true);
    usleep(200000);
    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_UP_AND_MOVING);
    assert(queue->size == 2);

    set_elevator_status(&elevator_status[0], 2, elevator_status[0].elevator_state, true);
    usleep(200000);
    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_UP_AND_MOVING);
    assert(queue->size == 2);

    set_elevator_status(&elevator_status[0], 3, elevator_status[0].elevator_state, true);
    usleep(1000000);
    assert(elevator_status[0].elevator_state == ELEVATOR_DIR_DOWN_AND_MOVING);
    assert(queue->size == 1);


    set_elevator_status(&elevator_status[0], 2, elevator_status[0].elevator_state, true);
    usleep(200000);
    assert(elevator_status[0].elevator_state == ELEVATOR_IDLE);
    assert(queue->size == 0);

    printf("-------------------All Tests Passed----------------------\n");
    elevator_algorithm_kill();
}

int set_order(order_event_t *order, int floor, order_types_t order_type, order_status_t order_status, char* elevator_id){
    order->order_id = order->order_id;
    order->order_type = order_type;
    order->floor = floor;
    order->order_status = order_status;
    strcpy(order->elevator_id, elevator_id);
    enqueue_order(queue, order);
    return 0;
}

int set_elevator_status(elevator_status_t *elevator_status, int floor, int elevator_state, int at_floor){
    elevator_status->floor = floor;
    elevator_status->elevator_state = elevator_state;
    elevator_status->at_floor = at_floor;
    printf("At Floor: %d \t Elevator State %d \n", elevator_status->at_floor, elevator_status->elevator_state);
    return 0;
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