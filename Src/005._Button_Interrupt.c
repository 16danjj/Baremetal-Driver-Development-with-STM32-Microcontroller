/*
 * 005_Button_Interrupt.c
 *
 *  Created on: Feb 18, 2025
 *      Author: danjj
 */


#include "stm32f407xx.h"
#include <string.h>


#define LOW 		DISABLE
#define BTN_PRESSED LOW


void delay(void) {

	for (uint32_t i = 0; i < 500000/2; i++);
}

int main(void) {

	GPIO_Handle_t GPIO_LED, GPIO_BTN;
	memset(&GPIO_LED, 0, sizeof(GPIO_LED));
	memset(&GPIO_BTN, 0, sizeof(GPIO_BTN));

	GPIO_LED.pGPIOx = GPIOD;
	GPIO_LED.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_LED.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIO_LED.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_LED.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIO_LED.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_Init(&GPIO_LED);

	GPIO_BTN.pGPIOx = GPIOD;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_Init(&GPIO_BTN);

	// IRQ Configurations
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, NVC_IRQ_PRIO15);
	GPIO_IRQ_InterruptConfig(IRQ_NO_EXTI9_5, ENABLE);

	while(1);
}


void EXTI9_5_IRQHandler(void){

	delay();
	GPIO_IRQHandling(GPIO_PIN_NO_5);
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
}
