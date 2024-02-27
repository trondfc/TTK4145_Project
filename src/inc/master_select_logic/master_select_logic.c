#include "master_select_logic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../keep_alive/keep_alive.h"

typedef struct {
    int alive_node_count;
    int alive_slave_count;
    int alive_master_count;
}keep_alive_node_count_t;


int keep_alive_update()
{
    keep_alive_node_count_t* node_count = (keep_alive_node_count_t*)malloc(sizeof(keep_alive_node_count_t));
    count_alive_nodes(node_count);
    print_node_count(node_count);
    free(node_count);
    return 0;
}


int count_alive_nodes(keep_alive_node_count_t* node_count)
{
    keep_alive_node_list_t* list = get_alive_node_list();
    int count = 0;
    int slave_count = 0;
    int master_count = 0;
    for (int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++)
    {
        if (list->nodes[i].state == ACTIVE)
        {
            count++;
            if (list->nodes[i].type == SLAVE)
            {
                slave_count++;
            }
            else if (list->nodes[i].type == MASTER)
            {
                master_count++;
            }
        }
    }
    node_count->alive_node_count = count;
    node_count->alive_slave_count = slave_count;
    node_count->alive_master_count = master_count;
    return 0;
}

int print_node_count(keep_alive_node_count_t* node_count)
{
    printf("Total active nodes: %d\n", node_count->alive_node_count);
    printf("Total active slaves: %d\n", node_count->alive_slave_count);
    printf("Total active masters: %d\n", node_count->alive_master_count);
    return 0;
}






