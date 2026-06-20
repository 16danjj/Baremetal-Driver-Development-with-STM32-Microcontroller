/*
 * 002_LED_Button.c
 *
 *  Created on: Feb 18, 2025
 *      Author: danjj
 */


#include "stm32f407xx.h"


#define LOW 		DISABLE
#define BTN_PRESSED LOW


void delay(void) {

	for (uint32_t i = 0; i < 500000/2; i++);
}

int main(void) {

	GPIO_Handle_t GPIO_LED, GPIO_BTN;



	GPIO_LED.pGPIOx = GPIOA;
	GPIO_LED.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GPIO_LED.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIO_LED.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_LED.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIO_LED.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GPIO_LED);

	GPIO_BTN.pGPIOx = GPIOB;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_Init(&GPIO_BTN);


	while(1){

		if (GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_12) == BTN_PRESSED) {
			delay();
			GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_8);
		}

	}


	return 0;
}
