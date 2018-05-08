
#ifndef MOTOR_CTRL_H_
#define MOTOR_CTRL_H_

typedef struct motor_calib {
   int16_t lmin;
   int16_t rmin;
   int16_t smax;
}motor_calib;

void drv_init(motor_calib set_mc);
void enbl_mot();
void dsbl_mot();
void drv_mot(int16_t sl, int16_t sr);

#endif /* MOTOR_CTRL_H_ */
