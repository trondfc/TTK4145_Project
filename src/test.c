#include <stdio.h>
#include <string.h>

int main(int argc, char * argv[]){
    if(argc >= 2){
        if(!strcmp(argv[1], "start")){
            printf("Got start");
        } else{
            printf("failed, got %s",argv[1]);
        }
    }

    return 0;
}