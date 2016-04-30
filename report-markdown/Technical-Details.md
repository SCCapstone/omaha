## Portable Electronic Ballot ##

The Portable Electronic Ballot, or PEB, is a hand-held cartridge 3.5 inches wide by 3.75 inches tall by 1 inch thick. The top and bottom casings are fastened together by a single security torx screw, and they are separated by a molded plastic ring. The color of that ring indicates the PEB's function: a red band is used for supervisor PEBs while a green or blue band is used for a voter PEB.

Inside the cartridge is a single two-sided printed circuit board with of the electrical components. Affixed to one side is a large-capacity, 3.5-volt battery (essentially a super-AA), a voltage-regulating capacitor, and seven pins used for In-Circuit Serial Programming of the device's microcontroller. The other side of the circuit board has the PIC16 microcontroller, AT45 flash memory chip, the 7.373-megahertz clock, and the numerous components that handle infrared communication with the iVotronic.

#### Logic ####

The PIC16 is an 8-bit microcontroller manufactured by Microchip Technology Inc. that operates at a frequency of 20 megahertz, that can hold up to 4 kilobytes of program memory and that can store a further 320 bytes of data in RAM and EEPROM. The PIC16 uses its Serial Peripheral Interface (SPI) port to transfer data between itself and the flash memory chip, the controller communicates with the IR components via its Universal Asynchronous Receiver/Transmitter (UART) port, and it is able to be reprogrammed using the In-Circuit Serial Programming (ICSP) protocol.

#### Memory ####

The AT45 is a 16-megabit (or 2-megabyte) DataFlash memory chip manufactured by Atmel Corp. that as two additional 528-byte SRAM buffers and is compatible with the SPI standard. The 16 megabits are organized into 4,096 pages, each containing 528 bytes, which are collected into 512 blocks at 16 sectors. All operations act at the page level, and data erase operations can act at block or sector levels as well (for example, a read or write operation can only operate on one page at a time, whereas a delete operation can wipe up to 256 pages at once).

In the PEB, the AT45 stores the ballot information and vote count. The information is encrypted using the Blowfish cipher by the microcontroller before storage, but the cipher key is stored in plaintext on the device.

## iVotronic ##

The iVotronic direct-recording electronic voting machine, in contrast to the PEB, is a large, table-top device. A touch screen LCD dominates the front, and navigation buttons adorn the top and bottom margins. To the left is a slot that fits the PEB, and at the bottom-left of the machine is a headphone jack for non-English speakers. The back of the device is blank, save for a kickstand. Like the molded plastic ring on the PEB, the iVotronic's soft rubber handles are color coded according to the machine's type, where red handles are used for supervisor iVotronics and voter iVotronics get blue handles.

The iVotronic's motherboard is a multi-layer printed circuit board that appears to be an unnavigable maze of wires and electronics. In the center of the board is the machine's processor, an Intel 386EX, and its support circuitry, but even more noticeable are the raised modules that provide memory storage and audio/video processing. Coming off of the motherboard are numerous cables that connect the IR circuitry, the LCD touchscreen, and the nickel-metalhydride backup battery.

#### Logic ####

The Intel 386EX microprocessor is a version of Intel's i386 processor built specially for embedded systems. The microprocessor is built on 32-bit architecture, operates at a frequency between 16 and 25 megahertz, and uses both serial and parallel I/O units to communicate with the infrared, memory, and A/V components. Although the 386EX can be externally accessed through Joint Test Action Group (JTAG) protocols, the visible JTAG pins on the iVotronic's circuit board do not appear to properly connect to the appropriate pins on the microprocessor.

#### Memory ####

A consistent and reliable memory storage is the single most important requirement of for the iVotronic. As a result, the iVotronic stores is information in duplicate, where each of the two copies are themselves stored across two memory chips. Every chip in a pair wired in parallel with the other so that data sent to one chip is received by both.

AMD's Am29LV 16-megabit flash memory chips comprise all four memory stores of the iVotronic: one pair is situated on the two memory modules, while another pair is soldered directly to the circuit board between the two modules. Each chip organizes its memory addresses into thirty-two 64-kilobyte sectors and complies with the Common Flash Interface protocol.

The two other memory chips on the board are Hynix Semiconductor's HY62V SRAM chips, each of which can store 4 megabits of memory. These are most likely used for RAM storage by the 386EX as they are only a quarter of the size of the other four memory chips, so they cannot be used to store a full copy of the information stored in those other chips.

In addition to the on-board memory chips, an iVotronic also has a slot for a Compact Flash drive (essentially a large SD card) which are used to log errors and other diagnostic information.

#### User Interface ####

The iVotronic dedicates its two other modules to the audio and video information sent to the user. The sound module uses a PIC18 microcontroller from Microchip and a combination of digital-to-analog converters and amplifiers to store and process the audio files for the visually impaired and non-English speakers in order to comply with ADA regulations. The video module's S1D1380 graphics processor by Epson generates graphics, text, and other visual information, which is encoded by National Semiconductor's DS90C low-voltage signaler and transmitted to the LCD touchscreen. Tactile input from the touchscreen is not processed by the video module, but is instead fed directly back to the 386EX for processing.

## Infrared Communications ##

The PEB and the iVotronic communicate with each other via infrared signals. The information sent is comprised of a basic data exchange language where the iVotronic sends requests for reading or writing information to the PEB's memory, and the PEB complies. These infrared signals are packaged into a network stack protocol standardized by the Infrared Data Association (IrDA) by the respective device's processor or controller. These IrDA packets are encoded and decoded by one or more IR1000 chips from Texas Instruments, and the packets are subsequently transmitted and received by Vishay Semiconductor's TFDU4100.

Externally, the communications are synchronized via magnetic field switches on both devices where are activated by a neodymium magnet adhered to the other device's casing. Internally, each device synchronizes their processor/controller with the IR1000 encoders via an external oscillator. For example, the PEB uses a 7.373-megahertz clock signal that is stepped-down by two D-type flip-flop gates to 1.843 megahertz for the IR1000.