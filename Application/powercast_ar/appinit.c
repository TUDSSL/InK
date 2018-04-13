#include "app.h"

extern void thread1_init();
extern void thread2_init();
extern void thread3_init();
//extern void thread4_init();
// this is the function that will be called only at initial boot by the runtime.
void __app_init(){
	
    thread1_init();
    thread2_init();
    thread3_init();
    thread4_init();
    //thread4_init();
}

