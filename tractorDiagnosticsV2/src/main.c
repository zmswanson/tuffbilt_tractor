/**
  ******************************************************************************
  * @file    main.c
  * @author  Zach Swanson
  * @version V1.0
  * @date    02-February-2018
  * @brief   Default main function.
  ******************************************************************************
*/

#include <AmbientTemperature.h>
#include "stdint.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_i2c.h"
#include "periph_setup.h"
#include "RaspberryPi_Comms.h"
#include "NEO6_GPS.h"
#include "usbSetup.h"
#include "BMS.h"

#define NEW_ADDR	0x6A
#define ADDR_PEC	0x18

uint16_t adcVal[2];
char mess[24];
char piMsg[100];
volatile int16_t piPtr;

uint16_t mcuTemp;
uint16_t batTemp;
uint16_t motTemp;
uint16_t ambTemp;
char speed[10];
int32_t current;
uint16_t voltage;
uint32_t power;
uint8_t batPct;


void changeMLX_slvADDR( void );
void readADDRtest(uint8_t address);
void readTEMPtest(uint8_t address);

int main(void)
{
	uint16_t us10Count = 0, us100Count = 0, msCount = 0, ms10Count = 0, ms100Count = 0, sCount = 0, mCount = 0, i;
	init_clock_72MHZ();
	init_AFIO();
	init_DMA();
	init_ADC1();
	init_TIM2();
	init_UART1();
	init_UART2();
	init_I2C1();
	initBMS();

	int addrCnt = 0;
	piPtr = 0;
	//global variables for Pi communication
	mcuTemp = 0;
	batTemp = 0;
	motTemp = 0;
	ambTemp = 0;
	memset(speed, 0, 10);
	current = 0;
	voltage = 0;
	power = 0;
	batPct = 0;

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

//	readADDRtest(0);
//	changeMLX_slvADDR();
//	readADDRtest(0);
//	printString("\nCycle Power NOW you have 10 seconds\n\n");

	while(1)
	{
		if(TIM_GetFlagStatus(TIM2, TIM_FLAG_Update))
		{
			TIM_ClearFlag(TIM2, TIM_FLAG_Update);
			us10Count++;
			//Poll ADC for Current and Voltage
			getAmps(&current);
			voltage = getVolts();

		}

		if(us10Count == 10){
			us100Count++;
			us10Count = 0;
		}

		if(us100Count == 10){
			msCount++;
			us100Count = 0;
			batPct = batterySOC();
			power = getPower((uint32_t)(abs(current)), (uint32_t)(abs(voltage)));
		}

		if(msCount == 10)
		{
			ms10Count++;
			msCount = 0;
		}

		if(ms10Count == 10)
		{
			ms100Count++;
			ms10Count = 0;

			requestTemperatureData(addrList[0]);
			mcuTemp = calculateTemperature('F');

			requestTemperatureData(addrList[1]);
			batTemp = calculateTemperature('F');
		}//if

		if(ms100Count == 10)
		{
			sCount++;
			ms100Count = 0;

			requestTemperatureData(addrList[2]);
			motTemp = calculateTemperature('F');

			requestTemperatureData(addrList[3]);
			ambTemp = calculateTemperature('F');

//			gpsGetSpeed();
			gpsQualityCheck(speed);


			sprintf(piMsg, "%u %u %u %u %s %ld %u %lu %u\n", mcuTemp, batTemp, motTemp, ambTemp, speed, current, voltage, power, batPct);
			printString(piMsg);
			current = 0;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void changeMLX_slvADDR( void )
{
	uint32_t i;

	/* Wait for an idle I2C line */
	while(I2C1->SR2 & I2C_SR2_BUSY);

	/* Generate start bit and wait */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_START);
	while(!(I2C1->SR1 & I2C_SR1_SB));
	while((I2C1->SR2 & (I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_BUSY | I2C_SR2_MSL));

	/* Transmit IR 7-bit address + Write (0) and wait */
	I2C1->DR = (uint8_t)((0 << 1) | 0);
	while(!(I2C1->SR1 & I2C_SR1_ADDR));
	while((I2C1->SR2 & (I2C_SR2_TRA  | I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_TRA  | I2C_SR2_BUSY | I2C_SR2_MSL));

	/* Transmit command to access EEPROM cell 0x0E and wait */
	I2C1->DR = (uint8_t)(0x2E);
	while((I2C1->SR1 & (I2C_SR1_TXE)) != (I2C_SR1_TXE));

	I2C1->DR = (uint8_t)(0x00);
	while((I2C1->SR1 & (I2C_SR1_TXE)) != (I2C_SR1_TXE));

	I2C1->DR = (uint8_t)(0x00);
	while((I2C1->SR1 & (I2C_SR1_TXE)) != (I2C_SR1_TXE));

	I2C1->DR = (uint8_t)(0x6F);
	while((I2C1->SR1 & (I2C_SR1_TXE)) != (I2C_SR1_TXE));

	/* Generate a stop */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_STOP);

	TIM_SetCounter(TIM2, 0);
	for(i = 0; i < 50; i++)
	{
		while(!TIM_GetFlagStatus(TIM2, TIM_FLAG_Update));
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	}

	//////////////////////////////////////////////////////////////////
	/****************** EEPROM cell cleared *************************/
	//////////////////////////////////////////////////////////////////

	/* Wait for an idle I2C line */
	while(I2C1->SR2 & I2C_SR2_BUSY);

	/* Regenerate start bit and wait */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_START);
	while(!(I2C1->SR1 & I2C_SR1_SB));
	while((I2C1->SR2 & (I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_BUSY | I2C_SR2_MSL));

	/* Retransmit IR 7-bit address + write (0) and wait */
	I2C1->DR = (uint8_t)((0 << 1) | 0);
	while(!(I2C1->SR1 & I2C_SR1_ADDR));
	while((I2C1->SR2 & (I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_BUSY | I2C_SR2_MSL));

	/* Transmit command to access EEPROM cell 0x0E and wait */
	I2C1->DR = (uint8_t)(0x2E);
	while((I2C1->SR1 & (I2C_SR1_TXE)) != (I2C_SR1_TXE));

	I2C1->DR = (uint8_t)(NEW_ADDR);
	while((I2C1->SR1 & (I2C_SR1_TXE)) != (I2C_SR1_TXE));

	I2C1->DR = (uint8_t)(0x00);
	while((I2C1->SR1 & (I2C_SR1_TXE)) != (I2C_SR1_TXE));

	I2C1->DR = (uint8_t)(ADDR_PEC);
	while((I2C1->SR1 & (I2C_SR1_TXE)) != (I2C_SR1_TXE));

	/* Generate a stop */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_STOP);

	TIM_SetCounter(TIM2, 0);
	for(i = 0; i < 50; i++)
	{
		while(!TIM_GetFlagStatus(TIM2, TIM_FLAG_Update));
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void readADDRtest(uint8_t address)
{
	/* Wait for an idle I2C line */
	while(I2C1->SR2 & I2C_SR2_BUSY);

	/* Generate start bit and wait */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_START);
	while(!(I2C1->SR1 & I2C_SR1_SB));
	while((I2C1->SR2 & (I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_BUSY | I2C_SR2_MSL));

	/* Transmit IR 7-bit address + Write (0) and wait */
	I2C1->DR = (uint8_t)((address << 1) | 0);
	while(!(I2C1->SR1 & I2C_SR1_ADDR));
	while((I2C1->SR2 & (I2C_SR2_TRA  | I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_TRA  | I2C_SR2_BUSY | I2C_SR2_MSL));

	/* Transmit command to access RAM cell 0x06 and wait */
	I2C1->DR = (uint8_t)(0x2E);
	while((I2C1->SR1 & (I2C_SR1_TXE | I2C_SR1_BTF)) !=
				(I2C_SR1_TXE | I2C_SR1_BTF));

	/* Enable acknowledge and PEC */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_ACK);

	/* Regenerate start bit and wait */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_START);
	while(!(I2C1->SR1 & I2C_SR1_SB));
	while((I2C1->SR2 & (I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_BUSY | I2C_SR2_MSL));

	/* Retransmit IR 7-bit address + read (1) and wait */
	I2C1->DR = (uint8_t)((address << 1) | 1);
	while(!(I2C1->SR1 & I2C_SR1_ADDR));
	while((I2C1->SR2 & (I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_BUSY | I2C_SR2_MSL));

	/* Wait for 3rd to last and 2nd to last byte to be received*/
	while((I2C1->SR1 & (I2C_SR1_RXNE | I2C_SR1_BTF)) !=
				(I2C_SR1_RXNE | I2C_SR1_BTF));

	/* Clear acknowledge bit and set PEC position to next */
	I2C1->CR1 &= (uint32_t)(~I2C_CR1_ACK);
	I2C1->CR1 |= (uint32_t)(I2C_CR1_POS);

	/* Read 3rd to last byte (Temperature Data Byte Low) */
	uint16_t checkAddr = (uint16_t)(I2C1->DR);

	/* Generate a stop */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_STOP);

	/* Read 2nd to last byte (Temperature Data Byte High) */
	checkAddr = (uint16_t)(I2C1->DR) << 8;

	/* Wait for last byte to be received in DR */
	while(!(I2C1->SR1 & I2C_SR1_RXNE));

	/* Enable PEC calculation */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_ENPEC);

	/* Read last byte received (PEC) */
	uint8_t tempPEC = (uint8_t)(I2C1->DR);

	sprintf(mess, "%u\n\n", checkAddr);
	printString(mess);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void readTEMPtest(uint8_t address)
{
	/* Wait for an idle I2C line */
	while(I2C1->SR2 & I2C_SR2_BUSY);

	/* Generate start bit and wait */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_START);
	while(!(I2C1->SR1 & I2C_SR1_SB));
	while((I2C1->SR2 & (I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_BUSY | I2C_SR2_MSL));

	/* Transmit IR 7-bit address + Write (0) and wait */
	I2C1->DR = (uint8_t)((address << 1) | 0);
	while(!(I2C1->SR1 & I2C_SR1_ADDR));
	while((I2C1->SR2 & (I2C_SR2_TRA  | I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_TRA  | I2C_SR2_BUSY | I2C_SR2_MSL));

	/* Transmit command to access RAM cell 0x06 and wait */
	I2C1->DR = (uint8_t)(0x06);
	while((I2C1->SR1 & (I2C_SR1_TXE | I2C_SR1_BTF)) !=
				(I2C_SR1_TXE | I2C_SR1_BTF));

	/* Enable acknowledge and PEC */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_ACK);

	/* Regenerate start bit and wait */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_START);
	while(!(I2C1->SR1 & I2C_SR1_SB));
	while((I2C1->SR2 & (I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_BUSY | I2C_SR2_MSL));

	/* Retransmit IR 7-bit address + read (1) and wait */
	I2C1->DR = (uint8_t)((address << 1) | 1);
	while(!(I2C1->SR1 & I2C_SR1_ADDR));
	while((I2C1->SR2 & (I2C_SR2_BUSY | I2C_SR2_MSL)) !=
				(I2C_SR2_BUSY | I2C_SR2_MSL));

	/* Wait for 3rd to last and 2nd to last byte to be received*/
	while((I2C1->SR1 & (I2C_SR1_RXNE | I2C_SR1_BTF)) !=
				(I2C_SR1_RXNE | I2C_SR1_BTF));

	/* Clear acknowledge bit and set PEC position to next */
	I2C1->CR1 &= (uint32_t)(~I2C_CR1_ACK);
	I2C1->CR1 |= (uint32_t)(I2C_CR1_POS);

	/* Read 3rd to last byte (Temperature Data Byte Low) */
	uint16_t checkTemp = (uint16_t)(I2C1->DR);

	/* Generate a stop */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_STOP);

	/* Read 2nd to last byte (Temperature Data Byte High) */
	checkTemp = (uint16_t)(I2C1->DR) << 8;

	/* Wait for last byte to be received in DR */
	while(!(I2C1->SR1 & I2C_SR1_RXNE));

	/* Enable PEC calculation */
	I2C1->CR1 |= (uint32_t)(I2C_CR1_ENPEC);

	/* Read last byte received (PEC) */
	uint8_t tempPEC = (uint8_t)(I2C1->DR);

	sprintf(mess, "...%u\n\n", checkTemp);
	printString(mess);
}
