/*
 * usbSetup.h
 *
 *  Created on: Feb 18, 2018
 *      Author: Zach
 */

/***************************************************************************
 **********************            INCLUDES            *********************
 ***************************************************************************/

#include "stm32f10x.h"
#include "stdint.h"

/***************************************************************************
 **********************           STRUCTURES           *********************
 ***************************************************************************/

/**
  * @brief Universal Serial Bus Full Speed Device
  */
typedef struct
{
	__IO uint32_t EP0R;
	__IO uint32_t EP1R;
	__IO uint32_t EP2R;
	__IO uint32_t EP3R;
	__IO uint32_t EP4R;
	__IO uint32_t EP5R;
	__IO uint32_t EP6R;
	__IO uint32_t EP7R;
	uint32_t RESERVED0;
	uint32_t RESERVED1;
	uint32_t RESERVED2;
	uint32_t RESERVED3;
	uint32_t RESERVED4;
	uint32_t RESERVED5;
	uint32_t RESERVED6;
	uint32_t RESERVED7;
	__IO uint32_t CNTR;
	__IO uint32_t ISTR;
	__IO uint32_t FNR;
	__IO uint32_t DADDR;
	__IO uint32_t BTABLE;
} USB_TypeDef;

/***************************************************************************
 **********************             DEFINES            *********************
 ***************************************************************************/

#define USB_BASE 			(APB1PERIPH_BASE + 0x5C00)
#define USB_PMAADDR			(APB1PERIPH_BASE + 0x6000)

#define USB 				((USB_TypeDef *) USB_BASE)

/***************************************************************************
 **********************            FUNCTIONS           *********************
 ***************************************************************************/

extern void USB_DeInit(void);
extern void USB_Init(void);
