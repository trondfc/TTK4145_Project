## Compiling elevator controller
- Compile the code by navigating to the src directory and run:
```
make main
``` 
## Running elevator controller
- The system needs to be run at a computer with the `elevatorserver` alias.
- By running `./main.out` the code starts up the controller and elevatorserver 

## Configuring ports
- if needed the network ports can be changed by their definitions in "inc/config.h". Default ports are:
    - 15657 : Elevator communication port
    - 5000 : UDP broadcast port
    - 9000 : TCP data sync port