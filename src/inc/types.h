#pragma once

#include "elevator_hardware/elevator_hardware.h"
#include "config.h"

typedef enum elevator_direction{
  UP,
  DOWN,
  STOP
}elevator_direction_t;

typedef struct elevator_status{
  elevator_hardware_info_t elevator;
  bool alive;
  int floor;
  bool at_floor;
  elevator_direction_t direction;
  bool obstruction;
  bool stop;
  int number_of_stop_readings;
  bool door_open;
  bool in_use;
  pthread_mutex_t mutex;
}elevator_status_t;

typedef struct floor_buttons{
  bool floors[NO_FLOORS];
}floor_buttons_t;

typedef struct button_lights{
  floor_buttons_t *up;
  floor_buttons_t *down;
  floor_buttons_t *cab[MAX_IP_NODES];
  pthread_mutex_t *mutex;
}button_lights_t;

typedef struct button_lights_history{
  button_lights_t *old;
  button_lights_t *new;
}button_lights_history_t;