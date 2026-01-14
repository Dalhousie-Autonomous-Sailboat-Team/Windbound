/** @file user_i2c.h
 *
 * @brief Driver code for interfacing with I2C peripherals.
 */

#ifndef USER_I2C_H
#define USER_I2C_H

#include "cmsis_os2.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    I2C_OP_READ,
    I2C_OP_WRITE,
    I2C_OP_WRITE_READ,
} I2C_Operation_t;

typedef struct
{
    uint16_t device_address;
    I2C_Operation_t operation;
    uint8_t *write_buffer;
    size_t write_length;
    uint8_t *read_buffer;
    size_t read_length;
    uint32_t timeout_ms;
    osSemaphoreId_t completion_semaphore;
    bool success;
} I2C_Transaction_t;

#endif /* USER_I2C_H */

/*** end of file ***/