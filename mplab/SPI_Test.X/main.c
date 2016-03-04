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

#include "spi.h"

void main(void) {
    unsigned char data_out; 
    unsigned char data_in; 

    // Configure SPI1 operation and I/O pins 
    SPI_Init();


    // Enable SPI1 
    SPI_Enable();
  
    // Sending one data byte 
    data_out = 0x55; 
    SPI_Send(data_out); 

    // Receiving one data byte 
    SPI_Receive(data_in); 

    while(1); 
}