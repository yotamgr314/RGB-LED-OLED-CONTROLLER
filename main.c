#include <stdlib.h> //PDF EX3 COPY PASTE
#include <stdio.h> //PDF EX3 COPY PASTE
#include "System/system.h" //PDF EX3 COPY PASTE
#include "System/delay.h" //PDF EX3 COPY PASTE
#include "oledDriver/oledC.h" //PDF EX3 COPY PASTE
#include "oledDriver/oledC_colors.h" //PDF EX3 COPY PASTE
#include "oledDriver/oledC_shapes.h" //PDF EX3 COPY PASTE

typedef unsigned char DISPLAY_MODE;
#define MODE_NORMAL 0
#define MODE_INVERSE !MODE_NORMAL

static uint16_t current_background_color;

void InitializeUserHardware(void) {
    TRISBbits.TRISB12 = 1; //PDF EX3 COPY PASTE NOTE:Set RB12 as input, 1 for input. 0 for output.
    ANSBbits.ANSB12 = 1;  //PDF EX3 COPY PASTE NOTE:ANSB (Analog select register) - Configure the RB12 Bit as an analog input. By default, all pins are digital unless explicitly configured as analog
    AD1CON1bits.SSRC = 0; //PDF EX3 COPY PASTE NOTE:Manual sample mode
    AD1CON1bits.FORM = 0; //PDF EX3 COPY PASTE NOTE: Selects the output format of the ADC result (integer, signed integer, fractional) 0 for unsigned int, 1 for signed, and 2 for fractional
    AD1CON1bits.ASAM = 0; //PDF EX3 COPY PASTE NOTE: Enables (1) or disables (0) automatic sampling after a conversion is complete.
    AD1CON1bits.ADSIDL = 0;//PDF EX3 COPY PASTE
    AD1CON1bits.DMABM = 0;//PDF EX3 COPY PASTE
    AD1CON1bits.DMAEN = 0;//PDF EX3 COPY PASTE
    AD1CON1bits.MODE12 = 0;//PDF EX3 COPY PASTE

    AD1CON2 = 0x00; // PDF EX3 COPY PASTE
    AD1CON3bits.ADCS = 0xFF;//PDF EX3 COPY PASTE
    AD1CON3bits.SAMC = 0x10;//PDF EX3 COPY PASTE
    AD1CON3bits.ADRC = 0x00;//PDF EX3 COPY PASTE
    AD1CON3bits.EXTSAM = 0x00;//PDF EX3 COPY PASTE
    AD1CON3bits.PUMPEN = 0x00;//PDF EX3 COPY PASTE

    TRISAbits.TRISA11 = 1; // PDF EX4 COPY PASTE  NOTE: RA11 which is the bit that mapped to S1 buttonm and it is configured as input.via TristA(1 = input).
    TRISAbits.TRISA12 = 1; // PDF EX4 COPY PASTE NOTE: RA12 which is the bit that mapped to S2 button, and it is configured as input.via TristA (1 = input).
    TRISAbits.TRISA8 = 0;// PDF EX4 COPY PASTE  NOTE: RA8 bit which is the bit that mapped to LED1, and it is configured as output. via TristA (0 = output).
    TRISAbits.TRISA9 = 0;// PDF EX4 COPY PASTE  NOTE: RA9 bit which is the bit that mapped to LED2, and it is configured as output. via TristA.(0 = output)

    AD1CHSbits.CH0SA = 8;// PDF EX3 COPY PASTE 
    AD1CON1bits.ADON = 1;// PDF EX3 COPY PASTE NOTE-Turn on the A/D converter, 0 for off, and 1 for on.
}


static void ClearOLED(void) //COPIED FROM OLED_EXAMPLE.C HE GAVE US NOTE: Clears the Entire Oled screen with the background Color.
{
    uint8_t column;
    uint8_t row;
    oledC_setColumnAddressBounds(0, 96);
    oledC_setRowAddressBounds(0, 96);
    for (column = 0; column < 96; column++) {
        for (row = 0; row < 96; row++) {
            oledC_sendColorInt(current_background_color);
        }
    }
}


static void SetOLEDBackground(uint16_t color) //COPIED FROM OLED_EXAMPLE.C HE GAVE US NOTE: sets the background color of the oled screen.
{
    if (current_background_color != color) {
        current_background_color = color;
        ClearOLED();
    }
}



void ToggleDisplayMode(int *counter, int *potentiometer_value, int *previous_potentiometer_value, DISPLAY_MODE *current_mode)
{
    if (*current_mode == MODE_NORMAL) 
    {
        oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_ON, NULL, 0);//PDF EX4 COPY PASTE
    } else{
        oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_INVERSE, NULL, 0); //PDF EX4 COPY PASTE NOTE - lecturer said we must used oledC_sendCommand to avoid flickering
    }

    *current_mode = !(*current_mode);

    UpdateDisplayCounter(counter);
    UpdatePotentiometerValue(potentiometer_value, previous_potentiometer_value);
}



void UpdateDisplayCounter(int *new_counter) 
{
    static int last_counter = -1;
    char text_buffer[10];

    if (last_counter != *new_counter) 
    {
        sprintf(text_buffer, "S:%3d", last_counter);
        oledC_DrawString(10, 60, 2, 2, (uint8_t *)text_buffer, current_background_color); //PDF EX4 COPY PASTE

        sprintf(text_buffer, "S:%3d", *new_counter);
        oledC_DrawString(10, 60, 2, 2, (uint8_t *)text_buffer, OLEDC_COLOR_ROYALBLUE);//PDF EX4 COPY PASTE

        last_counter = *new_counter;
    }
}


// updates oled with the current potentiometer value.
void UpdatePotentiometerValue(int *potentiometer_value, int *previous_potentiometer_value) 
{
    static int last_potentiometer = -1;
    char text_buffer[10];

    if (last_potentiometer != *potentiometer_value) 
    {
        sprintf(text_buffer, "P:%3d", last_potentiometer);
        oledC_DrawString(10, 10, 2, 2, (uint8_t *)text_buffer, current_background_color); //PDF EX4 COPY PASTE

        sprintf(text_buffer, "P:%3d", *potentiometer_value);
        oledC_DrawString(10, 10, 2, 2, (uint8_t *)text_buffer, OLEDC_COLOR_TURQUOISE); //PDF EX4 COPY PASTE

        last_potentiometer = *potentiometer_value;
    }
}



int main(void) 
{

    int display_counter = 0;
    int potentiometer = 0;
    int previous_potentiometer = -1;

    DISPLAY_MODE current_mode = MODE_INVERSE;


    // initialize the system
    SYSTEM_Initialize(); // WAS PROVIDED IN THE MAIN.C
    InitializeUserHardware(); // WAS PROVIDED IN THE MAIN.C

    SetOLEDBackground(OLEDC_COLOR_AZURE);

    UpdateDisplayCounter(&display_counter);
    UpdatePotentiometerValue(&potentiometer, &previous_potentiometer);

    while (1) // while forever..
    {
        if (PORTAbits.RA11 == 0) // NOTE: if portAbits.RA11 == 0 it means that the S1 button is pressed. (because it is reversed logic -pressing the button connects it to the ground.)
        {

            LATAbits.LATA8 = 1; // NOTE:then light up LED1

        } else if (LATAbits.LATA8 == 1) // NOTE: if S1 is not pressed, and the LED1 is on
        {
            LATAbits.LATA8 = 0; // turn off LED1
            display_counter++;
            UpdateDisplayCounter(&display_counter);
        }

        if (PORTAbits.RA12 == 0) // NOTE: if S2 is pressed 
        {
            LATAbits.LATA9 = 1; // NOTE : then light up LED2
        } else if (LATAbits.LATA9 == 1) // NOTE: if S2 is not pressed and LED2 is on 
        {
            LATAbits.LATA9 = 0;// turn off LED2

            ToggleDisplayMode(&display_counter, &potentiometer, &previous_potentiometer, &current_mode);
        }


        AD1CON1bits.SAMP = 1; // charge the cable ...
        for (int i = 0; i < 1000; i++); //PDF EX3 COPY PASTE NOTE : the lecturer explained we should wait for around 1000 loops for the cable to charge.
        AD1CON1bits.SAMP = 0; // stop charging the cable.

        while (!AD1CON1bits.DONE);//PDF EX3 COPY PASTE  NOTE: while the Analong digital conversion cycle has not started or is in progress - dont continue the program execution before the conversion ends. (empty while lop)
        potentiometer = ADC1BUF0; // PDF EX3 COPY PASTE NOTE: the result is being saved in the ADC1BUF0  

        if (abs(potentiometer - previous_potentiometer) > 8) // NOTE if there is change of 8 integers on the potentiometer then update the new value on the oled screen. 
        {
            UpdatePotentiometerValue(&potentiometer, &previous_potentiometer);
            previous_potentiometer = potentiometer;
        }
    }

    return 1;
}







// GENERAL IMPORTANT NOTES
/* 

01) ANSB (Analog Select Register):
    01) This register sets specific pins as either analog input or digital input. By default, all pins are digital unless explicitly configured as analog.
    02) RB12 --> the potentiometer Pin. 
    03) GPO (General-Purpose Output) --> Refers to microcontroller pins configured as outputs that can be manually controlled to output a logic High (1) or Low (0) signal. 0 means it wont work, and 1 means it works.
    04)  


    PIN CONFIGURATION SETTING - DATA SHEET
    01) ANSA - for port A.
    02) ANSB - for put B.
    03) ANSC - for port C.


    THE PORTS IS IN OUR MICROCONTROLLER - DATA SHEET CONFIRMED BY CHAT:
    Ports are collections of pins on our microController.
    01) PORTA - 
    01) PORTB -

    BUTTON TO BIT MAPPING - DATA SHEET CONFIRMED BY CHAT
    01) S1 - maped to the RA11 BIT. the RA11 bit belongs to PORT A.
    02) S2 - maped to the RA12 BIT. the RA12 bit belongs to PORT A.
    03) Potentiometer - mapped to the RB12 bit. the RB12 bit belongs to PORT B. 


    LED TO BIT MAPPING - DATA SHEET CONFIRMED BY CHAT:
    The leds are controlled by the LATAbits/LATBbits ports. each bit in the LATA register corresponds to one pin of the associated port.
    e.g LATAbits.Lata8 --> controls the state pin RA8 on PORTA.
    in the general form : LATAbits.LATAx -  Controls the output state of the RAx pin on PORTA.
    02) LED 1 - controlled by LATA8 bit. the LATA8 bit belongs to LATAbits port.
    03) LED 2 - controlled by LATA9 bit. the LATA9 bit belongs to LATAbits port. Each bit in the LATA bit 
    04) 


    INPUT / OUT PUT BIT CONFIGURATION VIA TRISTA/B
    TRISA Configures the direction (input or output) for each pin on PORTA.
    01) TRISAbits: A bitfield structure for accessing individual bits of the TRISA register.


    SOME HEADER ABOUT ADxCONxbits
    The AD1CONxbits configure and control the Analog-to-Digital Converter (ADC) module in the microcontroller.
    determines how sampling and conversion are triggered,and the data format of the output.
    01) TRISAbits.TRISA11 = 1;: Configures pin RA11 which is mapped to S1 as an input, allowing it to read signals


    GENERAL TERMS IM NOT SURE WHERE TO CATEGORIZE: 
    01) Idle Mode - A low-power state where the CPU halts but peripherals can continue operating.


 */