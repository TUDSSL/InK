// This file is part of InK.
// 
// author = "Kasım Sinan Yıldırım " 
// maintainer = "Kasım Sinan Yıldırım "
// email = "sinanyil81 [at] gmail.com" 
//  
// copyright = "Copyright 2018 Delft University of Technology" 
// license = "LGPL" 
// version = "3.0" 
// status = "Production"
//
// 
// InK is free software: you ca	n redistribute it and/or modify
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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/*
 * scheduler.c
 *
 *  Created on: 12 Feb 2018
 *
 */

#include "ink.h"
#include "scheduler.h"
#include "priority.h"

// all threads in the system
static __nv thread_t _threads[MAX_THREADS];

// variables for keeping track of the ready threads
static __nv priority_t _priorities;

enum { SCHED_SELECT, SCHED_BUSY };

// the id of the current thread being executed.
static __nv thread_t *_thread = NULL;

static volatile __nv uint8_t _sched_state = SCHED_SELECT;

void __scheduler_boot_init() {
    uint8_t i;

    // clear priority variables for the threads
    __priority_init(&_priorities);

    for (i = MAX_THREADS; i > 0; i--){
        // threads are not created yet
        _threads[i].state == THREAD_STOPPED;
    }
    _sched_state = SCHED_SELECT;
}

// Assigns a slot to a thread. Should be called ONLY at the first system boot
void __create_thread(uint8_t priority, void *entry, void *data_org,
                     void *data_temp, uint16_t size)
{
    // init properties
    _threads[priority].priority = priority;
    _threads[priority].entry = entry;
    _threads[priority].next = entry;
    _threads[priority].state = THREAD_STOPPED;

    // init shared buffer
    _threads[priority].buffer.buf[0] = data_org;
    _threads[priority].buffer.buf[1] = data_temp;
    _threads[priority].buffer.idx = 0;
    _threads[priority].buffer.size = size;
}

// puts the thread in waiting state
inline void __stop_thread(thread_t *thread){
    __priority_remove(thread->priority, &_priorities);
    thread->state = THREAD_STOPPED;
}

// puts the thread in waiting state
void __evict_thread(thread_t *thread){
    __priority_remove(thread->priority, &_priorities);
    thread->next = NULL;
    thread->state = THREAD_STOPPED;
}

void __set_sing_timer(thread_t *thread,uint16_t timing){
    thread->sing_timer = timing;
    return;
}

//TODO: update necessary
void __set_expr_timer(thread_t *thread,uint16_t timing){
    thread->expr_timer = timing;
    return;
}


void __set_pdc_timer(thread_t *thread,uint16_t timing){
    thread->pdc_timer = timing;
    return;
}

void __set_pdc_period(thread_t *thread,uint16_t period){
    thread->pdc_period = period;
    return;
}

uint16_t __get_pdc_timer(thread_t *thread){
    return thread->pdc_timer;
}

uint16_t __get_pdc_period(thread_t *thread){
    return thread->pdc_period;
}

// puts the thread in active state
inline void __start_thread(thread_t *thread) {
    thread->next = thread->entry;
    __priority_insert(thread->priority, &_priorities);
    thread->state = TASK_READY;
}

// returns the highest-priority thread
static inline thread_t *__next_thread(){
    uint8_t idx = __priority_highest(&_priorities);
    if(idx)
        return &_threads[idx];

    return NULL;
}

inline thread_t *__get_thread(uint8_t priority){
    return &_threads[priority];
}

// finish the interrupted task before enabling interrupts
static inline void __task_commit(){
    if(_thread){
        __tick(_thread);
    }
}

// at each step, the scheduler selects the highest priority thread and
// runs the next task within the thread
void __scheduler_run()
{
    // For the sake of consistency, the event insertion by an ISR which
    // was interrupted by a power failure should be committed to the
    // event queue _events in isrmanager.c before enabling the interrupts.
    __events_commit();

    // always finalize the latest task before enabling interrupts since
    // this task might be interrupted by a power failure and the changes
    // it performs on the system variables (e.g. on _priorities due to
    // signaling another task or on the event queue _events in isrmanager.c)
    // will be committed before enabling interrupts so that these variables
    // remain consistent and stable.
    __task_commit();

#ifdef TIMERS_ON
    __reboot_timers();
#endif

    // enable interrupts
    __enable_interrupt();

    while (1){
        switch (_sched_state){
        case SCHED_SELECT:
            // the scheduler selects the highest priority task right
            // after it has finished the execution of a single task
            _thread = __next_thread();
            _sched_state = SCHED_BUSY;
        case SCHED_BUSY:
            // always execute the selected task to completion
            // execute one task inside the highest priority thread
            if (_thread){
                __tick(_thread);
                // after execution of one task, check the events
                _sched_state = SCHED_SELECT;
                break;
            }
            _sched_state = SCHED_SELECT;
            __disable_interrupt();
            // check the ready queue for the last time
            if(!__next_thread()){
                __mcu_sleep();
                __enable_interrupt();
            }
        }
    }
}
