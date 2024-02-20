#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "elevator_button_inputs.h"

#define POLL_TIME 100 // microseconds between polls

pthread_mutex_t elevator_struct_mutex;

struct elevator_floor_button_inputs_t* elevator_struct;

void clear_elevator_struct(){
    for(int floor = 0; floor < N_FLOORS; floor++){
        elevator_struct->floor_inputs[floor].request_up = 0;
        elevator_struct->floor_inputs[floor].request_down = 0;
        elevator_struct->floor_inputs[floor].request_cabin = 0; 
    }
}

void* print_elevator_struct(){
    while(1){
        pthread_mutex_lock(&elevator_struct_mutex);
        printf("Elevator struct:\n");
        for(int floor = 0; floor < N_FLOORS; floor++){
            printf("Floor %d:\t", floor);
            printf("Request up: %d\t", elevator_struct->floor_inputs[floor].request_up);
            printf("Request down: %d\t", elevator_struct->floor_inputs[floor].request_down);
            printf("Request cabin: %d\n", elevator_struct->floor_inputs[floor].request_cabin);
        }
        printf("Cleraring struct\n");
        clear_elevator_struct();
        pthread_mutex_unlock(&elevator_struct_mutex);
        sleep(5);
    }
}

void* poll_elevator_buttons_wrapper(){
    while(1){
        pthread_mutex_lock(&elevator_struct_mutex);
        poll_elevator_floor_buttons_pressed(elevator_struct);
        pthread_mutex_unlock(&elevator_struct_mutex);
        usleep(POLL_TIME);
    }
}

int main(){
    elevator_hardware_init();

    elevator_struct = malloc(sizeof(struct elevator_floor_button_inputs_t));

    clear_elevator_struct();    // Dont need to lock mutex here, since no other thread is running yet

    if(pthread_mutex_init(&elevator_struct_mutex, NULL) != 0){
        printf("Mutex init failed\n");
        exit(1);
    }

    pthread_t thread_button_poller;
    pthread_t thread_print;

    if(pthread_create(&thread_button_poller, NULL, poll_elevator_buttons_wrapper, NULL) != 0){
        printf("Thread creation failed\n");
        exit(1);
    }
    if(pthread_create(&thread_print, NULL, print_elevator_struct, NULL) != 0){
        printf("Thread creation failed\n");
        exit(1);
    }

    if(pthread_join(thread_button_poller, NULL) != 0){
        printf("Thread join failed\n");
        exit(2);
    }
    if(pthread_join(thread_print, NULL) != 0){
        printf("Thread join failed\n");
        exit(2);
    }

}