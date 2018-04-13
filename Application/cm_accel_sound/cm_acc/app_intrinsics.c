//cm_acc Header
#include "app_intrinsics.h"
#include "ink.h"
#include "adxl345_i2c_lib.h"
#include "avg_filter.h"




inline uint8_t valid_signal(void){

  uint8_t acc_data[NUM_BYTES_RX];
  //get samples 
  uint8_t collected = 0;
  uint16_t tmp,current;

  while(ACC_SAMPLES-collected){
        collected++;
        i2c_read_multi(ADXL_345, READ_REG, NUM_BYTES_RX, &acc_data);
        tmp = (((int16_t)acc_data[5]) << 8) | acc_data[4];
        current = medfilter(tmp);
        __delay_cycles(200);
  }
  
  if (current < BOUND ) return 1;
  else return 0;
}

void setup_mcu(){

  // Configure GPIO
  //P2SEL1 |= BIT5 | BIT6;                    // USCI_A0 UART operation
  // P2SEL0 &= ~(BIT5 | BIT6);

  //Led
  P1OUT &= ~BIT0;                           // Clear P1.0 output latch
  P1DIR |= BIT0;                            // For LED on P1.0
  P4OUT &= ~BIT6;                           // Clear P4.6 output latch
  P4DIR |= BIT6;                            // For LED on P4.6

  PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                          // to activate previously configured port settings
#ifdef UART
#ifdef BAUD_115
   // Configure USCI_A0 for UART mode
  UCA1CTLW0 = UCSWRST;                      // Put eUSCI in reset
  UCA1CTL1 |= UCSSEL__SMCLK;                // CLK = SMCLK
  UCA1BR0 = 8;                              // 1000000/115200 = 8.68
  UCA1MCTLW = 0xD600;                       // 1000000/115200 - INT(1000000/115200)=0.68
                                          // UCBRSx value = 0xD6 (See UG)
  UCA1BR1 = 0;
  UCA1CTL1 &= ~UCSWRST;                     // release from reset
  UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
#else
  // Startup clock system with max DCO setting ~8MHz
  CSCTL0_H = CSKEY >> 8;                    // Unlock clock registers
  CSCTL1 = DCOFSEL_3 | DCORSEL;             // Set DCO to 8MHz
  CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
  CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
  CSCTL0_H = 0;                             // Lock CS registers

  // Configure USCI_A0 for UART mode
  UCA1CTLW0 = UCSWRST;                      // Put eUSCI in reset
  UCA1CTLW0 |= UCSSEL__SMCLK;               // CLK = SMCLK
  // Baud Rate calculation
  // 8000000/(16*9600) = 52.083
  // Fractional portion = 0.083
  // User's Guide Table 21-4: UCBRSx = 0x04
  // UCBRFx = int ( (52.083-52)*16) = 1
  UCA1BR0 = 52;                             // 8000000/16/9600
  UCA1BR1 = 0x00;
  UCA1MCTLW |= UCOS16 | UCBRF_1;
  UCA1CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
  UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

#endif
#endif
}

/**
 * Configure ADC for microphone sampling
 */
void ADC_config()
{
    //TODO:free 1.3
    // Pin P1.3 set for Ternary Module Function (which includes A3)
    P1SEL0 |= BIT3;
    P1SEL1 |= BIT3;
 
    // Clear ENC bit to allow register settings
    ADC12CTL0 &= ~ADC12ENC;
 
    // Sample-and-hold source select
    //
    // 000 -> ADC12SC bit (default)
    // ADC12CTL1 &= ~(ADC12SHS0 | ADC12SHS1 | ADC12SHS2);
 
    // Clock source select
    //
    // source: MCLK (DCO, 1 MHz)
    // pre-divider: 4
    // divider: 4
    ADC12CTL1 |= ADC12SSEL_2 | ADC12PDIV_1 | ADC12DIV_3;
 
    // sampling period select for MEM0: 64 clock cycles (*)
    // multiple sample and conversion: enabled
    // ADC module ON
    ADC12CTL0 |= ADC12SHT0_2 | ADC12MSC | ADC12ON;
    // (*) freq = MCLK / (ADC12PDIV_0 * ADC12DIV_0 * ADC12SHT0_4)
    //          = 1000000 / (4 * 4 * 16)
    //          = 3906 Hz
 
    // conversion sequence mode: repeat-single-channel
    // pulse-mode select: SAMPCON signal is sourced from the sampling timer
    ADC12CTL1 |= ADC12CONSEQ_2 | ADC12SHP;
 
    // resolution: 12 bit
    // data format: right-aligned, unsigned
    ADC12CTL2 |= ADC12RES__12BIT;
    ADC12CTL2 &= ~ADC12DF;
 
    // conversion start address: MEM0
    ADC12CTL3 |= ADC12CSTARTADD_0;
 
    // MEM0 control register
    // reference select: VR+ = AVCC (3V), VR- = AVSS (0V)
    // input channel select: A3
    ADC12MCTL0 |= ADC12VRSEL_0 | ADC12INCH_3;
 
    // Clear interrupt for MEM0
    ADC12IFGR0 &= ~ADC12IFG0;
 
    // Enable interrupt for (only) MEM0
    ADC12IER0 = ADC12IE0;
 
    // Trigger first conversion (Enable conversion and Start conversion)
    ADC12CTL0 |= ADC12ENC | ADC12SC;
}
