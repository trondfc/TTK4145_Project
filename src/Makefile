CC = gcc

CFLAGS = -Wall -g -std=gnu99
LDFLAGS = -lpthread
SRC = main.c inc/sverresnetwork/sverresnetwork.c inc/keep_alive/keep_alive.c inc/process_pair/process_pair.c

TARGET = main.out

all: $(TARGET)

# Define all object files.
OBJ = $(SRC:.c=.o)

# rule to link the program
$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compile: create object files from C source files.
%.o : %.c
	$(CC) $(CFLAGS) -c  $< -o $@ 

# rule for cleaning re-compilable files.

button_input_test:
	$(CC) $(CFLAGS) -o button_input_test.o inc/sverresnetwork/sverresnetwork.c inc/order_queue/send_order_queue.c inc/elevator_control/elevator_button_inputs.c inc/elevator_hardware/elevator_hardware.c inc/order_queue/orderQueue.c Tests/elevator_button_input_test.c $(LDFLAGS)

orderQueueTestListen:
	$(CC) $(CFLAGS) -o que_test_listen.out inc/order_queue/orderQueue.c inc/sverresnetwork/sverresnetwork.c inc/order_queue/send_order_queue.c Tests/orderQueueTestListen.c $(LDFLAGS)

orderQueueTest:
	$(CC) $(CFLAGS) -o que_test_listen.out inc/order_queue/orderQueue.c inc/sverresnetwork/sverresnetwork.c inc/order_queue/send_order_queue.c Tests/orderQueueTestListen.c $(LDFLAGS)

main:
	$(CC) $(CFLAGS) -o main.out main.c inc/elevator_control/elevator_button_inputs.c inc/elevator_hardware/elevator_hardware.c inc/order_queue/orderQueue.c inc/process_pair/process_pair.c inc/keep_alive/keep_alive.c inc/order_queue/send_order_queue.c inc/sverresnetwork/sverresnetwork.c $(LDFLAGS)

test:
	$(CC) $(CFLAGS) -o test.out inc/keep_alive/keep_alive.c inc/sverresnetwork/sverresnetwork.c Tests/test.c $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJ)

rebuild:	clean all

.PHONY: all rebuild clean
