/*
 * 002_LED_Button.c
 *
 *  Created on: Feb 18, 2025
 *      Author: danjj
 */


#include "stm32f407xx.h"


#define HIGH 		ENABLE
#define BTN_PRESSED HIGH


void delay(void) {

	for (uint32_t i = 0; i < 500000/2; i++);
}

int main(void) {

	GPIO_Handle_t GPIO_LED, GPIO_BTN;



	GPIO_LED.pGPIOx = GPIOD;
	GPIO_LED.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_LED.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIO_LED.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_LED.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIO_LED.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_Init(&GPIO_LED);

	GPIO_BTN.pGPIOx = GPIOA;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GPIO_BTN);


	while(1){

		if (GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) == BTN_PRESSED) {
			delay();
			GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
		}

	}


	return 0;
}
