// This file is part of InK.
// 
// author = "dpatoukas " 
// maintainer = "dpatoukas "
// email = "dpatoukas@gmail.com" 
//  
// copyright = "Copyright 2018 Delft University of Technology" 
// license = "LGPL" 
// version = "3.0" 
// status = "Production"
//
// 
// InK is free software: you ca	n redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//thread2.c
#include "app.h"
#include "ink.h"
#include "dsplib/include/DSPLib.h"
#include "app_intrinsics.h"
#include "adxl345_i2c_lib.h"
#include "interrupts.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

//Classification part expires at:
//#define EXPRIRATION_TIME 8000 //~460ms
//#define EXPRIRATION_TIME 13000 //~1000ms
//#define EXPRIRATION_TIME 6500 //~270ms

// Number of samples to discard before recording training set
#define N_SAMPLES 256
#define EXPODENTIAL 2
__nv uint16_t act = 0;
//walking
const int16_t ACT1_trained_maxZ = 74;
const int16_t ACT1_trained_minZ = -75;
const int16_t ACT1_trained_meanFeature = 281;
const int16_t ACT1_trained_stddevFeature = 76;
  
//talking
const int16_t ACT2_trained_maxZ = 70;
const int16_t ACT2_trained_minZ = -243;
const int16_t ACT2_trained_meanFeature = 199;
const int16_t ACT2_trained_stddevFeature = 170;
  
//standing 
const int16_t ACT3_trained_maxZ = 50;
const int16_t ACT3_trained_minZ = -121;
const int16_t ACT3_trained_meanFeature = 268;
const int16_t ACT3_trained_stddevFeature = 85;

extern uint8_t class_stat;

__shared(
  uint16_t status;
  int16_t array_X[N_SAMPLES];
  int16_t array_Y[N_SAMPLES];
  int16_t array_Z[N_SAMPLES];
  int16_t minFeatureZ;
  int16_t maxFeatureZ;
  uint16_t meanFeature;
  uint16_t stddevFeature;
  uint16_t ACT1_count;
  uint16_t ACT2_count;
  uint16_t ACT3_count;
)



ENTRY_TASK(t_init);
TASK(t_sample);
TASK(t_features);
TASK(t_distance);

void thread2_init(){

    // create a thread with priority 15 and entry task task1
    __CREATE(THREAD2,t_init);

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

int ipow(int base)
{   
    int expod = EXPODENTIAL;
    int result = 1;
    for (;;)
    {
        if (expod & 1)
            result *= base;
        expod >>= 1;
        if (!expod)
            break;
        base *= base;
    }

    return result;
}

__nv uint16_t thread2_count = 0;
#define NUM_BYTES_ACC 6
uint8_t acc_data[NUM_BYTES_ACC];

ENTRY_TASK(t_init){
  
  // P3OUT |= BIT6;
  thread2_count += 1;
  setup_mcu();
  P1IE |= BIT2;                              // P1.2 interrupt enabled high priority thread
  enable_acc_interrupts();
  // P3OUT &= ~BIT6;
  class_stat = 1;
  return t_sample;
}

TASK(t_sample){

  uint8_t first = 1;
  int16_t sampleZ, sampleX, sampleY;
//  int16_t minX, maxX;
//  int16_t minY, maxY;
  int16_t minZ, maxZ;
  uint16_t i;
  for (i = 0; i < N_SAMPLES; i++)
  {
    i2c_read_multi(ADXL_345, READ_REG, NUM_BYTES_RX, &acc_data);

    sampleX = (((int16_t)acc_data[1]) << 8) | acc_data[0];
    sampleY = (((int16_t)acc_data[3]) << 8) | acc_data[2];
    sampleZ = (((int16_t)acc_data[5]) << 8) | acc_data[4];

    if (first)
    {
       // minX = sampleX;
       // maxX = sampleX;

       // maxY = sampleY;
       // minY = sampleY;

       minZ = sampleZ;
       maxZ = sampleZ;

       first = 0;
    }else{

      // if (minX < sampleX)
      // {
      //     minX = sampleX;
      // }
      // if (maxX > sampleX)
      // {
      //     maxX = sampleX; 
      // }
      
      // if (minY < sampleY)
      // {
      //     minY = sampleY;
      // }
      // if (maxY > sampleY)
      // {
      //     maxY = sampleY; 
      // }
      
        //TODO:Fix
      if (minZ < sampleZ)
      {
          minZ = sampleZ;
      }
      if (maxZ > sampleZ)
      {
          maxZ = sampleZ; 
      }
    
    }

    __SET(array_X[i], sampleX);
    __SET(array_Y[i], sampleY);
    __SET(array_Z[i], sampleZ);
  
  }

  // __SET(maxFeatureX, maxX);
  // __SET(minFeatureX, minX);
  // __SET(maxFeatureY, maxY);
  // __SET(minFeatureY, minY);
  __SET(maxFeatureZ, maxZ);
  __SET(minFeatureZ, minZ);


  return t_features;
}


TASK(t_features)
{ 
    uint16_t i;
    // uint8_t divider = N_SAMPLES;
    uint16_t sumX;
    uint16_t sumY;
    uint16_t sumZ;

    for (i = 0; i < N_SAMPLES; i++)
    {
        sumX += __GET(array_X[i]);
        sumY += __GET(array_Y[i]);
        sumZ += __GET(array_Z[i]);
    }

    // __SET(meanX, (sumX >> 8));
    // __SET(meanY, (sumY >> 8));
    // __SET(meanZ, (sumZ >> 8));
    int16_t meanX =  (sumX >> 8);
    int16_t meanY = (sumY >> 8);
    int16_t meanZ = (sumZ >> 8);

    uint16_t stddevZ;
    uint16_t stddevY;
    uint16_t stddevX;
    
    for (i = 0; i < N_SAMPLES; i++)
    {
        if (__GET(array_X[i]) > meanX)
        {
          stddevX += __GET(array_X[i]) - meanX;
        }
        else
        {
          stddevX +=  meanX - __GET(array_X[i]);
        }

        if (__GET(array_Y[i]) > meanY)
        {
          stddevY += __GET(array_Y[i]) - meanY;
        }
        else
        {
          stddevY += meanY - __GET(array_Y[i]);
        }
        
        if (__GET(array_Z[i]) > meanZ)
        {
          stddevZ += __GET(array_Z[i]) - meanZ;
        }
        else
        {
          stddevZ +=  meanZ - __GET(array_Z[i]);
        }
    }

    stddevX >>= 2;
    stddevY >>= 2;
    stddevZ >>= 2;

    unsigned meanmag = meanX*meanX + meanY*meanY + meanZ*meanZ;
    unsigned stddevmag = stddevX*stddevX + stddevY*stddevY + stddevZ*stddevZ;
    __SET(meanFeature, isqr_Newton(meanmag));
    __SET(stddevFeature, isqr_Newton(stddevmag));

    return t_distance;
}


TASK(t_distance)
{

uint16_t  ACT1_eudistance =isqr_Newton(ipow(ACT1_trained_maxZ - __GET(maxFeatureZ)) +
                               ipow(ACT1_trained_meanFeature - __GET(meanFeature)) + 
                               ipow(ACT1_trained_stddevFeature - __GET(stddevFeature))+
                               ipow(ACT1_trained_minZ - __GET(minFeatureZ)));

uint16_t  ACT2_eudistance =isqr_Newton(ipow(ACT2_trained_maxZ - __GET(maxFeatureZ)) +
                               ipow(ACT2_trained_meanFeature - __GET(meanFeature)) + 
                               ipow(ACT2_trained_stddevFeature - __GET(stddevFeature)) +
                               ipow(ACT2_trained_minZ - __GET(minFeatureZ)));

uint16_t  ACT3_eudistance =isqr_Newton(ipow(ACT3_trained_maxZ - __GET(maxFeatureZ)) +
                               ipow(ACT3_trained_meanFeature - __GET(meanFeature)) + 
                               ipow(ACT3_trained_stddevFeature - __GET(stddevFeature)) +
                               ipow(ACT3_trained_minZ - __GET(minFeatureZ)));  
  

 // if (ACT1_eudistance <  ACT2_eudistance )
 // {
 //    if(ACT1_eudistance <  ACT3_eudistance)
 //    {
 //      __SET(ACT1_count,++__GET(ACT1_count));
 //      P2OUT |= BIT5;
 //      P2OUT &= ~BIT5;
    
 //    }
 //    else{ 

 //      __SET(ACT3_count,++__GET(ACT3_count));
 //      P4OUT |= BIT3;
 //      P4OUT &= ~BIT3;

 //    }
 // }
 // else
 // {

 //    if(ACT2_eudistance <  ACT3_eudistance)
 //    {

 //     __SET(ACT2_count,++__GET(ACT2_count));
 //      P2OUT |= BIT6;
 //      P2OUT &= ~BIT6;
 //    }  
   
 //    else {
    
 //      __SET(ACT3_count,++__GET(ACT3_count));
 //      P4OUT |= BIT3;
 //      P4OUT &= ~BIT3;
    
 //    }
 // }
 // 
    act += 1;
  __SET(ACT1_count,++__GET(ACT1_count));

 // P3OUT &= ~BIT6;
 if (class_stat == 1)
 {
   return t_init;
 }
 else 
 {
   __SIGNAL(THREAD1);
   return NULL;
 }

}
