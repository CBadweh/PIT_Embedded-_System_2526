/*
 * 001led_toggle.c
 *
 *  Created on: Feb 1, 2019
 *      Author: admin
 */

#include "stm32f407xx.h"


void delay(void)
{
	for(uint32_t i = 0 ; i < 500000 ; i ++);
}


int main(void)
{

	GPIO_Handle_t GpioLed;

	GpioLed.pGPIOx = GPIOD;  				// ((GPIO_RegDef_t*)(AHB1PERIPH_BASEADDR + 0x0C00)) // AHB1PERIPH_BASEADDR 0x40020000U
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
// htllo

