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



void task3(void *arg)
{

	vTaskDelay(pdMS_TO_TICKS(5000));
	for(;;)
	{
		printf("Hello From Task 3\n\r");
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}


