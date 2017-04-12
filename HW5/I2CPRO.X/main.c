/* 
 * File:   main.c
 * Author: Han
 *
 * Created on April 11, 2017, 4:26 PM
 */
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <stdio.h>
#include <stdlib.h>
#include "i2c_help.h" 


// DEVCFG0
#pragma config DEBUG = 0b10 // no debugging
#pragma config JTAGEN = 0b0 // no jtag
#pragma config ICESEL = 0b11 // use PGED1 and PGEC1
#pragma config PWP = 0x3f//0b111111111 // no write protect
#pragma config BWP = 0b0 // no boot write protect
#pragma config CP = 0b0 // no code protect

// DEVCFG1
#pragma config FNOSC = 0b011 // use primary oscillator with pll
#pragma config FSOSCEN = 0b0 // turn off secondary oscillator
#pragma config IESO = 0b0 // no switching clocks
#pragma config POSCMOD = 0b10 // high speed crystal mode
#pragma config OSCIOFNC = 0b1// free up secondary osc pins
#pragma config FPBDIV = 0b00 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = 0b10 // do not enable clock switch
#pragma config WDTPS = 0b10100 // slowest wdt
#pragma config WINDIS = 0b1 // no wdt window
#pragma config FWDTEN = 0b0 // wdt off by default
#pragma config FWDTWINSZ = 0b11 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = 0b001 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = 0b111 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = 0b001 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = 0b001 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = 0b0 // USB clock on

// DEVCFG3
#pragma config USERID = 0b0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = 0b0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0b0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 0b1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 0b1 // USB BUSON controlled by USB module

// Demonstrate I2C by having the I2C1 talk to I2C5 on the same PIC32 (PIC32MX795F512H)
// Master will use SDA1 (D9) and SCL1 (D10).  Connect these through resistors to
// Vcc (3.3 V) (2.4k resistors recommended, but around that should be good enough)
// Slave will use SDA5 (F4) and SCL5 (F5)
// SDA5 -> SDA1
// SCL5 -> SCL1
// Two bytes will be written to the slave and then read back to the slave.
#define SLAVE_ADDR 0x20
void initExpand();
void setExp(unsigned char pin);
unsigned char getExp();

int main() {
    __builtin_disable_interrupts();
    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;
    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;
    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    // do your TRIS and LAT commands here
    __builtin_enable_interrupts();
    TRISAbits.TRISA4=0;
    TRISBbits.TRISB4=1;
    ANSELBbits.ANSB2=0;
    ANSELBbits.ANSB3=0;

 // some initialization function to set the right speed setting


  // some initialization function to set the right speed setting
  i2c_master_setup();                       // init I2C2, which we use as a master
  i2c_master_start();
  i2c_master_send(SLAVE_ADDR<<1 | 0);
  i2c_master_send(0b00000000);
  i2c_master_send(0b11110000);
  i2c_master_stop(); 
  unsigned char val =0;
  while(1) {
    
//    i2c_master_start();                     // Begin the start sequence
//    i2c_master_send(SLAVE_ADDR << 1);       // send the slave address, left shifted by 1, 
//                                            // which clears bit 0, indicating a write
//    i2c_master_send(master_write0);         // send a byte to the slave       
//    i2c_master_send(master_write1);         // send another byte to the slave
//    i2c_master_restart();                   // send a RESTART so we can begin reading 
//    i2c_master_send((SLAVE_ADDR << 1) | 1); // send slave address, left shifted by 1,
//                                            // and then a 1 in lsb, indicating read
//    master_read0 = i2c_master_recv();       // receive a byte from the bus
//    i2c_master_ack(0);                      // send ACK (0): master wants another byte!
//    master_read1 = i2c_master_recv();       // receive another byte from the bus
//    i2c_master_ack(1);                      // send NACK (1):  master needs no more bytes
//    i2c_master_stop();                      // send STOP:  end transmission, give up bus

      //setExp(0b00001111);
      //setExp(0b00);
      //val= getExp();
    char r=0;
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1 | 0);
    i2c_master_send(0x09);
    i2c_master_restart();
    i2c_master_send((SLAVE_ADDR<<1) | 1);
    r=i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
      if (r==(0b10000000)){LATAbits.LATA4=0;}
      else {LATAbits.LATA4=1;}
      //else {setExp(0x01,0x01);}
      _CP0_SET_COUNT(0);
      while (_CP0_GET_COUNT()<24000);
    
  }  
}

void initExpand(){
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1 | 0);
    i2c_master_send(0x00);
    i2c_master_send(0xf0);
    i2c_master_stop(); 
}
void setExp(unsigned char pin){  //pin and level in hex
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1 | 0);
    i2c_master_send(0x09);
    i2c_master_send(0b00000001);
    //i2c_master_send(level);
    i2c_master_stop();
}
unsigned char getExp(){
    unsigned char r=0;
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1 | 0);
    i2c_master_send(0x09);
    i2c_master_restart();
    i2c_master_send((SLAVE_ADDR<<1) | 1);
    r=i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return r;
}