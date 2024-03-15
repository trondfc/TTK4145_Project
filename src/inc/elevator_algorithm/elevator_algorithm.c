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
        if(queue->orders[i].order_status == SYNCED){
            return &queue->orders[i];
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
                pthread_mutex_unlock(queue->queue_mutex);
            }
        }
    }
}

void unreserve_elevators_orders(order_queue_t* queue, elevator_status_t* elevator){
    // remove reserved order from queue
    for(int i = 0; i < queue->size; i++){
        if(strcmp(queue->orders[i].controller_id, elevator->elevator.ip) == 0){
            pthread_mutex_lock(queue->queue_mutex);
            queue->orders[i].order_status = SYNCED;
            strcpy(queue->orders[i].controller_id, "");
            pthread_mutex_unlock(queue->queue_mutex);
        }
    }
}

void remove_completed_order(order_queue_t* queue, elevator_status_t* elevator){
    // remove completed order from queue
    for(int i = 0; i < queue->size; i++){
        if(strcmp(queue->orders[i].controller_id, elevator->elevator.ip) == 0){
            if(elevator->floor == queue->orders[i].floor){
                dequeue_order(queue, &queue->orders[i]);
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
                continue;
            } 

            switch (elevator[i].elevator_state)
            {
            case STOP:
                order_event_t* oldest_order = return_oldes_order(queue, &elevator[i]);
                if(oldest_order == NULL){
                    break;
                }
                reserve_elevator(queue, oldest_order, &elevator[i]);
                elevator_state_t direction = direction_to_order(oldest_order, &elevator[i]);
                pthread_mutex_lock(&elevator[i].mutex);
                if(direction == UP){
                    elevator[i].elevator_state = TRANSPORT_UP;
                }
                else if(direction == DOWN){
                    elevator[i].elevator_state = TRANSPORT_DOWN;
                }
                pthread_mutex_unlock(&elevator[i].mutex);

                break;
            
            case UP:
                if(!elevator_has_reserved_orders(queue, &elevator[i])){
                    pthread_mutex_lock(&elevator[i].mutex);
                    elevator[i].elevator_state = STOP;
                    printf("Elevator %s is now stopped\n", elevator[i].elevator.ip);
                    pthread_mutex_unlock(&elevator[i].mutex);
                }

                for(int j = 0; j < queue->size; j++){
                    if(order_in_elevator_path(&queue->orders[j], &elevator[i])){
                        reserve_elevator(queue, &queue->orders[j], &elevator[i]);
                    }
                }

                break;

            case DOWN:
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

            case TRANSPORT_UP:
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

            case TRANSPORT_DOWN:
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

            remove_completed_order(queue, &elevator[i]);   
            if(!elevator_has_reserved_orders(queue, &elevator[i])){
                elevator[i].elevator_state = STOP;
            }
        }
    }
}
