#ifndef ELEVATOR_BUTTON_INPUTS_H
#define ELEVATOR_BUTTON_INPUTS_H

#include <stdbool.h>
#include "../elevator_hardware/elevator_hardware.h"

#define N_FLOORS 4

struct elevator_button_inputs_t
{
    bool request_up;
    bool request_down;
    bool request_cabin;
};

struct elevator_floor_button_inputs_t
{
    struct elevator_button_inputs_t floor_inputs[N_FLOORS];
};


struct elevator_button_inputs_t poll_elevator_buttons_pressed(int floor);
void poll_elevator_floor_buttons_pressed(struct elevator_floor_button_inputs_t* floor_button_inputs);
#endif // ELEVATOR_BUTTON_INPUTS_H