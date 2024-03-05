#pragma once

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../order_queue/orderQueue.h"
#include "../sverresnetwork/sverresnetwork.h"

void send_order_queue_init(TMessageCallback messageCallback, TTcpConnectionCallback connectionCallback);

void send_order_queue_connect(char * ip, int port);
void send_order_queue_listen(int port);

void send_order_queue_send_order(char * ip, order_queue_t *queue);


