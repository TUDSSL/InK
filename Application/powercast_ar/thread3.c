#include "app.h"
#include "ink.h"
#include "dsplib/include/DSPLib.h"
#include "app_intrinsics.h"

  /* Allocate 16-bit real FFT data vector with correct alignment */
DSPLIB_DATA(tb_fftd, MSP_ALIGN_FFT_Q15(FFT_SAMPLES))
 _q15 tb_fftd[FFT_SAMPLES];

#define ROUNDS 8
#define INDEX_BOUND 30
#define POWER_BOUND 50
__shared(

  _q15 pers_sdata[FFT_SAMPLES];
  uint16_t max_array[ROUNDS];
  uint16_t power_array[ROUNDS];
  uint8_t sample_count;
)

ENTRY_TASK(t_init);
TASK(t_sample);
TASK(task2);
TASK(t_consesus);

extern  uint8_t class_stat;

void thread3_init(){

    // create a thread with priority " " and entry task task1
    __CREATE(THREAD3,t_init);

}

uint16_t counter;
uint16_t is;
_q15 sampled_input[FFT_SAMPLES];
__nv uint16_t thread3_count = 0;
__nv uint16_t transition_count = 0;

ENTRY_TASK(t_init)
{
  thread3_count +=1;
  // class_stat = !class_stat ;

  // __SIGNAL(THREAD2);

  // return NULL;  
  if (__GET(sample_count) < ROUNDS)
  {
    return t_sample;
  }
  else
  {
    return t_consesus;
  }
}


TASK(t_sample){
  
  P3OUT |= BIT5;

  __disable_interrupt();
	// save interrupt state and then disable interrupts
	is = __get_interrupt_state();

	// configure ADC
	ADC_config();

	counter = 0;

	__enable_interrupt();

	while(counter < FFT_SAMPLES);
	
	// disable interrupt for (only) MEM0
	ADC12IER0 &= ~ADC12IE0;


	// turn off the ADC to save energy
	ADC12CTL0 &= ~ADC12ON;
  
	// restore interrupt state
	__set_interrupt_state(is);
 	uint16_t i;
	for (i = 0; i < FFT_SAMPLES ; i++)
	{
    __SET(pers_sdata[i], sampled_input[i]);
	}

  P3OUT &= ~BIT5;
	return task2;
}

TASK(task2){

  P3OUT |= BIT5;
  msp_status status;

  uint16_t i;
  for (i = 0; i < FFT_SAMPLES; i++)
  {
    tb_fftd[i] = __GET(pers_sdata[i]);
  }
  //  /* Configure parameters for FFT */
  msp_fft_q15_params fftParams;
  msp_abs_q15_params absParams;
  msp_max_q15_params maxParams;

  fftParams.length = FFT_SAMPLES;
  fftParams.bitReverse = 1;
  fftParams.twiddleTable = NULL;
#if !defined(MSP_USE_LEA)
  fftParams.twiddleTable = msp_cmplx_twiddle_table_128_q15;
#else
  fftParams.twiddleTable = NULL;
#endif

  absParams.length = FFT_SAMPLES;

  maxParams.length = FFT_SAMPLES;

  uint16_t shift = 0;
  uint16_t max_index = 0;

  /* Perform FFT */
  status = msp_fft_auto_q15(&fftParams, tb_fftd, &shift);

  /* Remove DC component  */
  tb_fftd[0] = 0;

  /* Compute absolute value of FFT */
  status = msp_abs_q15(&absParams, tb_fftd, tb_fftd);
  //msp_checkStatus(status);

  /* Get peak frequency */
  status = msp_max_q15(&maxParams, tb_fftd, NULL, &max_index); 

  // if (max_index > START_BOUND)
  // {
  //     //execution path 1   
  // }
  // else
  // {
  //     //execution path 2
  // }

  __SET(max_array[__GET(sample_count)],max_index);
  __SET(power_array[__GET(sample_count)],tb_fftd[max_index]);
  __SET(sample_count,++__GET(sample_count));
  // __SIGNAL(THREAD1);
  
//  P1IE |= BIT2;                              // P1.2 interrupt enabled
  P3OUT &= ~BIT5;

 return t_init;

}

TASK(t_consesus)
{
  P3OUT |= BIT5;

  uint16_t sumPow = 0;
  uint16_t sumIdx = 0;
  uint8_t i;
  for (i = 0; i < 8; i++)
  {
    sumPow +=(uint16_t) __GET(power_array[i]);
    sumIdx +=(uint16_t) __GET(max_array[i]);
  }
  
  uint16_t avgPow;
  avgPow = (sumPow / 8);
  uint16_t avgIdx;
  avgIdx = (sumIdx / 8 );
  
  if (avgPow>POWER_BOUND && avgIdx>INDEX_BOUND)
  {
    if (class_stat == 1)
    {
      __SIGNAL(THREAD1);

    }else
    {
        transition_count += 1;
      __SIGNAL(THREAD2);
    }
  }else{

    if (class_stat == 1)
    {
      __SIGNAL(THREAD2);

    }else
    {
        transition_count += 1;
      __SIGNAL(THREAD1);
    }
  }


  // if ( avgPow > POWER_BOUND )
  // {
  //   if (avgIdx > INDEX_BOUND)
  //   {
  //       if (class_stat == 1)
  //       {
  //         class_stat = 0;
  //         __SIGNAL(THREAD1);
  //       }
  //       else
  //       {
  //         __SIGNAL(THREAD2);
  //       }
  //   }
  //   else
  //   {
  //     if (class_stat == 1)
  //       {
  //         __SIGNAL(THREAD2);
  //       }
  //   }
  // }
  // else
  // {
  //     if (class_stat == 1)
  //     {
  //       __SIGNAL(THREAD2);
  //     }
  //     else
  //     {
  //       __SIGNAL(THREAD1);
  //     }
  // }
  __SET(sample_count,0);
     P3OUT &= ~BIT5;

  return NULL;
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
