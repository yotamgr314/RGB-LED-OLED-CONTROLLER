# RGB LED and OLED Display Controller

A C-based embedded system project for controlling RGB LED brightness and color using a potentiometer and push button, with real-time feedback displayed on an OLED screen. Designed for PIC microcontrollers, this project demonstrates PWM control, ADC reading, and OLED interfacing.

## Features

- **RGB LED Control**: Adjust the brightness of Red, Green, or Blue LEDs using a potentiometer.
- **Color Cycling**: Change the active LED color by pressing a push button.
- **OLED Feedback**: Display current potentiometer readings and active color on a connected OLED screen.
- **PWM Implementation**: Utilize PWM for smooth LED brightness transitions.
- **ADC Reading**: Read analog values from a potentiometer to determine LED brightness levels.

## Hardware Requirements

- PIC Microcontroller (PIC24FJ256GA7)
- RGB LED connected to PWM-capable pins
- Potentiometer connected to an analog input pin
- Push button connected to a digital input pin
- OLED display compatible with the `oledC` driver
- Necessary resistors and wiring for connections

## Software Components

- **`main.c`**: Contains the main loop handling input reading and output control.
- **`system.c/h`**: System initialization and configuration functions.
- **`oledDriver/`**: Drivers for OLED display operations.
- **`spiDriver/`**: SPI communication drivers for OLED interfacing.
- **`Makefile`**: Build instructions for compiling the project.

## Setup and Compilation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/yotamgr314/LC04.git
