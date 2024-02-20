// Description: Elevator light control module.
#include "elevator_light_control.h"

void update_cab_floor_button_lights(elevator_multiple_lights_t elevator_lights, elevator_ID_t elevator_ID) {
    for(uint8_t i = 0; i < NO_FLOORS; i++) {
        elevator_hardware_set_button_lamp(BUTTON_COMMAND, i, elevator_lights.elevator[elevator_ID].floor_button[i]);
    }
}

void update_cab_stop_light(elevator_multiple_lights_t elevator_lights, elevator_ID_t elevator_ID) {
    elevator_hardware_set_stop_lamp(elevator_lights.elevator[elevator_ID].stop_button);
}

void update_cab_door_light(elevator_multiple_lights_t elevator_lights, elevator_ID_t elevator_ID) {
    elevator_hardware_set_door_open_lamp(elevator_lights.elevator[elevator_ID].door_open);
}

void update_cab_up_lights(elevator_multiple_lights_t elevator_lights, elevator_ID_t elevator_ID) {
    for(uint8_t i = 0; i < NO_FLOORS; i++) {
        elevator_hardware_set_button_lamp(BUTTON_CALL_UP, i, elevator_lights.elevator[elevator_ID].up_button[i]);
    }
}

void update_cab_down_lights(elevator_multiple_lights_t elevator_lights, elevator_ID_t elevator_ID) {
    for(uint8_t i = 0; i < NO_FLOORS; i++) {
        elevator_hardware_set_button_lamp(BUTTON_CALL_DOWN, i, elevator_lights.elevator[elevator_ID].down_button[i]);
    }
}

void update_cab_floor_light(elevator_multiple_lights_t elevator_lights, elevator_ID_t elevator_ID) {
    for(uint8_t i = 0; i < NO_FLOORS; i++) {
        elevator_hardware_set_floor_indicator(i, elevator_lights.elevator[elevator_ID].cab_floor[i]);
    }
}