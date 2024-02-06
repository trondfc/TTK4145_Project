#!/bin/bash
gcc elevator_hardware.c elevator_hardware_test.c -o elevator_test.out -lpthread
./elevator_test.out


