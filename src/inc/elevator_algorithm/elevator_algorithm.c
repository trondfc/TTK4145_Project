#include<stdlib.h>
#include<pthread.h>
#include<stdbool.h>
#include<unistd.h>
#include<time.h>

#include "../logger.h"

#include "../order_queue/orderQueue.h"
#include "../elevator_hardware/elevator_hardware.h"
#include "elevator_algorithm.h"

#define LOG_LEVEL LOG_LEVEL_INFO

pthread_t elevator_thread[N_ELEVATORS], request_handler_thread;
elevator_arg_t elevator_arg[N_ELEVATORS];

//TODO: Double check mutexes in elevator_status and queue
uint64_t get_timestamp(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
}

int is_elevator_moving(elevator_status_t* selected_elevator)
{
    return (selected_elevator->elevator_state == ELEVATOR_DIR_UP_AND_MOVING || selected_elevator->elevator_state == ELEVATOR_DIR_DOWN_AND_MOVING) ? 1 : 0;
}

int is_elevator_dir_up(elevator_status_t* selected_elevator)
{
    return (selected_elevator->elevator_state == ELEVATOR_DIR_UP_AND_MOVING     || selected_elevator->elevator_state == ELEVATOR_DIR_UP_AND_STOPPED) ?  1 : 0;
}

int is_elevator_dir_down(elevator_status_t* selected_elevator)
{
    return (selected_elevator->elevator_state == ELEVATOR_DIR_DOWN_AND_MOVING     || selected_elevator->elevator_state == ELEVATOR_DIR_DOWN_AND_STOPPED) ?  1 : 0;
}

int set_elevator_state(elevator_status_t* elevator_status, int elevator_no, elevator_state_t state)
{
    #if LOG_LEVEL <= LOG_LEVEL_INFO
        printf("INFO: \tElevator %d \t Set to  %d\n", state);
    #endif
    elevator_status[elevator_no].elevator_state = state;
}

int start_elevator(elevator_status_t* elevator_status, int elevator_no)
{
    #if LOG_LEVEL <= LOG_LEVEL_INFO
        printf("INFO: \tElevator %d \t Starting\n", elevator_no);
    #endif
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
    #if LOG_LEVEL <= LOG_LEVEL_INFO
        printf("INFO: \tElevator %d \t Stopping\n", elevator_no);
    #endif
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
    
    selected_elevator->door_open = true;
    #if LOG_LEVEL <= LOG_LEVEL_INFO
        printf("INFO: \tElevator %d \t Door Opening\n", elevator_no);
    #endif
    uint64_t door_open_timestamp_us = get_timestamp();
    while(door_open_timestamp_us + ELEVATOR_DOOR_OPEN_TIME > get_timestamp()){
        usleep(10000);
        if(selected_elevator->obstruction){
            #if LOG_LEVEL == DEBUG
                printf("Obstruction Detected");
            #endif
            door_open_timestamp_us = get_timestamp();
        }
    }
    selected_elevator->door_open = false;

    #if LOG_LEVEL <= LOG_LEVEL_INFO
        printf("INFO: \tElevator %d \t Door Closing\n", elevator_no);
    #endif
    return 0;
}
int at_valid_floor_request(elevator_status_t* elevator_status, int elevator_no, order_queue_t* order_queue)
{
    elevator_status_t* selected_elevator = &elevator_status[elevator_no];
    for(int i = 0; i < order_queue->size; i++){

        if(order_queue->orders[i].floor != selected_elevator->floor){
            continue;
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
                printf("Invalid order type\n");
                break;
        }
    }
    return 0;
}

int get_elevator_no_from_order(elevator_status_t* elevator_status, order_event_t* order)
{
    //TODO: Implement
    for(int i = 0; i < N_ELEVATORS; i++){
        if(strcmp(elevator_status[i].elevator.ip, order->elevator_id) == 0){
            return i;
        }
    }
    printf("Elevator not found\n"); 
    return 0;
}

int is_any_valid_request_in_current_elevator_direction(elevator_status_t* elevator_status, int elevator_no, order_queue_t* order_queue)
{
    elevator_status_t* selected_elevator = &elevator_status[elevator_no];
    for(int i = 0; i < order_queue->size; i++){
        order_event_t* selected_order = &order_queue->orders[i];

        if (selected_order->order_status == RECIVED) continue;

        if (selected_order->order_type == GO_TO && elevator_no != get_elevator_no_from_order(elevator_status, selected_order)) continue;

        if(order_queue->orders[i].floor != selected_elevator->floor){
            continue;
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
                printf("Invalid order type\n");
                break;
        }
    }
    return 0;
}

int is_elevator_moving_towards_request(elevator_status_t* elevator_status, int elevator_no, int requested_flooor)
{
    elevator_status_t* selected_elevator = &elevator_status[elevator_no];
    if(is_elevator_dir_up(selected_elevator) && selected_elevator->floor < requested_flooor){
            return 1;
        }
    else if(is_elevator_dir_down(selected_elevator) && selected_elevator->floor > requested_flooor){
            return 1;
        }
    return 0;
}

int is_any_elevator_moving_towards_request(elevator_status_t* elevator_status, int requested_flooor)
{
    for(int i = 0; i < N_ELEVATORS; i++){
        if(is_elevator_moving_towards_request(elevator_status, i, requested_flooor)){
            return 1;
        }   
    }
    return 0;
}

int get_closest_idle_elevator(elevator_status_t* elevator_status, int requested_flooor){
    int closest_elevator = -1;
    for(int i = 0; i < N_ELEVATORS; i++){
        if(elevator_status[i].alive == false) continue;
        if(elevator_status[i].elevator_state == ELEVATOR_IDLE){
            if(closest_elevator == -1){
                closest_elevator = i;
            }
            else if(abs(elevator_status[i].floor - requested_flooor) < abs(elevator_status[closest_elevator].floor - requested_flooor)){
                closest_elevator = i;
            }
        }
    }
    return closest_elevator;
}

int set_elevator_dir_towards_floor(elevator_status_t* elevator_status, int elevator_no, int floor)
{
    //TODO: Should compare with prev floor because floor can be set to -1 when between floors
    if(elevator_status[elevator_no].floor < floor){
        elevator_status[elevator_no].elevator_state = ELEVATOR_DIR_UP_AND_MOVING;
    }
    else if(elevator_status[elevator_no].floor > floor){
        elevator_status[elevator_no].elevator_state = ELEVATOR_DIR_DOWN_AND_MOVING;
    }
    return 0;
}




int complete_floor_request(int floor, int elevator_no, elevator_status_t* elevator_status, order_queue_t* order_queue)
{
    pthread_mutex_lock(order_queue->queue_mutex);
    for(int i = 0; i < order_queue->size;i++){
        if(order_queue->orders[i].order_status == RECIVED) continue;
        if(order_queue->orders[i].order_type==GO_TO){
            if(elevator_no != get_elevator_no_from_order(elevator_status, &order_queue->orders[i])) continue;
        }

        if(order_queue->orders[i].floor == floor){
            //TODO: Should only synched orders be removed?
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

    printf("started elevator_control for elevator %d\n", elevator_no);
    
    elevator_status_t* selected_elevator = &elevator_status[elevator_no];
    while(1){
        usleep(1000);
        /* 
        if(selected_elevator->floor >= N_FLOORS && is_elevator_dir_up(selected_elevator)){
            selected_elevator->elevator_state = ELEVATOR_IDLE;
        }
        if(selected_elevator->floor <= 0 && is_elevator_dir_down(selected_elevator)){
            selected_elevator->elevator_state = ELEVATOR_IDLE;
        } */

        if(selected_elevator->alive == false){
            printf("Elevator %d is dead\n", elevator_no);
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
            if(selected_elevator->floor == -1) continue;

            //TODO: If any more requests in current direction, continue moving, else set to IDLE
            if(at_valid_floor_request(elevator_status, elevator_no, order_queue)){
                stop_elevator(elevator_status, elevator_no);
                open_elevator_door(elevator_status, elevator_no);
                complete_floor_request(elevator_status->floor, elevator_no, elevator_status, order_queue);
                if(is_any_valid_request_in_current_elevator_direction(elevator_status, elevator_no, order_queue)){
                    start_elevator(elevator_status, elevator_no);
                }else{
                    set_elevator_state(elevator_status, elevator_no, ELEVATOR_IDLE);
                }

                
            }

            break;
        default:
            break;
        }
    }
    return NULL;
}

int set_order_status(order_queue_t* order_queue, int index, order_status_t order_status)
{
    pthread_mutex_lock(order_queue->queue_mutex);
    order_queue->orders[index].order_status = order_status;
    #if LOG_LEVEL >= LOG_LEVEL_INFO
    printf("INFO: Order %ld status set to %d\n", order_queue->orders[index].order_id, order_status);
    #endif
    pthread_mutex_unlock(order_queue->queue_mutex);
    return -1;
}

void* request_handler(void* arg){
    elevator_arg_t* elevator_arg = (elevator_arg_t*)arg;
    elevator_status_t* elevator_status = elevator_arg->elevator_status;
    order_queue_t* order_queue = elevator_arg->order_queue;
    printf("Request_Handler Started\n");
    while(1){
        usleep(10000);
        if(order_queue->size == 0){
            //printf("Order_Queue_Empty!\n");
            //continue;
        }

        for (int i = 0; i < order_queue->size; i++){
            order_event_t slected_order = order_queue->orders[i];
            switch(slected_order.order_status){
                case RECIVED:
                    continue; //Ignore unsynched orders
                    //printf("RECIVED");
                case SYNCED:
                    //printf("SYNCHED\n");
                    if(slected_order.order_type == GO_TO){
                        int elevator_no = get_elevator_no_from_order(elevator_status, &slected_order);
                        if(is_elevator_moving_towards_request(elevator_status, elevator_no, slected_order.floor)){
                            set_order_status(order_queue, i, ACTIVE);
                        }
                        if(elevator_status[elevator_no].elevator_state == ELEVATOR_IDLE){
                            set_elevator_dir_towards_floor(elevator_status, elevator_no, slected_order.floor);
                        }
                    }
                    else{
                        if(is_any_elevator_moving_towards_request(elevator_status, slected_order.floor)){
                            set_order_status(order_queue, i, ACTIVE);
                            //slected_order.order_status = ACTIVE;
                            //TODO: Set order timestamp
                            continue;
                        } 
                        int closest_idle_elevator = get_closest_idle_elevator(elevator_status, slected_order.floor);
                        if(closest_idle_elevator == -1){
                            continue;
                        }else{
                            set_elevator_dir_towards_floor(elevator_status, closest_idle_elevator, slected_order.floor);
                            set_order_status(order_queue, i, ACTIVE);
                            //slected_order.order_status = ACTIVE;
                        }
                    }
                    break;
                case ACTIVE:
                    //TODO: Check order timestamp and set to synched if too old
                    //printf("ACTIVE");
                    break;
                case COMPLETED:
                    printf("COMPLETED");
                    break;
                default:
                    printf("Invalid order status\n");
                    break;
                }
            }       
    }
    return NULL;
}

int elevator_algorithm_init(elevator_status_t* elevator_status, order_queue_t* order_queue)
{
    
    for(int i = 0; i < N_ELEVATORS; i++){
        //Init all variables that are controlled locally
        elevator_status[i].elevator_state = ELEVATOR_IDLE;
        elevator_status[i].door_open = false;

        elevator_arg[i].elevator_no = i;
        elevator_arg[i].elevator_status = elevator_status;
        elevator_arg[i].order_queue = order_queue;
        pthread_create(&elevator_thread[i], NULL, elevator_control, (void*)&elevator_arg[i]);
    }

    pthread_create(&request_handler_thread, NULL, request_handler, (void*)&elevator_arg[0]);
    return 0;
}

int elevator_algorithm_kill()
{
    //pthread_cancel(request_handler_thread);
    for(int i = 0; i < N_ELEVATORS ; i++){
        //pthread_cancel(elevator_thread[i]);
        
    }
    return 0;
}  
