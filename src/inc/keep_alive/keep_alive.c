#include "keep_alive.h"

keep_alive_node_list_t keep_alive_node_list;

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
    return ip_address;
}

uint64_t get_timestamp(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
}

void* keep_alive_send(void* arg){
    keep_alive_node_list_t* list = (keep_alive_node_list_t*)arg;
    while(1){
        udp_broadcast(list->self->port, list->self->data, sizeof(list->self->data));
        usleep(BRODCAST_INTERVAL_US);
    }
}

int update_node_list(keep_alive_node_list_t* list, const char* ip, char* data, int data_size){
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
        if(strcmp(list->nodes[i].ip, ip) == 0){
            list->nodes[i].last_time = get_timestamp();
            list->nodes[i].status = ALIVE;
            if(strcmp(list->nodes[i].data, "MASTER") != 0){
                list->nodes[i].node_mode = MASTER;
            } else if(strcmp(list->nodes[i].data, "SLAVE") != 0){
                list->nodes[i].node_mode = SLAVE;
            } else {
                list->nodes[i].node_mode = UNDEFINED;
            }
            strcpy(list->nodes[i].data, data);
            return 0;
        }
    }
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
        if(list->nodes[i].status == DEAD){
            list->nodes[i].status = ALIVE;
            list->nodes[i].last_time = get_timestamp();
            strcpy(list->nodes[i].ip, ip);
            strcpy(list->nodes[i].data, data);
            if(strcmp(list->nodes[i].data, "MASTER") != 0){
                list->nodes[i].node_mode = MASTER;
            } else if(strcmp(list->nodes[i].data, "SLAVE") != 0){
                list->nodes[i].node_mode = SLAVE;
            } else {
                list->nodes[i].node_mode = UNDEFINED;
            }
            return 0;
        }
    }
    printf("Node list error\n");
    return -1;
}

void udp_receive_callback(const char* ip, char* data, int data_size){
    printf("Received data from %s \t %s\n", ip,data);

    if(strcmp(ip, keep_alive_node_list.self->ip) != 0){
        pthread_mutex_lock(keep_alive_node_list.mutex);
        update_node_list(&keep_alive_node_list, ip, data, data_size);
        pthread_mutex_unlock(keep_alive_node_list.mutex);

    }

}

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

    keep_alive_node_list.nodes = (keep_alive_node_t*)malloc(KEEP_ALIVE_NODE_AMOUNT * sizeof(keep_alive_node_t));
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
        keep_alive_node_list.nodes[i].status = DEAD;
    }

    pthread_t send_thread;
    pthread_create(&send_thread, NULL, keep_alive_send, (void*)&keep_alive_node_list);
    udp_startReceiving(port, udp_receive_callback);

}

void keep_alive_kill(){
    free(keep_alive_node_list.nodes);
    free(keep_alive_node_list.self);
    free(keep_alive_node_list.mutex);
}

keep_alive_node_list_t* get_alive_node_list(){
    return &keep_alive_node_list;
}

void print_alive_nodes(keep_alive_node_list_t* list){
    printf("Alive nodes:\n");
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
        if(list->nodes[i].status == ALIVE){
            printf("IP: %s \t Mode: %s\n", list->nodes[i].ip, list->nodes[i].data);
        }
    }
}

int is_host_highest_priority(keep_alive_node_list_t* list){
    for(int i = 0; i < KEEP_ALIVE_NODE_AMOUNT; i++){
        if(list->nodes[i].status == ALIVE){
            if(strcmp(list->nodes[i].ip, list->self->ip) <= 0){
                return 0;
            }
        }
    }
    return 1;
}

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
    list->node_count_alive = count;
    list->node_count_slave = slave_count;
    list->node_count_master = master_count;
}

void*