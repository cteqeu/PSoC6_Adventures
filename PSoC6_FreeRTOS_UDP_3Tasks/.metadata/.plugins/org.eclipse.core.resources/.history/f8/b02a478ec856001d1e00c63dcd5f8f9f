/*
 * task1.c
 *
 *  Created on: 27 okt. 2022
 *      Author: 20002890
 */

/* Header file includes. */
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <inttypes.h>

/* FreeRTOS header file. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

extern QueueHandle_t queue_handle;

void task2(void *arg)
{
	int ValueFromQueue=0;
	for(;;)
	{
		xQueueReceive(queue_handle,&ValueFromQueue,portMAX_DELAY );
		if(ValueFromQueue==0)
		{
			cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
		}
		else
		{
			cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
		}

	}
}


