/*
 * 008_SPI_CmdHandling.c
 *
 *  Created on: Mar 28, 2025
 *      Author: danjj
 */


/*
 * From datasheet, alternate functionality mapping : AF5 (ALT function mode : 5)
 * PB15 : SPI2_MOSI pin, PB14 : SPI2_MISO pin, PB13 : SPI2_SCLK, PB12 : SPI2_NSS
 */

#include "stm32f407xx.h"
#include <string.h>

// command codes

#define COMMAND_LED_CTRL		0x50
#define COMMAND_SENSOR_READ		0x51
#define COMMAND_LED_READ		0x52
#define COMMAND_PRINT			0x53
#define COMMAND_ID_READ			0x54


#define LED_ON		1
#define LED_OFF 	0

// arduino analog pins

#define ANALOG_PIN0		0
#define ANALOG_PIN1		1
#define ANALOG_PIN2		2
#define ANALOG_PIN3		3
#define ANALOG_PIN4		4

// arduino LED

#define LED_PIN			9


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
	SPI_Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&SPI_Pins);

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

uint8_t SPI_verify_response(uint8_t ack) {

	if (ack == 0xF5) {
		// positive ack
		return 1;

	}
	return 0;
}

int main(void) {


	uint8_t dummy_write = 0xff;
	uint8_t dummy_read;

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

		// (Send the first command) : CMD_LED_CTRL <pin_no(1)> <value(1)>

		uint8_t command_code = COMMAND_LED_CTRL;
		uint8_t ack_byte;

		uint8_t args[2];
		SPI_SendData(SPI2, &command_code, 1);

		// Send dummy byte (1 byte) to fetch the response (ACK) from the slave, meaning to move
		// the data out from the slave's shift register

		/*
		 * NOTE : In SPI communication, when master or slave send 1 byte, they also receive 1 byte in return
		 */

		// When this API call returns, the response from the slave would have arrived at the master, so let's
		// read next

		// So do a dummy read to clear off the RXNE for the initial send command transmission
		SPI_ReceiveData(SPI2, &dummy_read, 1);


		SPI_SendData(SPI2, &dummy_write, 1);

		SPI_ReceiveData(SPI2, &ack_byte, 1);

		if (SPI_verify_response(ack_byte)) {

			// only then, send arguments
			args[0] = LED_PIN;
			args[1] = LED_ON;
			SPI_SendData(SPI2, args, 2);
		}
		 // end of COMMAND_LED_CTRL


		// 2. CMD_SENSOR_READ <analog pin number (1)>


		while ( !GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		delay();

		command_code = COMMAND_SENSOR_READ;

		SPI_SendData(SPI2, &command_code, 1);

		SPI_ReceiveData(SPI2, &dummy_read, 1);


		SPI_SendData(SPI2, &dummy_write, 1);

		SPI_ReceiveData(SPI2, &ack_byte, 1);

		if (SPI_verify_response(ack_byte)) {


			args[0] = ANALOG_PIN0;
			SPI_SendData(SPI2, args, 1);

			// do a dummy read to clear off the RXNE
			SPI_ReceiveData(SPI2, &dummy_read, 1);

			// insert some delay to let the slave perform ADC to give us the correct value

			delay();

			// dummy write to fetch the required response of sensor read value
			SPI_SendData(SPI2, &dummy_write, 1);

			uint8_t analog_read;

			SPI_ReceiveData(SPI2, &analog_read, 1);
		}

		// Confirm that SPI is not busy, before closing the connection
		while (SPI_GetFlagStatus(SPI2, SPI_BUSY_FLAG));

		SPI_PeripheralControl(SPI2, DISABLE);

	}

	return 0;
}
