################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include drivers/src/subdir.mk
-include bsp/subdir.mk
-include autogen/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := MCU1_GPIO_Driver_Baremetal
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
MCU1_GPIO_Driver_Baremetal.elf \

MAP_FILES += \
MCU1_GPIO_Driver_Baremetal.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
MCU1_GPIO_Driver_Baremetal.list \


# All Target
all: main-build

# Main-build Target
main-build: MCU1_GPIO_Driver_Baremetal.elf secondary-outputs

# Tool invocations
MCU1_GPIO_Driver_Baremetal.elf MCU1_GPIO_Driver_Baremetal.map: $(OBJS) $(USER_OBJS) C:\Users\Sheen\Desktop\Embedded_System\PIT_Embedded_Systems\MCU1_GPIO_Driver_Baremetal\STM32F407VGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "MCU1_GPIO_Driver_Baremetal.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Sheen\Desktop\Embedded_System\PIT_Embedded_Systems\MCU1_GPIO_Driver_Baremetal\STM32F407VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="MCU1_GPIO_Driver_Baremetal.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

MCU1_GPIO_Driver_Baremetal.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "MCU1_GPIO_Driver_Baremetal.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) MCU1_GPIO_Driver_Baremetal.elf MCU1_GPIO_Driver_Baremetal.list MCU1_GPIO_Driver_Baremetal.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
