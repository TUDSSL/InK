
#include <msp430.h>
#include <stdint.h>
// include mathematical constants from math.h'
#ifndef __BSD_VISIBLE
#define __BSD_VISIBLE
#endif
#include <math.h>
#include "global.h"
#include "ctrl_loop.h"
#include "eusci_b0_i2c.h"
#include "gyro_sens.h"
#include "motor_ctrl.h"
#include "robot.h"

#if DEBUG
#pragma PERSISTENT(sensor_data);
int16_t sensor_data[400] =
{   0};
#endif

uint8_t curr_cmd;

#if RAMP
uint8_t rcnt = 19;
uint8_t ramp = 3;
#else
uint8_t ramp = 0;
#endif
int16_t lspeed = 1;
int16_t rspeed = 1;

// run set_setpoint()
float set = 0;
float ang_set = 0;

// run set_tunings()
float kp = 0, ki = 0, kd = 0;

// run set_limits()
float out_max = 75, out_min = -75;

float iterm = 0;
float prev = 0;

uint16_t num_loops = 0;
float ang = 0;

robot_conf curr_conf;

void ctrl_init(robot_conf set_conf)
{

    curr_conf = set_conf;

    // assume i2c is already initialized
    gyro_init();
    drv_init(curr_conf.mc);

    TA3CCR0 = 125000 * SAMPLE_TIME;           // Set timer frequency
    TA3CTL = TASSEL__SMCLK | ID__8 | MC__UP;  // SMCLK, divide by 8, UP mode

    __bis_SR_register(GIE);                   // Enable interrupt
}

void set_setpoint(float sp)
{
    set = sp;
}

// allow changing of tuning parameters for the different commands
void set_tunings(float Kp, float Ki, float Kd)
{
    kp = Kp;
    ki = Ki * SAMPLE_TIME;
    kd = Kd / SAMPLE_TIME;
}

void set_limits(float max, float min)
{
    out_max = max;
    out_min = min;
}

/*
 * Move, 0x01 straight, 0x02 turn left, 0x03 turn right
 * arg : 0x01 length in cm, 0x02 ang in deg, 0x03 ang in deg
 */
void move(uint8_t cmd, int16_t arg, float fram_ang)
{
    curr_cmd = cmd;
    switch (cmd)
    {
    case STRAIGHT:
        set_tunings(curr_conf.st.Kp, curr_conf.st.Ki, curr_conf.st.Kd);
        set_setpoint(0);
#if DEBUG
        num_loops = 200;
#else
        num_loops = (uint16_t) ((float) arg / VEL_CAL / SAMPLE_TIME);
#endif
#if !RAMP
        lspeed = MOT_TRG;
        rspeed = MOT_TRG;
#endif
    case CURVE_LEFT:
    case CURVE_RIGHT:
        set_tunings(curr_conf.st.Kp, curr_conf.st.Ki, curr_conf.st.Kd);
        if (curr_cmd == CURVE_LEFT)
        {
            set_setpoint((VEL_CAL / R_CRV) * (180 / M_PI));
            ang_set = arg;
        }
        else if (curr_cmd == CURVE_RIGHT)
        {
            set_setpoint(-(VEL_CAL / R_CRV) * (180 / M_PI));
            ang_set = -arg;
        }
        ang = fram_ang;                         // Always restore angle progress
        lspeed = MOT_TRG;
        rspeed = MOT_TRG;
        break;
    case TURN_LEFT:
        set_tunings(curr_conf.tl.Kp, curr_conf.tl.Ki, curr_conf.tl.Kd);
        set_setpoint(arg);
        ang = fram_ang;                         // Always restore angle progress
        break;
    case TURN_RIGHT:
        set_tunings(curr_conf.tr.Kp, curr_conf.tr.Ki, curr_conf.tr.Kd);
        set_setpoint(-arg);
        ang = fram_ang;                         // Always restore angle progress
        break;
        //default : /* Optional */
    }
    set_limits(curr_conf.mc.smax, -curr_conf.mc.smax);
    enbl_mot();
    TA3CCTL0 = CCIE;                          // TACCR0 interrupt enabled
}

// compute PID output
inline float pid_compute(float input)
{
    float err, dinp, output;
    err = set - input;
    iterm += (ki * err);
    if (iterm > out_max)
        iterm = out_max;
    else if (iterm < out_min)
        iterm = out_min;
    dinp = input - prev;
    output = kp * err + iterm + kd * dinp;
    /*if(output > out_max)
     output = out_max;
     else if(output < out_min)
     output = out_min;*/
    prev = input;
    return output;
}