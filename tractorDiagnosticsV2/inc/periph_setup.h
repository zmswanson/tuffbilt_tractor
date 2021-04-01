/*
 * periph_setup.h
 *
 *  Created on: Feb 11, 2018
 *      Author: Zach
 */

/***************************************************************************
 **********************           FUNCTIONS           **********************
 ***************************************************************************/

extern void init_clock_72MHZ();
extern void init_AFIO();
extern void init_UART1();
extern void init_UART2();
extern void init_TIM2();
extern void init_I2C1();
extern void init_SPI1();
extern void init_ADC1();
extern void init_DMA(void);
