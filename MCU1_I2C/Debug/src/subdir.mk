################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/012i2c_master_rx_testingIT_hardcoded.c 

OBJS += \
./src/012i2c_master_rx_testingIT_hardcoded.o 

C_DEPS += \
./src/012i2c_master_rx_testingIT_hardcoded.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o src/%.su src/%.cyclo: ../src/%.c src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I"C:/Users/Sheen/Desktop/Embedded_System/PIT_Embedded_Systems/MCU1_I2C/bsp" -I"C:/Users/Sheen/Desktop/Embedded_System/PIT_Embedded_Systems/MCU1_I2C/drivers/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src

clean-src:
	-$(RM) ./src/012i2c_master_rx_testingIT_hardcoded.cyclo ./src/012i2c_master_rx_testingIT_hardcoded.d ./src/012i2c_master_rx_testingIT_hardcoded.o ./src/012i2c_master_rx_testingIT_hardcoded.su

.PHONY: clean-src

