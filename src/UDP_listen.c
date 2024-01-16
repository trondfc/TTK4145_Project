#include <stdio.h>
#include <unistd.h>
#include "sverresnetwork.h"

void 
udpmessageReceived(const char * ip, char * data, int datalength){

  // Assuming an ascii string here - a binary blob (including '0's) will
  // be ugly/truncated.
  printf("Received UDP message from %s: '%s'\n",ip,data);
  
}

int main(){

  udp_startReceiving(30000,udpmessageReceived);
    // LAB udp server ip: 10.100.23.129
  sleep(100);
  return 0;
}
