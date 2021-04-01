/*
 * NEO6_GPS.c
 *
 *  Created on: Mar 14, 2018
 *      Author: Zach
 */

#include "NEO6_GPS.h"
#include "RaspberryPi_Comms.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stdio.h"
#include "string.h"

void gpsGetSpeed(char * speedNow)
{
	int i, j = 0;

	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	for(i = 0; i < SPEED; ++i)
	{
		while(gpsVTG[j++] != ',');
	}

	j++;

	while(gpsVTG[j] != ',')
	{
		speedNow[j] = gpsVTG[j];
		j++;
//		outChar(gpsVTG[j++]);
	}

	crlf();

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}
void gpsQualityCheck(char * qualitySpeed)
{
	int i, j = 0;

	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);

	for(i = 0; i < QUALITY; ++i)
	{
		while(gpsGGA[j++] != ',');
	}

	if(gpsGGA[j] != '0')
	{
		gpsGetSpeed(&qualitySpeed);
	}
	else
	{
		for(i = 0; i < 3; i++)
		{
			qualitySpeed[i] = '0';
		}
	}

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void USART2_IRQHandler()
{
	/* If a rx dr not empty interrupt has occurred */
	if((USART2->SR) & ((uint32_t)(USART_SR_RXNE)))
	{
		/* Put character into next buffer position */
		char gpsC = (USART2->DR);

//		outChar(gpsC);

		if(gpsC == '$'){
			gpsPtr = 0;
		}

		else if(gpsC == '*')
		{
			if(!strncmp(gpsIn, "GPGGA", 5))
			{
				strncpy(gpsGGA, gpsIn, gpsPtr);
	            gpsPtr = 0;
			}

			else if(!strncmp(gpsIn, "GPVTG", 5))
			{
				strncpy(gpsVTG, gpsIn, gpsPtr);
	            gpsPtr = 0;
			}
		}

		else
		{
			gpsIn[gpsPtr++] = gpsC;
		}


		/* Clear the interrupt flag */
		(USART2->SR) &= ((uint32_t)(~USART_SR_RXNE));

	}else{}
}
