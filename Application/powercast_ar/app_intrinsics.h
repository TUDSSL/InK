#include "ink.h"
//#define EMULATE 13

//cm_acc Header
#ifndef APP_INTR_H_
#define APP_INTR_H_

/*FFT characteristics definitions for sound detection thread*/
#define FFT_SAMPLES             256
#define FFT_FREQ_THRESHOLD      1500
#define REAL_ADC_FREQ           1000000 / (4 * 4 * 16)
#define INDEX_THRESHOLD         FFT_FREQ_THRESHOLD / (REAL_ADC_FREQ / (2 * FFT_SAMPLES))

//delay(us) to simulate hardware blocks
//values from logic analyzer traces with Ink 
#define FILTER_DURATION 5714
#define SAMPLE_MIC_DURATION 70830
#define FFT_MIC_DURATION 53660
#define FFT_ACC_DURATION 42000 
#define SINGLE_ACC_SAMPLE_DURATION 2000
#define SAMPLE_ACC_DURATION 128300

//filter bound 
#define BOUND 2000

//high priority signal filter
uint8_t valid_signal(void);

//setup ADC
void ADC_config();

//mcu settings 
void setup_mcu();

#endif //APP_INTR_H_
