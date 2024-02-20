#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

#include "elevator_algorithm.h"

void* elevator_algorithm(void* arg){

}

bool is_requests_empty(elevator_system_t *elevator_system){}


bool is_cab_requests_empty(elevator_system_t* elevator_system){
    bool request_exists = false;
    for(int elevator = 0; elevator < N_ELEVATORS; elevator++){
        for(int floor = 0; floor < N_FLOORS; floor++){ 
            request_exists |= elevator_system->cab_buttons.elevator[elevator].cab_floor_request[floor];
        }
    }
}


bool is_floor_buttons_request_empty(elevator_system_t* elevator_system)
{
    bool request_exists = false;
    for(int floor = 0; floor < N_FLOORS; floor++){

        request_exists |= elevator_system->floor_buttons.floor[floor].request_up;
        request_exists |= elevator_system->floor_buttons.floor[floor].request_down;
    }
}

bool is_request_at_floor(){

}






/*
Generates outputs based on inputs

struct elevator
    state (idle, moving_up, moving_down)
    current_floor
    motor_ctrl (up, down, none)

struct requests


struct outputs



struct system
    elevator[num_of_elevators]
    requests




event_handler (event):
    switch event
        case NEW_FLOOR:
            set_current_floor() 
            set_floor_light()
            if is_at_requested_floor()
                stop()
                open_doors()
                pop_request(floor)
                next()

        case FLOOR_REQUEST:
            update_requests(event)            

        case STOP:
            E_STOP()

        case DEFAULT:
            pass



algorithm(state)

    switch elevator.state
    case idle
        if is_request_empty() 
            return 

        set_next_floor()
        switch next_floor

            case > current_floor
                state = moving_up

            case < current_floor
                state = moving_down

            default:
                open_door
        
    default:
        if (!is_requests_in_dir())
            state = idle
            return
        set_next_floor()
        
case moving_down
    if (!is_requests_in_dir())
        state = idle
        return



if state_change
    output = algorithm(input)




door_logic(elevator)
    open door
    wait
    while door is obstructed:
        wait
    close
    return

update_input_state(event)




while 1 {
    if new_event:
        event_handler(event)
        action = algorithm(state)
        broadcast(state)
    else
        sleep
}

events:
    NEW_FLOOR: (0, 1, 2, 3, NULL)
    FLOOR_REQUEST: [floor][type]
    STOP
    OBSTRUCTION

OUTPUTS:
    lights floor.
*/



