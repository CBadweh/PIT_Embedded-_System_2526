/*
 * 010i2c_master_tx_testing_hardcoded.c
 * 
 * Hardcoded version - All abstraction layers removed
 * Direct register manipulation for STM32F407
 *
 * Hardware Configuration:
 * - I2C1: PB6 (SCL), PB9 (SDA)
 * - Button: PA0
 * - Slave Address: 0x68
 * - I2C Speed: Standard Mode (100kHz)
 * - Clock: 16MHz HSI (default), APB1 = 16MHz
 */

#include <stdint.h>

/* Local strlen implementation for bare-metal */
static uint32_t my_strlen(const char *str) {
    uint32_t len = 0;
    while(str[len] != '\0') {
        len++;
    }
    return len;
}

/* Base Addresses */
#define PERIPH_BASE           0x40000000U
#define AHB1PERIPH_BASE       0x40020000U
#define APB1PERIPH_BASE       PERIPH_BASE

#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400)
#define I2C1_BASE             (APB1PERIPH_BASE + 0x5400)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800)

/* Register Structures */
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} GPIO_TypeDef;

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

typedef struct {
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t AHB3RSTR;
    uint32_t RESERVED0;
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    uint32_t RESERVED1[2];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t AHB3ENR;
    uint32_t RESERVED2;
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
} RCC_TypeDef;

/* Peripheral Pointers */
#define GPIOA                 ((GPIO_TypeDef*)GPIOA_BASE)
#define GPIOB                 ((GPIO_TypeDef*)GPIOB_BASE)
#define I2C1                  ((I2C_TypeDef*)I2C1_BASE)
#define RCC                   ((RCC_TypeDef*)RCC_BASE)

/* Configuration */
#define MY_ADDR               0x61
#define SLAVE_ADDR            0x68

/* Data to send */
uint8_t some_data[] = "We are testing I2C master Tx\n";

/* Delay function */
void delay(void) {
    for(uint32_t i = 0; i < 500000/2; i++);
}

/*
 * I2C1 GPIO Initialization - Hardcoded
 * PB6 -> I2C1_SCL (AF4)
 * PB9 -> I2C1_SDA (AF4)
 */
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
    
    /* Configure PB9 (SDA) */
    
    /* 1. Mode: Alternate Function (0b10) - bits [19:18] for pin 9 */
    GPIOB->MODER &= ~(0x3 << (9 * 2));      // Clear
    GPIOB->MODER |= (0x2 << (9 * 2));       // Set to AF mode
    
    /* 2. Output Type: Open Drain (0b1) - bit 9 */
    GPIOB->OTYPER |= (1 << 9);
    
    /* 3. Speed: Fast (0b10) - bits [19:18] for pin 9 */
    GPIOB->OSPEEDR &= ~(0x3 << (9 * 2));    // Clear
    GPIOB->OSPEEDR |= (0x2 << (9 * 2));     // Set to Fast
    
    /* 4. Pull-up (0b01) - bits [19:18] for pin 9 */
    GPIOB->PUPDR &= ~(0x3 << (9 * 2));      // Clear
    GPIOB->PUPDR |= (0x1 << (9 * 2));       // Set to Pull-up
    
    /* 5. Alternate Function: AF4 - AFR[1] bits [7:4] for pin 9 */
    GPIOB->AFR[1] &= ~(0xF << ((9 - 8) * 4)); // Clear
    GPIOB->AFR[1] |= (0x4 << ((9 - 8) * 4));  // Set to AF4
}

/*
 * I2C1 Initialization - Hardcoded
 * Standard Mode (100kHz), ACK enabled
 * Assuming APB1 Clock = 16MHz
 */
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
    tempreg |= (ccr_value & 0xFFF);  // Standard mode (bit 15 = 0)
    I2C1->CCR = tempreg;
    
    /* Configure TRISE (Maximum rise time) */
    /* For Standard Mode: TRISE = (Freq_APB1 in MHz) + 1 */
    /* TRISE = 16 + 1 = 17 */
    tempreg = 17;
    I2C1->TRISE = (tempreg & 0x3F);
}

/*
 * Button Initialization - Hardcoded
 * PA0 as input
 */
void GPIO_Button_Init_Hardcoded(void) {
    /* Enable GPIOA clock - RCC_AHB1ENR bit 0 */
    RCC->AHB1ENR |= (1 << 0);
    
    /* Configure PA0 */
    
    /* 1. Mode: Input (0b00) - bits [1:0] for pin 0 */
    GPIOA->MODER &= ~(0x3 << (0 * 2));      // Clear (input mode)
    
    /* 2. Speed: Fast (0b10) - bits [1:0] for pin 0 */
    GPIOA->OSPEEDR &= ~(0x3 << (0 * 2));    // Clear
    GPIOA->OSPEEDR |= (0x2 << (0 * 2));     // Set to Fast
    
    /* 3. No Pull-up/Pull-down (0b00) - bits [1:0] for pin 0 */
    GPIOA->PUPDR &= ~(0x3 << (0 * 2));      // Clear (no PUPD)
}

/*
 * I2C Master Send Data - Hardcoded
 * Blocking mode transmission
 */
void I2C1_Master_Send_Data_Hardcoded(uint8_t *pTxBuffer, uint32_t len, uint8_t slaveAddr) {
    /* 1. Generate START condition - Set START bit (bit 8) in CR1 */
    I2C1->CR1 |= (1 << 8);
    
    /* 2. Wait until SB flag (bit 0) in SR1 is set */
    while(!(I2C1->SR1 & (1 << 0)));
    
    /* 3. Send slave address with write bit (R/W = 0) */
    uint8_t addr = slaveAddr << 1;  // Shift address left
    addr &= ~(1);                    // Clear bit 0 for write
    I2C1->DR = addr;
    
    /* 4. Wait until ADDR flag (bit 1) in SR1 is set */
    while(!(I2C1->SR1 & (1 << 1)));
    
    /* 5. Clear ADDR flag by reading SR1 then SR2 */
    volatile uint32_t dummy_read;
    dummy_read = I2C1->SR1;
    dummy_read = I2C1->SR2;
    (void)dummy_read;  // Avoid compiler warning
    
    /* 6. Send data until len becomes 0 */
    while(len > 0) {
        /* Wait until TXE flag (bit 7) in SR1 is set */
        while(!(I2C1->SR1 & (1 << 7)));
        
        /* Load data into DR */
        I2C1->DR = *pTxBuffer;
        pTxBuffer++;
        len--;
    }
    
    /* 7. Wait for TXE=1 and BTF=1 before generating STOP */
    while(!(I2C1->SR1 & (1 << 7)));  // Wait for TXE
    while(!(I2C1->SR1 & (1 << 2)));  // Wait for BTF (bit 2)
    
    /* 8. Generate STOP condition - Set STOP bit (bit 9) in CR1 */
    I2C1->CR1 |= (1 << 9);
}

/*
 * Read GPIO Input Pin - Hardcoded
 */
uint8_t GPIO_Read_Input_Pin_Hardcoded(GPIO_TypeDef *pGPIOx, uint8_t pinNumber) {
    uint8_t value;
    value = (uint8_t)((pGPIOx->IDR >> pinNumber) & 0x00000001);
    return value;
}

int main(void) {
    /* Initialize button */
    GPIO_Button_Init_Hardcoded();
    
    /* Initialize I2C1 GPIO pins */
    I2C1_GPIO_Init_Hardcoded();
    
    /* Initialize I2C1 peripheral */
    I2C1_Init_Hardcoded();
    
    /* Enable I2C1 peripheral - Set PE bit (bit 0) in CR1 */
    I2C1->CR1 |= (1 << 0);
    
    while(1) {
        /* Wait until button is pressed (PA0 = 1) */
        while(!GPIO_Read_Input_Pin_Hardcoded(GPIOA, 0));
        
        /* Delay for button debouncing */
        delay();
        
        /* Send data to slave */
        I2C1_Master_Send_Data_Hardcoded(some_data, my_strlen((char*)some_data), SLAVE_ADDR);
    }
}

