#include<pthread.h>

#include "elevator_algorithm.h"
#include "../config.h"




int main(){
    elevator_system_t elevator_system;
    
    pthread_mutex_t lock;
    if (pthread_mutex_init(&lock, NULL) != 0){
        printf("\n mutex init failed \n");
        exit(1);
    } 


    
    pthread_t state_machine_id;
    int error;

    error = pthread_create(&state_machine_id, NULL, &elevator_algorithm, NULL);
    if (error != 0){
        printf("\n pthread cant be created \n");
        exit(1);    
    }

    






    return 0;
}