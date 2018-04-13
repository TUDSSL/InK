/*
 * persistentqueue.h
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
