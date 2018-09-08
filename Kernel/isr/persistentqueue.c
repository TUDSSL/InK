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
/* persistentqueue.c
 *
 *  Created on: 19 Feb 2018
 *
 */

#include "ink.h"
#include "persistentqueue.h"

// inits the persistent queue
void __perqueue_init(per_queue_t *queue){
    queue->_head ^= queue->_head;
    queue->_tail ^= queue->_tail;
    queue->__head ^= queue->__head;
    queue->__tail ^= queue->__tail;
}

uint8_t __perqueue_is_empty(per_queue_t *queue){
    // if there is no event in the persistent queue
    if (queue->_head == queue->_tail) {
        return 1;
    }

    return 0;
}

uint8_t __perqueue_is_full(per_queue_t *queue){
    // use the temporary variable to calculate the next slot for the tail
    queue->__tail = queue->_tail + 1;
    if (queue->__tail == MAX_QUEUE_ITEMS){
        queue->__tail = 0;
    }

    // check if all slots are full
    if (queue->__tail == queue->_head) {
        return 1;
    }

    return 0;
}

uint8_t __perqueue_push(per_queue_t *queue,void *item){
    // use the temporary variable to calculate the next slot for the tail
    queue->__tail = queue->_tail + 1;
    if (queue->__tail == MAX_QUEUE_ITEMS)
        queue->__tail = 0;
    // if all slots are full, no way to register the interrupt! We loose one slot
    // with this check, but this allows us to update just one variable in the end
    // to insert the event: no need to keep track the number of inserted events!
    if (queue->__tail == queue->_head) {
        return 0;
    }

    // copy the event to the event queue but do not modify the tail
    queue->_items[queue->_tail] = item;

    return 1;
}

/* commit the operation on the queue */
void __perqueue_push_commit(per_queue_t *queue)
{
    // update the real tail of the queue in one step so that the item is inserted!
    queue->_tail = queue->__tail;
}

void *__perqueue_pop(per_queue_t *queue){
    // if there is no item in the persistent queue
    if (queue->_head == queue->_tail){
        return NULL;
    }

    // use the temporary variable to calculate the next slot for the tail
    queue->__head = queue->_head + 1;
    if (queue->__head == MAX_QUEUE_ITEMS)
        queue->__head = 0;

    return queue->_items[queue->_head];
}

// commit the operation on the queue
void __perqueue_pop_commit(per_queue_t *queue){
    // remove the item from the queue with one step!
    queue->_head = queue->__head;
}
