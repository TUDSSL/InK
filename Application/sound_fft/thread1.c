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

//This application samples a microphone via ADC 
//ADC pins and hardware must be configured according to the used device
//
//
//If no REAL_MIC is provided a virtual sample will be used
#include <msp430.h>
#include "app.h"
#include "ink.h"
#include "dsplib/include/DSPLib.h"

/*FFT characteristics definitions*/
#define FFT_SAMPLES             256
#define INPUT_AMPLITUDE         0.5
#define INPUT_FREQUENCY         64
#define SAMPL_FREQ              512
#define FFT_FREQ_THRESHOLD      1500
#define REAL_ADC_FREQ           1000000 / (4 * 4 * 16)
#define INDEX_THRESHOLD         FFT_FREQ_THRESHOLD / (REAL_ADC_FREQ / (2 * FFT_SAMPLES))


#ifdef REAL_MIC
/* Allocate 16-bit real FFT data vector with correct alignment */
DSPLIB_DATA(tb_fftd, MSP_ALIGN_FFT_Q15(FFT_SAMPLES))
 _q15 tb_fftd[FFT_SAMPLES];

__shared(

  _q15 pers_sdata[FFT_SAMPLES]

)

#else
DSPLIB_DATA(tb_fftd, MSP_ALIGN_FFT_Q15(FFT_SAMPLES))
__nv _q15 tb_fftd[FFT_SAMPLES] = {
     -9634,  32767,  19555,  10950,   4736,   9785,  -7267, -11995,
     -3027,  16331,  -6896, -14342,  -5820,  -7920, -18265, -13059,
      8401,  25561,   8439,   1384,  18253,   6700, -12424,  -5112,
     13543,  11971,  -3546,  -9726,  -5575, -17249, -27358,  -2533,
     12940,  21431,   -195,  13989,  17916,   -917, -14959, -11988,
     16960,  -5800, -23360, -10399, -11280, -18164, -18909,   7518,
     21235,  18131,   6905,  16367,  13048,  -4587, -13190,   6038,
     17784, -13350, -16788, -13129, -12414, -21115, -10926,  10118,
     21871,  11366,   6463,  15922,   5996, -17453,  -7421,   3532,
     13302, -13445, -17979,  -7560, -22309, -24273,  -6252,  31085,
     24774,  -3417,  12622,   7503,  -4640, -13781,   4571,  13492,
      7644, -16293,  -8240,  -5284, -21173, -16059,   8996,  21233,
      6603,  14525,  11112,   9318,  -8076, -10524,   5138,  10194,
     -7234,  -8000, -10695,  -9121, -23149, -12176,  12841,  21670,
      8944,   2334,  15443,  -1785, -14722,  -2664,   9938,   4850,
    -10809, -19666,  -2266,  -7774, -19165,  -1347,  25085,  13696,
     14694,  15847,  15660,  -9817, -12434,   1716,  12991,  -4154,
    -19000, -12479, -10210, -30129, -14759,  10675,  27447,  13617,
      9130,   7982,   2348, -14096,  -8530,  15822,   9241,  -3497,
    -11664,  -1590, -13763, -27381, -13937,  20479,  25026,   6870,
      7329,  12924,  -5599, -16760,  -2154,  14677,   3279, -10860,
    -10274,    503, -18588, -14100,    775,  26429,  15104,   9035,
     15376,   1436, -14078, -18774,   8380,  19757,  -5685, -11171,
     -5382, -15268, -22186, -17656,  20915,  23742,  17760,   5895,
     17149,   4022, -22111, -10856,  15239,   8182, -15534, -15516,
     -5412, -20451, -20025,  -6593,  29512,  15312,   8083,   4251,
     10380,  -6083, -12770,   5401,  15938,   -671, -10763,  -6229,
     -7606, -19099, -18056,   3407,  27818,  11500,   4021,  14445,
      7566, -11041,  -8469,  13462,  11344,  -5303, -15325,  -6044,
    -10591, -27097, -10006,  19913,  23276,  16692,   6006,  11480,
     -5742, -10437,   -400,  13456,   9912, -11519, -10595,  -6203,
    -16239, -22824,  11889,  18280,  10205,   1818,   7171,  10452
};

#endif
void ADC_config();


ENTRY_TASK(task1);
TASK(task2);

void thread1_init(){

    // create a thread with priority " " and entry task task1
    __CREATE(THREAD1,task1);
    __SIGNAL(THREAD1);
}

__app_reboot(){

}

uint16_t counter;
_q15 sampled_input[FFT_SAMPLES];
ENTRY_TASK(task1){

    //TODO:needed?
    __delay_cycles(100000);
  
#ifdef REAL_MIC
	// save interrupt state and then disable interrupts
	uint16_t is = __get_interrupt_state();
	__disable_interrupt();

	// configure ADC
	ADC_config();

	counter = 0;

	//why enable here and restore later
	__enable_interrupt();

	while(counter < FFT_SAMPLES);

	// turn off the ADC to save energy
	ADC12CTL0 &= ~ADC12ON;

	// restore interrupt state
	__set_interrupt_state(is);

#endif

  return task2;
}
   
TASK(task2){

  msp_status status;
#ifdef REAL_MIC
  uint16_t i;
  for (i = 0; i < FFT_SAMPLES; i++)
  {
    tb_fftd[i] = __GET(pers_sdata[i]);
  }
#endif
  //  /* Configure parameters for FFT */
  msp_fft_q15_params fftParams;
  msp_abs_q15_params absParams;
  msp_max_q15_params maxParams;

  fftParams.length = FFT_SAMPLES;
  fftParams.bitReverse = 1;
  //fftParams.twiddleTable = NULL; //msp_cmplx_twiddle_table_16_q15;
#if !defined(MSP_USE_LEA)
        fftParams.twiddleTable = msp_cmplx_twiddle_table_256_q15;
#else
        fftParams.twiddleTable = NULL;
#endif
  absParams.length = FFT_SAMPLES;

  maxParams.length = FFT_SAMPLES;

  uint16_t shift = 0;
  uint16_t max_index = 0;

  /* Perform FFT */
  status = msp_fft_auto_q15(&fftParams, tb_fftd, &shift);
  //msp_checkStatus(status);

  /* Remove DC component  */
  tb_fftd[0] = 0;

  /* Compute absolute value of FFT */
  status = msp_abs_q15(&absParams, tb_fftd, tb_fftd);
  //msp_checkStatus(status);

  /* Get peak frequency */
  status = msp_max_q15(&maxParams, tb_fftd, NULL, &max_index);
  //msp_checkStatus(status);

  __no_operation();

/* Turn on red LED if peak frequency greater than threshold */
//  uint16_t tmp = 1000;
//  if (max_index > INDEX_THRESHOLD)
//  {
//      //RED ON-GREEN OFF
//      P1OUT |= BIT0;
//      P4OUT &= BIT6;
//  }
//  else
//  {
//      P4OUT |= BIT6;
//      while(tmp--)
//      {
//       _delay_cycles(50000);
//        P1OUT ^= BIT0;
//      }
//  }
 __SIGNAL(THREAD1);

 return NULL;

}

/**
 * Configure ADC for microphone sampling
 */
void ADC_config()
{
    //TODO:free 1.3
    // Pin P1.3 set for Ternary Module Function (which includes A3)
    P1SEL0 |= BIT3;
    P1SEL1 |= BIT3;

    // Clear ENC bit to allow register settings
    ADC12CTL0 &= ~ADC12ENC;

    // Sample-and-hold source select
    //
    // 000 -> ADC12SC bit (default)
    // ADC12CTL1 &= ~(ADC12SHS0 | ADC12SHS1 | ADC12SHS2);

    // Clock source select
    //
    // source: MCLK (DCO, 1 MHz)
    // pre-divider: 4
    // divider: 4
    ADC12CTL1 |= ADC12SSEL_2 | ADC12PDIV_1 | ADC12DIV_3;

    // sampling period select for MEM0: 64 clock cycles (*)
    // multiple sample and conversion: enabled
    // ADC module ON
    ADC12CTL0 |= ADC12SHT0_2 | ADC12MSC | ADC12ON;
    // (*) freq = MCLK / (ADC12PDIV_0 * ADC12DIV_0 * ADC12SHT0_4)
    //          = 1000000 / (4 * 4 * 16)
    //          = 3906 Hz

    // conversion sequence mode: repeat-single-channel
    // pulse-mode select: SAMPCON signal is sourced from the sampling timer
    ADC12CTL1 |= ADC12CONSEQ_2 | ADC12SHP;

    // resolution: 12 bit
    // data format: right-aligned, unsigned
    ADC12CTL2 |= ADC12RES__12BIT ;//| ADC12DF_0;
    ADC12CTL2 &= ~ADC12DF;
    // conversion start address: MEM0
    ADC12CTL3 |= ADC12CSTARTADD_0;

    // MEM0 control register
    // reference select: VR+ = AVCC (3V), VR- = AVSS (0V)
    // input channel select: A3
    ADC12MCTL0 |= ADC12VRSEL_0 | ADC12INCH_3;

    // Clear interrupt for MEM0
    ADC12IFGR0 &= ~ADC12IFG0;

    // Enable interrupt for (only) MEM0
    ADC12IER0 = ADC12IE0;

    // Trigger first conversion (Enable conversion and Start conversion)
    ADC12CTL0 |= ADC12ENC | ADC12SC;
}

/**
 * ADC Interrupt handler
 */

_interrupt(ADC12_VECTOR)
{
    // static uint16_t i = 0;
    switch(__even_in_range(ADC12IV,12))
    {
    case  0: break;                         // Vector  0:  No interrupt
    case  2: break;                         // Vector  2:  ADC12BMEMx Overflow
    case  4: break;                         // Vector  4:  Conversion time overflow
    case  6: break;                         // Vector  6:  ADC12BHI
    case  8: break;                         // Vector  8:  ADC12BLO
    case 10: break;                         // Vector 10:  ADC12BIN
    case 12:                                // Vector 12:  ADC12BMEM0 Interrupt
        if (counter < FFT_SAMPLES)
            // Read ADC12MEM0 value
            sampled_input[counter++] = ADC12MEM0;
        else {
            // disable ADC conversion and disable interrupt request for MEM0
            ADC12CTL0 &= ~ADC12ENC;
            ADC12IER0 &= ~ADC12IE0;
        }
        break;
    case 14: break;                         // Vector 14:  ADC12BMEM1
    case 16: break;                         // Vector 16:  ADC12BMEM2
    case 18: break;                         // Vector 18:  ADC12BMEM3
    case 20: break;                         // Vector 20:  ADC12BMEM4
    case 22: break;                         // Vector 22:  ADC12BMEM5
    case 24: break;                         // Vector 24:  ADC12BMEM6
    case 26: break;                         // Vector 26:  ADC12BMEM7
    case 28: break;                         // Vector 28:  ADC12BMEM8
    case 30: break;                         // Vector 30:  ADC12BMEM9
    case 32: break;                         // Vector 32:  ADC12BMEM10
    case 34: break;                         // Vector 34:  ADC12BMEM11
    case 36: break;                         // Vector 36:  ADC12BMEM12
    case 38: break;                         // Vector 38:  ADC12BMEM13
    case 40: break;                         // Vector 40:  ADC12BMEM14
    case 42: break;                         // Vector 42:  ADC12BMEM15
    case 44: break;                         // Vector 44:  ADC12BMEM16
    case 46: break;                         // Vector 46:  ADC12BMEM17
    case 48: break;                         // Vector 48:  ADC12BMEM18
    case 50: break;                         // Vector 50:  ADC12BMEM19
    case 52: break;                         // Vector 52:  ADC12BMEM20
    case 54: break;                         // Vector 54:  ADC12BMEM21
    case 56: break;                         // Vector 56:  ADC12BMEM22
    case 58: break;                         // Vector 58:  ADC12BMEM23
    case 60: break;                         // Vector 60:  ADC12BMEM24
    case 62: break;                         // Vector 62:  ADC12BMEM25
    case 64: break;                         // Vector 64:  ADC12BMEM26
    case 66: break;                         // Vector 66:  ADC12BMEM27
    case 68: break;                         // Vector 68:  ADC12BMEM28
    case 70: break;                         // Vector 70:  ADC12BMEM29
    case 72: break;                         // Vector 72:  ADC12BMEM30
    case 74: break;                         // Vector 74:  ADC12BMEM31
    case 76: break;                         // Vector 76:  ADC12BRDY
    default: break;
    }
}
