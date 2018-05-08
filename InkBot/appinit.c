#include "robot.h"

// extern void main_thread_init();
extern void patrol_thread_init();
// extern void ctrl_thread_init();


// this is the function that will be called only at initial boot by the runtime.
void __app_init()
{
	//central thread to monitor all robot function 
	// main_thread_init();
    //patrol thread init
    patrol_thread_init();
    //control thread init
    // ctrl_thread_init();
}

void __app_reboot(){

	patrol_thread_reboot();
    
}

