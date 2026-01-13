# Default target
all: build/nucleo_toggle.bin

# Create build directory
build:
	mkdir build

# Compile main.c
build/main.o: Src/main.c | build
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -g -Wall -c Src/main.c -o build/main.o

# Compile syscalls.c
build/syscalls.o: Src/syscalls.c | build
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -g -Wall -c Src/syscalls.c -o build/syscalls.o

# Compile sysmem.c
build/sysmem.o: Src/sysmem.c | build
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -g -Wall -c Src/sysmem.c -o build/sysmem.o

# Assemble startup file
build/startup_stm32f401retx.o: Startup/startup_stm32f401retx.s | build
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -g -Wall -c Startup/startup_stm32f401retx.s -o build/startup_stm32f401retx.o

# Link all object files to ELF
build/nucleo_toggle.elf: build/main.o build/syscalls.o build/sysmem.o build/startup_stm32f401retx.o
	arm-none-eabi-gcc build/main.o build/syscalls.o build/sysmem.o build/startup_stm32f401retx.o -mcpu=cortex-m4 -mthumb -T STM32F401RETX_FLASH.ld -Wl,-Map=build/nucleo_toggle.map --specs=nosys.specs -o build/nucleo_toggle.elf

# Convert ELF to BIN
build/nucleo_toggle.bin: build/nucleo_toggle.elf
	arm-none-eabi-objcopy -O binary build/nucleo_toggle.elf build/nucleo_toggle.bin

# Flash to board
flash: build/nucleo_toggle.bin
	"C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe" -c port=SWD -w build/nucleo_toggle.bin 0x08000000 -v -rst

# Clean
clean:
	rm -rf build

.PHONY: all clean flash

