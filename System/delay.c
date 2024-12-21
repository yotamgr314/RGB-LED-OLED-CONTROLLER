#ifndef FCY
#define FCY (_XTAL_FREQ/2)
#endif
#include "clock.h"
#include <libpic30.h>
#include <stdint.h>

/**
*  \ingroup doc_driver_delay_code
*  Call this function to delay execution of the program for a certain number of milliseconds
@param milliseconds - number of milliseconds to delay
*/
void DELAY_milliseconds(uint16_t milliseconds) {
    while(milliseconds--){ 
        __delay_ms(1); 
    }
}

/**
*  \ingroup doc_driver_delay_code
*  Call this function to delay execution of the program for a certain number of microseconds
@param microseconds - number of microseconds to delay
*/
void DELAY_microseconds(uint16_t microseconds) {
    while( microseconds >= 32)
    {
        __delay_us(32);
        microseconds -= 32;
    }
    
    while(microseconds--)
    {
        __delay_us(1);
    }
}
