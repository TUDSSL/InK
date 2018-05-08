#include "robot.h"
#include "gyro_sens.h"
#include "motor_ctrl.h"
#include "ctrl_loop.h"
#include <math.h>
#include "eusci_b0_i2c.h"
#include "prox_sens.h"
#include <stdio.h>
#include "fram_log.h"

#define MAX_PROX_DATA_BUFFER 5
#define SEEK_COUNT_THRESHOLD 20
#define SEARCH_ANGLE 10
#define MAX_MOVES 4

#if ROBOT==1
robot_conf conf = { { 0.13 * 0.6, (20 / 100) / 2, (20 / 100) / 8 }, //tunings straight
        { 0.20, 0, 0 },                           //tunings turn_left
        { 0.07, 0, 0 },                           //tunings turn_right
        { 80, 65, 75 }                            //lmin, rmin, smax
};
#elif ROBOT==2
robot_conf conf =
{
    {   0.18*0.6, (20/100)/2, (20/100)/8},     //tunings straight
    {   0.25, 0, 0},                           //tunings turn_left
    {   0.07, 0, 0},                           //tunings turn_right
    {   70, 110, 90}                           // lmin, rmin, smax
};
#elif ROBOT==3
robot_conf conf =
{
    {   0.13*0.6, (20/100)/2, (20/100)/8},     //tunings straight
    {   0.3, 0, 0},                            //tunings turn_left
    {   0.3, 0, 0},                            //tunings turn_right
    {   90, 80, 85}                           // lmin, rmin, smax
};
#else
//ERROR
#endif

typedef enum 
{
    CALLIBRATE,
    FIND_OBJECT

}patrol_state_t;


typedef enum 
{
    SEARCH_FORWARD,
    SEARCH_LEFT,
    SEARCH_RIGHT,
    RESCAN_AREA

}seek_cmd_t;

typedef enum 
{
    CALLIBRATING,
    SEEK,
    DESTROY

}bot_state_t;

typedef enum
{
    LOCKED,
    UNLOCKED

}lock_t;

// define task-shared persistent variables.
__shared(
    nv_var fram;
    patrol_state_t patrol_state;
    uint8_t len;
    uint16_t inst[MAX_MOVES];
    uint8_t curr_cmd;
    uint8_t cp;
    float kp,ki,kd;
    float set;
    int16_t lspeed,rspeed;
    float ang;
    uint16_t num_loops;
    float ang_set;
    float out_max,out_min;
    uint16_t cnt;
    uint8_t stop;
    float iterm;
    float prev;
    uint8_t callibrated;
    uint8_t prox_cal_data[MAX_PROX_DATA_BUFFER];
    uint8_t prox_cal_idx;
    bot_state_t bot_state;
    seek_cmd_t cmd;
    uint8_t prox_base;
    uint8_t target_prox;
    uint16_t seek_cnt;
    int fram_count;

)

__nv lock_t lock;

__CREATE_CHANNEL(PATROL_THREAD,CTRL_THREAD,10);

ENTRY_TASK(main_task);
TASK(callibrate_init);
TASK(move_task);
TASK(execute_move);
TASK(move_complete);
TASK(find_object_task);
TASK(find_object_course);

// called at the very first boot
void patrol_thread_init()
{
    // create a thread with priority 15 and entry task task1
    __CREATE(PATROL_THREAD, main_task);
    __SIGNAL(PATROL_THREAD);
}
robot_conf curr_conf;


// called at the very first boot
void patrol_thread_reboot()
{
    PJDIR |= BIT6;
    PJOUT &= ~BIT6;
    PJOUT |= BIT6;
    lock = LOCKED;
    i2c_init();
    prox_init();
    ctrl_init(conf);
    curr_conf = conf;
    __SIGNAL(PATROL_THREAD);

}

int8_t len = 2;
uint16_t inst[2] = { CURVE_RIGHT, 360 };

ENTRY_TASK(main_task)
{   

    if (__GET(callibrated))
    {
        __SET(patrol_state,FIND_OBJECT);

    }else
        __SET(patrol_state,CALLIBRATE);

    switch (__GET(patrol_state))
    {

        case CALLIBRATE:
            PJOUT |= BIT6;
            __SET(bot_state,CALLIBRATING);
            return callibrate_init;
        case FIND_OBJECT:
            __SET(bot_state,SEEK);
            return find_object_task;
        default:
            return main_task;
    }

}
#define FIND_OBJECT_THRES 150
TASK(find_object_task)
{
    uint8_t found = 0;
    uint8_t cnt = 0;

    while(!found)
    {
        if (__GET(prox_cal_data[__GET(prox_cal_idx)]) > FIND_OBJECT_THRES)
        {
            found = 1;
            return find_object_course;
        }
       
        __SET(prox_cal_idx,(__GET(prox_cal_idx) + 1)%MAX_PROX_DATA_BUFFER);
        
        if (++cnt > MAX_PROX_DATA_BUFFER)
        {
            found = 1;
            __SET(callibrated,0);
            return main_task;
        }
    }
    
}

TASK(find_object_course)
{
    if (__GET(target_prox) > __GET(prox_base))
    {
        __SET(bot_state, DESTROY);
        // PJOUT |= BIT6;
        __SET(cmd,SEARCH_LEFT);
        __SET(seek_cnt,0);                     //reset seek counter
    
    }else{   

        __SET(seek_cnt, ++__GET(seek_cnt));   //keep track of successive seek attempts
        __SET(bot_state, SEEK);
        
        if (__GET(cmd) == SEARCH_FORWARD)
        {
            __SET(cmd,SEARCH_RIGHT);
        }
        if (__GET(cmd) == SEARCH_RIGHT)
        {
            __SET(cmd,SEARCH_LEFT);
        }
        if (__GET(cmd) == SEARCH_LEFT)
        {
            __SET(cmd,SEARCH_FORWARD);
        }
        if (__GET(seek_cnt) > SEEK_COUNT_THRESHOLD)
        {
            __SET(seek_cnt,0);              //reset seek counter
            __SET(cmd,RESCAN_AREA);
        }else
            __SET(cmd,SEARCH_FORWARD);
    }

    __SET(prox_base,prox_read());

    switch(__GET(bot_state))
    {
        case SEEK:
        default:
        switch(__GET(cmd))
        {
            case SEARCH_FORWARD:
                __SET(len, 2);
                __SET(inst[0],STRAIGHT);
                __SET(inst[1],1);
                return move_task;
            case SEARCH_RIGHT:
                __SET(len, 4);
                __SET(inst[0],STRAIGHT);
                __SET(inst[1],1);
                __SET(inst[2],CURVE_RIGHT);
                __SET(inst[3],SEARCH_ANGLE);
                return move_task;
            case SEARCH_LEFT:
                __SET(len, 4);
                __SET(inst[0],STRAIGHT);
                __SET(inst[1],1);
                __SET(inst[2],CURVE_LEFT);
                __SET(inst[3],SEARCH_ANGLE);
                return move_task;
            case RESCAN_AREA:
            default:
                __SET(len, 2);
                __SET(inst[0],CURVE_RIGHT);
                __SET(inst[1],360);
                return move_task;
        }
        case DESTROY:
            while(1);   
    }

}

//the robot will perform a 360 turn to register its surrounding 
//points of interest 
TASK(callibrate_init)
{
    __SET(len, 2);
    __SET(inst[0],CURVE_RIGHT);
    __SET(inst[1],360);

    return move_task;
}

TASK(move_task)
{
    if (__GET(cp) < __GET(len) / 2)
    {

        __SET(curr_cmd,inst[2 * __GET(cp)]);
        int16_t arg = inst[2 * __GET(cp) + 1];

        switch(__GET(curr_cmd)) {

            case STRAIGHT:
                __SET(kp,curr_conf.st.Kp);
                __SET(ki,curr_conf.st.Ki*SAMPLE_TIME);
                __SET(kd,curr_conf.st.Kd/SAMPLE_TIME);
                __SET(set,0);

                __SET(num_loops, (uint16_t)((float)arg / VEL_CAL / SAMPLE_TIME));
                __SET(lspeed , MOT_TRG);
                __SET(rspeed , MOT_TRG);

            case CURVE_LEFT:
            case CURVE_RIGHT:
                __SET(kp,curr_conf.st.Kp);
                __SET(ki,curr_conf.st.Ki*SAMPLE_TIME);
                __SET(kd,curr_conf.st.Kd/SAMPLE_TIME);
                if(__GET(curr_cmd) == CURVE_LEFT){
                    __SET(set,(VEL_CAL / R_CRV)*(180 / M_PI));
                    __SET(ang_set, arg);
                }
                else if(__GET(curr_cmd) == CURVE_RIGHT){
                    __SET(set,-(VEL_CAL / R_CRV)*(180 / M_PI));
                    __SET(ang_set, -arg);
                }
                __SET(ang,__GET(ang));                           // Always restore angle progress
                __SET(lspeed , MOT_TRG);
                __SET(rspeed , MOT_TRG);
                break;
            case TURN_LEFT:
                __SET(kp,curr_conf.tl.Kp);
                __SET(ki,curr_conf.tl.Ki*SAMPLE_TIME);
                __SET(kd,curr_conf.tl.Kd/SAMPLE_TIME);
                __SET(set,arg);
                __SET(ang,__GET(ang));                           // Always restore angle progress
                break;
            case TURN_RIGHT:
                __SET(kp,curr_conf.tr.Kp);
                __SET(ki,curr_conf.tr.Ki*SAMPLE_TIME);
                __SET(kd,curr_conf.tr.Kd/SAMPLE_TIME);
                __SET(set,-arg);
                __SET(ang,__GET(ang));                           // Always restore angle progress
                break;
            //default : /* Optional */
        }
        __SET(out_max,curr_conf.mc.smax);
        __SET(out_min,-curr_conf.mc.smax);
        //TODO:move to proper file
        enbl_mot();
        TA3CCTL0 = CCIE;                          // TACCR0 interrupt enabled
    }

    return execute_move;   
}

// compute PID output
static inline float pid_compute(float input,float set,float *iterm,float *prev,float out_min,float out_max,float kp,float ki,float kd){
    float err, dinp, output;

    err = set - input;
    
    *iterm += ki * err;
    
    if(*iterm > out_max)
        *iterm = out_max;
    else if(*iterm < out_min)
        *iterm = out_min;
    
    dinp = input - *prev;
    output = kp*err + *iterm + kd*dinp;
    /*if(output > out_max)
        output = out_max;
    else if(output < out_min)
        output = out_min;*/
    *prev = input;
    return output;
}

TASK(execute_move)
{
    int16_t data;
    uint8_t ramp = 0;
    float omega, turn;
    float local_iterm,local_prev;
    local_iterm = __GET(iterm);
    local_prev = __GET(prev);

    while(lock == LOCKED);
    
    data = gyro_read();
    omega = data / 32.767;
    float ang = __GET(ang);
    ang += omega * SAMPLE_TIME; // integrate to get the angle

    framLog(ang,__GET(bot_state),__GET(ang_set));

    __SET(ang, ang);             // save the current angle

    switch(__GET(curr_cmd)) {
        case STRAIGHT:
            if(__GET(cnt) >= __GET(num_loops) || __GET(stop))
            {
                return move_complete;
            }
            turn = pid_compute(omega,__GET(set),&local_iterm,&local_prev,__GET(out_min),__GET(out_max),__GET(kp),__GET(ki),__GET(kd));
            __SET(lspeed,ramp + __GET(lspeed) - (int16_t)turn);
            __SET(rspeed,ramp + __GET(rspeed) + (int16_t)turn);
            __SET(cnt,++__GET(cnt));
            break;
        case CURVE_LEFT:
        case CURVE_RIGHT:
            if(fabs(__GET(ang_set) - ang) < TOLERANCE_DEGREES){
                return move_complete;
            }
            turn = pid_compute(omega,__GET(set),&local_iterm,&local_prev,__GET(out_min),__GET(out_max),__GET(kp),__GET(ki),__GET(kd));
            __SET(lspeed, __GET(lspeed) - (int16_t)turn);
            __SET(rspeed, __GET(rspeed) + (int16_t)turn);
            break;
        case TURN_LEFT:
        case TURN_RIGHT:
            if(fabs(__GET(set) - ang) < TOLERANCE_DEGREES){
                return move_complete;
            }
            turn = pid_compute(ang,__GET(set),&local_iterm,&local_prev,__GET(out_min),__GET(out_max),__GET(kp),__GET(ki),__GET(kd));
            __SET(lspeed, -(int16_t)turn);
            __SET(rspeed, +(int16_t)turn);
            break;
    //default : /* Optional */
    }
    __SET(iterm,local_iterm);
    __SET(prev,local_prev);
    drv_mot(__GET(lspeed),__GET(rspeed));
    lock  = LOCKED;
    
    //update the proximity data
    switch (__GET(bot_state))
    {   
        case SEEK:
        case DESTROY:
            __SET(target_prox,prox_read());
            break;
        case CALLIBRATING:
            __SET(prox_cal_data[__GET(prox_cal_idx)],prox_read());
            __SET(prox_cal_idx, (__GET(prox_cal_idx)+1)%MAX_PROX_DATA_BUFFER);
        default:    
            break;

    }

    return execute_move;
}

TASK(move_complete)
{
    if (!__GET(callibrated)&& (__GET(bot_state) == CALLIBRATING))
    {
        __SET(callibrated,1);
        PJOUT &= ~BIT6;
    }

    dsbl_mot();
    TA3CCTL0 &= ~CCIE;
    __SET(set,0);                                // Always return set to 0 (straight)
    __SET(iterm,0);
    __SET(prev,0);

    __SET(cnt, 0);
    __SET(ang, 0);
    __SET(stop,1);
    __SET(cp,0);
    
    switch (__GET(bot_state))
    {
        
        case SEEK:
        case DESTROY:
            return find_object_course;
        case CALLIBRATING:
            return main_task;
    }
}

// the event that will be used to register the uart event
isr_event_t timer_event;

// Timer3_A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER3_A0_VECTOR
__interrupt void Timer3_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER3_A0_VECTOR))) Timer3_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{

    if (!__EVENT_BUFFER_FULL(PATROL_THREAD))
    {
        timer_event.data = 0;
        timer_event.size = 0;
        timer_event.timestamp = 0x1234;

        // __SIGNAL_EVENT(CTRL_THREAD, &timer_event);
    }

    lock = UNLOCKED;
    /* turn on CPU */
    __bic_SR_register_on_exit(LPM3_bits);
}
