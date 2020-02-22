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
// InK is free software: you ca n redistribute it and/or modify
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

#define THREAD2 15

// define task-shared persistent variables.
__shared(
    uint8_t last_out;
    uint8_t count;
)

TASK(t_init);
TASK(t_rtc);
TASK(t_update);

// called at the very first boot
void thread2_init(){
    // create a thread with priority and entry task t_init
    __CREATE(THREAD2,t_init);
//    __SIGNAL(THREAD2);
}

TASK(t_init){
    __SET(last_out, 0);
    __SET(count, 0);

    // next task
    return t_rtc;
}

TASK(t_rtc){
    // Configure RTC
    RTCCTL01 = RTCHOLD;
    RTCPS = 0;
    RTCPS1CTL = RT1IP_6 | RT1PSIE;
    RTCCTL01 &= ~(RTCHOLD);                 // Start RTC

    LPM3;

    RTCCTL01 = RTCHOLD;
    // next task
    return t_update;
}

TASK(t_update){
    uint8_t l_count = __GET(count) + 1;
    uint8_t out = __GET(last_out) ^ BIT0;
    P1OUT = BIT1 | out;  // always keep pull-up resistor on P1.1
    __SET(last_out, out);

    if(l_count < 8){
        __SET(count, l_count);
        return t_rtc;
    }
    else{
        return NULL;
    }
}
