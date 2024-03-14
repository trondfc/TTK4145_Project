#include<stdlib.h>
#include<pthread.h>
#include<stdbool.h>
#include<time.h>

#include "../logger.h"

#include "elevator_algorithm.h"
#include "../order_queue/orderQueue.h"
#include "../elevator_hardware/elevator_hardware.h"


pthread_t elevator_thread[N_ELEVATORS], request_handler_thread;
elevator_arg_t elevator_arg[N_ELEVATORS];

uint64_t get_timestamp(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
}

int is_elevator_dir_up(elevator_status_t* selected_elevator)
{
    return (selected_elevator->elevator_state == ELEVATOR_DIR_UP_AND_MOVING     || selected_elevator->elevator_state == ELEVATOR_DIR_UP_AND_STOPPED) ?  1 : 0;
}

int is_elevator_dir_down(elevator_status_t* selected_elevator)
{
    return (selected_elevator->elevator_state == ELEVATOR_DIR_DOWN_AND_MOVING     || selected_elevator->elevator_state == ELEVATOR_DIR_DOWN_AND_STOPPED) ?  1 : 0;
}

int start_elevator(elevator_status_t* elevator_status, int elevator_no)
{
    if(elevator_status[elevator_no].elevator_state == ELEVATOR_DIR_UP_AND_STOPPED){
        elevator_status[elevator_no].elevator_state = ELEVATOR_DIR_UP_AND_MOVING;
        //printd(LOG_LEVEL_INFO, sprintf("INFO: Elevator %d moving UP", elevator_no));
    }
    else if(elevator_status[elevator_no].elevator_state == ELEVATOR_DIR_DOWN_AND_STOPPED){
        elevator_status[elevator_no].elevator_state = ELEVATOR_DIR_DOWN_AND_MOVING;
        //printd(LOG_LEVEL_INFO, sprintf("INFO: Elevator %d moving UP", elevator_no));
    }
    return 0;
}

int stop_elevator(elevator_status_t* elevator_status, int elevator_no)
{
    if(elevator_status[elevator_no].elevator_state == ELEVATOR_DIR_UP_AND_MOVING){
        elevator_status[elevator_no].elevator_state = ELEVATOR_DIR_UP_AND_STOPPED;
        //printd(LOG_LEVEL_INFO, sprintf("INFO: Elevator %d STOPPED", elevator_no));
    }
    else if(elevator_status[elevator_no].elevator_state == ELEVATOR_DIR_DOWN_AND_MOVING){
        elevator_status[elevator_no].elevator_state = ELEVATOR_DIR_DOWN_AND_STOPPED;
        //printd(LOG_LEVEL_INFO, sprintf("INFO: Elevator %d STOPPED", elevator_no));
    }
    return 0;
}

int open_elevator_door(elevator_status_t* elevator_status, int elevator_no)
{
    elevator_status_t* selected_elevator = &elevator_status[elevator_no];
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
int at_valid_floor_request(elevator_status_t* elevator_status, int elevator_no, order_queue_t* order_queue)
{
    elevator_status_t* selected_elevator = &elevator_status[elevator_no];
    for(int i = 0; i < order_queue->size; i++){

        if(order_queue->orders[i].floor != selected_elevator->floor){
            return 0;
        }

        switch(order_queue->orders[i].order_type){
            case GO_TO:
                return 1;
                break;
            case UP_FROM:
                if(is_elevator_dir_up(selected_elevator)){
                    return 1;
                }
                break;
            case DOWN_FROM:
                if(is_elevator_dir_down(selected_elevator)){
                    return 1;
                }
                break;
            default:
                break;
        }
    }
    return 0;
}

int is_valid_requests_in_current_direction(elevator_status_t* elevator_status, int elevator_no, order_queue_t* order_queue)
{}

int complete_floor_request(int floor, order_queue_t* order_queue)
{
    pthread_mutex_lock(order_queue->queue_mutex);
    for(int i = 0; i < order_queue->size;i++){
        if(order_queue->orders[i].floor == floor){
            dequeue_order(order_queue, &order_queue->orders[i]);
            i = -1; //Reset Iterator
        }
    }
    pthread_mutex_unlock(order_queue->queue_mutex);
    return 0;
}


void* elevator_control(void* arg){
    //Handle and unpack arguments
    elevator_arg_t* elevator_arg = (elevator_arg_t*)arg;
    elevator_status_t* elevator_status = elevator_arg->elevator_status;
    order_queue_t* order_queue = elevator_arg->order_queue;
    int elevator_no = elevator_arg->elevator_no;
    
    elevator_status_t* selected_elevator = &elevator_status[elevator_no];
    while(1){
        usleep(1000);
        if(selected_elevator->alive == false){
            continue;
        }
        if(selected_elevator->emergency_stop == true){
            selected_elevator->elevator_state = ELEVATOR_EMERGENCY_STOP;
            continue;
        }

        switch(selected_elevator->elevator_state){
        case ELEVATOR_EMERGENCY_STOP:
            break;

        case ELEVATOR_IDLE:
            break;

        case ELEVATOR_DIR_UP_AND_MOVING || ELEVATOR_DIR_DOWN_AND_MOVING:
            if(selected_elevator->floor == -1){
                continue;}

            if(at_valid_floor_request(elevator_status, elevator_no, order_queue)){
                stop_elevator(elevator_status, elevator_no);
                open_elevator_door(elevator_status, elevator_no);
                complete_floor_request(elevator_status->floor, order_queue);
                start_elevator(elevator_status, elevator_no);
            }
            break;
        default:
            break;
        }
        open_elevator_door(elevator_status, elevator_no);
    }
    return NULL;
}

void* request_handler(void* arg){
    order_queue_t* order_queue = (order_queue_t*)arg;
    while(1){
        usleep(1000);
        if(order_queue->size == 0){
            continue;
        }
        for (int i = 0; i < order_queue->size; i++){
                
            }
        }


    return NULL;
}
 

int elevator_status_init(elevator_status_t* elevator_status, order_queue_t* order_queue)
{

    for(int i = 0; i < N_ELEVATORS; i++){
        //Init all variables that are controlled locally
        elevator_status[i].elevator_state = ELEVATOR_IDLE;
        elevator_status[i].door_open = false;

        elevator_arg[i].elevator_no = i;
        elevator_arg[i].elevator_status = elevator_status;
        pthread_create(&elevator_thread[i], NULL, elevator_control, (void*)&elevator_arg[i]);
    }

    pthread_create(&request_handler_thread, NULL, request_handler, (void*)&elevator_arg[0]);
    return 0;
}

int elevator_status_kill()
{
    pthread_cancel(request_handler_thread);
    for(int i = 0; i < N_ELEVATORS; i++){
        pthread_cancel(elevator_thread[i]);
        
    }
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

/* 
int is_elevator_moving(elevator_status_t* elevator_status, int elevator_no)
{
    if(elevator_status[elevator_no].elevator_state == ELEVATOR_DIR_UP_AND_MOVING || elevator_status[elevator_no].elevator_state == ELEVATOR_DIR_DOWN_AND_MOVING){
        return 1;
    }
    return 0;
}
 */

