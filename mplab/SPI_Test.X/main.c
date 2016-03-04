/* Main File for a SPI read test to get the memory from PEB Flash.
 * 
 * Most code is copied from example code provided by user JorgeF on the Microchip Forum.
 *
 * File:   main.c
 * Author: Myndert
 *
 * Created on March 4, 2016, 2:35 PM
 */

#include <xc.h>

#define TRISOutput  0
#define TRISInput   1

// ----
// SPI I/O Definitions
// ----
#define SPI_TRIS   TRISCbits
#define SPI_CLK    RC3
#define SPI_SDI    RC4
#define SPI_SDO    RC5

// Initialize I/O
#define INIT_SPI_IO()  { SPI_TRIS.SPI_CLK=TRISOutput; SPI_TRIS.SPI_SDO=TRISOutput; SPI_TRIS.SPI_SDI=TRISInput; }

// ------------------ 
// SPI Configuration 
// ------------------ 
 
// MSSP: SPI Config and control 
#define SPI_CONFIG      0b00000010    // Master, Clock = FOSC/64, Disabled 
#define SPI_Enable()    {SSPCON1bits.SSPEN=1;} 
#define SPI_Disable()   {SSPCON1bits.SSPEN=0;} 

// SPI interrupt control 
#define SPI_IntEnable()     {PIR1bits.SSPIF = 0; PIE1bits.SSPIE=1; INTCONbits.PEIE = 1;} 
#define SPI_IntDisable()    {PIE1bits.SSPIE=0;} 
 
// Init MSSP1 in SPI mode and I/O pins 
#define    SPI_Init()            {INIT_SPI_IO(); SSPCON = SPI_CONFIG; SSPSTATbits.CKE=1; PIR1bits.SSPIF=0;} 
 
// ------------------------- 
// SPI1 operation primitives 
// ------------------------- 
   
// Function for pooling the MSSP1 interrupt flag. 
// Wait transfer complete 
// return content TX/RX Buffer after transfer complete (received data) 
unsigned char SSP_Wait_IF(void); 
 
// Send a byte to the SPI1 bus 
#define SPI_SEND(DATA)      {SSPBUF = DATA; SSP_Wait_IF();} 
 
// Read a byte from the SPI1 bus (send dummy to generate clock) 
#define SPI_RECEIVE(DATA)   {SSPBUF = 0; DATA = SSP_Wait_IF();} 

void main(void) {
    return;
}

// Wait for SSP interrupt flag (pooling) to detect SSP operation completion 
unsigned char SSP_Wait_IF(void) 
{ 
    while(!PIR1bits.SSPIF); 
    PIR1bits.SSPIF = 0; 
    return SSPBUF; 
} 