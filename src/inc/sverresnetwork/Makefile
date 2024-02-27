run_send: compile_send
	./send

compile_send:
	gcc -lpthread -Wall -o send TCP_send.c sverresnetwork.c


run_listen: compile_listen
	./listen

compile_listen:
	gcc -lpthread -Wall -o listen TCP_listen.c sverresnetwork.c