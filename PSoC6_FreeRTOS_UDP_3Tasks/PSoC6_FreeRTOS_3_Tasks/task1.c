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

void task1(void *arg)
{
	int ReadValue=0;
	for(;;)
	{
		if(cyhal_gpio_read(CYBSP_USER_BTN)==0)
		{
			ReadValue=0;
			xQueueSend( queue_handle,	&ReadValue, 0UL );
		}
		else
		{
			ReadValue=1;
			xQueueSend( queue_handle,	&ReadValue, 0UL );
		}
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}


