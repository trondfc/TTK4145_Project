#ifndef CONFIG_H
#define CONFIG_H

/// Convert seconds to microseconds
#define SEC_TO_US(sec) ((sec)*1000000)
/// Convert milliseconds to microseconds
#define MS_TO_US(ms)    ((ms)*1000)
/// Convert nanoseconds to microseconds
#define NS_TO_US(ns)    ((ns)/1000)

#define NO_FLOORS 4
#define NO_ELEVATORS 3
#define NO_CONTROLLERS 3

#define MAX_QUEUE_SIZE 20
#define MAX_IP_NODES 10

#define ORDER_POLL_DELAY 50 * 1000 // 10 ms
#define ORDER_SYNC_DELAY 500 * 1000 // 500 ms

#define ORDER_TIMEOUT 30 // 60 seconds
#define DOOR_OPEN_TIME 3 // 5 seconds

#define TCP_PORT 9000
#define UDP_PORT 5000
#define ELEVATOR_PORT 15657

#endif