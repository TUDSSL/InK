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
 /* persistentqueue.h
 *
 *  Created on: 19 Feb 2018
 *
 */

#ifndef COMM_PERSISTENTQUEUE_H_
#define COMM_PERSISTENTQUEUE_H_

#define MAX_QUEUE_ITEMS MAX_PENDING_INTERRUPTS

// a general persistent queue structure that works
// via two-phase commit
typedef struct {
    void *_items[MAX_QUEUE_ITEMS];
    volatile uint8_t _head;
    volatile uint8_t _tail;

    // for the consistent operations on the list
    volatile uint8_t __head;
    volatile uint8_t __tail;
} per_queue_t;

void __perqueue_init(per_queue_t *queue);
uint8_t __perqueue_is_empty(per_queue_t *queue);
uint8_t __perqueue_is_full(per_queue_t *queue);

void *__perqueue_pop(per_queue_t *queue);
void __perqueue_pop_commit(per_queue_t *queue);

uint8_t __perqueue_push(per_queue_t *queue,void *item);
void __perqueue_push_commit(per_queue_t *queue);

#endif /* COMM_PERSISTENTQUEUE_H_ */
