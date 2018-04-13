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

