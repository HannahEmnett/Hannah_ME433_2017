#include <xc.h>
#include "spi.h"

 // constants, funcs for startup and UART
// Demonstrates spi by accessing external ram
// PIC is the master, ram is the slave
// Uses microchip 23K256 ram chip (see the data sheet for protocol details)
// SDO4 -> SI (pin F5 -> pin 5)
// SDI4 -> SO (pin F4 -> pin 2)
// SCK4 -> SCK (pin B14 -> pin 6)
// SS4 -> CS (pin B8 -> pin 1)
// Additional SRAM connections
// Vss (Pin 4) -> ground
// Vcc (Pin 8) -> 3.3 V
// Hold (pin 7) -> 3.3 V (we don't use the hold function)
// 
// Only uses the SRAM's sequential mode
//
#define CS LATAbits.LATA4     // chip select pin

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte SPIRBF
    ;
  }
  return SPI1BUF;
}

// initialize spi4 and the ram module
void spi_init() {
  // set up the chip select pin as an output
  // the chip select pin is used by the sram to indicate
  // when a command is beginning (clear CS to low) and when it
  // is ending (set CS high)
  
  

  // Master - SPI4, pins are: SDI4(F4), SDO4(F5), SCK4(F13).  
  // we manually control SS4 as a digital output (F12)
  // since the pic is just starting, we know that spi is off. We rely on defaults here
  
  // setup spi4
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 0xAD;            // baud rate to 10 MHz [SPI4BRG = (80000000/(2*desired))-1] 10MHz 0x3, 1MHz, 0x17, 100khz 0xEF
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.SSEN=0;
  SPI1CONbits.MODE32=0;
  SPI1CONbits.MODE16=0;
  
  RPA1Rbits.RPA1R=0b0011; //A1
  SDI1Rbits.SDI1R=0b0100; //B8
  SS1Rbits.SS1R=0000;
  TRISAbits.TRISA4=0;
  CS = 1;
  SPI1CONbits.ON = 1;       // turn on spi 1                          // send a ram set status command.
  //CS = 0;                   // enable the ram
  //spi_io(0x01);             // ram write status
  //spi_io(0x41);             // sequential mode (mode = 0b01), hold disabled (hold = 0)
  //CS = 1;                   // finish the command
}

// write len bytes to the ram, starting at the address addr
void setVoltage(char cha, char volt){
    unsigned char val1=0, val2=0, valval=0;
    valval= (unsigned char) volt >> 4;
    if (cha == 1) {
        val1= (unsigned char) (0b11110000 || valval); 
    }
    else {
        val1= (unsigned char) (0b01110000 || valval);
    }
    val2 = (unsigned char) volt << 4;
    char n=0;
    CS = 0;
    CS=0;
    n=spi_io(val1);
    n=spi_io(val2);
    CS= 1;
    CS=1;
}
/*void ram_write(unsigned short addr, const char data[], int len) {
  int i = 0;
  CS = 0;                        // enable the ram by lowering the chip select line
  spi_io(0x2);                   // sequential write operation
  spi_io((addr & 0xFF00) >> 8 ); // most significant byte of address
  spi_io(addr & 0x00FF);         // the least significant address byte
  for(i = 0; i < len; ++i) {
    spi_io(data[i]);
  }
  CS = 1;                        // raise the chip select line, ending communication
}

// read len bytes from ram, starting at the address addr
void ram_read(unsigned short addr, char data[], int len) {
  int i = 0;
  CS = 0;
  spi_io(0x3);                   // ram read operation
  spi_io((addr & 0xFF00) >> 8);  // most significant address byte
  spi_io(addr & 0x00FF);         // least significant address byte
  for(i = 0; i < len; ++i) {
    data[i] = spi_io(0);         // read in the data
  }
  CS = 1;
}*/
