#include <stdlib.h>
#include <stdio.h>
#include "System/system.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"

// Define modes and states
typedef enum
{
    COLOR_DISPLAY,
    COMPOSITE_DISPLAY
} DISPLAY_MODE;
typedef enum
{
    RED,
    GREEN,
    BLUE
} COLOR;

// Global variables for brightness levels and states
static int red_brightness = 512;
static int green_brightness = 512;
static int blue_brightness = 512;
static DISPLAY_MODE current_mode = COLOR_DISPLAY;
static COLOR current_color = RED;
static int potentiometer_last_value = -1; // To track potentiometer matching stored brightness

// Function prototypes
void InitializeHardware(void);
void ConfigurePWM(void);
void SetLEDIntensity(COLOR color, int value);
void HandleButtons(void);
void UpdateOLED(void);
void ToggleMode(void);
void CycleColor(void);
int ReadPotentiometer(void);
void UpdateCompositeBrightness(int potentiometer);
void UpdateColorDisplayBrightness(int potentiometer);
void oledC_clearScreen(void);

// OLED clear screen function
void oledC_clearScreen(void)
{
    uint8_t column, row;

    oledC_setColumnAddressBounds(0, 96);
    oledC_setRowAddressBounds(0, 96);

    for (column = 0; column < 96; column++)
    {
        for (row = 0; row < 96; row++)
        {
            oledC_sendColorInt(OLEDC_COLOR_BLACK);
        }
    }
}

// Initialize ADC, buttons, and LEDs
void InitializeHardware(void)
{
    // Potentiometer (AN12)
    TRISBbits.TRISB12 = 1;
    ANSBbits.ANSB12 = 1;
    AD1CON1bits.SSRC = 0;
    AD1CON1bits.FORM = 0;
    AD1CON1bits.ASAM = 0;
    AD1CON1bits.MODE12 = 0;
    AD1CON2 = 0x00;
    AD1CON3bits.ADCS = 0xFF;
    AD1CHSbits.CH0SA = 12; // AN12
    AD1CON1bits.ADON = 1;

    // Buttons
    TRISAbits.TRISA11 = 1; // S1
    TRISAbits.TRISA12 = 1; // S2

    // LED Indicators
    TRISAbits.TRISA8 = 0; // LED1
    TRISAbits.TRISA9 = 0; // LED2
}

// Configure PWM for RGB LEDs
void ConfigurePWM(void)
{
    // Red LED (RA0 → RP26)
    RPOR13 = 13; // OC1
    OC1RS = 1023;
    OC1CON2bits.SYNCSEL = 0x1F;
    OC1CON1bits.OCTSEL = 0b111;
    OC1CON1bits.OCM = 0b110;
    OC1CON2bits.TRIGSTAT = 1;

    // Green LED (RA1 → RP27)
    RPOR13bits.RP27R = 14; // OC2
    OC2RS = 1023;
    OC2CON2bits.SYNCSEL = 0x1F;
    OC2CON1bits.OCTSEL = 0b111;
    OC2CON1bits.OCM = 0b110;
    OC2CON2bits.TRIGSTAT = 1;

    // Blue LED (RC7 → RP23)
    RPOR11bits.RP23R = 15; // OC3
    OC3RS = 1023;
    OC3CON2bits.SYNCSEL = 0x1F;
    OC3CON1bits.OCTSEL = 0b111;
    OC3CON1bits.OCM = 0b110;
    OC3CON2bits.TRIGSTAT = 1;
}

// Set brightness for a specific LED
void SetLEDIntensity(COLOR color, int value)
{
    switch (color)
    {
    case RED:
        OC1R = value;
        break;
    case GREEN:
        OC2R = value;
        break;
    case BLUE:
        OC3R = value;
        break;
    }
}

// Read potentiometer value
int ReadPotentiometer(void)
{
    AD1CON1bits.SAMP = 1;
    for (int i = 0; i < 1000; i++)
        ;
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE)
        ;
    return ADC1BUF0;
}

// Update brightness in composite mode
void UpdateCompositeBrightness(int potentiometer)
{
    float percentage = potentiometer / 1023.0;
    SetLEDIntensity(RED, red_brightness * percentage);
    SetLEDIntensity(GREEN, green_brightness * percentage);
    SetLEDIntensity(BLUE, blue_brightness * percentage);
}

// Update brightness in color display mode
void UpdateColorDisplayBrightness(int potentiometer)
{
    int *current_brightness;
    switch (current_color)
    {
    case RED:
        current_brightness = &red_brightness;
        break;
    case GREEN:
        current_brightness = &green_brightness;
        break;
    case BLUE:
        current_brightness = &blue_brightness;
        break;
    }

    if (potentiometer == *current_brightness || potentiometer_last_value == *current_brightness)
    {
        *current_brightness = potentiometer;
        SetLEDIntensity(current_color, potentiometer);
    }

    potentiometer_last_value = potentiometer;
}

// Toggle between modes
void ToggleMode(void)
{
    current_mode = (current_mode == COLOR_DISPLAY) ? COMPOSITE_DISPLAY : COLOR_DISPLAY;
    potentiometer_last_value = -1; // Reset tracking for potentiometer
}

// Cycle through colors
void CycleColor(void)
{
    if (current_mode == COLOR_DISPLAY)
    {
        current_color = (current_color == RED) ? GREEN : (current_color == GREEN) ? BLUE
                                                                                  : RED;
        potentiometer_last_value = -1; // Reset tracking for potentiometer
    }
}

// Update OLED display
void UpdateOLED(void)
{
    oledC_clearScreen();

    if (current_mode == COLOR_DISPLAY)
    {
        const char *color_name = (current_color == RED) ? "RED" : (current_color == GREEN) ? "GREEN"
                                                                                           : "BLUE";
        char text[20];
        sprintf(text, "%s: %d", color_name,
                (current_color == RED) ? red_brightness : (current_color == GREEN) ? green_brightness
                                                                                   : blue_brightness);
        oledC_DrawString(10, 10, 2, 2, (uint8_t *)text, OLEDC_COLOR_WHITE);
    }
    else
    {
        oledC_DrawString(10, 10, 2, 2, (uint8_t *)"COMPOSITE", OLEDC_COLOR_WHITE);
    }
}

// Handle button presses
void HandleButtons(void)
{
    if (PORTAbits.RA11 == 0)
    {                       // S1 pressed
        LATAbits.LATA8 = 1; // LED1 ON
        ToggleMode();
    }
    else
    {
        LATAbits.LATA8 = 0; // LED1 OFF
    }

    if (PORTAbits.RA12 == 0)
    {                       // S2 pressed
        LATAbits.LATA9 = 1; // LED2 ON
        CycleColor();
    }
    else
    {
        LATAbits.LATA9 = 0; // LED2 OFF
    }
}

// Main function
int main(void)
{
    SYSTEM_Initialize();
    InitializeHardware();
    ConfigurePWM();

    while (1)
    {
        int potentiometer = ReadPotentiometer();
        HandleButtons();

        if (current_mode == COLOR_DISPLAY)
        {
            UpdateColorDisplayBrightness(potentiometer);
        }
        else
        {
            UpdateCompositeBrightness(potentiometer);
        }

        UpdateOLED();
    }

    return 1;
}
