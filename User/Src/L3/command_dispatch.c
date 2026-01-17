/* Module Header */
#include "command_dispatch.h"

/* Standard Libraries */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* User Defined Libraries */
#include "L1/user_uart.h"
#include "L2/serial_parser.h"
#include "L3/angle_sensors.h"
#include "app_freertos.h"

static void Get_Angle_Handler(char arguments[MAX_COMMAND_ARGUMENTS][MAX_ARGUMENT_LEN], uint8_t arg_count);

/* Command Entry Structure */
typedef struct COMMAND_ENTRY
{
    char command_string[16];
    void (*handler_function)(char arguments[MAX_COMMAND_ARGUMENTS][MAX_ARGUMENT_LEN], uint8_t arg_count);
} Command_Entry_t;

/* Command Table */
Command_Entry_t Command_Table[] = {
    {"get_angle", Get_Angle_Handler},
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
    /* Command not found - handle as needed */
    return;
}

static void Get_Angle_Handler(char arguments[MAX_COMMAND_ARGUMENTS][MAX_ARGUMENT_LEN], uint8_t arg_count)
{
    if (arg_count < 1)
    {
        /* Handle error: insufficient arguments */
        return;
    }
    if (strcmp(arguments[0], "mast") == 0)
    {
        uint16_t angle = Get_Mast_Angle();
        char response[32];
        snprintf(response, sizeof(response), "MAST_ANGLE:%u\n", angle);
        Debug_Print_String(response);
    }
    else
    {
        /* Handle error: unknown angle type */
        return;
    }
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