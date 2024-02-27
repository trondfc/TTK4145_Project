#ifndef ELEVATOR_LIGHT_CONTROL_C
#define ELEVATOR_LIGHT_CONTROL_C

#include "../config.h"
#include "../elevator_hardware/elevator_hardware.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct elevator_single_lights_t
{
    /*
    Legge til Elevator_IP som en del av structen?
    */
	bool floor_button[N_FLOORS];
    bool stop_button;
    bool door_open;
    bool up_button[N_FLOORS];
    bool down_button[N_FLOORS];
    bool cab_floor[N_FLOORS];
}elevator_single_lights_t;

typedef struct elevator_multiple_lights_t
{
	elevator_single_lights_t elevator[N_ELEVATORS];
}elevator_multiple_lights_t;

// example usage
/*
elevator_multiple_lights_t elevator_lights;
elevator_lights.elevator[1].stop_button = true; // turn on stop button light in elevator 1
*/


#endif // ELEVATOR_LIGHT_CONTROL_C