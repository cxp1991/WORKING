################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Uart/command.c \
../Uart/receive.c \
../Uart/uart.c \
../Uart/uart_controller.c 

OBJS += \
./Uart/command.o \
./Uart/receive.o \
./Uart/uart.o \
./Uart/uart_controller.o 

C_DEPS += \
./Uart/command.d \
./Uart/receive.d \
./Uart/uart.d \
./Uart/uart_controller.d 


# Each subdirectory must supply rules for building sources it contributes
Uart/%.o: ../Uart/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


