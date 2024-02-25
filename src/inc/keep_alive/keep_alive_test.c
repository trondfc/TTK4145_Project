#include<unistd.h>

#include"keep_alive.h"

int main()
{
    int port = 4000;
    int timeout_us = 4000000;
    keep_alive_type_t type = SLAVE;

    keep_alive_init(port, type, timeout_us);

    keep_alive_node_list_t* list = get_alive_node_list();
    while (1)
    {
        print_alive_nodes(list);
        sleep(5);
    }
    

    return 0;
}