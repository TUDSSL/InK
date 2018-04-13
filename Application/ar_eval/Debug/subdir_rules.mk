################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
appinit.obj: ../appinit.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/ar_eval" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/ar_eval" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5969__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="appinit.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

divmodhi3.obj: ../divmodhi3.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/ar_eval" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/ar_eval" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5969__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="divmodhi3.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

mult.obj: ../mult.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/ar_eval" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/ar_eval" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5969__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="mult.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

sqrt.obj: ../sqrt.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/ar_eval" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/ar_eval" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5969__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="sqrt.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

thread1.obj: ../thread1.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/opt/ti/ccsv8/ccs_base/msp430/include" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/ar_eval" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/InkMerge/InK" --include_path="/home/dpatoukas/Documents/TUDelft/Thesis/Ink++/InKApplications/ar_eval" --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.1.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5969__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="thread1.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


