#include "../inc/keep_alive/keep_alive.h"

int main(){
   char* ip = get_host_ip();
    printf("System IP Address is: %s\n", ip);

    keep_alive_init(5000, SLAVE);
    while(1){
        sleep(1);
        print_alive_nodes(get_alive_node_list());
    }
  return 0;
}


