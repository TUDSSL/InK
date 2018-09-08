// This file is part of InK.
// 
// author = "dpatoukas" 
// maintainer = "dpatoukas"
// email = "dpatoukas@gmail.com" 
//  
// copyright = "Copyright 2018 Delft University of Technology" 
// license = "LGPL" 
// version = "3.0" 
// status = "Production"
//
// 
// InK is free software: you can redistribute it and/or modify
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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.#ifndef _CLK_H_
#define _CLK_H_
#include "ink.h"

//timer controls
#ifdef TIMERS_ON
#define UART_TIME 44
#endif

#define RTC 0x53
#define RTC_LOW_REG 0x00
#define RTC_CONF_REG 0x01
#define NUM_BYTES_RX 2
/* sets a one-shot timer using Timer A2 */
void set_timer(uint16_t ticks);

/*
* start the __get_time interface
*/
void __get_time_init();

/*
* stop the __get_time interface
*/
void __get_time_stop();

/*
* return current ticks + number of times the current timer has overflown
*/
uint32_t __get_time();

	
void __setup_rtc();

void __setup_clock();

/* stop timer A2 */
void stop_timer();

void timerA2_init();
void timerA2_set_CCR0(uint16_t ticks);


void set_timer_wkup(uint16_t ticks);

void set_timer_xpr(uint16_t ticks);

void set_timer_pdc(uint16_t ticks);

void stop_timer_xpr();
#endif
