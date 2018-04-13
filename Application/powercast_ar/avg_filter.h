//cm_acc Header
#ifndef AVG_FILTER_H_
#define AVG_FILTER_H_

#include <stdio.h>
/* Size of the data buffer; length of the sequence. */
#define NWIDTH 3
/* Smaller than any datum */ 
#define STOPPER 0 

unsigned int medfilter(unsigned int datum);

#endif

