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

#include "ink.h"
#include <stdbool.h>

//cuckoo specific definitions 
#define NUM_BUCKETS 128 // must be a power of 2
#define NUM_INSERTS (NUM_BUCKETS / 4) // shoot for 25% occupancy
#define NUM_LOOKUPS NUM_INSERTS
#define MAX_RELOCATIONS 8
#define BUFFER_SIZE 32
#define TASK_NUM 15

#define RAISE_PIN

//#define TIME_ME
#ifdef TIME_ME
#include <stdio.h>
uint32_t tmp[10];
#endif

#ifdef RAISE_PIN
__nv uint8_t full_run_started = 0;
#endif

// Debug defines and flags.
#define DEBUG_PORT 3
#define DEBUG_PIN  4

typedef uint16_t value_t;
typedef uint16_t hash_t;
typedef uint16_t fingerprint_t;
typedef uint16_t index_t; // bucket index

typedef struct _insert_count {
    unsigned insert_count;
    unsigned inserted_count;
} insert_count_t;

typedef struct _lookup_count {
    unsigned lookup_count;
    unsigned member_count;
} lookup_count_t;

//enumeration of tasks for switching to relative execution path
enum task_index {
    t_init,
    t_init_array,
    t_generate_key,
    t_calc_indexes,
    t_calc_indexes_index_1,

    t_calc_indexes_index_2,
    t_insert,
    t_add,
    t_relocate,
    t_insert_done,

    t_lookup,
    t_lookup_search,
    t_lookup_done,
    t_print_stats,
    t_done
};

// define task-shared persistent variables.
__shared(

    uint8_t pinRaised;
    fingerprint_t _v_filter[NUM_BUCKETS];
    index_t _v_index;
    value_t _v_key;
    enum task_index _v_next_task;
    fingerprint_t _v_fingerprint;
    value_t _v_index1;
    value_t _v_index2;
    value_t _v_relocation_count;
    value_t _v_insert_count;
    value_t _v_inserted_count;
    value_t _v_lookup_count;
    value_t _v_member_count;
    bool _v_success;
    bool _v_member;    
)

ENTRY_TASK(task_init);
TASK(task_init_array);
TASK(task_generate_key);
TASK(task_calc_indexes);
TASK(task_calc_indexes_index_1);

TASK(task_calc_indexes_index_2);
TASK(task_insert);
TASK(task_add);
TASK(task_relocate);
TASK(task_insert_done);

TASK(task_lookup);
TASK(task_lookup_search);
TASK(task_lookup_done);
TASK(task_print_stats);
TASK(task_done);

// called at the very first boot
void thread1_init(){
    // create a thread with priority 15 and entry task t_init
    __CREATE(15,task_init);
    __SIGNAL(15);
}

__app_reboot(){

    __no_operation();

}

static value_t init_key = 0x0001; // seeds the pseudo-random sequence of keys

static hash_t djb_hash(uint8_t* data, unsigned len)
{
    uint16_t hash = 5381;
    unsigned int i;

    for(i = 0; i < len; data++, i++)
        hash = ((hash << 5) + hash) + (*data);


    return hash & 0xFFFF;
}

static index_t hash_to_index(fingerprint_t fp)
{
    hash_t hash = djb_hash((uint8_t *)&fp, sizeof(fingerprint_t));
    return hash & (NUM_BUCKETS - 1); // NUM_BUCKETS must be power of 2
}

static fingerprint_t hash_to_fingerprint(value_t key)
{
    return djb_hash((uint8_t *)&key, sizeof(value_t));
}

ENTRY_TASK(task_init){

#ifdef RAISE_PIN
    full_run_started = 1;
#endif

    unsigned i;
    for (i = 0; i < NUM_BUCKETS ; ++i) {
        __SET( _v_filter[i],0);
    }
    __SET( _v_insert_count , 0);
    __SET( _v_lookup_count , 0);
    __SET( _v_inserted_count , 0);
    __SET( _v_member_count , 0);
    __SET( _v_key , init_key);
    __SET( _v_next_task , t_insert);


    
    return task_generate_key;
}

TASK(task_init_array){

        unsigned i;
        
        for (i = 0; i < BUFFER_SIZE - 1; ++i) {
            __SET(_v_filter[i + __GET(_v_index)*(BUFFER_SIZE-1)] , 0);
        }
        
        __SET(_v_index, ++__GET(_v_index));

        if ( __GET(_v_index) == NUM_BUCKETS/(BUFFER_SIZE-1)) {
            return task_generate_key;
        }
        else {
            return task_init_array;
        }
}

TASK(task_generate_key){

    // insert pseufo-random integers, for testing
    // If we use consecutive ints, they hash to consecutive DJB hashes...
    // NOTE: we are not using rand(), to have the sequence available to verify
    // that that are no false negatives (and avoid having to save the values).

    uint16_t __cry;
    uint16_t next;

    __cry = (__GET(_v_key) + 1) * 17; 
    __SET(_v_key, __cry);
   
    enum task_index next_task = __GET(_v_next_task);
    
    if (next_task == t_insert) {
        return task_insert;
    } else if (next_task == t_lookup) {
        return task_lookup;
    } else {
        while(1); // Debugging purpose
    }
}

TASK(task_calc_indexes){

    uint16_t __cry;
    __cry = hash_to_fingerprint(__GET(_v_key));
    __SET(_v_fingerprint , __cry);

    return task_calc_indexes_index_1;        
}

TASK(task_calc_indexes_index_1){

	uint16_t __cry;
	__cry = hash_to_index(__GET(_v_key));
	__SET(_v_index1 , __cry);

	return task_calc_indexes_index_2;
}

TASK(task_calc_indexes_index_2){

    index_t fp_hash = hash_to_index(__GET(_v_fingerprint));
    uint16_t __cry;
    uint16_t next;

    __cry = __GET(_v_index1) ^ fp_hash;

    __SET(_v_index2 , __cry);

    enum task_index next_task = __GET(_v_next_task);


    if (next_task == t_add) {
        return task_add;
    } else if (next_task == t_lookup_search) {
        return task_lookup_search;
    } else {
        while(1); // Debugging purpose
    }

}

TASK(task_insert){

        __SET(_v_next_task , t_add);
        return task_calc_indexes;
}

TASK(task_add){

    uint16_t __cry;
    uint16_t __cry_idx = __GET(_v_index1);
    uint16_t __cry_idx2 = __GET(_v_index2);
	fingerprint_t fingerprint = __GET(_v_fingerprint);

    if (!__GET(_v_filter[__cry_idx])) {

        __SET(_v_success , true);
        __cry = fingerprint;
        __SET(_v_filter[__cry_idx], __cry);

        return task_insert_done;

    } else {

        if (!__GET(_v_filter[__cry_idx2])) {

	        __SET(_v_success , true);
            __cry = fingerprint;
            __SET(_v_filter[__cry_idx2], __cry);
        
            return task_insert_done;

        } else { 
        	// evict one of the two entries
            fingerprint_t fp_victim;
            index_t index_victim;

            if (rand() % 2) {
                index_victim = __cry_idx;
                fp_victim = __GET(_v_filter[__cry_idx]);
            } else {
                index_victim = __cry_idx2;
                fp_victim = __GET(_v_filter[__cry_idx2]);
            }

            // Evict the victim
            __cry = fingerprint;
            __SET(_v_filter[index_victim], __cry);
            __SET(_v_index1 , index_victim);
            __SET(_v_fingerprint , fp_victim);
            __SET(_v_relocation_count ,0);

            return task_relocate;
        }
    }
}

TASK(task_relocate){

    uint16_t __cry;
    fingerprint_t fp_victim = __GET(_v_fingerprint);
    index_t fp_hash_victim = hash_to_index(fp_victim);
    index_t index2_victim = __GET(_v_index1) ^ fp_hash_victim;

    if (!__GET(_v_filter[index2_victim])) { // slot was free
    
        __SET(_v_success, true);
        __SET(_v_filter[index2_victim], fp_victim);
    
        return task_insert_done;
    
    } else {

    	 // slot was occupied, rellocate the next victim
        if (__GET(_v_relocation_count) >= MAX_RELOCATIONS) { // insert failed
            __SET(_v_success, false);
        
            return task_insert_done;
        
        }

        __SET(_v_relocation_count, ++__GET(_v_relocation_count));
        __SET(_v_index1, index2_victim);
        __cry = __GET(_v_filter[index2_victim]);
        __SET(_v_fingerprint, __cry);
        __SET(_v_filter[index2_victim], fp_victim);
    
        return task_relocate;
    
    }
}

TASK(task_insert_done){

    uint16_t __cry;
    __SET(_v_insert_count, ++__GET(_v_insert_count));
    __cry = __GET(_v_inserted_count);
    __cry+= __GET(_v_success);

    __SET(_v_inserted_count, __cry);


    if (__GET(_v_insert_count) < NUM_INSERTS) {
        
        __SET(_v_next_task, t_insert);
        
        return task_generate_key;
    
    } else {

        __SET(_v_next_task, t_lookup);
        __SET(_v_key, init_key);
        
        return task_generate_key;
    
    }

}

TASK(task_lookup){

        __SET(_v_next_task, t_lookup_search);
        return task_calc_indexes;
}

TASK(task_lookup_search){

    if (__GET(_v_filter[__GET(_v_index1)]) == __GET(_v_fingerprint)) {
        __SET(_v_member, true);

    } else {

        if (__GET(_v_filter[__GET(_v_index2)]) == __GET(_v_fingerprint)) {
            __SET(_v_member, true);
        }
        else {
            __SET(_v_member, false);
        }
    }

    return task_lookup_done;

}

TASK(task_lookup_done){

    uint16_t __cry;
    __SET(_v_lookup_count, ++__GET(_v_lookup_count));
    __cry = __GET(_v_member_count);
    __cry+= __GET(_v_member);
    __SET(_v_member_count, __cry);

    if (__GET(_v_lookup_count) < NUM_LOOKUPS) {
        __SET(_v_next_task, t_lookup);

        return task_generate_key;

    }
    else {


        return task_print_stats;

    }

}

TASK(task_print_stats){
    
    __no_operation();

    return task_done;

}

TASK(task_done)
{

#ifdef RAISE_PIN
    if (full_run_started) {
        __port_on(3, 4);
        __port_off(3, 4);
        full_run_started = 0;
    }
#endif


// #ifdef TIME_ME
//     //measure some time and stop
//     tmp[0] = __get_time();
//     __delay_cycles(200);
//     tmp[1] = __get_time();
//     __get_time_stop();
// #endif
    
    return task_init;
}
