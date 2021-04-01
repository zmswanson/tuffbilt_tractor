################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/AmbientTemperature.c \
../src/BMS.c \
../src/NEO6_GPS.c \
../src/RaspberryPi_Comms.c \
../src/main.c \
../src/periph_setup.c \
../src/syscalls.c \
../src/system_stm32f10x.c \
../src/usbSetup.c 

OBJS += \
./src/AmbientTemperature.o \
./src/BMS.o \
./src/NEO6_GPS.o \
./src/RaspberryPi_Comms.o \
./src/main.o \
./src/periph_setup.o \
./src/syscalls.o \
./src/system_stm32f10x.o \
./src/usbSetup.o 

C_DEPS += \
./src/AmbientTemperature.d \
./src/BMS.d \
./src/NEO6_GPS.d \
./src/RaspberryPi_Comms.d \
./src/main.d \
./src/periph_setup.d \
./src/syscalls.d \
./src/system_stm32f10x.d \
./src/usbSetup.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -I"C:/Users/Zach/Dropbox/TuffBilt Code/tractorDiagnosticsV2/StdPeriph_Driver/inc" -I"C:/Users/Zach/Dropbox/TuffBilt Code/tractorDiagnosticsV2/inc" -I"C:/Users/Zach/Dropbox/TuffBilt Code/tractorDiagnosticsV2/CMSIS/device" -I"C:/Users/Zach/Dropbox/TuffBilt Code/tractorDiagnosticsV2/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


