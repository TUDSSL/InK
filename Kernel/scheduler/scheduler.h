/*
 * scheduler.h
 *
 *  Created on: 14 Feb 2018
 *
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "task.h"
#include "thread.h"

#define MAX_THREADS NUM_PRIORITIES

void __scheduler_boot_init();
void __scheduler_run();
void __create_thread(uint8_t priority, void *entry, void *data_org,
                    void *data_temp, uint16_t size);

// restart thread
inline void __start_thread(thread_t *thread);

// stop thread
inline void __stop_thread(thread_t *thread);

//evict thread 
void __evict_thread(thread_t *thread);

// priority to thread conversion
inline thread_t *__get_thread(uint8_t priority);


//timer needed functions
void __set_sing_timer(thread_t *thread,uint16_t timing);

void __set_xpr_timer(thread_t *thread,uint16_t timing);

void __set_pdc_timer(thread_t *thread, uint16_t timing);

void __set_pdc_period(thread_t *thread,uint16_t period);

uint16_t __get_pdc_timer(thread_t *thread);

uint16_t __get_pdc_period(thread_t *thread);
#endif /* SCHEDULER_H_ */
