# Development Journal

## Hardware Errata/Changes

- Windvane (USART3) RX and TX are swapped on the schematic/PCB. **Can be fixed in firmware.**
- UART5 TX was swapped with USART1 RX on the schematic/PCB. **Will require trace cut and bodge wire to fix.**
- Motor 1 output (labelled MAST on the PCB) is now used as a constant power source for the rudder servo. Timer 1 channel 3 is used on GPIO 4 to control rudder servo PWM.
- Motor 2 output (labelled RUDDER on the PCB) is now used to control the Mast motor.
- Motor 3 output (labelled FLAP2 on the PCB) is used to control the back flap motor.
- Silkscreen on/off switch labels are reversed.

## Journal Entries

### 2026-01-13

Start of Windbound project. Forked from Nautono V3 firmware repo.

~~The sleep mode implementation from the Nautono repo seemed to have unnecessary calls for disabling and enabling the systick timer, as these are handled in the FreeRTOS port layer. Removed these calls in `PreSleepProcessing` and `PostSleepProcessing`. Now these functions only toggle GPIO pins to indicate when the system is entering and exiting sleep mode.~~

Apparently, even though the FreeRTOS port layer adds assembly calls to the wait for interrupt instruction, the systick timer is not automatically disabled/enabled. Re-added the HAL_SuspendTick() call to PreSleepProcessing to disable the systick timer when entering sleep mode fixed the issue of the systick interrupt waking the MCU immediately after entering sleep mode. However, the MCU still wakes up every 200 ms.

There seems to be a typo in the HAL drivers where an expression is wrapped twice in parentheses, causing a warning. This is in a generated file, so it cannot be fixed directly. The warning was suppressed in the clang cmake file.

I am currently using the HSE (I think? That is what I selected in CubeMX, but I have not verified. For all I know it is failing over to the HSI anyway) as the clock source. The HSE is running at 25 MHz, and according to the datasheet, this consumes around 5 mA on avaerage. I should consider switching to the HSI unless the HSE is absolutely necessary for some reason.