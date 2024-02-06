/*
Rewrite of the original elevator hardware module from the TTK4145 Real Time Programming course at NTNU
    Rewritten for posibility of multiple elevators
    Uses a struct to store the socket file descriptor and mutex
    Uses a config file to read the ip and port of the elevator

*/
#include <unistd.h>
#include "elevator_hardware.h"

void* elevatorControll(void* arg) {
	elevator_hardware_info_t* hardware = (elevator_hardware_info_t*) arg;
	while(1) {
		elevator_hardware_set_motor_direction(DIRN_DOWN, hardware);
		while(elevator_hardware_get_floor_sensor_signal(hardware) != 0) {}
		elevator_hardware_set_motor_direction(DIRN_UP, hardware);
		while(elevator_hardware_get_floor_sensor_signal(hardware) != 3) {}
	}
}

int main() {
	elevator_hardware_info_t elevator_1;
	elevator_hardware_info_t elevator_2;

	elevator_hardware_read_config("elv1_ip", "elv1_port", &elevator_1);
	elevator_hardware_read_config("elv2_ip", "elv2_port", &elevator_2);

    elevator_hardware_init(&elevator_1);
	elevator_hardware_init(&elevator_2);


	while(0) {
		sleep(1);
		int elv1_floor = elevator_hardware_get_floor_sensor_signal(&elevator_1);
		int elv2_floor = elevator_hardware_get_floor_sensor_signal(&elevator_2);
		if(elv1_floor != -1) {
			elevator_hardware_set_floor_indicator(elv1_floor, &elevator_2);
		}
		if(elv2_floor != -1) {
			elevator_hardware_set_floor_indicator(elv2_floor, &elevator_1);
		}
		printf("Elevator 1: %d\t Elevator 2: %d\n", elv1_floor, elv2_floor);
	}

	for(int i = 0; i < 4; i++) {
		elevator_hardware_set_floor_indicator(i, &elevator_1);
		elevator_hardware_set_floor_indicator((3-i), &elevator_2);
		sleep(5);
	}

	pthread_t elevator1;
	pthread_t elevator2;

	pthread_create(&elevator1, NULL, elevatorControll, &elevator_1);
	pthread_create(&elevator2, NULL, elevatorControll, &elevator_2);

	pthread_join(elevator1, NULL);
	pthread_join(elevator2, NULL);
}
