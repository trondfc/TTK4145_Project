#include<stdlib.h>
#include<pthread.h>

#include "elevator_algorithm.h"

elevator_system_t elevator_system;

elevator_system_t* elevator_init(order_queue_t* queue)
{
    elevator_system.elevator = (elevator_status_t*)malloc(sizeof(elevator_status_t)*N_ELEVATORS);
    elevator_system.order_queue = queue;
    elevator_system.mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(elevator_system.mutex, NULL);

    for(int i = 0; i < N_ELEVATORS; i++){
        elevator_system.elevator[i].current_floor = DEFAULT_FLOOR;
        elevator_system.elevator[i].prev_floor = DEFAULT_FLOOR;
        elevator_system.elevator[i].state = IDLE;
        elevator_system.elevator[i].current_direction = DIRN_STOP;
    }
    return &elevator_system;
}

int elevator_kill()
{
    pthread_mutex_destroy(elevator_system.mutex);
    free(elevator_system.mutex);
    free(elevator_system.elevator);
}

int is_any_elevators_idle(elevator_system_t* elevator_system)
{
    for(int i = 0; i < N_ELEVATORS; i++){
        if(elevator_system->elevator[i].state == IDLE){
            return 1;
        }
    }
    return 0;
}

int is_elevators_moving_towards_floor(elevator_system_t* elevator_system, int floor)
{
    for(int i = 0; i < N_ELEVATORS; i++){

        if(elevator_system->elevator[i].current_floor == floor){
            return 1;
        }else if(elevator_system->elevator[i].current_direction == DIRN_DOWN && elevator_system->elevator[i].prev_floor < floor){
            return 1;
        }else if(elevator_system->elevator[i].current_direction == DIRN_UP && elevator_system->elevator[i].prev_floor > floor){
            return 1;
        }
    }
    return 0;
}

int is_valid_elevator_request_in_current_dir()
{
    return 0;
}





int elevator_algorithm(elevator_system_t* elevator_system){

    while(1){

    }


    return 0;
}

