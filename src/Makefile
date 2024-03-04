CC = gcc

CFLAGS = -Wall -g -std=gnu99 
LDFLAGS = -lpthread -g
SRC = Tests/orderQueueTest.c inc/order_queue/orderQueue.c

TARGET = result.exe

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
	$(CC) $(CFLAGS) -o button_input_test.o inc/elevator_control/elevator_button_inputs.c inc/elevator_hardware/elevator_hardware.c inc/order_queue/orderQueue.c Tests/elevator_button_input_test.c $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJ)

rebuild:	clean all

.PHONY: all rebuild clean
