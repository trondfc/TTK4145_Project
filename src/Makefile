CC = gcc

CFLAGS = -Wall -g -std=gnu99 
LDFLAGS = -lpthread -g 
SRC = main.c inc/sverresnetwork/sverresnetwork.c inc/master_select_logic/master_select_logic.c inc/keep_alive/keep_alive.c inc/process_pair/process_pair.c

TARGET = main

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
clean:
	rm -f $(TARGET) $(OBJ)

rebuild:	clean all

.PHONY: all rebuild clean
