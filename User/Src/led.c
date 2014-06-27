/*
 * ledflash.c
 *
 *  Created on: Jun 5, 2013
 *      Author: dell
 */

#include "led.h"

// pin_0 led 
// pin_1 amber
// pin_2 green
// pin_3 change

#define RED_DURATION 500	 // 10ms*500 = 5s
#define RED_AND_AMBER_DURATION 300
#define GREEN_DURATION 600
#define AMBER_DURATION 300

static eLED_STATE led_state;
static int time_in_state;

void led_init(void)
{
    led_state = RED;
    /* red led light */
	GPIO_Write(GPIOC, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
}

void led_change_state(void)
{
    switch (led_state)
    {
    case RED:
        GPIO_ResetBits(GPIOC, GPIO_Pin_0);
        
        if (++time_in_state >= RED_DURATION)
        {
            led_state = RED_AMBER;
            time_in_state = 0;
        }
        
        break;
        
    case RED_AMBER:
        GPIO_Write(GPIOC, GPIO_Pin_2 | GPIO_Pin_3);
        
        if (++time_in_state >= RED_AND_AMBER_DURATION)
        {
            led_state = GREEN;
            time_in_state = 0;
        }
        
        break;
        
    case GREEN:
        GPIO_Write(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3);
        
        if (++time_in_state >= GREEN_DURATION)
        {
            led_state = AMBER;
            time_in_state = 0;
        }
        
        break;
        
    case AMBER:
        GPIO_Write(GPIOC, GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3);
        
        if (++time_in_state >= AMBER_DURATION)
        {
            led_state = RED;
            time_in_state = 0;
        }
        
        break;
        
    default:
        GPIO_Write(GPIOC, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
        break;
    }
}
