
#ifndef GYRO_SENS_H_
#define GYRO_SENS_H_

#define BMG_ADDR 0x68

void gyro_init();
int16_t gyro_read();

#endif /* GYRO_SENS_H_ */
