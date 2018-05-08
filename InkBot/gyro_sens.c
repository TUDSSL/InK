

#include <msp430.h>
#include <stdint.h>
#include "gyro_sens.h"
#include "eusci_b0_i2c.h"

void gyro_init(){

    //Software reset
    //i2c_write(BMG_ADDR, 0x7E, 0xB6);

    //__delay_cycles(100000);

    //Set to normal mode
    i2c_write(BMG_ADDR, 0x7E, 0x15);

    //Wait for normal mode (requires typically 55ms)
    __delay_cycles(400000);
    //while(!(i2c_read(BMG_ADDR, 0x03) & 0x04));

    //Set data rate to 100Hz and bandwidth to 10.4Hz
    i2c_write(BMG_ADDR, 0x42, 0x28);

    //Set angular rate measurement range to 1000deg/s
    i2c_write(BMG_ADDR, 0x43, 0x01);

    //Start automatic offset compensation
    i2c_write(BMG_ADDR, 0x7E, 0x03);

    //Wait for completion (max 250ms)
    while(!(i2c_read(BMG_ADDR, 0x1B) & 0x08));

    //Enable FOC?
    i2c_write(BMG_ADDR, 0x77, (i2c_read(BMG_ADDR, 0x77) | 0x80));

    //Remove stall data, clear data ready bit
    gyro_read();
}

int16_t gyro_read() {

    uint8_t data[2];
    int16_t ret;

    //Check if data ready bit is set (multiple reads can result in hang here)
    //while(!(i2c_read(BMG_ADDR, 0x1B) & 0x40));

    // Only return z-axis data!
    i2c_read_multi(BMG_ADDR, 0x16, 2, data);

    ret = data[1] << 8;
    ret |= data[0];
    return ret;
}
