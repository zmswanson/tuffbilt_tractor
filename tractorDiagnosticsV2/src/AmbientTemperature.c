/*
 * AmbientTemperature.c
 *
 *  Created on: Feb 14, 2018
 *      Author: Zach
 */

#include <AmbientTemperature.h>
#include "stdint.h"
#include "stm32f10x.h"
#include "stm32f10x_i2c.h"

volatile uint16_t rawTemperature = 0;
volatile uint16_t tempKelv = 0;
volatile uint16_t tempCels = 0;
volatile uint16_t tempFahr = 0;

volatile uint8_t tempPEC = 0;

const uint8_t addrList[] = {0x3A, 0x4A, 0x5A, 0x6A};

uint16_t calculateTemperature(char temperatureUnit)
{
	tempKelv = rawTemperature/50;
	tempCels = tempKelv - 273;
	tempFahr = ((tempCels * 9)/5) + 32;

	switch(temperatureUnit)
	{
	case 'C':
	case 'c':
		return tempCels;
		break;

	case 'K':
	case 'k':
		return tempKelv;
		break;

	case 'F':
	case 'f':
		return tempFahr;
		break;

	default:
		return 0;
		break;
	}
}

void storeTemperatureData()
{
	I2C1->CR2 &= (uint32_t)(~I2C_CR2_ITBUFEN &
							~I2C_CR2_ITERREN &
							~I2C_CR2_ITEVTEN);

	rawTemperature = (uint16_t)(i2c_rxBuff[i2c_rxTail++]);
	i2c_rxTail &= I2C_RX_BUFF_MASK;
	rawTemperature |= (uint16_t)(i2c_rxBuff[i2c_rxTail++] << 8);
	i2c_rxTail &= I2C_RX_BUFF_MASK;

	I2C1->CR2 |= (uint32_t)(I2C_CR2_ITBUFEN |
							I2C_CR2_ITERREN |
							I2C_CR2_ITEVTEN);
}

void requestTemperatureData(uint8_t address)
{
	uint16_t timeOut = 0;

	/* Wait for an idle I2C line */
	while(I2C1->SR2 & I2C_SR2_BUSY){
		timeOut++;
	};

	/* Generate start bit and wait */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_START);
	while(!(I2C1->SR1 & I2C_SR1_SB)){
		timeOut++;//hangs out here for 13 loops
	};
	timeOut = 0;
	while((I2C1->SR2 & (I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_BUSY | I2C_SR2_MSL)){
		timeOut++;//hangs out here for 0 loops
	};
	timeOut = 0;
	/* Transmit IR 7-bit address + Write (0) and wait */
	I2C1->DR = (uint8_t)((address << 1) | THERM_WRITE);
	while(!(I2C1->SR1 & I2C_SR1_ADDR)){
		timeOut++;//hangs out here for 217 loops
	};
	timeOut = 0;
	while((I2C1->SR2 & (I2C_SR2_TRA  | I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_TRA  | I2C_SR2_BUSY | I2C_SR2_MSL)){
		timeOut++;//hangs out here for 0 loops
	};
	timeOut = 0;
	/* Transmit command to access RAM cell 0x06 and wait */
	I2C1->DR = (uint8_t)(TA_RAM_CELL);
	while((I2C1->SR1 & (I2C_SR1_TXE | I2C_SR1_BTF)) !=
				(I2C_SR1_TXE | I2C_SR1_BTF)){
		timeOut++;//hangs out here for 215 loops
	};
	timeOut = 0;
	/* Enable acknowledge and PEC */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_ACK);

	/* Regenerate start bit and wait */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_START);
	while(!(I2C1->SR1 & I2C_SR1_SB)){
		timeOut++;//hangs out here for 38 loops
	};
	timeOut = 0;
	while((I2C1->SR2 & (I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_BUSY | I2C_SR2_MSL)){
		timeOut++;//hangs out here for 0 loops
	};
	timeOut = 0;

	/* Retransmit IR 7-bit address + read (1) and wait */
	I2C1->DR = (uint8_t)((address << 1) | THERM_READ);
	while(!(I2C1->SR1 & I2C_SR1_ADDR)){
		timeOut++;//hangs out here for 201 loops
	};
	timeOut = 0;
	while((I2C1->SR2 & (I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_BUSY | I2C_SR2_MSL)){
		timeOut++;//hangs out here for 0 loops
	};
	timeOut = 0;

	/* Wait for 3rd to last and 2nd to last byte to be received*/
	while((I2C1->SR1 & (I2C_SR1_RXNE | I2C_SR1_BTF)) !=
				(I2C_SR1_RXNE | I2C_SR1_BTF)){
		timeOut++;//hangs out here for 428 loops
	};
	timeOut = 0;
	/* Clear acknowledge bit and set PEC position to next */
	I2C1->CR1 &= (uint32_t)(~I2C_CR1_ACK);
	I2C1->CR1 |= (uint32_t)(I2C_CR1_POS);

	/* Read 3rd to last byte (Temperature Data Byte Low) */
	i2c_rxBuff[i2c_rxHead++] = (uint8_t)(I2C1->DR);
	i2c_rxHead &= I2C_RX_BUFF_MASK;

	/* Generate a stop */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_STOP);

	/* Read 2nd to last byte (Temperature Data Byte High) */
	i2c_rxBuff[i2c_rxHead++] = (uint8_t)(I2C1->DR);
	i2c_rxHead &= I2C_RX_BUFF_MASK;

	/* Wait for last byte to be received in DR */
	while(!(I2C1->SR1 & I2C_SR1_RXNE)){
		timeOut++;//hangs out here for 212 loops
	};
	timeOut = 0;

	/* Enable PEC calculation */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_ENPEC);

	/* Read last byte received (PEC) */
	tempPEC = (uint8_t)(I2C1->DR);

	/* Store the temperature data */
	storeTemperatureData();
}
