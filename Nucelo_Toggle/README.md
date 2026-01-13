# STM32F401RE Baremetal LED Toggle Project

A simple baremetal project that toggles the onboard LED (LD2) on the STM32F401RE Nucleo board using direct register manipulation.

## What This Project Does

- Toggles PA5 (onboard LED LD2) using GPIO registers
- No HAL or external libraries - pure register-level programming
- Demonstrates baremetal embedded development on ARM Cortex-M4

---

## Project Structure

```
Nucelo_Toggle/
├── Src/
│   ├── main.c              # Main application code
│   ├── syscalls.c          # System call stubs for newlib
│   └── sysmem.c            # Memory management stubs
├── Startup/
│   └── startup_stm32f401retx.s  # Startup code (vector table, Reset_Handler)
├── STM32F401RETX_FLASH.ld  # Linker script (memory layout)
├── Makefile                # Build automation
└── build/                  # Generated files (created during build)
```

### Key Files Explained

**Startup File (`startup_stm32f401retx.s`)**
- Defines the vector table (interrupt addresses)
- Contains `Reset_Handler` - first code that runs after power-on
- Initializes .data section (copies initialized variables from Flash to RAM)
- Clears .bss section (zeroes out uninitialized variables)
- Calls `main()`

**Linker Script (`STM32F401RETX_FLASH.ld`)**
- Defines memory regions (Flash at 0x08000000, RAM at 0x20000000)
- Places code sections (.text, .rodata) in Flash
- Places data sections (.data, .bss, stack, heap) in RAM
- Sets stack pointer initial value

---

## Memory Layout (STM32F401RE)

| Region | Start Address | Size | Purpose |
|--------|---------------|------|---------|
| Flash  | 0x08000000    | 512KB | Program code and constants |
| SRAM   | 0x20000000    | 96KB  | Variables, stack, heap |

**Why 0x08000000?**
- ARM Cortex-M processors start executing from address 0x00000000
- STM32 aliases Flash memory (0x08000000) to 0x00000000 at boot
- The vector table (containing stack pointer and Reset_Handler address) must be at 0x08000000

---

## Prerequisites

1. **ARM GCC Toolchain**
   - Download: [ARM GNU Toolchain](https://developer.arm.com/downloads/-/gnu-rm)
   - Required tools: `arm-none-eabi-gcc`, `arm-none-eabi-objcopy`
   - Add to system PATH

2. **Make Tool**
   - Windows: Use `make` from MinGW/Cygwin, or `mingw32-make` from STM32CubeIDE
   - Linux/Mac: Usually pre-installed

3. **Flash Tool (STM32CubeProgrammer CLI)**
   - Included with STM32CubeIDE
   - Location: `C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\`

4. **Hardware**
   - STM32F401RE Nucleo board
   - USB cable (ST-LINK connection)

---

## Option 1: Manual Build (Step-by-Step)

This section explains each compilation step manually to understand what happens under the hood.

### Step 1: Compile C Source Files

```bash
# Navigate to project directory
mkdir build
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -g -Wall -c Src/main.c -o build/main.o
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -g -Wall -c Src/syscalls.c -o build/syscalls.o
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -g -Wall -c Src/sysmem.c -o build/sysmem.o
```

**Flag Explanations:**
- `-mcpu=cortex-m4` - Target Cortex-M4 processor (enables correct instruction set)
- `-mthumb` - Use Thumb instruction set (16/32-bit mixed instructions, more code-dense)
- `-O0` - No optimization (easier debugging)
- `-g` - Include debug symbols (for GDB debugging)
- `-Wall` - Enable all warnings
- `-c` - Compile only (don't link), produce object file (.o)
- `-o` - Specify output file name

**What happens:**
- Preprocessor expands macros and includes
- Compiler translates C code to ARM assembly
- Assembler converts assembly to machine code
- Produces `.o` files (relocatable object files with symbols)

### Step 2: Assemble Startup File

```bash
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -c Startup/startup_stm32f401retx.s -o build/startup_stm32f401retx.o
```

**Why this is needed:**
- Startup code is written in assembly (`.s` file)
- Contains the vector table and Reset_Handler
- Must be assembled into machine code before linking

### Step 3: Link All Object Files

```bash
arm-none-eabi-gcc build/main.o build/syscalls.o build/sysmem.o build/startup_stm32f401retx.o \
  -mcpu=cortex-m4 -mthumb \
  -T STM32F401RETX_FLASH.ld \
  -Wl,-Map=build/nucleo_toggle.map \
  --specs=nosys.specs \
  -o build/nucleo_toggle.elf
```

**Flag Explanations:**
- `-T STM32F401RETX_FLASH.ld` - Use this linker script (defines memory layout)
- `-Wl,-Map=build/nucleo_toggle.map` - Generate map file (shows symbol addresses)
- `--specs=nosys.specs` - Use nosys variant (no operating system syscalls)
- `-o build/nucleo_toggle.elf` - Output ELF file (Executable and Linkable Format)

**What happens:**
- Linker combines all .o files
- Resolves symbol references (function calls, global variables)
- Places code/data sections according to linker script
- Produces `.elf` file (contains code, data, debug info, symbol table)

### Step 4: Convert ELF to Binary

```bash
arm-none-eabi-objcopy -O binary build/nucleo_toggle.elf build/nucleo_toggle.bin
```

**Why this is needed:**
- ELF files contain headers, debug info, symbol tables
- Flash programmer needs raw binary (just the machine code and data)
- `-O binary` strips everything except the actual program content

**Result:** `nucleo_toggle.bin` - pure binary ready to flash

### Step 5: Flash to Board

```bash
"C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe" \
  -c port=SWD \
  -w build/nucleo_toggle.bin 0x08000000 \
  -v -rst
```

**Flag Explanations:**
- `-c port=SWD` - Connect via Serial Wire Debug interface
- `-w <file> <address>` - Write binary to flash at address 0x08000000
- `-v` - Verify written data matches the file
- `-rst` - Reset MCU after flashing (starts running the new code)

---

## Option 2: Build with Makefile (Automated)

The Makefile automates all the manual steps above.

### Quick Start

```bash
make          # Build the project
make flash    # Flash to board
make clean    # Remove all build files
```

### Makefile Structure Explained

#### 1. Variables (Lines 2-32)

```makefile
TARGET = nucleo_toggle
CC = arm-none-eabi-gcc
```

**Purpose:** Define reusable values that can be referenced later using `$(VARIABLE_NAME)`.

**Why useful:**
- Change compiler once at the top instead of every rule
- Easy to modify project name, flags, or paths
- Makes Makefile portable and maintainable

#### 2. Pattern Rules (Lines 39-47)

```makefile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
```

**Syntax breakdown:**
- `$(BUILD_DIR)/%.o` - Target pattern (any .o file in build/)
- `$(SRC_DIR)/%.c` - Dependency pattern (corresponding .c file in Src/)
- `%` - Wildcard (matches any filename)
- `|` - Order-only prerequisite (build dir must exist first, but its timestamp doesn't trigger rebuild)
- `$<` - Automatic variable (first dependency, the .c file)
- `$@` - Automatic variable (target name, the .o file)

**What this does:**
- Tells Make how to build any `.o` file from its corresponding `.c` file
- Example: `build/main.o` depends on `Src/main.c`
- If `.c` is newer than `.o`, rebuild it

#### 3. Explicit Rules (Lines 49-62)

```makefile
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) -o $@
```

**Dependencies:**
- `.elf` depends on all `.o` files (stored in `$(OBJECTS)` variable)
- If any `.o` file changes, relink the `.elf`

**Cascading build:**
```
all → .bin → .elf → .o files → .c/.s files
```
Make automatically builds dependencies in the correct order.

#### 4. Special Targets

**Default target (line 35):**
```makefile
all: $(BUILD_DIR)/$(TARGET).bin
```
- Running `make` with no arguments builds the `all` target
- `all` depends on `.bin`, which triggers the entire build chain

**Phony targets (line 73):**
```makefile
.PHONY: all clean flash
```
- Tells Make these are not actual files
- Prevents conflicts if files named "clean" or "flash" exist
- These targets always run when called

#### 5. Common Makefile Automatic Variables

| Variable | Meaning |
|----------|---------|
| `$@` | Target name |
| `$<` | First dependency |
| `$^` | All dependencies |
| `$*` | The stem matched by `%` |

**Example:**
```makefile
build/main.o: Src/main.c
	gcc -c $< -o $@
```
Expands to:
```bash
gcc -c Src/main.c -o build/main.o
```

### How Make Decides What to Build

1. Check if target file exists
2. Compare timestamp of target vs dependencies
3. If dependency is newer than target, rebuild target
4. Recursively check dependencies (depth-first)

**Example scenario:**
```
make
→ Needs build/nucleo_toggle.bin
  → Needs build/nucleo_toggle.elf
    → Needs build/main.o
      → Src/main.c exists and is newer
      → Compile Src/main.c → build/main.o
    → All .o files ready
    → Link → build/nucleo_toggle.elf
  → Convert elf → build/nucleo_toggle.bin
→ Done
```

If you run `make` again without changes: "Nothing to be done for 'all'" (all files up-to-date).

---

## Understanding the Build Process

### Complete Build Flow

```
Source Files (.c, .s)
        ↓
    Compile/Assemble (-c flag)
        ↓
    Object Files (.o)
        ↓
    Link (with linker script)
        ↓
    ELF File (.elf)
        ↓
    objcopy (extract binary)
        ↓
    Binary File (.bin)
        ↓
    Flash to MCU (0x08000000)
```

### File Types Explained

| Extension | Type | Contains |
|-----------|------|----------|
| `.c` | C source | Human-readable code |
| `.s` | Assembly | Low-level assembly code |
| `.o` | Object | Compiled machine code (relocatable) |
| `.ld` | Linker script | Memory layout and section placement |
| `.elf` | Executable | Complete program with debug info and symbols |
| `.bin` | Binary | Raw machine code (for flashing) |
| `.map` | Map file | Symbol addresses and memory usage |

---

## Testing

After flashing, the onboard LED (LD2) should blink continuously.

**LED location:** Green LED next to the Arduino headers on Nucleo board (connected to PA5)

**Blink rate:** Approximately 0.9 seconds per toggle (adjust `delay()` in main.c to change)

---

## Modifying the Code

### Change blink speed
Edit `Src/main.c` line 19:
```c
for (volatile uint32_t i = 0; i < 900000; ++i) ;  // Increase = slower, decrease = faster
```

### After making changes
```bash
make clean    # Remove old build
make          # Rebuild
make flash    # Flash new version
```

---

## Common Commands Summary

| Command | Purpose |
|---------|---------|
| `make` | Build the project |
| `make clean` | Delete build directory |
| `make flash` | Flash firmware to board |
| `make all` | Same as `make` (default target) |

---

## Learning Resources

**Understanding the code:**
- Study `Src/main.c` for GPIO register manipulation
- Read STM32F401RE reference manual for register details
- Check `startup_stm32f401retx.s` to see how the MCU initializes

**Makefile tutorials:**
- GNU Make Manual: https://www.gnu.org/software/make/manual/
- Practice by adding new source files to this project

**ARM Cortex-M4:**
- ARM Cortex-M4 Technical Reference Manual
- STM32F401RE Datasheet and Reference Manual

---

## Project Info

- **Board:** STM32 Nucleo-F401RE
- **MCU:** STM32F401RET6 (ARM Cortex-M4, 84MHz)
- **Flash:** 512KB
- **RAM:** 96KB
- **Toolchain:** ARM GCC (arm-none-eabi)
- **Debug Interface:** ST-LINK V2-1 (onboard)

---

## License

Educational project - free to use and modify.
