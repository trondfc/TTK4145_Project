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
    keep_alive_node_list_t* list = get_alive_node_list(keep_alive_config_ptr);
    print_alive_nodes(list);
    
    keep_alive_node_count_t* node_count = count_alive_init(keep_alive_config_ptr);
    count_alive_nodes(keep_alive_config_ptr, node_count);
    print_node_count(node_count); 

    





    int count = 0;
    while (count < 10)
    {
        //usleep(3000000);
        count++;
        sleep(3);

        printf("Printing alive nodes:\n");  
        count_alive_nodes(keep_alive_config_ptr, node_count);
        print_node_count(node_count); 
        printf("--------------------------------\n");


        printf("Priority:%d \n", is_host_highest_priority(keep_alive_config_ptr));
        printf("--------------------------------\n");

    }
    count_alive_kill(node_count);
    keep_alive_kill(keep_alive_config_ptr);
    return 0;
}