#ifndef _XTAL_FREQ
#define _XTAL_FREQ  8000000UL
#endif

#include "xc.h"
#include "stdint.h"

#ifndef SYSTEM_H
#define	SYSTEM_H

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Initializes the device to the default states configured in the
 *                  MCC GUI
 * @Example
    SYSTEM_Initialize(void);
 */
void SYSTEM_Initialize(void);
#endif	/* SYSTEM_H */
/**
 End of File
*/