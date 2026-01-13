# Side-by-Side Comparison: Abstracted vs Hardcoded

## Overview
This document provides a detailed comparison between the original abstracted version and the hardcoded refactored version of the I2C master TX testing code.

---

## 1. GPIO Initialization for I2C Pins

### Original (Abstracted):
```c
void I2C1_GPIOInits(void){
    GPIO_Handle_t I2CPins;

    /*Note : Internal pull-up resistors are used */
    I2CPins.pGPIOx = GPIOB;
    I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
    I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
    I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    //scl
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
    GPIO_Init(&I2CPins);

    //sda
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
    GPIO_Init(&I2CPins);
}
```

**What GPIO_Init() does internally:**
- Enables peripheral clock: `RCC->AHB1ENR |= (1 << 1);`
- Configures mode register
- Configures output type
- Configures speed
- Configures pull-up/pull-down
- Configures alternate function

### Hardcoded:
```c
void I2C1_GPIO_Init_Hardcoded(void) {
    /* Enable GPIOB clock - RCC_AHB1ENR bit 1 */
    RCC->AHB1ENR |= (1 << 1);
    
    /* Configure PB6 (SCL) */
    
    /* 1. Mode: Alternate Function (0b10) - bits [13:12] for pin 6 */
    GPIOB->MODER &= ~(0x3 << (6 * 2));      // Clear
    GPIOB->MODER |= (0x2 << (6 * 2));       // Set to AF mode
    
    /* 2. Output Type: Open Drain (0b1) - bit 6 */
    GPIOB->OTYPER |= (1 << 6);
    
    /* 3. Speed: Fast (0b10) - bits [13:12] for pin 6 */
    GPIOB->OSPEEDR &= ~(0x3 << (6 * 2));    // Clear
    GPIOB->OSPEEDR |= (0x2 << (6 * 2));     // Set to Fast
    
    /* 4. Pull-up (0b01) - bits [13:12] for pin 6 */
    GPIOB->PUPDR &= ~(0x3 << (6 * 2));      // Clear
    GPIOB->PUPDR |= (0x1 << (6 * 2));       // Set to Pull-up
    
    /* 5. Alternate Function: AF4 - AFR[0] bits [27:24] for pin 6 */
    GPIOB->AFR[0] &= ~(0xF << (6 * 4));     // Clear
    GPIOB->AFR[0] |= (0x4 << (6 * 4));      // Set to AF4
    
    /* Configure PB9 (SDA) - similar steps */
    GPIOB->MODER &= ~(0x3 << (9 * 2));
    GPIOB->MODER |= (0x2 << (9 * 2));
    GPIOB->OTYPER |= (1 << 9);
    GPIOB->OSPEEDR &= ~(0x3 << (9 * 2));
    GPIOB->OSPEEDR |= (0x2 << (9 * 2));
    GPIOB->PUPDR &= ~(0x3 << (9 * 2));
    GPIOB->PUPDR |= (0x1 << (9 * 2));
    GPIOB->AFR[1] &= ~(0xF << ((9 - 8) * 4));
    GPIOB->AFR[1] |= (0x4 << ((9 - 8) * 4));
}
```

**Key Difference:**
- Abstracted: 15 lines, hides hardware details
- Hardcoded: 32 lines, exposes every register operation
- Hardcoded shows exact bit positions and calculations

---

## 2. I2C Peripheral Initialization

### Original (Abstracted):
```c
void I2C1_Inits(void){
    I2C1Handle.pI2Cx = I2C1;
    I2C1Handle.I2C_Config.I2C_AckControl = I2C_ACK_ENABLE;
    I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
    I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
    I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;

    I2C_Init(&I2C1Handle);
}
```

**What I2C_Init() does internally:**
1. Enables I2C clock
2. Configures CR1 (ACK control)
3. Configures CR2 (FREQ field)
4. Configures OAR1 (own address)
5. Calculates and sets CCR
6. Calculates and sets TRISE

### Hardcoded:
```c
void I2C1_Init_Hardcoded(void) {
    uint32_t tempreg = 0;
    
    /* Enable I2C1 clock - RCC_APB1ENR bit 21 */
    RCC->APB1ENR |= (1 << 21);
    
    /* Configure I2C1_CR1: ACK control bit (bit 10) */
    tempreg = 0;
    tempreg |= (1 << 10);  // I2C_ACK_ENABLE
    I2C1->CR1 = tempreg;
    
    /* Configure I2C1_CR2: FREQ field (bits [5:0]) */
    /* FREQ = APB1 Clock frequency in MHz = 16 */
    tempreg = 0;
    tempreg |= 16;  // 16MHz
    I2C1->CR2 = (tempreg & 0x3F);
    
    /* Program device own address in OAR1 */
    tempreg = 0;
    tempreg |= (MY_ADDR << 1);  // 7-bit address shifted left by 1
    tempreg |= (1 << 14);        // Bit 14 should be kept at 1 by software
    I2C1->OAR1 = tempreg;
    
    /* Calculate and set CCR (Clock Control Register) */
    /* For Standard Mode: CCR = Fpclk1 / (2 * Fscl) */
    /* Fpclk1 = 16,000,000 Hz, Fscl = 100,000 Hz */
    /* CCR = 16000000 / (2 * 100000) = 80 */
    uint16_t ccr_value = 80;
    tempreg = 0;
    tempreg |= (ccr_value & 0xFFF);
    I2C1->CCR = tempreg;
    
    /* Configure TRISE (Maximum rise time) */
    /* For Standard Mode: TRISE = (Freq_APB1 in MHz) + 1 */
    /* TRISE = 16 + 1 = 17 */
    tempreg = 17;
    I2C1->TRISE = (tempreg & 0x3F);
}
```

**Key Difference:**
- Abstracted: Calculates values dynamically using `RCC_GetPCLK1Value()`
- Hardcoded: Uses fixed values (16 MHz assumed)
- Hardcoded shows the actual CCR and TRISE calculations

---

## 3. I2C Peripheral Enable

### Original (Abstracted):
```c
I2C_PeripheralControl(I2C1, ENABLE);
```

**What it does:**
```c
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
    if(EnOrDi == ENABLE)
    {
        pI2Cx->CR1 |= (1 << I2C_CR1_PE);
    }else
    {
        pI2Cx->CR1 &= ~(1 << 0);
    }
}
```

### Hardcoded:
```c
/* Enable I2C1 peripheral - Set PE bit (bit 0) in CR1 */
I2C1->CR1 |= (1 << 0);
```

**Key Difference:**
- Abstracted: Function call with macro parameter
- Hardcoded: Direct register manipulation with explicit comment

---

## 4. Button Reading

### Original (Abstracted):
```c
while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) );
```

**What it does:**
```c
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
   uint8_t value;
   value = (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001);
   return value;
}
```

### Hardcoded:
```c
while(!GPIO_Read_Input_Pin_Hardcoded(GPIOA, 0));

// Function implementation:
uint8_t GPIO_Read_Input_Pin_Hardcoded(GPIO_TypeDef *pGPIOx, uint8_t pinNumber) {
    uint8_t value;
    value = (uint8_t)((pGPIOx->IDR >> pinNumber) & 0x00000001);
    return value;
}
```

**Key Difference:**
- Functionally identical
- Hardcoded version uses direct numeric value (0) instead of macro (GPIO_PIN_NO_0)

---

## 5. I2C Master Send Data

### Original (Abstracted):
```c
I2C_MasterSendData(&I2C1Handle, some_data, strlen((char*)some_data), SLAVE_ADDR, 0);
```

**What it does internally (from driver):**
```c
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxbuffer, 
                        uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
    // 1. Generate START condition
    I2C_GenerateStartCondition(pI2CHandle->pI2Cx);
    
    // 2. Wait for SB flag
    while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));
    
    // 3. Send address with R/W=0
    I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, SlaveAddr);
    
    // 4. Wait for ADDR flag
    while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));
    
    // 5. Clear ADDR flag
    I2C_ClearADDRFlag(pI2CHandle);
    
    // 6. Send data
    while(Len > 0)
    {
        while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));
        pI2CHandle->pI2Cx->DR = *pTxbuffer;
        pTxbuffer++;
        Len--;
    }
    
    // 7. Wait for TXE and BTF
    while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));
    while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));
    
    // 8. Generate STOP condition
    if(Sr == I2C_DISABLE_SR)
        I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
}
```

### Hardcoded:
```c
I2C1_Master_Send_Data_Hardcoded(some_data, my_strlen((char*)some_data), SLAVE_ADDR);

void I2C1_Master_Send_Data_Hardcoded(uint8_t *pTxBuffer, uint32_t len, uint8_t slaveAddr) {
    /* 1. Generate START condition - Set START bit (bit 8) in CR1 */
    I2C1->CR1 |= (1 << 8);
    
    /* 2. Wait until SB flag (bit 0) in SR1 is set */
    while(!(I2C1->SR1 & (1 << 0)));
    
    /* 3. Send slave address with write bit (R/W = 0) */
    uint8_t addr = slaveAddr << 1;
    addr &= ~(1);
    I2C1->DR = addr;
    
    /* 4. Wait until ADDR flag (bit 1) in SR1 is set */
    while(!(I2C1->SR1 & (1 << 1)));
    
    /* 5. Clear ADDR flag by reading SR1 then SR2 */
    volatile uint32_t dummy_read;
    dummy_read = I2C1->SR1;
    dummy_read = I2C1->SR2;
    (void)dummy_read;
    
    /* 6. Send data until len becomes 0 */
    while(len > 0) {
        while(!(I2C1->SR1 & (1 << 7)));
        I2C1->DR = *pTxBuffer;
        pTxBuffer++;
        len--;
    }
    
    /* 7. Wait for TXE=1 and BTF=1 before generating STOP */
    while(!(I2C1->SR1 & (1 << 7)));
    while(!(I2C1->SR1 & (1 << 2)));
    
    /* 8. Generate STOP condition - Set STOP bit (bit 9) in CR1 */
    I2C1->CR1 |= (1 << 9);
}
```

**Key Difference:**
- Abstracted: Uses helper functions and macros
- Hardcoded: All flag checks are direct bit operations
- Hardcoded explicitly shows bit positions (SB=0, ADDR=1, BTF=2, TXE=7, START=8, STOP=9)
- Hardcoded removes repeated start (Sr) parameter for simplicity

---

## 6. Macro Expansions

### Clock Enable Macros

**Original:**
```c
#define I2C1_PCLK_EN() (RCC->APB1ENR |= (1 << 21))
// Usage:
I2C1_PCLK_EN();
```

**Hardcoded:**
```c
/* Enable I2C1 clock - RCC_APB1ENR bit 21 */
RCC->APB1ENR |= (1 << 21);
```

### Flag Check Macros

**Original:**
```c
#define I2C_FLAG_SB    (1 << I2C_SR1_SB)  // where I2C_SR1_SB = 0
#define I2C_FLAG_TXE   (1 << I2C_SR1_TXE) // where I2C_SR1_TXE = 7

// Usage:
I2C_GetFlagStatus(pI2Cx, I2C_FLAG_SB)
```

**Hardcoded:**
```c
/* Check SB flag directly */
(I2C1->SR1 & (1 << 0))

/* Check TXE flag directly */
(I2C1->SR1 & (1 << 7))
```

### Bit Position Macros

**Original:**
```c
#define I2C_CR1_START  8
#define I2C_CR1_STOP   9
#define I2C_CR1_PE     0

// Usage:
pI2Cx->CR1 |= (1 << I2C_CR1_START);
```

**Hardcoded:**
```c
/* No macros - direct bit positions with comments */
I2C1->CR1 |= (1 << 8);  // START bit
I2C1->CR1 |= (1 << 9);  // STOP bit
I2C1->CR1 |= (1 << 0);  // PE (Peripheral Enable) bit
```

---

## 7. Type Definitions

### Original (Abstracted):
```c
// From stm32f407xx.h
typedef struct
{
    __vo uint32_t CR1;
    __vo uint32_t CR2;
    __vo uint32_t OAR1;
    __vo uint32_t OAR2;
    __vo uint32_t DR;
    __vo uint32_t SR1;
    __vo uint32_t SR2;
    __vo uint32_t CCR;
    __vo uint32_t TRISE;
    __vo uint32_t FLTR;
} I2C_RegDef_t;

#define I2C1  ((I2C_RegDef_t*)I2C1_BASEADDR)

// From stm32f407xx_i2c_driver.h
typedef struct
{
    I2C_RegDef_t *pI2Cx;
    I2C_Config_t I2C_Config;
    // ... more fields
} I2C_Handle_t;
```

### Hardcoded:
```c
// Minimal type definitions
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t CCR;
    volatile uint32_t TRISE;
    volatile uint32_t FLTR;
} I2C_TypeDef;

#define I2C1  ((I2C_TypeDef*)I2C1_BASE)

// No handle structure - direct peripheral access
```

**Key Difference:**
- Abstracted: Uses handle structures to manage state
- Hardcoded: Direct peripheral access without handles
- Hardcoded has no driver state management

---

## 8. Dependencies

### Original (Abstracted):
**Required Files:**
- `stm32f407xx.h` (800+ lines)
- `stm32f407xx_gpio_driver.h` (139 lines)
- `stm32f407xx_gpio_driver.c` (439 lines)
- `stm32f407xx_i2c_driver.h` (162 lines)
- `stm32f407xx_i2c_driver.c` (941 lines)
- `stm32f407xx_rcc_driver.h` (22 lines)
- `stm32f407xx_rcc_driver.c` (125 lines)

**Total: ~2,628 lines across 7 files**

### Hardcoded:
**Required Files:**
- `010i2c_master_tx_testing_hardcoded.c` (295 lines)

**Total: 295 lines in 1 file**

---

## 9. Portability

### Original (Abstracted):
- **Portable across STM32F4 series**
- Easy to switch I2C peripherals (I2C1, I2C2, I2C3)
- Easy to change pin configurations
- Works with different clock configurations
- Supports interrupt mode (not used in this example)

### Hardcoded:
- **Not portable** - specific to I2C1 on PB6/PB9
- Hardcoded for 16 MHz clock only
- Would need complete rewrite for different configuration
- Polling mode only
- Educational value for understanding hardware

---

## 10. Performance

### Code Size:
- **Abstracted**: Larger due to function call overhead and unused driver functions
- **Hardcoded**: Smaller - only includes what's needed

### Execution Speed:
- **Abstracted**: Slower due to function call overhead
- **Hardcoded**: Faster - direct register access, no function calls

### Optimization:
- **Abstracted**: Compiler must optimize through multiple function layers
- **Hardcoded**: Already optimized - no unnecessary indirection

---

## Summary Table

| Aspect | Abstracted | Hardcoded |
|--------|-----------|-----------|
| **Lines of code (single file)** | 117 | 295 |
| **Total lines (with drivers)** | ~2,628 | 295 |
| **Dependencies** | 7 files | 1 file |
| **Portability** | High | None |
| **Readability** | Simple API | Verbose but clear |
| **Learning curve** | Requires understanding drivers | Direct hardware access |
| **Debugging** | Abstract | Concrete |
| **Maintainability** | Easy | Hard |
| **Performance** | Good | Excellent |
| **Educational value** | Hides details | Shows everything |
| **Best for** | Production code | Learning hardware |

---

## When to Use Each Approach

### Use Abstracted (Driver-based):
- Production embedded systems
- Team projects with multiple developers
- Projects requiring code reuse
- When supporting multiple hardware configurations
- When using vendor-provided HAL/LL libraries

### Use Hardcoded:
- Learning embedded systems
- Understanding hardware at register level
- Debugging driver issues
- Performance-critical sections
- Minimal resource environments
- Educational demonstrations
- Quick prototypes for specific hardware

---

## Conclusion

The abstracted version provides a clean, maintainable API that hides hardware complexity. The hardcoded version exposes every register operation, making it excellent for learning but impractical for production. Both approaches have their place in embedded development - use abstraction for maintainability, use hardcoded for understanding.

