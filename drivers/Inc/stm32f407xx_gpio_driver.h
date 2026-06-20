/*
 * stm32f407xx_gpio_driver.h
 *
 *  Created on: Feb 17, 2025
 *      Author: danjj
 */

#ifndef INC_STM32F407XX_GPIO_DRIVER_H_
#define INC_STM32F407XX_GPIO_DRIVER_H_

#include "stm32f407xx.h"


typedef struct {
	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode;		// from @GPIO_PIN_MODES (see below)
	uint8_t GPIO_PinSpeed;
	uint8_t GPIO_PinPuPdControl;
	uint8_t GPIO_PinOPType;
	uint8_t GPIO_PinAltFunMode;
} GPIO_PinConfig_t;

/*
 * This is a Handle structure
 */

typedef struct {
	GPIO_RegDef_t *pGPIOx; // Holds the base_addr of the GPIO port to which the pin belongs to
	GPIO_PinConfig_t GPIO_PinConfig; // Holds GPIO pin configuration settings
} GPIO_Handle_t;


/*
 * @GPIO_PIN_NUMBERS
 * GPIO pin numbers
 */

#define GPIO_PIN_NO_0 	0
#define GPIO_PIN_NO_1 	1
#define GPIO_PIN_NO_2 	2
#define GPIO_PIN_NO_3 	3
#define GPIO_PIN_NO_4 	4
#define GPIO_PIN_NO_5 	5
#define GPIO_PIN_NO_6 	6
#define GPIO_PIN_NO_7 	7
#define GPIO_PIN_NO_8 	8
#define GPIO_PIN_NO_9 	9
#define GPIO_PIN_NO_10 	10
#define GPIO_PIN_NO_11 	11
#define GPIO_PIN_NO_12 	12
#define GPIO_PIN_NO_13 	13
#define GPIO_PIN_NO_14 	14
#define GPIO_PIN_NO_15 	15

/*
 * @GPIO_PIN_MODES
 * GPIO pin possible modes
 */

#define GPIO_MODE_IN 		0
#define GPIO_MODE_OUT 		1
#define GPIO_MODE_ALTFN 	2
#define GPIO_MODE_ANALOG 	3
#define GPIO_MODE_IT_FT		4 	// Interrupt falling edge trigger
#define GPIO_MODE_IT_RT		5 	// Interrupt rising edge trigger
#define GPIO_MODE_IT_RFT	6   // Interrupt rising and falling edge trigger


/*
 * @GPIO_PIN_OPTYPE
 * GPIO pin possible output types
 */

#define GPIO_OP_TYPE_PP		0	// PUSH-PULL
#define GPIO_OP_TYPE_OD		1	// OPEN-DRAIN


/*
 * @GPIO_PIN_SPEED
 * GPIO pin possible output speeds
 */

#define GPIO_SPEED_LOW			0
#define GPIO_SPEED_MEDIUM		1
#define GPIO_SPEED_FAST			2
#define GPIO_SPEED_HIGH			3

/*
 * @GPIO_PIN_PUPD
 * GPIO pin pull-up/pull-down configuration
 */

#define GPIO_NO_PUPD		0
#define GPIO_PIN_PU			1
#define GPIO_PIN_PD			2



/********************************************************************
 * 				APIs supported by this driver
 */

/*
 * Peripheral Clock setup
 */

void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t En_or_Di);

/*
 * Init and De-Init
 */

void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

/*
 * Data read and write
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t pin_number);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t pin_number, uint8_t value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t pin_number);

/*
 * IRQ Handling and ISR Handling
 */
void GPIO_IRQ_InterruptConfig(uint8_t IRQ_Number, uint8_t En_or_Di);
void GPIO_IRQPriorityConfig(uint8_t IRQ_Number, uint8_t IRQ_Priority);
void GPIO_IRQHandling(uint8_t pin_number);



#endif /* INC_STM32F407XX_GPIO_DRIVER_H_ */
