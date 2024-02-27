#include "master_select_logic.h"

typedef enum{
    SLAVE=0,
    MASTER=1
}state_t;

typedef struct {
    state_t state;
}config_t;

int main()
{
    
    keep_alive_update();
    return 0;
}