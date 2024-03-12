#pragma once

#include "elevator_hardware/elevator_hardware.h"

typedef enum elevator_direction{
  UP,
  DOWN,
  STOP
}elevator_direction_t;

typedef struct elevator_status{
  elevator_hardware_info_t elevator;
  bool alive;
  int floor;
  elevator_direction_t direction;
  bool obstruction;
  bool stop;
  bool door_open;
  bool in_use;
  pthread_mutex_t mutex;
}elevator_status_t;