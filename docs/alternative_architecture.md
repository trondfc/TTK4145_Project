# Alternative Architecture

- [Alternative Architecture](#alternative-architecture)
  - [flow](#flow)
    - [Button pressed](#button-pressed)
    - [Floor light](#floor-light)
    - [Serving requests](#serving-requests)
    - [Timed out requests](#timed-out-requests)
    - [Syncing controllers](#syncing-controllers)
  - [Proposed interface types](#proposed-interface-types)
    - [Enums](#enums)
    - [Order struct](#order-struct)
    - [Order que](#order-que)
    - [Elevator status](#elevator-status)
    - [All elevator status](#all-elevator-status)

Proposed alternative architecture for the project.

p2p network of nodes with que system for elevator requests. Shold not need to be avare of the state of the other controllers, only the state of the elevators and the requests.

I think this will be a more robust system and easier to implement than the current architecture. Will alow us to focus on the elevator system and not the network.

There is probably a lot of things that needs to be added to this, but I think it is a good starting point.

## flow

### Button pressed

- M1
  - compare order to existing orders
  - add if new order
  - update peers about new order
  - Order lights set by controller receiving order from peer, enshuring that multiple controllers know about the order before setting the light
  
- M2
  - Wait x time for M1 to handle order
  - compare order to existing orders (if added by M1 it will be the same)
  - add if new order
  - update peers about new order
  - Order lights set by controller receiving order from peer, enshuring that multiple controllers know about the order before setting the light
- M3
  - Wait 2x time for M1 or m2 to handle order
  - compare order to existing orders (if added by M1 or M2 it will be the same)
  - add if new order
  - update peers about new order
  - Order lights set by controller receiving order from peer, enshuring that multiple controllers know about the order before setting the light

### Floor light

- All controllers can set floor light in parallel (shuld be no problem with current elevator system)

### Serving requests

- Free controller serves oldest request in que
- serves all requests in same travel direction at same time
- sets status as active
- "books"/"locks" elevator
- updates peers about status change

### Timed out requests

- after y amount of time since started serving requests get reset to received in que if not completed

### Syncing controllers

  - all sync messages get sent as old que + change
  - upon missing order in reciever, missing order gets added
  - upon missing order in sender, missing order gets sent as add
  - possible change requests:
	- add: command with a complete order struct
	- modify: command with a complete order struct
	- *remove: modify to complete, completed orders get removed
- in addition the controllers need a get all command to be used upon reboot.

## Proposed interface types

### Enums

```c
typedef enum order_types_t{
  UP_FROM = 0;
  DOWN_FROM = 1;
  GO_TO = 2;
}order_types_t;
typedef enum order_status_t{
  RECIVED = 0;
  ACTIVE = 1;
  COMPLETED = 2;
}
```

### Order struct

```c
typedef struct order_event_t{
  int order_id; // Randomly generated number (big enough to make duplicates unlikely)
  uint8_t elevator_id;
  order_types_t order;
  uint64_t timestamp; // modified timestamp?
  order_status_t status;
  uint8_t controller_id; // Controller serving if active
}order_event_t;
```

### Order que

```c
typedef struct order_que_t{
  order_event_t order[N_ORDERS];
}order_que_t;
```

### Elevator status

```c
typedef struct elevator_status_t{
  uint8_t prev_floor; // last floor visited, current floor if at floor
  bool at_floor; // true if at floor, false if between floors
  elevator_direction_t current_direction;
  bool responding; // true if elevator is responding to orders, false if network is down
  int used_by; // controller id, -1 if not used
}elevator_status_t;
```

### All elevator status

```c
typedef struct all_elevator_status_t{
  elevator_status_t elevator[N_ELEVATORS];
}all_elevator_status_t;
```