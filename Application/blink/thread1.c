// This file is part of InK.
// 
// author = "Philo Tang"
// maintainer = "Philo Tang"
// email = "H.TANG-1@student.tudelft.nl"
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

#include "ink.h"

#define THREAD1 15

// define task-shared persistent variables.
__shared(
    uint8_t last_out;
    uint8_t dummy;  // to avoid alignment 2 error
)

TASK(t_task);
// Reserved for future use
//TASK(t_end);

// called at the very first boot
void thread1_init(){
    // create a thread with priority 15 and entry task t_init
    __CREATE(THREAD1,t_task);
    __SIGNAL(THREAD1);
}

__app_reboot(){
    P1DIR |= BIT0;

    PJSEL0 = BIT4 | BIT5;                   // Initialize LFXT pins

    // Configure LFXT 32kHz crystal
    CSCTL0_H = CSKEY >> 8;                  // Unlock CS registers
    CSCTL4 &= ~LFXTOFF;                     // Enable LFXT
    do
    {
      CSCTL5 &= ~LFXTOFFG;                  // Clear LFXT fault flag
      SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);              // Test oscillator fault flag
    CSCTL0_H = 0;                           // Lock CS registers

    // Configure RTC_C
    RTCCTL01 = RTCHOLD;
    RTCPS = 0;
    RTCPS1CTL = RT1IP_6 | RT1PSIE;
    RTCCTL01 &= ~(RTCHOLD);                 // Start RTC
}

TASK(t_task){
    uint8_t out = __GET(last_out);
    out ^= BIT0;
    P1OUT = out;
    __SET(last_out, out);

    // next task
    return NULL;
}

// Reserved for future use
//TASK(t_end){
//    return t_init;
//}

_interrupt(RTC_VECTOR)
{
    switch(__even_in_range(RTCIV, RTCIV_RT1PSIFG))
    {
        case RTCIV_RT1PSIFG:
            __SIGNAL(THREAD1);
            break;
        default: break;
    }
    LPM3_EXIT;
}
