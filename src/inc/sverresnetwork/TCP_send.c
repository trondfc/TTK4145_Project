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
  //char ip[255] = "10.24.37.195";
  char ip[255] = "127.0.0.1";
  char msg[255] = "Hello!";
  int port = 9000;

  tcp_init(messageReceived,connectionStatus);

  tcp_openConnection(ip,port);

  tcp_send(ip, msg, sizeof(msg));

  return 0;
}
