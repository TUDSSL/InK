// This file is part of InK.
// 
// author = "dpatoukas " 
// maintainer = "dpatoukas "
// email = "dpatoukas@gmail.com" 
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

#include <msp430.h> 
#include <stdint.h>

//#define ACC
//#define SIG
//#define STABLE 1505
//uint16_t data[] ={};

#define STAY_UP 13
//Dataset size
#define DATASET 999
//Logic Analyzer trigger

#define TRIGGER
//#define SEMI_STABLE

//UART communication codes
#define ACC 13
#define SIGNAL 52
#define FREE 166
#define STOP 26

//Boundaries in "amplitude" for raising a pin 
#define BOUND_SINGAL 1900
#define BOUND_ACC 900

//Import dataset
#ifdef ACC
    static const uint16_t data_acc[] = {
    #include "accPattern.txt"
    };
#endif
#ifdef SIGNAL
    static const uint16_t data_singal[] = {
    #include  "signalPattern.txt"
    };
#elif STABLE
    uint16_t data[DATASET+1] ;
#endif
#ifdef SEMI_STABLE
    uint16_t data_acc[] = {1505,1457,1505,1457,1505,1457,1505,1457,1505,1457,1505,1457,1505,1457,1505,1457,1505,1457,1505,1457};
#endif


uint16_t RXData = 0;
static uint8_t flag = FREE;

//Boundaries for signal detection
static const uint16_t bound_acc = BOUND_ACC;
static const uint16_t bound_signal = BOUND_SINGAL;

//flags for uart communication
static uint16_t counted	 = 0;

//forward declarations
void setup_mcu();
void uartSend(unsigned char *pucData, uint8_t ucLength); 
void interrupt_pin_raise(uint16_t data, uint8_t cur_flag);
void led_state();
void blink_on();
void blink_off();

int main(void)
{
  uint16_t i = 0;
	setup_mcu();
#ifdef TRIGGER
    P3OUT |= BIT5;
    P3OUT &= ~BIT5;
#endif
	
	//enable interrupts;
	__bis_SR_register(GIE);
	 while (1)
	{
		//raise a pin if the boundary has been exceeded
	    //P1OUT |= BIT0;

		interrupt_pin_raise(i,flag);
		// if (flag == ACC)
		// {
		// 	uartSend(&data_acc[i], 2);

		// }
		// else if (flag == SIGNAL)
		// {
		// 	uartSend(&data_singal[i],2);
		// }
		// else if(flag == STOP)
		// {
		//  	P1OUT |= BIT0 | BIT1;
		// }
		//__delay_cycles(1000);

		if (++i > DATASET)
		{
			i = 0;
		}
	}
		
}

void interrupt_pin_raise(uint16_t data, uint8_t cur_flag){


	if (counted == 5)
	{
		counted = 0;
		if (data_acc[data] >= bound_acc)
		{	
#ifndef STAY_UP
			P3OUT |= BIT4 | BIT5;
			P3OUT &= ~(BIT4 | BIT5);
			P1OUT |= BIT1;
#else 
			P3OUT |= BIT4 | BIT5;
			__delay_cycles(2000);
			P3OUT &= ~(BIT4 | BIT5);
			P1OUT |= BIT1;		
#endif
		}
		else
		{
			P3OUT &= ~(BIT4 | BIT5);
	    	P1OUT &= ~BIT1;
		}
	}

	if (data_singal[data] >= bound_signal)
	{
#ifndef STAY_UP
		P1OUT |= BIT0;
		P3OUT |= BIT3 | BIT6;
		P3OUT &= ~(BIT3 | BIT6);
#else 
		P3OUT |= BIT3 | BIT6;
		__delay_cycles(2000);
		P3OUT &= ~(BIT3 | BIT6);
		P1OUT |= BIT0;
#endif
	}
	else
	{
		P1OUT &= ~BIT0;
		P3OUT &= ~(BIT3 | BIT6);
	}
	__delay_cycles(200000);
	__delay_cycles(200000);
	__delay_cycles(200000);
	counted++;
}

void blink_off(){
	//debug led 
	P1OUT &= ~(BIT0 | BIT1);
}

void blink_on(){
	//debug led  
}

void led_state(){
	
	//debug blink ;)
	P1OUT &= ~BIT1;
	P1OUT |=  BIT0;

}
//void uartSend(unsigned char *pucData, uint8_t ucLength)
//{
//	while(ucLength)
//	{
//	    P1OUT ^= BIT1;
//		// Wait for TX buffer to be ready for new data
//		while(!(UCA3IFG & UCTXIFG));
//		// Push data to TX buffer
//		UCA3TXBUF = *pucData;
//
//		// Update variables
//		ucLength--;
//		pucData++;
//	}
//
//	// Wait until the last byte is completely sent
//	while(UCA3STATW & UCBUSY);
//}


//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//#pragma vector=EUSCI_A3_VECTOR
//__interrupt void USCI_A3_ISR(void)
//#elif defined(__GNUC__)
//void __attribute__ ((interrupt(EUSCI_A3_VECTOR))) USCI_A3_ISR (void)
//#else
//#error Compiler not supported!
//#endif
//{
//  switch(__even_in_range(UCA3IV, USCI_UART_UCTXCPTIFG))
//  {
//    case USCI_NONE: break;
//    case USCI_UART_UCRXIFG:
//		RXData = UCA3RXBUF;                   // Read buffer
//
//		switch((uint8_t) RXData){
//			case ACC:
//				flag = ACC;
//				break;
//			case SIGNAL:
//				flag = SIGNAL;
//				break;
//			case STOP:
//				flag = STOP;
//				break;
//			case FREE :
//				flag = FREE;
//				break;
//			default:
//				//flag = 404;
//				break;
//		}
//		//P1OUT = BIT0;                      // If incorrect turn on P1.0
//		//__delay_cycles(50);
//		//P1OUT ^= BIT0;                      // If incorrect turn on P1.0
//      break;
//    case USCI_UART_UCTXIFG: break;
//    case USCI_UART_UCSTTIFG: break;
//    case USCI_UART_UCTXCPTIFG: break;
//  }
//}

void setup_mcu(){

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	// Configure GPIO
	P1OUT &= ~BIT0;                           // Clear P1.0 output latch
	P1DIR |= BIT0;                            // For LED on P1.0

	P1OUT &= ~BIT1;                           // Clear P1.0 output latch
	P1DIR |= BIT1;                            // For LED on P1.0

	P3OUT &= ~(BIT4|BIT5|BIT6| BIT3);		  //Set up pin 3.4-3.5 for accel notify 
   	P3DIR |= BIT4 | BIT5 | BIT6 | BIT3;		  //Set up pin 3.3-3.6 for signal2 notify

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings


//#ifdef BAUD_115
//    // Disable the GPIO power-on default high-impedance mode to activate
//    // previously configured port settings
//    PM5CTL0 &= ~LOCKLPM5;
//
//    // Configure USCI_A3 for UART mode
//    UCA3CTLW0 = UCSWRST;                    // Put eUSCI in reset
//    UCA3CTLW0 |= UCSSEL__SMCLK;             // CLK = SMCLK
//    UCA3BRW = 8;                            // 1000000/115200 = 8.68
//    UCA3MCTLW = 0xD600;                     // 1000000/115200 - INT(1000000/115200)=0.68
//                                            // UCBRSx value = 0xD6 (See UG)
//    UCA3CTLW0 &= ~UCSWRST;                  // release from reset
//    UCA3IE |= UCRXIE;                       // Enable USCI_A3 RX interrupt
//#else
    // Startup clock system with max DCO setting ~8MHz
    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;           // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers
    CSCTL0_H = 0;                           // Lock CS registers
//
//        // Configure USCI_A3 for UART mode
//    UCA3CTLW0 = UCSWRST;                    // Put eUSCI in reset
//    UCA3CTLW0 |= UCSSEL__SMCLK;             // CLK = SMCLK
//    // Baud Rate calculation
//    // 8000000/(16*9600) = 52.083
//    // Fractional portion = 0.083
//    // User's Guide Table 21-4: UCBRSx = 0x04
//    // UCBRFx = int ( (52.083-52)*16) = 1
//    UCA3BRW = 52;                           // 8000000/16/9600
//    UCA3MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
//    UCA3CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
//    UCA3IE |= UCRXIE;                       // Enable USCI_A3 RX interrupt
//#endif

}
