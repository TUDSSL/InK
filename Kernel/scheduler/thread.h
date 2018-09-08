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
 * thread.h
 *
 *  Created on: 12 Feb 2018
 *
 */

#ifndef THREAD_H_
#define THREAD_H_

// the state of the threads
typedef enum {TASK_READY = 1, TASK_RELEASE_EVENT =2, TASK_FINISHED = 4, TASK_COMMIT = 8,THREAD_STOPPED = 16} state_t;

// each thread will hold the double buffer for the variables
// shared by the tasks it is encapsulating.
typedef struct {
    void *buf[2];           // holds original and temporary stack pointers
    volatile uint8_t idx;   // index of the original buffer
    volatile uint8_t _idx;  // index of the new buffer
    uint16_t size;          // sizes of the buffers
}buffer_t;

// the task definition (single C function)
// the parameter param will be passed by the run-time
// and it holds the thread structure defined below.
typedef void* (*task_t) (buffer_t *);

// the entry task should take event data as an argument.
typedef void* (*entry_task_t) (buffer_t *,void *event);

// the main thread structure that holds all necessary info
// to execute the computation represented by the wired
// tasks
typedef struct {
    uint8_t priority;       // thread priority (unique)
    volatile state_t state; // thread state
    void *entry;            // the first task to be executed
    void *next;             // the current task to be executed
    buffer_t buffer;        // holds task shared persistent variables
    uint16_t sing_timer;// holds the time when the thread will be executed
    uint16_t pdc_timer; // holds the time for "periodic" execution of the thread
    uint16_t expr_timer; // hold the expiration time of the thread from time of completion
    uint16_t pdc_period; // holds the current period 
}thread_t;

// allocates a double buffer for the persistent variables in FRAM
#define __shared(...)   \
        typedef struct {    \
            __VA_ARGS__     \
        } FRAM_data_t  __attribute__ ((aligned (2)));    \
        static __nv FRAM_data_t __persistent_vars[2];

// runs one task inside the current thread.
void __tick(thread_t *thread);

#endif /* THREAD_H_ */
