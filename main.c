// IMPORTS SECTION
#include <stdlib.h> 
#include <stdio.h>
#include "System/system.h" // NOTE: Contains implementd system initialization functions (like SYSTEM_Initialize()), which configure microcontroller peripherals like clocks, timers, and other hardware components
#include "System/delay.h" // NOTE: Used for adding delays, usually __delay_ms() or __delay_us() functions.
                          // for example:
                          // 01) Timing-sensitive hardware:
                          // 02) Waiting for sensors.
                          // 03) displays
                          // 04) or other hardware devices to complete an operation (e.g., clearing the OLED screen or waiting for a signal from an input pin).
#include "oledDriver/oledC.h" // NOTE: Provides functions for controlling the OLED screen, like oledC_DrawString, oledC_sendColorInt, and more.
#include "oledDriver/oledC_colors.h"// NOTE: Defines color constants like OLEDC_COLOR_ORANGE and OLEDC_COLOR_GREEN.
#include "oledDriver/oledC_shapes.h" // NOTE:  Provides functions for shapes and drawing on the OLED, as well as the oledC_DrawString function.


// DEFINES SECTION
typedef unsigned char MODE; // NOTE: Define a typedef for display modes.
#define NORMAL 0
#define INVERSE !NORMAL


static uint16_t background_color;// NOTE: Stores the background color for the OLED screen in a 16 bits unsigned int  - a color is packed into 16 bits where the first 5 bits are for red Red, the next 6 bits are for Green and the next remaining 5 bits are for Blue.


// TRANSFORMING POTENTIOMETER ANALOG INPUT INTO DIGITAL.
//EXPLANATION STARTS
// 01) When working with a potentiometer (or any analog sensor), the goal is to convert the analog voltage (continuous signal) into a digital value that the microcontroller can process.
//     This is done using an Analog-to-Digital Converter (ADC).
//EXPLANATION ENDS

void User_Initialize(void)
{
  //Configure A/D Control Registers (ANSB & AD1CONx SFRs).
    TRISBbits.TRISB12 = 1;//PDF EX3 COPY PASTE NOTE:Set RB12 as input, 1 for input. 0 for output.
    ANSBbits.ANSB12 = 1; //PDF EX3 COPY PASTE NOTE:ANSB (Analog select register) - Configure the RB12 Bit as an analog input. By default, all pins are digital unless explicitly configured as analog
    AD1CON1bits.SSRC = 0;//PDF EX3 COPY PASTE NOTE:Manual sample mode
    AD1CON1bits.FORM = 0;//PDF EX3 COPY PASTE NOTE: Selects the output format of the ADC result (integer, signed integer, fractional) 0 for unsigned int, 1 for signed, and 2 for fractional
    AD1CON1bits.ASAM = 0;//PDF EX3 COPY PASTE NOTE: Enables (1) or disables (0) automatic sampling after a conversion is complete.
    AD1CON1bits.ADSIDL = 0;//PDF EX3 COPY PASTE
    AD1CON1bits.DMABM = 0;//PDF EX3 COPY PASTE
    AD1CON1bits.DMAEN = 0; //PDF EX3 COPY PASTE
    AD1CON1bits.MODE12 = 0;//PDF EX3 COPY PASTE


    
    AD1CON2 = 0x00;// PDF EX3 COPY PASTE NOTE:PDD required AD1CON2(use 0)
    AD1CON3bits.ADCS = 0xFF;//PDF EX3 COPY PASTE
    AD1CON3bits.SAMC = 0x10;// PDF EX3 COPY PASTE
    AD1CON3bits.ADRC = 0x00;//PDF EX3 COPY PASTE
    AD1CON3bits.EXTSAM = 0x00;//PDF EX3 COPY PASTE
    AD1CON3bits.PUMPEN = 0x00;//PDF EX3 COPY PASTE

  //Configure S1/S2 and LED1/LED2 IO directions (TRISA)
    TRISAbits.TRISA11 = 1; // NOTE: RA11 which is the bit that mapped to S1 buttonm and it is configured as input.via TristA(1 = input).
    TRISAbits.TRISA12 = 1;// NOTE: RA12 which is the bit that mapped to S2 button, and it is configured as input.via TristA (1 = input).
    TRISAbits.TRISA8 = 0; // NOTE: RA8 bit which is the bit that mapped to LED1, and it is configured as output. via TristA (0 = output).
    TRISAbits.TRISA9 = 0;// NOTE: RA9 bit which is the bit that mapped to LED2, and it is configured as output. via TristA.(0 = output)
    
    AD1CHSbits.CH0SA = 8;  // PDF EX3
    AD1CON1bits.ADON = 1; // NOTE: Turn on the A/D converter, 0 for off, and 1 for on.
}

// CLEARS THE ENTIRE OLED SCREEN WITH THE BACKGROUND COLOR.
static void oledC_clearScreen(void) // from Oled_example.c
{    
    uint8_t x;
    uint8_t y;
    oledC_setColumnAddressBounds(0,96);
    oledC_setRowAddressBounds(0,96);
    for(x = 0; x < 96; x++)
    {
        for(y = 0; y < 96; y++)
        {
            oledC_sendColorInt(background_color);
        }
    }
}


// SETS THE BACKGROUND COLOR FOR THE OLED SCREEN.
static void oledC_setBackground(uint16_t color) // from Oled_example.c
{
    background_color = color;
    oledC_clearScreen();
}


// KODI WROTE , oledC_sendCommand() - must use it to prevent flickering from changes.
// TOGGLES THE DISPLAY MODE BETWEEN NORMAL AND INVERSE
void changeMode() 
{
    static MODE currentMode =  INVERSE;

    if(currentMode == NORMAL){
       
        oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_ON,NULL,0);  
    }else {
        
        oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_INVERSE,NULL,0);
    }
    currentMode = !currentMode;  
    oledC_clearScreen(); 
}






// Updates the OLED with the current count value
void updateCount(int newCount) // KODI WROTE.
{
    static int oldCount = 0;
    char buffer[10];
    sprintf(buffer, "S:%d",oldCount ); // turn it into a string.
    oledC_DrawString(10, 60, 2, 2, (uint8_t *)buffer,background_color );
    sprintf(buffer, "S:%d", newCount);
    oledC_DrawString(10, 60, 2, 2, (uint8_t *)buffer, OLEDC_COLOR_MAGENTA);
    
    oldCount = newCount;
}


// UPDATES THE OLED WITH THE CURRENT POTENTIOMETER VALUE
void updatePotentio(int potentio,int oldPotentio) // KODI WROTE> 
{
    char desPotentio[10];
    
    sprintf(desPotentio,"p:%d", oldPotentio);
    oledC_DrawString(10, 10, 2, 2, (uint8_t *)desPotentio,background_color);
    sprintf(desPotentio, "p:%d",potentio );
    oledC_DrawString(10, 10, 2, 2, (uint8_t *)desPotentio, OLEDC_COLOR_GREEN );
}

    

int main(void)
{
    int count=0, potentio=0;
    static int oldPotentio = -1;
     
    
    // initialize the system
    SYSTEM_Initialize();
    User_Initialize();

    //Set OLED Background color and Clear the display
    oledC_setBackground(OLEDC_COLOR_DARKSEAGREEN);

    updateCount(count);

    updatePotentio(potentio,oldPotentio);

    //Main loop
    while(1)
    {
        
	//Count S1 hits and display (Light LED1 when S1 pressed) - GPO usage 
        if (PORTAbits.RA11 == 0)// NOTE: if portAbits.RA11 == 0 it means that the S1 button is pressed. (because it is reversed logic -pressing the button connects it to the ground.)
        {
            
            LATAbits.LATA8 = 1; // if pressed - light up LED1
        }
        else {
            if( LATAbits.LATA8 == 1) // if S1 not pressed and LED1 is on then - turn off LED1. 
            {
                
               LATAbits.LATA8 = 0; // turn off led 1.
               count++;// inc the count of pressed on S1.
               updateCount(count);
       
            }
            
        }
	//Toggle background color with S2 hits (Light LED2 when S2 pressed)
        if(PORTAbits.RA12 == 0) // NOTE: if portAbits.RA12 == 0 it means that the S2 button is pressed. (because it is reversed logic -pressing the button connects it to the ground.)
        {
            LATAbits.LATA9 = 1; // NOTE: light up LED 2 which is controlled by RA9 bit, and belongs to PORT.
            
        }else{
            if( LATAbits.LATA9 == 1) // else if S2 is not pressed
            {
                LATAbits.LATA9 = 0; // turn off LED 2.
                changeMode(); // change mode. 
            }
            
        }
        
        
	//Get potentiometer position and display decimal value
         AD1CON1bits.SAMP = 1; // Samp and hold emplifier are sampling - DATA SHEET.
            
        for(int i = 0; i < 1000; i++ ); // 1000 cycles for the cable to charge - lecturer said should be enough for it to charge - LEC03 PDF

        AD1CON1bits.SAMP = 0;  // stop charging the cable.
            
        while (!AD1CON1bits.DONE); // while the Analong digital conversion cycle has not started or is in progress, dont cointuie the progrma before the conversion ends. (empty while lop)
        potentio = ADC1BUF0; 
        
       
        if ( abs(potentio - oldPotentio) > 8 )
        {
            
            updatePotentio(potentio,oldPotentio);
            oldPotentio = potentio;
            
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