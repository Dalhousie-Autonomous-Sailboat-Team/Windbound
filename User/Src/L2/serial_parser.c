/**
 * @file serial_parser.c
 *
 * @brief Router code for parsing serial data.
 */

/* System Headers */
#include "assert.h"
#include "main.h"
#include "cmsis_os2.h"

/* Module Header */
#include "serial_parser.h"

#include "user_uart.h"

extern osMessageQueueId_t uart_rx_queueHandle;

static void ProcessDebugData(uint8_t data)
{
    /* Placeholder for processing UART4 data */
    /* Implement actual parsing logic here */
}

/**
 * @brief UART Serial Data Parser Task
 *
 * @param argument Pointer to the I2C bus number (1 or 2)
 */
void UARTParserTask(void *argument)
{
    UART_Char_t uart_char;
    while (true)
    {
        osMessageQueueGet(uart_rx_queueHandle, &uart_char, NULL, osWaitForever);
        switch (uart_char.port)
        {
        case UART_PORT_4:
            /* Handle data from UART4 */
            ProcessDebugData(uart_char.data);
            break;
        default:
            continue;
        }
    }
}