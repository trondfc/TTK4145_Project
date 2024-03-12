#ifndef ELERATOR_AUTOFIND_H
#define ELERATOR_AUTOFIND_H
/* LIBS */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../types.h"
#include "../keep_alive/keep_alive.h"
#include "../elevator_hardware/elevator_hardware.h"

/* DEFINES */
#define MAX_ELEVATORS 10

/* PROTOTYPES */

elevator_status_t* elevator_struct_init();
int compare_ips(char* a, char* b);
void elevator_init_ip(elevator_status_t* elevator, char* ip);
void elevator_init(elevator_status_t* elevator);



#endif // ELERATOR_AUTOFIND_H