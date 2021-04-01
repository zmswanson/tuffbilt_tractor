/*
 * BMS.c
 *
 *  Created on: Apr 4, 2018
 *      Author: Zach
 */
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "BMS.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

extern uint16_t adcVal[];
volatile uint16_t tempADC0 = 0, tempADC1 = 0;
const uint32_t batteryFull = FULL_CHARGE;

void DMA1_Channel1_IRQHandler()
{
	DMA_ClearFlag(DMA1_FLAG_TC1);
}

void initBMS( void )
{
	oldAmps = 0;
	newAmps = 0;
	amp_usecs = 0;
	amp_msecs = 0;
	total_A_ms = batteryFull;
	avgVolts = 0;
	newVolts = 0;
	maxAmps = 0;
	pwr = 0;
}

void getAmps( int32_t * maxAmps )
{
	oldAmps = newAmps;

    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, DISABLE);
    newAmps = (int32_t)adcVal[0];
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

	newAmps = newAmps * 1612;
	newAmps = newAmps - 3200000;
	newAmps = newAmps / 7600;

	amp_usecs += (int32_t)(((oldAmps + newAmps) / 2) * 10);

	if( (newAmps > *maxAmps) || ((newAmps * -1) > *maxAmps) )
	{
		*maxAmps = newAmps;
	}
}

uint8_t batterySOC( void )
{
	amp_msecs = (uint32_t)(amp_usecs / 1000);
	total_A_ms -= amp_msecs;

	charge = (uint8_t)(total_A_ms / (batteryFull / 100));

	amp_usecs = 0;

	if(charge > 100)
	{
		charge = 100;
	}

	if(charge < 0)
	{
		charge = 0;
	}

	return charge;
}

void batteryRunTime( int * minutes )
{
	avgAmps = (int32_t)(amp_usecs / 100);

	if( avgAmps > 0 )
	{
		*minutes = (int)((total_A_ms / avgAmps) / 60000);
	}
}

uint16_t getVolts( void )
{
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, DISABLE);
    tempADC1 = (uint16_t)(adcVal[1]);
	newVolts = (uint16_t)(tempADC1);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	newVolts = (uint16_t)(((newVolts * 3) + (newVolts / 4)) / 100);

	avgVolts += (uint32_t)(newVolts);

	return newVolts;
}



uint32_t getPower( uint32_t deezAmps, uint32_t deezVolts )
{
	avgVolts /= 100;
	pwr = deezAmps * deezVolts;

	avgVolts = 0;
	avgAmps = 0;

	return pwr;
}
