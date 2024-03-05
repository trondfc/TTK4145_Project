#pragma once 

#include<sys/time.h> 
#include<stdint.h>   
#include<pthread.h>
#define KEEP_ALIVE_NODE_AMOUNT 10
#define MESSAGE_SIZE 10*sizeof(char)

typedef enum{
    UNDEFINED = 0,
    SLAVE = 1,
    MASTER = 2,
    RESET = 3
} keep_alive_type_t;

typedef struct{
    keep_alive_type_t type;
    char data[MESSAGE_SIZE];
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
    char data[MESSAGE_SIZE];
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
    int interval_us;
    char* self_ip_address;
    keep_alive_node_list_t* nodes;
    pthread_t recv_thread;
    pthread_t send_thread;
    pthread_t timeout_thread;
    keep_alive_msg_t msg;
} keep_alive_config_t;

typedef struct{
    int alive_node_count;
    int alive_slave_count;
    int alive_master_count;
} keep_alive_node_count_t;


typedef struct{
    keep_alive_type_t host_state;
    keep_alive_node_list_t  *node_list;
    pthread_mutex_t mutex;
}host_config_t;


void* keep_alive_recv(void* arg);
void* keep_alive_send(void* arg);
keep_alive_config_t* keep_alive_init(int port, keep_alive_type_t type, int keep_alive_timeout_us, int keep_alive_ping_interval_us);
keep_alive_node_list_t* get_alive_node_list();
int print_alive_nodes(keep_alive_node_list_t* list);
int set_keep_alive_config_state(keep_alive_config_t* config, keep_alive_type_t type);
int keep_alive_kill(keep_alive_config_t* config);


keep_alive_node_count_t* count_alive_init(keep_alive_config_t* conf);
int count_alive_nodes(keep_alive_config_t* conf, keep_alive_node_count_t* node_count);
int print_node_count(keep_alive_node_count_t* node_count);
int count_alive_kill(keep_alive_node_count_t* node_count);
int is_host_highest_priority(keep_alive_config_t* conf);
keep_alive_config_t* keep_alive_start();
keep_alive_type_t get_host_state(keep_alive_config_t* conf);

