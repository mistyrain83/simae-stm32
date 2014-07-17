/**
  ******************************************************************************
  * File Name          : main.c
  * Date               : 25/05/2014 07:53:32
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2014 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "temp.h"

/* Includes ------------------------------------------------------------------*/
//#include "stm32f1xx_hal.h"
#include "main.h"
#include "gpio.h"
#include "io.h"
#include "usart.h"
#include "can.h"


#define LED0_OFF()   GPIO_SetBits(GPIOC,GPIO_Pin_0);
#define LED0_ON()  GPIO_ResetBits(GPIOC,GPIO_Pin_0);

#define LED2_OFF()   GPIO_SetBits(GPIOC,GPIO_Pin_2);
#define LED2_ON()  GPIO_ResetBits(GPIOC,GPIO_Pin_2);

/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;

/* Private function prototypes -----------------------------------------------*/
static void vLEDTask( void *pvParameters );
static void vIOTask( void *pvParameters );
static void vRELYTask( void  );
static void vUSARTTask( void *pvParameters );
static void vTEMPTask( void *pvParameters );
static void vCANTask( void *pvParameters );

/* Private functions ---------------------------------------------------------*/


/* USER CODE BEGIN 0 */
/* ADC1转换的电压值通过MDA方式传到sram*/
extern __IO u16 ADC_ConvertedValue;

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */
  USART2_Config();
  //NVIC_Configuration();
  Temp_ADC1_Init();

  USER_CAN_Init();
  /* USER CODE END 2 */

  /* USER CODE BEGIN 3 */
  // rs485 oe - high send low recv
  GPIO_ResetBits(GPIOA, GPIO_Pin_0);
  // off
  GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
  // disable relay off
  GPIO_SetBits(GPIOE, GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11 
                          |GPIO_Pin_12|GPIO_Pin_13);

  /* Setup Sys Clock */
  if (SysTick_Config(SystemCoreClock / 100))
  { 
    /* Capture error */ 
    while (1);
  }

  /* Infinite loop */

  /* 建立任务 */
  xTaskCreate( vLEDTask, ( signed portCHAR * ) "LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5, NULL );

  xTaskCreate( vIOTask, ( signed portCHAR * ) "IO", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 4, NULL );

  //xTaskCreate( vRELYTask, ( signed portCHAR * ) "RELY", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL );

  //xTaskCreate( vUSARTTask, ( signed portCHAR * ) "USART", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 6, NULL );

  //xTaskCreate( vTEMPTask, ( signed portCHAR * ) "TEMP", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 7, NULL );

  //xTaskCreate( vCANTask, ( signed portCHAR * ) "CAN", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 8, NULL );

  /* 启动OS */
  vTaskStartScheduler();

  /* USER CODE END 3 */
return 0;
}

/* USER CODE BEGIN 4 */

void vLEDTask( void *pvParameters )
{
  for( ;; )
  {
    LED0_ON();
	LED2_OFF();
    vTaskDelay( 500/portTICK_RATE_MS );
    LED0_OFF();
	LED2_ON();
    vTaskDelay( 500/portTICK_RATE_MS );
	
  }
}

void vIOTask( void *pvParameters )
{
	vRELYTask();
	
  for( ;; )
  {
    SyncIO();
    vTaskDelay( 500/portTICK_RATE_MS );
  }
}

void vRELYTask( void )
{
	uint16_t i = 0;
  for(i = 0 ; i < 5; i++)
  {
  	vTaskDelay( 1000/portTICK_RATE_MS );
    GPIO_ResetBits(GPIOE, GPIO_Pin_8);
    vTaskDelay( 2000/portTICK_RATE_MS );
	GPIO_ResetBits(GPIOE, GPIO_Pin_9);
    vTaskDelay( 2000/portTICK_RATE_MS );
	GPIO_ResetBits(GPIOE, GPIO_Pin_10);
    vTaskDelay( 2000/portTICK_RATE_MS );
	GPIO_ResetBits(GPIOE, GPIO_Pin_11);
    vTaskDelay( 2000/portTICK_RATE_MS );
	GPIO_ResetBits(GPIOE, GPIO_Pin_12);
    vTaskDelay( 2000/portTICK_RATE_MS );
	GPIO_ResetBits(GPIOE, GPIO_Pin_13);
    vTaskDelay( 2000/portTICK_RATE_MS );
	GPIO_SetBits(GPIOE, GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11 
                          |GPIO_Pin_12|GPIO_Pin_13);
  }
  GPIO_SetBits(GPIOE, GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11 
                          |GPIO_Pin_12|GPIO_Pin_13);
}

void vUSARTTask( void *pvParameters )
{
	int i;
	uint16_t num;
	uint8_t str[16];
	for(i = 0; i < 16; i ++)
	{
		str[i] = i;
	}
  for( ;; )
  {
    num = USART_Recv(USART2, str, 16);
	if(num > 0)
	{
		USART_Send(USART2, str, num);
	}
  }
}


void vTEMPTask( void *pvParameters )
{
	/*计算后的温度值*/
 	uint16_t Current_Temp;
	uint8_t str[3];
	while(1)
	{
		vTaskDelay( 1000/portTICK_RATE_MS );
		Current_Temp = (V25 - ADC_ConvertedValue)/AVG_SLOPE + 25;
		
		str[0] = (uint8_t)((Current_Temp>>8)&0x00FF);
		str[1] = (uint8_t)(Current_Temp&0x00FF);
		USART_Send(USART2, str, 2);
	}
}

void vCANTask( void *pvParameters )
{
	while(1)
	{
		vTaskDelay( 1000/portTICK_RATE_MS );
		USER_CAN_Test();
		vTaskDelay( 1000/portTICK_RATE_MS );
	}
}

/* USER CODE END 4 */



#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
