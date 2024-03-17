/**
 * @file process_pair.h
 * @brief library for process pair communication
 *          needs file "keyfile" in the same directory as the executable
 * @version 0.1
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define PROSESS_PAIR_DEBUG 0 //set to 1 to enable debug prints

typedef struct queMsg_t {
    long message_type;
    char message_text[6];
} queMsg_t;

typedef struct queMsgCtrl_t {
    queMsg_t* message;
    int msgid;
    struct msqid_ds* buf;
    int sysQueTimeout;
    bool sysQueMaster;
    pthread_mutex_t* queOutputMutex;
} queMsgData_t;

void sysQueInit(int timeOutTimeS); //start the system queue reset system, timeout as argument

void* sysQueRead(void* arg); //read from system queue
void* sysQueWrite(void* arg); //write to system queue
void* sysQueTimeout(void* arg); //timeout for system queue

void* crashSystem(void* arg); //crash the system, used for testing

void makeMaster(queMsgData_t* data); //make this process the sysQue master


