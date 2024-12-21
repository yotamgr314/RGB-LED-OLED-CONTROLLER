#include <stdlib.h>
#include <stdio.h>
#include "System/system.h"
#include "System/delay.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"

#define MODE_COLOR_DISPLAY 0
#define MODE_COMPOSITE_DISPLAY 1

typedef unsigned char DISPLAY_MODE;

static DISPLAY_MODE current_mode = MODE_COLOR_DISPLAY;
static int red_brightness = 0;
static int green_brightness = 0;
static int blue_brightness = 0;
static int current_color = 0; // 0 = RED, 1 = GREEN, 2 = BLUE

void InitializeUserHardware(void) {
    // Potentiometer setup
    TRISBbits.TRISB12 = 1;
    ANSBbits.ANSB12 = 1;
    AD1CHSbits.CH0SA = 12;
    AD1CON1bits.ADON = 1;

    // Button setup
    TRISAbits.TRISA11 = 1; // S1
    TRISAbits.TRISA12 = 1; // S2

    // LED setup
    TRISAbits.TRISA0 = 0; // Red LED
    TRISAbits.TRISA1 = 0; // Green LED
    TRISCbits.TRISC7 = 0; // Blue LED

    // PWM Configuration
    __builtin_write_OSCCONL(OSCCON & 0xBF); // Unlock PPS
    RPOR13bits.RP26R = 13; // Connect OC1 to RP26 (RA0)
    RPOR13bits.RP27R = 14; // Connect OC2 to RP27 (RA1)
    RPOR11bits.RP23R = 15; // Connect OC3 to RP23 (RC7)
    __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS

    // Configure PWM channels
    OC1CON1bits.OCM = 0b110; // Edge-aligned PWM
    OC1CON1bits.OCTSEL = 0b111; // FOSC/2
    OC1RS = 1023; // Full duty cycle
    OC1R = 0; // Initial brightness

    OC2CON1bits.OCM = 0b110; // Edge-aligned PWM
    OC2CON1bits.OCTSEL = 0b111; // FOSC/2
    OC2RS = 1023; // Full duty cycle
    OC2R = 0; // Initial brightness

    OC3CON1bits.OCM = 0b110; // Edge-aligned PWM
    OC3CON1bits.OCTSEL = 0b111; // FOSC/2
    OC3RS = 1023; // Full duty cycle
    OC3R = 0; // Initial brightness
}

int ReadPotentiometer(void) {
    AD1CON1bits.SAMP = 1; // Start sampling
    for (int i = 0; i < 1000; i++); // Wait for sampling
    AD1CON1bits.SAMP = 0; // Stop sampling

    while (!AD1CON1bits.DONE); // Wait for conversion to complete
    return ADC1BUF0;
}

void UpdateOLED(int potentiometer_value) {
    char buffer[16];
    oledC_setColumnAddressBounds(0, 95);
    oledC_setRowAddressBounds(0, 95);
    for (int col = 0; col < 96; col++) {
        for (int row = 0; row < 96; row++) {
            oledC_sendColorInt(OLEDC_COLOR_BLACK); // Clear screen
        }
    }

    if (current_mode == MODE_COLOR_DISPLAY) {
        sprintf(buffer, "Color: %s", (current_color == 0) ? "RED" : (current_color == 1) ? "GREEN" : "BLUE");
        oledC_DrawString(0, 0, 1, 1, (uint8_t *)buffer, OLEDC_COLOR_WHITE);
        sprintf(buffer, "Brightness: %d", potentiometer_value);
        oledC_DrawString(0, 16, 1, 1, (uint8_t *)buffer, OLEDC_COLOR_WHITE);
    } else {
        sprintf(buffer, "Mode: Composite");
        oledC_DrawString(0, 0, 1, 1, (uint8_t *)buffer, OLEDC_COLOR_WHITE);
        sprintf(buffer, "Pot: %d", potentiometer_value);
        oledC_DrawString(0, 16, 1, 1, (uint8_t *)buffer, OLEDC_COLOR_WHITE);
    }
}

void UpdateLEDs(int potentiometer_value) {
    if (current_mode == MODE_COLOR_DISPLAY) {
        switch (current_color) {
            case 0: red_brightness = potentiometer_value; break;
            case 1: green_brightness = potentiometer_value; break;
            case 2: blue_brightness = potentiometer_value; break;
        }
    }

    OC1R = red_brightness;
    OC2R = green_brightness;
    OC3R = blue_brightness;

    if (current_mode == MODE_COMPOSITE_DISPLAY) {
        OC1R = (red_brightness * potentiometer_value) / 1023;
        OC2R = (green_brightness * potentiometer_value) / 1023;
        OC3R = (blue_brightness * potentiometer_value) / 1023;
    }
}

void HandleButtonS1(void) {
    if (current_mode == MODE_COLOR_DISPLAY) {
        current_mode = MODE_COMPOSITE_DISPLAY;
    } else {
        current_mode = MODE_COLOR_DISPLAY;
    }
}

void HandleButtonS2(void) {
    if (current_mode == MODE_COLOR_DISPLAY) {
        current_color = (current_color + 1) % 3; // Cycle through RED -> GREEN -> BLUE
    }
}

int main(void) {
    SYSTEM_Initialize();
    InitializeUserHardware();

    int potentiometer_value = 0;
    int previous_potentiometer_value = -1;

    while (1) {
        if (PORTAbits.RA11 == 0) { // S1 pressed
            LATAbits.LATA8 = 1; // LED1 ON
            HandleButtonS1();
        } else {
            LATAbits.LATA8 = 0; // LED1 OFF
        }

        if (PORTAbits.RA12 == 0) { // S2 pressed
            LATAbits.LATA9 = 1; // LED2 ON
            HandleButtonS2();
        } else {
            LATAbits.LATA9 = 0; // LED2 OFF
        }

        potentiometer_value = ReadPotentiometer();
        if (abs(potentiometer_value - previous_potentiometer_value) > 5) {
            previous_potentiometer_value = potentiometer_value;
            UpdateOLED(potentiometer_value);
            UpdateLEDs(potentiometer_value);
        }
    }

    return 1;
}
