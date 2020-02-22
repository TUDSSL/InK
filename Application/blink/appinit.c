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

#define THREAD1 7
#define THREAD2 15

extern void thread1_init();
extern void thread2_init();

// this is the function that will be called only at initial boot by the runtime.
void __app_init(){
    thread1_init();
    thread2_init();
}

void __app_reboot(){
    // Configure GPIO
    P1OUT = 0;
    P4OUT = 0;
    P1DIR |= BIT0;
    P4DIR |= BIT6;

    P1OUT |= BIT1;                             // Pull-up resistor on P1.1
    P1REN |= BIT1;                             // Select pull-up mode for P1.1
    P1DIR &= ~BIT1;
    P1IES |= BIT1;                             // P1.1 Hi/Lo edge
    P1IFG = 0;                                // Clear all P1 interrupt flags
    P1IE |= BIT1;                              // P1.1 interrupt enabled

    P4OUT |= BIT5;                             // Pull-up resistor on P4.5
    P4REN |= BIT5;                             // Select pull-up mode for P4.5
    P4DIR &= ~BIT5;

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
}

_interrupt(PORT1_VECTOR)
{
    P1IFG &= ~BIT1;                           // Clear P1.1 IFG
    __SIGNAL((P4IN & BIT5) ? THREAD2 : THREAD1);
    LPM3_EXIT;
}

_interrupt(RTC_VECTOR)
{
    RTCPS1CTL &= ~RT1PSIE;  // disable RTC Prescale Timer 1 Interrupt
    LPM3_EXIT;
}
