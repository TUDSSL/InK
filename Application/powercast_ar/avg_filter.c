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

/* medfilt_new.c    Median filter in C*/

#include "avg_filter.h"
/* Size of the data buffer; length of the sequence. */
#define NWIDTH 3
/* Smaller than any datum */ 
#define STOPPER 0 

unsigned int medfilter(unsigned int datum)
{
    unsigned int i;
    struct pair
    {
        /* Pointers forming list linked in sorted order */
        struct pair   *point;
        /* Values to sort */
        unsigned int  value; 
    };
  /* Buffer of nwidth pairs */
    static struct pair buffer[NWIDTH];
  /* pointer into circular buffer of data */        
    static struct pair *datpoint={buffer};
  /* chain stopper. */
    static struct pair small={NULL,STOPPER} ;
  /* pointer to head (largest) of linked list.*/
    static struct pair big={&small,0} ;
  /* pointer to successor of replaced data item */   
    struct pair *successor   ;
  /* pointer used to scan down the sorted list */                
    struct pair *scan        ;
  /* previous value of scan */                
    struct pair *scanold     ;
  /* pointer to median */                
    struct pair *median;     ;


  /* No stoppers allowed. */
    if(datum == STOPPER) datum = STOPPER + 1;
  /* increment and wrap data in pointer.*/     
    if( (++datpoint - buffer) >= NWIDTH) datpoint=buffer;
  /* Copy in new datum */
    datpoint->value=datum        ;
  /* save pointer to old value's successor */          
    successor=datpoint->point    ;
  /* median initially to first in chain */
    median = &big                ;
  /* scanold initially null. */
    scanold = NULL               ;
  /* points to pointer to first (largest) datum in chain */
    scan = &big
  /* Handle chain-out of first item in chain as special case */             ;
        if( scan->point == datpoint ) scan->point = successor;
       /* Save this pointer and   */
        scanold = scan ;
       /* step down chain */
        scan = scan->point ;

   /* loop through the chain, normal loop exit via break. */
    for( i=0 ;i<NWIDTH ; i++ )
    {
     /* Handle odd-numbered item in chain  */
      /* Chain out the old datum.*/
        if( scan->point == datpoint ) scan->point = successor;
      /* If datum is larger than scanned value,*/
        if( (scan->value < datum) )
        {
          /* chain it in here.  */
            datpoint->point = scanold->point;
          /* mark it chained in. */
            scanold->point = datpoint;
            datum = STOPPER;
        };
  /* Step median pointer down chain after doing odd-numbered element */
      /* Step median pointer.  */
        median = median->point       ;
      /* Break at end of chain  */
        if ( scan == &small ) break ;
      /* Save this pointer and   */
        scanold = scan ;
      /* step down chain */
        scan = scan->point ;

  /* Handle even-numbered item in chain.  */
        if( scan->point == datpoint ) scan->point = successor; 
        if( (scan->value < datum) )         
        {
            datpoint->point = scanold->point;       
            scanold->point = datpoint;
            datum = STOPPER;
        };
        if ( scan == &small ) break;
        scanold = scan ;                            
        scan = scan->point;
    };
    return( median->value );
}
