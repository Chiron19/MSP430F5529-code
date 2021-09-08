################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
driverlib/MSP430F5xx_6xx/%.obj: ../driverlib/MSP430F5xx_6xx/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-msp430_20.2.5.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --include_path="C:/ti/ccs930/ccs/ccs_base/msp430/include" --include_path="C:/Users/dssy2/workspace_v9/adc10_a_ex2_intRef" --include_path="C:/Users/dssy2/workspace_v9/adc10_a_ex2_intRef/driverlib/MSP430F5xx_6xx" --include_path="C:/ti/ccs930/ccs/tools/compiler/ti-cgt-msp430_20.2.5.LTS/include" --advice:power="none" --define=__MSP430F5510__ --define=DEPRECATED -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="driverlib/MSP430F5xx_6xx/$(basename $(<F)).d_raw" --obj_directory="driverlib/MSP430F5xx_6xx" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


