//cm_acc Header
#include "app_intrinsics.h"
#include "ink.h"
#include "adxl345_i2c_lib.h"
#include "avg_filter.h"

#define ACC_SAMPLES 3


inline uint8_t valid_signal(void){

  // P2OUT |= BIT2;
#ifndef EMULATE
  //__disable_interrupt();
  uint8_t acc_data[NUM_BYTES_RX];
  //get samples 
  i2c_init();
  i2c_write(ADXL_345 , ADXL_CONF_REG , 0x00);
  i2c_write(ADXL_345, ADXL_CONF_REG, 0x10);
  i2c_write(ADXL_345, ADXL_CONF_REG, 0x08);
  
  uint8_t collected = 0;
  uint16_t tmp,current;

  while(ACC_SAMPLES-collected){
        collected++;
        i2c_read_multi(ADXL_345, READ_REG, NUM_BYTES_RX, &acc_data);
        tmp = (((int16_t)acc_data[5]) << 8) | acc_data[4];
        current = medfilter(tmp);
        __delay_cycles(200);
  }
#else

  __delay_cycles(FILTER_DURATION);
  uint16_t current = 1;
  
#endif
  //__enable_interrupt();
  // P2OUT &= ~BIT2;
  
  if (current < BOUND ) return 1;
  else return 0;
}

void setup_mcu(){

  //TODO: add this to __user_mcu_init
  //Led
  P1OUT &= ~BIT0;                           // Clear P1.0 output latch
  P1DIR |= BIT0;                            // For LED on P1.0
  P4OUT &= ~BIT6;                           // Clear P4.6 output latch
  P4DIR |= BIT6;                            // For LED on P4.6

  PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
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
