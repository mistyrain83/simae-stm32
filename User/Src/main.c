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

/* Includes ------------------------------------------------------------------*/
//#include "stm32f1xx_hal.h"
#include "main.h"
#include "gpio.h"
#include "usart2.h"


#define LED0_ON()   GPIO_SetBits(GPIOC,GPIO_Pin_0);
#define LED0_OFF()  GPIO_ResetBits(GPIOC,GPIO_Pin_0);

/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;

/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nTime);
void STM_EMCAE_LEDAllOn(void);
void STM_EMCAE_LEDAllOff(void);

/* Private functions ---------------------------------------------------------*/
static void vLEDTask( void *pvParameters );

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  char c;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */
  USART2_Config();
  //NVIC_Configuration();
  /* USER CODE END 2 */

  /* USER CODE BEGIN 3 */
  GPIO_SetBits(GPIOA, GPIO_Pin_0);
  // off
  GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
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
  xTaskCreate( vLEDTask, ( signed portCHAR * ) "LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, NULL );
  /* 启动OS */
  vTaskStartScheduler();
  
  /*
  while (1)
  {
	    GPIO_Write(GPIOC, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
		//GPIO_SetBits(GPIOE, GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13);

		// Insert 500 ms delay
		Delay(50);
		GPIO_Write(GPIOC, GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3);
		//GPIO_ResetBits(GPIOE, GPIO_Pin_8);

		Delay(50);
		GPIO_Write(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3);
		//GPIO_ResetBits(GPIOE, GPIO_Pin_9);

		Delay(50);
		GPIO_Write(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
		//GPIO_ResetBits(GPIOE, GPIO_Pin_10);

		Delay(50);
		GPIO_ResetBits(GPIOE, GPIO_Pin_11);
		STM_EMCAE_LEDAllOn();
//
//		Delay(50);
//		GPIO_ResetBits(GPIOE, GPIO_Pin_12);

//		Delay(50);
//		GPIO_ResetBits(GPIOE, GPIO_Pin_13);
		USART_SendData(USART2, c);
		USART2_printf(USART2, "\r\n This is a USART2_printf demo \r\n");
		c++;
		// Insert 500 ms delay
		Delay(50);
  }

  */
  /* USER CODE END 3 */
return 0;
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4  
  * @retval None
  */
void STM_EMCAE_LEDAllOn(void)
{
	GPIO_WriteBit(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, Bit_RESET);
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4 
  * @retval None
  */
void STM_EMCAE_LEDAllOff(void)
{
  GPIO_WriteBit(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, Bit_SET);  
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}
/* USER CODE END 4 */

void vLEDTask( void *pvParameters )
{
  for( ;; )
  {
    LED0_ON();
    vTaskDelay( 500/portTICK_RATE_MS );
    LED0_OFF();
    vTaskDelay( 500/portTICK_RATE_MS );
  }
}



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
