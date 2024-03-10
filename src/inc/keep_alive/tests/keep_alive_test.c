#include<unistd.h>
#include<stdio.h>
#include"../keep_alive.h"
int main()
{
    int port = 4000;
    node_mode_t type = SLAVE;

    printf("Starting keep alive\n");
    keep_alive_init(port, type);

    sleep(10);
    printf("Killing keep alive\n");
    keep_alive_kill();
    return 0;
}