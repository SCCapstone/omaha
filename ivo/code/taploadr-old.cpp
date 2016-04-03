/***************************** TAPLOADR.CPP *********************************
*
* Program Name: TAPLOADR.CPP
*
* Version:      1.0
*
* Date:         July 18, 1995
*
* Author:       Daniel S. Hays
*               386 Applications Engineer
*
* References:   Excerpts of code taken from modules of the article
*               "Beyond the Myth of JTAG Boundary Scan Port" by Dmitrii
*               Loukianov, Intel Corp., 1995.
*
* Program Spec: This program will take an input flash file residing on a PC
*               and program it into the boot block flash of the 386EX
*               Evaluation Board utilizing the JTAG unit onboard the 386EX
*               embedded processor. It will also erase the entire FLASH
*               chip beforehand, including the boot block area, if
*               enabled as described in the requirements section below.
*
* Requirements: In addition to the eval board itself, it is required that
*               the user has a JTAG interface board plugged into both the
*               evaluation board's expansion bus slot and the host PC's
*               parallel port. The U16 PLD chip must be updated in order to
*               disable the FLASH_WE# signal, and a jumper must be installed
*               on pins 1-2 of Jumper J12, which is not normally populated
*               on the standard eval board.
*
*               *** Note: The power supply for the 386EX eval board must be
*               ON in order for successful programming of the flash to take
*               place. The program implies that UCS is the CS# pin for flash
*               memory being programmed. UCS is set LOW for any address!
*
*               The user must also know the location and name of the input
*               data file in .BIN format, as well as the starting location
*               in FLASH memory that the file is to be located at.
*
* Disclaimer:   Information in this document is provided 'as is' solely to
*               enable use of Intel products. Intel assumes no liability
*               whatsoever, including infringement of any patent or
*               copyright, concerning the included software. Intel
*               Corporation makes no warranty for the use of this software
*               and assumes no responsibility for any errors which may
*               appear in this document nor does it make a commitment to
*               update the information contained herein.
*
*               Copyright (C) Intel Corporation 1995
*               All Rights Reserved.
*
*************************  GLOBAL DECLARATIONS  **************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <conio.h> // Non-existent on Linux.

// Make these function annotations harmless. GCC chokes otherwise.
#define near
#define far

/**** Definitions of JTAG BSR pins for sequence for Intel 386 EX CPU ****/
/***** Note: MIO shifted out LAST, D15 - first! *************************/

#define     D15               0
#define     D14               1
#define     D13               2
#define     D12               3
#define     D11               4
#define     D10               5
#define     D9                6
#define     D8                7
#define     D7                8
#define     D6                9
#define     D5               10
#define     D4               11
#define     D3               12
#define     D2               13
#define     D1               14
#define     D0               15
#define     LBA              16
#define     CS6              17
#define     UCS              18
#define     P27              19
#define     P26              20
#define     P25              21
#define     DACK0            22
#define     P24              23
#define     P23              24
#define     P22              25
#define     P21              26
#define     P20              27
#define     SMIACT           28
#define     DRQ1             29
#define     DRQ0             30
#define     WDTOUT           31
#define     EOP              32
#define     DACK1            33
#define     P17              34
#define     RESET            35
#define     P16              36
#define     P15              37
#define     P14              38
#define     P13              39
#define     P12              40
#define     P11              41
#define     P10              42
#define     FLT              43
#define     STXCLK           44
#define     INT7             45
#define     INT6             46
#define     INT5             47
#define     INT4             48
#define     TMRGATE2         49
#define     TMROUT2          50
#define     NMI              51
#define     PEREQ            52
#define     P37              53
#define     P36              54
#define     P35              55
#define     P34              56
#define     P33              57
#define     P32              58
#define     SSIOTX           59
#define     SSIORX           60
#define     SRXCLK           61
#define     P31              62
#define     P30              63
#define     SMI              64
#define     A25              65
#define     A24              66
#define     A23              67
#define     A22              68
#define     A21              69
#define     A20              70
#define     A19              71
#define     A18              72
#define     A17              73
#define     A16              74
#define     A15              75
#define     A14              76
#define     A13              77
#define     A12              78
#define     A11              79
#define     A10              80
#define     A9               81
#define     A8               82
#define     A7               83
#define     A6               84
#define     A5               85
#define     A4               86
#define     A3               87
#define     A2               88
#define     A1               89
#define     NA               90
#define     ADS              91
#define     BHE              92
#define     BLE              93
#define     WR               94
#define     RD               95
#define     BS8              96
#define     READY            97
#define     WRD              98
#define     DC               99
#define     MIO             100

#define     TRUE              1
#define     FALSE             0

typedef      unsigned int    word;        // 16 Bit word
typedef      unsigned char   byte;        //  8 Bit Byte
typedef      char            *Pchar;
typedef      Pchar           PJTAGdata;   // JTAG Data array / null term string

const        word  BSR_Length=202;        // # bits in JTAG BSR string 101x2
const        word  ID_String_Length=32;   // # bits in JTAG CPU ID String

unsigned long int A;                      // Stores address data

unsigned long int i;                    // Stores index value
unsigned long int data_start_address;   // Holds starting address of program
word Rx;                                // Stores register data
word new_word;                          // Holds word to be written to FLASH
word high_part;                         // Temp Holder for upper part of word
char PinState[BSR_Length];              // Holds Pin Data to move in and out
char input_file[80];                    // Holds name of input file
int  c;                                 // Holds character being worked with
FILE *in;                               // Points to input file location

/****** JTAG1149 Commands for Intel386EX Embedded Processor *************/

char *BYPASS     ="1111";               // Use BYPASS register in data path
char *EXTEST     ="0000";               // External Test Mode
char *SAMPLE     ="1000";               // Sample/Preload Instruction
char *IDCODE     ="0100";               // Read ID CODE from the chip
char *INTEST     ="1001";               // On-chip System Test
char *HIGHZ      ="0001";               // Place device into Hi-z mode

/******************* Assembly language variables ************************/

#define     TCK         1;              // Bit 0 is TCK output
#define     TMS         2;              // Bit 1 is TMS output
#define     TCKTMS      3;              // Bit 0+1
#define     TDI         0x40;           // Bit 6 is TDI output
#define     notTCKTMS   0xFC;           // Bit 0+1
#define     TDITMS      0x42;           // Bit TDI+TMS
#define     TRST        4;              // JTAG+2
#define     TDO         0x80;           // JTAG+1, bit is inverted!

static      word JTAG=0x378;          // LPT1 Data Address Default
const       word JTAGI=JTAG+1;        // Contains circuit input
const       word JTAGR=JTAG+2;        // Reset bit is here

/**************************************************************************/
/************** INLINE ASSEMBLER FUNCTIONS FOR JTAG I/O *******************/
/**************************************************************************/

/**************** Assembly function to reset the JTAG unit ****************/

void far Reset_JTAG() /** Reset TAP logic by optional TRST# signal **/
{
        __asm
        {
                mov dx,JTAG
                mov al,0        // +TDI
                out dx,al
                mov dx,JTAGR
                mov al,0        // TRST# LOW
                out dx,al
                mov dx,JTAGR
                mov al,TRST     // TRST# HIGH
                out dx,al
        }
}

/*** Assembly function to go into Run_Test_Idle state from unknown state **/

void far Restore_Idle () /** Restore Test_Logic_Reset state by 5 TCK's **/
{                        /** Goes into TLR state from any **/
                         /** unknown state of the JTAG controller **/
        __asm
        {
                mov     cx,5
                mov     dx,JTAG

        FiveTimes:

                mov     al,TMS  // TMS HIGH
                out     dx,al   // Set TMS/TDI
                or      al,TCK
                out     dx,al   // TCK High
                xor     al,TCK  // TCK Low
                out     dx,al
                loop    FiveTimes
        }
}
/********* Assembly function to do one transition with TMS High ***********/

void near TMS_High () /** One transition with TMS High **/
{
        __asm
        {
                mov     dx,JTAG
                mov     al,TMS  // Sets TMS high
                out     dx,al   // Set TMS/TDI
                or      al,TCK
                out     dx,al   // TCK High
                xor     al,TCK  // TCK Low
                out     dx,al
        }
}

/********* Assembly function to do one transition with TMS Low ************/

void near TMS_Low () /** One transition with TMS Low **/
{
        ___asm
        {
                mov     dx,JTAG
                mov     al,0    // Set TMS Low
                out     dx,al   // Set TMS/TDI
                or      al,TCK
                out     dx,al   // TCK High
                xor     al,TCK  // TCK Low
                out     dx,al
        }
}
/***** Assembly function to shift data into JTAG port while reading *****/

void near Shift_Data_Array(unsigned S, char far *D)
{
            /** Shifts data String into JTAG port while reading data **/
            /** from JTAG port back into D, **/
            /** The procedure should be called when JTAG controller **/
            /** is in the SelectDRScan state **/
        __asm
        {
                mov     dx,JTAG
                push    es
                push    di
                les     di, D       // Get array pointer
                cld
                xor     ax,ax
                mov     ax, S       // Get Size
                dec     ax
                mov     cx,ax
                jz      LastClock3

        I_Shift3:

                mov     al, byte ptr es:[di]
                shl     al,6
                and     al, notTCKTMS   // Clear TCK and TMS bits
                out     dx,al           // Put first data bit
                or      al,TCK          // Set TCK high
                out     dx,al           // Shift in first data bit

                inc     dx
                                        // Sample first data bit
                in      al,dx
                and     al,80h
                mov     al,'1'
                je      Ex_1
                mov     al,'0'

        ex_1:

                stosb
                dec     dx
                loop    I_Shift3

        LastClock3:

                mov     al, byte ptr es:[di]
                shl     al,6
                and     al, notTCKTMS
                or      ax, TMS     // Set TMS bit
                out     dx,al       // Put last data bit
                or      al,TCK      // Set TCK high
                out     dx,al       // Shift in first data bit
                inc     dx
                                    // Sample first data bit
                in      al,dx
                and     al,80h

                mov     al,'1'
                je      Ex_2
                mov     al,'0'

        ex_2:

                stosb

                dec     dx

                mov     al,TDITMS   // Leave TCK pin Low
                out     dx,al

                pop     di
                pop     es
        }
}
/*** Assembly function to shift data into JTAG port while not reading ***/

void near Shift_Data_Array_IN(unsigned S, char far *D)
{
    /** Shifts data String into JTAG port WITHOUT reading data **/
    /** from JTAG port back into D. **/
    /** The procedure should be called when JTAG controller is in the **/
    /** SelectDRScan state. **/
        __asm
        {
                mov     dx,JTAG
                push    es
                push    di
                les     di, D       // Get string
                cld
                xor     ax,ax
                mov     ax, S ; Get Size
                dec     ax
                mov     cx,ax
                jz      LastClock4

        I_Shift4:

                mov     al, byte ptr es:[di]
                shl     al,6
                and     al, notTCKTMS
                out     dx,al       // Put first data bit
                or      al,TCK      // Set TCK high
                out     dx,al       // Shift in first data bit
                inc     di          // Update pointer
                loop    I_Shift4

        LastClock4:

                mov     out al, byte ptr es:[di]
                shl     al,6
                and     al, notTCKTMS
                or      al, TMS
                out     dx,al       // Put last data bit
                or      al,TCK      // Set TCK high
                out     dx,al       // Shift in last data bit
                mov     al,TDITMS   // Leave TCK pin Low!
                out     dx,al

                pop     di
                pop     es
        }
}
/********* Assembly function to pulse STROBE line on parallel ports ******/

void far Strobe_Data_In ()
{
        __asm
        {
                push    dx
                mov     dx,JTAGR
                mov     al,1+TRST   // Sets STROBE# bit low for WE# use
                out     dx,al
                mov     al,TRST     // Returns STROBE# without RESET#
                out     dx,al
                pop     dx
        }
}
/**************************************************************************/
/******************** C++ FUNCTIONS FOR JTAG PROGRAMMING ******************/
/**************************************************************************/

/************** Function to send instruction to JTAG **********************/

void Send_Instruction (unsigned S, char far *D)
            /* Send instruction string into JTAG port, replace */
            /* the original string with the data that comes out TDO */

{
        TMS_Low;                // Go to Run_Test_Idle
        TMS_Low;                // Go to Run_Test_Idle
        TMS_High;               // Go to Select_DR_Scan
        TMS_High;               // Go to Select_IR_Scan
        TMS_Low;                // Go to Capture_IR
        TMS_Low;                // Go to Shift_IR
        Shift_Data_Array(S,D);  
        TMS_High;               // Update_IR, new instr. in effect
        TMS_Low;                // Run_Test_Idle
}
/******** Function to send instruction into JTAG port, do not read TDO ***/

void Send_Instruction_IN (unsigned S, char far *D)
{
        TMS_Low();                          // Go to Run_Test_Idle
        TMS_Low();                          // Go to Run_Test_Idle
        TMS_High();                         // Go To Select_DR_Scan
        TMS_High();                         // Go To Select_IR_Scan
        TMS_Low();                          // Go to Capture_IR
        TMS_Low();                          // Go to Shift_IR }
        Shift_Data_Array_IN(S,D);
        TMS_High();                         // Update_IR, new instr. in effect
        TMS_Low();                          // Run_Test_Idle
}
/**** Function to send data string into JTAG port + replace original *****/

void Send_Data (unsigned S, char far *D)
/* Send data string into JTAG port */
/* replace the original string with the data that comes out TDO */
{
TMS_Low();                                  // Go to Run_Test_Idle
TMS_Low();                                  // Go to Run_Test_Idle
TMS_High();                                 // Go To Select_DR_Scan
TMS_Low();                                  // Go to Capture_DR
TMS_Low();                                  // Go to Shift_DR
Shift_Data_Array(S,D);
TMS_High();                                 // Update_IR, new data is in effect
TMS_Low();                                  // Run_Test_Idle
}
/**** Function to send data string into JTAG port w/o replacing orig. ****/

void far Send_Data_IN (unsigned S, char far *D)
/* Send data string into JTAG port, */
/* The original data is not overwritten */
{
        TMS_Low();                          // Go to Run_Test_Idle
        TMS_Low();                          // Go to Run_Test_Idle
        TMS_High();                         // Go To Select_DR_Scan
        TMS_Low();                          // Go to Capture_DR
        TMS_Low();                          // Go to Shift_DR
        Shift_Data_Array_IN(S,D);
        TMS_High();                         // Update_IR, new data is in effect
        TMS_Low();                          // Run_Test_Idle
}
/*************** Function to invert a data string so MSB is first *********/

void Flip_ID_String (int length, char Input[ID_String_Length])
{                   /* Flips the JTAG Unit ID string */
                    /* since it is read in backwards */
    int i, j;
    char Temp[ID_String_Length];

    j = 0;                              // Initialize Temporary place holder

    for (i = length; i >= 1; --i)
    {
        Temp[j] = Input[i-1];
        ++j;
    }

    for (i = 0; i <= (length-1); ++i) {
        Input[i] = Temp[i];             // Copy Temp string to perm. one
    }
}
/********** Function to get ID string from the Intel(tm)386EX Chip ********/

void Get_JTAG_Device_ID ()
{
    const char *p="01010101010101010101010101010101";
                                            // Dummy string, will change value
                                            // after Send_Data executes
    char ID[ID_String_Length];
    strcpy(ID,p);                           // Fill with dummy string
    Send_Instruction_IN(strlen(IDCODE), IDCODE); // Do NOT overwrite Instr.
                                            // Because it resides in the
                                            // Fixed string area!
    Send_Data(strlen(ID), ID);
    Flip_ID_String(strlen(ID), ID);         // Makes MSB first in array
    printf("\nThe JTAG CPU Chip Identifier is: %s\n",ID);
    printf(
        "For Intel386(tm)EX it should be: 00000000001001110000000000010011\n");
}
/**** Function to fill the JTAG array with zeros and set all as inputs **/

void Fill_JTAG(PJTAGdata P)

/******************************************
 Configures pins for typical configuration:
     P15: Out, Low
     ADS: Out, Low
     BHE: Out, Low
     BLE: Out, Low
     WR : Out, Don't Care
     RD : Out, Don't Care
     WRD: Out, Low
     DC : Out, High
     MIO: Out, High
     UCS: Out, Don't Care
     LBA: Out, Low
     All other entries configured as inputs
     Dir Bit Output = i*2
     Data Bit       = i*2+1
*******************************************/

{
    unsigned i;
    for (i = 0; i <= BSR_Length-1; i++) {
        P[i] ='0';
    }
    P[P15*2]   = '1';
    P[P15*2+1] = '0'; // Make Vpp active to program FLASH
    P[ADS*2]   = '1';
    P[ADS*2+1] = '0';
    P[BHE*2]   = '1';
    P[BHE*2+1] = '0'; // BHE and BLE active for 16 Bit
    P[BLE*2]   = '1';
    P[BLE*2+1] = '0';
    P[WR*2]    = '1'; // Not necessary to initialize value
    P[RD*2]    = '1'; // Not necessary to initialize value
    P[WRD*2]   = '1';
    P[WRD*2+1] = '0'; // WRD is Read by default
    P[DC*2]    = '1';
    P[DC*2+1]  = '1';
    P[MIO*2]   = '1';
    P[MIO*2+1] = '1';
    P[UCS*2]   = '1'; // Not necessary to initialize value
    P[LBA*2]   = '1';
    P[LBA*2+1] = '0'; // Enables U8 by fooling PLD
}
/******** Function to Set Data Pins given 16 Bit Data ********************/

void Set_Data (PJTAGdata P, word D) /* Sets data onto pins and makes them */
{                                   /* into outputs */
    int i;
    word M;
    M = 1;
    for (i = D0; i >= D15; --i )
    {
        if ((D & M) != FALSE) {
            P[i*2+1] = '1';
        } else {
            P[i*2+1] = '0';
        }
        P[i*2] = '1'; // Data pins are Outputs now
        M <<= 1;
    }
}
/******** Function to set data DIR bits to 0 on 16 bit data bus ***********/

void Get_Data(PJTAGdata P)  /** Configures data lines as inputs **/
{
    int i;
    for ( i=D0; i>=D15; --i)
    {
        P[i*2] = '0'; // Configure as inputs
    }
}
/**** Function to convert JTAG output string into byte ******************/
word Parse_Data(PJTAGdata P) /** Reads data lines and returns data word **/
{
    int i;
    word M=1,D=0;
    for (i = D0; i >= (D15); --i) // Reads data lines
    {
        if (P[i*2+1] == '1') {
            D=D|M;
        }
        M <<= 1;
    }
    return(D);
}
/*********** Function to set the address on the address pins *************/

void Set_Address(PJTAGdata P, unsigned long int Address)
{               /* Sets address lines and makes them into outputs */
    int i;
    long int M=1;
    for (i = A1; i >= A25; --i)
    {
        if ((Address & M) != 0) {
            P[i*2+1]='1';
        } else {
            P[i*2+1]='0';
        }
        M <<= 1;
        P[i*2]='1';
    }
    P[UCS*2+1] = '0';
}
/************* Function to read data from FLASH *************************/

word Flash_Read(PJTAGdata P, unsigned long int Address)
{
    Get_Data(P);                // Configure Data Bus as inputs
    Set_Address(P,Address);     // Set addr on bus
    P[UCS*2+1] = '0';           // Selects Flash chip
    P[RD*2+1] = '0';            // RD#=Low Data
    P[WR*2+1] = '1';            // WR#=High Data
    P[WRD*2+1] = '0';           // For Read
    Send_Data_IN(BSR_Length,P);
            // sets data on the Address bus, Data bus in the input mode
    Send_Data(BSR_Length,P);    
            // Latches Data bus into BSR and then shifts it out into P
    return(Parse_Data(P));      // Convert result into binary
}
/******************* Function to Write Data to Flash **********************/

void Flash_Write(PJTAGdata P, unsigned long int A, word D)
{
    Set_Data(P,D);    // Output data on bus
    Set_Address(P,A); // Output address
    P[UCS*2+1] = '0'; // Selects Flash Chip
    P[RD*2+1] = '1';  // RD#=High Data

// !!!! ONLY ONE OF SECTIONS 1 or 2 MAY BE USED - COMMENT OUT THE OTHER !!!!

// SECTION 1 - USE IF STROBE# IS CONNECTED DIRECTLY TO FLASH_WE# - FASTEST

    Send_Data_IN(BSR_Length,P);
    Strobe_Data_In(); // Clocks the Par. Port STROBE line

// SECTION 2 - USE IF DRAM WE# IS CONNECTED DIRECTLY TO FLASH_WE# - SLOWER
//
// P[WR*2+1] = '1';             // WR#=High Data
// P[WRD*2+1] = '0';            // For Read
// Send_Data_IN(BSR_Length,P);  // Can skip if WE# is already High!
// P[WR*2+1] = '0';             // WR#=Low Data
// P[WRD*2+1] = '1';            // For Write access
// Send_Data_IN(BSR_Length,P);
// P[WR*2+1] = '1';             // WR#=High Data again
// P[WRD*2+1] = '0';            // Read access again */
// Send_Data_IN(BSR_Length,P);
}
/************** Function to read input file name and data *****************/

int Input_File_Name_OK (char input_file_name[80])
{
    FILE *in;                           // Points to the input file

    printf ("\nEnter name of input file: ");
    scanf ("%80s", input_file_name);

    if ( (in = fopen (input_file_name, "rb")) == (FILE *) NULL )
    {
        printf ("Could not open %s for input data.\n", input_file_name);
        fclose (in);
        return (FALSE);                 // File not loaded into memory
    }
    else
    {
        printf ("File name is good ..... continuing..... \n");
        fclose (in);
        return (TRUE);                  // File is loaded in memory
    }
}
/****** Function to retrieve info about FLASH manufacturer and Device ****/

void Get_Flash_Device_ID ()
{
    Send_Instruction_IN(strlen(SAMPLE),SAMPLE);
                                            // Sample/Preload to initialize BSR
    Send_Instruction_IN(strlen(EXTEST),EXTEST);
                                            // Configure for External Test
    A=0x0;                                  // Initializer
    Flash_Write(PinState,A,0x90);           // Send command to flash: read ID
    RX = Flash_Read(PinState,A);            // Rd 1 word Flash Device ID
    printf("\nFlash Chip Intelligent ID reads: %4.4xH",RX);// Print first word
    RX = Flash_Read(PinState,A+1);
    printf(" * %4.4xH\n",RX);               // Print second word
    printf("Flash ID for 28F400-T should be: 0089H * 4470H\n");
}
/*** Function checks FLASH status register and displays the contents *****/

void Check_Flash_Status ()
{
    Flash_Write(PinState,A,0x50);           // Clears Status Registers
    Flash_Write(PinState,A,0x70);           // Send command to flash: RD Status
    RX = Flash_Read(PinState, A);
    printf("\nStatus of the FLASH part is: %4.4xH\n",RX);
    printf("FLASH status should be read: 0080H\n");
}
/******** Function to erase the contents of the entire FLASH chip ********/

void Erase_Flash ()
{
    int index;
    unsigned long int blocks[] =
                    {0x0000,0x10000,0x20000,0x30000,0x3C000,0x3D000,0x3E000};
                                    // Above = Starting *word* address of
                                    // each of the blocks in a 28F400BV-T
    printf("\nNow Erasing FLASH......Please be patient.....\n");
    for (index=0; index<=6; index++)
    {
        A=blocks[index];
        Flash_Write(PinState,A,0x20);
        Flash_Write(PinState,A,0xD0);
                                    // Wait until Erase Complete
        do
        {
            Flash_Write(PinState,A,0x70);   // Check Status Register
            RX = Flash_Read(PinState,A);
        }
        while ((RX & 0x80) == FALSE);       // Wait Until Ready again
        printf("Status of FLASH block #%x is: %4.4xH\n", index+1,RX);
        Flash_Write(PinState,A,0x50);       // Clears Status Registers for next
    }                                       // block erase
    printf("FLASH status should be read: 0080H\n");
    printf("FLASH has been erased.....Ready to write data.... \n");
}
/****** Function to program the data in the file into the FLASH **********/

unsigned long int Program_Flash_Data ()
{                           /* Code below outputs data from */
                            /* binary file to the FLASH. Outputs words. */
    A = data_start_address >> 1;    // So that starting point can be remembered
    in = fopen (input_file, "rb");
    printf("\nWriting input file data into FLASH... \n");
    printf("Please be patient.... May take 2-10 seconds per kilobyte.\n");
    while ((c = fgetc(in)) != EOF)
    {               // Code to make a word from two chars
        new_word = 0;                       // Initializes the two byte word
        new_word = (new_word | c);          // Puts first byte into low 8 bits
        c = fgetc(in);                      // Gets second bytes
        high_part = 0;                      // Initializes temporary space
        high_part = (high_part | c);        // Puts second byte into low 8 bits
        high_part = high_part << 8;     // Shifts second byte up 8 bits to top
        new_word = (new_word | high_part);  // Combines low 8 and high
        Flash_Write(PinState,A,0x40);       // Program set-up command
        Flash_Write(PinState,A,new_word);   // Writes 16 bit word

// May add the following section to do status checks for each write
// Not necessary for the very slow speed of parallel port.
// Will severely inhibit performance.

    //  do
    //  {
    //      Flash_Write(PinState,A,0x70);   // Check Status Register
    //      RX = Flash_Read(PinState,A);    // for each word ....
    //  }
    //  while ((RX & 0x80) == FALSE);       // Wait Until Ready again
        ++A;                                // Increments address in word mode
    }
    printf("File has been sucessfully read from disk.\n");
    printf("Data programmed at hex byte location %lxH\n", data_start_address);

    if (fclose (in)) {
        printf ("The file %s was not closed successfully.\n", input_file);
    } else {
        printf ("The file %s was closed successfully.\n", input_file);
    }
    return (A - (data_start_address >> 1));
}
/************ Function to read the upper 32k of FLASH for Debug **********/

void Read_FLASH_Data (char *FileName,
                        unsigned long int AStart,
                        unsigned long int Size)
/* Reads 16 bit words from FLASH chip into binary file starting @ AStart */
{
    FILE *DataFile;
    unsigned long int Address;
    word Data;

    printf("\nNow reading back data for verification of program success...\n");
    printf("Please be patient. May take up to 2 seconds per kilobyte.....\n");

    printf("\nFile starting location in FLASH is %lxH\n", AStart);
    printf("File ending location in FLASH is   %lxH\n", AStart+(Size<<1));

    Flash_Write(PinState,A,0xFF);       // Sets up to read back data
    DataFile = fopen(FileName, "w+b");
    AStart = AStart >> 1;               // For word access addressing
    for (Address = AStart; Address < AStart+Size; Address++)
    {
        Data=Flash_Read(PinState,Address);
        if (fwrite(&Data, sizeof(Data),1,DataFile) != 1) {
            printf("problem writing to file");
        }
    }
    fclose(DataFile);
    printf
    ("\nFile verification image has been written to file ""VERIFY.BIN""...\n");
    printf("WARNING: Verification file will contain one extra byte for\n");
    printf("input files with odd byte counts.\n");
}
/*************************************************************************/
/***************************** BEGIN MAIN ******************************/
/*************************************************************************/

void main ()
{
    if (Input_File_Name_OK (input_file))
    {
        printf                              // On next line...
    ("\n********* INTEL i386EX PROGRAMS FLASH VIA THE JTAG PORT *********\n");
        Fill_JTAG(PinState);                // Initialization string
        Reset_JTAG();                       // Reset the JTAG unit
                                            // Reset board while TRST# is low
                                            // to insure proper startup
        printf("\nWARNING: Reset Evaluation Board now and press any key.\n");
        while (!_kbhit());                  // Waits until a key is hit
        _getch();                           // Throws away character
        Restore_Idle();                     // Used to reset JTAG state machine
        Get_JTAG_Device_ID();           // Get ID - see 386EX manual for code
        Get_Flash_Device_ID();          // Get ID - see flash manual
        Check_Flash_Status();           // Check status register example
        Erase_Flash();                  // Erases the entire Flash chip
        printf("\nEnter starting address of program data in hex bytes: ");
        scanf("%lx",&data_start_address);   // Scans starting address in hex
                                            // Uses word mode below
        i = Program_Flash_Data();       // Opens file and programs FLASH data
        Check_Flash_Status();           // Checks status before continuing
        Read_FLASH_Data("verify.bin", data_start_address, i);
                                            // Copy contents to
                                            // file to verify OK
        printf("\nThe board must now be reset to return to normal operation.");
                                            // Reset board while TRST# is low
                                            // to insure proper startup
        printf("\nWARNING: Reset Evaluation Board now and press any key.\n");
        while (!_kbhit());                  // Waits until a key is hit
        _getch();                           // Throws away character
        Reset_JTAG();                       // Reset TAP to release BSR control
        printf("\n<<<<<<<<<<<<<<<< The end... >>>>>>>>>>>>>>>>>\n\n");
        printf("     Hit any key to return to DOS.\n");
        while (!_kbhit());                  // Waits until a key is hit
        _getch();                           // Throws away character
    }
    else
    {
        printf("File transmission unsuccessful.\n");
        printf("Please check input file and physical connections.\n");
    }
}
/*****************************  END MAIN  ********************************/
