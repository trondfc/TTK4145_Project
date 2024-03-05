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
    list->nodes[i].state = KA_ACTIVE;
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
            list->nodes[i].state = KA_ACTIVE;
            if (strcmp(list->nodes[i].data, "MASTER") != 0)
            {
                list->nodes[i].type = MASTER;
            }else if (strcmp(list->nodes[i].data, "SLAVE") != 0)
            {
                list->nodes[i].type = SLAVE;
            }
            else{
                list->nodes[i].type = UNDEFINED;
            }
            strcpy(list->nodes[i].data, data);
            return 0;
        }

        else if(list->nodes[i].is_occupied == EMPTY)
        {
            node_list_add(list, ip, i, data, datalength);
            list->nodes[i].is_occupied = OCCUPIED;
            
            return 0;
        }
    }
    return -1;
    printf("Node list error\n");
}

void udpmessageReceived(const char * ip, char * data, int datalength){
  // Assuming an ascii string here - a binary blob (including '0's) will
  // be ugly/truncated.
    printf("Received UDP message from %s: '%s'\n",ip,data); 
    //pthread_mutex_lock(&keep_alive_config.nodes->mutex);  
    //update_node_list(keep_alive_config.nodes, ip, data, datalength);
    //pthread_mutex_unlock(&keep_alive_config.nodes->mutex);  

    
    if (strcmp(ip, keep_alive_config.self_ip_address) != 0)
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
            if (config->nodes->nodes[i].state == KA_ACTIVE)
            {
                uint64_t current_time = get_timestamp();
                if (current_time - config->nodes->nodes[i].last_seen_timestamp > config->timeout_us)
                {
                    config->nodes->nodes[i].state = KA_INACTIVE;
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
        //printf("Sent keep alive message\n");
        //printf("Keep alive message type: %d\n", config->msg.type);
        //printf("Keep alive message: %s\n", config->msg.data);
        //printf("--------------------------------\n");
        usleep(config->interval_us);
    }
    return NULL;
}

int print_alive_nodes(keep_alive_node_list_t* list){
    int active_nodes = 0;
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++)
    {
        if(list->nodes[i].state == KA_ACTIVE)
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
    

    keep_alive_config.nodes = (keep_alive_node_list_t*)malloc(sizeof(keep_alive_node_list_t));
    keep_alive_config.self_ip_address = get_host_ip();
    set_keep_alive_config_state(&keep_alive_config, type);
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

keep_alive_node_list_t* get_alive_node_list(keep_alive_config_t* conf)
{
    return conf->nodes;
}

int set_keep_alive_config_state(keep_alive_config_t* config, keep_alive_type_t type){
    pthread_mutex_lock(&config->nodes->mutex);
    config->msg.type = type;
    pthread_mutex_unlock(&config->nodes->mutex);

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

int count_alive_nodes(keep_alive_config_t* conf, keep_alive_node_count_t* node_count){
    node_count->alive_master_count = 0;
    node_count->alive_slave_count = 0;
    node_count->alive_node_count = 0;
    
    pthread_mutex_lock(&conf->nodes->mutex);
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++)
    {
        if(conf->nodes->nodes[i].state == KA_ACTIVE)
        {
            node_count->alive_node_count++;
            if(conf->nodes->nodes[i].type == SLAVE)
            {
                node_count->alive_slave_count++;
            }
            else if(conf->nodes->nodes[i].type == MASTER)
            {
                node_count->alive_master_count++;
            }
        }
    }
    pthread_mutex_unlock(&conf->nodes->mutex);
    return 0;   
}

int print_node_count(keep_alive_node_count_t* node_count)
{
    printf("Total active nodes: %d\n", node_count->alive_node_count);
    printf("Total active slaves: %d\n", node_count->alive_slave_count);
    printf("Total active masters: %d\n", node_count->alive_master_count);
    return 0;
}


keep_alive_node_count_t* count_alive_init(keep_alive_config_t* conf)
{
    keep_alive_node_count_t* node_count = (keep_alive_node_count_t*)malloc(sizeof(keep_alive_node_count_t));
    return node_count;
}


int count_alive_kill(keep_alive_node_count_t* node_count)
{
    free(node_count);
    return 0;
}

int is_no_master(keep_alive_node_count_t* node_count)
{
    if (node_count->alive_master_count == 0)
    {
        return 1;
    }
    return 0;
}

uint64_t ip_to_int(const char* ip)
{
    return 0;
}


int is_host_highest_priority(keep_alive_config_t* conf)
{
    int highest_priority = 1;
    for (int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++)
    {
        if (conf->nodes->nodes[i].state == KA_ACTIVE)
        {
            if (strcmp(conf->nodes->nodes[i].ip, conf->self_ip_address) <= 0)
            {
                highest_priority = 0;
            }
        }
    }
    return highest_priority;
}

 int host_state_select_logic(keep_alive_config_t* conf)
{
    keep_alive_config_t* config = conf;
    keep_alive_node_count_t* node_count = count_alive_init(config);
    keep_alive_type_t state = config->msg.type;

    if (node_count->alive_node_count == 0)
    {
        state = RESET;
    }

    if (is_no_master(node_count))
    {
        if(is_host_highest_priority(conf)){
            state = MASTER;
        }
        else
        {
            state = SLAVE;
        }
    }
    count_alive_kill(node_count);
    config->msg.type = state;
    return 0;
}

keep_alive_type_t get_host_state(keep_alive_config_t* conf)
{
    return conf->msg.type;
}

keep_alive_config_t* keep_alive_start()
{
    int port = 4000;
    int timeout_us = 4000000;
    int interval_us = 1000000;
    keep_alive_type_t type = SLAVE;
    keep_alive_config_t* keep_alive = keep_alive_init(port, type, timeout_us, interval_us);
    return keep_alive;
}