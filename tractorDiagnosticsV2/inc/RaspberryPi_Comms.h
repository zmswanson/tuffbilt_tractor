/*
 * RaspberryPi_Comms.h
 *
 *  Created on: Feb 12, 2018
 *      Author: Zach
 */

/***************************************************************************
 **********************            INCLUDES           **********************
 ***************************************************************************/

#include "stdint.h"

/***************************************************************************
 **********************            DEFINES            **********************
 ***************************************************************************/

#define RX_BUFF_SIZE 	256
#define TX_BUFF_SIZE 	256
#define RX_BUFF_MASK 	0xFF
#define TX_BUFF_MASK 	0XFF

#define COMPLETE 		1
#define TRANSMITTING 	0

#define CMD_BUFF_SIZE 	256
#define CMD_BUFF_MASK 	0xFF

#define CMDN_SIZE		64
#define MSG_SIZE		1000

/***************************************************************************
 **********************           VARIABLES           **********************
 ***************************************************************************/

char rxBuff[RX_BUFF_SIZE];
char txBuff[TX_BUFF_SIZE];
volatile uint8_t rxHead ;
volatile uint8_t rxTail;
volatile uint8_t txHead;
volatile uint8_t txTail;
volatile uint8_t txStatus;

char cmdBuff[CMD_BUFF_SIZE];
volatile uint8_t cmdPtr;

char systemMessage[MSG_SIZE];
char cmd1[CMDN_SIZE];
char cmd2[CMDN_SIZE];
char errorMsg[MSG_SIZE];
char degreeStr[8];

/***************************************************************************
 **********************           FUNCTIONS           **********************
 ***************************************************************************/

extern void outChar(char c);
extern void crlf();
extern void printString(char *s);
extern void printStringCRLF(char *s);
extern void copyCommand();
extern void checkReceivedData();
extern void getNthCmd(int cmdNum, char * nthCmd, char * cmdList);
extern void processCMD();
