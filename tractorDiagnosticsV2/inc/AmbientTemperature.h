/*
 * AmbientTemperature.h
 *
 *  Created on: Feb 14, 2018
 *      Author: Zach
 */

#ifndef AMBIENTTEMPERATURE_H_
#define AMBIENTTEMPERATURE_H_

/***************************************************************************
 **********************            INCLUDES           **********************
 ***************************************************************************/

#include "stdint.h"

/***************************************************************************
 **********************            DEFINES            **********************
 ***************************************************************************/

#define I2C_RX_BUFF_SIZE 			256
#define TEMPERATURE_BUFF_SIZE 		256
#define I2C_RX_BUFF_MASK 			0xFF
#define TEMPERATURE_BUFF_MASK		0xFF

#define THERM_ADDR_AMB		(uint8_t)(0x3A)
#define THERM_ADDR_MTR		(uint8_t)(0x4A)
#define THERM_ADDR_BTR		(uint8_t)(0x5A)
#define THERM_ADDR_CTL		(uint8_t)(0x6A)
#define THERM_READ		(uint8_t)(1)
#define THERM_WRITE		(uint8_t)(0)
#define TA_RAM_CELL 	(uint8_t)(0x07)
#define TO1_RAM_CELL	(uint8_t)(0x07)
#define TO2_RAM_CELL	(uint8_t)(0x08)

/***************************************************************************
 **********************           VARIABLES           **********************
 ***************************************************************************/

volatile char i2c_rxBuff[I2C_RX_BUFF_SIZE];
volatile char temperatureBuff[TEMPERATURE_BUFF_SIZE];
volatile uint8_t i2c_rxHead;
volatile uint8_t i2c_rxTail;
volatile uint8_t temperatureHead;
volatile uint8_t temperatureTail;
extern const uint8_t addrList[];

/***************************************************************************
 **********************           FUNCTIONS           **********************
 ***************************************************************************/

extern uint16_t calculateTemperature(char tempUnit);
extern void storeTemperatureData();
extern void requestTemperatureData(uint8_t address);

#endif
