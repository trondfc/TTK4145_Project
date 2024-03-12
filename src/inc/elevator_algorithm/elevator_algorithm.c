#include<stdlib.h>
#include<pthread.h>
#include<stdbool.h>

#include "elevator_algorithm.h"
#include "../order_queue/orderQueue.h"
#include "../elevator_hardware/elevator_hardware.h"
#include "../../main.c"

elevator_status_t* elevator_status_init()
{
    elevator_status_t* elevator_status = (elevator_status_t*)malloc(N_ELEVATORS * sizeof(elevator_status_t));
    for(int i = 0; i < N_ELEVATORS; i++){
        elevator_status[i].alive = false;
        elevator_status[i].floor = DEFAULT_FLOOR;
        elevator_status[i].elevator_state = ELEVATOR_IDLE;
        elevator_status[i].obstruction = false;
        elevator_status[i].stop_at_floor = false;
        elevator_status[i].door_open = false;
        elevator_status[i].emergency_stop = false;
    }
    return elevator_status;
}


int is_any_elevator_moving_towards_hall_request(elevator_status_t* elevator_status, int floor)
//is any elevator moving towards requested floor and has same request dir
{
    for(int i = 0; i < N_ELEVATORS; i++){
        if(elevator_status[i].elevator_state == MOVING_UP && elevator_status[i].floor < floor){
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
        if(elevator_status[i].elevator_state == ELEVATOR_IDLE){
            return i;
        }
    }
    return -1;
}



int elevator_algorithm(elevator_status_t* elevator_status, order_queue_t* order_queue){

    while(1){

    }


    return 0;
}

