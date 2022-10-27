#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cycfg.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"

static volatile int uxTopUsedPriority; // needed for debugging with OpenOCD

static void PrintTask(void *pvParameters);
static void LEDTask(void *pvParameters);
static TaskHandle_t xPrintTask;
static TaskHandle_t xLEDTask;

int main(void)
{
    cy_rslt_t result;
    uxTopUsedPriority = configMAX_PRIORITIES - 1;
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

    printf("\r\nIn main....\r\n");
    xTaskCreate(PrintTask, ( const char * ) "Print_Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xPrintTask);
    xTaskCreate(LEDTask, ( const char * ) "LED_Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &xLEDTask);
    vTaskStartScheduler();
}

static void PrintTask( void *pvParameters )
{
	// Block for 500ms.
	const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
	for( ;; )
	{
		vTaskDelay(xDelay);
		printf("in Task...\r\n");
	}
}

static void LEDTask( void *pvParameters )
{
	// Block for 500ms.
	const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
	cyhal_gpio_init(CYBSP_USER_LED,CYHAL_GPIO_DIR_OUTPUT,CYHAL_GPIO_DRIVE_STRONG,0);
	for( ;; )
	{
		vTaskDelay(xDelay);
		cyhal_gpio_toggle(CYBSP_USER_LED);
	}
}
