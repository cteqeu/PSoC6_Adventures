// by cteq.eu
// 26/05/2021

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "GUI.h"
#include "mtb_st7789v.h"
#include "cy8ckit_028_tft_pins.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "string.h"

const mtb_st7789v_pins_t tft_pins =
{
    .db08 = CY8CKIT_028_TFT_PIN_DISPLAY_DB8,
    .db09 = CY8CKIT_028_TFT_PIN_DISPLAY_DB9,
    .db10 = CY8CKIT_028_TFT_PIN_DISPLAY_DB10,
    .db11 = CY8CKIT_028_TFT_PIN_DISPLAY_DB11,
    .db12 = CY8CKIT_028_TFT_PIN_DISPLAY_DB12,
    .db13 = CY8CKIT_028_TFT_PIN_DISPLAY_DB13,
    .db14 = CY8CKIT_028_TFT_PIN_DISPLAY_DB14,
    .db15 = CY8CKIT_028_TFT_PIN_DISPLAY_DB15,
    .nrd  = CY8CKIT_028_TFT_PIN_DISPLAY_NRD,
    .nwr  = CY8CKIT_028_TFT_PIN_DISPLAY_NWR,
    .dc   = CY8CKIT_028_TFT_PIN_DISPLAY_DC,
    .rst  = CY8CKIT_028_TFT_PIN_DISPLAY_RST
};

static void TxTask(void *pvParameters);
static void RxTask(void *pvParameters);

static TaskHandle_t xTxTask;
static TaskHandle_t xRxTask;

static QueueHandle_t xQueue = NULL;

char HWString[7]= "cteq.eu";


int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    __enable_irq();

    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,CY_RETARGET_IO_BAUDRATE);
    result = mtb_st7789v_init8(&tft_pins);
    GUI_Init();
    GUI_SetColor(GUI_GREEN);
    GUI_SetBkColor(GUI_BLACK);
    GUI_SetFont(GUI_FONT_20B_1);
    GUI_DispString("Before FreeRTOS Scheduler...");
    printf("Before FreeRTOS Scheduler");

    xTaskCreate(TxTask, ( const char * ) "Tx", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTxTask);
    xTaskCreate(RxTask, ( const char * ) "Rx", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &xRxTask);

    xQueue = xQueueCreate(1,sizeof(HWString));

    vTaskStartScheduler();
}

static void TxTask( void *pvParameters )
{

	int i=0;
	for( ;; )
	{
		Cy_SysLib_Delay(1000);
		if( i==0)
			xQueueSend( xQueue,	"Red", 0UL );
		if(i==1)
			xQueueSend( xQueue,	"Green", 0UL );
		if(i==2)
			xQueueSend( xQueue,	"Blue", 0UL );
		i++;
		if (i>2)
			i=0;
	}
}

/*-----------------------------------------------------------*/
static void RxTask( void *pvParameters )
{
char Recdstring[7] = "";

	for( ;; )
	{
		xQueueReceive( 	xQueue,Recdstring,portMAX_DELAY );
		printf( "Rx task received: %s\r\n", Recdstring );
		Cy_GPIO_Clr(CYBSP_LED_RGB_RED_PORT,CYBSP_LED_RGB_RED_PIN);
		Cy_GPIO_Clr(CYBSP_LED_RGB_BLUE_PORT,CYBSP_LED_RGB_BLUE_PIN);
		Cy_GPIO_Clr(CYBSP_LED_RGB_GREEN_PORT,CYBSP_LED_RGB_GREEN_PIN);
		GUI_Clear();

		if(strcmp(Recdstring,"Red")==0){
			Cy_GPIO_Set(CYBSP_LED_RGB_RED_PORT,CYBSP_LED_RGB_RED_PIN);
			Cy_GPIO_Clr(CYBSP_LED_RGB_BLUE_PORT,CYBSP_LED_RGB_BLUE_PIN);
			Cy_GPIO_Clr(CYBSP_LED_RGB_GREEN_PORT,CYBSP_LED_RGB_GREEN_PIN);
			GUI_SetBkColor(GUI_RED);
		}
		if(strcmp(Recdstring,"Green")==0){
			Cy_GPIO_Set(CYBSP_LED_RGB_GREEN_PORT,CYBSP_LED_RGB_GREEN_PIN);
			Cy_GPIO_Clr(CYBSP_LED_RGB_BLUE_PORT,CYBSP_LED_RGB_BLUE_PIN);
			Cy_GPIO_Clr(CYBSP_LED_RGB_RED_PORT,CYBSP_LED_RGB_RED_PIN);
			GUI_SetBkColor(GUI_GREEN);
		}
		if(strcmp(Recdstring,"Blue")==0){
			Cy_GPIO_Set(CYBSP_LED_RGB_BLUE_PORT,CYBSP_LED_RGB_BLUE_PIN);
			Cy_GPIO_Clr(CYBSP_LED_RGB_RED_PORT,CYBSP_LED_RGB_RED_PIN);
			Cy_GPIO_Clr(CYBSP_LED_RGB_GREEN_PORT,CYBSP_LED_RGB_GREEN_PIN);
			GUI_SetBkColor(GUI_BLUE);
		}

	}
}


/* [] END OF FILE */
