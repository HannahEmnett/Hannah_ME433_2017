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
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

#define SLAVE_ADDR 0b0100000 //0x20

void initExpand();
void setExp(unsigned char pin, unsigned char level);
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
    
    TRISAbits.TRISA4=0;
    TRISBbits.TRISB4=1;
    ANSELBbits.ANSB2=0;
    ANSELBbits.ANSB3=0;
    
    __builtin_enable_interrupts();
    
    initExpand();
    

//#include <stdio.h>
    // sprintf(msg, "hello %d", i)
    //dispstr(char string, x, y, c1, c2)
    // int i =0 
    //while msg[i]!= 0
    //dispchar(char c, x ,y, c1, c2)
// row= c-0x20
    //for i= 0:4
    //if x+1 <128 
    //for j=0:7
    //if ascii[row][i]>> j & 1 == 1
    //if (y+j< 128)
    //draw (x+i,y+j, color)
 // some initialization function to set the right speed setting
  // some initialization function to set the right speed setting

  

  while(1) {
      if (((getExp()>>7) & 1) ==1) {setExp(0,1);}
      else {setExp(0,0);}
  }  
}

void initExpand(){
    i2c_master_setup();                       // init I2C2, which we use as a master
  
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1 | 0);
    i2c_master_send(0x00);
    i2c_master_send(0xF0);
    i2c_master_stop(); 
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1 | 0);
    i2c_master_send(0x06);
    i2c_master_send(0xF0);
    i2c_master_stop(); 

    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1 | 0);
    i2c_master_send(0x09);
    i2c_master_send(0x00);
    i2c_master_stop(); 
}

void setExp(unsigned char pin, unsigned char level){  
    unsigned char com = level << pin;
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1 | 0);
    i2c_master_send(0x09);
    i2c_master_send(com);
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