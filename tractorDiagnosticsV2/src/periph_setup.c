/*
 * periph_setup.c
 *
 *  Created on: Feb 11, 2018
 *      Author: Zach Swanson
 */

#include "stdint.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"

extern uint16_t adcVal[2];

void init_clock_72MHZ()
{
	/* De-initialize rcc */
	RCC_DeInit();

	/* Turn on external high-freq clock source and wait */
	RCC_HSEConfig(RCC_HSE_ON);
	RCC_WaitForHSEStartUp();

	/* Set flash latency to 2 cycles */
	FLASH_SetLatency(FLASH_Latency_2);

	/* Configure PLL freq to 72 MHz, i.e. (8 / 1) * 9 = 72 */
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

	/* Enable phase locked loop (PLL) */
	RCC_PLLCmd(ENABLE);

	/* Set the system clock to PLL and wait until PLL is recognized as sysclk */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08);

	/* Configure AHB to 72 MHz */
	RCC_HCLKConfig(RCC_SYSCLK_Div1);

	/* Configure APB1 to 36 MHz */
	RCC_PCLK1Config(RCC_HCLK_Div2);

	/* Configure APB2 to 72 MHz */
	RCC_PCLK2Config(RCC_HCLK_Div1);

	/* Configure ADC clk to 12 MHz */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	/* Configure USB clock to 48 MHz */
//	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
}

void init_AFIO()
{
	GPIO_AFIODeInit();
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);

	/* Enable desired GPIO peripheral clock and alternate function clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_USART2|RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_ADC1, ENABLE);

	GPIO_InitTypeDef pA0, pA1, pA2, pA3, pA4, pA5, pA6, pA7, pA9, pA10, pB6, pB7;

	/* Configure GPIOA pin 0 as ADC analog pin */
	pA0.GPIO_Pin = GPIO_Pin_0;
	pA0.GPIO_Mode = GPIO_Mode_AIN;
	pA0.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pA0);

	/* Configure GPIOA pin 1 as ADC analog pin */
	pA1.GPIO_Pin = GPIO_Pin_1;
	pA1.GPIO_Mode = GPIO_Mode_AIN;
	pA1.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pA1);

	/* Configure GPIOA pin 2 as USART2 TX line */
	pA2.GPIO_Pin = GPIO_Pin_2;
	pA2.GPIO_Mode = GPIO_Mode_AF_PP;
	pA2.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pA2);

	/* Configure GPIOA pin 3 as USART2 RX line */
	pA3.GPIO_Pin = GPIO_Pin_3;
	pA3.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	pA3.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pA3);

	/* Configure GPIOA pin 4 as ADC analog pin */
	pA4.GPIO_Pin = GPIO_Pin_4;
	pA4.GPIO_Mode = GPIO_Mode_AIN;
	pA4.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pA4);

	/* Configure GPIOA pin 5 as ADC analog pin */
	pA5.GPIO_Pin = GPIO_Pin_5;
	pA5.GPIO_Mode = GPIO_Mode_AIN;
	pA5.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pA5);

//	/* Configure GPIOA pin 4 as SPI1 NSS line */
//	pA4.GPIO_Pin = GPIO_Pin_4;
//	pA4.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	pA4.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &pA4);
//
//
//	/* Configure GPIOA pin 5 as SPI1 SCK line */
//	pA5.GPIO_Pin = GPIO_Pin_5;
//	pA5.GPIO_Mode = GPIO_Mode_AF_PP;
//	pA5.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &pA5);
//
//
//	/* Configure GPIOA pin 6 as SPI1 MISO line */
//	pA6.GPIO_Pin = GPIO_Pin_6;
//	pA6.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	pA6.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &pA6);
//
//	/* Configure GPIOA pin 7 as SPI1 MOSI line */
//	pA7.GPIO_Pin = GPIO_Pin_7;
//	pA7.GPIO_Mode = GPIO_Mode_AF_PP;
//	pA7.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &pA7);

	/* Configure GPIOA pin 9 as USART1 TX line */
	pA9.GPIO_Pin = GPIO_Pin_9;
	pA9.GPIO_Mode = GPIO_Mode_AF_PP;
	pA9.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pA9);

	/* Configure GPIOA pin 10 as USART1 RX line */
	pA10.GPIO_Pin = GPIO_Pin_10;
	pA10.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	pA10.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &pA10);

	/* Configure GPIOB pin 6 as I2C1 SCL line */
	pB6.GPIO_Pin = GPIO_Pin_6;
	pB6.GPIO_Mode = GPIO_Mode_AF_OD;
	pB6.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &pB6);

	/* Configure GPIOB pin 7 as I2C1 SDA line */
	pB7.GPIO_Pin = GPIO_Pin_7;
	pB7.GPIO_Mode = GPIO_Mode_AF_OD;
	pB7.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &pB7);
}

void init_DMA(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitTypeDef  DMA_InitStructure;
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 2;
    DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)0x4001244C);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adcVal;
    //send values to DMA registers
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    // Enable DMA1 Channel Transfer Complete interrupt
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE); //Enable the DMA1 - Channel1

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void init_ADC1()
{
	int i;

	ADC_DeInit(ADC1);

	ADC_InitTypeDef ADC1_Init;

	ADC1_Init.ADC_Mode = ADC_Mode_Independent;
	ADC1_Init.ADC_ScanConvMode = ENABLE;
	ADC1_Init.ADC_ContinuousConvMode = ENABLE;
	ADC1_Init.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC1_Init.ADC_DataAlign = ADC_DataAlign_Right;
	ADC1_Init.ADC_NbrOfChannel = 2;

	ADC_Init(ADC1, &ADC1_Init);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_239Cycles5);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 3, ADC_SampleTime_1Cycles5);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 4, ADC_SampleTime_1Cycles5);

	ADC_ExternalTrigConvCmd(ADC1, ENABLE);

	ADC_DMACmd(ADC1, ENABLE);

//	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
//	NVIC_EnableIRQ(ADC1_2_IRQn);

	ADC_ResetCalibration(ADC1);
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_Cmd(ADC1, ENABLE);
	for(i = 0; i < 72; i++);
	ADC_Cmd(ADC1, ENABLE);
}

void init_UART1()
{
	USART_DeInit(USART1);

	/* Define UART1 initialization structures */
	USART_InitTypeDef UART1_Init;
	USART_ClockInitTypeDef UART1_Clk;

	/* Setup initialization structures */
	USART_StructInit(&UART1_Init);
	USART_ClockStructInit(&UART1_Clk);

	USART_Init(USART1, &UART1_Init);
	USART_ClockInit(USART1, &UART1_Clk);

	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART1, ENABLE);

	NVIC_EnableIRQ(USART1_IRQn);
}

void init_UART2()
{
	USART_DeInit(USART2);

	/* Define UART1 initialization structures */
	USART_InitTypeDef UART2_Init;
	USART_ClockInitTypeDef UART2_Clk;

	/* Setup initialization structures */
	USART_StructInit(&UART2_Init);
	USART_ClockStructInit(&UART2_Clk);

	USART_Init(USART2, &UART2_Init);
	USART_ClockInit(USART2, &UART2_Clk);

//	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART2, ENABLE);

	NVIC_EnableIRQ(USART2_IRQn);
}

void init_TIM2()
{
	TIM_TimeBaseInitTypeDef TIM2_Init;

	TIM2_Init.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM2_Init.TIM_CounterMode = TIM_CounterMode_Up;
	TIM2_Init.TIM_Period = 360;
	TIM2_Init.TIM_Prescaler = 0;

	TIM_TimeBaseInit(TIM2, &TIM2_Init);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM2, ENABLE);
}

void init_I2C1()
{
	I2C_DeInit(I2C1);

	I2C_InitTypeDef I2C1_Init;

	I2C1_Init.I2C_Ack = I2C_Ack_Disable;
	I2C1_Init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C1_Init.I2C_ClockSpeed = 100000;
	I2C1_Init.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C1_Init.I2C_Mode = I2C_Mode_SMBusDevice;
	I2C1_Init.I2C_OwnAddress1 = 0;

	I2C_Init(I2C1, &I2C1_Init);

	I2C_ITConfig(I2C1, I2C_IT_BUF|I2C_IT_EVT|I2C_IT_ERR, ENABLE);

	I2C_Cmd(I2C1, ENABLE);
}

void init_SPI1()
{
	SPI_I2S_DeInit(SPI1);

	SPI_InitTypeDef SPI1_Init;

	SPI1_Init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI1_Init.SPI_Mode = SPI_Mode_Master;
	SPI1_Init.SPI_DataSize = SPI_DataSize_16b;
	SPI1_Init.SPI_CPOL = SPI_CPOL_Low;
	SPI1_Init.SPI_CPHA = SPI_CPHA_1Edge;
	SPI1_Init.SPI_NSS = SPI_NSS_Soft;
	SPI1_Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI1_Init.SPI_FirstBit = SPI_FirstBit_MSB;

	SPI_Init(SPI1, &SPI1_Init);

	SPI_Cmd(SPI1, ENABLE);
}
