#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sverresnetwork.h"


// LAB udp server ip: 10.100.23.129
char * server_ip = "10.100.23.129";

void udpmessageReceived(const char * ip, char * data, int datalength){

  // Assuming an ascii string here - a binary blob (including '0's) will
  // be ugly/truncated.
  printf("Received UDP message from %s: '%s'\n",ip,data);
  if(strcomp(data,"ping") == 0){
    sleep(1000);
    udp_send(server_ip, 20014,"pong",5);
  }
  else if (strcomp(data,"pong") == 0)
  {
    sleep(1000);
    udp_send(server_ip,20014,"ping",5);
  }
  
}

int main(int argc, char * argv[]){
  char * ip = getMyIpAddress("enp0s31f6");
  printf("My Ip is %s\n",ip);
  free(ip);
  if(argc >= 2){
    server_ip = argv[1];
  }

  udp_startReceiving(20014,udpmessageReceived);
  sleep(100); // wait for recieve to start
  if(argc >= 3){
    if(strcomp(argv[1],"start") == 0){
      printf("Starting ping-pong");
      udp_send(server_ip,20014,"ping",5);
    }
  }


  return 0;
}

