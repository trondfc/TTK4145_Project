/**
 * @file elevator_control.h
 * @brief functions for controlling elevator input and output
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include "../elevator_hardware/elevator_hardware.h"
#include "../keep_alive/keep_alive.h"
#include "../order_queue/orderQueue.h"
#include "../config.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define NUMBER_OF_STOP_READINGS 5 // Number of stop readings before elevator restarts

/*
typedef enum{
  ELEVATOR_IDLE,
  ELEVATOR_DIR_UP_AND_MOVING,
  ELEVATOR_DIR_UP_AND_STOPPED,
  ELEVATOR_DIR_DOWN_AND_MOVING, 
  ELEVATOR_DIR_DOWN_AND_STOPPED,
  ELEVATOR_EMERGENCY_STOP
}elevator_state_t;
*/

typedef enum{
  STOP = 0,
  UP = 1,
  DOWN = 2,
  TRANSPORT_UP = 3,
  TRANSPORT_DOWN = 4,
}elevator_state_t;

/**
 * @brief status data for a single elevator
 * 
 */
typedef struct elevator_status{
  elevator_hardware_info_t elevator;
  pthread_mutex_t mutex;
  bool alive;
  int end_floor;
  int floor;
  bool at_floor;
  elevator_state_t elevator_state;
  bool obstruction;
  bool emergency_stop;
  int number_of_stop_readings;
  bool door_open;
}elevator_status_t;

/**
 * @brief struct for floor buttons
 * 
 */
typedef struct floor_buttons{
  bool floors[NO_FLOORS];
}floor_buttons_t;

/**
 * @brief struct for button lights
 * 
 */
typedef struct button_lights{
  floor_buttons_t *up;
  floor_buttons_t *down;
  floor_buttons_t *cab[MAX_IP_NODES];
  pthread_mutex_t *mutex;
}button_lights_t;

/**
 * @brief struct for button lights history
 * 
 */
typedef struct button_lights_history{
  button_lights_t *old;
  button_lights_t *new;
}button_lights_history_t;


button_lights_history_t* button_light_struct_init();
void poll_stopped_elevators(elevator_status_t* elevator);
void poll_obstructed_elevators(elevator_status_t* elevator);
void poll_elevator_floor(elevator_status_t* elevator);

void update_elevator_floor_lights(elevator_status_t* elevator);
void update_elevator_stop_light(elevator_status_t* elevator);
void update_elevator_door_light(elevator_status_t* elevator);


void add_elevator_button_lights(button_lights_history_t* button_lights, order_queue_t* queue, elevator_status_t* elevator);
void set_changed_button_lights(button_lights_history_t* button_lights, elevator_status_t* elevator);
void sett_all_button_lights(button_lights_history_t* button_lights, elevator_status_t* elevator);