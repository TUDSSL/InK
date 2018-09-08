// This file is part of InK.
// 
// author = "dpatoukas" 
// maintainer = "dpatoukas"
// email = "dpatoukas@gmail.com" 
//  
// copyright = "Copyright 2018 Delft University of Technology" 
// license = "LGPL" 
// version = "3.0" 
// status = "Production"
//
// 
// InK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.//perssistent timers data
#ifndef PERS_TMR_
#define PERS_TMR_
#include "ink.h"

//how many timing Ink interfaces are used
#define TIMER_TOOLS 3
#define MAX_TIMED_THREADS 3
#define MAX_WKUP_THREADS 3
#define MAX_XPR_THREADS 3
#define MAX_PDC_THREADS 1

//TODO:make sure types are correctly used 
typedef enum{TIMER_INSERT,TIMER_COMMIT,TIMER_DONE} tmr_st;

typedef enum{NOT_DIRTY,DIRTY} dirty_st;

typedef enum{NOT_USED,USED} used_st;

typedef enum{WKUP,PDC,XPR} ink_time_interface_t;

/**
 * Contains the timing data for each timer 
 */
typedef struct
{
    used_st status;/** USED - NOT_USED*/
    uint8_t thread_id;  /** thread ID*/
    int32_t data;      /** remaining time for thread execution*/
    dirty_st __dirty;    /** DIRTY - NOT_DIRTY*/

}timing_d;

/**
 * Contains the next thread to be executed persistent timer
 */
typedef struct 
{
	used_st status;/** USED - NOT_USED*/
	uint8_t next_thread;/** next thread candidate*/
	uint16_t next_time; /** next timing candidate*/
	dirty_st __dirty;/** DIRTY - NOT_DIRTY*/

}next_d;

typedef struct 
{	
	uint16_t on_time;   /**time the system has been on*/
	uint16_t off_time; /** time the system has been off*/
	dirty_st __dirty;   /** DIRTY - NOT_DIRTY*/

}pers_time_d;

/**
 * Contains timing data for WakeUp/Expiration/Periodic timer,
 * global time,and next thread to be fired by each timer.
 *  
*/
typedef struct 
{
	timing_d wkup_timing[MAX_WKUP_THREADS];/**Timings for WakeUp timer*/
	timing_d xpr_timing[MAX_XPR_THREADS]; /**Timings for Expiration timer*/
	timing_d pdc_timing[MAX_PDC_THREADS]; /**Timings for Periodic timer*/
	next_d next_info[TIMER_TOOLS];			/**Next time to be executed persistent timer*/
	pers_time_d time_bank;					/**Global time*/

}pers_timer_t;


static volatile __nv tmr_st pdc_tstatus = TIMER_DONE;/**Initialize the Periodic state timer commit state machine */
static volatile __nv tmr_st xpr_tstatus = TIMER_DONE;/**Initialize the Expire state timer commit state machine */
static volatile __nv tmr_st wkup_tstatus = TIMER_DONE;/**Initialize the WakeUp state timer commit state machine */

/**
 * Initialize the buffers for holding timing information
 */
void _pers_timer_init();

/**
 * @param idx Index 
 * @param interface selected timer  
 * @param time data timing data	
 */
void _pers_timer_update_data(uint8_t idx, ink_time_interface_t interface , uint32_t time_data);

/**
 * @param idx Index 
 * @param interface selected timer  
 * @param thread_id Thread ID	
 */
void _pers_timer_update_thread_id(uint8_t idx, ink_time_interface_t interface , uint8_t thread_id);

/**
 * @param idx Index 
 * @param interface selected timer  
 * @param status Status of the buffer
 */
void _pers_timer_update_status(uint8_t idx, ink_time_interface_t interface , used_st status);

/**
 * @param interface selected timer  
 * @param next_tread next thread for executing set for this timer
 */
void _pers_timer_update_nxt_thread(ink_time_interface_t interface ,uint8_t next_thread);

/**
 * @param interface selected timer  
 * @param  next_time remaining time for next interrupt event	
 */
void _pers_timer_update_nxt_time(ink_time_interface_t interface, uint16_t next_time);


//collect data from the perssistent buffer
/**
 * @param idx Index
 * @param interface selected timer
 * @return Timing information from the persistent buffer 
 */
timing_d _pers_timer_get(uint8_t idx, ink_time_interface_t interface );

/**
 * @param	idx 	Index
 * @param	interface 	selected timer
 * @return	time information from persistent buffer
 */
uint16_t _pers_timer_get_data(uint8_t idx, ink_time_interface_t interface );

/**
 * @param	idx 	Index 
 * @param	interface 	selected timer 
 * @return  thread id
 */
uint8_t _pers_timer_get_thread_id(uint8_t idx, ink_time_interface_t interface );

/**
 * @param	idx 	Index 
 * @param	interface 	selected timer 
 * @return	timer status
 */
used_st _pers_timer_get_status(uint8_t idx, ink_time_interface_t interface );

/**
 * @param	idx 	Index 
 * @param	interface 	selected timer 
 * @return  next thread for execution
 */
uint8_t _pers_timer_get_nxt_thread(ink_time_interface_t interface);

/**
 * @param	idx 	Index 
 * @param	interface 	selected timer 
 * @return  next time for execution
 */
uint16_t _pers_timer_get_nxt_time(ink_time_interface_t interface);


//timer buffer is ready to commit
/**
 * Lock the dirty buffer and prepare for commit
 * @param interface selected timer
 */
void _pers_timer_update_lock(ink_time_interface_t interface);

//commit into the persistent buffer
/**
 * Commit to persistent safe buffer 
 * @param interface selected timer 
 */
void _pers_timer_commit(ink_time_interface_t interface);

//internal function
/**
 * Internal function for commiting data
 * @param interface selected timer
 */
void _commit_timer_buffers(ink_time_interface_t interface);

#endif
