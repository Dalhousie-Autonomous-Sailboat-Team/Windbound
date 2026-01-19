/**
 * @file command_dispatch.c
 *
 * @brief Command dispatching for user application.
 */

/* Module Header */
#include "command_dispatch.h"

/* System Headers */
#include "main.h"
#include "cmsis_os2.h"

/* Standard Libraries */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* User Headers */
#include "user_main.h"
#include "L1/user_uart.h"
#include "L1/pwm.h"
#include "L2/serial_parser.h"
#include "L3/angle_sensors.h"

static void Get_Angle_Handler(char arguments[MAX_COMMAND_ARGUMENTS][MAX_ARGUMENT_LEN], uint8_t arg_count);
static void Set_PWM_Handler(char arguments[MAX_COMMAND_ARGUMENTS][MAX_ARGUMENT_LEN], uint8_t arg_count);

extern osMessageQueueId_t debug_command_queueHandle;

/* Command Entry Structure */
typedef struct COMMAND_ENTRY
{
    char command_string[16];
    void (*handler_function)(char arguments[MAX_COMMAND_ARGUMENTS][MAX_ARGUMENT_LEN], uint8_t arg_count);
} Command_Entry_t;

/* Command Table */
Command_Entry_t Command_Table[] = {
    {"get_angle", Get_Angle_Handler},
    {"set_pwm", Set_PWM_Handler}, /* Placeholder for future PWM command handler */
    {"", NULL}                    /* End of Table Marker */
};

#define COMMAND_TABLE_SIZE (sizeof(Command_Table) / sizeof(Command_Entry_t))
/**
 * @brief Dispatches a command to the appropriate handler function.
 *
 * @param command The command string.
 * @param arguments The command arguments.
 * @param arg_count The number of arguments.
 */
static void Dispatch_Command(const char *command, char arguments[MAX_COMMAND_ARGUMENTS][MAX_ARGUMENT_LEN], uint8_t arg_count)
{
    for (size_t i = 0; i < COMMAND_TABLE_SIZE; i++)
    {
        if (strcmp(command, Command_Table[i].command_string) == 0)
        {
            Command_Table[i].handler_function(arguments, arg_count);
            return;
        }
    }
    char response[64];
    snprintf(response, sizeof(response), "Unknown command >%s<\n", command);
    Debug_Print_String(response);
    return;
}

static void Get_Angle_Handler(char arguments[MAX_COMMAND_ARGUMENTS][MAX_ARGUMENT_LEN], uint8_t arg_count)
{
    static char response[64];
    if (arg_count != 1)
    {
        Debug_Print_String("Invalid arg count\n");
        return;
    }
    if (strcmp(arguments[0], "mast") == 0)
    {
        uint16_t angle = Get_Mast_Angle();
        snprintf(response, sizeof(response), "MAST_ANGLE:%u\n", angle);
        Debug_Print_String(response);
    }
    else
    {
        snprintf(response, sizeof(response), "Unknown argument >%s<\n", arguments[0]);
        Debug_Print_String(response);
        return;
    }
}

static void Set_PWM_Handler(char arguments[MAX_COMMAND_ARGUMENTS][MAX_ARGUMENT_LEN], uint8_t arg_count)
{
    static char response[64];
    if (arg_count != 2)
    {
        Debug_Print_String("Invalid arg count\n");
        return;
    }

    PWM_Channel_t channel;
    if (strcmp(arguments[0], "rudder") == 0)
    {
        channel = PWM_CHANNEL_RUDDER_SERVO;
    }
    else if (strcmp(arguments[0], "mast_1") == 0)
    {
        channel = PWM_CHANNEL_MAST_1_MOTOR;
    }
    else if (strcmp(arguments[0], "mast_2") == 0)
    {
        channel = PWM_CHANNEL_MAST_2_MOTOR;
    }
#ifdef USE_FLAP_MOTOR
    else if (strcmp(arguments[0], "flap_1") == 0)
    {
        channel = PWM_CHANNEL_FLAP_1_MOTOR;
    }
    else if (strcmp(arguments[0], "flap_2") == 0)
    {
        channel = PWM_CHANNEL_FLAP_2_MOTOR;
    }
#endif
#ifdef USE_MOTOR_CHANNEL_4
    else if (strcmp(arguments[0], "flap2_1") == 0)
    {
        channel = PWM_CHANNEL_FLAP2_1_MOTOR;
    }
    else if (strcmp(arguments[0], "flap2_2") == 0)
    {
        channel = PWM_CHANNEL_FLAP2_2_MOTOR;
    }
#endif
    else
    {
        snprintf(response, sizeof(response), "Unknown channel >%s<\n", arguments[0]);
        Debug_Print_String(response);
        return;
    }

    int16_t duty_cycle = (int16_t)atoi(arguments[1]);
    PWM_SetDutyCycle(channel, duty_cycle);
}

void CommandDispatchTask(void *argument)
{
    Command_Message_t command_message;

    while (true)
    {
        /* Wait for a command message from the queue */
        if (osMessageQueueGet(
                debug_command_queueHandle,
                &command_message,
                NULL,
                osWaitForever) == osOK)
        {
            /* Dispatch the command */
            Dispatch_Command(
                command_message.command,
                command_message.arguments,
                command_message.arg_count);
        }
    }
}