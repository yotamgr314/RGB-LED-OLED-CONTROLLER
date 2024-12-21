#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>
#include "../system/system.h"
#include "../system/clock.h"
#include "spi1_driver.h"

void (*spi1_interruptHandler)(void); 

void spi1_close(void)
{
    SPI1CON1Lbits.SPIEN = 0;
}

//con == SPIxCONL, brg == SPIxBRGL, operation == Master/Slave
typedef struct { uint16_t con1; uint16_t brg; uint8_t operation;} spi1_configuration_t;
static const spi1_configuration_t spi1_configuration[] = {   
    { 0x0120, 0x0000, 0 },
    { 0x0120, 0x000F, 0 }
};

bool spi1_open(/*spi1_modes spiUniqueConfiguration*/)
{
    if(!SPI1CON1Lbits.SPIEN)
    {
        SPI1CON1L = 0x0120;//spi1_configuration[spiUniqueConfiguration].con1;
        SPI1BRGL = 0;//spi1_configuration[spiUniqueConfiguration].brg;
        
        TRISBbits.TRISB15 = 0;//spi1_configuration[spiUniqueConfiguration].operation;
        SPI1CON1Lbits.SPIEN = 1;
        return true;
    }
    return false;
}

// Full Duplex SPI Functions
uint8_t spi1_exchangeByte(uint8_t b)
{
    SPI1BUFL = b;
    while(!SPI1STATLbits.SPIRBF);
    return SPI1BUFL;
}

void spi1_exchangeBlock(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        *data = spi1_exchangeByte(*data );
        data++;
    }
}

// Half Duplex SPI Functions
void spi1_writeBlock(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        spi1_exchangeByte(*data++);
    }
}

void spi1_readBlock(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        *data++ = spi1_exchangeByte(0);
    }
}

void spi1_writeByte(uint8_t byte)
{
    SPI1BUFL = byte;
}

uint8_t spi1_readByte(void)
{
    return SPI1BUFL;
}

/**
 * Interrupt from SPI on bit 8 received and SR moved to buffer
 * If interrupts are not being used, then call this method from the main while(1) loop
 */
void spi1_isr(void)
{
    if(IFS3bits.SPI1RXIF == 1){
        if(spi1_interruptHandler){
            spi1_interruptHandler();
        }
        IFS3bits.SPI1RXIF = 0;
    }
}

void spi1_setSpiISR(void(*handler)(void))
{
    spi1_interruptHandler = handler;
}

