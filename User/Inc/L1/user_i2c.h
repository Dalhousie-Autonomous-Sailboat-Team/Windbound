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

typedef enum
{
    I2C_MUX_NO_CHANNEL = 0x00,
    I2C_MUX_CHANNEL_0 = 0x01,
    I2C_MUX_CHANNEL_1 = 0x02,
    I2C_MUX_CHANNEL_2 = 0x04,
    I2C_MUX_CHANNEL_3 = 0x08,
    I2C_MUX_CHANNEL_4 = 0x10,
    I2C_MUX_CHANNEL_5 = 0x20,
    I2C_MUX_CHANNEL_6 = 0x40,
    I2C_MUX_CHANNEL_7 = 0x80,
} I2C_Mux_Channel_t;

typedef struct
{
    uint16_t device_address;
    I2C_Mux_Channel_t mux_channel;
    I2C_Operation_t operation;
    uint8_t *write_buffer;
    size_t write_length;
    uint8_t *read_buffer;
    size_t read_length;
    uint32_t timeout_ms;
    osSemaphoreId_t completion_semaphore;
    bool *success;
} I2C_Transaction_t;

#endif /* USER_I2C_H */

/*** end of file ***/