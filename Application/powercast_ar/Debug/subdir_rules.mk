################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
adxl345_i2c_lib.obj: ../adxl345_i2c_lib.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu/msp430" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar/dsplib" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/channel" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/isr" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/scheduler" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/timer" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="adxl345_i2c_lib.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

app_intrinsics.obj: ../app_intrinsics.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu/msp430" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar/dsplib" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/channel" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/isr" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/scheduler" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/timer" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="app_intrinsics.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

appinit.obj: ../appinit.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu/msp430" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar/dsplib" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/channel" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/isr" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/scheduler" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/timer" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="appinit.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

avg_filter.obj: ../avg_filter.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu/msp430" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar/dsplib" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/channel" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/isr" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/scheduler" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/timer" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="avg_filter.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

interrupts.obj: ../interrupts.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu/msp430" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar/dsplib" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/channel" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/isr" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/scheduler" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/timer" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="interrupts.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

thread1.obj: ../thread1.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu/msp430" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar/dsplib" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/channel" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/isr" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/scheduler" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/timer" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="thread1.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

thread2.obj: ../thread2.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu/msp430" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar/dsplib" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/channel" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/isr" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/scheduler" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/timer" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="thread2.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

thread3.obj: ../thread3.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu/msp430" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar/dsplib" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/channel" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/isr" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/scheduler" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/timer" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="thread3.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

thread4.obj: ../thread4.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu/msp430" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar/dsplib" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/channel" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/isr" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/scheduler" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/timer" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="thread4.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


