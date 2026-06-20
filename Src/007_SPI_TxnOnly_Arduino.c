/*
 * 007_SPI_TxnOnly_Arduino.c
 *
 *  Created on: Mar 27, 2025
 *      Author: danjj
 */


/*
 * From datasheet, alternate functionality mapping : AF5 (ALT function mode : 5)
 * PB15 : SPI2_MOSI pin, PB14 : SPI2_MISO pin, PB13 : SPI2_SCLK, PB12 : SPI2_NSS
 */

#include "stm32f407xx.h"
#include <string.h>

void delay(void) {

	for (uint32_t i = 0; i < 500000/2; i++);
}


void SPI2_GPIOInit(void) {

	GPIO_Handle_t SPI_Pins;

	SPI_Pins.pGPIOx = GPIOB;
	SPI_Pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPI_Pins.GPIO_PinConfig.GPIO_PinAltFunMode = 5; // AF5
	SPI_Pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPI_Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPI_Pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST; // does not matter


	// SCLK
	SPI_Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPI_Pins);

	// MOSI
	SPI_Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPI_Pins);

	// MISO
	//SPI_Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	//GPIO_Init(&SPI_Pins);

	//NSS
	SPI_Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&SPI_Pins);

}

void SPI2_Init(void) {

	SPI_Handle_t spi2_handle;

	spi2_handle.pSPIx = SPI2;
	spi2_handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	spi2_handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	spi2_handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV8; // Generates SCLK of 2MHz
	spi2_handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	spi2_handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	spi2_handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	spi2_handle.SPIConfig.SPI_SSM = SPI_SSM_DI; // Software slave management disabled for NSS pin

	SPI_Init(&spi2_handle);

}

void GPIO_ButtonInit(void) {

	GPIO_Handle_t GPIO_BTN;

	GPIO_BTN.pGPIOx = GPIOA;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_BTN.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GPIO_BTN);
}

int main(void) {

 	char user_data[] = "Hello World";

 	GPIO_ButtonInit();

	// This function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI2_GPIOInit();

	SPI2_Init();

	//NSS output is enabled with the SSOE pin of SPI_CR2 (SEE BELOW)
	SPI_SSOEConfig(SPI2, ENABLE);

	while (1) {

		while ( !GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		delay();
		// Enable the SPI2 peripheral, this is the final step before sending data. Enable bit 6 of the CR1 register of SPI2
		// With SSM = 0, making enabling the clock pulls NSS output to 0, which if connected to the SS pin of the slave,
		// makes a slave !
		SPI_PeripheralControl(SPI2, ENABLE);

		// First, send length information of bytes to send
		uint8_t data_len = strlen(user_data);
		SPI_SendData(SPI2, &data_len, 1);

		SPI_SendData(SPI2, (uint8_t *)user_data, strlen(user_data));

		// Confirm that SPI is not busy, before closing the connection
		while (SPI_GetFlagStatus(SPI2, SPI_BUSY_FLAG));

		SPI_PeripheralControl(SPI2, DISABLE);

	}

	return 0;
}
