//thread4
//
#include "app.h"
#include "ink.h"
#include "app_intrinsics.h"
#include "interrupts.h"
// define task-shared persistent variables.

 __shared(
     uint16_t timer_value;
     uint16_t energy_status;
 )


ENTRY_TASK(t_init);
TASK(t_energy_check);
TASK(t_transmit);

// called at the very first boot
void thread4_init(){
    // create a thread with priority 15 and entry task task1
    __CREATE(THREAD4,t_init);

}

__nv uint8_t init = 0;

ENTRY_TASK(t_init) 
{
	P3OUT |= BIT4;
	P3OUT &= ~BIT4;
	if (init){
	    __SET(timer_value,++__GET(timer_value));
	}
	else
	{
	    init = 1;
	    __SET(timer_value,0);
	}

	if (__GET(energy_status))
	{
		return t_energy_check;

	}
	else
	{
		return t_transmit;
	}


}

TASK(t_energy_check)
{
	__delay_cycles(1000);

	__SET(energy_status,1);
	__SIGNAL(THREAD1);
	return NULL;
}

TASK(t_transmit)
{
	__delay_cycles(1000);
	__SIGNAL(THREAD1);
	__SET(energy_status,0);
	return NULL;

}

