################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/SX1278.c \
../Drivers/SX1278_hw.c 

OBJS += \
./Drivers/SX1278.o \
./Drivers/SX1278_hw.o 

C_DEPS += \
./Drivers/SX1278.d \
./Drivers/SX1278_hw.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/%.o Drivers/%.su Drivers/%.cyclo: ../Drivers/%.c Drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers

clean-Drivers:
	-$(RM) ./Drivers/SX1278.cyclo ./Drivers/SX1278.d ./Drivers/SX1278.o ./Drivers/SX1278.su ./Drivers/SX1278_hw.cyclo ./Drivers/SX1278_hw.d ./Drivers/SX1278_hw.o ./Drivers/SX1278_hw.su

.PHONY: clean-Drivers

