#include<stdlib.h>
#include<pthread.h>
#include<stdbool.h>
#include<time.h>

#include "elevator_algorithm.h"
#include "../order_queue/orderQueue.h"
#include "../elevator_hardware/elevator_hardware.h"


pthread_t elevator_thread[N_ELEVATORS];
elevator_status_t* elevator_status;

uint64_t get_timestamp(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
}


elevator_status_t* get_elevator_status(){
    return elevator_status;
}

int open_elevator_door(elevator_status_t* elevator_status, int elevator_nr)
{
    elevator_status_t* selected_elevator = &elevator_status[elevator_nr];
    elevator_state_t prev_elevator_state = selected_elevator->elevator_state;
    

    if  (selected_elevator->elevator_state = ELEVATOR_DIR_UP_AND_MOVING){
        selected_elevator->elevator_state = ELEVATOR_DIR_UP_AND_STOPPED;
    }
    else if (selected_elevator->elevator_state = ELEVATOR_DIR_DOWN_AND_MOVING){
        selected_elevator->elevator_state = ELEVATOR_DIR_DOWN_AND_STOPPED;
    }else{
        printf("Elevator is not moving\n");
    }
    
    selected_elevator->door_open = true;
    uint64_t door_open_timestamp_us = get_timestamp();
    while(door_open_timestamp_us + ELEVATOR_DOOR_OPEN_TIME > get_timestamp()){
        usleep(1000);
        if(selected_elevator->obstruction){
            door_open_timestamp_us = get_timestamp();
        }
    }
    selected_elevator->door_open = false;
    selected_elevator->elevator_state = prev_elevator_state;
    return 0;
}


void* elevator_algorithm(void* arg){
    int elevator_nr = *(int*)arg;
    
    elevator_status_t* selected_elevator = &elevator_status[elevator_nr];
    while(1){
        usleep(1000);
        if(selected_elevator->alive == false){
            continue;
        }
        switch(selected_elevator->elevator_state){
        case ELEVATOR_EMERGENCY_STOP:
            break;

        case ELEVATOR_IDLE:
            break;
        default:
            break;
        }
        open_elevator_door(elevator_status, elevator_nr);
    }
    return NULL;
}
 

int elevator_status_init(order_queue_t* order_queue)
{
    elevator_status = (elevator_status_t*)malloc(N_ELEVATORS * sizeof(elevator_status_t));
    for(int i = 0; i < N_ELEVATORS; i++){
        elevator_status[i].alive = false;
        elevator_status[i].floor = DEFAULT_FLOOR;
        elevator_status[i].elevator_state = ELEVATOR_IDLE;
        elevator_status[i].obstruction = false;
        elevator_status[i].door_open = false;
        elevator_status[i].emergency_stop = false;

        pthread_create(&elevator_thread[i], NULL, elevator_algorithm, (void*)&i);
    }
    return 0;
}

int elevator_status_kill()
{
    for(int i = 0; i < N_ELEVATORS; i++){
        pthread_cancel(elevator_thread[i]);
    }
    free(elevator_status);
    return 0;
}  


/* 
int is_any_elevator_moving_towards_hall_request(elevator_status_t* elevator_status, int requested_flooor)
//is any elevator moving towards requested floor and has same request dir
{
    for(int i = 0; i < N_ELEVATORS; i++){
        if(elevator_status[i].elevator_state == MOVING_UP && elevator_status[i].floor < requested_flooor){
            return 1;
        }
        else if(elevator_status[i].elevator_state == MOVING_DOWN && elevator_status[i].floor > floor){
            return 1;
        }
    }
    return 0;
}
int get_idle_elevator( elevator_status_t* elevator_status)
{
    for(int i = 0; i < N_ELEVATORS; i++){
\
        if(elevator_status[i].elevator_state == ELEVATOR_IDLE && elevator_status[i].alive == true){
            return i;
        }
    }
    return -1;
}
 */



