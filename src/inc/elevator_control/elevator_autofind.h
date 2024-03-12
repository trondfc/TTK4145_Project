#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "../types.h"
#include "../keep_alive/keep_alive.h"
#include "../elevator_hardware/elevator_hardware.h"
#include "../config.h"

#define MAX_ELEVATORS MAX_IP_NODES


elevator_status_t* elevator_struct_init();
int compare_ips(char* a, char* b);
void elevator_init_ip(elevator_status_t* elevator, char* ip);
void elevator_init(elevator_status_t* elevator);