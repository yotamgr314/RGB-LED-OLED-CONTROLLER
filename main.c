#include <stdlib.h>
#include <stdio.h>
#include "System/system.h"
#include "System/delay.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"
#include <libpic30.h> // For __delay_ms

#define FCY 4000000 // Define instruction cycle frequency

typedef unsigned char DISPLAY_MODE;
#define MODE_COMPOSITE 0
#define MODE_COLOR 1

// Global Variables
static uint16_t red_brightness = 512;
static uint16_t green_brightness = 512;
static uint16_t blue_brightness = 512;
static uint8_t current_color = 0; // 0 = Red, 1 = Green, 2 = Blue
static DISPLAY_MODE current_mode = MODE_COMPOSITE;

// Function Declarations
void ConfigurePWM(void);
void InitializeUserHardware(void);
uint16_t ReadPotentiometer(void);
void ClearOLED(void);
void UpdateOLEDDisplay(void);
void UpdateLEDs(void);
void HandleButtonS1(void);
void HandleButtonS2(void);

// Configure PWM for LEDs
void ConfigurePWM(void) {
    // Red LED (RA0 → RP26 → OC1)
    RPOR13bits.RP26R = 13; // Assign OC1 to RP26
    OC1RS = 1023; // Set PWM period
    OC1CON2bits.SYNCSEL = 0x1F;
    OC1CON2bits.OCTRIG = 0;
    OC1CON1bits.OCTSEL = 0b111;
    OC1CON1bits.OCM = 0b110;

    // Green LED (RA1 → RP27 → OC2)
    RPOR13bits.RP27R = 14; // Assign OC2 to RP27
    OC2RS = 1023;
    OC2CON2bits.SYNCSEL = 0x1F;
    OC2CON2bits.OCTRIG = 0;
    OC2CON1bits.OCTSEL = 0b111;
    OC2CON1bits.OCM = 0b110;

    // Blue LED (RC7 → RP23 → OC3)
    RPOR11bits.RP23R = 15; // Assign OC3 to RP23
    OC3RS = 1023;
    OC3CON2bits.SYNCSEL = 0x1F;
    OC3CON2bits.OCTRIG = 0;
    OC3CON1bits.OCTSEL = 0b111;
    OC3CON1bits.OCM = 0b110;
}

// Initialize Hardware
void InitializeUserHardware(void) {
    // Configure potentiometer as input
    TRISBbits.TRISB12 = 1;
    ANSBbits.ANSB12 = 1;
    AD1CON1bits.SSRC = 0;
    AD1CON1bits.FORM = 0;
    AD1CON1bits.ASAM = 0;
    AD1CHSbits.CH0SA = 12;
    AD1CON1bits.ADON = 1;

    // Configure buttons and LEDs
    TRISAbits.TRISA11 = 1; // S1
    TRISAbits.TRISA12 = 1; // S2
    TRISAbits.TRISA8 = 0;  // LED1
    TRISAbits.TRISA9 = 0;  // LED2

    LATAbits.LATA8 = 0; // Turn off LED1
    LATAbits.LATA9 = 0; // Turn off LED2
}

// Read potentiometer value
uint16_t ReadPotentiometer(void) {
    AD1CON1bits.SAMP = 1;
    for (int i = 0; i < 1000; i++); // Delay for sampling
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE);
    return ADC1BUF0;
}

// Clear OLED Display
void ClearOLED(void) {
    uint8_t column, row;
    oledC_setColumnAddressBounds(0, 96);
    oledC_setRowAddressBounds(0, 96);
    for (column = 0; column < 96; column++) {
        for (row = 0; row < 96; row++) {
            oledC_sendColorInt(OLEDC_COLOR_BLACK);
        }
    }
}

// Update OLED Display
void UpdateOLEDDisplay(void) {
    char buffer[20];
    ClearOLED();
    if (current_mode == MODE_COMPOSITE) {
        sprintf(buffer, "Mode: Composite");
        oledC_DrawString(10, 10, 2, 2, (uint8_t *)buffer, OLEDC_COLOR_WHITE);
    } else {
        const char *colors[] = {"Red", "Green", "Blue"};
        sprintf(buffer, "Mode: Color");
        oledC_DrawString(10, 10, 2, 2, (uint8_t *)buffer, OLEDC_COLOR_WHITE);
        sprintf(buffer, "Color: %s", colors[current_color]);
        oledC_DrawString(10, 30, 2, 2, (uint8_t *)buffer, OLEDC_COLOR_WHITE);
    }
}

// Update LED Brightness
void UpdateLEDs(void) {
    uint16_t pot_value = ReadPotentiometer();
    if (current_mode == MODE_COMPOSITE) {
        OC1R = (red_brightness * pot_value) / 1023;
        OC2R = (green_brightness * pot_value) / 1023;
        OC3R = (blue_brightness * pot_value) / 1023;
    } else {
        switch (current_color) {
            case 0:
                red_brightness = pot_value;
                OC1R = red_brightness;
                break;
            case 1:
                green_brightness = pot_value;
                OC2R = green_brightness;
                break;
            case 2:
                blue_brightness = pot_value;
                OC3R = blue_brightness;
                break;
        }
    }
}

// Handle S1 Button (Mode Toggle)
void HandleButtonS1(void) {
    current_mode = (current_mode == MODE_COMPOSITE) ? MODE_COLOR : MODE_COMPOSITE;
    UpdateOLEDDisplay();
}

// Handle S2 Button (Cycle Colors)
void HandleButtonS2(void) {
    if (current_mode == MODE_COLOR) {
        current_color = (current_color + 1) % 3;
        UpdateOLEDDisplay();
    }
}

// Main Function
int main(void) {
    SYSTEM_Initialize();
    InitializeUserHardware();
    ConfigurePWM();
    UpdateOLEDDisplay();

    while (1) {
        if (!PORTAbits.RA11) { // S1 pressed
            LATAbits.LATA8 = 1; // Turn on LED1
            HandleButtonS1();
            __delay_ms(200);
            LATAbits.LATA8 = 0; // Turn off LED1
        }

        if (!PORTAbits.RA12) { // S2 pressed
            LATAbits.LATA9 = 1; // Turn on LED2
            HandleButtonS2();
            __delay_ms(200);
            LATAbits.LATA9 = 0; // Turn off LED2
        }

        UpdateLEDs();
    }

    return 0;
}
