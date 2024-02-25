#pragma once 

#include<sys/time.h> 
#include<stdint.h>   
#include<pthread.h>
#define KEEP_ALIVE_NODE_AMOUNT 10

typedef enum{
    SLAVE = 0,
    MASTER = 1
} keep_alive_type_t;

typedef struct{
    keep_alive_type_t type;
    char * data;
} keep_alive_msg_t;

typedef enum{
    INACTIVE = 0,
    ACTIVE = 1
} keep_alive_node_state_t;

typedef enum{
    EMPTY = 0,
    OCCUPIED = 1
} node_occupied_flag_t;

typedef struct{
    char ip[255];
    keep_alive_type_t type;
    keep_alive_node_state_t state;
    node_occupied_flag_t is_occupied;
    uint64_t last_seen_timestamp;
} keep_alive_node_t;

typedef struct{
    keep_alive_node_t nodes[KEEP_ALIVE_NODE_AMOUNT];
    pthread_mutex_t mutex;
} keep_alive_node_list_t;

typedef struct{
    int port;
    int timeout_us;
    char* self_ip_address;
    keep_alive_type_t node_type;
    keep_alive_node_list_t* nodes;
} keep_alive_config_t;



void* keep_alive_recv(void* arg);
void* keep_alive_send(void* arg);
int keep_alive_init(int port, keep_alive_type_t type, int timeout_us);
keep_alive_node_list_t* get_alive_node_list();
int print_alive_nodes(keep_alive_node_list_t* list);