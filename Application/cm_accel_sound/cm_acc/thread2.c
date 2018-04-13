//thread2.c
#include "app.h"
#include "ink.h"
#include "dsplib/include/DSPLib.h"
#include "app_intrinsics.h"
#include "adxl345_i2c_lib.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

//Classification part expires at:
//#define EXPRIRATION_TIME 8000 //~460ms
//#define EXPRIRATION_TIME 13000 //~1000ms
//#define EXPRIRATION_TIME 6500 //~270ms

// Number of samples to discard before recording training set
#define NUM_WARMUP_SAMPLES 3

#define ACCEL_WINDOW_SIZE 3
#define MODEL_SIZE 16
#define SAMPLE_NOISE_FLOOR 10 // TODO: made up value

// Number of classifications to complete in one experiment
#define SAMPLES_TO_COLLECT 512

typedef struct 
{
    int16_t x,y,z;

}threeAxis_t_8;

typedef threeAxis_t_8 accelReading;

typedef accelReading accelWindow[ACCEL_WINDOW_SIZE];

typedef struct {
    unsigned meanmag;
    unsigned stddevmag;
} features_t;

typedef enum {
    CLASS_STATIONARY,
    CLASS_MOVING,
} class_t;


typedef enum {
    MODE_IDLE = 3,
    MODE_TRAIN_STATIONARY = 2,
    MODE_TRAIN_MOVING = 1,
    MODE_RECOGNIZE = 0, // default
} run_mode_t;


uint8_t acc_data[NUM_BYTES_RX];

/* Allocate 16-bit real FFT data vector with correct alignment */
DSPLIB_DATA(sampled_input_tmp, MSP_ALIGN_FFT_Q15(N_SAMPLES))
 _q15 sampled_input_tmp[N_SAMPLES];
__shared(

  _q15 data_array[N_SAMPLES];
  uint8_t pinCont;
  uint16_t _v_pinState;
  unsigned _v_seed;
  unsigned _v_count;
  unsigned _v_discardedSamplesCount;
  unsigned _v_totalCount;
  unsigned _v_movingCount;
  unsigned _v_stationaryCount;
  unsigned _v_trainingSetSize;
  unsigned _v_samplesInWindow;
  accelReading _v_window[ACCEL_WINDOW_SIZE];
  features_t _v_features;
  features_t _v_model_stationary[MODEL_SIZE];
  features_t _v_model_moving[MODEL_SIZE];
  class_t _v_class;
  run_mode_t _v_mode;
)

__USE_CHANNEL(THREAD1,THREAD2);


ENTRY_TASK(task1);
TASK(task2);
TASK(task_init);
TASK(task_selectMode);
TASK(task_resetStats);
TASK(task_sample);
TASK(task_transform);
TASK(task_featurize);
TASK(task_classify);
TASK(task_stats);
TASK(task_warmup);
TASK(task_train);
TASK(task_idle);


uint8_t acc_data[NUM_BYTES_RX];

unsigned isqr_Newton(unsigned x);

void thread2_init(){

    // create a thread with priority 15 and entry task task1
    __CREATE(THREAD2,task1);

}


ENTRY_TASK(task1){
  
  P3OUT |= BIT0;
#ifndef EMULATE
  setup_mcu();

  i2c_init();
  
  i2c_write(ADXL_345 , ADXL_CONF_REG , 0x00);
  i2c_write(ADXL_345, ADXL_CONF_REG, 0x10);
  i2c_write(ADXL_345, ADXL_CONF_REG, 0x08);

    
  //get samples 
  uint8_t collected = 0;
  uint16_t z;
  while(N_SAMPLES-collected){

        i2c_read_multi(ADXL_345, READ_REG, NUM_BYTES_RX, &acc_data);
        z = (((int16_t)acc_data[5]) << 8) | acc_data[4];
        __SET(data_array[collected++],z);
  }
#else
  __delay_cycles(SAMPLE_ACC_DURATION);
#endif
  P3OUT &= ~BIT0;
 
  return task2;
}

TASK(task2){

  P3OUT |= BIT0;
#ifndef EMULATE
  msp_status status;

  uint8_t i;
  for (i = 0; i < N_SAMPLES; i++)
  {
    sampled_input_tmp[i] = __GET(data_array[i]);
  }
  //  /* Configure parameters for FFT */
  msp_fft_q15_params fftParams;
  msp_abs_q15_params absParams;
  msp_max_q15_params maxParams;

  fftParams.length = N_SAMPLES;
  fftParams.bitReverse = 1;
  fftParams.twiddleTable = msp_cmplx_twiddle_table_128_q15;

  absParams.length = N_SAMPLES;

  maxParams.length = N_SAMPLES;

  uint16_t max_index = 0;

  /* Perform FFT */
  status = msp_fft_fixed_q15(&fftParams, sampled_input_tmp);

  /* Remove DC component  */
  sampled_input_tmp[0] = 0;

  /* Compute absolute value of FFT */
  status = msp_abs_q15(&absParams, sampled_input_tmp, sampled_input_tmp);

  /* Get peak frequency */
  status = msp_max_q15(&maxParams, sampled_input_tmp, NULL, &max_index); 
#else

  __delay_cycles(FFT_ACC_DURATION);

#endif  

  P3OUT &= ~BIT0;

  //Initialize the AR-app 
  //AR test app is used here for an estimation 
  //of a high computation application 
  return task_init;

}

//Dummy data sampling
void ACCEL_singleSample_(threeAxis_t_8* result){
   
  // P4OUT |= BIT3;
#ifndef EMULATE
  i2c_init();

  i2c_write(ADXL_345 , ADXL_CONF_REG , 0x00);
  i2c_write(ADXL_345, ADXL_CONF_REG, 0x10);
  i2c_write(ADXL_345, ADXL_CONF_REG, 0x08);

  i2c_read_multi(ADXL_345, READ_REG, NUM_BYTES_RX, &acc_data);
  // P4OUT &= ~BIT3;
#else

  __delay_cycles(SINGLE_ACC_SAMPLE_DURATION);

  acc_data[0] = 1;
  acc_data[1] = 2;
  acc_data[2] = 3;
  acc_data[3] = 4;
  acc_data[4] = 5;
  acc_data[5] = 6;

#endif


  result->x = (((int16_t)acc_data[1]) << 8) | acc_data[0];//(__GET(_v_seed)*17)%85;
  result->y = (((int16_t)acc_data[3]) << 8) | acc_data[2];//(__GET(_v_seed)*17*17)%85;
  result->z = (((int16_t)acc_data[5]) << 8) | acc_data[4];;//(__GET(_v_seed)*17*17*17)%85;
}


TASK(task_init)
{

  P3OUT |= BIT0;
    
  //uint32_t tmp = __get_time();
#ifdef EXPRIRATION_TIME
  set_expire_timer(THREAD2,EXPRIRATION_TIME);
  P2OUT |= BIT6;
  P2OUT &= ~BIT6;
#endif

  
  __SET(pinCont, 1);
  __SET(_v_pinState , MODE_IDLE);
  __SET( _v_count , 0);
  __SET( _v_seed , 1);
  
  P3OUT &= ~BIT0;
  
  return task_selectMode;
}

TASK(task_selectMode)
{
  P3OUT |= BIT0;

  uint16_t pinState = __GET(_v_pinState);
  unsigned discardedSamplesCount = __GET(_v_discardedSamplesCount);
  unsigned samplesInWindow = __GET(_v_samplesInWindow);
  unsigned count = __GET(_v_count);
  class_t  lc_class = __GET(_v_class);
  run_mode_t lc_mode = __GET(_v_mode);
  
  uint16_t pin_state=1;

  ++count;
  
  if(count >= 3)  pin_state=2;
  if(count >= 5)  pin_state=0;
  
  __SET(_v_count,count);
  
  // Don't re-launch training after finishing training
  if ((pin_state == MODE_TRAIN_STATIONARY || pin_state == MODE_TRAIN_MOVING) 
      && pin_state == __GET(_v_pinState))
  {
    pin_state = MODE_IDLE;
  } 
  else 
  {
    __SET(_v_pinState , pin_state);
  }

  
  switch(pin_state) {
    case MODE_TRAIN_STATIONARY:
        __SET(_v_discardedSamplesCount , 0);
        __SET(_v_mode , MODE_TRAIN_STATIONARY);
        __SET(_v_class , CLASS_STATIONARY);
        __SET(_v_samplesInWindow , 0);
      P3OUT &= ~BIT0;
        return task_warmup;

    case MODE_TRAIN_MOVING:
        __SET(_v_discardedSamplesCount , 0);
        __SET(_v_mode , MODE_TRAIN_MOVING);
        __SET(_v_class , CLASS_MOVING);
        __SET(_v_samplesInWindow , 0);          
      P3OUT &= ~BIT0;
        return task_warmup;

    case MODE_RECOGNIZE:
        __SET(_v_mode , MODE_RECOGNIZE);
      P3OUT &= ~BIT0;
        return task_resetStats;

    default:
      P3OUT &= ~BIT0;
        return task_idle;
      }
}

TASK(task_warmup)
{
  P3OUT |= BIT0;
  
  unsigned discardedSamplesCount = __GET(_v_discardedSamplesCount);
  unsigned trainingSetSize= __GET(_v_trainingSetSize);
      
    threeAxis_t_8 sample;
    

    if (discardedSamplesCount < NUM_WARMUP_SAMPLES) {

        ACCEL_singleSample_(&sample);
        __SET(_v_discardedSamplesCount , ++discardedSamplesCount);
    P3OUT &= ~BIT0;
        return task_warmup;
    } else {
        __SET(_v_trainingSetSize , 0);
    P3OUT &= ~BIT0;
        return task_sample;
    }

}

TASK(task_sample)
{
  P3OUT |= BIT0;

    int8_t ios_i;
    
  unsigned samplesInWindow = __GET(_v_samplesInWindow);
    

      accelReading sample;
      ACCEL_singleSample_(&sample);
      __SET(_v_window[samplesInWindow].x , sample.x);
      __SET(_v_window[samplesInWindow].y , sample.y);
      __SET(_v_window[samplesInWindow].z , sample.z);
      ++samplesInWindow;
      __SET(_v_samplesInWindow,samplesInWindow);

      if (samplesInWindow < ACCEL_WINDOW_SIZE) {
      P3OUT &= ~BIT0;
          return task_sample;
      } else {
          __SET(_v_samplesInWindow , 0);
      P3OUT &= ~BIT0;
          return task_transform;
      }

}

TASK(task_transform)
{
  P3OUT |= BIT0;

  int8_t ios_i;
    
  unsigned samplesInWindow = __GET(_v_samplesInWindow);


  accelReading window[ACCEL_WINDOW_SIZE];

  for (ios_i = 0; ios_i < ACCEL_WINDOW_SIZE; ++ios_i)
  {
    window[ios_i] = __GET(_v_window[ios_i]);
  }
      

    unsigned i;

      for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
          if (window[i].x < SAMPLE_NOISE_FLOOR ||
                  window[i].y < SAMPLE_NOISE_FLOOR ||
                  window[i].z < SAMPLE_NOISE_FLOOR) {

              if (window[i].x > SAMPLE_NOISE_FLOOR) __SET(_v_window[i].x,window[i].x);
                else __SET(_v_window[i].x,0);

              if(window[i].y > SAMPLE_NOISE_FLOOR) __SET(_v_window[i].y,window[i].y);
                  else __SET(_v_window[i].y,0);

              if(window[i].z > SAMPLE_NOISE_FLOOR) __SET(_v_window[i].z,window[i].z);
                  else __SET(_v_window[i].z,0);
          }
      }
    P3OUT &= ~BIT0;
    return task_featurize;      

}

TASK(task_featurize)
{
  P3OUT |= BIT0;
  
    int8_t ios_i;
    
    run_mode_t mode = __GET(_v_mode);
    features_t features = __GET(_v_features);


  accelReading window[ACCEL_WINDOW_SIZE];

  for (ios_i = 0; ios_i < ACCEL_WINDOW_SIZE; ++ios_i)
  {
    window[ios_i] = __GET(_v_window[ios_i]);
  }
    

    accelReading mean, stddev;
    mean.x = mean.y = mean.z = 0;
    stddev.x = stddev.y = stddev.z = 0;


    uint8_t i = 0;
    for (i = 0; i < ACCEL_WINDOW_SIZE; i++)
    {
        mean.x += window[i].x;
        mean.y += window[i].y;
        mean.z += window[i].z;
    }
    
    mean.x >>= 2;
    mean.y >>= 2;
    mean.z >>= 2;

    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        stddev.x += window[i].x > mean.x ? window[i].x - mean.x
            : mean.x - window[i].x;
        stddev.y += window[i].y > mean.y ? window[i].y - mean.y
            : mean.y - window[i].y;
        stddev.z += window[i].z > mean.z ? window[i].z - mean.z
            : mean.z - window[i].z;
    }
    stddev.x >>= 2;
    stddev.y >>= 2;
    stddev.z >>= 2;

    unsigned meanmag = mean.x*mean.x + mean.y*mean.y + mean.z*mean.z;
    unsigned stddevmag = stddev.x*stddev.x + stddev.y*stddev.y + stddev.z*stddev.z;
    features.meanmag   = isqr_Newton(meanmag);
    features.stddevmag = isqr_Newton(stddevmag);

    switch (mode) {
        case MODE_TRAIN_STATIONARY:
        case MODE_TRAIN_MOVING:
            __SET(_v_features.meanmag , features.meanmag);
            __SET(_v_features.stddevmag , features.stddevmag);
      P3OUT &= ~BIT0;
            return task_train;
        case MODE_RECOGNIZE:
            __SET(_v_features.meanmag , features.meanmag);
            __SET(_v_features.stddevmag , features.stddevmag);
      P3OUT &= ~BIT0;
            return task_classify;
        default:
            // TODO: abort
            break;
    }
}

TASK(task_classify)
{
  P3OUT |= BIT0;

    int8_t ios_i;
    
    class_t lc_class = __GET(_v_class);
    features_t features = __GET(_v_features);

  accelReading window[ACCEL_WINDOW_SIZE];

    for (ios_i = 0; ios_i < ACCEL_WINDOW_SIZE; ios_i++)
    {
        window[ios_i] =
                __GET(_v_window[ios_i]);
    }

    features_t model_stationary[MODEL_SIZE];
    for (ios_i = 0; ios_i < MODEL_SIZE; ios_i++)
    {
      model_stationary[ios_i] =
                __GET(_v_model_stationary[ios_i]);
    }    

    features_t model_moving[MODEL_SIZE];
    for (ios_i = 0; ios_i < MODEL_SIZE; ios_i++)
    {
        model_moving[ios_i] =
                __GET(_v_model_moving[ios_i]);
    }    
    int move_less_error = 0;
    int stat_less_error = 0;
    int i;
    long int meanmag;
    long int stddevmag;

    meanmag = __GET(_v_features.meanmag);
    stddevmag = __GET(_v_features.stddevmag);

    for (i = 0; i < MODEL_SIZE; ++i) {
        long int stat_mean_err = ((model_stationary[i].meanmag) > meanmag)?((model_stationary[i].meanmag) - meanmag) : (meanmag - model_stationary[i].meanmag);

        long int stat_sd_err = ((model_stationary[i].stddevmag) > stddevmag)?((model_stationary[i].stddevmag) - stddevmag) : ((stddevmag - model_stationary[i].stddevmag));
    
        long int move_mean_err = ((model_moving[i].meanmag) > meanmag) ? ((model_moving[i].meanmag) - meanmag) : (meanmag - model_moving[i].meanmag);

        long int move_sd_err = ((model_moving[i].stddevmag) > stddevmag) ? ((model_moving[i].stddevmag) - stddevmag) : ((stddevmag - model_moving[i].stddevmag));
        
        if (move_mean_err < stat_mean_err) {
            move_less_error++;
        } else {
            stat_less_error++;
        }

        if (move_sd_err < stat_sd_err) {
            move_less_error++;
        } else {
            stat_less_error++;
        }

      }

    if (move_less_error > stat_less_error) __SET(_v_class, CLASS_MOVING);
      else __SET(_v_class,CLASS_STATIONARY);

  P3OUT &= ~BIT0;
    return task_stats;
}

TASK(task_stats)
{
  P3OUT |= BIT0;

  unsigned totalCount = __GET(_v_totalCount);    
  unsigned movingCount = __GET(_v_movingCount);
  unsigned stationaryCount = __GET(_v_stationaryCount);        
    class_t lc_class = __GET(_v_class);

    ++totalCount;
    __SET(_v_totalCount,totalCount);

    switch (lc_class) {
        case CLASS_MOVING:

            __SET(_v_movingCount,++movingCount);
            break;
        case CLASS_STATIONARY:

            __SET(_v_stationaryCount, ++stationaryCount);
            break;
    }

    if (totalCount == SAMPLES_TO_COLLECT) {

        unsigned resultStationaryPct = stationaryCount * 100 / totalCount;
        unsigned resultMovingPct = movingCount * 100 / totalCount;

        unsigned sum = stationaryCount + movingCount;
         // printf("stats: s %u (%u%%) m %u (%u%%) sum/tot %u/%u: %c\r\n",
         //        _v_stationaryCount, resultStationaryPct,
         //        _v_movingCount, resultMovingPct,
         //        _v_totalCount, sum, sum == _v_totalCount ? 'V' : 'X');
         //printf("%d,%d,%d\n",_v_totalCount,_v_stationaryCount,_v_movingCount);
    P3OUT &= ~BIT0;
        return task_idle;
    } else {
    P3OUT &= ~BIT0;
        return task_sample;
    }
}


TASK(task_idle) 
{
  P3OUT |= BIT0;

  uint8_t lc_pinCont = __GET(pinCont);
  #ifdef EXPRIRATION_TIME
      stop_expire_timer(THREAD2);
      P2OUT |= BIT5;
      P2OUT &= ~BIT5;
  #endif
  P3OUT |= BIT4;
  P3OUT &=~BIT4;
  P1IE |= BIT4;                               // P1.4 interrupt enabled


    if (lc_pinCont){
      __SET(pinCont,0);
      __SIGNAL(THREAD1);
    }

  P3OUT &= ~BIT0;
  return NULL;
}

TASK(task_resetStats)
{ 
    P3OUT |= BIT0;
      __SET(_v_movingCount , 0);
      __SET(_v_stationaryCount , 0);
      __SET(_v_totalCount , 0);

      __SET(_v_samplesInWindow , 0);
    P3OUT &= ~BIT0;

      return task_sample;
}


TASK(task_train)
{
  P3OUT |= BIT0;

    int8_t ios_i;
    
  unsigned trainingSetSize = __GET(_v_trainingSetSize);
    class_t lc_class= __GET(_v_class);
    features_t features = __GET(_v_features);
    features_t model_moving[MODEL_SIZE],model_stationary[MODEL_SIZE];
    
    for (ios_i = 0; ios_i < MODEL_SIZE; ios_i++)
    {
      model_stationary[ios_i] =
                __GET(_v_model_stationary[ios_i]);
    }    


    for (ios_i = 0; ios_i < MODEL_SIZE; ios_i++)
    {
        model_moving[ios_i] =
                __GET(_v_model_moving[ios_i]);
    }    
    
      switch (lc_class) {
          case CLASS_STATIONARY:
              __SET(_v_model_stationary[trainingSetSize].meanmag , features.meanmag);
              __SET(_v_model_stationary[trainingSetSize].stddevmag , features.stddevmag);
              break;
          case CLASS_MOVING:
              __SET(_v_model_moving[trainingSetSize].meanmag , features.meanmag);
              __SET(_v_model_moving[trainingSetSize].stddevmag , features.stddevmag);
              break;
      }

      ++trainingSetSize;
      __SET(_v_trainingSetSize,trainingSetSize);

      if (trainingSetSize < MODEL_SIZE) {
      P3OUT &= ~BIT0;
          return task_sample;

      } else {

      P3OUT &= ~BIT0;
          return task_idle;
      }

}



unsigned isqr_Newton(unsigned x) {
  unsigned rt[3] = {1,0,0};  // current and previous 2 root candidates

  unsigned x2 = x; // Form initial guess
  while (x2 > rt[0]) {
    x2 >>= 1;
    rt[0] <<= 1;
  }

  do {
    rt[2] = rt[1];
    rt[1] = rt[0];
    rt[0] = (x/rt[0] + rt[0])/2;
    } while (rt[1] != rt[0] && rt[2] != rt[0]);
  return (rt[0] + rt[1])/2;
}
