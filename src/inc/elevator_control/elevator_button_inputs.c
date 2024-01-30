#include "elevator_button_inputs.h"

/**
 * @brief polls the hardware for button presses on a given floor
 *        using the elevator_hardware module
 * 
 * @param floor floor to poll for button presses
 * @return struct elevator_button_inputs_t 
 */
struct elevator_button_inputs_t poll_elevator_buttons_pressed(int floor){
    struct elevator_button_inputs_t button_inputs;
    button_inputs.request_up = elevator_hardware_get_button_signal(BUTTON_CALL_UP, floor);
    button_inputs.request_down = elevator_hardware_get_button_signal(BUTTON_CALL_DOWN, floor);
    button_inputs.request_cabin = elevator_hardware_get_button_signal(BUTTON_COMMAND, floor);
    return button_inputs;
}

/**
 * @brief pulls all the buttons on all floors and stores them in a struct
 *        N_FLOORS(macro) from headder file
 *        
 * 
 * @param floor_button_inputs pointer to struct to store button inputs in
 */
void poll_elevator_floor_buttons_pressed(struct elevator_floor_button_inputs_t* floor_button_inputs){
    for(int floor = 0; floor < N_FLOORS; floor++){
        struct elevator_button_inputs_t floor_inputs = poll_elevator_buttons_pressed(floor);
        floor_button_inputs->floor_inputs[floor].request_up |= floor_inputs.request_up;
        floor_button_inputs->floor_inputs[floor].request_down |= floor_inputs.request_down;
        floor_button_inputs->floor_inputs[floor].request_cabin |= floor_inputs.request_cabin;
    }
}

