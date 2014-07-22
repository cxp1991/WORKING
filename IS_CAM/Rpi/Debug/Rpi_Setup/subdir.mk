################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Rpi_Setup/Check_Hardware.c \
../Rpi_Setup/utils.c 

OBJS += \
./Rpi_Setup/Check_Hardware.o \
./Rpi_Setup/utils.o 

C_DEPS += \
./Rpi_Setup/Check_Hardware.d \
./Rpi_Setup/utils.d 


# Each subdirectory must supply rules for building sources it contributes
Rpi_Setup/%.o: ../Rpi_Setup/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


