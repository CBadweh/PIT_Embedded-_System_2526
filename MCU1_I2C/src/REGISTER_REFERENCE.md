# STM32F407 Register Reference - I2C Master TX

Quick reference for all registers used in the hardcoded I2C master TX implementation.

---

## RCC (Reset and Clock Control) - 0x40023800

### RCC_AHB1ENR (AHB1 Peripheral Clock Enable Register)
**Offset:** 0x30  
**Address:** 0x40023830

| Bit | Name      | Value | Description                |
|-----|-----------|-------|----------------------------|
| 0   | GPIOAEN   | 1     | GPIOA clock enable         |
| 1   | GPIOBEN   | 1     | GPIOB clock enable         |

**Usage:**
```c
RCC->AHB1ENR |= (1 << 0);  // Enable GPIOA
RCC->AHB1ENR |= (1 << 1);  // Enable GPIOB
```

### RCC_APB1ENR (APB1 Peripheral Clock Enable Register)
**Offset:** 0x40  
**Address:** 0x40023840

| Bit | Name    | Value | Description             |
|-----|---------|-------|-------------------------|
| 21  | I2C1EN  | 1     | I2C1 clock enable       |

**Usage:**
```c
RCC->APB1ENR |= (1 << 21);  // Enable I2C1
```

---

## GPIOA (General Purpose I/O Port A) - 0x40020000

### GPIOA_MODER (Port Mode Register)
**Offset:** 0x00  
**Address:** 0x40020000

| Bits | Pin | Value | Description  |
|------|-----|-------|--------------|
| [1:0]| PA0 | 0b00  | Input mode   |

**Values:**
- 0b00 = Input mode
- 0b01 = General purpose output mode
- 0b10 = Alternate function mode
- 0b11 = Analog mode

**Usage:**
```c
GPIOA->MODER &= ~(0x3 << (0 * 2));  // PA0 as input
```

### GPIOA_OSPEEDR (Port Output Speed Register)
**Offset:** 0x08  
**Address:** 0x40020008

| Bits | Pin | Value | Description |
|------|-----|-------|-------------|
| [1:0]| PA0 | 0b10  | Fast speed  |

**Values:**
- 0b00 = Low speed
- 0b01 = Medium speed
- 0b10 = Fast speed
- 0b11 = High speed

### GPIOA_PUPDR (Port Pull-up/Pull-down Register)
**Offset:** 0x0C  
**Address:** 0x4002000C

| Bits | Pin | Value | Description      |
|------|-----|-------|------------------|
| [1:0]| PA0 | 0b00  | No pull-up/down  |

**Values:**
- 0b00 = No pull-up, no pull-down
- 0b01 = Pull-up
- 0b10 = Pull-down
- 0b11 = Reserved

### GPIOA_IDR (Port Input Data Register)
**Offset:** 0x10  
**Address:** 0x40020010

| Bits  | Description                        |
|-------|------------------------------------|
| [15:0]| Input data (read only)             |

**Usage:**
```c
uint8_t value = (GPIOA->IDR >> 0) & 0x1;  // Read PA0
```

---

## GPIOB (General Purpose I/O Port B) - 0x40020400

### GPIOB_MODER (Port Mode Register)
**Offset:** 0x00  
**Address:** 0x40020400

| Bits    | Pin | Value | Description           |
|---------|-----|-------|-----------------------|
| [13:12] | PB6 | 0b10  | Alternate function    |
| [19:18] | PB9 | 0b10  | Alternate function    |

**Usage:**
```c
GPIOB->MODER &= ~(0x3 << (6 * 2));
GPIOB->MODER |= (0x2 << (6 * 2));  // PB6 as AF
```

### GPIOB_OTYPER (Port Output Type Register)
**Offset:** 0x04  
**Address:** 0x40020404

| Bit | Pin | Value | Description    |
|-----|-----|-------|----------------|
| 6   | PB6 | 1     | Open drain     |
| 9   | PB9 | 1     | Open drain     |

**Values:**
- 0 = Push-pull
- 1 = Open drain

**Usage:**
```c
GPIOB->OTYPER |= (1 << 6);  // PB6 as open drain
```

### GPIOB_OSPEEDR (Port Output Speed Register)
**Offset:** 0x08  
**Address:** 0x40020408

| Bits    | Pin | Value | Description |
|---------|-----|-------|-------------|
| [13:12] | PB6 | 0b10  | Fast speed  |
| [19:18] | PB9 | 0b10  | Fast speed  |

### GPIOB_PUPDR (Port Pull-up/Pull-down Register)
**Offset:** 0x0C  
**Address:** 0x4002040C

| Bits    | Pin | Value | Description |
|---------|-----|-------|-------------|
| [13:12] | PB6 | 0b01  | Pull-up     |
| [19:18] | PB9 | 0b01  | Pull-up     |

### GPIOB_AFR[0] (Alternate Function Low Register)
**Offset:** 0x20  
**Address:** 0x40020420

| Bits    | Pin | Value | Description     |
|---------|-----|-------|-----------------|
| [27:24] | PB6 | 0x4   | AF4 (I2C1_SCL)  |

**Usage:**
```c
GPIOB->AFR[0] &= ~(0xF << (6 * 4));
GPIOB->AFR[0] |= (0x4 << (6 * 4));  // PB6 = AF4
```

### GPIOB_AFR[1] (Alternate Function High Register)
**Offset:** 0x24  
**Address:** 0x40020424

| Bits  | Pin | Value | Description     |
|-------|-----|-------|-----------------|
| [7:4] | PB9 | 0x4   | AF4 (I2C1_SDA)  |

**Usage:**
```c
GPIOB->AFR[1] &= ~(0xF << ((9-8) * 4));
GPIOB->AFR[1] |= (0x4 << ((9-8) * 4));  // PB9 = AF4
```

---

## I2C1 (Inter-Integrated Circuit) - 0x40005400

### I2C1_CR1 (Control Register 1)
**Offset:** 0x00  
**Address:** 0x40005400

| Bit | Name      | R/W | Description                              |
|-----|-----------|-----|------------------------------------------|
| 0   | PE        | R/W | Peripheral enable                        |
| 8   | START     | R/W | Start generation                         |
| 9   | STOP      | R/W | Stop generation                          |
| 10  | ACK       | R/W | Acknowledge enable                       |

**Usage:**
```c
I2C1->CR1 |= (1 << 0);   // Enable peripheral (PE)
I2C1->CR1 |= (1 << 8);   // Generate START
I2C1->CR1 |= (1 << 9);   // Generate STOP
I2C1->CR1 |= (1 << 10);  // Enable ACK
```

### I2C1_CR2 (Control Register 2)
**Offset:** 0x04  
**Address:** 0x40005404

| Bits  | Name | R/W | Description                             |
|-------|------|-----|-----------------------------------------|
| [5:0] | FREQ | R/W | Peripheral clock frequency (in MHz)     |

**Usage:**
```c
I2C1->CR2 = 16;  // 16 MHz APB1 clock
```

### I2C1_OAR1 (Own Address Register 1)
**Offset:** 0x08  
**Address:** 0x40005408

| Bits  | Name     | R/W | Description                      |
|-------|----------|-----|----------------------------------|
| [7:1] | ADD[7:1] | R/W | 7-bit own address                |
| 14    | -        | R/W | Should always be kept at 1       |

**Usage:**
```c
I2C1->OAR1 = (MY_ADDR << 1) | (1 << 14);
```

### I2C1_DR (Data Register)
**Offset:** 0x10  
**Address:** 0x40005410

| Bits  | Name | R/W | Description                     |
|-------|------|-----|---------------------------------|
| [7:0] | DR   | R/W | 8-bit data register             |

**Usage:**
```c
I2C1->DR = data;           // Write data
uint8_t data = I2C1->DR;   // Read data
```

### I2C1_SR1 (Status Register 1)
**Offset:** 0x14  
**Address:** 0x40005414

| Bit | Name  | R/W | Description                             |
|-----|-------|-----|-----------------------------------------|
| 0   | SB    | R   | Start bit (Master mode)                 |
| 1   | ADDR  | R   | Address sent/matched                    |
| 2   | BTF   | R   | Byte transfer finished                  |
| 6   | RXNE  | R   | Receive buffer not empty                |
| 7   | TXE   | R   | Transmit buffer empty                   |

**Usage:**
```c
while(!(I2C1->SR1 & (1 << 0)));  // Wait for SB
while(!(I2C1->SR1 & (1 << 1)));  // Wait for ADDR
while(!(I2C1->SR1 & (1 << 2)));  // Wait for BTF
while(!(I2C1->SR1 & (1 << 7)));  // Wait for TXE
```

### I2C1_SR2 (Status Register 2)
**Offset:** 0x18  
**Address:** 0x40005418

| Bit | Name | R/W | Description                         |
|-----|------|-----|-------------------------------------|
| 0   | MSL  | R   | Master/Slave mode                   |
| 1   | BUSY | R   | Bus busy                            |
| 2   | TRA  | R   | Transmitter/Receiver                |

**Usage:**
```c
volatile uint32_t dummy = I2C1->SR2;  // Read to clear ADDR flag
```

### I2C1_CCR (Clock Control Register)
**Offset:** 0x1C  
**Address:** 0x4000541C

| Bits   | Name | R/W | Description                          |
|--------|------|-----|--------------------------------------|
| [11:0] | CCR  | R/W | Clock control register value         |
| 15     | F/S  | R/W | Fast mode selection (0=Sm, 1=Fm)     |

**Standard Mode Calculation:**
```
CCR = F_PCLK1 / (2 × F_SCL)
    = 16,000,000 / (2 × 100,000)
    = 80
```

**Usage:**
```c
I2C1->CCR = 80;  // 100 kHz in standard mode
```

### I2C1_TRISE (Rise Time Register)
**Offset:** 0x20  
**Address:** 0x40005420

| Bits  | Name  | R/W | Description                     |
|-------|-------|-----|---------------------------------|
| [5:0] | TRISE | R/W | Maximum rise time               |

**Standard Mode Calculation:**
```
TRISE = F_PCLK1_MHz + 1
      = 16 + 1
      = 17
```

**Usage:**
```c
I2C1->TRISE = 17;  // For 16 MHz clock
```

---

## I2C Communication Sequence

### Master Transmit Sequence:

```
1. Generate START:        I2C1->CR1 |= (1 << 8);
2. Wait for SB:           while(!(I2C1->SR1 & (1 << 0)));
3. Send address (Write):  I2C1->DR = (SlaveAddr << 1) | 0;
4. Wait for ADDR:         while(!(I2C1->SR1 & (1 << 1)));
5. Clear ADDR:            dummy = I2C1->SR1; dummy = I2C1->SR2;
6. Wait for TXE:          while(!(I2C1->SR1 & (1 << 7)));
7. Send data:             I2C1->DR = data;
8. Wait for BTF:          while(!(I2C1->SR1 & (1 << 2)));
9. Generate STOP:         I2C1->CR1 |= (1 << 9);
```

---

## Memory Map Summary

| Peripheral | Base Address | Size   |
|------------|--------------|--------|
| GPIOA      | 0x40020000   | 0x400  |
| GPIOB      | 0x40020400   | 0x400  |
| RCC        | 0x40023800   | 0x400  |
| I2C1       | 0x40005400   | 0x400  |

---

## Pin Alternate Functions

| Pin | AF0 | AF1 | AF2 | AF3 | **AF4**     | AF5 |
|-----|-----|-----|-----|-----|-------------|-----|
| PB6 | -   | TIM4| -   | -   | **I2C1_SCL**| -   |
| PB9 | -   | TIM4| -   | -   | **I2C1_SDA**| -   |

---

## Quick Bit Position Reference

### Common Operations:

```c
/* Enable clocks */
RCC->AHB1ENR |= (1 << 0);   // GPIOA
RCC->AHB1ENR |= (1 << 1);   // GPIOB
RCC->APB1ENR |= (1 << 21);  // I2C1

/* I2C control */
I2C1->CR1 |= (1 << 0);      // Enable peripheral
I2C1->CR1 |= (1 << 8);      // START
I2C1->CR1 |= (1 << 9);      // STOP
I2C1->CR1 |= (1 << 10);     // ACK

/* I2C status checks */
I2C1->SR1 & (1 << 0)        // SB (Start Bit)
I2C1->SR1 & (1 << 1)        // ADDR (Address sent)
I2C1->SR1 & (1 << 2)        // BTF (Byte Transfer Finished)
I2C1->SR1 & (1 << 7)        // TXE (Transmit Empty)

/* GPIO mode for pin N */
GPIOx->MODER &= ~(0x3 << (N * 2));  // Clear
GPIOx->MODER |= (mode << (N * 2));   // Set

/* GPIO alternate function for pin N (N < 8) */
GPIOx->AFR[0] &= ~(0xF << (N * 4));
GPIOx->AFR[0] |= (AF << (N * 4));

/* GPIO alternate function for pin N (N >= 8) */
GPIOx->AFR[1] &= ~(0xF << ((N-8) * 4));
GPIOx->AFR[1] |= (AF << ((N-8) * 4));
```

---

## Reference Documents

- **RM0090**: STM32F407 Reference Manual
  - Section 7: General-purpose I/Os (GPIO)
  - Section 27: Inter-integrated circuit (I2C) interface
  - Section 6: Reset and clock control (RCC)
  
- **DS8626**: STM32F407VG Datasheet
  - Table 9: Alternate function mapping

---

## Notes

1. All registers are 32-bit wide
2. Always read-modify-write for bit operations
3. `volatile` keyword prevents compiler optimization
4. ADDR flag cleared by reading SR1 then SR2
5. Open-drain outputs require pull-up resistors
6. I2C speed depends on APB1 clock frequency

