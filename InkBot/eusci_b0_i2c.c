
#include <msp430.h>
#include <stdint.h>
#include "eusci_b0_i2c.h"

void i2c_init(){

    P1SEL1 |= BIT6 | BIT7;                  // configure I2C pins
    P1SEL0 &= ~(BIT6 | BIT6);               // configure I2C pins

    // I2C default uses SMCLK
    UCB0CTL1 |= UCSWRST;                    // put eUSCI_B in reset state
    UCB0CTLW0 |= UCMODE_3 | UCMST | UCSYNC; // I2C, master, sync
    UCB0BR0 = 0x0A;                         // clock frequency = SMCLK / 10 = 100kHz
    UCB0CTL1 &= ~UCSWRST;                   // eUSCI_B in operational state
}

void i2c_write(uint8_t slv_addr, uint8_t reg_addr, uint8_t data){

    while(UCB0STAT & UCBBUSY);

    UCB0I2CSA = slv_addr;                   // set slave address
    UCB0CTLW0 |= UCTR | UCTXSTT;            // transmitter mode and START condition.

    while (UCB0CTLW0 & UCTXSTT);
    UCB0TXBUF = reg_addr;
    while(!(UCB0IFG & UCTXIFG0));
    UCB0TXBUF = data;
    while(!(UCB0IFG & UCTXIFG0));

    UCB0CTLW0 |= UCTXSTP;
    while(UCB0CTLW0 & UCTXSTP);             // wait for stop
}

uint8_t i2c_read(uint8_t slv_addr, uint8_t reg_addr){

    uint8_t data = 0;

    while(UCB0STAT & UCBBUSY);
    UCB0I2CSA = slv_addr;                   // set slave address
    UCB0CTLW0 |= UCTR | UCTXSTT;            // transmitter mode and START condition.

    while(UCB0CTLW0 & UCTXSTT);
    UCB0TXBUF = reg_addr;
    while(!(UCB0IFG & UCTXIFG0));

    UCB0CTLW0 &= ~UCTR;                     // receiver mode
    UCB0CTLW0 |= UCTXSTT;                   // START condition

    while(UCB0CTLW0 & UCTXSTT);             // make sure start has been cleared
    UCB0CTLW0 |= UCTXSTP;                   // STOP condition
    while(!(UCB0IFG & UCRXIFG0));
    data = UCB0RXBUF;

    while(UCB0CTLW0 & UCTXSTP);

    return data;
}

void i2c_read_multi(uint8_t slv_addr, uint8_t reg_addr, uint8_t l, uint8_t *arr){

    uint8_t i;

    while(UCB0STAT & UCBBUSY);
    UCB0I2CSA = slv_addr;                   // set slave address
    UCB0CTLW0 |= UCTR | UCTXSTT;            // transmitter mode and START condition.

    while(UCB0CTLW0 & UCTXSTT);
    UCB0TXBUF = reg_addr;
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
    }

    while(UCB0CTLW0 & UCTXSTP);
}
