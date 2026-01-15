/** @file angle_sensors.c
 *
 * @brief Implementation for angle sensor interfacing.
 */

/* System Headers */
#include "main.h"
#include "cmsis_os2.h"

/* Module Header */
#include "angle_sensors.h"

#include "user_i2c.h"

#define ANGLE_SENSOR_ADDRESS (0x36 << 1) /* I2C address for the angle sensor */