// by cteq.eu
// 26/05/2021

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"


//extern cyhal_uart_t cy_retarget_io_uart_obj;

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

    for (;;)
    {
    	Cy_GPIO_Inv(CYBSP_LED_RGB_BLUE_PORT,CYBSP_LED_RGB_BLUE_PIN);
    	Cy_SysLib_Delay(500);
    	Cy_GPIO_Inv(CYBSP_LED_RGB_GREEN_PORT,CYBSP_LED_RGB_GREEN_PIN);
    	Cy_SysLib_Delay(500);
    	Cy_GPIO_Inv(CYBSP_LED_RGB_RED_PORT,CYBSP_LED_RGB_RED_PIN);
    	Cy_SysLib_Delay(500);
    	printf( "Happy to send some info over UART using retarget-io...\r\n" );
    }
}

/* [] END OF FILE */
