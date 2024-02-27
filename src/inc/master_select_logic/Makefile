run_test: compile_test
	./master_select_logic

compile_test: master_select_logic_test.c master_select_logic.c
	gcc -o master_select_logic -g -Wall master_select_logic_test.c master_select_logic.c ../keep_alive/keep_alive.c ../sverresnetwork/sverresnetwork.c