/*
 * priority.h
 *
 *  Created on: 14 Feb 2018
 *
 */

#ifndef PRIORITY_H_
#define PRIORITY_H_

#define NUM_PRIORITY_LEVEL 8
#define NUM_PRIORITIES 64

typedef struct {
    volatile uint8_t _readyTable[NUM_PRIORITY_LEVEL];
    volatile uint8_t _readyLevel;
}priority_t;

// priority queue operations
void __priority_init(priority_t *p);
void __priority_insert(uint8_t priority,priority_t *p);
void __priority_remove(uint8_t priority,priority_t *p);
uint8_t __priority_highest(priority_t *p);

#endif /* PRIORITY_H_ */
