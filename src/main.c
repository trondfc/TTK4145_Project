#include <stdio.h>
#include <stdlib.h>
#include "sverresnetwork.h"

void udpmessageReceived(const char * ip, char * data, int datalength){

  // Assuming an ascii string here - a binary blob (including '0's) will
  // be ugly/truncated.
  printf("Received UDP message from %s: '%s'\n",ip,data);
}

/*void udpmessageReceived(const char * ip, int port, char * data, int datalength){

  printf("Received UDP message from %s:%d: '%s'\n",ip,port,data);
}  */

int main(){
  char * ip = getMyIpAddress("enp0s31f6");
  printf("My Ip is %s\n",ip);


  udp_startReceiving(30000,udpmessageReceived);
    // LAB udp server ip: 10.100.23.129
  char * server_ip = "10.100.23.129";

  udp_send(server_ip,20014,"Hello World",12);

  free(ip);

  return 0;
}

