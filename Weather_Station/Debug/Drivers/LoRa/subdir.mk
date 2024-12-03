################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/LoRa/SX1278.c \
../Drivers/LoRa/SX1278_hw.c 

OBJS += \
./Drivers/LoRa/SX1278.o \
./Drivers/LoRa/SX1278_hw.o 

C_DEPS += \
./Drivers/LoRa/SX1278.d \
./Drivers/LoRa/SX1278_hw.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/LoRa/%.o Drivers/LoRa/%.su Drivers/LoRa/%.cyclo: ../Drivers/LoRa/%.c Drivers/LoRa/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-LoRa

clean-Drivers-2f-LoRa:
	-$(RM) ./Drivers/LoRa/SX1278.cyclo ./Drivers/LoRa/SX1278.d ./Drivers/LoRa/SX1278.o ./Drivers/LoRa/SX1278.su ./Drivers/LoRa/SX1278_hw.cyclo ./Drivers/LoRa/SX1278_hw.d ./Drivers/LoRa/SX1278_hw.o ./Drivers/LoRa/SX1278_hw.su

.PHONY: clean-Drivers-2f-LoRa

