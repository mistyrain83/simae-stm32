/*
 * io.c
 *
 *  Created on: Jun 5, 2013
 *      Author: dell
 */

#include "io.h"

static uint16_t g_InData;


void SyncIO(void)
{
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
}

