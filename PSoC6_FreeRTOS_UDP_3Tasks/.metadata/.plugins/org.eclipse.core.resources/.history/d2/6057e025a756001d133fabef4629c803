/******************************************************************************
* File Name:   udp_client.c
*
* Description: This file contains task and functions related to UDP client
*              operation.
*
********************************************************************************
* Copyright 2020-2022, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

/* Header file includes. */
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <inttypes.h>

/* FreeRTOS header file. */
#include <FreeRTOS.h>
#include <task.h>

/* Cypress secure socket header file. */
#include "cy_secure_sockets.h"

/* Wi-Fi connection manager header files. */
#include "cy_wcm.h"
#include "cy_wcm_error.h"

/* UDP client task header file. */
#include "udp_client.h"

/*******************************************************************************
* Macros
********************************************************************************/
/* LED ON, LED OFF and Invalid commands. */
#define LED_OFF_CMD                        '0'
#define LED_ON_CMD                         '1'
#define INVALID_CMD                        '2'

/* Acknowledgment messages to be sent UDP Server. */
#define ACK_LED_ON                         "LED ON ACK"
#define ACK_LED_OFF                        "LED OFF ACK"
#define INVALID_CMD_MSG                    "INVALID CMD RECEIVED"

/* Initial message sent to UDP Server to confirm client availability. */
#define START_COMM_MSG                         "A"

/* Buffer size to store the incoming messages from server, in bytes. */
#define MAX_UDP_RECV_BUFFER_SIZE          (20)

/* RTOS related macros for UDP client task. */
#define RTOS_TASK_TICKS_TO_WAIT           (1000)

/*******************************************************************************
* Function Prototypes
********************************************************************************/
static cy_rslt_t create_udp_client_socket(void);
static cy_rslt_t udp_client_recv_handler(cy_socket_t socket_handle, void *arg);
static cy_rslt_t connect_to_wifi_ap(void);
void print_heap_usage(char* msg);

/*******************************************************************************
* Global Variables
********************************************************************************/
/* UDP client socket handle */
cy_socket_t client_handle;
cy_socket_sockaddr_t peer_addr;

/* UDP Client task handle. */
extern TaskHandle_t client_task_handle;

/*******************************************************************************
 * Function Name: udp_client_task
 *******************************************************************************
 * Summary:
 *  Task used to create a UDP Client and communicate with remote UDP Server to
 *  control the LED state (ON/OFF) based on command received from UDP server.
 *
 * Parameters:
 *  void *args : Task parameter defined during task creation (unused).
 *
 * Return:
 *  void
 *
 *******************************************************************************/
void udp_client_task(void *arg)
{
    cy_rslt_t result ;

    /* Variable to store the number of bytes sent to the UDP server. */
    uint32_t bytes_sent = 0;

    /* Variable to receive LED ON/OFF from udp_client_recv_handler. */
    uint32_t led_state_ack = LED_OFF_CMD;

    /* IP address and UDP port number of the UDP server */
    cy_socket_sockaddr_t udp_server_addr = {
        .ip_address.ip.v4 = UDP_SERVER_IP_ADDRESS,
        .ip_address.version = CY_SOCKET_IP_VER_V4,
        .port = UDP_SERVER_PORT
    };

    /* Connect to Wi-Fi AP */
    if(connect_to_wifi_ap() != CY_RSLT_SUCCESS )
    {
        printf("\n Failed to connect to Wi-FI AP.\n");
        CY_ASSERT(0);
    }

    /* Secure Sockets initialized */
    result = cy_socket_init();
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Secure Sockets initialization failed!\n");
        CY_ASSERT(0);
    }
    printf("Secure Sockets initialized\n");

    result = create_udp_client_socket();
    if (result != CY_RSLT_SUCCESS)
    {
        printf("UDP Client Socket creation failed!\n");
        CY_ASSERT(0);
    }

    /* First send data to Server and wait to receive command */
    result = cy_socket_sendto(client_handle, START_COMM_MSG, strlen(START_COMM_MSG), CY_SOCKET_FLAGS_NONE,
                                &udp_server_addr, sizeof(cy_socket_sockaddr_t), &bytes_sent);
    if(result == CY_RSLT_SUCCESS)
    {
        printf("Data sent to server\n");
    }
    else
    {
        printf("Failed to send data to server. Error : %"PRIu32"\n", result);
    }

    while(true)
    {
        /* Wait till ON/OFF command is received from UDP Server . */
        xTaskNotifyWait(0, 0, &led_state_ack, portMAX_DELAY);

        printf("============================================================\n");

        /* Send acknowledgment to server after setting the LED ON or OFF */
        if(led_state_ack == LED_ON_CMD)
        {
            /* Turn the LED ON and set flag to send acknowledgment  */
            printf("Command received from server to turn on LED\n");
            cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
            printf("LED turned ON\n");

            result = cy_socket_sendto(client_handle, ACK_LED_ON, strlen(ACK_LED_ON), CY_SOCKET_FLAGS_NONE,
                                        &udp_server_addr, sizeof(cy_socket_sockaddr_t), &bytes_sent);
            if(result != CY_RSLT_SUCCESS)
            {
                printf("Failed to send Acknowledgment to server. Error: %"PRIu32"\n", result);
            }
        }
        else if(led_state_ack == LED_OFF_CMD)
        {
            /* Turn the LED OFF and set flag to send acknowledgment */
            printf("Command received from server to turn off LED\n");
            cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
            printf("LED turned OFF\n");

            result = cy_socket_sendto(client_handle, ACK_LED_OFF, strlen(ACK_LED_OFF), CY_SOCKET_FLAGS_NONE,
                                        &udp_server_addr, sizeof(cy_socket_sockaddr_t), &bytes_sent);
            if(result != CY_RSLT_SUCCESS)
            {
                printf("Failed to send acknowledgment to server. Error: %"PRIu32"\n", result);
            }
        }
        else
        {
            printf("Invalid command received.\n");
            result = cy_socket_sendto(client_handle, INVALID_CMD_MSG, strlen(INVALID_CMD_MSG), CY_SOCKET_FLAGS_NONE,
                                        &udp_server_addr, sizeof(cy_socket_sockaddr_t), &bytes_sent);
            if(result != CY_RSLT_SUCCESS)
            {
                printf("Failed to send acknowledgment to server. Error: %"PRIu32"\n", result);
            }
        }
        
        print_heap_usage("After controlling the LED and ACKing the server");
    }
 }

/*******************************************************************************
 * Function Name: connect_to_wifi_ap()
 *******************************************************************************
 * Summary:
 *  Connects to Wi-Fi AP using the user-configured credentials, retries up to a
 *  configured number of times until the connection succeeds.
 *
 *******************************************************************************/
cy_rslt_t connect_to_wifi_ap(void)
{
    cy_rslt_t result;

    /* Variables used by Wi-Fi connection manager.*/
    cy_wcm_connect_params_t wifi_conn_param;

    cy_wcm_config_t wifi_config = { .interface = CY_WCM_INTERFACE_TYPE_STA };

    cy_wcm_ip_address_t ip_address;

     /* Initialize Wi-Fi connection manager. */
    result = cy_wcm_init(&wifi_config);

    if (result != CY_RSLT_SUCCESS)
    {
        printf("Wi-Fi Connection Manager initialization failed!\n");
        return result;
    }
    printf("Wi-Fi Connection Manager initialized.\r\n");

     /* Set the Wi-Fi SSID, password and security type. */
    memset(&wifi_conn_param, 0, sizeof(cy_wcm_connect_params_t));
    memcpy(wifi_conn_param.ap_credentials.SSID, WIFI_SSID, sizeof(WIFI_SSID));
    memcpy(wifi_conn_param.ap_credentials.password, WIFI_PASSWORD, sizeof(WIFI_PASSWORD));
    wifi_conn_param.ap_credentials.security = WIFI_SECURITY_TYPE;

    /* Join the Wi-Fi AP. */
    for(uint32_t conn_retries = 0; conn_retries < MAX_WIFI_CONN_RETRIES; conn_retries++ )
    {
        result = cy_wcm_connect_ap(&wifi_conn_param, &ip_address);

        if(result == CY_RSLT_SUCCESS)
        {
            printf("Successfully connected to Wi-Fi network '%s'.\n",
                                wifi_conn_param.ap_credentials.SSID);
            printf("IP Address Assigned: %d.%d.%d.%d\n", (uint8)ip_address.ip.v4,
                    (uint8)(ip_address.ip.v4 >> 8), (uint8)(ip_address.ip.v4 >> 16),
                    (uint8)(ip_address.ip.v4 >> 24));
            return result;
        }

        printf("Connection to Wi-Fi network failed with error code %d."
               "Retrying in %d ms...\n", (int)result, WIFI_CONN_RETRY_INTERVAL_MSEC);

        vTaskDelay(pdMS_TO_TICKS(WIFI_CONN_RETRY_INTERVAL_MSEC));
    }

    /* Stop retrying after maximum retry attempts. */
    printf("Exceeded maximum Wi-Fi connection attempts\n");

    return result;
}

/*******************************************************************************
 * Function Name: create_udp_client_socket
 *******************************************************************************
 * Summary:
 *  Function to create a socket and set the socket options
 *  to set callback function for handling incoming messages.
 *
 *******************************************************************************/
cy_rslt_t create_udp_client_socket(void)
{
    cy_rslt_t result;

    /* Create a UDP socket. */
    result = cy_socket_create(CY_SOCKET_DOMAIN_AF_INET, CY_SOCKET_TYPE_DGRAM, CY_SOCKET_IPPROTO_UDP, &client_handle);
    if(result != CY_RSLT_SUCCESS)
    {
        return result;
    }

    /* Variable used to set socket receive callback function. */
    cy_socket_opt_callback_t udp_recv_option = {
            .callback = udp_client_recv_handler,
            .arg = NULL
    };

    /* Register the callback function to handle messages received from UDP client. */
    result = cy_socket_setsockopt(client_handle, CY_SOCKET_SOL_SOCKET, CY_SOCKET_SO_RECEIVE_CALLBACK,
                                    &udp_recv_option, sizeof(cy_socket_opt_callback_t));

    return result;
}

/*******************************************************************************
 * Function Name: udp_client_recv_handler
 *******************************************************************************
 * Summary:
 *  Callback function to handle incoming UDP server messages.
 *
 * Parameters:
 *  cy_socket_t socket_handle: Connection handle for the UDP client socket
 *  void *args : Parameter passed on to the function (unused)
 *
 * Return:
 *  cy_result result: Result of the operation
 *
 *******************************************************************************/
cy_rslt_t udp_client_recv_handler(cy_socket_t socket_handle, void *arg)
{
    cy_rslt_t result;
    /* Variable to store the number of bytes received. */
    uint32_t bytes_received = 0;
    /* Buffer to store received data. */
    char rx_buffer[MAX_UDP_RECV_BUFFER_SIZE] = {0};

    /* Receive incoming message from UDP server. */
    result = cy_socket_recvfrom(client_handle, rx_buffer, MAX_UDP_RECV_BUFFER_SIZE,
                                    CY_SOCKET_FLAGS_RECVFROM_NONE, NULL, 0, &bytes_received);

    /* Send notification to the task to turn the LED on/off and send acknowledgment to the server. */
    xTaskNotify(client_task_handle, (uint32_t)rx_buffer[0], eSetValueWithoutOverwrite);

    return result;
}

/* [] END OF FILE */
