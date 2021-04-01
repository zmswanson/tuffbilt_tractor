/*
 * RaspberryPi_Comms.c
 *
 *  Created on: Feb 12, 2018
 *      Author: Zach Swanson
 */

#include "AmbientTemperature.h"
#include "RaspberryPi_Comms.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stdio.h"
#include "string.h"

/***************************************************************************
 **********************          TX Functions          *********************
 ***************************************************************************/

void outChar(char c){
  txBuff[txHead++] = c;
  txHead &= TX_BUFF_MASK;
  USART1->CR1  |= USART_CR1_TXEIE;
}

void crlf(){
  outChar(0x0D);
  outChar(0x0A);
}

void printString(char *s){
  while(s[0]){
    outChar(s[0]);
    s++;
  }
}

void printStringCRLF(char *s){
  printString(s);
  crlf();
}

/***************************************************************************
 **********************       Command Processing       *********************
 ***************************************************************************/

/*
 * getNthCmd -
 */
void getNthCmd(int cmdNum, char * nthCmd, char * cmdList)
{
	// cmdList and thisCmd position indexes //
	int i = 0;
	int j = 0;

	// Skip over initial non-characters //
	while(cmdList[i] == ' ')i++;

	while(cmdNum)
	{
		// Skip over first command and recognize null, space, cr, nl.... //
		while(cmdList[i] > ' ')i++;
		// Skip over blanks leading up to next command //
		while(cmdList[i] <= ' ')
		{
			if(cmdList[i] == 0)
			{
				//do nothing break out
			}else{
				i++;
			}
		}
		cmdNum--;
	}

	while(cmdList[i] > ' ')
	{
		nthCmd[j++] = cmdList[i++];
	}
	nthCmd[j++] = 0;
}

/*
 * processCMD -
 */
void processCMD()
{
	memset(cmd1, 0, CMDN_SIZE);
	memset(cmd2, 0, CMDN_SIZE);
	memset(errorMsg, 0, MSG_SIZE);
	memset(degreeStr, 0, 8);

	getNthCmd(0, &cmd1[0], &cmdBuff[0]);

	switch(cmd1[0])
	{
	case 'T':
	case 't':
//		requestTemperatureData();
		getNthCmd(1, &cmd2[0], &cmdBuff[0]);

		switch(cmd2[0])
		{
		case 'F':
		case 'f':
			sprintf(degreeStr, "%i", calculateTemperature('f'));
			printString(&degreeStr[0]);
			break;
		case 'C':
		case 'c':
			sprintf(degreeStr, "%i", calculateTemperature('c'));
			printString(&degreeStr[0]);
			break;
		case 'K':
		case 'k':
			sprintf(degreeStr, "%i", calculateTemperature('k'));
			printString(&degreeStr[0]);
			break;
		default:
			sprintf(errorMsg, "Unknown temperature unit.");
			printStringCRLF(&errorMsg[0]);
			break;
		}
		break;
	default:
		sprintf(errorMsg, "Unknown command entered.");
		printStringCRLF(&errorMsg[0]);
		break;
	}
	memset(cmdBuff, 0, CMD_BUFF_SIZE);
}

/***************************************************************************
 **********************          RX Functions          *********************
 ***************************************************************************/

/*
 * copyCommand -
 */
void copyCommand()
{
	if(cmdPtr > CMD_BUFF_SIZE - 2)
	{
		cmdPtr = CMD_BUFF_SIZE - 2;
	}else{}

	USART_ITConfig(USART1, USART_IT_RXNE|USART_IT_TXE, DISABLE);
	char c = rxBuff[rxTail++];
	USART_ITConfig(USART1, USART_IT_RXNE|USART_IT_TXE, ENABLE);
	rxTail &= RX_BUFF_MASK;

	if(c == '\n')
	{
		cmdPtr = 0;
		processCMD();
	}else {
		cmdBuff[cmdPtr] = c;
		cmdPtr++;
	}
}

/*
 * checkReceivedData -
 */
void checkReceivedData()
{
	while(rxHead != rxTail)
	{
		copyCommand();
	}
}

/***************************************************************************
 **********************       USART1 Interrupts        *********************
 ***************************************************************************/

void USART1_IRQHandler()
{
	/* If a rx dr not empty interrupt has occurred */
	if((USART1->SR) & ((uint32_t)(USART_SR_RXNE)))
	{
		/* Put character into next buffer position */
		rxBuff[rxHead++] = (USART1->DR);

		/* Apply buffer mask to create circular buffer */
		rxHead &= RX_BUFF_MASK;

		/* Clear the interrupt flag */
		(USART1->SR) &= ((uint32_t)(~USART_SR_RXNE));

	}else{}

	/* If a tx dr empty interrupt has occurred */
	if((USART1->SR) & ((uint32_t)(USART_SR_TXE)))
	{
		if(txTail != txHead)
		{
			(USART1->DR) = txBuff[txTail++];
			txTail &= TX_BUFF_MASK;

		}else
		{
			/* Turn TX data register empty interrupt off */
			(USART1->CR1) &= (uint32_t)(~USART_CR1_TXEIE);
		}
	}else{}
}
