################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../P2P_Stream/controller.c \
../P2P_Stream/gstreamer_log.c \
../P2P_Stream/receive_audio.c \
../P2P_Stream/send_audio.c \
../P2P_Stream/send_video.c \
../P2P_Stream/stream.c \
../P2P_Stream/stun.c \
../P2P_Stream/text.c 

OBJS += \
./P2P_Stream/controller.o \
./P2P_Stream/gstreamer_log.o \
./P2P_Stream/receive_audio.o \
./P2P_Stream/send_audio.o \
./P2P_Stream/send_video.o \
./P2P_Stream/stream.o \
./P2P_Stream/stun.o \
./P2P_Stream/text.o 

C_DEPS += \
./P2P_Stream/controller.d \
./P2P_Stream/gstreamer_log.d \
./P2P_Stream/receive_audio.d \
./P2P_Stream/send_audio.d \
./P2P_Stream/send_video.d \
./P2P_Stream/stream.d \
./P2P_Stream/stun.d \
./P2P_Stream/text.d 


# Each subdirectory must supply rules for building sources it contributes
P2P_Stream/%.o: ../P2P_Stream/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


