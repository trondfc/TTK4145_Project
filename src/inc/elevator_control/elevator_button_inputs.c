#include "elevator_button_inputs.h"

elevator_button_inputs_t poll_elevator_buttons_pressed(int floor){
    elevator_button_inputs_t button_inputs;
    button_inputs.request_up |= elevator_hardware_get_button_signal(BUTTON_CALL_UP, floor);
    button_inputs.request_down |= elevator_hardware_get_button_signal(BUTTON_CALL_DOWN, floor);
    button_inputs.request_cabin |= elevator_hardware_get_button_signal(BUTTON_COMMAND, floor);
    return button_inputs;
}

void poll_elevator_floor_buttons_pressed(elevator_floor_button_inputs_t* floor_button_inputs){
    for(int floor = 0; floor < N_FLOORS; floor++){
        floor_button_inputs->floor_inputs[floor] = poll_elevator_buttons_pressed(floor);
    }
}

