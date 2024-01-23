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
    elevator_button_inputs_t floor_inputs[N_FLOORS];
};


elevator_button_inputs_t poll_elevator_buttons_pressed(int floor);
elevator_floor_button_inputs_t poll_elevator_floor_buttons_pressed(void);
#endif // ELEVATOR_BUTTON_INPUTS_H