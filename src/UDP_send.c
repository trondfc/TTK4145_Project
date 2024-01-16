#include <stdio.h>
#include <unistd.h>
#include "sverresnetwork.h"

void 
udpmessageReceived(const char * ip, int port, char * data, int datalength){

  printf("Received UDP message from %s:%d: '%s'\n",ip,port,data);
  
}

int main(){
    // LAB udp server ip: 10.100.23.129
  char * ip = "10.100.23.129";

  udp_send(ip,20014,"Hello World",12);

  return 0;
}
