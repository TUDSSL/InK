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
// along with this program.  If not, see <http://www.gnu.org/licenses/>./*
 /* eventsmanager.c
 *
 *  Created on: 19 Feb 2018
 *
 */
#include "ink.h"
#include "persistentqueue.h"
#include "scheduler/priority.h"

enum{   EVENT_INSERT, EVENT_COMMIT, EVENT_SIGNAL,EVENT_DONE } ;

static volatile __nv uint8_t _status = EVENT_DONE;

// keep track of the current event insertion
static __nv thread_t *_thread;
static __nv isr_event_t _event;

// an event queue per each thread
static __nv per_queue_t _events[MAX_THREADS];

// keep track of the popped event since tasks
// might be restarted
static __nv isr_event_t *_popped[MAX_THREADS];

// should be called at the first boot only
void __events_boot_init(){
    uint8_t i;

    for(i=MAX_THREADS;i>0;i--){
        // initialize each queue
        __perqueue_init(&_events[i]);
        _popped[i] = NULL;
    }
}

// This function will be executed withing the context of an ISR or
// it should be also called at each reboot to finish event insertion
void __events_commit(){
    switch(_status){
    case EVENT_INSERT:
        __perqueue_push(&_events[_thread->priority], &_event);
        _status = EVENT_COMMIT;
    case EVENT_COMMIT:
        __perqueue_push_commit(&_events[_thread->priority]);
        _status = EVENT_SIGNAL;
    case EVENT_SIGNAL:
        // if the thread is sleeping, activate it!
        if(_thread->state == THREAD_STOPPED){
            __start_thread(_thread);
        }
        _status = EVENT_DONE;
    }
}

// check if all slots are full or not
inline uint8_t __event_buffer_full_ISR(thread_t *thread){
    return __perqueue_is_full(&_events[_thread->priority]);
}


// Signaling of the events from ISRs-- can be interrupted by power failures.
// Therefore, additional state handling is required
inline void __event_signal_ISR(thread_t *thread, isr_event_t *event){
    // insert to the event queue
    _thread = thread;
    _event = *event;
    _status = EVENT_INSERT;
    __events_commit();
}

// check if there is a pending event for the given thread
inline uint8_t __has_events(thread_t *thread){
    if(__perqueue_is_empty(&_events[thread->priority]))
        return 0;

    return 1;
}

// This function is called by the tasks. Therefore, when the task is restarted,
// this function is recalled. Therefore, we need to keep track of our state.
// We first pop an event and lock it
inline isr_event_t *__lock_event(thread_t *thread){
    _popped[_thread->priority] = __perqueue_pop(&_events[_thread->priority]);
    return _popped[_thread->priority];
}

// Events should be released after lock
inline void __release_event(thread_t *thread){
    __perqueue_pop_commit(&_events[_thread->priority]);
    _popped[_thread->priority] = NULL;
}
