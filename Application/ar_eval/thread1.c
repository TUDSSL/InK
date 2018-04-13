#include "ink.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "msp-math.h"
#define RAISE_PIN

// Number of samples to discard before recording training set
#define NUM_WARMUP_SAMPLES 3

#define ACCEL_WINDOW_SIZE 3
#define MODEL_SIZE 16
#define SAMPLE_NOISE_FLOOR 10 // TODO: made up value

// Number of classifications to complete in one experiment
#define SAMPLES_TO_COLLECT 128

typedef struct 
{
    uint8_t x,y,z;

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

__shared(
  uint8_t pinCont;
  uint16_t _v_pinState;
  // unsigned _v_seed;
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


ENTRY_TASK(task_init);
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

#define THREAD1 15
void thread1_init(){

    // create a thread with priority 15 and entry task task1
    __CREATE(THREAD1,task_init);
    __SIGNAL(THREAD1);

}

__nv unsigned _v_seed;
__nv uint8_t full_run_started;

__app_reboot(){
    __no_operation();
}

ENTRY_TASK(task_init)
{
  
#ifdef RAISE_PIN
    full_run_started = 1;
#endif

  __SET(pinCont, 1);
  __SET(_v_pinState , MODE_IDLE);
  __SET( _v_count , 0);
  // __SET( _v_seed , 1);
  _v_seed = 1;

  return task_selectMode;
}

//Dummy data sampling
//Dummy data sampling
void ACCEL_singleSample_(threeAxis_t_8* result){

    unsigned seed = _v_seed;

    result->x = (seed*17)%85;
    result->y = (seed*17*17)%85;
    result->z = (seed*17*17*17)%85;
    _v_seed = ++seed;
}

TASK(task_selectMode)
{

  // uint16_t pinState = __GET(_v_pinState);
  unsigned discardedSamplesCount = __GET(_v_discardedSamplesCount);
  unsigned samplesInWindow = __GET(_v_samplesInWindow);
  unsigned count = __GET(_v_count);
  class_t  lc_class = __GET(_v_class);
  run_mode_t lc_mode = __GET(_v_mode);
  
  uint16_t pin_state=1;

  ++count;
  __SET(_v_count,count);
  
  if(count >= 3)  pin_state=2;
  if(count >= 5)  pin_state=0;
  if(count >= 7) 
  {
    
#ifdef RAISE_PIN
    if (full_run_started) 
    {
      full_run_started = 0;
      __port_on(3, 4);
      __port_off(3, 4);
    }
#endif
      return task_init;
  }

  
  
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
        return task_warmup;
        break;
    case MODE_TRAIN_MOVING:
        __SET(_v_discardedSamplesCount , 0);
        __SET(_v_mode , MODE_TRAIN_MOVING);
        __SET(_v_class , CLASS_MOVING);
        __SET(_v_samplesInWindow , 0);          
        return task_warmup;
        break;
    case MODE_RECOGNIZE:
        __SET(_v_mode , MODE_RECOGNIZE);
        return task_resetStats;
        break;
    default:
        return task_idle;
        break;
      }
}

TASK(task_warmup)
{
  
  // unsigned discardedSamplesCount = __GET(_v_discardedSamplesCount);
  // unsigned trainingSetSize= __GET(_v_trainingSetSize);
      
    threeAxis_t_8 sample;
    

    if (__GET(_v_discardedSamplesCount) < NUM_WARMUP_SAMPLES) {

        ACCEL_singleSample_(&sample);
        __SET(_v_discardedSamplesCount , ++__GET(_v_discardedSamplesCount));
        return task_warmup;
    } else {

        __SET(_v_trainingSetSize , 0);
        return task_sample;
    }

}

TASK(task_sample)
{

//    int8_t ios_i;
    
  unsigned samplesInWindow = __GET(_v_samplesInWindow);
    

      accelReading sample;
      ACCEL_singleSample_(&sample);
      __SET(_v_window[samplesInWindow].x , sample.x);
      __SET(_v_window[samplesInWindow].y , sample.y);
      __SET(_v_window[samplesInWindow].z , sample.z);
      ++samplesInWindow;
      __SET(_v_samplesInWindow,samplesInWindow);

      if (samplesInWindow < ACCEL_WINDOW_SIZE) 
      {
          return task_sample;
      } 
      else 
      {
          __SET(_v_samplesInWindow , 0);
          return task_transform;
      }

}

TASK(task_transform)
{

  int8_t ios_i;
    
  // unsigned samplesInWindow = __GET(_v_samplesInWindow);


  accelReading window[ACCEL_WINDOW_SIZE];

  for (ios_i = 0; ios_i < ACCEL_WINDOW_SIZE; ++ios_i)
  {
    window[ios_i] = __GET(_v_window[ios_i]);
  }
      

    unsigned i;

  for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {

      if (window[i].x < SAMPLE_NOISE_FLOOR ||
              window[i].y < SAMPLE_NOISE_FLOOR ||
              window[i].z < SAMPLE_NOISE_FLOOR)
      {

          if (window[i].x > SAMPLE_NOISE_FLOOR)
          {
            __SET(_v_window[i].x,window[i].x);
          } 
          else
          {
             __SET(_v_window[i].x,0);
          }
  
          if(window[i].y > SAMPLE_NOISE_FLOOR) 
          {
              __SET(_v_window[i].y,window[i].y);
          }
          else
          {
             __SET(_v_window[i].y,0);
          }
  
          if(window[i].z > SAMPLE_NOISE_FLOOR)
          {
           __SET(_v_window[i].z,window[i].z);
          }
          else
          {
            __SET(_v_window[i].z,0);
          } 
      }
  }

    return task_featurize;      

}

TASK(task_featurize)
{
  
//    int8_t ios_i;
    
    run_mode_t mode = __GET(_v_mode);
    features_t features = __GET(_v_features);

//
//    accelReading window[ACCEL_WINDOW_SIZE];
//
//   for (ios_i = 0; ios_i < ACCEL_WINDOW_SIZE; ++ios_i)
//    {
//      window[ios_i] = __GET(_v_window[ios_i]);
//  }
//

    accelReading mean, stddev;
    mean.x = mean.y = mean.z = 0;
    stddev.x = stddev.y = stddev.z = 0;


    uint8_t i = 0;
    for (i = 0; i < ACCEL_WINDOW_SIZE; i++)
    {
        mean.x += __GET(_v_window[i].x);
        mean.y += __GET(_v_window[i].y);
        mean.z += __GET(_v_window[i].z);
    }
    
    mean.x >>= 2;
    mean.y >>= 2;
    mean.z >>= 2;

    for (i = 0; i < ACCEL_WINDOW_SIZE; i++)
    {
        if (__GET(_v_window[i].x) > mean.x)
        {
          stddev.x += __GET(_v_window[i].x) - mean.x;
        }
        else
        {
          stddev.x +=  mean.x - __GET(_v_window[i].x);
        }

        if (__GET(_v_window[i].y) > mean.y)
        {
          stddev.y += __GET(_v_window[i].y) - mean.y;
        }
        else
        {
          stddev.y += mean.y - __GET(_v_window[i].y);
        }
        
        if (__GET(_v_window[i].z) > mean.z)
        {
          stddev.z += __GET(_v_window[i].z) - mean.z;
        }
        else
        {
          stddev.z +=  mean.z - __GET(_v_window[i].z);
        }

        // stddev.x += window[i].x > mean.x ? window[i].x - mean.x
        //     : mean.x - window[i].x;
        // stddev.y += window[i].y > mean.y ? window[i].y - mean.y
        //     : mean.y - window[i].y;
        // stddev.z += window[i].z > mean.z ? window[i].z - mean.z
        //     : mean.z - window[i].z;
    }
    stddev.x >>= 2;
    stddev.y >>= 2;
    stddev.z >>= 2;

    unsigned meanmag = mean.x*mean.x + mean.y*mean.y + mean.z*mean.z;
    unsigned stddevmag = stddev.x*stddev.x + stddev.y*stddev.y + stddev.z*stddev.z;
    features.meanmag   = sqrt16(meanmag);
    features.stddevmag = sqrt16(stddevmag);

    switch (mode)
     {
        case MODE_TRAIN_STATIONARY:
        case MODE_TRAIN_MOVING:
            __SET(_v_features.meanmag , features.meanmag);
            __SET(_v_features.stddevmag , features.stddevmag);
            return task_train;
            break;
        case MODE_RECOGNIZE:
            __SET(_v_features.meanmag , features.meanmag);
            __SET(_v_features.stddevmag , features.stddevmag);
            return task_classify;
            break;
        default:
            // TODO: abort
            __no_operation();
            break;
    }
}

TASK(task_classify)
{

    // int8_t ios_i;
    
    // class_t lc_class = __GET(_v_class);
    // features_t features = __GET(_v_features);

    // accelReading window[ACCEL_WINDOW_SIZE];

    // for (ios_i = 0; ios_i < ACCEL_WINDOW_SIZE; ios_i++)
    // {
    //     window[ios_i] =
    //             __GET(_v_window[ios_i]);
    // }

    // features_t model_stationary[MODEL_SIZE];
    // for (ios_i = 0; ios_i < MODEL_SIZE; ios_i++)
    // {
    //   model_stationary[ios_i] =
    //             __GET(_v_model_stationary[ios_i]);
    // }    

    // features_t model_moving[MODEL_SIZE];
    // for (ios_i = 0; ios_i < MODEL_SIZE; ios_i++)
    // {
    //     model_moving[ios_i] =
    //             __GET(_v_model_moving[ios_i]);
    // }    
    
    unsigned int move_less_error = 0;
    unsigned int stat_less_error = 0;
    int i;

    unsigned int meanmag;
    unsigned int stddevmag;
    meanmag = __GET(_v_features.meanmag);
    stddevmag = __GET(_v_features.stddevmag);

  unsigned int stat_mean_err = 0;
  unsigned int stat_sd_err = 0;
  unsigned int move_mean_err = 0;
  unsigned int move_sd_err = 0;

    for (i = 0; i < MODEL_SIZE; ++i) 
    {

      if (__GET(_v_model_stationary[i].meanmag) > meanmag)
      {
         stat_mean_err = __GET(_v_model_stationary[i].meanmag) - meanmag; 
        
      }
      else
      {
          stat_mean_err = meanmag - __GET(_v_model_stationary[i].meanmag);
      }
 
      if (__GET(_v_model_stationary[i].stddevmag) > stddevmag)
      {
         stat_sd_err = __GET(_v_model_stationary[i].stddevmag) - stddevmag; 
        
      }
      else
      {
          stat_sd_err = stddevmag - __GET(_v_model_stationary[i].stddevmag);
      }
      
      unsigned int tmp;
      tmp = __GET(_v_model_stationary[i].meanmag);

      if (tmp > meanmag)
      {
         move_mean_err = (tmp - meanmag);
        
      }
      else
      {
          move_mean_err = meanmag - tmp;
      } 
       
      if (__GET(_v_model_stationary[i].stddevmag) > stddevmag)
      {
         move_sd_err = __GET(_v_model_stationary[i].stddevmag) - stddevmag; 
        
      }
      else
      {
          move_sd_err = stddevmag - __GET(_v_model_stationary[i].stddevmag);
      }  
        
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

    if (move_less_error > stat_less_error) 
    {
        __SET(_v_class, CLASS_MOVING);
    }
    else
    {
        __SET(_v_class,CLASS_STATIONARY);
    }

    return task_stats;
}

TASK(task_stats)
{
  unsigned movingCount = 0, stationaryCount = 0;


  // unsigned totalCount = __GET(_v_totalCount);    
  __SET(_v_totalCount,++__GET(_v_totalCount));
  // unsigned movingCount = __GET(_v_movingCount);
  // unsigned stationaryCount = __GET(_v_stationaryCount);        
  // class_t lc_class = __GET(_v_class);

    // ++totalCount;
    // __SET(_v_totalCount,totalCount);

    switch (__GET(_v_class))
    {
        case CLASS_MOVING:

            __SET(_v_movingCount,++__GET(_v_movingCount));
            break;
        case CLASS_STATIONARY:

            __SET(_v_stationaryCount, ++__GET(_v_stationaryCount));
            break;
    }

    if ( __GET(_v_totalCount) == SAMPLES_TO_COLLECT) {

//        unsigned resultStationaryPct = stationaryCount * 100 / totalCount;
//        unsigned resultMovingPct = movingCount * 100 / totalCount;

//        unsigned sum = stationaryCount + movingCount;
         // printf("stats: s %u (%u%%) m %u (%u%%) sum/tot %u/%u: %c\r\n",
         //        _v_stationaryCount, resultStationaryPct,
         //        _v_movingCount, resultMovingPct,
         //        _v_totalCount, sum, sum == _v_totalCount ? 'V' : 'X');
         //printf("%d,%d,%d\n",_v_totalCount,_v_stationaryCount,_v_movingCount);
        return task_idle;
    } else {
        return task_sample;
    }
}


TASK(task_idle) 
{

//   uint8_t lc_pinCont = __GET(pinCont);
//   #ifdef EXPRIRATION_TIME
//   stop_expire_timer(THREAD2);
//   P2OUT |= BIT5;
//   P2OUT &= ~BIT5;
//   #endif
//   P1IE |= BIT4;                               // P1.4 interrupt enabled


//     if (lc_pinCont){
//       __SET(pinCont,0);
//       __SIGNAL(THREAD1);
// #ifdef RAISE_PIN
//         __port_on(3, 4);
//         __port_off(3, 4);
// #endif
//     __SIGNAL(THREAD1);
//     }

  return task_selectMode;
}

TASK(task_resetStats)
{ 
      __SET(_v_movingCount , 0);
      __SET(_v_stationaryCount , 0);
      __SET(_v_totalCount , 0);

      __SET(_v_samplesInWindow , 0);

      return task_sample;
}


TASK(task_train)
{

    // int8_t ios_i;
    
    unsigned trainingSetSize = __GET(_v_trainingSetSize);
    // class_t lc_class= __GET(_v_class);
    // features_t features = __GET(_v_features);
    // features_t model_moving[MODEL_SIZE],model_stationary[MODEL_SIZE];
    
    // for (ios_i = 0; ios_i < MODEL_SIZE; ios_i++)
    // {
    //   model_stationary[ios_i] =
    //             __GET(_v_model_stationary[ios_i]);
    // }    


    // for (ios_i = 0; ios_i < MODEL_SIZE; ios_i++)
    // {
    //     model_moving[ios_i] =
    //             __GET(_v_model_moving[ios_i]);
    // }    
    
      switch (__GET(_v_class)) 
      {
          case CLASS_STATIONARY:
              __SET(_v_model_stationary[trainingSetSize].meanmag , __GET(_v_features.meanmag));
              __SET(_v_model_stationary[trainingSetSize].stddevmag , __GET(_v_features.stddevmag));
              break;
          case CLASS_MOVING:
              __SET(_v_model_moving[trainingSetSize].meanmag , __GET(_v_features.meanmag));
              __SET(_v_model_moving[trainingSetSize].stddevmag , __GET(_v_features.stddevmag));
              break;
      }

      ++trainingSetSize;
      __SET(_v_trainingSetSize,trainingSetSize);

      if (__GET(_v_trainingSetSize) < MODEL_SIZE) 
      {
          return task_sample;

      } 
      else 
      {

          return task_idle;
      }

}

