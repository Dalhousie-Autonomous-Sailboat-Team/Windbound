# Development Journal

## Hardware Errata/Changes

- Windvane (USART3) RX and TX are swapped on the schematic/PCB. **Can be fixed in firmware.**
- UART5 TX was swapped with USART1 RX on the schematic/PCB. **Will require trace cut and bodge wire to fix.**
- Motor 1 output (labelled MAST on the PCB) is now used as a constant power source for the rudder servo. Timer 1 channel 3 is used on GPIO 4 to control rudder servo PWM.
- Motor 2 output (labelled RUDDER on the PCB) is now used to control the Mast motor.
- Motor 3 output (labelled FLAP2 on the PCB) is used to control the back flap motor.
- Silkscreen on/off switch labels are reversed.

## Journal Entries

### 2026-01-13 - Zach

Start of Windbound project. Forked from Nautono V3 firmware repo.

~~The sleep mode implementation from the Nautono repo seemed to have unnecessary calls for disabling and enabling the systick timer, as these are handled in the FreeRTOS port layer. Removed these calls in `PreSleepProcessing` and `PostSleepProcessing`. Now these functions only toggle GPIO pins to indicate when the system is entering and exiting sleep mode.~~

Apparently, even though the FreeRTOS port layer adds assembly calls to the wait for interrupt instruction, the systick timer is not automatically disabled/enabled. Re-added the HAL_SuspendTick() call to PreSleepProcessing to disable the systick timer when entering sleep mode fixed the issue of the systick interrupt waking the MCU immediately after entering sleep mode. However, the MCU still wakes up every 200 ms.

There seems to be a typo in the HAL drivers where an expression is wrapped twice in parentheses, causing a warning. This is in a generated file, so it cannot be fixed directly. The warning was suppressed in the clang cmake file.

I am currently using the HSE (I think? That is what I selected in CubeMX, but I have not verified. For all I know it is failing over to the HSI anyway) as the clock source. The HSE is running at 25 MHz, and according to the datasheet, this consumes around 5 mA on avaerage. I should consider switching to the HSI unless the HSE is absolutely necessary for some reason.

### 2026-01-14 - Zach

Before going into more of that level of optimization, I should focus on getting an MVP version of the firmware working first. I think an MVP should include:

- Communication with Raspberry Pi over UART7 using the defined protocol
- Communication with Radio over UART8 using the defined protocol
- Reading Windvane data over USART3 - NMEA 0183 Protocol Listener
- Reading GPS Data over USART2
- Reading Mast Angle over I2C2 - Through I2C Multiplexer
- Mast Motor Actuation using Timer 2 Channel 1 and 2 PWM Outputs
- Rudder Servo Actuation using Timer 1 Channel 3 PWM Output

**I2C Implementation:**

- Created a basic I2C manager task that handles I2C transactions in a queue. This task will service I2C requests from other tasks to avoid bus contention.
- Created a user_i2c driver with functions to initialize the I2C peripherals and to create and submit I2C transactions to the I2C manager task.
