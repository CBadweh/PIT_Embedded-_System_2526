/*
 * stm32f407xx_gpio_driver.h
 *
 */

#ifndef INC_STM32F407XX_GPIO_DRIVER_H_
#define INC_STM32F407XX_GPIO_DRIVER_H_

#include "stm32f407xx.h"

typedef struct{
	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode;			              
	uint8_t GPIO_PinSpeed;			            
	uint8_t GPIO_PinPuPdControl;
	uint8_t GPIO_PinOPType;
	uint8_t GPIO_PinAltFunMode;
}GPIO_PinConfig_t;

typedef struct{
	GPIO_RegDef_t *pGPIOx;       		       
	GPIO_PinConfig_t GPIO_PinConfig;           
}GPIO_Handle_t;

// @GPIO_PIN_NUMBERS
#define GPIO_PIN_NO_0  		0
#define GPIO_PIN_NO_8  		8 
#define GPIO_PIN_NO_12  	12 

// @GPIO_PIN_MODES
#define GPIO_MODE_IN 		0 
#define GPIO_MODE_OUT 		1 

// GPIO pin possible output types
#define GPIO_OP_TYPE_PP   	0 
#define GPIO_OP_TYPE_OD   	1

// @GPIO_PIN_SPEED
#define GPIO_SPEED_FAST		2

// GPIO pin pull up AND pull down configuration macros
#define GPIO_PIN_PU			1

// Peripheral Clock setup
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi);

// Init and De-init
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);

// Data read and write
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);

#endif /* INC_STM32F407XX_GPIO_DRIVER_H_ */
