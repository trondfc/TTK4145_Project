#include "elevator_button_inputs.h"

struct elevator_button_inputs_t poll_elevator_buttons_pressed(int floor){
    struct elevator_button_inputs_t button_inputs;
    button_inputs.request_up = elevator_hardware_get_button_signal(BUTTON_CALL_UP, floor);
    button_inputs.request_down = elevator_hardware_get_button_signal(BUTTON_CALL_DOWN, floor);
    button_inputs.request_cabin = elevator_hardware_get_button_signal(BUTTON_COMMAND, floor);
    return button_inputs;
}

void poll_elevator_floor_buttons_pressed(struct elevator_floor_button_inputs_t* floor_button_inputs){
    for(int floor = 0; floor < N_FLOORS; floor++){
        struct elevator_button_inputs_t floor_inputs = poll_elevator_buttons_pressed(floor);
        floor_button_inputs->floor_inputs[floor].request_up |= floor_inputs.request_up;
        floor_button_inputs->floor_inputs[floor].request_down |= floor_inputs.request_down;
        floor_button_inputs->floor_inputs[floor].request_cabin |= floor_inputs.request_cabin;
    }
}

