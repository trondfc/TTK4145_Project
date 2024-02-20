#include <stdio.h>
#include <unistd.h>
#include "sverresnetwork.h"


    // LAB udp server ip: 10.100.23.129

void 
messageReceived(const char * ip, char * data, int datalength){

  printf("Received message from %s: '%s'\n",ip,data);
}

void 
connectionStatus(const char * ip, int status){

  printf("A connection got updated %s: %d\n",ip,status);
}



int main(){

  tcp_init(messageReceived,connectionStatus);

  tcp_openConnection("10.100.23.129",33546);

  tcp_send("10.100.23.129","Hello!",7);

  return 0;
}