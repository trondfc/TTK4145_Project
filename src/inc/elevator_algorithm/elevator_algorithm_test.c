#include<pthread.h>
#include<stdlib.h>

#include "elevator_algorithm.h"
#include "../config.h"


int main(){
    elevator_system_t elevator_system;
    elevator_system.all_elevator_status = (all_elevator_status_t*)malloc(sizeof(all_elevator_status_t));
    elevator_system.order_que           = (order_que_t*) malloc(sizeof(order_que_t));


    pthread_mutex_t lock;
    if (pthread_mutex_init(&lock, NULL) != 0){
        printf("\n mutex init failed \n");
        exit(1);
    } 
    
    pthread_t state_machine_id;
    int error;




    error = pthread_create(&state_machine_id, NULL, &elevator_algorithm, &elevator_system);
    if (error != 0){
        printf("\n pthread cant be created \n");
        exit(1);    
    }

    






    return 0;
}