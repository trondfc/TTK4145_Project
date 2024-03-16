/**
 * @file keep_alive.c
 * @brief Functions to keep track of nodes in the network.
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "keep_alive.h"

/**
 * @brief List of nodes in the network.
 * 
 */
keep_alive_node_list_t keep_alive_node_list;

/**
 * @brief Get the host ip object
 * 
 * @return char* 
 */
char* get_host_ip(){
    
    FILE *fp;
    int size = 60*sizeof(char);
    char* ip_address = (char*)malloc(size);
    //char ip_address[100];
    fp = popen("hostname -I | grep -E -o \"([0-9]{1,3}\\.){3}[0-9]{1,3}\"", "r");
    fgets(ip_address, size, fp);
    //printf("System IP Address is: %s\n", ip_address);
    pclose(fp);
    for (int i = 0; i < size; i++){
        if (ip_address[i] == '\n'){
            ip_address[i] = '\0';
            break;
        }
    }
    //printf("System IP Address is: %s\n", ip_address);
    return ip_address;
}

/**
 * @brief Get the timestamp object
 * 
 * @return uint64_t 
 */
uint64_t get_timestamp(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
}

/**
 * @brief Send keep alive message to all nodes in the network.
 * 
 * @param arg 
 * @return void* 
 */
void* keep_alive_send(void* arg){
    keep_alive_node_list_t* list = (keep_alive_node_list_t*)arg;
    while(1){
        if(list->single_master == false){
            //printf("Sending keep alive, mode: %d\n", list->self->node_mode);
            udp_broadcast(list->self->port, list->self->data, sizeof(list->self->data));
        }
        usleep(BRODCAST_INTERVAL_US);
    }
}

/**
 * @brief Update the node list with new data.
 * 
 * @param list 
 * @param ip 
 * @param data 
 * @param data_size 
 * @return int 
 */
int update_node_list(keep_alive_node_list_t* list, const char* ip, char* data, int data_size){
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
        if(strcmp(list->nodes[i].ip, ip) == 0){
            list->nodes[i].last_time = get_timestamp();
            list->nodes[i].status = ALIVE;
            if(strcmp(list->nodes[i].data, "MASTER") == 0){
                //printf("Setting node %s to MASTER\n", list->nodes[i].ip);
                list->nodes[i].node_mode = MASTER;
            } else if(strcmp(list->nodes[i].data, "SLAVE") == 0){
                //printf("Setting node %s to SLAVE\n", list->nodes[i].ip);
                list->nodes[i].node_mode = SLAVE;
            } else {
                //printf("Setting node %s to UNDEFINED\n", list->nodes[i].ip);
                list->nodes[i].node_mode = UNDEFINED;
            }
            strncpy(list->nodes[i].data, data, sizeof(list->nodes[i].data) - 1);
            return 0;
        }
    }
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
        if(list->nodes[i].status == DEAD){
            list->nodes[i].status = ALIVE;
            list->nodes[i].last_time = get_timestamp();
            strcpy(list->nodes[i].ip, ip);
            strcpy(list->nodes[i].data, data);
            if(strcmp(list->nodes[i].data, "MASTER") == 0){
                //printf("New node. Setting node %s to MASTER\n", list->nodes[i].ip);
                list->nodes[i].node_mode = MASTER;
            } else if(strcmp(list->nodes[i].data, "SLAVE") == 0){
                //printf("New node. Setting node %s to SLAVE\n", list->nodes[i].ip);
                list->nodes[i].node_mode = SLAVE;
            } else {
                //printf("New node. Setting node %s to UNDEFINED\n", list->nodes[i].ip);
                list->nodes[i].node_mode = UNDEFINED;
            }
            return 0;
        }
    }
    printf("Node list error\n");
    return -1;
}

/**
 * @brief Callback function for receiving keep alive messages.
 * 
 * @param ip 
 * @param data 
 * @param data_size 
 */
void udp_receive_callback(const char* ip, char* data, int data_size){
    //printf("Received data from %s \t %s\n", ip,data);
    if(keep_alive_node_list.single_master == true){
        return;
    }

    if(strcmp(ip, keep_alive_node_list.self->ip) != 0){
        pthread_mutex_lock(keep_alive_node_list.mutex);
        update_node_list(&keep_alive_node_list, ip, data, data_size);
        pthread_mutex_unlock(keep_alive_node_list.mutex);
    }

}

/**
 * @brief Initialize keep alive module.
 * 
 * @param port 
 * @param mode 
 */
void keep_alive_init(int port, node_mode_t mode){
    keep_alive_node_list.mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(keep_alive_node_list.mutex, NULL);

    keep_alive_node_list.node_count_alive = 0;
    keep_alive_node_list.node_count_master = 0;
    keep_alive_node_list.node_count_slave = 0;

    keep_alive_node_list.self = (keep_alive_node_t*)malloc(sizeof(keep_alive_node_t));
    strcpy(keep_alive_node_list.self->ip, get_host_ip());
    keep_alive_node_list.self->port = port;
    keep_alive_node_list.self->node_mode = mode;
    strcpy(keep_alive_node_list.self->data, mode == MASTER ? "MASTER" : "SLAVE");
    keep_alive_node_list.single_master = false;

    keep_alive_node_list.nodes = (keep_alive_node_t*)malloc(KEEP_ALIVE_NODE_AMOUNT * sizeof(keep_alive_node_t));
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
        keep_alive_node_list.nodes[i].status = DEAD;
        keep_alive_node_list.nodes[i].node_mode = UNDEFINED;
        strcpy(keep_alive_node_list.nodes[i].ip, "\0");
    }

    pthread_t send_thread;
    pthread_t update_thread;
    pthread_t timeout_thread;

    pthread_create(&send_thread, NULL, keep_alive_send, (void*)&keep_alive_node_list);
    pthread_create(&update_thread, NULL, keep_alive_update, (void*)&keep_alive_node_list);
    pthread_create(&timeout_thread, NULL, keep_alive_timeout, (void*)&keep_alive_node_list);

    udp_startReceiving(port, udp_receive_callback);

}

/**
 * @brief Free memory used by keep alive module.
 * 
 */
void keep_alive_kill(){
    free(keep_alive_node_list.nodes);
    free(keep_alive_node_list.self);
    free(keep_alive_node_list.mutex);
}

/**
 * @brief Wrapper to get the node list object
 * 
 * @return keep_alive_node_list_t* 
 */
keep_alive_node_list_t* get_node_list(){
    return &keep_alive_node_list;
}

/**
 * @brief Print the alive nodes in the network.
 * 
 * @param list 
 */
void print_alive_nodes(keep_alive_node_list_t* list){
    printf("Alive nodes:\n");
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
        if(list->nodes[i].status == ALIVE){
            printf("IP: %s \t Mode: (%d) %s\n", list->nodes[i].ip, list->nodes[i].node_mode,list->nodes[i].data);
        }
    }
}

/**
 * @brief Return the ip as a long.
 * 
 * @param ip 
 * @return long 
 */
long ipv4_to_int(char* ip)
{
    char delim[2] = ".";
    char* token;
    char* ip_copy = (char*)malloc(MAX_IP_LEN + 1);
    strcpy(ip_copy, ip);

    char* formated_ipv4 = (char*)calloc(sizeof(char), MAX_IP_LEN);
    token = strtok(ip_copy, delim);
    while(token != NULL){
        //printf("%s\n", token);
        char segment[4] = "000";
        strncpy(segment + strlen(segment) - strlen(token), token, strlen(token));
        //strncat(formated_ipv4, segment, sizeof(formated_ipv4)-strlen(formated_ipv4)-1);
        strncat(formated_ipv4, segment, MAX_IP_LEN-strlen(formated_ipv4)-1);
        token = strtok(NULL, delim);
    }

    printf("formated ip: %s\n", formated_ipv4);
    long ip_int = atol(formated_ipv4);

    free(ip_copy);
    free(formated_ipv4);

    return ip_int;
}

/**
 * @brief check if the host is the highest priority node in the network.
 * 
 * @param list 
 * @return int 
 */
int is_host_highest_priority(keep_alive_node_list_t* list){
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
        if(list->nodes[i].status == ALIVE){
            //if(strcmp(list->self->ip, list->nodes[i].ip) <= 0){
            if(ipv4_to_int(list->self->ip) < ipv4_to_int(list->nodes[i].ip)){
                return 0;
            }
        }
    }
    return 1;
}

/**
 * @brief update the node count in the node list.
 * 
 * @param list 
 */
void update_node_count(keep_alive_node_list_t* list){
    int count = 0;
    int slave_count = 0;
    int master_count = 0;
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
        if(list->nodes[i].status == ALIVE){
            count++;
            if(list->nodes[i].node_mode == SLAVE){
                slave_count++;
            } else if(list->nodes[i].node_mode == MASTER){
                master_count++;
            }
        }
    }
    //printf("Alive nodes: %d\n", count);
    //printf("Alive slaves: %d\n", slave_count);
    //printf("Alive masters: %d\n", master_count);
    list->node_count_alive = count;
    list->node_count_slave = slave_count;
    list->node_count_master = master_count;
}

/**
 * @brief Update host mode.
 * 
 * @param arg 
 * @return void* 
 */
void* keep_alive_update(void* arg){
    keep_alive_node_list_t* list = (keep_alive_node_list_t*)arg;
    sleep(1);
    uint64_t last_contact_timestamp = get_timestamp();
    while(1){
        pthread_mutex_lock(list->mutex);
        update_node_count(list);
        if(list->node_count_master == 0){
            if(list->self->node_mode == SLAVE){
                if(is_host_highest_priority(list)){
                    list->self->node_mode = MASTER;
                    strcpy(list->self->data, "MASTER");
                    udp_broadcast(list->self->port, list->self->data, sizeof(list->self->data));
                }
            }
        } else if (list->node_count_master >= 1 && list->self->node_mode == MASTER){
            if(list->self->node_mode == MASTER){
                list->self->node_mode = SLAVE;
                strcpy(list->self->data, "SLAVE");
                udp_broadcast(list->self->port, list->self->data, sizeof(list->self->data));
            }
        }

        
        if(list->node_count_alive == 0){
            if((get_timestamp() - last_contact_timestamp) > NO_ACTIVE_NODES_TIMEOUT_US){
                if(list->single_master == false){
                    printf("No active nodes for %d us. Setting single master\n", NO_ACTIVE_NODES_TIMEOUT_US);
                    list->single_master = true;
                }
                //exit(1);
            }
        }else{
            last_contact_timestamp = get_timestamp();
        }

        pthread_mutex_unlock(list->mutex);
        usleep(1000000);
    }
}

/**
 * @brief Keep alive timeout function.
 * 
 * @param arg 
 * @return void* 
 */
void* keep_alive_timeout(void* arg){
    keep_alive_node_list_t* list = (keep_alive_node_list_t*)arg;
    while(1){
        pthread_mutex_lock(list->mutex);
        for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
            if(list->nodes[i].status == ALIVE){
                if(get_timestamp() - list->nodes[i].last_time > KEEP_ALIVE_TIMEOUT_US){
                    printf("Node %s is dead\n", list->nodes[i].ip);
                    send_order_queue_close_connection(list->nodes[i].ip);
                    close_elevator_hardware(list->nodes[i].ip);
                    list->nodes[i].status = DEAD;
                    list->nodes[i].node_mode = UNDEFINED;
                    strcpy(list->nodes[i].data, "");
                    strcpy(list->nodes[i].ip, "");
                }
            }
        }
        pthread_mutex_unlock(list->mutex);
        usleep(SEC_TO_US(1));
    }
}