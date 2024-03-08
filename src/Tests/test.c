#include "../inc/keep_alive/keep_alive.h"

int main(){
   char* ip = get_host_ip();
    printf("System IP Address is: %s\n", ip);

    keep_alive_init(5000, MASTER);
  return 0;
}


