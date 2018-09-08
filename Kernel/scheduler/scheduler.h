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
