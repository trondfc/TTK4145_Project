#include "elevator_algorithm.h"

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

bool elevator_has_reserved_orders(order_queue_t* queue, elevator_status_t* elevator){
    //See if elevator has any more reserved orders
    for(int i = 0; i < queue->size; i++){
        if(strcmp(queue->orders[i].controller_id, elevator->elevator.ip) == 0){
            return 1;
        }
    }
    return 0;
}

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