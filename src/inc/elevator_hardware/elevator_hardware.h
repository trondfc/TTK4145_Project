/*
Rewrite of the original elevator hardware module from the TTK4145 Real Time Programming course at NTNU
    Rewritten for posibility of multiple elevators
    Uses a struct to store the socket file descriptor and mutex
    Uses a config file to read the ip and port of the elevator

*/
#pragma once

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <string.h>


#include "con_load.h"

#define N_FLOORS 4
#define N_BUTTONS 3

typedef enum tag_elevator_hardware_motor_direction {
    DIRN_DOWN = -1,
    DIRN_STOP = 0,
    DIRN_UP = 1
} elevator_hardware_motor_direction_t;

typedef enum tag_elevator_hardware_lamp_type {
    BUTTON_CALL_UP = 0,
    BUTTON_CALL_DOWN = 1,
    BUTTON_COMMAND = 2
} elevator_hardware_button_type_t;

typedef struct elevator_hardware_info_t
{
    char ip[16];
    char port[8];
    int sockfd;
    pthread_mutex_t sockmtx;

} elevator_hardware_info_t;

void elevator_hardware_read_config(const char* ip_name, const char* port_name, elevator_hardware_info_t* hardware);

int elevator_hardware_init(elevator_hardware_info_t* hardware);
void elevator_hardware_destroy(elevator_hardware_info_t* hardware);

void elevator_hardware_set_motor_direction(elevator_hardware_motor_direction_t dirn, elevator_hardware_info_t* hardware);
int elevator_hardware_set_button_lamp(elevator_hardware_button_type_t button, int floor, int value, elevator_hardware_info_t* hardware);
void elevator_hardware_set_floor_indicator(int floor, elevator_hardware_info_t* hardware);
void elevator_hardware_set_door_open_lamp(int value, elevator_hardware_info_t* hardware);
void elevator_hardware_set_stop_lamp(int value, elevator_hardware_info_t* hardware);

int elevator_hardware_get_button_signal(elevator_hardware_button_type_t button, int floor, elevator_hardware_info_t* hardware);
int elevator_hardware_get_floor_sensor_signal(elevator_hardware_info_t* hardware);
int elevator_hardware_get_stop_signal(elevator_hardware_info_t* hardware);
int elevator_hardware_get_obstruction_signal(elevator_hardware_info_t* hardware);