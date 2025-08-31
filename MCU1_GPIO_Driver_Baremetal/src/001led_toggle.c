/*
 * 001led_toggle.c
 *	Corresponding to FastBit MCU Lesson 102: Exercise LED Toggling with Push Pull Configuration
 *	Pin: GPIOD12
 */

#include "stm32f407xx.h"


void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}


int main(void)
{

	GPIO_Handle_t GpioLed;

	GpioLed.pGPIOx = GPIOD;  										// ((GPIO_RegDef_t*)(AHB1PERIPH_BASEADDR + 0x0C00)) // AHB1PERIPH_BASEADDR 0x40020000U
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12; 	// 12
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;		// 1
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;		// 2
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;	// 0
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;	// 0

	GPIO_PeriClockControl(GPIOD,ENABLE);

	GPIO_Init(&GpioLed);

	while(1)
	{
		GPIO_ToggleOutputPin(GPIOD,GPIO_PIN_NO_12);
		delay();
	}
	return 0;
}




/*
#include <stdint.h>

#define PERIPH_BASE     0x40000000U
#define AHB1PERIPH_BASE (PERIPH_BASE + 0x20000U)
#define GPIOD_BASE      (AHB1PERIPH_BASE + 0x0C00U)
#define RCC_BASE        (AHB1PERIPH_BASE + 0x3800U)

#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define GPIOD_MODER     (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_OTYPER    (*(volatile uint32_t *)(GPIOD_BASE + 0x04))
#define GPIOD_ODR       (*(volatile uint32_t *)(GPIOD_BASE + 0x14))

// Simple delay loop
static void delay(void) {
    for (volatile uint32_t i = 0; i < 500000; i++);
}

int main(void) {
    // 1. Enable GPIOD clock (bit 3 in AHB1ENR)
    RCC_AHB1ENR |= (1 << 3);

    // 2. Set PD12 as output (MODER12 = 01)
    GPIOD_MODER &= ~(3U << (12 * 2));  // Clear bits
    GPIOD_MODER |=  (1U << (12 * 2));  // Set 01

    // 3. Set PD12 as push-pull (OTYPER12 = 0)
    GPIOD_OTYPER &= ~(1U << 12);

    // 4. Toggle PD12 in an infinite loop
    while (1) {
        GPIOD_ODR ^= (1 << 12);  // Toggle bit 12
        delay();
    }
}
*/

