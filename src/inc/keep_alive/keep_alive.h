#pragma once

#include <sys/time.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../sverresnetwork/sverresnetwork.h"
#include "../config.h"

/// Convert seconds to microseconds
#define SEC_TO_US(sec) ((sec)*1000000)
/// Convert milliseconds to microseconds
#define MS_TO_US(ms)    ((ms)*1000)
/// Convert nanoseconds to microseconds
#define NS_TO_US(ns)    ((ns)/1000)

#define KEEP_ALIVE_NODE_AMOUNT MAX_IP_NODES
#define MAX_IP_LEN 16

#define KEEP_ALIVE_TIMEOUT_US SEC_TO_US(2) 
#define NO_ACTIVE_NODES_TIMEOUT_US  SEC_TO_US(5)
#define MESSAGE_SIZE 10 * sizeof(char)
#define BRODCAST_INTERVAL_US MS_TO_US(500)


typedef enum{
    SLAVE = 0,
    MASTER = 1,
    UNDEFINED = 2
}node_mode_t;

typedef enum{
    ALIVE = 0,
    DEAD = 1
}node_status_t;

typedef enum{
    DISCONNECTED = 0,
    CONNECTED = 1
}node_connection_t;

typedef struct{
    char ip[16];
    int port;
    char data[MESSAGE_SIZE];
    uint64_t last_time;
    node_mode_t node_mode;
    node_status_t status;
    node_connection_t connection;
}keep_alive_node_t;

typedef struct{
    pthread_mutex_t* mutex;
    int node_count_alive;
    int node_count_master;
    int node_count_slave;
    keep_alive_node_t* self;
    keep_alive_node_t* nodes;
}keep_alive_node_list_t;

//PUBLIC
void keep_alive_init(int port, node_mode_t mode);
void keep_alive_kill();
keep_alive_node_list_t* get_node_list();

//PRIVATE
char* get_host_ip();
uint64_t get_timestamp();
void* keep_alive_send(void* arg);
int update_node_list(keep_alive_node_list_t* list, const char* ip, char* data, int data_size);
void udp_receive_callback(const char* ip, char* data, int data_size);

void print_alive_nodes(keep_alive_node_list_t* list);
int is_host_highest_priority(keep_alive_node_list_t* list);
void update_node_count(keep_alive_node_list_t* list);
void* keep_alive_update(void* arg);
void* keep_alive_timeout(void* arg);
long ipv4_to_int(char* ip);