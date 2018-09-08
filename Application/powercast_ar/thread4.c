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

//thread4
//
#include "app.h"
#include "ink.h"
#include "app_intrinsics.h"
#include "interrupts.h"
// define task-shared persistent variables.

 __shared(
     uint16_t timer_value;
     uint16_t energy_status;
 )


ENTRY_TASK(t_init);
TASK(t_energy_check);
TASK(t_transmit);

// called at the very first boot
void thread4_init(){
    // create a thread with priority 15 and entry task task1
    __CREATE(THREAD4,t_init);

}

__nv uint8_t init = 0;

ENTRY_TASK(t_init) 
{
	P3OUT |= BIT4;
	P3OUT &= ~BIT4;
	if (init){
	    __SET(timer_value,++__GET(timer_value));
	}
	else
	{
	    init = 1;
	    __SET(timer_value,0);
	}

	if (__GET(energy_status))
	{
		return t_energy_check;

	}
	else
	{
		return t_transmit;
	}


}

TASK(t_energy_check)
{
	__delay_cycles(1000);

	__SET(energy_status,1);
	__SIGNAL(THREAD1);
	return NULL;
}

TASK(t_transmit)
{
	__delay_cycles(1000);
	//find the library at:
	//https://github.com/dpatoukas/IRHermes
	__SIGNAL(THREAD1);
	__SET(energy_status,0);
	return NULL;

}

