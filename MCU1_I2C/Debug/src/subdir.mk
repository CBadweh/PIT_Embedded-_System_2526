################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/005button_interrupt.c 

OBJS += \
./src/005button_interrupt.o 

C_DEPS += \
./src/005button_interrupt.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o src/%.su src/%.cyclo: ../src/%.c src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I"C:/Users/Sheen/Desktop/Embedded_System/PIT_Embedded_Systems/MCU1_GPIO_Driver_Baremetal_Interrupt/bsp" -I"C:/Users/Sheen/Desktop/Embedded_System/PIT_Embedded_Systems/MCU1_GPIO_Driver_Baremetal_Interrupt/drivers/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src

clean-src:
	-$(RM) ./src/005button_interrupt.cyclo ./src/005button_interrupt.d ./src/005button_interrupt.o ./src/005button_interrupt.su

.PHONY: clean-src

