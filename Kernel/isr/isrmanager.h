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
 /* isrmanager.h
 *
 *  Created on: 19 Feb 2018
 *
 */

#ifndef COMM_ISRMANAGER_H_
#define COMM_ISRMANAGER_H_

typedef struct {
    void *data;             // the data should be allocated in FRAM by the caller
    uint32_t size;          // size of the data buffer
    uint32_t timestamp;     // timestamp of the event
}isr_event_t;

// should be called from the first boot
void __events_boot_init();

// should be called at each reboot
void __events_commit();

// check if the thread has pending events
inline uint8_t __has_events(thread_t *thread);

// We first pop an event and lock it
inline isr_event_t *__lock_event(thread_t *thread);

// Events should be released after lock
inline void __release_event(thread_t *thread);

// check if the thread event slots are not full
inline uint8_t __event_buffer_full_ISR(thread_t *thread);

// signaling of the events from ISRs
inline void __event_signal_ISR(thread_t *thread, isr_event_t *event);

#endif /* COMM_ISRMANAGER_H_ */
