#include<unistd.h>
#include<stdio.h>
#include"keep_alive.h"

int main()
{
    int port = 4000;
    int timeout_us = 4000000;
    int interval_us = 1000000;
    keep_alive_type_t type = MASTER;
    keep_alive_config_t* keep_alive_config_ptr;

    keep_alive_config_ptr = keep_alive_init(port, type, timeout_us, interval_us);

    

    printf("Printing alive nodes:\n");
    keep_alive_node_list_t* list = get_alive_node_list();
    print_alive_nodes(list);
    

    int count = 0;
    while (count < 10)
    {
        //usleep(3000000);
        sleep(3);
        if(keep_alive_config_ptr->msg.type == MASTER)
        {
            type = SLAVE;
            printf("Master\n");
        }
        else if(keep_alive_config_ptr->msg.type == SLAVE)
        {
            type = MASTER;
            printf("Slave\n");
        }
        else
        {
            printf("Invalid node type\n");
        }
        set_keep_alive_config_state(keep_alive_config_ptr, type);
        count++;

        //printf (err != 0 ? "Error in sleep\n" : "");
        //printf("alive nodes:");
        //print_alive_nodes(list);
    }
    keep_alive_kill(keep_alive_config_ptr);

    
    
    return 0;
}