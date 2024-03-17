# Interface types

- [Interface types](#interface-types)
  - [Elevator panel interface types](#elevator-panel-interface-types)
    - [Elevator buttons](#elevator-buttons)
    - [Elevator lights](#elevator-lights)
  - [Elevator status interface types](#elevator-status-interface-types)
  - [Elevator controlle interface types](#elevator-controlle-interface-types)
  
## Elevator panel interface types

### Elevator buttons

```c
typedef struct elevator_single_cab_buttons_t
{
    bool cab_floor_request[N_FLOORS];
    bool emergency_stop;
    bool door_obstruction;
}elevator_single_cab_buttons_t;

typedef struct elevator_multiple_cab_buttons_t
{
	elevator_external_button_t elevator[N_ELEVATORS]
}elevator_multiple_cab_buttons_t;
// example usage
elevator_multiple_cab_buttons_t cab_buttons;
cab_buttons.elevator[1].cab_floor_request[1]; // check for flor 1 request in elevator 1

////////////////////////////////////////////////////////////////////////////
typedef struct elevator_single_floor_buttons_t
{
    bool request_up;	// Comon bool for all elevators
    bool request_down;	// Comon bool
}elevator_single_floor_buttons_t;

typedef struct elevator_multiple_floors_buttons_t
{
	elevator_single_floor_buttons_t floor[N_FLOORS]
}elevator_multiple_floors_buttons_t;
// Example usage
elevator_multiple_floors_buttons_t floor_buttons;
floor_buttons.floor[1].requenst_up; // check for request up at floor 1

```

### Elevator lights

```c
typedef struct elevator_single_lights_t
{
	bool floor_button[N_FLOORS];
    bool stop_button;
    bool door_open;
    bool up_button[N_FLOORS];
    bool down_button[N_FLOORS];
    bool cab_floor[N_FLOORS];
}elevator_single_lights_t;
typedef struct elevator_multiple_lights_t
{
	elevator_single_cab_lights_t elevator[N_ELEVATORS];
}elevator_multiple_lights_t;
// example usage
elevator_multiple_lights_t elevator_lights;
elevator_lights.elevator[1].stop_button = true; // turn on stop button light in elevator 1
```

## Elevator status interface types

```c
typedef enum elevator_direction_t
{
    DIRN_DOWN = -1,
    DIRN_STOP = 0,
    DIRN_UP = 1
}elevator_direction_t;

typedef struct elevator_single_cab_status_t
{
    uint8_t prev_floor; // last floor visited, current floor if at floor
    bool at_floor; // true if at floor, false if between floors
    elevator_direction_t current_direction;
    bool responding; // true if elevator is responding to orders, false if network is down
}elevator_single_cab_status_t;

typedef struct elevator_multiple_cab_status_t
{
    elevator_single_cab_status_t elevator[N_ELEVATORS];
}elevator_multiple_cab_status_t;
// example usage
elevator_multiple_cab_status_t cab_status;
cab_status.elevator[1].prev_floor; // get previous floor of elevator 1
```

## Elevator controlle interface types

```c
typedef enum controller_mode_t
{
    MODE_SLAVE = 0,
    MODE_MASTER = 1
}controller_mode_t;

typedef struct controller_single_status_t
{
    bool initialized;
    controller_mode_t mode;
    uint8_t master_priority;
}controller_single_status_t;

typedef struct controller_multiple_status_t
{
    controller_single_status_t controller[N_CONTROLLERS];
}controller_multiple_status_t;
// example usage
controller_multiple_status_t controller_status;
controller_status.controller[1].mode = MODE_MASTER; // set elevator 1 to master mode
```
