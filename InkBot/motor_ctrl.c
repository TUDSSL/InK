
#include <msp430.h>
#include <stdint.h>
#include "motor_ctrl.h"

motor_calib curr_mc = {0};

void drv_init(motor_calib set_mc) {

    // set motor calibration
    curr_mc = set_mc;

    P2DIR |= BIT1;                            // Set P2.1 (UART_RX) to output
    P2OUT &= ~(BIT1);                         // Disable DRV8836

    //Motor driver config
    P1DIR |= BIT4;                            // Setup AIN1 (TB01) (M1 BLUE)
    P1SEL0 |= BIT4;                           // Option select timer output

    P3DIR |= BIT4;                            // Setup AIN2 (TB03) (M1 RED)
    P3SEL0 |= BIT4;                           // Option select timer output

    P3DIR |= BIT5;                            // Setup BIN1 (TB04) (M2 RED)
    P3SEL0 |= BIT5;                           // Option select timer output

    P2DIR |= BIT0;                            // Setup BIN2 (TB06) (M2 BLUE)
    P2SEL0 |= BIT0;                           // Option select timer output

    TB0CCR0 = 500;                            // PWM Period (SMCLK / 500 = 2kHz)
    TB0CTL = TBSSEL__SMCLK;                   // SMCLK
}

void enbl_mot() {
    P2OUT |= BIT1;                            // Enable DRV8836
}

void dsbl_mot(){
    P2OUT &= ~(BIT1);
    TB0CTL |= MC__STOP;
}

void drv_mot(int16_t sl, int16_t sr){

    if(sl > curr_mc.smax)
        sl = curr_mc.smax;
    else if(sl < -curr_mc.smax)
        sl = -curr_mc.smax;
    if(sr > curr_mc.smax)
        sr = curr_mc.smax;
    else if(sr < -curr_mc.smax)
        sr = -curr_mc.smax;

    TB0CTL |= MC__STOP;

    // right forward
    if(sr > 0){
        TB0CCTL1 = 0x00;
        TB0CCTL3 = OUTMOD_7;                      // CCR1 reset/set
        TB0CCR3 = sr + curr_mc.rmin;
    } // right reverse
    else if(sr < 0){
        TB0CCTL3 = 0x00;
        TB0CCTL1 = OUTMOD_7;                      // CCR3 reset/set
        TB0CCR1 = (-sr) + curr_mc.rmin;
    } // right stop
    else{
        TB0CCTL1 = 0x00;
        TB0CCTL3 = 0x00;
    }
    // left forward
    if(sl > 0){
        TB0CCTL4 = 0x00;
        TB0CCTL6 = OUTMOD_7;                      // CCR4 reset/set
        TB0CCR6 = sl + curr_mc.lmin;
    } // left reverse
    else if(sl < 0){
        TB0CCTL6 = 0x00;
        TB0CCTL4 = OUTMOD_7;                      // CCR6 reset/set
        TB0CCR4 = (-sl) + curr_mc.lmin;
    } // left stop
    else{
        TB0CCTL4 = 0x00;
        TB0CCTL6 = 0x00;
    }

    TB0CTL |= MC__UP | TBCLR;                    //Up mode, clear TBR
}
