// by cteq.eu
// 26/05/2021

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "GUI.h"
#include "mtb_st7789v.h"
#include "cy8ckit_028_tft_pins.h"


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
    GUI_DispString("Hello TFT World!");
    for (;;)
    {
    	Cy_GPIO_Inv(CYBSP_LED_RGB_BLUE_PORT,CYBSP_LED_RGB_BLUE_PIN);
    	Cy_SysLib_Delay(500);
    	Cy_GPIO_Inv(CYBSP_LED_RGB_GREEN_PORT,CYBSP_LED_RGB_GREEN_PIN);
    	Cy_SysLib_Delay(500);
    	Cy_GPIO_Inv(CYBSP_LED_RGB_RED_PORT,CYBSP_LED_RGB_RED_PIN);
    	Cy_SysLib_Delay(500);
    	printf( "Happy to send some info over UART using retarget-io and using the TFT...\r\n" );
    }
}

/* [] END OF FILE */
