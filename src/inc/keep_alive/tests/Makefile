run_test: compile_test
	./keep_alive_test.out

compile_test: keep_alive_test.c
	gcc -pthread -o keep_alive_test.out -g -Wall keep_alive_test.c ../keep_alive.c ../../sverresnetwork/sverresnetwork.c

run_state_test: compile_state_select_test
	./state_select_test.out
compile_state_select_test: 
	gcc -pthread -o state_select_test.out -g -Wall state_select_test.c ../keep_alive.c ../../sverresnetwork/sverresnetwork.c