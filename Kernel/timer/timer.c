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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.#include "persistent_timer_commit/persistent_timer.h"
#include "mcu_specifics/clk.h"
#include "timer.h"
//#include "persistent_timer.h"
#define MAX_TIMED_THREADS 3
#define MAX_XPR_THREADS 3
#define MAX_PDC_THREADS 1
//persistent timers "emulation"
//TODO:Fix that 
typedef uint16_t __pers_time_t;

__pers_time_t __on_time = 0;
__pers_time_t __off_time = 0;

//global
//TODO:check if needed
uint8_t tol = 0;

uint8_t nxt_wkup;
uint16_t min_wkup;

uint8_t nxt_xpr;
uint16_t min_xpr;

uint8_t nxt_pdc;
uint16_t min_pdc;

// double buffer 
// 0 persistent - 1 dirty persistent 
extern pers_timer_t per_timer_vars[];

timing_d wkup_timing[MAX_WKUP_THREADS];
timing_d xpr_timing[MAX_XPR_THREADS];
timing_d pdc_timing[MAX_PDC_THREADS];


void __timers_init()
{
    _pers_timer_init();
}

void __reboot_timers()
{

#ifdef WKUP_TIMER
    _pers_timer_commit(WKUP);
    unpack_wkup_to_local();
    refresh_wkup_timers();
    _pers_timer_update_lock(WKUP);
    _pers_timer_commit(WKUP);
#endif
#ifdef XPR_TIMER
    _pers_timer_commit(XPR);
    unpack_xpr_to_local();
    refresh_xpr_timers();
    _pers_timer_update_lock(XPR);
    _pers_timer_commit(XPR);
#endif  
#ifdef PDC_TIMER
    _pers_timer_commit(PDC);
    unpack_pdc_to_local();
    refresh_pdc_timers();
    _pers_timer_update_lock(PDC);
    _pers_timer_commit(PDC);
#endif

}

//WKUP timers 
//These timers are set to schedule an ISR for a specified time
/*************************************************************************************************************/

//clears the status flag on wkup_d struct containing the thread/timing information for the one shot timer
void clear_wkup_status(uint8_t thread_id){
    
    uint8_t i; 
    for (i = 0; i < MAX_WKUP_THREADS; i++)
    {
        if (wkup_timing[i].thread_id == thread_id) {
            wkup_timing[i].status = NOT_USED;
            _pers_timer_update_status(i,WKUP,NOT_USED);

        }
    }
}

//unload persistent buffer to local variables for fewer fram accesses
void unpack_wkup_to_local(){

    uint8_t i;
    for (i = 0; i < MAX_WKUP_THREADS; i++)
    {
        wkup_timing[i] = _pers_timer_get(i,WKUP);
    }

}

//updates the information on which thread is scheduled to execute next based on timing 
void refresh_wkup_timers(){

    uint8_t i,first = 1;

    for (i = 0; i < MAX_WKUP_THREADS; i++)
    {

        if (wkup_timing[i].status == USED)
        {
            
            if (first)
            {
                min_wkup = wkup_timing[i].data;
                nxt_wkup = wkup_timing[i].thread_id;
                first = 0;
            }
            

            wkup_timing[i].data = wkup_timing[i].data - __get_time(); 
            _pers_timer_update_data(i, WKUP, wkup_timing[i].data);

            if (wkup_timing[i].data < 0 && wkup_timing[i].data > -tol)
            {
                if ((min_wkup > -wkup_timing[i].data) || (min_wkup == -wkup_timing[i].data && nxt_wkup > wkup_timing[i].thread_id))
                {
                    min_wkup = -wkup_timing[i].data;
                    nxt_wkup = wkup_timing[i].thread_id;
                }
            }else if (wkup_timing[i].data < 0 && wkup_timing[i].data < -tol) wkup_timing[i].status = NOT_USED;

            if (wkup_timing[i].data > 0)
            {   //TODO:priority convention
                if ((min_wkup > wkup_timing[i].data) || (min_wkup == wkup_timing[i].data && nxt_wkup > wkup_timing[i].thread_id))
                {
                    min_wkup = wkup_timing[i].data;
                    nxt_wkup = wkup_timing[i].thread_id;
                }
            }
       }
    }

    if (!first)
    {
       _pers_timer_update_nxt_thread(WKUP,nxt_wkup);
       _pers_timer_update_nxt_time(WKUP,min_wkup);
        //set the new pending ISR timer.
        //TODO:set correct CLK
        //timerA2_set_CCR0(min_wkup);
        set_timer_wkup(min_wkup);
    }
    //no pending wake up timer was found.
    else stop_timer();


}

// sets a one-shot timer using Timer A2 
void set_wkup_timer(uint8_t thread_id, uint16_t ticks)
{
    uint8_t i,cmpl = 0;

    //fetch from persistent buffer to local variables 
    unpack_wkup_to_local();

    //init the timer 
    //TODO:check if needed 
    stop_timer();

    //set the compare register on the device
    for (i = 0; i < MAX_WKUP_THREADS; i++)
    {
        if (wkup_timing[i].status == NOT_USED)
        {
            wkup_timing[i].data = ticks;
            _pers_timer_update_data(i,WKUP , ticks);
            wkup_timing[i].thread_id = thread_id;
            _pers_timer_update_thread_id(i, WKUP, thread_id);
            wkup_timing[i].status = USED;
            _pers_timer_update_status(i, WKUP, USED);
            cmpl = 1;
            break;
        }
    }

    if (cmpl)
    {
        refresh_wkup_timers();
    
    }else{

        //buffer is full
        //TODO: ADD failcheck 
        wkup_timing[2].data = ticks;
        _pers_timer_update_data(2,WKUP,ticks);
        wkup_timing[2].thread_id = thread_id;
        _pers_timer_update_thread_id(i,WKUP,thread_id);
        refresh_wkup_timers();
    }

    _pers_timer_update_lock(WKUP);
    _pers_timer_commit(WKUP);
}


//EXPR timers 
//These timers are set to schedule a time limit in the execution of a thread
//The timer starts counting from the time initiated up to the specified amount of time,
//if the time limit has been surpassed by a death event the thread is evicted from the 
//scheduler.
//The expiration counter is cleared by function call at a specified in the code.
/*************************************************************************************************************/
void clear_xpr_status(uint8_t thread_id){
    
    uint8_t i;
    for (i = 0; i < MAX_XPR_THREADS; i++)
    {
        if (xpr_timing[i].thread_id == thread_id){
            xpr_timing[i].status = NOT_USED;
            _pers_timer_update_status(i,XPR,NOT_USED);
            break;
        }    
    }

}

//unload persistent buffer to local variables for fewer fram accesses
void unpack_xpr_to_local(){

    uint8_t i;
    for (i = 0; i < MAX_XPR_THREADS; i++)
    {
        xpr_timing[i] = _pers_timer_get(i,XPR);  
    }

}

void refresh_xpr_timers(){
    
    uint8_t i,first = 1;
    uint8_t nxt_xpr;
    int32_t min_xpr;

    for (i = 0; i < MAX_XPR_THREADS; i++)
    {

        if (xpr_timing[i].status == USED)
        {
            
            if (first)
            {
                min_xpr = xpr_timing[i].data;
                nxt_xpr = xpr_timing[i].thread_id;
                first = 0;
            }
            
            xpr_timing[i].data = xpr_timing[i].data - __get_time(); 
            _pers_timer_update_data(i,XPR,xpr_timing[i].data);

            if (xpr_timing[i].data <= 0)
            {
                //evict thread 
                //TODO:fix bug to disable ISR
                //TODO:find a more elegant way??
                //__stop_thread(__get_thread(xpr_timing[i].thread_id));
                __evict_thread(__get_thread(xpr_timing[i].thread_id));
                //update struct
                clear_xpr_status(xpr_timing[i].thread_id);
            }

            if (xpr_timing[i].data > 0)
            {   //TODO:priority convention
                if ((min_xpr > xpr_timing[i].data) || (min_xpr == xpr_timing[i].data && nxt_xpr < xpr_timing[i].thread_id))
                {
                    min_xpr = xpr_timing[i].data;
                    nxt_xpr = xpr_timing[i].thread_id;
                }
            }
       }
    }

    if (!first)
    {   _pers_timer_update_nxt_thread(XPR,nxt_xpr);
        _pers_timer_update_nxt_time(XPR,min_xpr); 
        //set the new pending ISR timer.
        //timerA2_set_CCR0(min_xpr);
        set_timer_xpr(min_xpr);
    }
    //no pending wake up timer was found.
    else stop_timer_xpr();

}


void set_expire_timer(uint8_t thread_id, uint32_t ticks){

    uint8_t i,cmpl = 0;

    //__set_xpr_timer(__get_thread(thread_id), ticks);

    unpack_xpr_to_local();

    for ( i = 0; i < MAX_XPR_THREADS; i++)
    {
        if (xpr_timing[i].status == NOT_USED)
        {
            xpr_timing[i].data = ticks + __get_time();
            _pers_timer_update_data(i,XPR,__get_time()+ticks);
            xpr_timing[i].thread_id = thread_id;
            _pers_timer_update_thread_id(i,XPR,thread_id);
            xpr_timing[i].status = USED;
            _pers_timer_update_status(i,XPR,USED);
            cmpl = 1;
            break;
        }
    }

    if (cmpl)
    {
        refresh_xpr_timers();
    
    }else{

        //failure 
        //TODO: ADD fail check 
        _pers_timer_update_data(MAX_XPR_THREADS-1,XPR,ticks);
        _pers_timer_update_thread_id(MAX_XPR_THREADS-1,XPR,thread_id);
        refresh_xpr_timers();
    }

    _pers_timer_update_lock(XPR);
    _pers_timer_commit(XPR);
}


void stop_expire_timer(uint8_t thread_id){
   
    //__set_xpr_timer(__get_thread(thread_id),0);

    unpack_xpr_to_local();

    clear_xpr_status(thread_id);

    //TODO:care infinite loop?
    refresh_xpr_timers();

    _pers_timer_update_lock(XPR);
    _pers_timer_commit(XPR);

}
//PDC timers (EXPERIMENTAL)
//These timers are set to schedule "periodic" execution of a thread
//The timer starts counting from the time initiated up to the specified amount of time,
//if the time limit has been surpassed by a death event the thread is evicted from the 
//scheduler.
//The expiration counter is cleared by function call at a specified in the code.
/*************************************************************************************************************/
//unload persistent buffer to local variables for fewer fram accesses
void unpack_pdc_to_local(){

    uint8_t i;
    for (i = 0; i < MAX_TIMED_THREADS; i++)
    {
        pdc_timing[i] = _pers_timer_get(i,PDC);  
    }

}

//set a periodic firing of an event 
void set_periodic_timer(uint8_t thread_id, uint16_t ticks){
    uint8_t i,cmpl = 0;

    unpack_pdc_to_local();
    
    //TODO:figure this out 
    //tm_pdc = ticks;

    for ( i = 0; i < MAX_PDC_THREADS; i++)
    {
        if (pdc_timing[i].status == NOT_USED)
        {               
            __set_pdc_timer(__get_thread(thread_id), ticks);
            __set_pdc_period(__get_thread(thread_id), 1);
            _pers_timer_update_data(i,PDC,ticks);
            pdc_timing[i].data = ticks;
            _pers_timer_update_thread_id(i,PDC,thread_id);
            pdc_timing[i].thread_id = thread_id;
            _pers_timer_update_status(i,PDC,USED);
            pdc_timing[i].status = USED;
            cmpl = 1;

            break;
        }
    }

    if (cmpl)
    {
        refresh_pdc_timers();
    
    }else{

        //failure 
        //TODO: ADD failcheck 
        pdc_timing[MAX_PDC_THREADS-1].data = ticks;
        _pers_timer_update_data(MAX_PDC_THREADS-1,PDC,ticks);
        pdc_timing[MAX_PDC_THREADS-1].thread_id = thread_id;
        _pers_timer_update_thread_id(MAX_PDC_THREADS-1,PDC,thread_id);

        refresh_pdc_timers();
    }

    _pers_timer_update_lock(PDC);
    _pers_timer_commit(PDC);
}

//stop the periodic firing of the event 
void stop_periodic_timer(uint8_t thread_id){
    
    unpack_pdc_to_local();

    clear_pdc_status(thread_id);

    //TODO:care infinite loop?
    refresh_pdc_timers();

    _pers_timer_update_lock(XPR);
    _pers_timer_commit(XPR);

}

/*Internal functions*/
void refresh_pdc_timers(){

    uint8_t i,first = 1;

    for (i = 0; i < MAX_PDC_THREADS; i++)
    {

        if (pdc_timing[i].status == USED)
        {
            
            if (first)
            {
                min_pdc = pdc_timing[i].data;
                nxt_pdc = pdc_timing[i].thread_id;
                first = 0;
            }


            pdc_timing[i].data = pdc_timing[i].data - __get_time(); 
            _pers_timer_update_data(i,PDC,pdc_timing[i].data);
            
            if (pdc_timing[i].data < 0 && pdc_timing[i].data > -tol)
            {
                if ((min_pdc > -pdc_timing[i].data) || (min_pdc == -pdc_timing[i].data && nxt_pdc > pdc_timing[i].thread_id))
                {
                    min_pdc = -pdc_timing[i].data;
                    nxt_pdc = pdc_timing[i].thread_id;
                }
            }else if (pdc_timing[i].data < 0 && pdc_timing[i].data < -tol) pdc_timing[i].status = NOT_USED;

            if (pdc_timing[i].data > 0)
            {   //TODO:priority convention
                if ((min_pdc > pdc_timing[i].data) || (min_pdc == pdc_timing[i].data && nxt_pdc > pdc_timing[i].thread_id))
                {
                    min_pdc = pdc_timing[i].data;
                    nxt_pdc = pdc_timing[i].thread_id;
                }
            }
       }
    }

    if (!first)
    {
       _pers_timer_update_nxt_thread(PDC,nxt_pdc);
       _pers_timer_update_nxt_time(PDC,min_pdc);
        //set the new pending ISR timer.
        //timerA2_set_CCR0(min_pdc);
        set_timer_pdc(min_pdc);
    }
    //no pending wake up timer was found.
    else stop_timer();



}

void clear_pdc_status(uint8_t thread_id){
    
    uint8_t i; 
    for (i = 0; i < MAX_PDC_THREADS; i++)
    {
        if (pdc_timing[i].thread_id == thread_id){

            pdc_timing[i].status = NOT_USED;
            _pers_timer_update_status(i,PDC,NOT_USED);

        } 
    }
}


//ISR register section
/*************************************************************************************************************/

// the event that will be used to register the uart event
isr_event_t timer_event;

#ifdef WKUP_TIMER
/* the timer interrupt handler */
//wkup_interrupt
__interrupt(TIMER0_A0_VECTOR)
{
    /* stop timer */
    TA0CCTL0 = 0x00;
    TA0CTL = 0;

    if(!__EVENT_BUFFER_FULL(_pers_timer_get_nxt_thread(WKUP))){
        timer_event.ISRtype = 0;
        timer_event.data = NULL;
        timer_event.size = 0;
        timer_event.timestamp = __get_time();
        //__on_time = TA2CCR0;
        __SIGNAL_EVENT(_pers_timer_get_nxt_thread(WKUP),&timer_event);
        
        //TODO: intermittent protection
        unpack_wkup_to_local();
        clear_wkup_status(_pers_timer_get_nxt_thread(WKUP));
        refresh_wkup_timers();
        _pers_timer_update_lock(WKUP);
        _pers_timer_commit(WKUP);
    }

    
    /* turn on CPU */
    __bic_SR_register_on_exit(LPM3_bits);
}
#endif

#ifdef XPR_TIMER
/* the timer interrupt handler */
//xpr_interrupt
_interrupt(TIMER2_A0_VECTOR)
{
    /* stop timer */
    TA2CCTL0 = 0x00;
    TA2CTL = 0;

    //__on_time = TA2CCR0;

    //TODO: intermittent protection
    unpack_xpr_to_local();
    //__stop_thread(__get_thread(nxt_xpr));
    __evict_thread(__get_thread(nxt_xpr));
    clear_xpr_status(_pers_timer_get_nxt_thread(XPR));
    refresh_xpr_timers();
    P4OUT |= BIT2;
    P4OUT &= ~BIT2;
    _pers_timer_update_lock(XPR);
    _pers_timer_commit(XPR);
    /* turn on CPU */
    __bic_SR_register_on_exit(LPM3_bits);
}
#endif

#ifdef PDC_TIMER
/* the timer interrupt handler */
//pdc_interrupt
uint16_t pdc_count = 1;
__interrupt(TIMER1_A0_VECTOR)
{
    /* stop timer */
    TA1CCTL0 = 0x00;
    TA1CTL = 0;

    if(!__EVENT_BUFFER_FULL(_pers_timer_get_nxt_thread(PDC))){
        timer_event.ISRtype = 0;
        timer_event.data = NULL;
        timer_event.size = 0;
        timer_event.timestamp = __get_time();
        //__on_time = TA1CCR0;
        __SIGNAL_EVENT(_pers_timer_get_nxt_thread(PDC),&timer_event);
        
        //TODO: intermittent protection
        //TODO: more clever implementation
        __set_pdc_period(__get_thread(thread_id), ++__get_pdc_period(__get_thread(thread_id)));


        unpack_to_local();
        clear_pdc_status(_pers_timer_get_nxt_thread(PDC));
        set_persiodic_timer(_pers_timer_get_nxt_thread(PDC),__get_pdc_period(__get_thread(thread_id)*__get_pdc_timer(__get_thread(thread_id)));
        refresh_pdc_timers();
        _pers_timer_update_lock(PDC);
        _pers_timer_commit(PDC);
    }

    
    /* turn on CPU */
    __bic_SR_register_on_exit(LPM3_bits);
}
#endif
