/*
 * isrmanager.h
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
