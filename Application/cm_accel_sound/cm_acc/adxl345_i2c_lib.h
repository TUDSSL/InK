//adxl345 I2C library
#ifndef EUSCI_B0_I2C_H_
#define EUSCI_B0_I2C_H_

#define NUM_BYTES_TX 2  
#define NUM_BYTES_RX 6
#define ADXL_345     0x53
#define ADXL_CONF_REG 0x2d
#define READ_REG	  0x32

#include <msp430.h>
#include <stdint.h>

void i2c_init();
void i2c_write(uint8_t slv_addr, uint8_t reg_addr, uint8_t data);
uint8_t i2c_read(uint8_t slv_addr, uint8_t reg_addr);
void i2c_read_multi(uint8_t slv_addr, uint8_t reg_addr, uint8_t l, uint8_t *arr);
void print_data(unsigned char *data, uint8_t i);

#endif /* EUSCI_B0_I2C_H_ */
