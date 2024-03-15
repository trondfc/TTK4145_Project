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
        else if(order->order_status == DOWN){
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

/**
 * @brief Finds the closest synced order to the elevator
 * 
 * @param queue 
 * @param elevator 
 * @return long 
 */
long find_closest_order(order_queue_t* queue, elevator_status_t* elevator){
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
    pthread_mutex_lock(queue->queue_mutex);
    order->order_status = ACTIVE;
    strcpy(order->controller_id, elevator->elevator.ip);
    pthread_mutex_unlock(queue->queue_mutex);
}

/**
 * @brief Function that sends an elevator to a floor
 * 
 * @param order 
 * @param elevator 
 * @return true 
 * @return false 
 */
bool elevator_goto_floor(order_queue_t* queue, order_event_t* order, elevator_status_t* elevator){
    if(!elevator->alive){
        printf("Elevator %ld is not alive\n", elevator->elevator.ip);
        return 0;
    }
    else if (order->order_type != GO_TO){
        printf("Order %ld is not a go to order\n", order->order_id);
        return 0;
    }
    else if (elevator->door_open || elevator->emergency_stop || elevator->obstruction){
        printf("Elevator %ld is not ready to receive orders. Door open: %d, Emergency stop: %d, Obstruction: %d\n", elevator->elevator.ip, elevator->door_open, elevator->emergency_stop, elevator->obstruction);
        return 0;
    }
    else if (order->floor == elevator->floor){
        printf("Elevator is already at floor %d\n", order->floor);
        return 0;
    }
    else{
        pthread_mutex_lock(&elevator->mutex);
        elevator->elevator_state = direction_to_order(order, elevator);
        elevator->end_floor = order->floor;
        pthread_mutex_unlock(&elevator->mutex);
        reserve_elevator(queue, order, elevator);
        return 1;
    }
}

