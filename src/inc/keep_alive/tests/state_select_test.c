#include<unistd.h>
#include<stdio.h>
#include"../keep_alive.h"

int main()
{
    int port = 4000;
    node_mode_t type = SLAVE;

    printf("Starting keep alive\n");
    keep_alive_init(port, type);

    keep_alive_node_list_t* list = get_node_list();
    update_node_list(list, "192.168.0.1", "SLAVE", 6);
    update_node_list(list, "192.168.0.2", "SLAVE", 6);
    update_node_list(list, "192.168.0.3", "SLAVE", 6);

    sleep(10);
    printf("Killing keep alive\n");
    keep_alive_kill();
    return 0;
}