// spi.h -- SPI library for PIC16F873.
/* Usage Notes:
 * To send/recv a single byte over SPI, use the following procedure:
 *     SPI_Enable();
 *     SPI_Transfer(...);
 *     SPI_Disable();
 * 
 * To send/recv multiple bytes in one SPI session, use the following procedure:
 *     SPI_Enable();
 *     unsigned char send_data[10] = { ... };
 *     unsigned char recv_data[10];
 *     for (int i = 0; i < 10; i++) {
 *         recv_data[i] = SPI_Transfer(send_data[i]);
 *     }
 *     SPI_Disable();
 */
#ifndef SPI_H
#define	SPI_H

#include <xc.h>

#define TRISOutput  0
#define TRISInput   1

// -------------------
// SPI I/O Definitions
// -------------------
#define SPI_TRIS   TRISCbits
// Orignal Code:
//#define SPI_CLK    RC3
//#define SPI_SDI    RC4
//#define SPI_SDO    RC5
#define SPI_CLK    TRISC3
#define SPI_SDI    TRISC4
#define SPI_SDO    TRISC5

// Initialize I/O
inline void Init_SPI_IO();

// -----------------
// SPI Configuration
// -----------------

// MSSP: SPI Config and control
#define SPI_CONFIG      0b00000010    // Master, Clock = FOSC/64, Disabled
inline void SPI_Enable();
inline void SPI_Disable();

// SPI interrupt control
inline void SPI_IntEnable();
inline void SPI_IntDisable();

// Init MSSP1 in SPI mode and I/O pins
inline void SPI_Init(void);

// -------------------------
// SPI1 operation primitives
// -------------------------

// Function for pooling the MSSP1 interrupt flag.
// Wait transfer complete
// return content TX/RX Buffer after transfer complete (received data)
unsigned char SSP_Wait_IF(void);

// Send a byte to the SPI1 bus
inline unsigned char SPI_Send(unsigned char data);

// Read a byte from the SPI1 bus (send dummy to generate clock)
inline unsigned char SPI_Receive(unsigned char data);

// Wait for SSP interrupt flag (pooling) to detect SSP operation completion
unsigned char SSP_Wait_IF(void);

// Send the SPI slave device a byte, and return the slave device's reply byte.
inline unsigned char SPI_Transfer(unsigned char data);

#endif	/* SPI_H */

