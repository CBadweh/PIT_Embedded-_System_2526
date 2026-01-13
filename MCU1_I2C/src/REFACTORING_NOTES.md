# I2C Master TX Testing - Hardcoded Version Documentation

## Overview
This document explains the refactoring of `010i2c_master_tx_testing.c` to its hardcoded version, removing all abstraction layers and using direct register manipulation.

## File: `010i2c_master_tx_testing_hardcoded.c`

## Key Changes

### 1. **Removed Dependencies**
- No longer includes driver header files (`stm32f407xx.h`, `stm32f407xx_i2c_driver.h`, `stm32f407xx_gpio_driver.h`)
- Removed all typedef structures from drivers (`I2C_Handle_t`, `GPIO_Handle_t`, etc.)
- All register definitions embedded directly in the file

### 2. **Base Address Definitions**
```c
Peripheral Base Addresses (STM32F407VG):
- PERIPH_BASE:     0x40000000  (Peripheral base)
- AHB1PERIPH_BASE: 0x40020000  (AHB1 bus peripherals)
- APB1PERIPH_BASE: 0x40000000  (APB1 bus peripherals)
- GPIOA_BASE:      0x40020000  (GPIOA)
- GPIOB_BASE:      0x40020400  (GPIOB)
- I2C1_BASE:       0x40005400  (I2C1)
- RCC_BASE:        0x40023800  (Reset and Clock Control)
```

### 3. **GPIO Configuration (GPIOB - I2C Pins)**

#### Pin Configuration for PB6 (I2C1_SCL):
| Register   | Bits      | Value | Description                    |
|------------|-----------|-------|--------------------------------|
| MODER      | [13:12]   | 0b10  | Alternate Function Mode        |
| OTYPER     | [6]       | 0b1   | Open Drain                     |
| OSPEEDR    | [13:12]   | 0b10  | Fast Speed                     |
| PUPDR      | [13:12]   | 0b01  | Pull-up enabled                |
| AFR[0]     | [27:24]   | 0x4   | AF4 (I2C1_SCL)                 |

#### Pin Configuration for PB9 (I2C1_SDA):
| Register   | Bits      | Value | Description                    |
|------------|-----------|-------|--------------------------------|
| MODER      | [19:18]   | 0b10  | Alternate Function Mode        |
| OTYPER     | [9]       | 0b1   | Open Drain                     |
| OSPEEDR    | [19:18]   | 0b10  | Fast Speed                     |
| PUPDR      | [19:18]   | 0b01  | Pull-up enabled                |
| AFR[1]     | [7:4]     | 0x4   | AF4 (I2C1_SDA)                 |

#### Pin Configuration for PA0 (Button):
| Register   | Bits      | Value | Description                    |
|------------|-----------|-------|--------------------------------|
| MODER      | [1:0]     | 0b00  | Input Mode                     |
| OSPEEDR    | [1:0]     | 0b10  | Fast Speed                     |
| PUPDR      | [1:0]     | 0b00  | No Pull-up/Pull-down           |

### 4. **I2C1 Configuration**

#### Assumptions:
- **System Clock**: 16 MHz (HSI - default after reset)
- **APB1 Clock**: 16 MHz (no prescaler)
- **I2C Mode**: Standard Mode (100 kHz)
- **Own Address**: 0x61
- **Slave Address**: 0x68

#### I2C1 Register Configuration:

**CR1 (Control Register 1):**
| Bit  | Name | Value | Description           |
|------|------|-------|-----------------------|
| 10   | ACK  | 1     | Acknowledge enabled   |
| 0    | PE   | 1     | Peripheral enabled    |

**CR2 (Control Register 2):**
| Bits  | Name  | Value | Description                 |
|-------|-------|-------|-----------------------------|
| [5:0] | FREQ  | 16    | APB1 frequency = 16 MHz     |

**OAR1 (Own Address Register 1):**
| Bits   | Name    | Value         | Description                |
|--------|---------|---------------|----------------------------|
| [7:1]  | ADD[7:1]| 0x61 << 1     | Own 7-bit address          |
| 14     | -       | 1             | Must be kept at 1          |

**CCR (Clock Control Register):**
| Bits   | Name | Value | Description                     |
|--------|------|-------|---------------------------------|
| [11:0] | CCR  | 80    | Clock control in Standard Mode  |
| 15     | F/S  | 0     | Standard Mode (Sm)              |

**Calculation:**
```
CCR = F_PCLK1 / (2 × F_SCL)
    = 16,000,000 / (2 × 100,000)
    = 80
```

**TRISE (Rise Time Register):**
| Bits  | Name  | Value | Description          |
|-------|-------|-------|----------------------|
| [5:0] | TRISE | 17    | Maximum rise time    |

**Calculation:**
```
TRISE = F_PCLK1_MHz + 1
      = 16 + 1
      = 17
```

### 5. **I2C Master Transmission Process**

The `I2C1_Master_Send_Data_Hardcoded()` function implements the complete I2C transmission sequence:

#### Step-by-Step Process:

1. **Generate START Condition**
   - Set START bit (bit 8) in I2C1->CR1
   - Master initiates communication

2. **Wait for SB Flag**
   - Poll SR1 bit 0 until set
   - Indicates START condition generated

3. **Send Slave Address**
   - Write (SlaveAddr << 1) | 0 to DR
   - R/W bit = 0 for write operation

4. **Wait for ADDR Flag**
   - Poll SR1 bit 1 until set
   - Indicates address sent and acknowledged

5. **Clear ADDR Flag**
   - Read SR1, then read SR2
   - Hardware requirement to clear flag

6. **Data Transmission Loop**
   - For each byte:
     - Wait for TXE flag (SR1 bit 7)
     - Write byte to DR register
     - Decrement length counter

7. **Wait for Transmission Complete**
   - Wait for TXE = 1 (SR1 bit 7)
   - Wait for BTF = 1 (SR1 bit 2)
   - Both flags ensure complete transmission

8. **Generate STOP Condition**
   - Set STOP bit (bit 9) in I2C1->CR1
   - Terminates communication

### 6. **Clock Enable Registers**

| Peripheral | Register      | Bit | Value |
|------------|---------------|-----|-------|
| GPIOA      | RCC->AHB1ENR  | 0   | 1     |
| GPIOB      | RCC->AHB1ENR  | 1   | 1     |
| I2C1       | RCC->APB1ENR  | 21  | 1     |

### 7. **Removed Abstractions**

#### Original Code → Hardcoded Equivalent:

```c
// Original:
GPIO_Init(&I2CPins);

// Hardcoded:
GPIOB->MODER &= ~(0x3 << (6 * 2));
GPIOB->MODER |= (0x2 << (6 * 2));
// ... (multiple register writes)
```

```c
// Original:
I2C_Init(&I2C1Handle);

// Hardcoded:
RCC->APB1ENR |= (1 << 21);
I2C1->CR1 = (1 << 10);
I2C1->CR2 = 16;
// ... (multiple register writes)
```

```c
// Original:
I2C_PeripheralControl(I2C1, ENABLE);

// Hardcoded:
I2C1->CR1 |= (1 << 0);
```

```c
// Original:
GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0);

// Hardcoded:
(GPIOA->IDR >> 0) & 0x00000001
```

```c
// Original:
I2C_MasterSendData(&I2C1Handle, some_data, strlen((char*)some_data), SLAVE_ADDR, 0);

// Hardcoded:
I2C1_Master_Send_Data_Hardcoded(some_data, strlen((char*)some_data), SLAVE_ADDR);
// (with full register-level implementation)
```

### 8. **Benefits of Hardcoded Version**

**Advantages:**
- No driver dependencies
- Direct understanding of hardware
- Easier debugging at register level
- Smaller code size
- Faster execution (no function call overhead)
- Educational value for understanding hardware

**Disadvantages:**
- Less portable
- No parameter validation
- Harder to maintain
- Duplicate code if used in multiple files
- No error handling
- Tightly coupled to specific hardware

### 9. **Testing Notes**

To test this code:
1. Connect I2C slave device to PB6 (SCL) and PB9 (SDA)
2. Ensure pull-up resistors (3.3kΩ) or use internal pull-ups
3. Connect button to PA0
4. Flash the hardcoded program
5. Press button to trigger transmission
6. Monitor I2C bus with logic analyzer

### 10. **Reference Documents**

- STM32F407VG Reference Manual (RM0090)
  - Section 7: GPIO
  - Section 27: I2C
  - Section 6: RCC
- STM32F407VG Datasheet
  - Pinout and alternate functions

## Compilation

This file is standalone and can be compiled without driver files:

```bash
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb \
  -O0 -g \
  -I. \
  -c 010i2c_master_tx_testing_hardcoded.c \
  -o 010i2c_master_tx_testing_hardcoded.o
```

## Summary

The hardcoded version demonstrates bare-metal programming for STM32F407, showing exactly what happens at the hardware level when initializing and using I2C communication. Every macro, function call, and configuration option has been replaced with direct register access, making the hardware interaction completely transparent.

