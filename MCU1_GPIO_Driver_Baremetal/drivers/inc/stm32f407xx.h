/*
 * stm3f407xx.h
 *
 *  Created on: Jan 29, 2019
 *      Author: admin 
 */

#ifndef INC_STM3F407XX_H_
#define INC_STM3F407XX_H_

//#include<stddef.h>
#include<stdint.h>

#define __vo volatile
#define __weak __attribute__((weak))


/*
 * base addresses of Flash and SRAM memories
 */

#define FLASH_BASEADDR						0x08000000U   		/*!<explain this macro briefly here  */
#define SRAM1_BASEADDR						0x20000000U  		/*!<explain this macro briefly here  */
#define SRAM2_BASEADDR						0x2001C000U 		/*!<explain this macro briefly here  */
#define ROM_BASEADDR						0x1FFF0000U
#define SRAM 								SRAM1_BASEADDR


/*
 * AHBx and APBx Bus Peripheral base addresses
 */

#define PERIPH_BASEADDR 						0x40000000U
#define APB1PERIPH_BASEADDR					PERIPH_BASEADDR
#define APB2PERIPH_BASEADDR					0x40010000U
#define AHB1PERIPH_BASEADDR					0x40020000U
#define AHB2PERIPH_BASEADDR					0x50000000U

/*
 * Base addresses of peripherals which are hanging on AHB1 bus
 * TODO : Complete for all other peripherals
 */

#define GPIOA_BASEADDR           (AHB1PERIPH_BASEADDR + 0x0000)
#define GPIOB_BASEADDR           (AHB1PERIPH_BASEADDR + 0x0400)
#define GPIOC_BASEADDR 					 (AHB1PERIPH_BASEADDR + 0x0800)
#define GPIOD_BASEADDR 					 (AHB1PERIPH_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR 					 (AHB1PERIPH_BASEADDR + 0x1000)
#define GPIOF_BASEADDR 					 (AHB1PERIPH_BASEADDR + 0x1400)
#define GPIOG_BASEADDR 					 (AHB1PERIPH_BASEADDR + 0x1800)
#define GPIOH_BASEADDR 					 (AHB1PERIPH_BASEADDR + 0x1C00)
#define GPIOI_BASEADDR 					 (AHB1PERIPH_BASEADDR + 0x2000)
#define RCC_BASEADDR             (AHB1PERIPH_BASEADDR + 0x3800)



/**********************************peripheral register definition structures **********************************/

typedef struct{
	__vo uint32_t MODER;    /*!< 0x00      */
	__vo uint32_t OTYPER;   /*!< 0x04      */
	__vo uint32_t OSPEEDR;  /*!< 0x08      */
	__vo uint32_t PUPDR;    /*!< 0x0C      */
	__vo uint32_t IDR;      /*!< 0x10      */
	__vo uint32_t ODR;      /*!< 0x14      */
	__vo uint32_t BSRR;     /*!< 0x18      */
	__vo uint32_t LCKR;     /*!< 0x1C      */
	__vo uint32_t AFR[2];	  /*!< 0x20-0x24 */
}GPIO_RegDef_t;

typedef struct{
  __vo uint32_t CR;            /*!< 0x00 */
  __vo uint32_t PLLCFGR;       /*!< 0x04 */
  __vo uint32_t CFGR;          /*!< 0x08 */
  __vo uint32_t CIR;           /*!< 0x0C */
  __vo uint32_t AHB1RSTR;      /*!< 0x10 */
  __vo uint32_t AHB2RSTR;      /*!< 0x14 */
  __vo uint32_t AHB3RSTR;      /*!< 0x18 */
  uint32_t      RESERVED0;     /*!< Reserved, 0x1C  */
  __vo uint32_t APB1RSTR;      /*!< 0x20 */
  __vo uint32_t APB2RSTR;      /*!< 0x24 */
  uint32_t      RESERVED1[2];  /*!< Reserved, 0x28-0x2C */
  __vo uint32_t AHB1ENR;       /*!< 0x30 */
  __vo uint32_t AHB2ENR;       /*!< 0x34 */
  __vo uint32_t AHB3ENR;       /*!< 0x38 */
  uint32_t      RESERVED2;     /*!< Reserved, 0x3C */
  __vo uint32_t APB1ENR;       /*!< 0x40 */
  __vo uint32_t APB2ENR;       /*!< 0x44 */
  uint32_t      RESERVED3[2];  /*!< Reserved, 0x48-0x4C */
  __vo uint32_t AHB1LPENR;     /*!< 0x50 */
  __vo uint32_t AHB2LPENR;     /*!< 0x54 */
  __vo uint32_t AHB3LPENR;     /*!< 0x58 */
  uint32_t      RESERVED4;     /*!< Reserved, 0x5C */
  __vo uint32_t APB1LPENR;     /*!< 0x60 */
  __vo uint32_t APB2LPENR;     /*!< 0x64 */
  uint32_t      RESERVED5[2];  /*!< Reserved, 0x68-0x6C */
  __vo uint32_t BDCR;          /*!< 0x70 */
  __vo uint32_t CSR;           /*!< 0x74 */
  uint32_t      RESERVED6[2];  /*!< Reserved, 0x78-0x7C */
  __vo uint32_t SSCGR;         /*!< 0x80 */
  __vo uint32_t PLLI2SCFGR;    /*!< 0x84 */
  __vo uint32_t PLLSAICFGR;    /*!< 0x88 */
  __vo uint32_t DCKCFGR;       /*!< 0x8C */
  __vo uint32_t CKGATENR;      /*!< 0x90 */
  __vo uint32_t DCKCFGR2;      /*!< 0x94 */
} RCC_RegDef_t;

// peripheral definitions ( Peripheral base addresses typecasted to xxx_RegDef_t)

#define GPIOA  				((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB  				((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOD  				((GPIO_RegDef_t*)GPIOD_BASEADDR)

#define RCC 				  ((RCC_RegDef_t*)RCC_BASEADDR)

// Clock Enable Macros for GPIOx peripherals

#define GPIOA_PCLK_EN()   (RCC->AHB1ENR |= (1 << 0)) //
#define GPIOB_PCLK_EN()		(RCC->AHB1ENR |= (1 << 1)) //
#define GPIOD_PCLK_EN()		(RCC->AHB1ENR |= (1 << 3)) //

// Clock Disable Macros for GPIOx peripherals
#define GPIOA_PCLK_DI()

// Macros to reset GPIOx peripherals
#define GPIOA_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); }while(0)
#define GPIOB_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 1)); (RCC->AHB1RSTR &= ~(1 << 1)); }while(0)
#define GPIOD_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 3)); (RCC->AHB1RSTR &= ~(1 << 3)); }while(0)

//some generic macros

#define ENABLE 				1
#define DISABLE 			0
#define SET 				ENABLE
#define RESET 				DISABLE
#define GPIO_PIN_SET        SET
#define GPIO_PIN_RESET      RESET
#define FLAG_RESET         RESET
#define FLAG_SET 			SET

#include "stm32f407xx_gpio_driver.h"


#endif /* INC_STM3F407XX_H_ */
