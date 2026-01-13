# I2C Master TX - Hardcoded Version

## Overview

This directory contains a completely refactored, hardcoded version of the I2C master transmit testing code for STM32F407. All abstraction layers have been removed, exposing direct register-level hardware manipulation.

## Files Delivered

### 1. **010i2c_master_tx_testing_hardcoded.c** (295 lines)
The main hardcoded implementation with:
- Zero dependencies on driver files
- Direct register manipulation
- Embedded register structure definitions
- Complete I2C master transmit sequence
- GPIO and I2C initialization
- Button reading for trigger

### 2. **REFACTORING_NOTES.md** (480 lines)
Comprehensive documentation covering:
- Base addresses and memory map
- GPIO configuration details (bit-by-bit)
- I2C peripheral configuration
- CCR and TRISE calculations
- Clock enable sequences
- Complete I2C transmission process
- Removed abstractions explained

### 3. **COMPARISON.md** (650 lines)
Side-by-side comparison showing:
- Original abstracted code vs hardcoded
- Function expansions
- Macro expansions
- Performance differences
- Code size analysis
- When to use each approach

### 4. **REGISTER_REFERENCE.md** (370 lines)
Quick reference guide with:
- All RCC, GPIO, and I2C registers used
- Bit positions and values
- Complete memory map
- I2C communication sequence
- Common bit operations
- Pin alternate function table

## Hardware Configuration

```
STM32F407VG Discovery Board

I2C1 Configuration:
├── PB6  → I2C1_SCL (Alternate Function 4)
├── PB9  → I2C1_SDA (Alternate Function 4)
├── Mode → Standard Mode (100 kHz)
├── Pull → Internal pull-up resistors
└── Type → Open-drain outputs

Button:
└── PA0  → Input (active high)

Clock:
├── System → 16 MHz (HSI default)
└── APB1   → 16 MHz (no prescaler)

I2C Parameters:
├── Own Address  → 0x61
├── Slave Address→ 0x68
└── Speed        → 100 kHz (Standard Mode)
```

## Key Differences from Original

| Aspect | Original | Hardcoded |
|--------|----------|-----------|
| Files | 7 files | 1 file |
| Lines of code | ~2,628 | 295 |
| Dependencies | Multiple headers | None |
| Abstraction | High | None |
| Hardware visibility | Hidden | Fully exposed |
| Portability | High | Low |
| Learning value | Moderate | Very High |

## What Was Removed

1. **Driver Functions:**
   - `GPIO_Init()`
   - `I2C_Init()`
   - `I2C_PeripheralControl()`
   - `GPIO_ReadFromInputPin()`
   - `I2C_MasterSendData()`
   - `I2C_GenerateStartCondition()`
   - `I2C_ExecuteAddressPhaseWrite()`
   - `I2C_GetFlagStatus()`
   - `I2C_ClearADDRFlag()`
   - `I2C_GenerateStopCondition()`
   - `RCC_GetPCLK1Value()`

2. **Macros Expanded:**
   - All `GPIO_PIN_NO_x` macros → Direct numbers
   - All `GPIO_MODE_x` macros → Direct bit patterns
   - All `I2C_FLAG_x` macros → Direct bit operations
   - All clock enable macros → Direct register writes
   - All bit position macros → Direct bit numbers

3. **Type Definitions Simplified:**
   - Removed `I2C_Handle_t`
   - Removed `GPIO_Handle_t`
   - Removed `I2C_Config_t`
   - Kept only essential register structures

## Register Operations Exposed

### Example 1: GPIO Configuration
**Before (Abstracted):**
```c
I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
GPIO_Init(&I2CPins);
```

**After (Hardcoded):**
```c
GPIOB->MODER &= ~(0x3 << (6 * 2));  // Clear mode bits
GPIOB->MODER |= (0x2 << (6 * 2));   // Set alternate function
```

### Example 2: I2C START Condition
**Before (Abstracted):**
```c
I2C_GenerateStartCondition(pI2CHandle->pI2Cx);
```

**After (Hardcoded):**
```c
I2C1->CR1 |= (1 << 8);  // Set START bit
```

### Example 3: Flag Checking
**Before (Abstracted):**
```c
while(!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_TXE));
```

**After (Hardcoded):**
```c
while(!(I2C1->SR1 & (1 << 7)));  // Wait for TXE bit
```

## Compilation

### Original Version:
```bash
# Requires multiple files
arm-none-eabi-gcc -c stm32f407xx_gpio_driver.c
arm-none-eabi-gcc -c stm32f407xx_i2c_driver.c
arm-none-eabi-gcc -c stm32f407xx_rcc_driver.c
arm-none-eabi-gcc -c 010i2c_master_tx_testing.c
# ... link all objects
```

### Hardcoded Version:
```bash
# Single file compilation
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -g \
  -c 010i2c_master_tx_testing_hardcoded.c
```

## Usage

1. **Build the project:**
   - Use your existing Makefile/IDE
   - Or compile standalone with the command above

2. **Flash to STM32F407:**
   ```bash
   # Using ST-LINK
   st-flash write firmware.bin 0x8000000
   ```

3. **Hardware connections:**
   - Connect I2C slave device to PB6 (SCL) and PB9 (SDA)
   - Use 3.3kΩ external pull-ups OR rely on internal pull-ups
   - Connect button to PA0

4. **Test:**
   - Press button on PA0
   - Message "We are testing I2C master Tx\n" sent to slave
   - Monitor with logic analyzer or I2C sniffer

## Learning Path

For understanding bare-metal embedded programming:

1. **Start with:** `010i2c_master_tx_testing_hardcoded.c`
   - Read through the code
   - Understand the structure

2. **Then read:** `REFACTORING_NOTES.md`
   - Understand each configuration step
   - Learn the calculations (CCR, TRISE)

3. **Reference:** `REGISTER_REFERENCE.md`
   - Look up specific register bits
   - Understand memory layout

4. **Compare:** `COMPARISON.md`
   - See how abstraction works
   - Understand trade-offs

## Educational Value

This hardcoded version is excellent for:
- **Understanding I2C protocol** at hardware level
- **Learning STM32 peripherals** without HAL/driver abstraction
- **Debugging** I2C communication issues
- **Preparing for bare-metal development**
- **Interview preparation** for embedded positions
- **Teaching embedded systems** courses

## Advantages of Hardcoded Approach

✅ **Complete transparency** - See exactly what hardware does  
✅ **No hidden magic** - Every register write is visible  
✅ **Easier debugging** - Direct correlation to reference manual  
✅ **Smaller code size** - Only what you need  
✅ **Faster execution** - No function call overhead  
✅ **Self-contained** - No external dependencies  
✅ **Educational** - Learn hardware from ground up  

## Disadvantages of Hardcoded Approach

❌ **Not portable** - Specific to I2C1/PB6/PB9  
❌ **Hard to maintain** - Changes require many edits  
❌ **No error handling** - Assumes perfect conditions  
❌ **Clock-dependent** - Hardcoded for 16 MHz  
❌ **Single configuration** - Can't easily change parameters  
❌ **Code duplication** - If used in multiple places  

## When to Use This Approach

**Use Hardcoded for:**
- Learning and education
- Understanding hardware deeply
- Debugging driver issues
- Performance-critical code
- Minimal systems

**Use Driver/HAL for:**
- Production code
- Team projects
- Multiple configurations
- Long-term maintenance
- Rapid prototyping

## Next Steps

### To Extend This Code:

1. **Add Master Receive:**
   - Implement read sequence
   - Handle NACK for last byte
   
2. **Add Error Handling:**
   - Check for NACK (AF flag)
   - Handle bus errors
   - Add timeouts

3. **Support Different Clocks:**
   - Make CCR calculation dynamic
   - Read actual APB1 clock
   
4. **Add Fast Mode:**
   - Calculate CCR for 400 kHz
   - Set F/S bit in CCR register
   
5. **Add Interrupt Mode:**
   - Enable I2C interrupts
   - Implement IRQ handlers

## Testing Checklist

- [ ] Build without errors
- [ ] Flash to hardware
- [ ] Verify GPIO initialization (PB6, PB9 as AF4)
- [ ] Verify I2C clock (100 kHz on oscilloscope)
- [ ] Button triggers transmission
- [ ] START condition generated
- [ ] Slave address sent (0x68)
- [ ] Data transmitted
- [ ] STOP condition generated
- [ ] No bus errors

## Troubleshooting

**Problem:** No START condition  
**Solution:** Check I2C peripheral enabled (PE bit)

**Problem:** Slave doesn't ACK  
**Solution:** Verify slave address, check pull-ups

**Problem:** Clock too fast/slow  
**Solution:** Verify APB1 clock, recalculate CCR

**Problem:** Random data on bus  
**Solution:** Check open-drain configuration, pull-ups

**Problem:** Code hangs in while loop  
**Solution:** Check clock enables, verify peripheral power

## Reference Documents

- **RM0090** - STM32F407 Reference Manual (2,068 pages)
  - Chapter 7: GPIO
  - Chapter 27: I2C
  - Chapter 6: RCC

- **DS8626** - STM32F407VG Datasheet
  - Pinout diagrams
  - Alternate function mapping

- **PM0214** - STM32F4 Programming Manual
  - Cortex-M4 core details

## Credits

Original abstracted code from: STM32 I2C driver tutorial  
Refactored to hardcoded version: Bare-metal embedded example  
Date: 2025  
Target: STM32F407VG Discovery Board  

## License

This code is provided for educational purposes.
Use at your own risk.

---

## Summary

You now have a **complete, self-contained, bare-metal I2C master transmit implementation** that:
- Shows every register operation explicitly
- Requires no external driver files
- Demonstrates STM32 hardware at the lowest level
- Includes comprehensive documentation
- Provides side-by-side comparison with abstracted version

Perfect for learning embedded systems from first principles! 🚀

