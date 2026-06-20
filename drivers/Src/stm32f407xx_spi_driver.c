/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: Mar 10, 2025
 *      Author: danjj
 */
#include "stm32f407xx_spi_driver.h"

void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t En_or_Di) {
	if (En_or_Di == ENABLE) {

		if(pSPIx == SPI1){
			SPI1_PCLK_EN();
		} else if (pSPIx == SPI2){
			SPI2_PCLK_EN();
		} else if (pSPIx == SPI3){
			SPI3_PCLK_EN();
		}

	}else {

		if(pSPIx == SPI1){
			SPI1_PCLK_DI();
		} else if (pSPIx == SPI2){
			SPI2_PCLK_DI();
		} else if (pSPIx == SPI3){
			SPI3_PCLK_DI();
		}
	}
}

void SPI_Init(SPI_Handle_t *pSPIHandle) {

	// Enable the clock here, so that the user does not have to explicitly enable in the application
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	// First, configure the SPI_CR1 Register

	uint32_t temp_reg = 0;

	// 1. Configure the device mode

	temp_reg |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

	// 2. Configure the bus config

	if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD) {
		// Bi-Directional mode should be cleared
		temp_reg &= ~(1 << SPI_CR1_BIDIMODE);
	} else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD){
		// Bi-Directional mode should be set
		temp_reg |= (1 << SPI_CR1_BIDIMODE);
	} else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY){
		// Bi-Directional mode should be cleared & RXONLY bit must be set
		temp_reg &= ~(1 << SPI_CR1_BIDIMODE);
		temp_reg |= (1 << SPI_CR1_RXONLY);
	}

	// 3. Configure the SPI Serial clock speed (Baud Rate)

	temp_reg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

	// 4. Configure the DFF

	temp_reg |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF;

	// 5. Configure the CPOL

	temp_reg |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

	// 6. Configure the CPHA

	temp_reg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;


	// 7. Configure the SSM
	temp_reg |= pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM;

	pSPIHandle->pSPIx->CR1 |= temp_reg;

}

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t flag_name){

	if (pSPIx->SR & (flag_name)) {
		return FLAG_SET;
	}
	return FLAG_RESET;
}

void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t En_or_Di) {

	if (En_or_Di == ENABLE) {
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	} else {
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}

void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t En_or_Di) {

	if (En_or_Di == ENABLE) {
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	} else {
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	}
}

void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t En_or_Di) {
	if (En_or_Di == ENABLE) {
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	} else {
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	}
}

// This is a blocking call !
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len) {

	while(len > 0) {

		// 1. Wait until TXE: Transmit buffer empty ( so wait till the bit is set)

		while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG)== FLAG_RESET);

		// 2. Check the DFF bit in CR1 to know if 8-bit or 16-bit transfer

		if (pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
			// 16-bit DFF

			// 1. load the data into the DR

			pSPIx->DR = *((uint16_t *)pTxBuffer);
			len--;
			len--;
			(uint16_t *)pTxBuffer++;

		} else {
			// 8-bit DFF
			pSPIx->DR = *pTxBuffer;
			len--;
			pTxBuffer++;
		}

	}
}

void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t len) {

	while(len > 0) {

		// 1. Wait until RXNE is set

		while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG)== FLAG_RESET);

		// 2. Check the DFF bit in CR1 to know if 8-bit or 16-bit transfer

		if (pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
			// 16-bit DFF

			// 1. load the data from DR to receive buffer


			*((uint16_t *)pRxBuffer) = pSPIx->DR;
			len--;
			len--;
			(uint16_t *)pRxBuffer++;

		} else {
			// 8-bit DFF
			*pRxBuffer = pSPIx->DR;
			len--;
			pRxBuffer++;
		}

	}


}
