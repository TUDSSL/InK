// This file is part of InK.
// 
// author = "Kasım Sinan Yıldırım " 
// maintainer = "Kasım Sinan Yıldırım "
// email = "sinanyil81 [at] gmail.com" 
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
// along with this program.  If not, see <http://www.gnu.org/licenses/>./*
 /* flush.c
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
