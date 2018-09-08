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
// along with this program.  If not, see <http://www.gnu.org/licenses/>./*
 
 
/* msp430fr5969.h
 *
 *  Created on: 15 Feb 2018
 *
 */

#ifndef MCU_MSP430_MSP430FR5969_H_
#define MCU_MSP430_MSP430FR5969_H_

#include <msp430.h>

void __mcu_init();

// #define LED1_WISP BIT0
// #define LED1_WISP_OUT P4OUT
// #define LED1_WISP_DIR P4DIR

#define LED1 BIT6
#define LED1_OUT P4OUT
#define LED1_DIR P4DIR

#define LED2 BIT0
#define LED2_OUT P1OUT
#define LED2_DIR P1DIR

#define __port_init(port,bit) \
        P##port##OUT &= ~BIT##bit; \
        P##port##DIR |= BIT##bit; \
        P##port##OUT &= ~BIT##bit;

#define __port_on(port,bit) P##port##OUT |= BIT##bit;
#define __port_off(port,bit) P##port##OUT &= ~BIT##bit;
#define __port_toggle(port,bit) P##port##OUT ^= BIT##bit;

/* Macros for easy access to the LEDs on the evaulation board. */
#define __led_init(led) \
        led##_OUT &= ~led; \
        led##_DIR |= led; \
        led##_OUT &= ~led;

#define __led_on(led) led##_OUT |= led;
#define __led_off(led) led##_OUT &= ~led;
#define __led_toggle(led) led##_OUT ^= led;

/* defines an interrupt routine which does not allow nested interrupts */
#define _interrupt(signame)                                                \
  __attribute__((interrupt(signame))) void isr_##signame()

/* defines an interrupt routine which allows nested interrupts */
#define __nested_interrupt(signame)                                         \
  __attribute__(                                                               \
      (interrupt(signame), signal, wakeup)) void nested_isr_##signame()

/* puts microcontroller into sleep mode */
#define __mcu_sleep() __bis_SR_register(LPM3_bits | GIE)//__no_operation();



/* Disable the GPIO power-on default high-impedance mode to activate
 previously configured port settings.*/
#define __mcu_gpio_unlock() \
	PM5CTL0 &= ~LOCKLPM5


#endif /* MCU_MSP430_MSP430FR5969_H_ */
