
#include "ink.h"

#define RAISE_PIN

//cem specific defines 
#define NIL 0 // like NULL, but for indexes, not real pointers

#define DICT_SIZE         512
#define BLOCK_SIZE         64

#define NUM_LETTERS_IN_SAMPLE        2
#define LETTER_MASK             0x00FF
#define LETTER_SIZE_BITS             8
#define NUM_LETTERS (LETTER_MASK + 1)
#ifdef RAISE_PIN
__nv uint8_t full_run_started = 0;
#endif

typedef uint16_t index_t;
typedef uint16_t letter_t;
typedef uint16_t sample_t;
typedef struct _node_t {
    letter_t letter; // 'letter' of the alphabet
    index_t sibling; // this node is a member of the parent's children list
    index_t child;   // link-list of children
} node_t;

// Tasks.
ENTRY_TASK(task_init);
TASK(task_init_dict);
TASK(task_sample);
TASK(task_measure_temp);
TASK(task_letterize);
TASK(task_compress);
TASK(task_find_sibling);
TASK(task_add_node);
TASK(task_add_insert);
TASK(task_append_compressed);
TASK(task_print);
TASK(task_done);

__shared(

    uint8_t pinCont;
    unsigned _v_letter_idx;
    letter_t _v_letter;
    sample_t _v_prev_sample;
    index_t _v_out_len;
    index_t _v_node_count;
    sample_t _v_sample;
    index_t _v_sample_count;
    index_t _v_sibling;
    index_t _v_child;
    index_t _v_parent;
    index_t _v_parent_next;
    node_t _v_parent_node;
    node_t _v_sibling_node;
    index_t _v_symbol;
    node_t _v_compressed_data[BLOCK_SIZE];
    node_t _v_dict[DICT_SIZE];

)

// called at the very first boot
void thread1_init(){
    // create a thread with priority 15 and entry task t_init
    __CREATE(15,task_init);
    __SIGNAL(15);
}

__app_reboot(){
    __no_operation();
}
static sample_t acquire_sample(letter_t prev_sample)
{
    letter_t sample = (prev_sample + 1) & 0x03;
    return sample;
}

ENTRY_TASK(task_init){
#ifdef RAISE_PIN
    full_run_started = 1;
	 // __port_on(3,4);
	// __port_off(3,4);
    __SET( pinCont,1);
#endif
    __SET(_v_parent_next,0);
    __SET(_v_out_len,0);
    __SET(_v_letter,0);
    __SET(_v_prev_sample,0);
    __SET(_v_letter_idx,0);
    __SET(_v_sample_count,1);
    return task_init_dict;
}

TASK(task_init_dict){

    uint16_t i = __GET(_v_letter);

    __SET(_v_dict[i].letter, i) ;
    __SET(_v_dict[i].sibling, NIL);
    __SET(_v_dict[i].child, NIL);
    __SET(_v_letter,++__GET(_v_letter));

    if (i < NUM_LETTERS) {
        return task_init_dict;
        //printf("task_init_dict->task_init_dict\n");

    } else {
        __SET(_v_node_count,NUM_LETTERS);
        return task_sample;
        //printf("task_init_dict->task_sample\n");    
    }
}

TASK(task_sample){

	unsigned letter_idx = __GET(_v_letter_idx);
    unsigned next_letter_idx = letter_idx + 1;
    
    if (next_letter_idx == NUM_LETTERS_IN_SAMPLE)
        next_letter_idx = 0;

    if (letter_idx == 0) {
        __SET(_v_letter_idx, next_letter_idx);
        return task_measure_temp;
        //printf("task_sample->task_measure_temp\n");
    } else {
        __SET(_v_letter_idx, next_letter_idx);
        return task_letterize;
        //printf("task_sample->task_letterize\n");
    }

}

TASK(task_measure_temp){

	sample_t prev_sample;
	prev_sample = __GET(_v_prev_sample);

	sample_t sample = acquire_sample(prev_sample);
	prev_sample = sample;
	__SET(_v_prev_sample, prev_sample);
	__SET(_v_sample, sample);
	return task_letterize;
	//printf("task_measure_temp->task_letterize\n");

}

TASK(task_letterize){

        unsigned letter_idx = __GET(_v_letter_idx);

        if (letter_idx == 0)
            letter_idx = NUM_LETTERS_IN_SAMPLE;
        else
            letter_idx--;

        unsigned letter_shift = LETTER_SIZE_BITS * letter_idx;
        letter_t letter = (__GET(_v_sample) & (LETTER_MASK << letter_shift)) >> letter_shift;

        __SET(_v_letter, letter);
        return task_compress;
        //printf("task_letterize->task_compress\n");
}

TASK(task_compress){

	        // pointer into the dictionary tree; starts at a root's child
        index_t parent = __GET(_v_parent_next);

        uint16_t __cry;

        __cry = __GET(_v_dict[parent].child);
        __SET(_v_sibling, __cry);

        __cry = __GET(_v_dict[parent].letter);
        __SET(_v_parent_node.letter,  __cry);

        __cry = __GET(_v_dict[parent].sibling);
        __SET(_v_parent_node.sibling, __cry);

        __cry = __GET(_v_dict[parent].child);
        __SET(_v_parent_node.child, __cry);

        __SET(_v_parent, parent);
        __cry = __GET(_v_dict[parent].child);
        __SET(_v_child, __cry);

        __SET(_v_sample_count, ++__GET(_v_sample_count));

        return task_find_sibling;
        //printf("task_compress->task_find_sibling\n");
}

TASK(task_find_sibling){
        
        int i = __GET(_v_sibling);

        if (i != NIL)
        {

            uint16_t __cry = __GET(_v_letter);
            if (__GET(_v_dict[i].letter) == __cry )
            {   // found
                __cry = __GET(_v_sibling);
                __SET(_v_parent_next, __cry);

                return task_letterize;
                //printf("task_find_sibling->task_letterize\n");
            }
            else if(__GET(_v_dict[i].sibling) != 0)
            {
                // continue traversing the siblings
                __cry = __GET(_v_dict[i].sibling);
                __SET(_v_sibling, __cry);
                return task_find_sibling;
                //printf("task_find_sibling->task_letterize\n");
        	}
        	else
        	{
	            index_t starting_node_idx = (index_t) __GET(_v_letter);

	            __SET(_v_parent_next, starting_node_idx);

	            if (__GET(_v_child) == NIL)
	            {
	                return task_add_insert;
	                //printf("task_find_sibling->task_add_insert\n");
	            }
	            else 
	            {
	                return task_add_node;
	                //printf("task_find_sibling->task_add_node\n");
	            }                
            }            
        }
        else
        {    
            index_t starting_node_idx = (index_t) __GET(_v_letter);

            __SET(_v_parent_next, starting_node_idx);

            if ( __GET(_v_child) == NIL )
            {
                return task_add_insert;
                //printf("task_find_sibling->task_add_insert\n");
            }
            else 
            {
                return task_add_node;
                //printf("task_find_sibling->task_add_node\n");
            }
        }

}

TASK(task_add_node){

    int i = __GET(_v_sibling);


    if ( __GET(_v_dict[i].sibling) != NIL ) {
        index_t next_sibling = __GET(_v_dict[i].sibling);
        __SET(_v_sibling, next_sibling);
        return task_add_node;
        //printf("task_add_node->task_add_node\n");
    } else { // found last sibling in the list

        uint16_t __cry;

         __cry = __GET(_v_dict[i].letter);
         __SET(_v_sibling_node.letter, __cry);
        __cry = __GET(_v_dict[i].sibling);
        __SET(_v_sibling_node.sibling, __cry);
        __cry = __GET(_v_dict[i].child);
        __SET(_v_sibling_node.child, __cry);

        return task_add_insert;
        //printf("task_add_node->task_add_insert\n");
    }


}

TASK(task_add_insert){

    if (__GET(_v_node_count) == DICT_SIZE) { // wipe the table if full
        while (1);
    }

    index_t child = __GET(_v_node_count);
    uint16_t __cry;
    if (__GET(_v_parent_node.child) == NIL) { // the only child

        __SET(_v_parent_node.child, child);
        int i = __GET(_v_parent);


        __cry = __GET(_v_parent_node.letter);
        __SET(_v_dict[i].letter, __cry);
        __cry  = __GET(_v_parent_node.sibling);
        __SET(_v_dict[i].sibling, __cry);
        __cry = __GET(_v_parent_node.child);
        __SET(_v_dict[i].child, __cry);

    } else { // a sibling

        index_t last_sibling = __GET(_v_sibling);

        __SET(_v_sibling_node.sibling, child);
        __cry = __GET(_v_sibling_node.letter);
        __SET(_v_dict[last_sibling].letter,__cry);
        __cry = __GET(_v_sibling_node.sibling);
        __SET(_v_dict[last_sibling].sibling, __cry);
        __cry  = __GET(_v_sibling_node.child);
        __SET(_v_dict[last_sibling].child, __cry);
    }
    __cry = __GET(_v_letter);
    __SET(_v_dict[child].letter, __cry);
    __SET(_v_dict[child].sibling, NIL);
    __SET(_v_dict[child].child, NIL);
    __cry = __GET(_v_parent);
    __SET(_v_symbol, __cry);
    //printf("Add_insert:%d\n",__cry);
    __SET(_v_node_count, ++__GET(_v_node_count));

    return task_append_compressed;
    //printf("task_add_insert->task_append_compressed\n");

}

TASK(task_append_compressed){

        uint16_t __cry;
        int i = __GET(_v_out_len);
        __cry = __GET(_v_symbol);
        //printf("Append:%d",ios_persistent_vars[1]._v_symbol);
        __SET(_v_compressed_data[i].letter, __cry);

        __SET(_v_out_len,++__GET(_v_out_len));

        if (__GET(_v_out_len) == BLOCK_SIZE) {
            return task_print;
            //printf("task_append_compressed->task_print\n");

        } else {
            return task_sample;
            //printf("task_append_compressed->task_sample\n");

        }

}

TASK(task_print){

    unsigned i;

    for (i = 0; i < BLOCK_SIZE; ++i) {
        index_t index = __GET(_v_compressed_data[i].letter);
    }
    return task_done;

}

TASK(task_done){

#ifdef RAISE_PIN
    if (full_run_started) {
        full_run_started = 0;
    	__port_on(3, 4);
        __port_off(3, 4);
    	__SET(pinCont, 0);
    }
#endif

    return task_init;
}
