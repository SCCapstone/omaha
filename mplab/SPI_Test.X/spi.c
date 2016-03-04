// spi.c -- SPI library for PIC16F873.

#include "spi.h"


// Initialize I/O
inline void Init_SPI_IO(void) {
    SPI_TRIS.SPI_CLK = TRISOutput;
    SPI_TRIS.SPI_SDO = TRISOutput;
    SPI_TRIS.SPI_SDI = TRISInput;
}


// -----------------
// SPI Configuration
// -----------------

// MSSP: SPI Config and control
// Original code had: { SSPCON1bits.SSPEN = ...; }
inline void SPI_Enable()  { SSPCONbits.SSPEN = 1; }
inline void SPI_Disable() { SSPCONbits.SSPEN = 0; }


// SPI interrupt control
inline void SPI_IntEnable() {
    PIR1bits.SSPIF = 0;
    PIE1bits.SSPIE=1;
    INTCONbits.PEIE = 1;
}

inline void SPI_IntDisable() { PIE1bits.SSPIE = 0; }


// Init MSSP1 in SPI mode and I/O pins
inline void SPI_Init(void) {
    Init_SPI_IO();
    SSPCON = SPI_CONFIG;
    SSPSTATbits.CKE=1;
    PIR1bits.SSPIF=0;
}


// -------------------------
// SPI1 operation primitives
// -------------------------

// Function for pooling the MSSP1 interrupt flag.
// Wait transfer complete
// return content TX/RX Buffer after transfer complete (received data)
// Wait for SSP interrupt flag (pooling) to detect SSP operation completion
unsigned char SSP_Wait_IF(void) {
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    return SSPBUF;
}

// Send a byte to the SPI1 bus
inline unsigned char SPI_Send(unsigned char data) {
    SSPBUF = data;
    SSP_Wait_IF();
    return SSPBUF;
}

// Read a byte from the SPI1 bus (send dummy to generate clock)
inline unsigned char SPI_Receive(unsigned char data) {
    SSPBUF = 0;
    data = SSP_Wait_IF();
    return data;
}

// SPI Send/Recv are identical in effect.
// Value passed in will be sent over the SPI1 bus.
// Value returned is whatever comes back over the SPI1 bus.
inline unsigned char SPI_Transfer(unsigned char data) {
    SSPBUF = data;
    data = SSP_Wait_IF();
    return data;
}