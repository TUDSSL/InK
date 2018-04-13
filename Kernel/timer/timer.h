#ifndef INC_TMR_
#define INC_TMR_

//persistent timers "emulation"
//TODO:Fix that 
//typedef uint16_t __pers_time_t;
//
//
#include "mcu_specifics/clk.h"

void __timers_init();
void __reboot_timers();

//WKUP timers 
//These timers are set to schedule an ISR for a specified time
/*************************************************************************************************************/

//clears the status flag on wkup_d struct containing the thread/timing information for the one shot timer
void clear_wkup_status(uint8_t thread_id);

//unload persistent buffer to local variables for fewer fram accesses
void unpack_wkup_to_local();

//updates the information on which thread is scheduled to execute next based on timing 
void refresh_wkup_timers();

// sets a one-shot timer using Timer A2 
void set_wkup_timer(uint8_t thread_id, uint16_t ticks);

//EXPR timers 
//These timers are set to schedule a time limit in the execution of a thread
//The timer starts counting from the time initiated up to the specified amount of time,
//if the time limit has been surpassed by a death event the thread is evicted from the 
//scheduler.
//The expiration counter is cleared by function call at a specified in the code.
/*************************************************************************************************************/
void clear_xpr_status(uint8_t thread_id);

//unload persistent buffer to local variables for fewer fram accesses
void unpack_xpr_to_local();

void refresh_xpr_timers();

void set_expire_timer(uint8_t thread_id, uint32_t ticks);

void stop_expire_timer(uint8_t thread_id);

//PDC timers (EXPERIMENTAL)
//These timers are set to schedule "periodic" execution of a thread
//The timer starts counting from the time initiated up to the specified amount of time,
//if the time limit has been surpassed by a death event the thread is evicted from the 
//scheduler.
//The expiration counter is cleared by function call at a specified in the code.
/*************************************************************************************************************/
//unload persistent buffer to local variables for fewer fram accesses
void unpack_pdc_to_local();

//set a periodic firing of an event 
void set_periodic_timer(uint8_t thread_id, uint16_t ticks);

//stop the periodic firing of the event 
void stop_periodic_timer(uint8_t thread_id);

/*Internal functions*/
void refresh_pdc_timers();

void clear_pdc_status(uint8_t thread_id);

#endif

