/*
 * io.c
 *
 *  Created on: Jun 5, 2013
 *      Author: dell
 */

#include "io.h"

#define _EMC_CBI

#ifdef _EMC_CBI

#else
static uint16_t g_InData;
#endif

void SyncIO(void)
{
	#ifdef _EMC_CBI
	uint8_t INA1 = 0;
	uint8_t INA2 = 0;
	uint8_t INA3 = 0;
	uint8_t INA4 = 0;
	uint8_t INB1 = 0;
	uint8_t INB2 = 0;
	uint8_t INB3 = 0;
	uint8_t INB4 = 0;

	// read input data
	INA1 = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0);
	INA2 = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2);
	INA3 = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4);
	INA4 = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6);
	INB1 = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1);
	INB2 = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3);
	INB3 = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5);
	INB4 = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7);

	if((INA1 == (uint8_t)Bit_SET) || (INB1 == (uint8_t)Bit_SET))
	{
		GPIO_ResetBits(GPIOE, GPIO_Pin_8);
	}
	else
	{
		GPIO_SetBits(GPIOE, GPIO_Pin_8);
	}

	if((INA2 == (uint8_t)Bit_SET) || (INB2 == (uint8_t)Bit_SET))
	{
		GPIO_ResetBits(GPIOE, GPIO_Pin_9);
	}
	else
	{
		GPIO_SetBits(GPIOE, GPIO_Pin_9);
	}

	if((INA3 == (uint8_t)Bit_SET) || (INB3 == (uint8_t)Bit_SET))
	{
		GPIO_ResetBits(GPIOE, GPIO_Pin_10);
	}
	else
	{
		GPIO_SetBits(GPIOE, GPIO_Pin_10);
	}

	if((INA4 == (uint8_t)Bit_SET) || (INB4 == (uint8_t)Bit_SET))
	{
		GPIO_ResetBits(GPIOE, GPIO_Pin_11);
	}
	else
	{
		GPIO_SetBits(GPIOE, GPIO_Pin_11);
	}

	GPIO_SetBits(GPIOE, GPIO_Pin_12);
	GPIO_ResetBits(GPIOE, GPIO_Pin_13);
	
	#else
	uint16_t inData = 0;
	uint16_t outData = 0;
	
	inData = GPIO_ReadInputData(GPIOD);
	if(g_InData != inData)
	{
		uint16_t tmp;
		tmp = (inData << 8);
		outData = ~tmp;
		GPIO_Write(GPIOE, outData);
		g_InData = inData;
	}
	#endif
}

