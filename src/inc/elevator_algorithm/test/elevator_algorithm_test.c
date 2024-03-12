#include<pthread.h>
#include<stdlib.h>

#include "../elevator_algorithm.h"
#include "../../config.h"


int main(){
    elevator_status_init(NULL);
    printf("main\n");
    elevator_status_kill();
}