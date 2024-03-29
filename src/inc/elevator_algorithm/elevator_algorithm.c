#include "elevator_algorithm.h"
/**
 * @brief Functions that are used to control the elevator algorithm
 * 
 */

/**
 * @brief Returns true if there is an order in the elevator path
 * 
 * @param order 
 * @param elevator 
 * @return true 
 * @return false 
 */
bool order_in_elevator_path(order_event_t* order, elevator_status_t* elevator){
    //See if given order is in current path of given elevator
    if(order->order_status == SYNCED){
        if(elevator->elevator_state == UP){
            return (order->floor >= elevator->floor);
        }
        else if(elevator->elevator_state == DOWN){
            return (order->floor <= elevator->floor);
        }
    }
    return 0;
}

/**
 * @brief Function that returns true if the elevator has reserved orders in the queue
 * 
 * @param queue 
 * @param elevator 
 * @return true 
 * @return false 
 */
bool elevator_has_reserved_orders(order_queue_t* queue, elevator_status_t* elevator){
    //See if elevator has any more reserved orders
    for(int i = 0; i < queue->size; i++){
        if(strcmp(queue->orders[i].controller_id, elevator->elevator.ip) == 0){
            return 1;
        }
    }
    return 0;
}


elevator_status_t* get_elevator_by_ip(elevator_status_t* elevator, char* ip){
    for(int i = 0; i < MAX_IP_NODES; i++){
        if(strcmp(elevator[i].elevator.ip, ip) == 0){
            return &elevator[i];
        }
    }
    return NULL;
}

bool elevator_has_cab_orders(order_queue_t* queue, elevator_status_t* elevator){
    for(int i = 0; i < queue->size; i++){
        if(queue->orders[i].order_type == GO_TO){
            if(strcmp(queue->orders[i].elevator_id, elevator->elevator.ip) == 0){
                return 1;
            }
        }
    }
    return 0;
}

/**
 * @brief Returns true if there is a reserved order at the current elevator position
 * 
 * @param queue 
 * @param elevator 
 * @return true 
 * @return false 
 */
bool reserved_order_at_position(order_queue_t* queue, elevator_status_t* elevator){
    for(int i = 0; i < queue->size; i++){
        if(strcmp(queue->orders[i].controller_id, elevator->elevator.ip) == 0){
            if(queue->orders[i].floor == elevator->floor){
                return 1;
            }
        }
    }
    return 0;
}

order_event_t* return_reserved_order_at_floor(order_queue_t* queue,elevator_status_t* elevator, int floor){
    for(int i = 0; i < queue->size; i++){
        if(strcmp(queue->orders[i].controller_id, elevator->elevator.ip) == 0){
            if(queue->orders[i].floor == floor){
                return &queue->orders[i];
            }
        }
    }
    return NULL;
}

/**
 * @brief Finds the closest synced order to the elevator
 * 
 * @param queue 
 * @param elevator 
 * @return long 
 */
long return_closest_order(order_queue_t* queue, elevator_status_t* elevator){
    // find closest order to elevator
    int closest_order_id = -1;
    long closest_order_distance = 1000;
    for(int i = 0; i < queue->size; i++){
        if(queue->orders[i].order_status == SYNCED){
            int temp = queue->orders[i].floor - elevator->floor;
            if(abs(temp) < closest_order_distance){
                closest_order_distance = abs(temp);
                closest_order_id = queue->orders[i].order_id;
            }
        }
    }
    return closest_order_id;
}

order_event_t* return_oldes_order(order_queue_t* queue, elevator_status_t* elevator){
    // find oldest order in queue
    for(int i = 0; i < queue->size; i++){
        if(queue->orders[i].order_type != GO_TO){
            if(queue->orders[i].order_status == SYNCED){
                return &queue->orders[i];
            }
        }
        else{
            if(strcmp(queue->orders[i].elevator_id, elevator->elevator.ip) == 0){
                if(queue->orders[i].order_status == SYNCED){
                    return &queue->orders[i];
                }
            }
        }
    }
    return NULL;
}

/**
 * @brief Finds the direction an elevator has to move to reach an order
 * 
 * @param order 
 * @param elevator 
 * @return elevator_state_t 
 */
elevator_state_t direction_to_order(order_event_t* order, elevator_status_t* elevator){
    // find direction to order
    if(order->floor > elevator->floor){
        return UP;
    }
    else if(order->floor < elevator->floor){
        return DOWN;
    }
    else{
        return STOP;
    }
}

/**
 * @brief Function that reserves an elevator for an order
 * 
 * @param queue 
 * @param order 
 * @param elevator 
 */
void reserve_elevator(order_queue_t* queue, order_event_t* order, elevator_status_t* elevator){
    if(order->order_type != GO_TO){
        pthread_mutex_lock(queue->queue_mutex);
        order->order_status = ACTIVE;
        strcpy(order->controller_id, elevator->elevator.ip);
        pthread_mutex_unlock(queue->queue_mutex);
    } 
    else{
        if(strcmp(order->elevator_id, elevator->elevator.ip) == 0){
            pthread_mutex_lock(queue->queue_mutex);
            order->order_status = ACTIVE;
            strcpy(order->controller_id, elevator->elevator.ip);
            pthread_mutex_unlock(queue->queue_mutex);
        }
    }
}

void order_completion_timedout(order_queue_t* queue){
    // check if any orders have timed out
    time_t current_time;
    time(&current_time);
    for(int i = 0; i < queue->size; i++){
        if(queue->orders[i].order_status == ACTIVE){
            if(difftime(current_time, queue->orders[i].timestamp) > ORDER_TIMEOUT){
                pthread_mutex_lock(queue->queue_mutex);
                queue->orders[i].order_status = SYNCED;
                strcpy(queue->orders[i].controller_id, "");
                queue->orders[i].timestamp = current_time;
                pthread_mutex_unlock(queue->queue_mutex);
            }
        }
    }
}

void unreserve_elevators_orders(order_queue_t* queue, elevator_status_t* elevator){
    // remove reserved order from queue
    for(int i = 0; i < queue->size; i++){
        if(strcmp(queue->orders[i].controller_id, elevator->elevator.ip) == 0){
            if(strcmp(queue->orders[i].elevator_id, "") != 0){
                pthread_mutex_lock(queue->queue_mutex);
                time_t current_time;
                time(&current_time);
                queue->orders[i].timestamp = current_time;
                queue->orders[i].order_status = SYNCED;
                strcpy(queue->orders[i].controller_id, "");
                pthread_mutex_unlock(queue->queue_mutex);
            }
        }
    }
}

void set_at_floor(order_queue_t* queue, elevator_status_t* elevator){
    // remove completed order from queue
    for(int i = 0; i < queue->size; i++){
        if(strcmp(queue->orders[i].controller_id, elevator->elevator.ip) == 0){
            if(queue->orders[i].order_status == ACTIVE){
                if(elevator->floor == queue->orders[i].floor && elevator->at_floor){
                    time_t current_time;
                    time(&current_time);
                    pthread_mutex_lock(queue->queue_mutex);
                    queue->orders[i].order_status = AT_FLOOR;
                    queue->orders[i].timestamp = current_time;
                    pthread_mutex_unlock(queue->queue_mutex);
                    pthread_mutex_lock(&elevator->mutex);
                    elevator->door_open = true;
                    pthread_mutex_unlock(&elevator->mutex);
                }
            }
        }
    }
}


void set_completed_order(order_queue_t* queue, elevator_status_t* elevator){
    for(int i = 0; i < queue->size; i++){
        if(strcmp(queue->orders[i].controller_id, elevator->elevator.ip) == 0){
            if(queue->orders[i].order_status == AT_FLOOR){
                time_t current_time;
                time(&current_time);
                if((current_time - queue->orders[i].timestamp) > DOOR_OPEN_TIME){
                    if(!elevator->obstruction){
                        printf("Order %ld set to completed\n", queue->orders[i].order_id);
                        pthread_mutex_lock(queue->queue_mutex);
                        queue->orders[i].order_status = COMPLETED;
                        pthread_mutex_unlock(queue->queue_mutex);
                        pthread_mutex_lock(&elevator->mutex);
                        elevator->door_open = false;
                        pthread_mutex_unlock(&elevator->mutex);
                    }
                }
            }
        }
        
    }
}

void remove_completed_order(order_queue_t* queue, elevator_status_t* elevator){
    // remove completed order from queue
    for(int i = 0; i < queue->size; i++){
        if(queue->orders[i].order_status == COMPLETED){
            pthread_mutex_lock(queue->queue_mutex);
            dequeue_order(queue, &queue->orders[i]);
            pthread_mutex_unlock(queue->queue_mutex);
        }
    }
    
}

void remove_passed_orders(order_queue_t* queue, elevator_status_t* elevator){
    for(int i = 0; i < queue->size; i++){
        if(strcmp(queue->orders[i].controller_id, elevator->elevator.ip) == 0){
            if(elevator->elevator_state == UP || elevator->elevator_state == TRANSPORT_UP){
                if(queue->orders[i].floor < elevator->floor){
                    pthread_mutex_lock(queue->queue_mutex);
                    queue->orders[i].order_status = SYNCED;
                    strcpy(queue->orders[i].controller_id, "");
                    pthread_mutex_unlock(queue->queue_mutex);
                }
            }
            if(elevator->elevator_state == DOWN || elevator->elevator_state == TRANSPORT_DOWN){
                if(queue->orders[i].floor > elevator->floor){
                    pthread_mutex_lock(queue->queue_mutex);
                    queue->orders[i].order_status = SYNCED;
                    strcpy(queue->orders[i].controller_id, "");
                    pthread_mutex_unlock(queue->queue_mutex);
                }
            }
        }
    }
}

void* thr_handle_orders(void* args){
    printf("Starting thread to handle orders\n");
    elevator_arg_t* elevator_arg = (elevator_arg_t*)args;
    elevator_status_t* elevator = elevator_arg->elevator;
    order_queue_t* queue = elevator_arg->queue;

    while(1){
        usleep(ORDER_POLL_DELAY);
        order_completion_timedout(queue);

        for(int i = 0; i < MAX_IP_NODES; i++){
            if(!elevator[i].alive){
                unreserve_elevators_orders(queue, &elevator[i]);
                continue;
            }

            if(elevator[i].emergency_stop){
                unreserve_elevators_orders(queue, &elevator[i]);
                elevator[i].elevator_state = STOP;
                continue;
            }

            switch (elevator[i].elevator_state)
            {
            case STOP: ;
                order_event_t* oldest_order = return_oldes_order(queue, &elevator[i]);
                if(oldest_order == NULL){
                    break;
                }
                pthread_mutex_lock(&elevator[i].mutex);
                elevator[i].door_open = false;
                pthread_mutex_unlock(&elevator[i].mutex);
                
                reserve_elevator(queue, oldest_order, &elevator[i]);
                elevator_state_t direction = direction_to_order(oldest_order, &elevator[i]);
                pthread_mutex_lock(&elevator[i].mutex);
                if(direction == UP){
                    if(oldest_order->order_type == GO_TO){
                        elevator[i].elevator_state = UP;
                    }
                    else{
                        elevator[i].elevator_state = TRANSPORT_UP;
                    }
                }
                else if(direction == DOWN){
                    if(oldest_order->order_type == GO_TO){
                        elevator[i].elevator_state = DOWN;
                    }
                    else{
                        elevator[i].elevator_state = TRANSPORT_DOWN;
                    }
                }
                pthread_mutex_unlock(&elevator[i].mutex);

                break;
            
            case UP: ;
                if(!elevator_has_reserved_orders(queue, &elevator[i])){
                    pthread_mutex_lock(&elevator[i].mutex);
                    elevator[i].elevator_state = STOP;
                    pthread_mutex_unlock(&elevator[i].mutex);
                }

                for(int j = 0; j < queue->size; j++){
                    if(order_in_elevator_path(&queue->orders[j], &elevator[i])){
                        reserve_elevator(queue, &queue->orders[j], &elevator[i]);
                    }
                }

                break;

            case DOWN: ;
                if(!elevator_has_reserved_orders(queue, &elevator[i])){
                    pthread_mutex_lock(&elevator[i].mutex);
                    elevator[i].elevator_state = STOP;
                    pthread_mutex_unlock(&elevator[i].mutex);
                }

                for(int j = 0; j < queue->size; j++){
                    if(order_in_elevator_path(&queue->orders[j], &elevator[i])){
                        reserve_elevator(queue, &queue->orders[j], &elevator[i]);
                    }
                }

                break;

            case TRANSPORT_UP: ;
                if(!elevator_has_reserved_orders(queue, &elevator[i])){
                    pthread_mutex_lock(&elevator[i].mutex);
                    elevator[i].elevator_state = STOP;
                    pthread_mutex_unlock(&elevator[i].mutex);
                }

                if(reserved_order_at_position(queue, &elevator[i])){
                    order_event_t* reserved_order = return_reserved_order_at_floor(queue, &elevator[i], elevator[i].floor);
                    if(reserved_order != NULL){
                        if(reserved_order->order_type == UP_FROM){
                            pthread_mutex_lock(&elevator[i].mutex);
                            elevator[i].elevator_state = UP;
                            pthread_mutex_unlock(&elevator[i].mutex);
                        }
                        else if(reserved_order->order_type == DOWN_FROM){
                            pthread_mutex_lock(&elevator[i].mutex);
                            elevator[i].elevator_state = DOWN;
                            pthread_mutex_unlock(&elevator[i].mutex);
                        }
                        else{
                            pthread_mutex_lock(&elevator[i].mutex);
                            elevator[i].elevator_state = STOP;
                            pthread_mutex_unlock(&elevator[i].mutex);
                        }
                    }
                }
                break;

            case TRANSPORT_DOWN: ;
                if(!elevator_has_reserved_orders(queue, &elevator[i])){
                    pthread_mutex_lock(&elevator[i].mutex);
                    elevator[i].elevator_state = STOP;
                    pthread_mutex_unlock(&elevator[i].mutex);
                }

                if(reserved_order_at_position(queue, &elevator[i])){
                    order_event_t* reserved_order = return_reserved_order_at_floor(queue, &elevator[i], elevator[i].floor);
                    if(reserved_order != NULL){
                        if(reserved_order->order_type == UP_FROM){
                            pthread_mutex_lock(&elevator[i].mutex);
                            elevator[i].elevator_state = UP;
                            pthread_mutex_unlock(&elevator[i].mutex);
                        }
                        else if(reserved_order->order_type == DOWN_FROM){
                            pthread_mutex_lock(&elevator[i].mutex);
                            elevator[i].elevator_state = DOWN;
                            pthread_mutex_unlock(&elevator[i].mutex);
                        }
                        else{
                            pthread_mutex_lock(&elevator[i].mutex);
                            elevator[i].elevator_state = STOP;
                            pthread_mutex_unlock(&elevator[i].mutex);
                        }
                    }
                }
                break;
            
            default:
                break;
            }
            remove_passed_orders(queue, &elevator[i]);
            set_at_floor(queue, &elevator[i]);
            set_completed_order(queue, &elevator[i]);
            remove_completed_order(queue, &elevator[i]);   
            if(!elevator_has_reserved_orders(queue, &elevator[i])){
                pthread_mutex_lock(&elevator[i].mutex);
                elevator[i].elevator_state = STOP;
                pthread_mutex_unlock(&elevator[i].mutex);
            }
        }
    }
}
