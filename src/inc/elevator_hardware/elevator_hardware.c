/*
Rewrite of the original elevator hardware module from the TTK4145 Real Time Programming course at NTNU
    Rewritten for posibility of multiple elevators
    Uses a struct to store the socket file descriptor and mutex
    Uses a config file to read the ip and port of the elevator

*/
#include "elevator_hardware.h"

void elevator_hardware_read_config(const char* ip_name, const char* port_name, elevator_hardware_info_t* hardware){
    con_load("elevator_hardware.con",
        con_val(ip_name,   hardware->ip,   "%s")
        con_val(port_name, hardware->port, "%s")
    )
}


void elevator_hardware_init(elevator_hardware_info_t* hardware){
    pthread_mutex_init(&hardware->sockmtx, NULL);
    
    hardware->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(hardware->sockfd != -1 && "Unable to set up socket");
    
    struct addrinfo hints = {
        .ai_family      = AF_INET, 
        .ai_socktype    = SOCK_STREAM, 
        .ai_protocol    = IPPROTO_TCP,
    };
    struct addrinfo* res;
    getaddrinfo(hardware->ip, hardware->port, &hints, &res);
    
    int fail = connect(hardware->sockfd, res->ai_addr, res->ai_addrlen);
    assert(fail == 0 && "Unable to connect to simulator server");
    
    freeaddrinfo(res);
    
    send(hardware->sockfd, (char[4]) {0}, 4, 0);
}

void elevator_hardware_set_motor_direction(elevator_hardware_motor_direction_t dirn, elevator_hardware_info_t* hardware){
    pthread_mutex_lock(&hardware->sockmtx);
    send(hardware->sockfd, (char[4]) {1, dirn}, 4, 0);
    pthread_mutex_unlock(&hardware->sockmtx);
}

void elevator_hardware_set_button_lamp(elevator_hardware_button_type_t button, int floor, int value, elevator_hardware_info_t* hardware){
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(button >= 0);
    assert(button < N_BUTTONS);

    pthread_mutex_lock(&hardware->sockmtx);
    send(hardware->sockfd, (char[4]) {2, button, floor, value}, 4, 0);
    pthread_mutex_unlock(&hardware->sockmtx);
}

void elevator_hardware_set_floor_indicator(int floor, elevator_hardware_info_t* hardware){
    assert(floor >= 0);
    assert(floor < N_FLOORS);

    pthread_mutex_lock(&hardware->sockmtx);
    send(hardware->sockfd, (char[4]) {3, floor}, 4, 0);
    pthread_mutex_unlock(&hardware->sockmtx);
}

void elevator_hardware_set_door_open_lamp(int value, elevator_hardware_info_t* hardware){
    pthread_mutex_lock(&hardware->sockmtx);
    send(hardware->sockfd, (char[4]) {4, value}, 4, 0);
    pthread_mutex_unlock(&hardware->sockmtx);
}

void elevator_hardware_set_stop_lamp(int value, elevator_hardware_info_t* hardware){
    pthread_mutex_lock(&hardware->sockmtx);
    send(hardware->sockfd, (char[4]) {5, value}, 4, 0);
    pthread_mutex_unlock(&hardware->sockmtx);
}



int elevator_hardware_get_button_signal(elevator_hardware_button_type_t button, int floor, elevator_hardware_info_t* hardware){
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(button >= 0);
    assert(button < N_BUTTONS);

    pthread_mutex_lock(&hardware->sockmtx);
    send(hardware->sockfd, (char[4]) {6, button, floor}, 4, 0);
    char buf[4];
    recv(hardware->sockfd, buf, 4, 0);
    pthread_mutex_unlock(&hardware->sockmtx);
    
    return buf[1];
}

int elevator_hardware_get_floor_sensor_signal(elevator_hardware_info_t* hardware){
    pthread_mutex_lock(&hardware->sockmtx);
    send(hardware->sockfd, (char[4]) {7}, 4, 0);
    char buf[4];
    recv(hardware->sockfd, buf, 4, 0);
    pthread_mutex_unlock(&hardware->sockmtx);
    
    return buf[1] ? buf[2] : -1;
}

int elevator_hardware_get_stop_signal(elevator_hardware_info_t* hardware){
    pthread_mutex_lock(&hardware->sockmtx);
    send(hardware->sockfd, (char[4]) {8}, 4, 0);
    char buf[4];
    recv(hardware->sockfd, buf, 4, 0);
    pthread_mutex_unlock(&hardware->sockmtx);
    
    return buf[1];
}

int elevator_hardware_get_obstruction_signal(elevator_hardware_info_t* hardware){
    pthread_mutex_lock(&hardware->sockmtx);
    send(hardware->sockfd, (char[4]) {9}, 4, 0);
    char buf[4];
    recv(hardware->sockfd, buf, 4, 0);
    pthread_mutex_unlock(&hardware->sockmtx);
    
    return buf[1];
}