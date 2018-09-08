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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.//clk.c

#include "clk.h"
#include "ink.h"


__nv uint32_t current_ticks;

//RTC protocol helper infrastructure 
//**************************************************//

//in case of no rtc a timer through UART can be deployed
//for developing or debugging 
#ifdef UART_TIME
static const uint8_t flag_start = UART_TIME;

#define QUEUE_SIZE 5
typedef struct {
    uint8_t Data[QUEUE_SIZE];
    uint16_t first,last;
    uint16_t count;
} queue;

void init_queue(queue *q){

  q->first = 0;
  q->last = QUEUE_SIZE - 1;
  q->count = 0;
}

void enqueue(queue *q,char x){

  q->last = (q->last + 1) % QUEUE_SIZE;
  q->Data[ q->last ] = x;
  q->count += 1;
}

char dequeue(queue *q){

  char x = q->Data[ q->first ];
  q->first = (q->first + 1) % QUEUE_SIZE;
  q->count -= 1;
  return x;
}
queue rx_queue;
unsigned char rx_buff;


void tx_data(uint8_t flag){

  uint8_t *p;

  //transmit code
  p = &flag;
  UCA3TXBUF = *p; // Load data onto buffer
  while(!(UCA3IFG & UCTXIFG));
}
#elif RTC_ON

void i2c_init(){

    P1SEL1 |= BIT6 | BIT7;                  // configure I2C pins
    P1SEL0 &= ~(BIT6 | BIT6);               // configure I2C pins

    // I2C default uses SMCLK
    UCB0CTL1 |= UCSWRST;                    // put eUSCI_B in reset state
    UCB0CTLW0 |= UCMODE_3 | UCMST | UCSYNC; // I2C, master, sync
    UCB0BRW = 0x000A;                       // baud rate = SMCLK / 10 = 100khz
    UCB0CTL1 &= ~UCSWRST;                   // eUSCI_B in operational state
}

void i2c_write(uint8_t slv_addr, uint8_t reg_addr, uint8_t data){

    __disable_interrupt();
    while(UCB0STAT & UCBBUSY);

    UCB0I2CSA = slv_addr;                   // set slave address
    UCB0CTLW0 |= UCTR | UCTXSTT;            // transmitter mode and START condition.

    while (UCB0CTLW0 & UCTXSTT);
    UCB0TXBUF = reg_addr;
    __delay_cycles(10);
    while(!(UCB0IFG & UCTXIFG0));
    UCB0TXBUF = data;
   __delay_cycles(10);
    while(!(UCB0IFG & UCTXIFG0));

    UCB0CTLW0 |= UCTXSTP;
    while(UCB0CTLW0 & UCTXSTP);             // wait for stop
    __enable_interrupt();
}

void i2c_read_multi(uint8_t slv_addr, uint8_t reg_addr, uint8_t l, uint8_t *arr){

    uint8_t i;
    __disable_interrupt();

    while(UCB0STAT & UCBBUSY);
    UCB0I2CSA = slv_addr;                   // set slave address
    UCB0CTLW0 |= UCTR | UCTXSTT;            // transmitter mode and START condition.

    while(UCB0CTLW0 & UCTXSTT);
    UCB0TXBUF = reg_addr;
    __delay_cycles(10);
    while(!(UCB0IFG & UCTXIFG0));

    UCB0CTLW0 &= ~UCTR;                     // receiver mode
    UCB0CTLW0 |= UCTXSTT;                   // START condition

    while(UCB0CTLW0 & UCTXSTT);             // make sure start has been cleared

    for (i = 0; i < l; i++) {

        if(i == l - 1){
            UCB0CTLW0 |= UCTXSTP;           // STOP condition
        }
        while(!(UCB0IFG & UCRXIFG0));
        arr[i] = UCB0RXBUF;
        __delay_cycles(10);
    }

    while(UCB0CTLW0 & UCTXSTP);
    __enable_interrupt();

}
#endif
//*************************************************//


void stop_timer()
{
    /* stop timer */
    TA2CCTL0 = 0x00;
    TA2CTL = 0;
}

/* stop timer A2 */
void timerA2_init()
{
    /* stop timer */
    TA2CCTL0 = 0x00;
    TA2CTL = 0;
}

/* sets a one-shot timer using Timer A2 */
void set_timer(uint16_t ticks)
{
    stop_timer();
    timerA2_set_CCR0(ticks);
}


void set_timer_wkup(uint16_t ticks){

    TA0CTL = 0;
    TA0CCR0 = ticks;
    TA0CCTL0 = CCIE;
    TA0CTL = TASSEL__SMCLK  | MC__UP | TACLR ;
}

void set_timer_xpr(uint16_t ticks){
    TA2CTL = 0;
    TA2CCR0 = ticks;
    TA2CCTL0 = CCIE;
    TA2CTL = TASSEL__ACLK  | MC__UP | TACLR ;

}


void stop_timer_xpr()
{
    /* stop timer */
    TA2CCTL0 = 0x00;
    TA2CTL = 0;
}

void set_timer_pdc(uint16_t ticks){
    TA1CTL = 0;
    TA1CCR0 = ticks;
    TA1CCTL0 = CCIE;
    TA1CTL = TASSEL__SMCLK  | MC__UP | TACLR ;
}

void __setup_rtc()
{

#ifdef UART_TIME
    init_queue(&rx_queue);

    // Configure GPIO
    P6SEL1 &= ~(BIT0 | BIT1);
    P6SEL0 |= (BIT0 | BIT1);                // USCI_A3 UART operation

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Startup clock system with max DCO setting ~8MHz
    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;           // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers
    CSCTL0_H = 0;                           // Lock CS registers

    // Configure USCI_A3 for UART mode
    UCA3CTLW0 = UCSWRST;                    // Put eUSCI in reset
    UCA3CTLW0 |= UCSSEL__SMCLK;             // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA3BRW = 52;                           // 8000000/16/9600
    UCA3MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA3CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
    UCA3IE |= UCRXIE;                       // Enable USCI_A3 RX interrupt

#else
  //Use RTC I2C communication
  i2c_init();
  i2c_write(RTC , RTC_CONF_REG , 0x00);

#endif
}

void __setup_clock()
{
  // Clock System Setup
  CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
  CSCTL2 = SELA__VLOCLK;
  CSCTL3 = DIVA__1 ;     // Set all dividers to 1
  CSCTL0_H = 0;

}
uint32_t __get_rtc_time()
{
    uint32_t buff;
    uint32_t current_time;

#ifdef UART_TIME
    unsigned char *ptr;

    ptr = &buff;

    tx_data(flag_start);
    
    __enable_interrupt();

    while(rx_queue.count < 4)
    {
        if(rx_queue.count == 4)
        {
            *ptr = dequeue(&rx_queue);
            ptr++;
            *ptr = dequeue(&rx_queue);
            ptr++;
            *ptr = dequeue(&rx_queue);
            ptr++;
            *ptr = dequeue(&rx_queue);
            break;
        }
    }
    __disable_interrupt();
    UCA3CTLW0 = UCSWRST;                    // Put eUSCI in reset
    UCA3IE &= ~UCRXIE;                       // Enable USCI_A3 RX interrupt
    P6DIR = 0;
#else
    //time in ms, bound buy RTC accuracy
    i2c_read_multi(RTC, RTC_LOW_REG, NUM_BYTES_RX, &buff);
#endif
    current_time =(uint32_t) buff;

    return current_time;

}

#ifdef  UART_TIME
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=EUSCI_A3_VECTOR
__interrupt void USCI_A3_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(EUSCI_A3_VECTOR))) USCI_A3_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(UCA3IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:
            while(!(UCA3IFG&UCTXIFG));
            rx_buff = UCA3RXBUF;
            enqueue(&rx_queue , rx_buff);
            break;
        case USCI_UART_UCTXIFG: break;
        case USCI_UART_UCSTTIFG: break;
        case USCI_UART_UCTXCPTIFG: break;
        default: break;
    }
}
#endif
/*
* start the __get_time interface
*/
void __get_time_init()
{   
    //configure rtc interface
    __setup_rtc();

    //get the current time from rtc;
    current_ticks = __get_rtc_time();

    //stop TA1 timer
    TA3CTL = 0;
    //Start timer with ACLK source in CONTINUOUS mode CLEAR bit and enable interrupts;
    TA3CTL = TASSEL__ACLK  | MC__CONTINUOUS | TACLR | TAIE;

    __bis_SR_register(GIE);
}

/*
* stop the __get_time interface
*/
void __get_time_stop()
{
    TA3CTL = 0;
    TA3CCTL0 = 0;
}

/*
* return current ticks + number of times the current timer has overflown
* TASSEL_SMLK:1Mhz -> time will be reported in milliseconds (us)
*/
uint32_t __get_time()
{
   uint32_t tmp;

   TA3CTL ^= MC__CONTINUOUS ; //halt the timer

   tmp = TA3R;

   TA3CTL ^= MC__CONTINUOUS; //restart the timer



   return (tmp*10 + current_ticks);//current_ticks/1000;
}

/*
*__get_time() ISR
*increments time_count every 65553 ticks of the specified CLK
*/
_interrupt (TIMER3_A1_VECTOR)
{
        current_ticks += 0xA00AA;
       //current_ticks += 0x10000;
       TA3CTL &= ~TAIFG;
}


// /* the timer interrupt handler */
// __interrupt(TIMER2_A0_VECTOR)
// {
//    /* stop timer */
//    TA2CCTL0 = 0x00;
//    TA2CTL = 0;

//    event_flag = 1;
//    /* turn on CPU */
//    __bic_SR_register_on_exit(LPM3_bits);
// }
