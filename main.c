#include <stdlib.h>
#include <stdio.h>
#include "System/system.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"

// Define modes and states
typedef enum
{
    RED,
    GREEN,
    BLUE
} COLOR;

// Global variables
static COLOR current_color = RED;

// Function prototypes
void InitializeHardware(void);
void ConfigurePWM(void);
void SetLEDIntensity(COLOR color, int value);
void HandleButtonS2(void);
void DelayMs(unsigned int milliseconds);

// Delay function
void DelayMs(unsigned int milliseconds)
{
    while (milliseconds--)
    {
        for (volatile unsigned int i = 0; i < 4000; i++)
            ;
    }
}

// Initialize ADC, buttons, and LEDs
void InitializeHardware(void)
{
    // Buttons
    TRISAbits.TRISA11 = 1; // sets S1 as input
    TRISAbits.TRISA12 = 1; // stes S2 as input

    // GPIOs for LEDs
    TRISAbits.TRISA0 = 0; // sets Ra0 - Red LED (PWM) as output.
    TRISAbits.TRISA1 = 0; // sets Ra1 - Green LED (PWM) as output.
    TRISCbits.TRISC7 = 0; // sets Ra7 - Blue LED (PWM) as output.
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
    // Turn off all LEDs first
    OC1R = 0; // Red
    OC2R = 0; // Green
    OC3R = 0; // Blue

    // Turn on the specified LED
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

// Handle button S2 for color transitions
void HandleButtonS2(void)
{
    static int last_button_state = 1; // 1 = released, 0 = pressed
    int current_button_state = PORTAbits.RA12;

    if (current_button_state == 0 && last_button_state == 1)
    {
        // Button press detected (falling edge)
        current_color = (current_color == BLUE) ? RED : (COLOR)(current_color + 1);
        SetLEDIntensity(current_color, 512); // Set to 50% brightness
        DelayMs(50);                         // Debounce delay
    }

    last_button_state = current_button_state;
}

// Main function
int main(void)
{
    SYSTEM_Initialize();
    InitializeHardware();
    ConfigurePWM();

    // Set initial LED state
    SetLEDIntensity(current_color, 512); // Start with Red at 50%

    while (1)
    {
        HandleButtonS2(); // Check for button press and update color
    }

    return 1;
}
