################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
dr_sdcard/%.obj: ../dr_sdcard/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs930/ccs/tools/compiler/msp430_4.1.9/bin/cl430" -vmsp --abi=eabi --code_model=large --data_model=large -g --include_path="C:/ti/ccs930/ccs/ccs_base/msp430/include" --include_path="C:/ti/ccs930/ccs/tools/compiler/msp430_4.1.9/include" --advice:power=all --define=__MSP430F5529__ --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="dr_sdcard/$(basename $(<F)).d_raw" --obj_directory="dr_sdcard" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


