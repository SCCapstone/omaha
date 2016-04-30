### Hardware Overview

Once we were able to open the cases on everything, we quickly inventoried all of the interesting chips in the Portable Electronic Ballot (PEB) unit and began figuring out how components connected to each other.

The PEB is controlled by a PIC16 microcontroller, which is programmable through a set of programming pins which extend from the back of the PCB board.  A 7.37 MHz crystal oscillator serves as the central clock for the device, and is directly connected to the PIC16.

The PEB is powered during programming through the programming pins, but during normal operation runs off of an onboard 3.3V lithium battery soldered onto the back of the PCB board. To prevent the battery from running down during storage, a magnetic field switch is wired into the main power circuit, turning off the PEB when it is not slotted into a PEB cradle or iVotronic voting machine. A hand-held magnet was used during our testing to trick the PEB into powering on. It takes approximately 20-25 seconds for power to drain out of the circuits after a PEB is powered down.

The PIC16's SPI pins are wired to a 2 MB (16 Mbit) Flash memory chip. This chip serves as the primary offline storage unit for voting data during normal operations.

For communications, the PEB uses the IrDA infrared communications protocol suite. The infrared hardware is connected to the central clocking crystal through a D flip-flop, which steps down the clock rate by a factor of 4. The PIC16's UART TX/RX lines are wired directly to a Texas Instruments TIR1000 IrDA encoder/decoder unit. This unit translates incoming bytes into pulses of the correct duration for IrDA communications. To actually get the bits off the board, a bi-directional IrDA transceiver (connected to the encoder/decoder) is located near the bottom of the board.

### Our Setup

We had 2 PEB units, courtesy of Dr. Buell, and used these for all of our work during this project.

Our toolkit contained:

 - [Bus Pirate v3.6](http://dangerousprototypes.com/docs/Bus_Pirate) - A swiss-army-knife of embedded electronic tools. Able to understand SPI, I2C, UART, JTAG, and other embedded communication protocols.
 - [PICKit3](https://en.wikipedia.org/wiki/PICKit) - A programmer unit for PIC-series microcontrollers.
 - Digital Multimeter - Used to probe the circuitry of the PEB, and to tell when IO pins were turned on.
 - A bar magnet - Used to turn on the PEB for testing.
 - Torx (security version) screwdriver bit - Used to open the PEB's plastic case (see [[Disassembly Procedures]]).


### The Failed Hexdump

We wanted to get a hexdump of the flash memory of the PIC16 in order to understand the ES&S code. This meant that we needed a programmer/debugger unit compatible with the PIC16. We ordered a [PICKit3][pickit] unit off of eBay, and waited a week or two for it to ship in from Hong Kong, working on other aspects of the project while we waited.

   [pickit]: https://en.wikipedia.org/wiki/PICKit

Upon arrival, we hooked the PICKit up to the PEB and attempted to get our hexdump. Unfortunately, [Code Protection][wikipedia-code-protection] was enabled for this chip, which meant that our hex dump consisted almost entirely of `0xFF` bytes.

   [wikipedia-code-protection]: https://en.wikipedia.org/wiki/Code_protection

While we were disappointed that we wouldn't get to benefit from the ES & S programmer's work, we decided to forge ahead anyway with our own programming efforts. This proved more difficult than expected.


### Programming the PIC16, part I

The PIC16 is apparently an unloved member of the modern Microchip PIC family of processors. Few code samples exist online for this processor, compared to the hundreds to thousands of code libraries, samples, and tutorials available for its bigger brother, the PIC18. There were a handful of helpful code bits on the internet that we found, allowing us to write at least a simple 'blink' demo (powering up 1 of the IO pins for 1 second, and then turning the IO pin off for 1 second, in an infinite loop), and begin work on UART and SPI communications.

The chief problem for us was that our PIC16 had limited connectivity to the outside world. It was not meant to be debugged. The programming header pins on the back of the board are wired such that the PIC16 can be programmed, but cannot communicate back to the programmer unit. The UART pins needed for that communication are wired directly into the IrDA hardware, and are not wired to any easily accessible external interface. 

After some experiments with the Bus Pirate, trying to spy on the UART pins by manually holding wires to them, we decided to do a sanity check.


### The Zombie PEB

Thanks to earlier tests, we knew that at least one of the PEBs worked perfectly. This PEB had been set aside early on as a "golden" item, not to be messed with if possible. The other PEB had been designated our "work" PEB, and was the subject of much of our early testing.

When we finally decided to do a sanity check, we opened up the known-good PEB, and held the multimeter's probes to one of the UART pins, and the PIC16's ground pin. We observed a small voltage on the pin, which fluctuated frequently. We inferred this to mean the PIC16 was attempting to communicate over the UART lines.

We then took out our work PEB, and performed the same test after loading a blink demo on the PIC16 that should have lit up the UART pins. After many seconds (and several iterations of our code), we observed no voltage, and determined that the chip was very likely dead. The chip appeared fine to the programmer, but simply would not run.


### Programming the PIC16, part II

We made a command decision to try programming the known-good PEB. We first tried a hexdump, just in case the Code Protection bit had not been enabled. As expected, Code Protection was on, so we moved on to programming the PIC16 with a 'blink' demo.

This proved successful, and we immediately attempted to move on to UART communications. However, due to the small voltages present on the UART lines, our Bus Pirate unit was not able to observe the communications.

We determined soon after that the IrDA encoder/decoder unit would not handle encapsulating data sent across the UART lines in the IrDA packet format. Its sole purpose was to get pulse timings correct. To send data as IrDA packets, we would have had to write our own IrDA protocol stack to handle encapsulating messages with the appropriate headers/trailers and checksums.

Due to time constraints, we did not explore this route.


### Final Results

We were able to programmatically blink IO pins on and off, including the pins connected to the programming header. This means that future teams may be able to devise a setup to get bits off the PIC16. We however, did not have time to perfect this, but encourage future teams to explore this route, as well as the IrDA stack.
