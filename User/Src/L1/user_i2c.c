/**
 * @file user_i2c.c
 *
 * @brief Driver code for interfacing with I2C peripherals.
 */

/* System Headers */
#include "assert.h"
#include "main.h"
#include "cmsis_os2.h"

/* Module Header */
#include "user_i2c.h"

extern osMessageQueueId_t i2c1_queueHandle;
extern osMessageQueueId_t i2c2_queueHandle;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

#define I2C_FLAG_DONE (1U << 0)
#define I2C_FLAG_ERROR (1U << 1)

osThreadId_t i2c1_manager_thread;
osThreadId_t i2c2_manager_thread;

void I2CManagerTask(void *argument)
{
    /* Task Initialization */
    I2C_HandleTypeDef *hi2c;
    osMessageQueueId_t queueHandle;
    /* Determine which I2C peripheral and transaction queue to use based on the argument */
    switch ((uint8_t)(uintptr_t)argument)
    {
    case 1:
        hi2c = &hi2c1;
        queueHandle = i2c1_queueHandle;
        i2c1_manager_thread = osThreadGetId();
        break;
    case 2:
        hi2c = &hi2c2;
        queueHandle = i2c2_queueHandle;
        i2c2_manager_thread = osThreadGetId();
        break;
    default:
        osThreadTerminate(NULL);
        ASSERT(0 && "Invalid I2C task", "I2CManagerTask received invalid argument");
        return;
        break;
    }

    while (true)
    {
        /* Wait for an I2C transaction from the queue */
        I2C_Transaction_t transaction;

        if (osMessageQueueGet(queueHandle, &transaction, NULL, osWaitForever) == osOK)
        {
            HAL_StatusTypeDef status = HAL_OK;
            uint32_t flags;

            osThreadFlagsClear(I2C_FLAG_DONE | I2C_FLAG_ERROR);

            /* Start the I2C transaction based on the operation type */
            switch (transaction.operation)
            {
            case I2C_OP_WRITE:
                status = HAL_I2C_Master_Transmit_IT(
                    hi2c,
                    transaction.device_address,
                    transaction.write_buffer,
                    transaction.write_length);
                break;

            case I2C_OP_READ:
                status = HAL_I2C_Master_Receive_IT(
                    hi2c,
                    transaction.device_address,
                    transaction.read_buffer,
                    transaction.read_length);
                break;

            case I2C_OP_WRITE_READ:
                status = HAL_I2C_Master_Transmit_IT(
                    hi2c,
                    transaction.device_address,
                    transaction.write_buffer,
                    transaction.write_length);
                break;

            default:
                status = HAL_ERROR;
                break;
            }

            if (status != HAL_OK)
            {
                transaction.success = false;
                osSemaphoreRelease(transaction.completion_semaphore);
                continue;
            }

            /* Wait for ISR completion */
            flags = osThreadFlagsWait(
                I2C_FLAG_DONE | I2C_FLAG_ERROR,
                osFlagsWaitAny,
                transaction.timeout_ms);

            if ((flags & I2C_FLAG_ERROR) != 0U)
            {
                transaction.success = false;
                osSemaphoreRelease(transaction.completion_semaphore);
                continue;
            }

            /* Handle second phase of WRITE_READ */
            if (transaction.operation == I2C_OP_WRITE_READ)
            {
                osThreadFlagsClear(I2C_FLAG_DONE | I2C_FLAG_ERROR);

                status = HAL_I2C_Master_Receive_IT(
                    hi2c,
                    transaction.device_address,
                    transaction.read_buffer,
                    transaction.read_length);

                if (status != HAL_OK)
                {
                    transaction.success = false;
                    osSemaphoreRelease(transaction.completion_semaphore);
                    continue;
                }

                flags = osThreadFlagsWait(
                    I2C_FLAG_DONE | I2C_FLAG_ERROR,
                    osFlagsWaitAny,
                    transaction.timeout_ms);
            }

            transaction.success = ((flags & I2C_FLAG_DONE) != 0U);
            osSemaphoreRelease(transaction.completion_semaphore);
        }
    }
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == hi2c1.Instance)
    {
        osThreadFlagsSet(i2c1_manager_thread, I2C_FLAG_DONE);
    }
    else if (hi2c->Instance == hi2c2.Instance)
    {
        osThreadFlagsSet(i2c2_manager_thread, I2C_FLAG_DONE);
    }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == hi2c1.Instance)
    {
        osThreadFlagsSet(i2c1_manager_thread, I2C_FLAG_DONE);
    }
    else if (hi2c->Instance == hi2c2.Instance)
    {
        osThreadFlagsSet(i2c2_manager_thread, I2C_FLAG_DONE);
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == hi2c1.Instance)
    {
        osThreadFlagsSet(i2c1_manager_thread, I2C_FLAG_ERROR);
        return;
    }
    else if (hi2c->Instance == hi2c2.Instance)
    {
        osThreadFlagsSet(i2c2_manager_thread, I2C_FLAG_ERROR);
        return;
    }
}
