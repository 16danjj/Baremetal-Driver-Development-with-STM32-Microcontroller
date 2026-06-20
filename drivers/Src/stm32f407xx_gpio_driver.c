/*
 * stm32f407xx_gpio.c
 *
 *  Created on: Feb 17, 2025
 *      Author: danjj
 */

#include "stm32f407xx_gpio_driver.h"


/*
 * Peripheral Clock setup
 */

/*******************************************************
 * @fn 			- GPIO_PeriClockControl
 *
 * @brief 		- This function enables or disables peripheral clock for the given
 * 				  GPIO port
 *
 * 	@param[in] 	- Base address of GPIO peripheral
 * 	@param[in]	- ENABLE/DISABLE macro
 * 	@param[in]
 *
 * 	@return 	- None
 *
 * 	@Note		- None
 *
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t En_or_Di){

	if (En_or_Di == ENABLE) {

		if(pGPIOx == GPIOA){
			GPIOA_PCLK_EN();
		} else if (pGPIOx == GPIOB){
			GPIOB_PCLK_EN();
		} else if (pGPIOx == GPIOC){
			GPIOC_PCLK_EN();
		} else if (pGPIOx == GPIOD){
			GPIOD_PCLK_EN();
		} else if (pGPIOx == GPIOE){
			GPIOE_PCLK_EN();
		} else if (pGPIOx == GPIOF){
			GPIOF_PCLK_EN();
		} else if (pGPIOx == GPIOG){
			GPIOG_PCLK_EN();
		} else if (pGPIOx == GPIOH){
			GPIOH_PCLK_EN();
		} else if (pGPIOx == GPIOI){
			GPIOI_PCLK_EN();
		}

	}else {

		if(pGPIOx == GPIOA){
			GPIOA_PCLK_DI();
		} else if (pGPIOx == GPIOB){
			GPIOB_PCLK_DI();
		} else if (pGPIOx == GPIOC){
			GPIOC_PCLK_DI();
		} else if (pGPIOx == GPIOD){
			GPIOD_PCLK_DI();
		} else if (pGPIOx == GPIOE){
			GPIOE_PCLK_DI();
		} else if (pGPIOx == GPIOF){
			GPIOF_PCLK_DI();
		} else if (pGPIOx == GPIOG){
			GPIOG_PCLK_DI();
		} else if (pGPIOx == GPIOH){
			GPIOH_PCLK_DI();
		} else if (pGPIOx == GPIOI){
			GPIOI_PCLK_DI();
		}
	}
}

/*
 * Init and De-Init
 */

void GPIO_Init(GPIO_Handle_t *pGPIOHandle){

	uint32_t temp = 0; //temporary 32-bit register

	// Edit : Enable the clock here, so that the user does not have to explicitly enable in the application
	GPIO_PeriClockControl(pGPIOHandle->pGPIOx, ENABLE);

	// first, configure the mode of GPIO pin

	// non-interrupt modes
	if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG){

		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		pGPIOHandle->pGPIOx->MODER &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); // clearing before setting
		pGPIOHandle->pGPIOx->MODER |= temp;

	} else {
		if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT){

			// Configure the Falling-Trigger selection register

			EXTI->FTSR |= (1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

			//Also, for safety clear the RTSR bit

			EXTI->RTSR &= ~(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		} else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT){

			// Configure the Rising-Trigger selection register

			EXTI->RTSR |= (1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

			EXTI->FTSR &= ~(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		} else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT) {

			// Configure both the Falling, Rising-Trigger selection register

			EXTI->RTSR |= (1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

			EXTI->FTSR |= (1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}

		// Then configure the GPIO port selection in SYSCFG_EXTICR
		uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4;
		uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;
		uint8_t port_code = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
		SYSCFG_PCLK_EN();
		SYSCFG->EXTICR[temp1] &= ~(0xF << (temp2 * 4));
		SYSCFG->EXTICR[temp1] |= port_code << (temp2 * 4);


		// Then, enable EXTI interrupt delivery using Interrupt Mask Register (IMR)

		EXTI->IMR |= (1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}

	temp = 0;

	// then, configure the speed
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;

	temp = 0;

	// then, configure the pull-up/pull-down settings

	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR |= temp;

	temp = 0;
	// then, configure the output type

	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OTYPER |= temp;


	// if needed, then configure the alt functionality

	if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN) {

		uint8_t temp1, temp2;

		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;
		pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xF << (4* temp2));
		pGPIOHandle->pGPIOx->AFR[temp1] |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4* temp2));
	}


}
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx){

	if(pGPIOx == GPIOA){
		GPIOA_REG_RESET();
	} else if (pGPIOx == GPIOB){
		GPIOB_REG_RESET();
	} else if (pGPIOx == GPIOC){
		GPIOC_REG_RESET();
	} else if (pGPIOx == GPIOD){
		GPIOD_REG_RESET();
	} else if (pGPIOx == GPIOE){
		GPIOE_REG_RESET();
	} else if (pGPIOx == GPIOF){
		GPIOF_REG_RESET();
	} else if (pGPIOx == GPIOG){
		GPIOG_REG_RESET();
	} else if (pGPIOx == GPIOH){
		GPIOH_REG_RESET();
	} else if (pGPIOx == GPIOI){
		GPIOI_REG_RESET();
	}
}

/*
 * Data read and write
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t pin_number){

	uint8_t value;
	value = (uint8_t)((pGPIOx->IDR >> pin_number) & 0x00000001);
	return value;
}
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx){

	uint16_t value;
	value = (uint16_t)pGPIOx->IDR;
	return value;
}


void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t pin_number, uint8_t value){
	if (value == GPIO_PIN_SET) {
		pGPIOx->ODR |= (1<<pin_number);
	} else {
		pGPIOx->ODR &= ~(1<<pin_number);
	}
}
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t value){
	pGPIOx->ODR = value;
}
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t pin_number){
	pGPIOx->ODR ^= (1<<pin_number);
}

/*
 * IRQ Handling and ISR Handling. Get information to work on this from the Cortex-M4 devices generic user guide?
 */
void GPIO_IRQ_InterruptConfig(uint8_t IRQ_Number, uint8_t En_or_Di){

	if(En_or_Di == ENABLE) {

		if (IRQ_Number <= 31) {

			// program ISER0 register

			*NVIC_ISER0 |= (1 << IRQ_Number);

		} else if (IRQ_Number > 31 && IRQ_Number < 64) {

			// program ISER1 register

			*NVIC_ISER1 |= (1 << (IRQ_Number % 32));

		} else if (IRQ_Number >= 64 && IRQ_Number < 96){

			// program ISER2 register

			*NVIC_ISER2 |= (1 << (IRQ_Number % 64));
		}

	} else {

		if (IRQ_Number <= 31) {

			// program ICER0 register
			*NVIC_ICER0 |= (1 << IRQ_Number);

		} else if (IRQ_Number > 31 && IRQ_Number < 64) {

			// program ICER1 register
			*NVIC_ICER1 |= (1 << (IRQ_Number % 32));

		} else if (IRQ_Number >= 64 && IRQ_Number < 96){

			// program ICER2 register
			*NVIC_ICER2 |= (1 << (IRQ_Number % 64));

		}

	}
}

void GPIO_IRQPriorityConfig(uint8_t IRQ_Number, uint8_t IRQ_Priority) {

	// First, find out the IPR register number
	uint8_t iprx = IRQ_Number / 4;

	uint8_t iprx_section = IRQ_Number % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8 - NUMBER_PRIORITY_BITS_IMPLEMENTED);

	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQ_Priority << shift_amount);

}

void GPIO_IRQHandling(uint8_t pin_number){

	// Clear the EXTI PR(Pending Register) corresponding to the pin number

	if(EXTI->PR & (1 << pin_number)){

		// clear the corresponding bit ( here, by writing '1')
		EXTI->PR |= (1<<pin_number);

	}
}
