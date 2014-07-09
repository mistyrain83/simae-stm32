#ifndef __USART2_H
#define	__USART2_H

#include "stm32f10x.h"
#include <stdio.h>

void USART2_Config(void);

uint16_t USART_Send(USART_TypeDef* USARTx, uint8_t *Data, uint16_t nBytes);
uint16_t USART_Recv(USART_TypeDef* USARTx, uint8_t *Data, uint16_t nBytes);

int fputc(int ch, FILE *f);
void USART2_printf(USART_TypeDef* USARTx, uint8_t *Data,...);

#endif /* __USART2_H */
