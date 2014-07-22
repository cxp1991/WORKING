################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utils/base64.c \
../Utils/debug.c 

OBJS += \
./Utils/base64.o \
./Utils/debug.o 

C_DEPS += \
./Utils/base64.d \
./Utils/debug.d 


# Each subdirectory must supply rules for building sources it contributes
Utils/%.o: ../Utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


