/** @file serial_parser.h
 *
 * @brief Router code for parsing serial data.
 */

#ifndef USER_SERIAL_PARSER_H
#define USER_SERIAL_PARSER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_COMMAND_ARGUMENTS 4
#define MAX_COMMAND_LEN 32
#define MAX_ARGUMENT_LEN 32

typedef struct
{
    char command[MAX_COMMAND_LEN];
    char arguments[MAX_COMMAND_ARGUMENTS][MAX_ARGUMENT_LEN];
    uint8_t arg_count;
} Command_Message_t;

#endif /* USER_SERIAL_PARSER_H */

/*** end of file ***/