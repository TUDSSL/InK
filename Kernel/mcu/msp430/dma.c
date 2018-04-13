/*
 * flush.c
 *
 *  Created on: 14 Feb 2018
 *
 */
#include "msp430.h"

// size should be in words
void __dma_word_copy(unsigned int from, unsigned int to, unsigned short size)
{
    // Configure DMA channel 0
    __data16_write_addr((unsigned short) &DMA0SA,(unsigned long) from);
                                              // Source block address
    __data16_write_addr((unsigned short) &DMA0DA,(unsigned long) to);
                                              // Destination single address
    DMA0SZ = size;                          // Block size
    DMA0CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA0CTL |= DMAEN;                         // Enable DMA0

    DMA0CTL |= DMAREQ;                      // Trigger block transfer
}
