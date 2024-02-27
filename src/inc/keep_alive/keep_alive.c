#define _POSIX_C_SOURCE 199309L
#include <time.h>      
/// Convert seconds to microseconds
#define SEC_TO_US(sec) ((sec)*1000000)
/// Convert nanoseconds to microseconds
#define NS_TO_US(ns)    ((ns)/1000)

#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdint.h>

#include "../sverresnetwork/sverresnetwork.h"
#include "keep_alive.h" 

keep_alive_config_t keep_alive_config;


char* get_host_ip(){
    
    FILE *fp;
    int size = 100*sizeof(char);
    char* ip_address = (char*)malloc(size);
    //char ip_address[100];
    fp = popen("hostname -I | grep -E -o \"([0-9]{1,3}\\.){3}[0-9]{1,3}\"", "r");
    fgets(ip_address, size, fp);
    //printf("System IP Address is: %s\n", ip_address);
    pclose(fp);
    for (int i = 0; i < size; i++)
    {
        if (ip_address[i] == '\n')
        {
            ip_address[i] = '\0';
            break;
        }
    }
    return ip_address;
}

uint64_t get_timestamp(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
}

int update_node_timestamp(keep_alive_node_t* node){
    node->last_seen_timestamp = get_timestamp();
    return 0;
}


int node_list_add(keep_alive_node_list_t* list,const char* ip, int i, char* data, int datalength){
    list->nodes[i].state = ACTIVE;
    strcpy(list->nodes[i].ip, ip);
    strcpy(list->nodes[i].data, data);
    update_node_timestamp(&list->nodes[i]);
    printf("Added node %s to list\n", list->nodes[i].ip);
    return 0;
}


int update_node_list(keep_alive_node_list_t* list, const char* ip, char* data, int datalength){
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++)
    {
        if(strcmp(list->nodes[i].ip, ip) == 0)
        {
            update_node_timestamp(&list->nodes[i]);
            list->nodes[i].state = ACTIVE;
            strcpy(list->nodes[i].data, data);
            return 0;
        }

        else if(list->nodes[i].is_occupied == EMPTY)
        {
            node_list_add(list, ip, i, data, datalength);
            return 0;
        }
    }
    return -1;
    printf("Node list error\n");
}

void udpmessageReceived(const char * ip, char * data, int datalength){
  // Assuming an ascii string here - a binary blob (including '0's) will
  // be ugly/truncated.
    //printf("Received UDP message from %s: '%s'\n",ip,data); 
    if (strcmp(ip, keep_alive_config.self_ip_address) == 0)
    {
        return;
    } else
    {
        pthread_mutex_lock(&keep_alive_config.nodes->mutex);  
        update_node_list(keep_alive_config.nodes, ip, data, datalength);
        pthread_mutex_unlock(&keep_alive_config.nodes->mutex);  
    }
}

void* keep_alive_recv(void* arg){
    keep_alive_config_t* config = (keep_alive_config_t*)arg;
    udp_startReceiving(config->port, udpmessageReceived);
    while(1)
    {
        sleep(1);
    }
    return NULL;
}

void* keep_alive_timeout(void* arg){
    keep_alive_config_t* config = (keep_alive_config_t*)arg;
    while(1){
        int err = usleep(1000);
        printf( err != 0 ? "Error in usleep\n" : "");

        pthread_mutex_lock(&config->nodes->mutex);
        for (int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++)
        {
            if (config->nodes->nodes[i].state == ACTIVE)
            {
                uint64_t current_time = get_timestamp();
                if (current_time - config->nodes->nodes[i].last_seen_timestamp > config->timeout_us)
                {
                    config->nodes->nodes[i].state = INACTIVE;
                    printf("Node %s set to inactive\n", config->nodes->nodes[i].ip);
                }
            }
        }
        pthread_mutex_unlock(&config->nodes->mutex);
    }
    return NULL;
} 

void* keep_alive_send(void* arg){
    keep_alive_config_t* config = (keep_alive_config_t*)arg;
    /*keep_alive_msg_t msg;
    //msg.type = config->node_type;
    //msg.data = (char*)malloc(sizeof(char)*10);

    if (config->node_type == SLAVE)
    {
        strcpy(msg.data, "SLAVE");
    }
    else if (config->node_type == MASTER)
    {
        strcpy(msg.data, "MASTER");
    }
    else
    {
        printf("Invalid node type\n");
        return NULL;
    }
    */

    while(1)
    {
        udp_broadcast(config->port, config->msg.data, sizeof(config->msg.data));
        printf("Sent keep alive message\n");
        printf("Keep alive message type: %d\n", config->msg.type);
        printf("Keep alive message: %s\n", config->msg.data);
        printf("--------------------------------\n");
        usleep(config->interval_us);
    }
    return NULL;
}

int print_alive_nodes(keep_alive_node_list_t* list){
    int active_nodes = 0;
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++)
    {
        if(list->nodes[i].state == ACTIVE)
        {
            printf("Node %s is active\n", list->nodes[i].ip);
            active_nodes++;
        }
    }
    printf("Total active nodes: %d\n", active_nodes);
    return 0;
}

keep_alive_config_t* keep_alive_init(int port, keep_alive_type_t type, int keep_alive_timeout_us, int keep_alive_ping_interval_us){

    keep_alive_config.port = port;
    keep_alive_config.timeout_us = keep_alive_timeout_us;
    keep_alive_config.interval_us = keep_alive_ping_interval_us;
    keep_alive_config.msg.type = type;
    set_keep_alive_config_state(&keep_alive_config, type);

    keep_alive_config.nodes = (keep_alive_node_list_t*)malloc(sizeof(keep_alive_node_list_t));
    keep_alive_config.self_ip_address = get_host_ip();
    printf("Host IP is: %s\n", keep_alive_config.self_ip_address);

    pthread_create(&keep_alive_config.send_thread, NULL, &keep_alive_send, &keep_alive_config);
    pthread_create(&keep_alive_config.recv_thread, NULL, &keep_alive_recv, &keep_alive_config);
    pthread_create(&keep_alive_config.timeout_thread, NULL, &keep_alive_timeout, &keep_alive_config);


    //pthread_join(send, NULL);
    //pthread_join(recv, NULL);
    //pthread_join(timeout_thread, NULL);
    return &keep_alive_config;     
}

int keep_alive_kill(keep_alive_config_t* config){
    pthread_cancel(config->send_thread)       == 0 ? printf("Send thread killed\n")       : printf("Error killing send thread\n");
    pthread_cancel(config->recv_thread)       == 0 ? printf("Recv thread killed\n")       : printf("Error killing recv thread\n");
    pthread_cancel(config->timeout_thread)    == 0 ? printf("Timeout thread killed\n")    : printf("Error killing timeout thread\n");

    free(keep_alive_config.nodes);
    free(keep_alive_config.self_ip_address);

    return 0;
}

keep_alive_node_list_t* get_alive_node_list()
{
    pthread_mutex_lock(&keep_alive_config.nodes->mutex);
    return keep_alive_config.nodes;
    pthread_mutex_unlock(&keep_alive_config.nodes->mutex);
}

int set_keep_alive_config_state(keep_alive_config_t* config, keep_alive_type_t type){
    config->msg.type = type;
    if (type == SLAVE)
    {
        strcpy(config->msg.data, "SLAVE");
        printf("Node type set to SLAVE\n");
    }
    else if (type == MASTER)
    {
        strcpy(config->msg.data, "MASTER");
        printf("Node type set to MASTER\n");
    }
    else
    {
        printf("Invalid node type\n");
        return -1;
    }
    return 0;
}