extern void thread1_init();

// this is the function that will be called only at initial boot by the runtime.
void __app_init(){
	
    thread1_init();
}

