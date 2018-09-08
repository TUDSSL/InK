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
 * priority.c
 *
 *  Created on: 14 Feb 2018
 *
 */

#include "ink.h"
#include "priority.h"

// inspired by UCOS-II source code

// bit mask table
static const uint8_t PriorityTbl[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
                                       0x80 };

static const uint8_t UnmapTbl[] = { 0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1,
                                    0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0,
                                    1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2,
                                    0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0,
                                    2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1,
                                    0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0,
                                    1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3,
                                    0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0,
                                    3, 0, 1, 0, 2, 0, 1, 0, 7, 0, 1, 0, 2, 0, 1,
                                    0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0,
                                    1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2,
                                    0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0,
                                    2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1,
                                    0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0,
                                    1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
                                    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1,
                                    0, };

// Priority is presented by 8 bits. Since the queue supports 64 elements,
// 6 bits are enough. Right-most 3 bits represents holds the index of the
// item, left-most 3 bits represent priority.
//
//  xxss syyy ( 8 bit priority )
//    || ||||___
//    || |||____  index
//    || ||_____
//    || |__
//    ||____  priority level
//    |_____

#define PRIORITY_LEVEL_MASK 0x38
#define PRIORITY_BIT_MASK 0x07



// each of the following queue functions are idempotent.

void __priority_init(priority_t *p){
    uint8_t i;

    for (i = NUM_PRIORITY_LEVEL; i > 0; i--){
        p->_readyTable[i] = 0x0;
    }

    p->_readyLevel = 0x0;
}

// inserts the task with given priority and adjusts the priority
// levels. This is an idempotent function! (restartable)
void __priority_insert(uint8_t priority,priority_t *p){
    uint8_t priorityLevel;
    uint8_t priorityBits;

    priorityLevel = (priority & PRIORITY_LEVEL_MASK) >> 3;
    priorityBits = priority & PRIORITY_BIT_MASK;

    p->_readyLevel|= PriorityTbl[priorityLevel];
    p->_readyTable[priorityLevel] |= PriorityTbl[priorityBits];
}

// removes the task with given priority and adjusts the priority
// levels. This is an idempotent function! (restartable)
void __priority_remove(uint8_t priority,priority_t *p){
    uint8_t priorityLevel;
    uint8_t priorityBits;

    priorityLevel = (priority & PRIORITY_LEVEL_MASK)>>3;
    priorityBits = priority & PRIORITY_BIT_MASK;

    p->_readyTable[priorityLevel] &= ~PriorityTbl[priorityBits];

    if(!p->_readyTable[priorityLevel])    {
        p->_readyLevel &= ~PriorityTbl[priorityLevel];
    }
}

// returns the task with highest priority
uint8_t __priority_highest(priority_t *p){
    uint8_t priorityLevel;
    uint8_t priorityBits;

    priorityLevel = UnmapTbl[p->_readyLevel];
    priorityBits = p->_readyTable[priorityLevel];

    return (priorityLevel << 3) | (UnmapTbl[priorityBits]);
}

