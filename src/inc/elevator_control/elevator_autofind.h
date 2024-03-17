/**
 * @file elevator_autofind.c
 * @brief functions for finding and initializing elevators
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "../keep_alive/keep_alive.h"
#include "../elevator_hardware/elevator_hardware.h"
#include "../elevator_control/elevator_control.h"
#include "../config.h"

#define MAX_ELEVATORS MAX_IP_NODES // Maximum number of elevators in the system

// Function prototypes
elevator_status_t* elevator_struct_init();
int compare_ips(char* a, char* b);
void elevator_init_ip(elevator_status_t* elevator, char* ip);
void elevator_init(elevator_status_t* elevator);