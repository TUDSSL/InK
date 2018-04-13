/*
 * task.h
 *
 *  Created on: 12 Feb 2018
 *
 */

#ifndef TASK_H_
#define TASK_H_

#define TASK(name)  static void *name(void *__buffer)

#define ENTRY_TASK(name)  static void *name(void *__buffer,isr_event_t *__event)

// reads the value from the original stack
#define __GET(x) ( (FRAM_data_t *)__buffer)->x

// returns the address of the variable
#define __GET_ADDR(x) &( (FRAM_data_t *)__buffer-)->x

// writes the value to the temporary stack
#define __SET(x,val) ( (FRAM_data_t *)__buffer)->x = val

// creates a thread
#define __CREATE(priority,entry)  \
        __create_thread(priority,(void *)entry,(void *)&__persistent_vars[0],(void *)&__persistent_vars[1],sizeof(FRAM_data_t))

// puts the thread state into ACTIVE
#define __SIGNAL(priority) \
        __disable_interrupt();  \
        __start_thread(__get_thread(priority)); \
        __enable_interrupt()

// event related information
#define __EVENT __event
#define __EVENT_DATA __event->data
#define __EVENT_DATALEN __event->size
#define __EVENT_TIME __event->timestamp

#endif /* TASK_H_ */
