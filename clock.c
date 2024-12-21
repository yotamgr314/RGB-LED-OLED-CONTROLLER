#include <stdint.h>
#include "xc.h"
#include "clock.h"

void CLOCK_Initialize(void)
{
    // CPDIV 1:1; PLLEN disabled; DOZE 1:8; RCDIV FRC; DOZEN disabled; ROI disabled; 
    CLKDIV = 0x3000;
    // STOR disabled; STORPOL Interrupt when STOR is 1; STSIDL disabled; STLPOL Interrupt when STLOCK is 1; STLOCK disabled; STSRC SOSC; STEN disabled; TUN Center frequency; 
    OSCTUN = 0x00;
    // ROEN disabled; ROSWEN disabled; ROSEL FOSC; ROOUT disabled; ROSIDL disabled; ROSLP disabled; 
    REFOCONL = 0x00;
    // RODIV 0; 
    REFOCONH = 0x00;
    // DCOTUN 0; 
    DCOTUN = 0x00;
    // DCOFSEL 8; DCOEN disabled; 
    DCOCON = 0x700;
    // DIV 0; 
    OSCDIV = 0x00;
    // TRIM 0; 
    OSCFDIV = 0x00;
    // AD1MD enabled; T3MD enabled; T1MD enabled; U2MD enabled; T2MD enabled; U1MD enabled; SPI2MD enabled; SPI1MD enabled; I2C1MD enabled; 
    PMD1 = 0x00;
    // IC3MD enabled; OC1MD enabled; IC2MD enabled; OC2MD enabled; IC1MD enabled; OC3MD enabled; 
    PMD2 = 0x00;
    // PMPMD enabled; RTCCMD enabled; CMPMD enabled; CRCMD enabled; I2C2MD enabled; 
    PMD3 = 0x00;
    // CTMUMD enabled; REFOMD enabled; LVDMD enabled; 
    PMD4 = 0x00;
    // CCP2MD enabled; CCP1MD enabled; CCP4MD enabled; CCP3MD enabled; CCP5MD enabled; 
    PMD5 = 0x00;
    // SPI3MD enabled; 
    PMD6 = 0x00;
    // DMA1MD enabled; DMA0MD enabled; 
    PMD7 = 0x00;
    // CLC1MD enabled; CLC2MD enabled; 
    PMD8 = 0x00;
    // CF no clock failure; NOSC FRC; SOSCEN disabled; POSCEN disabled; CLKLOCK unlocked; OSWEN Switch is Complete; IOLOCK not-active; 
    __builtin_write_OSCCONH((uint8_t) (0x00));
    __builtin_write_OSCCONL((uint8_t) (0x00));
}
