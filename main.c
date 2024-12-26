#include <stdlib.h>
#include <stdio.h>
#include "System/system.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"

// Define modes and states
#define COLOR_DISPLAY_MODE 1
#define COMPOSITE_DISPLAY_MODE !COLOR_DISPLAY_MODE

typedef enum
{
    RED,
    GREEN,
    BLUE
} COLOR;

// Global variables
static uint16_t current_background_color; // Declare without initialization
static COLOR current_color = RED;         // Start with RED as the default color

// Function prototypes
void InitializeHardware(void);
void ConfigurePWM(void);
void SetLEDIntensity(COLOR color, int value);
void HandleButtonS2(void);
void DelayMs(unsigned int milliseconds);
void UpdatePotentiometerValue(int *potentiometer_value, int *previous_potentiometer_value);

// Delay function
void DelayMs(unsigned int milliseconds)
{
    while (milliseconds--)
    {
        for (volatile unsigned int i = 0; i < 4000; i++)
            ;
    }
}

// Initialize ADC, buttons, and LEDs --> SAME INTILIZE FUNCTION FROM LC04, JUST ADDED THE CONFIGURE GPIOS FOR RGB.
void InitializeHardware(void)
{
    // Configure Buttons
    TRISAbits.TRISA11 = 1; // S1 input
    TRISAbits.TRISA12 = 1; // S2 input

    // Configure GPIOs for RGB
    TRISAbits.TRISA0 = 0; // RA0 - Red LED (PWM) output
    TRISAbits.TRISA1 = 0; // RA1 - Green LED (PWM) output
    TRISCbits.TRISC7 = 0; // RC7 - Blue LED (PWM) output

    // Configure potentiometer input
    TRISBbits.TRISB12 = 1; // Potentiometer input
    ANSBbits.ANSB12 = 1;   // Analog input enabled

    // Configure LED1, LED2 as output
    TRISAbits.TRISA8 = 0; // LED1 output
    TRISAbits.TRISA9 = 0; // LED2 output

    // Configure ADC setup for potentiometer
    AD1CON1bits.SSRC = 0;
    AD1CON1bits.FORM = 0;
    AD1CON1bits.ASAM = 0;
    AD1CON1bits.MODE12 = 0;
    AD1CON2 = 0x00;
    AD1CON3bits.ADCS = 0xFF;
    AD1CON3bits.SAMC = 0x10;
    AD1CHSbits.CH0SA = 8;
    AD1CON1bits.ADON = 1;
}

// Configure PWM for RGB LEDs
void ConfigurePWM(void)
{
    // Red LED (RA0 → RP26) --> COPY PASTE FROM THE RGB LED PDF
    RPOR13 = 13;                // Assign the Output Compare 1 (OC1) function to pin RP26 (RA0).
    OC1RS = 1023;               // Set the PWM period to 1023, defining the full duty cycle.
    OC1CON2bits.SYNCSEL = 0x1F; // Synchronize the PWM with itself (self-sync mode).
    OC1CON2bits.OCTRIG = 0;     // Ensure synchronous operation (no trigger required).
    OC1CON1bits.OCTSEL = 0b111; // Select the clock source as FOSC/2 (system clock divided by 2).
    OC1CON1bits.OCM = 0b110;    // Set PWM mode as edge-aligned PWM.
    OC1CON2bits.TRIGSTAT = 1;   // Start the PWM output.

    // Green LED (RA1 → RP27) -->  INSPIRED FROM THE RED RGB PDF.
    RPOR13bits.RP27R = 14;      // Assign the Output Compare 2 (OC2) function to pin RP27 (RA1).
    OC2RS = 1023;               // Set the PWM period to 1023 for the green LED.
    OC2CON2bits.SYNCSEL = 0x1F; // Synchronize the PWM with itself (self-sync mode).
    OC2CON2bits.OCTRIG = 0;     // Ensure synchronous operation (no trigger required).
    OC2CON1bits.OCTSEL = 0b111; // Select the clock source as FOSC/2 (system clock divided by 2).
    OC2CON1bits.OCM = 0b110;    // Set PWM mode as edge-aligned PWM.
    OC2CON2bits.TRIGSTAT = 1;   // Start the PWM output.

    // Blue LED (RC7 → RP23) --> INSPIRED FROM THE RED RGB PDF.
    RPOR11bits.RP23R = 15;      // Assign the Output Compare 3 (OC3) function to pin RP23 (RC7).
    OC3RS = 1023;               // Set the PWM period to 1023 for the blue LED.
    OC3CON2bits.SYNCSEL = 0x1F; // Synchronize the PWM with itself (self-sync mode).
    OC3CON2bits.OCTRIG = 0;     // Ensure synchronous operation (no trigger required).
    OC3CON1bits.OCTSEL = 0b111; // Select the clock source as FOSC/2 (system clock divided by 2).
    OC3CON1bits.OCM = 0b110;    // Set PWM mode as edge-aligned PWM.
    OC3CON2bits.TRIGSTAT = 1;   // Start the PWM output.
}

// Set brightness for a specific LED
void SetLEDIntensity(COLOR color, int value)
{
    // Turn off all LEDs first
    OC1R = 0; // Turn off the Red LED by setting its duty cycle to 0.
    OC2R = 0; // Turn off the Green LED by setting its duty cycle to 0.
    OC3R = 0; // Turn off the Blue LED by setting its duty cycle to 0.

    // Turn on the specified LED
    switch (color)
    {
    case RED:
        OC1R = value; // Set the duty cycle for the Red LED to the given value.
        break;
    case GREEN:
        OC2R = value; // Set the duty cycle for the Green LED to the given value.
        break;
    case BLUE:
        OC3R = value; // Set the duty cycle for the Blue LED to the given value.
        break;
    }
}

void UpdatePotentiometerValue(int *potentiometer_value, int *previous_potentiometer_value) // COPY PASTE FROM LC04
{
    static int last_potentiometer = -1;
    char text_buffer[10];

    if (last_potentiometer != *potentiometer_value)
    {
        sprintf(text_buffer, "P:%3d", last_potentiometer);
        oledC_DrawString(10, 10, 2, 2, (uint8_t *)text_buffer, current_background_color);

        sprintf(text_buffer, "P:%3d", *potentiometer_value);
        oledC_DrawString(10, 10, 2, 2, (uint8_t *)text_buffer, OLEDC_COLOR_TURQUOISE);

        last_potentiometer = *potentiometer_value;
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
    int previous_potentiometer = -1;
    int potentiometer = 0;

    SYSTEM_Initialize();
    InitializeHardware();
    ConfigurePWM();

    // Initialize global variable
    current_background_color = OLEDC_COLOR_BLACK;

    // Set initial LED state
    SetLEDIntensity(current_color, 512); // Start with Red at 50%

    while (1)
    {
        HandleButtonS2(); // Check for button press and update color

        // Read potentiometer value
        AD1CON1bits.SAMP = 1;
        for (int i = 0; i < 1000; i++)
            ;
        AD1CON1bits.SAMP = 0;

        while (!AD1CON1bits.DONE)
            ;
        potentiometer = ADC1BUF0;

        if (abs(potentiometer - previous_potentiometer) > 8)
        {
            UpdatePotentiometerValue(&potentiometer, &previous_potentiometer);
            SetLEDIntensity(current_color, potentiometer / 4); // Scale to 10-bit PWM range
            previous_potentiometer = potentiometer;
        }
    }

    return 1;
}
