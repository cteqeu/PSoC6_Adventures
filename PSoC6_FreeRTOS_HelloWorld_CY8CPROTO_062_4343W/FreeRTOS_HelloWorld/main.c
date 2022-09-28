#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#define TIMER_ID	1

static void TxTask(void *pvParameters);
static void RxTask(void *pvParameters);
static void vTimerCallback( TimerHandle_t pxTimer );

static TaskHandle_t xTxTask;
static TaskHandle_t xRxTask;

static QueueHandle_t xQueue = NULL;
static TimerHandle_t xTimer = NULL;

int ButtonValue=0;

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    
    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);

    /* retarget-io init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the User LED */
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, 
                             CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    /* GPIO init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    
    result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT,
                        CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);

    /* GPIO init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    xTimer = xTimerCreate( (const char *) "Timer",
    							pdMS_TO_TICKS(1000UL),
    							pdTRUE,
    							(void *) TIMER_ID,
    							vTimerCallback);


    printf("Before FreeRTOS Scheduler\r\n");

    xTaskCreate(TxTask, ( const char * ) "Tx", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTxTask);
    xTaskCreate(RxTask, ( const char * ) "Rx", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &xRxTask);

    xQueue = xQueueCreate(1,sizeof(int));
    xTimerStart( xTimer, 0 );

    vTaskStartScheduler();
}

static void TxTask( void *pvParameters )
{
	int ReadValue=0;
	for(;;)
	{
		if(cyhal_gpio_read(CYBSP_USER_BTN)==0)
		{
			ReadValue=0;
			xQueueSend( xQueue,	&ReadValue, 0UL );
		}
		else
		{
			ReadValue=1;
			xQueueSend( xQueue,	&ReadValue, 0UL );
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

static void RxTask( void *pvParameters )
{
	int ValueFromQueue=0;

	for(;;)
	{
		xQueueReceive(xQueue,&ValueFromQueue,portMAX_DELAY );
		if(ValueFromQueue ==0)
		{
			cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
		}
		else
		{
			cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
		}

	}
}
static void vTimerCallback( TimerHandle_t pxTimer )
{
	printf("Timer\r\n");
}

/* [] END OF FILE */