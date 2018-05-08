/*
 * eusci_b0_i2c.h
 *
 *  Created on: 16 jan. 2017
*********************************************** */

#ifndef EUSCI_B0_I2C_H_
#define EUSCI_B0_I2C_H_

void i2c_init();
void i2c_write(uint8_t slv_addr, uint8_t reg_addr, uint8_t data);
uint8_t i2c_read(uint8_t slv_addr, uint8_t reg_addr);
void i2c_read_multi(uint8_t slv_addr, uint8_t reg_addr, uint8_t l, uint8_t *arr);

#endif /* EUSCI_B0_I2C_H_ */
