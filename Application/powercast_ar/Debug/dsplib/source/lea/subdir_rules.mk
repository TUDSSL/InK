################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
dsplib/source/lea/msp_lea_flags.obj: ../dsplib/source/lea/msp_lea_flags.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu/msp430" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar/dsplib" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/channel" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/isr" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/scheduler" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/timer" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="dsplib/source/lea/msp_lea_flags.d_raw" --obj_directory="dsplib/source/lea" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

dsplib/source/lea/msp_lea_init.obj: ../dsplib/source/lea/msp_lea_init.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu/msp430" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar/dsplib" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/channel" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/isr" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/scheduler" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/timer" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="dsplib/source/lea/msp_lea_init.d_raw" --obj_directory="dsplib/source/lea" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

dsplib/source/lea/msp_lea_isr.obj: ../dsplib/source/lea/msp_lea_isr.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu/msp430" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar/dsplib" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/channel" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/isr" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/scheduler" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/timer" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="dsplib/source/lea/msp_lea_isr.d_raw" --obj_directory="dsplib/source/lea" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

dsplib/source/lea/msp_lea_patch.obj: ../dsplib/source/lea/msp_lea_patch.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu/msp430" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar/dsplib" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/channel" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/isr" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/mcu" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/scheduler" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK/timer" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/powercast_ar" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="dsplib/source/lea/msp_lea_patch.d_raw" --obj_directory="dsplib/source/lea" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


