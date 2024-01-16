#include <stdio.h>
#include <stdlib.h>
#include "sverresnetwork.h"

int main(){
  char * ip = getMyIpAddress("enp0s31f6");
  printf("My Ip is %s\n",ip);

  free(ip);

  return 0;
}

