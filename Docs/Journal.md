# Development Journal

## Hardware Errata/Changes

- Windvane (USART3) RX and TX are swapped on the schematic/PCB. **Can be fixed in firmware.**
- UART5 TX was swapped with USART1 RX on the schematic/PCB. **Will require trace cut and bodge wire to fix.**
- Motor 1 output (labelled MAST on the PCB) is now used as a constant power source for the rudder servo. Timer 1 channel 3 is used on GPIO 4 to control rudder servo PWM.
- Motor 2 output (labelled RUDDER on the PCB) is now used to control the Mast motor.
- Motor 3 output (labelled FLAP2 on the PCB) is used to control the back flap motor.
- Silkscreen on/off switch labels are reversed.
