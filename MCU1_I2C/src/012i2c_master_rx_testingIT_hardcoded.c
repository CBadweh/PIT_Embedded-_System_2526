/*
 * 012i2c_master_rx_testingIT_hardcoded.c
 *
 * I2C Master RX with Interrupts - FULLY HARDCODED VERSION
 * All abstraction layers removed to show direct register access
 * Comments map code to I2C waveform signals (START, ADDRESS, ACK/NACK, DATA, STOP)
 *
 * Hardware: STM32F407VG Discovery Board
 * I2C1: PB6 (SCL), PB7 (SDA)
 * Button: PA0
 * LED: PD12
 * Slave Address: 0x68
 */

#include<stdio.h>
#include<string.h>
#include<stdint.h>

extern void initialise_monitor_handles();

/* ============================================================================
 * MEMORY-MAPPED REGISTER ADDRESSES
 * ============================================================================ */

// Base Addresses
#define RCC_BASE                0x40023800
#define GPIOA_BASE              0x40020000
#define GPIOB_BASE              0x40020400
#define GPIOD_BASE              0x40020C00
#define I2C1_BASE               0x40005400
#define NVIC_ISER0_BASE         0xE000E100

// RCC Registers
#define RCC_AHB1ENR             (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_APB1ENR             (*(volatile uint32_t *)(RCC_BASE + 0x40))

// GPIOA Registers (Button on PA0)
#define GPIOA_MODER             (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_OSPEEDR           (*(volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_PUPDR             (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))
#define GPIOA_IDR               (*(volatile uint32_t *)(GPIOA_BASE + 0x10))

// GPIOB Registers (I2C1 on PB6, PB7)
#define GPIOB_MODER             (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_OTYPER            (*(volatile uint32_t *)(GPIOB_BASE + 0x04))
#define GPIOB_OSPEEDR           (*(volatile uint32_t *)(GPIOB_BASE + 0x08))
#define GPIOB_PUPDR             (*(volatile uint32_t *)(GPIOB_BASE + 0x0C))
#define GPIOB_AFRL              (*(volatile uint32_t *)(GPIOB_BASE + 0x20))

// GPIOD Registers (LED on PD12)
#define GPIOD_MODER             (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_OTYPER            (*(volatile uint32_t *)(GPIOD_BASE + 0x04))
#define GPIOD_OSPEEDR           (*(volatile uint32_t *)(GPIOD_BASE + 0x08))
#define GPIOD_PUPDR             (*(volatile uint32_t *)(GPIOD_BASE + 0x0C))

// I2C1 Registers
#define I2C1_CR1                (*(volatile uint32_t *)(I2C1_BASE + 0x00))
#define I2C1_CR2                (*(volatile uint32_t *)(I2C1_BASE + 0x04))
#define I2C1_OAR1               (*(volatile uint32_t *)(I2C1_BASE + 0x08))
#define I2C1_DR                 (*(volatile uint32_t *)(I2C1_BASE + 0x10))
#define I2C1_SR1                (*(volatile uint32_t *)(I2C1_BASE + 0x14))
#define I2C1_SR2                (*(volatile uint32_t *)(I2C1_BASE + 0x18))
#define I2C1_CCR                (*(volatile uint32_t *)(I2C1_BASE + 0x1C))
#define I2C1_TRISE              (*(volatile uint32_t *)(I2C1_BASE + 0x20))

// NVIC Registers (Interrupt Controller)
#define NVIC_ISER0              (*(volatile uint32_t *)(NVIC_ISER0_BASE + 0x00))
#define NVIC_ISER1              (*(volatile uint32_t *)(NVIC_ISER0_BASE + 0x04))

/* ============================================================================
 * CONFIGURATION VALUES
 * ============================================================================ */

#define MY_ADDR                 0x61        // Our I2C address (master mode, not used)
#define SLAVE_ADDR              0x68        // Slave device address

// I2C States
#define I2C_READY               0
#define I2C_BUSY_IN_TX          2
#define I2C_BUSY_IN_RX          1

// Application Events
#define I2C_EV_TX_CMPLT         0
#define I2C_EV_RX_CMPLT         1
#define I2C_ERROR_AF            5

/* ============================================================================
 * GLOBAL VARIABLES (I2C Handle Structure - Manual)
 * ============================================================================ */

uint8_t *pTxBuffer_Global;                  // Pointer to TX buffer
uint8_t *pRxBuffer_Global;                  // Pointer to RX buffer
uint32_t TxLen_Global;                      // TX length
uint32_t RxLen_Global;                      // RX length
uint8_t TxRxState_Global = I2C_READY;       // Current state
uint8_t DevAddr_Global;                     // Device address
uint32_t RxSize_Global;                     // Total RX size
uint8_t Sr_Global;                          // Repeated start flag

uint8_t rxComplt = 0;                       // RX complete flag
uint8_t rcv_buf[32];                        // Receive buffer


/* ============================================================================
 * DELAY FUNCTION
 * ============================================================================ */
void delay(void)
{
    for(uint32_t i = 0 ; i < 500000/2 ; i++);
}


/* ============================================================================
 * GPIO INITIALIZATION
 * ============================================================================ */
void GPIO_Init_Hardcoded(void)
{
    /* ----------------- BUTTON INIT (PA0) ----------------- */
    
    // Enable GPIOA clock (RCC_AHB1ENR bit 0)
    RCC_AHB1ENR |= (1 << 0);
    
    // Configure PA0 as input (MODER[1:0] = 00)
    GPIOA_MODER &= ~(3 << 0);
    
    // Set speed to high (OSPEEDR[1:0] = 10)
    GPIOA_OSPEEDR |= (2 << 0);
    
    // No pull-up/pull-down (PUPDR[1:0] = 00)
    GPIOA_PUPDR &= ~(3 << 0);
    
    
    /* ----------------- LED INIT (PD12) ----------------- */
    
    // Enable GPIOD clock (RCC_AHB1ENR bit 3)
    RCC_AHB1ENR |= (1 << 3);
    
    // Configure PD12 as output (MODER[25:24] = 01)
    GPIOD_MODER &= ~(3 << 24);
    GPIOD_MODER |= (1 << 24);
    
    // Set output type to open-drain (OTYPER bit 12 = 1)
    GPIOD_OTYPER |= (1 << 12);
    
    // Set speed to high (OSPEEDR[25:24] = 10)
    GPIOD_OSPEEDR |= (2 << 24);
    
    // No pull-up/pull-down (PUPDR[25:24] = 00)
    GPIOD_PUPDR &= ~(3 << 24);
    
    
    /* ----------------- I2C1 GPIO INIT (PB6=SCL, PB7=SDA) ----------------- */
    
    // Enable GPIOB clock (RCC_AHB1ENR bit 1)
    RCC_AHB1ENR |= (1 << 1);
    
    // Configure PB6 as alternate function (MODER[13:12] = 10)
    GPIOB_MODER &= ~(3 << 12);
    GPIOB_MODER |= (2 << 12);
    
    // Configure PB7 as alternate function (MODER[15:14] = 10)
    GPIOB_MODER &= ~(3 << 14);
    GPIOB_MODER |= (2 << 14);
    
    // Set both to open-drain (OTYPER bits 6 and 7 = 1)
    GPIOB_OTYPER |= (1 << 6);
    GPIOB_OTYPER |= (1 << 7);
    
    // Set speed to high (OSPEEDR[13:12] = 10, OSPEEDR[15:14] = 10)
    GPIOB_OSPEEDR |= (2 << 12);
    GPIOB_OSPEEDR |= (2 << 14);
    
    // Enable pull-up resistors (PUPDR[13:12] = 01, PUPDR[15:14] = 01)
    GPIOB_PUPDR &= ~(3 << 12);
    GPIOB_PUPDR |= (1 << 12);
    GPIOB_PUPDR &= ~(3 << 14);
    GPIOB_PUPDR |= (1 << 14);
    
    // Set alternate function 4 for PB6 (AFRL[27:24] = 0100)
    GPIOB_AFRL &= ~(0xF << 24);
    GPIOB_AFRL |= (4 << 24);
    
    // Set alternate function 4 for PB7 (AFRL[31:28] = 0100)
    GPIOB_AFRL &= ~(0xF << 28);
    GPIOB_AFRL |= (4 << 28);
}


/* ============================================================================
 * I2C1 INITIALIZATION
 * ============================================================================ */
void I2C1_Init_Hardcoded(void)
{
    uint32_t tempreg;
    
    // Enable I2C1 clock (RCC_APB1ENR bit 21)
    RCC_APB1ENR |= (1 << 21);
    
    /* Configure CR1 Register */
    // Enable ACK bit (bit 10), keep peripheral disabled for now
    tempreg = (1 << 10);  // ACK enabled
    I2C1_CR1 = tempreg;
    
    /* Configure CR2 Register */
    // Set FREQ field to APB1 clock frequency in MHz
    // Assuming 16 MHz APB1 clock
    tempreg = 16;  // 16 MHz
    I2C1_CR2 = (tempreg & 0x3F);
    
    /* Configure OAR1 Register */
    // Set our own address (not really used in master mode)
    tempreg = (MY_ADDR << 1);
    tempreg |= (1 << 14);  // Bit 14 should be kept at 1 by software
    I2C1_OAR1 = tempreg;
    
    /* Configure CCR Register for Standard Mode (100 kHz) */
    // CCR = FPCLK / (2 * FSCL)
    // CCR = 16000000 / (2 * 100000) = 80
    tempreg = 80;
    I2C1_CCR = (tempreg & 0xFFF);
    
    /* Configure TRISE Register */
    // For standard mode: TRISE = (FPCLK / 1000000) + 1
    // TRISE = 16 + 1 = 17
    tempreg = 17;
    I2C1_TRISE = (tempreg & 0x3F);
}


/* ============================================================================
 * NVIC (INTERRUPT) INITIALIZATION
 * ============================================================================ */
void I2C1_IRQ_Init_Hardcoded(void)
{
    // IRQ_NO_I2C1_EV = 31 (Event interrupt)
    // IRQ_NO_I2C1_ER = 32 (Error interrupt)
    
    // Enable I2C1_EV interrupt (IRQ 31) in NVIC_ISER0
    NVIC_ISER0 |= (1 << 31);
    
    // Enable I2C1_ER interrupt (IRQ 32) in NVIC_ISER1 (bit 0 = IRQ 32)
    NVIC_ISER1 |= (1 << 0);
}


/* ============================================================================
 * I2C MASTER SEND DATA (INTERRUPT MODE)
 * ============================================================================ */
uint8_t I2C_MasterSendDataIT_Hardcoded(uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
    uint8_t busystate = TxRxState_Global;
    
    if((busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
    {
        // Save parameters to global variables
        pTxBuffer_Global = pTxBuffer;
        TxLen_Global = Len;
        TxRxState_Global = I2C_BUSY_IN_TX;
        DevAddr_Global = SlaveAddr;
        Sr_Global = Sr;
        
        /* WAVEFORM: Generate START condition (S) */
        // Set START bit in CR1 (bit 8)
        // This pulls SDA LOW while SCL is HIGH
        I2C1_CR1 |= (1 << 8);
        
        // Enable interrupt control bits
        I2C1_CR2 |= (1 << 10);  // ITBUFEN: Buffer interrupt enable
        I2C1_CR2 |= (1 << 9);   // ITEVTEN: Event interrupt enable  
        I2C1_CR2 |= (1 << 8);   // ITERREN: Error interrupt enable
    }
    
    return busystate;
}


/* ============================================================================
 * I2C MASTER RECEIVE DATA (INTERRUPT MODE)
 * ============================================================================ */
uint8_t I2C_MasterReceiveDataIT_Hardcoded(uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
    uint8_t busystate = TxRxState_Global;
    
    if((busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
    {
        // Save parameters to global variables
        pRxBuffer_Global = pRxBuffer;
        RxLen_Global = Len;
        TxRxState_Global = I2C_BUSY_IN_RX;
        RxSize_Global = Len;
        DevAddr_Global = SlaveAddr;
        Sr_Global = Sr;
        
        /* WAVEFORM: Generate START condition (S) */
        // Set START bit in CR1 (bit 8)
        I2C1_CR1 |= (1 << 8);
        
        // Enable interrupt control bits
        I2C1_CR2 |= (1 << 10);  // ITBUFEN: Buffer interrupt enable
        I2C1_CR2 |= (1 << 9);   // ITEVTEN: Event interrupt enable
        I2C1_CR2 |= (1 << 8);   // ITERREN: Error interrupt enable
    }
    
    return busystate;
}


/* ============================================================================
 * I2C EVENT INTERRUPT HANDLER (CALLED BY I2C1_EV_IRQHandler)
 * ============================================================================ */
void I2C_EV_IRQHandling_Hardcoded(void)
{
    uint32_t temp1, temp2, temp3;
    uint32_t dummy_read;
    
    // Check if event and buffer interrupts are enabled
    temp1 = I2C1_CR2 & (1 << 9);   // ITEVTEN
    temp2 = I2C1_CR2 & (1 << 10);  // ITBUFEN
    
    
    /* ========== EVENT 1: START condition generated (SB flag) ========== */
    /* WAVEFORM: START bit sent, bus is now busy, SDA went LOW while SCL HIGH */
    temp3 = I2C1_SR1 & (1 << 0);   // SB flag (bit 0)
    if(temp1 && temp3)
    {
        // START condition successfully generated
        // Now send slave address with R/W bit
        
        if(TxRxState_Global == I2C_BUSY_IN_TX)
        {
            /* WAVEFORM: Send 7-bit ADDRESS + W(0) = 8 bits on SDA */
            // Shift address left and clear bit 0 (write operation)
            uint8_t addr = (DevAddr_Global << 1) & ~(1);
            I2C1_DR = addr;
            // SB flag cleared by reading SR1 (done above) and writing to DR (done here)
        }
        else if(TxRxState_Global == I2C_BUSY_IN_RX)
        {
            /* WAVEFORM: Send 7-bit ADDRESS + R(1) = 8 bits on SDA */
            // Shift address left and set bit 0 (read operation)
            uint8_t addr = (DevAddr_Global << 1) | 1;
            I2C1_DR = addr;
            // SB flag cleared by reading SR1 and writing to DR
        }
    }
    
    
    /* ========== EVENT 2: Address sent/matched (ADDR flag) ========== */
    /* WAVEFORM: Slave sent ACK after receiving address */
    temp3 = I2C1_SR1 & (1 << 1);   // ADDR flag (bit 1)
    if(temp1 && temp3)
    {
        // Address phase completed, slave acknowledged
        
        // Check if we're in master mode
        if(I2C1_SR2 & (1 << 0))  // MSL bit (master mode)
        {
            if(TxRxState_Global == I2C_BUSY_IN_RX)
            {
                if(RxSize_Global == 1)
                {
                    /* WAVEFORM: For single byte RX, send NACK after this byte */
                    // Disable ACK before clearing ADDR flag
                    I2C1_CR1 &= ~(1 << 10);
                    
                    // Clear ADDR flag by reading SR1 then SR2
                    dummy_read = I2C1_SR1;
                    dummy_read = I2C1_SR2;
                    (void)dummy_read;
                }
                else
                {
                    // Clear ADDR flag by reading SR1 then SR2
                    dummy_read = I2C1_SR1;
                    dummy_read = I2C1_SR2;
                    (void)dummy_read;
                }
            }
            else  // TX mode
            {
                // Clear ADDR flag by reading SR1 then SR2
                dummy_read = I2C1_SR1;
                dummy_read = I2C1_SR2;
                (void)dummy_read;
            }
        }
    }
    
    
    /* ========== EVENT 3: Byte Transfer Finished (BTF flag) ========== */
    /* WAVEFORM: Both shift register and data register are empty/full */
    temp3 = I2C1_SR1 & (1 << 2);   // BTF flag (bit 2)
    if(temp1 && temp3)
    {
        // BTF flag is set
        if(TxRxState_Global == I2C_BUSY_IN_TX)
        {
            // Make sure TXE is also set
            if(I2C1_SR1 & (1 << 7))  // TXE flag
            {
                // Both BTF and TXE are set
                if(TxLen_Global == 0)
                {
                    /* WAVEFORM: All data sent, generate STOP condition (P) */
                    // STOP: SDA goes HIGH while SCL is HIGH
                    if(Sr_Global == 0)  // If repeated start disabled
                    {
                        I2C1_CR1 |= (1 << 9);  // Set STOP bit
                    }
                    
                    // Disable interrupts
                    I2C1_CR2 &= ~(1 << 10);  // ITBUFEN
                    I2C1_CR2 &= ~(1 << 9);   // ITEVTEN
                    
                    // Reset state
                    TxRxState_Global = I2C_READY;
                    pTxBuffer_Global = ((void *)0);
                    TxLen_Global = 0;
                    
                    // Notify application
                    printf("Tx is completed\n");
                }
            }
        }
    }
    
    
    /* ========== EVENT 4: Transmit buffer empty (TXE flag) ========== */
    /* WAVEFORM: Ready to load next data byte to send on SDA */
    temp3 = I2C1_SR1 & (1 << 7);   // TXE flag (bit 7)
    if(temp1 && temp2 && temp3)
    {
        // Check if we're in master mode
        if(I2C1_SR2 & (1 << 0))  // MSL bit
        {
            // TXE flag is set, we can transmit data
            if(TxRxState_Global == I2C_BUSY_IN_TX)
            {
                if(TxLen_Global > 0)
                {
                    /* WAVEFORM: Write DATA byte, it will be shifted out on SDA */
                    // Load data into DR register
                    I2C1_DR = *pTxBuffer_Global;
                    
                    // Decrement length and increment pointer
                    TxLen_Global--;
                    pTxBuffer_Global++;
                }
            }
        }
    }
    
    
    /* ========== EVENT 5: Receive buffer not empty (RXNE flag) ========== */
    /* WAVEFORM: Data byte received from slave via SDA, master sent ACK */
    temp3 = I2C1_SR1 & (1 << 6);   // RXNE flag (bit 6)
    if(temp1 && temp2 && temp3)
    {
        // Check if we're in master mode
        if(I2C1_SR2 & (1 << 0))  // MSL bit
        {
            // RXNE flag is set, data available
            if(TxRxState_Global == I2C_BUSY_IN_RX)
            {
                // Handle reception based on size
                if(RxSize_Global == 1)
                {
                    /* WAVEFORM: Read last byte, NACK was already sent */
                    *pRxBuffer_Global = I2C1_DR;
                    RxLen_Global--;
                }
                else if(RxSize_Global > 1)
                {
                    if(RxLen_Global == 2)
                    {
                        /* WAVEFORM: Second-to-last byte, disable ACK for last byte */
                        // Clear ACK bit to send NACK on last byte
                        I2C1_CR1 &= ~(1 << 10);
                    }
                    
                    /* WAVEFORM: Read DATA byte from SDA (received and ACKed) */
                    *pRxBuffer_Global = I2C1_DR;
                    pRxBuffer_Global++;
                    RxLen_Global--;
                }
                
                // Check if reception complete
                if(RxLen_Global == 0)
                {
                    /* WAVEFORM: All bytes received, generate STOP condition (P) */
                    // Generate STOP condition if repeated start disabled
                    if(Sr_Global == 0)  // DISABLE_SR
                    {
                        I2C1_CR1 |= (1 << 9);  // Set STOP bit
                    }
                    
                    // Disable interrupts
                    I2C1_CR2 &= ~(1 << 10);  // ITBUFEN
                    I2C1_CR2 &= ~(1 << 9);   // ITEVTEN
                    
                    // Reset state
                    TxRxState_Global = I2C_READY;
                    pRxBuffer_Global = ((void *)0);
                    RxLen_Global = 0;
                    RxSize_Global = 0;
                    
                    // Re-enable ACK for next transaction
                    I2C1_CR1 |= (1 << 10);
                    
                    // Notify application
                    printf("Rx is completed\n");
                    rxComplt = 1;  // Set flag
                }
            }
        }
    }
}


/* ============================================================================
 * I2C ERROR INTERRUPT HANDLER (CALLED BY I2C1_ER_IRQHandler)
 * ============================================================================ */
void I2C_ER_IRQHandling_Hardcoded(void)
{
    uint32_t temp1, temp2;
    
    // Check if error interrupt is enabled
    temp2 = I2C1_CR2 & (1 << 8);  // ITERREN
    
    /* Check for ACK failure error (slave did not ACK) */
    /* WAVEFORM: No ACK pulse received from slave after data/address */
    temp1 = I2C1_SR1 & (1 << 10);  // AF flag (bit 10)
    if(temp1 && temp2)
    {
        // ACK failure error occurred
        printf("Error : Ack failure\n");
        
        // Clear AF flag by writing 0
        I2C1_SR1 &= ~(1 << 10);
        
        // Close send operation
        I2C1_CR2 &= ~(1 << 10);  // ITBUFEN
        I2C1_CR2 &= ~(1 << 9);   // ITEVTEN
        TxRxState_Global = I2C_READY;
        pTxBuffer_Global = ((void *)0);
        TxLen_Global = 0;
        
        /* WAVEFORM: Generate STOP condition to release bus */
        I2C1_CR1 |= (1 << 9);
        
        // Hang in infinite loop
        while(1);
    }
    
    /* Check for Bus error */
    temp1 = I2C1_SR1 & (1 << 8);  // BERR flag (bit 8)
    if(temp1 && temp2)
    {
        // Clear BERR flag
        I2C1_SR1 &= ~(1 << 8);
        printf("Error: Bus error\n");
    }
    
    /* Check for Arbitration lost error */
    temp1 = I2C1_SR1 & (1 << 9);  // ARLO flag (bit 9)
    if(temp1 && temp2)
    {
        // Clear ARLO flag
        I2C1_SR1 &= ~(1 << 9);
        printf("Error: Arbitration lost\n");
    }
    
    /* Check for Overrun error */
    temp1 = I2C1_SR1 & (1 << 11);  // OVR flag (bit 11)
    if(temp1 && temp2)
    {
        // Clear OVR flag
        I2C1_SR1 &= ~(1 << 11);
        printf("Error: Overrun\n");
    }
}


/* ============================================================================
 * INTERRUPT HANDLERS (CALLED BY HARDWARE)
 * ============================================================================ */
void I2C1_EV_IRQHandler(void)
{
    I2C_EV_IRQHandling_Hardcoded();
}

void I2C1_ER_IRQHandler(void)
{
    I2C_ER_IRQHandling_Hardcoded();
}


/* ============================================================================
 * MAIN APPLICATION
 * ============================================================================ */
int main(void)
{
    uint8_t commandcode;
    uint8_t len;
    
    initialise_monitor_handles();
    printf("Application is running\n");
    
    // Initialize GPIO (Button, LED, I2C pins)
    GPIO_Init_Hardcoded();
    
    // Initialize I2C1 peripheral
    I2C1_Init_Hardcoded();
    
    // Configure I2C1 interrupts in NVIC
    I2C1_IRQ_Init_Hardcoded();
    
    /* WAVEFORM: Enable I2C peripheral (allows START generation) */
    // Enable I2C1 by setting PE bit (bit 0) in CR1
    I2C1_CR1 |= (1 << 0);
    
    /* WAVEFORM: Enable ACK generation after PE=1 */
    // Enable ACK bit (bit 10) in CR1
    // Master will send ACK after receiving data bytes
    I2C1_CR1 |= (1 << 10);
    
    while(1)
    {
        /* Wait for button press on PA0 */
        while(!(GPIOA_IDR & (1 << 0)));
        
        // Debounce delay
        delay();
        
        /* ADD I2C RESET HERE ↓ */
        // Reset I2C peripheral to clear any stuck state
        I2C1_CR1 &= ~(1 << 0);  // Disable I2C
        delay();
        I2C1_CR1 |= (1 << 0);   // Re-enable I2C
        I2C1_CR1 |= (1 << 10);  // Re-enable ACK

        /* ====================================================================
         * TRANSACTION 1: Send command 0x51 to get data length
         * ==================================================================== */
        commandcode = 0x51;
        
        /* WAVEFORM SEQUENCE:
         * S - START condition (SDA falls while SCL high)
         * ADDR+W - Send slave address 0x68 with Write bit (0)
         * ACK - Slave acknowledges (SDA low during 9th clock)
         * DATA - Send 0x51 command byte
         * ACK - Slave acknowledges
         * P - STOP condition (SDA rises while SCL high)
         */
        while(I2C_MasterSendDataIT_Hardcoded(&commandcode, 1, SLAVE_ADDR, 1) != I2C_READY);
        
        
        /* ====================================================================
         * TRANSACTION 2: Receive 1 byte (length) from slave
         * ==================================================================== */
        
        /* WAVEFORM SEQUENCE:
         * S - START condition (or Sr - repeated START)
         * ADDR+R - Send slave address 0x68 with Read bit (1)
         * ACK - Slave acknowledges
         * DATA - Master receives 1 byte (length)
         * NACK - Master sends NACK (no more data needed)
         * P - STOP condition (SDA rises while SCL high)
         */
        while(I2C_MasterReceiveDataIT_Hardcoded(&len, 1, SLAVE_ADDR, 1) != I2C_READY);
        
        
        /* ====================================================================
         * TRANSACTION 3: Send command 0x52 to request data
         * ==================================================================== */
        commandcode = 0x52;
        
        /* WAVEFORM SEQUENCE:
         * S - START condition
         * ADDR+W - Send slave address 0x68 with Write bit
         * ACK - Slave acknowledges
         * DATA - Send 0x52 command byte
         * ACK - Slave acknowledges
         * P - STOP condition
         */
        while(I2C_MasterSendDataIT_Hardcoded(&commandcode, 1, SLAVE_ADDR, 1) != I2C_READY);
        
        
        /* ====================================================================
         * TRANSACTION 4: Receive multiple bytes from slave
         * ==================================================================== */
        
        /* WAVEFORM SEQUENCE:
         * S - START condition
         * ADDR+R - Send slave address 0x68 with Read bit
         * ACK - Slave acknowledges
         * DATA1 - Master receives byte 1
         * ACK - Master sends ACK (more data expected)
         * DATA2 - Master receives byte 2
         * ACK - Master sends ACK
         * ...
         * DATAn-1 - Master receives second-to-last byte
         * ACK - Master sends ACK
         * DATAn - Master receives last byte
         * NACK - Master sends NACK (no more data)
         * P - STOP condition
         */
        while(I2C_MasterReceiveDataIT_Hardcoded(rcv_buf, len, SLAVE_ADDR, 0) != I2C_READY);
        
        rxComplt = 0;
        
        // Wait for RX completion (set in interrupt handler)
        while(rxComplt != 1)
        {
            // Waiting...
        }
        
        // Null-terminate received string
        rcv_buf[len] = '\0';
        
        // Print received data
        printf("Data : %s\n", rcv_buf);
        
        rxComplt = 0;
    }
}


/* ============================================================================
 * I2C WAVEFORM SUMMARY
 * ============================================================================
 *
 * Typical I2C Master TX Waveform:
 * 
 *      S  ADDR+W  ACK  DATA   ACK   P
 * SDA: \_/‾‾‾‾‾‾\_/‾‾\/‾‾‾‾‾‾\_/‾‾\_/‾
 * SCL: ‾‾\_/‾\_/‾\_/‾\_/‾\_/‾\_/‾‾‾‾
 *
 * Typical I2C Master RX Waveform:
 * 
 *      S  ADDR+R  ACK  DATA   ACK  DATA  NACK  P
 * SDA: \_/‾‾‾‾‾‾\_/‾‾\/‾‾‾‾‾‾\_/‾\/‾‾‾‾‾\‾‾‾\_/‾
 * SCL: ‾‾\_/‾\_/‾\_/‾\_/‾\_/‾\_/‾\_/‾\_/‾‾‾‾
 *
 * Signal Descriptions:
 * - S (START): SDA falls while SCL is HIGH
 * - P (STOP): SDA rises while SCL is HIGH
 * - ADDR: 7-bit slave address
 * - W/R: Write (0) or Read (1) bit
 * - ACK: SDA held LOW by receiver during 9th clock pulse
 * - NACK: SDA held HIGH by receiver during 9th clock pulse
 * - DATA: 8 bits of data, MSB first
 *
 * ============================================================================ */

