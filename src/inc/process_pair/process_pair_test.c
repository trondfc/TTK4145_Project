// code dosent auto remove the old que,
// run "ipcs -q" to see the que, and "ipcrm -q <que id>" to remove it

/**
 * @file process_pair_test.c
 * @author Trondfc ()
 * @brief test file for process_pair module
 * @version 0.1
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "process_pair.h"

int main(){
    sysQueInit(5); //start the system queue reset system, timeout 5 seconds
    printf("done\n");
    while (1)
    {
        /* code */
    }
    
    
    return 0;
}