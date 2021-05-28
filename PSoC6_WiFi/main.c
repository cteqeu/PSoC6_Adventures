// by cteq.eu
// 26/05/2021

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cycfg.h"
#include "cy_retarget_io.h"
#include "GUI.h"
#include "mtb_st7789v.h"
#include "cy8ckit_028_tft_pins.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "cy_wcm.h"

#define CLOUD_WIFI_AP        "XXXXXXXXXXXX"
#define CLOUD_WIFI_PW        "XXXXXXXXXXXX"
#define CLOUD_WIFI_SECURITY  CY_WCM_SECURITY_WPA2_MIXED_PSK
#define CLOUD_WIFI_BAND      CY_WCM_WIFI_BAND_ANY

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




void wifi_task(void* param);
void dm_task(void* param);

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

    printf("Before FreeRTOS Scheduler");
    Cy_SysLib_Delay(1000);
    xTaskCreate(wifi_task,    "WiFi"     ,configMINIMAL_STACK_SIZE*8  , NULL, 2, 0);
    xTaskCreate(dm_task,    "Display"     ,configMINIMAL_STACK_SIZE*8  , NULL, 1, 0);
    vTaskStartScheduler();
}


void wifi_task(void* param)
{
	cy_rslt_t result;
	char str[100];
		cy_wcm_connect_params_t connect_param = {
			.ap_credentials.SSID = CLOUD_WIFI_AP,
			.ap_credentials.password = CLOUD_WIFI_PW,
			.ap_credentials.security = CLOUD_WIFI_SECURITY,
			.static_ip_settings = 0,
			.BSSID = {0},
			.band = CLOUD_WIFI_BAND,
		};
		cy_wcm_config_t config = {.interface = CY_WCM_INTERFACE_TYPE_STA}; // We are a station (not a Access Point)

		cy_wcm_init(&config); // Initialize the connection manager

		printf("\nWi-Fi Connection Manager initialized.\n");
		do
			{

				cy_wcm_ip_address_t ip_address;

				printf("Connecting to Wi-Fi AP '%s'\n", connect_param.ap_credentials.SSID);
				result = cy_wcm_connect_ap(&connect_param, &ip_address);

				if (result == CY_RSLT_SUCCESS)
				{
					printf("Successfully connected to Wi-Fi network '%s'.\n",
							connect_param.ap_credentials.SSID);

					// Print IP Address
					if (ip_address.version == CY_WCM_IP_VER_V4)
					{
						printf("IPv4 Address Assigned: %d.%d.%d.%d\n", (uint8_t)ip_address.ip.v4,
								(uint8_t)(ip_address.ip.v4 >> 8), (uint8_t)(ip_address.ip.v4 >> 16),
								(uint8_t)(ip_address.ip.v4 >> 24));
						sprintf(str, "IPv4 Address Assigned: %d.%d.%d.%d\n", (uint8_t)ip_address.ip.v4,
								(uint8_t)(ip_address.ip.v4 >> 8), (uint8_t)(ip_address.ip.v4 >> 16),
								(uint8_t)(ip_address.ip.v4 >> 24));
						GUI_Clear();
						GUI_SetColor(GUI_BLACK);
						GUI_SetBkColor(GUI_GREEN);
						GUI_SetFont(GUI_FONT_20B_1);
						GUI_DispString(str);
					}
					else if (ip_address.version == CY_WCM_IP_VER_V6)
					{
						printf("IPv6 Address Assigned: %0X:%0X:%0X:%0X\n", (unsigned int)ip_address.ip.v6[0],
								(unsigned int)ip_address.ip.v6[1], (unsigned int)ip_address.ip.v6[2],
								(unsigned int)ip_address.ip.v6[3]);
						sprintf(str, "IPv6 Address Assigned: %0X:%0X:%0X:%0X\n", (unsigned int)ip_address.ip.v6[0],
								(unsigned int)ip_address.ip.v6[1], (unsigned int)ip_address.ip.v6[2],
								(unsigned int)ip_address.ip.v6[3]);
						GUI_Clear();
						GUI_SetColor(GUI_BLACK);
						GUI_SetBkColor(GUI_GREEN);
						GUI_SetFont(GUI_FONT_20B_1);
						GUI_DispString(str);
					}
					break; /* Exit the for loop once the connection has been made */
				}
				else
				{
					printf("WiFi Connect Failed Retrying\n");
					vTaskDelay(2000); // wait 2 seconds and try again;
				}

			} while (result != CY_RSLT_SUCCESS);
		while(1){};
}

void dm_task(void *arg)
{
    /* Initialize the display controller */
		mtb_st7789v_init8(&tft_pins);
		GUI_Init();
	    Cy_SysLib_Delay(1000);
	    GUI_SetColor(GUI_BLUE);
	    GUI_SetBkColor(GUI_BLACK);
	    GUI_SetFont(GUI_FONT_20B_1);
	    GUI_DispString("Before FreeRTOS Scheduler...");
	    xTaskCreate(wifi_task,    "WiFi"     ,configMINIMAL_STACK_SIZE*8  , NULL, 2, 0);
    while(1)
        vTaskDelay(portMAX_DELAY);
}
