// This file is part of InK.
// 
// author = "dpatoukas" 
// maintainer = "dpatoukas"
// email = "dpatoukas@gmail.com" 
//  
// copyright = "Copyright 2018 Delft University of Technology" 
// license = "LGPL" 
// version = "3.0" 
// status = "Production"
//
// 
// InK is free software: you can redistribute it and/or modify
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
#include "persistent_timer.h"


// 0 persistent - 1 dirty persistent 
// double buffer 
__nv pers_timer_t pers_timer_vars[2];

void _pers_timer_init(){
	uint8_t i;
	for (i = 0; i < MAX_TIMED_THREADS; i++)
	{
			pers_timer_vars[0].wkup_timing[i].status = NOT_USED; 
			pers_timer_vars[0].xpr_timing[i].status = NOT_USED; 
			pers_timer_vars[0].pdc_timing[i].status = NOT_USED; 
	}
}

void _pers_timer_update_data(uint8_t idx,ink_time_interface_t interface , uint32_t time_data){
	//update the persistent timer dirty buffer
	switch(interface)
	{
		case WKUP:
			pers_timer_vars[1].wkup_timing[idx].data = time_data;
			pers_timer_vars[1].wkup_timing[idx].__dirty = DIRTY;
			break;
		case XPR:
			pers_timer_vars[1].xpr_timing[idx].data = time_data;
			pers_timer_vars[1].xpr_timing[idx].__dirty = DIRTY;
			return;
		case PDC:
			pers_timer_vars[1].pdc_timing[idx].data = time_data;
			pers_timer_vars[1].pdc_timing[idx].__dirty = DIRTY;
			break;
		default:
			break;
	}

		
}

void _pers_timer_update_thread_id(uint8_t idx,ink_time_interface_t interface , uint8_t thread_id){
	//update the persistent timer dirty buffer
	switch (interface)
	{
		case WKUP:
			pers_timer_vars[1].wkup_timing[idx].thread_id = thread_id;
			pers_timer_vars[1].wkup_timing[idx].__dirty = DIRTY;
			break;
		case XPR:
			pers_timer_vars[1].xpr_timing[idx].thread_id = thread_id;
			pers_timer_vars[1].xpr_timing[idx].__dirty = DIRTY;
			break;
		case PDC:
			pers_timer_vars[1].pdc_timing[idx].thread_id = thread_id;
			pers_timer_vars[1].pdc_timing[idx].__dirty = DIRTY;
			break;
		default:
			break;
	}

}

void _pers_timer_update_status(uint8_t idx,ink_time_interface_t interface , used_st status){
	//update the persistent timer dirty buffer
	switch (interface)
	{
		case WKUP:
			pers_timer_vars[1].wkup_timing[idx].status = status;
			pers_timer_vars[1].wkup_timing[idx].__dirty = DIRTY;
			break;
		case XPR:
			pers_timer_vars[1].xpr_timing[idx].status = status;
			pers_timer_vars[1].xpr_timing[idx].__dirty = DIRTY;
			break;
		case PDC:
			pers_timer_vars[1].pdc_timing[idx].status = status;
			pers_timer_vars[1].pdc_timing[idx].__dirty = DIRTY;
			break;
		default:
			break;

	}
}

void _pers_timer_update_nxt_thread(ink_time_interface_t ink_time_interface ,uint8_t next_thread){
	//update the persistent timer dirty buffer
	pers_timer_vars[1].next_info[ink_time_interface].next_thread = next_thread;
	pers_timer_vars[1].next_info[ink_time_interface].__dirty = DIRTY;
}

void _pers_timer_update_nxt_time(ink_time_interface_t ink_time_interface, uint16_t next_time){
	//update the persistent timer dirty buffer
	pers_timer_vars[1].next_info[ink_time_interface].next_time = next_time;
	pers_timer_vars[1].next_info[ink_time_interface].__dirty = DIRTY;
}

//timer buffer is ready to commit
void _pers_timer_update_lock(ink_time_interface_t interface){
switch (interface)
	{
		case WKUP:
			wkup_tstatus = TIMER_COMMIT;
		break;
		case XPR:
			xpr_tstatus = TIMER_COMMIT;
		break;
		case PDC:
			pdc_tstatus = TIMER_COMMIT;
			break;
		default:
			break;

	}

}

//commit into the persistent buffer

void _commit_timer_buffers(ink_time_interface_t interface){
	
	uint8_t i;
	
	switch (interface){
		case WKUP:
		for (i = 0; i < MAX_WKUP_THREADS; i++)
		{
			if (pers_timer_vars[1].wkup_timing[i].__dirty == DIRTY)
			{
				
				pers_timer_vars[0].wkup_timing[i] = pers_timer_vars[1].wkup_timing[i];
				pers_timer_vars[0].wkup_timing[i].__dirty = NOT_DIRTY;
			}
		}
		break;
	case XPR:
		for (i = 0; i < MAX_XPR_THREADS; i++)
		{
			if (pers_timer_vars[1].xpr_timing[i].__dirty == DIRTY)
			{
				
				pers_timer_vars[0].xpr_timing[i] = pers_timer_vars[1].xpr_timing[i];
				pers_timer_vars[0].xpr_timing[i].__dirty = NOT_DIRTY;
			}
		}
		break;
	case PDC:
		for (i = 0; i < MAX_PDC_THREADS; i++)
		{
			if (pers_timer_vars[1].pdc_timing[i].__dirty == DIRTY)
			{
				
				pers_timer_vars[0].pdc_timing[i] = pers_timer_vars[1].pdc_timing[i];
				pers_timer_vars[0].pdc_timing[i].__dirty = NOT_DIRTY;
			}
		}
		break;
	default:
		break;
	}	

	for (i = 0; i < TIMER_TOOLS; i++)
	{
		if (	pers_timer_vars[1].next_info[interface].__dirty == DIRTY)
		{
			pers_timer_vars[0].next_info[interface] = 
				pers_timer_vars[1].next_info[interface];
			pers_timer_vars[0].next_info[interface].__dirty = 
				NOT_DIRTY;
		}
	}
}

void _pers_timer_commit(ink_time_interface_t interface){
switch (interface)
	{
		case WKUP:
		    if (wkup_tstatus == TIMER_COMMIT)
		    {
		    	_commit_timer_buffers(interface);
		        wkup_tstatus = TIMER_DONE;
		    }
			break;
		case XPR:
			if (xpr_tstatus == TIMER_COMMIT)
		    {
		    	_commit_timer_buffers(interface);
		        xpr_tstatus = TIMER_DONE;
		    }	break;
		case PDC:
			if (pdc_tstatus == TIMER_COMMIT)
		    {
		    	_commit_timer_buffers(interface);
		        pdc_tstatus = TIMER_DONE;
		    }
			break;
		default:
			break;

	}
}

timing_d _pers_timer_get(uint8_t idx,ink_time_interface_t interface ){
	switch (interface)
	{
		case WKUP:
			return pers_timer_vars[0].wkup_timing[idx];
		case XPR:
			return pers_timer_vars[0].xpr_timing[idx];
		case PDC:
			return pers_timer_vars[0].pdc_timing[idx];
		default:
		    return pers_timer_vars[0].wkup_timing[idx];
	}
		
}

uint16_t _pers_timer_get_data(uint8_t idx,ink_time_interface_t interface ){
	//get the persistent timer from persistent buffer
	switch (interface)
	{
		case WKUP:
			return pers_timer_vars[0].wkup_timing[idx].data ;		
		case XPR:
			return pers_timer_vars[0].xpr_timing[idx].data ;			
		case PDC:
			return pers_timer_vars[0].pdc_timing[idx].data ;			
		default:
			return 0;

	}

}

uint8_t _pers_timer_get_thread_id(uint8_t idx,ink_time_interface_t interface ){
	//get the persistent timer from persistent buffer
	switch (interface)
	{
		case WKUP:
			return pers_timer_vars[0].wkup_timing[idx].thread_id ;
		case XPR:
			return pers_timer_vars[0].xpr_timing[idx].thread_id ;
		case PDC:
			return pers_timer_vars[0].pdc_timing[idx].thread_id ;
		default:
		    return	0;
	}


}

used_st _pers_timer_get_status(uint8_t idx,ink_time_interface_t interface ){
	//get the persistent timer from persistent buffer
switch (interface)
	{
		case WKUP:
			return pers_timer_vars[0].wkup_timing[idx].status ;
		case XPR:
			return pers_timer_vars[0].xpr_timing[idx].status ;
		case PDC:
			return pers_timer_vars[0].pdc_timing[idx].status ;
		default:
		    return USED;
	}
}
//TODO: change types
uint8_t _pers_timer_get_nxt_thread(ink_time_interface_t ink_time_interface){
	//get the persistent timer from persistent buffer
	return pers_timer_vars[0].next_info[ink_time_interface].next_thread;
}

uint16_t _pers_timer_get_nxt_time(ink_time_interface_t ink_time_interface){
	//get the persistent timer from persistent buffer
	return pers_timer_vars[0].next_info[ink_time_interface].next_time;
}
