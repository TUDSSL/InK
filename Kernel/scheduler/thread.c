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
 * thread.c
 *
 *  Created on: 11 Feb 2018
 *
 */

#include "ink.h"

// prepares the stack of the thread for the task execution
static inline void __prologue(thread_t *thread)
{
    buffer_t *buffer = &thread->buffer;
#ifdef RAISE_PIN
    __port_on(3,6);
#endif
    // copy original stack to the temporary stack
    __dma_word_copy(buffer->buf[buffer->idx],buffer->buf[buffer->idx ^ 1], buffer->size>>1);
#ifdef RAISE_PIN
    __port_off(3,6);
#endif

}

// runs one task inside the current thread
void __tick(thread_t *thread)
{
    void *buf;
    switch (thread->state)
    {
    case TASK_READY:
#ifdef RAISE_PIN
        __port_off(3,5);
#endif
        // refresh thread stack
        __prologue(thread);
        // get thread buffer
        buf = thread->buffer.buf[thread->buffer._idx^1];
        // Check if it is the entry task. The entry task always
        // consumes an event in the event queue.
        if(thread->next == thread->entry){
            // pop an event since the thread most probably woke up due to
            // an event
            isr_event_t *event = __lock_event(thread);
            // push event data to the entry task
            thread->next = (void *)((entry_task_t)thread->entry)(buf,(void *)event);
            // the event should be released (deleted)
            thread->state = TASK_RELEASE_EVENT;
        }
        else{
            thread->next = (void *)(((task_t)thread->next)(buf));
            thread->state = TASK_FINISHED;
            break;
        }
    case TASK_RELEASE_EVENT:
        // release any event which is popped by the task
        __release_event(thread);
        thread->state = TASK_FINISHED;
    case TASK_FINISHED:
        //switch stack index to commit changes
        thread->buffer._idx = thread->buffer.idx ^ 1;
        thread->state = TASK_COMMIT;
    case TASK_COMMIT:
        // copy the real index from temporary index
        thread->buffer.idx = thread->buffer._idx;
        // Task execution finished. Check if the whole tasks are executed (thread finished)
        if (thread->next == NULL)
        {
            __disable_interrupt();
            // check if there are any pending events
            if(!__has_events(thread)){
                // suspend the thread if there are no pending events
                __stop_thread(thread);
            }
            else{
                // thread re-starts from the entry task
                thread->next = thread->entry;
                // ready to execute tasks again.
                thread->state = TASK_READY;
            }
            __enable_interrupt();
        }
        else{
            // ready to execute successive tasks
            thread->state = TASK_READY;
        }
    }
}

