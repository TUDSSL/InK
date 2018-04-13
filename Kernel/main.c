#include <msp430.h> 
#include "ink.h"

// indicates if this is the first boot.
__nv uint8_t __inited = 0;

//global time in ticks
extern uint32_t current_ticks;

// this is the entry function for the application initialization.
// applications should implement it.
extern void __app_init();
extern void __app_reboot();

int main(void)
{
    // always init microcontroller
    __mcu_init();

	// if this is the first boot
	if(!__inited){
	    // init the scheduler state
	    __scheduler_boot_init();
	    // init the event handler
	    __events_boot_init();
#ifdef TIMERS_ON
	    __get_time_init();
	    //init the timers
	    __timers_init();
#endif	    
	    // init the applications
	    __app_init();
	    // the first and initial boot is finished
	    __inited = 1;
	}
#ifdef TIMERS_ON
	else{
		__get_time_init();
	}
#endif

	// will be called at each reboot of the application
	__app_reboot();

	// activate the scheduler
	__scheduler_run();

	return 0;
}
