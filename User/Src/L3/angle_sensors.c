/** @file angle_sensors.c
 *
 * @brief Implementation for angle sensor interfacing.
 */

/* System Headers */
#include "assert.h"
#include "main.h"
#include "cmsis_os2.h"

/* Module Header */
#include "angle_sensors.h"

#include "user_i2c.h"

#define MAST_ANGLE_READ_PERIOD_MS 1000
#define ANGLE_SENSOR_ADDRESS (0x36 << 1) /* I2C address for the angle sensor */
#define MAST_ANGLE_MUX_CHANNEL I2C_MUX_CHANNEL_4
extern osSemaphoreId_t mastAngleReadCompleteHandle;
extern osMessageQueueId_t i2c2_queueHandle;
extern osMessageQueueId_t mast_angle_queueHandle;

typedef enum
{
    ZMCO_REG = 0x00,
    ZPOS_REG = 0x01,
    MPOS_REG = 0x03,
    MANG_REG = 0x05,
    CONF_REG = 0x07,
    RAW_ANGLE_REG = 0x0C,
    ANGLE_REG = 0x0E,
    STATUS_REG = 0x0B,
    AGC_REG = 0x1A,
    MAGNITUDE_REG = 0x1B,
    BURN_REG = 0xFF,
} AngleSensor_Register_t;

/**
 * @brief Periodically reads the mast angle from the angle sensor.
 *
 * @param argument - Unused
 */
void MastAngleTask(void *argument)
{
    uint8_t angle_data[2] = {0};
    static uint8_t angle_reg = ANGLE_REG;
    bool success;
    I2C_Transaction_t transaction;

    transaction.device_address = ANGLE_SENSOR_ADDRESS;
    transaction.mux_channel = MAST_ANGLE_MUX_CHANNEL;
    transaction.operation = I2C_OP_WRITE_READ;
    transaction.write_buffer = &angle_reg;
    transaction.write_length = 1;
    transaction.read_buffer = angle_data;
    transaction.read_length = 2;
    transaction.timeout_ms = 10;
    transaction.completion_semaphore = mastAngleReadCompleteHandle;
    transaction.success = &success;

    while (true)
    {
        uint32_t current_tick = osKernelGetTickCount();
        if (osMessageQueuePut(
                i2c2_queueHandle,
                &transaction,
                0,
                0) == osOK)
        {
            osSemaphoreAcquire(mastAngleReadCompleteHandle, osWaitForever);
            if (!*transaction.success)
            {
                /* TODO: Log I2C read failure to Debug Output/EEPROM */
                continue;
            }
            uint16_t angle = (angle_data[0] << 8) | angle_data[1];
            if (osMessageQueueGetSpace(mast_angle_queueHandle) == 0)
            {
                /* If the queue is full, remove the oldest entry */
                uint16_t discarded_angle;
                osMessageQueueGet(mast_angle_queueHandle, &discarded_angle, NULL, 0);
            }
            osMessageQueuePut(
                mast_angle_queueHandle,
                &angle,
                0,
                0);
            osDelayUntil(current_tick + MAST_ANGLE_READ_PERIOD_MS);
        }
    }

    UNUSED(argument);
}