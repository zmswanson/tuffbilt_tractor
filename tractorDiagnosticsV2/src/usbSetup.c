/*
 * usbSetup.c
 *
 *  Created on: Feb 18, 2018
 *      Author: Zach
 */

/***************************************************************************
 **********************            INCLUDES            *********************
 ***************************************************************************/

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "usbSetup.h"
#include "stdint.h"

/***************************************************************************
 **********************            VARIABLES           *********************
 ***************************************************************************/



/***************************************************************************
 **********************            FUNCTIONS           *********************
 ***************************************************************************/

void USB_DeInit(void)
{
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USB, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USB, DISABLE);
}

void USB_Init(void)
{
	USB->CNTR &= (uint32_t)(~USB_CNTR_PDWN);
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) < 37);
	USB->CNTR &= (uint32_t)(~USB_CNTR_FRES);
	USB->ISTR = 0;
	USB->DADDR |= (uint32_t)(USB_DADDR_EF);
}
