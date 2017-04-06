
#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro
#include <stdio.h>
#include <stdlib.h>
#include "spi.h"
#include "math.h"


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

#define MAX_TIME 24000
#define sampleRate 1000
#define freq 10
char make_sine();
char make_tri();
char sine[sampleRate];
char tri[sampleRate];

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

    spi_init();
    make_sine();
    make_tri();
    while(1) {
        char j=0;
        while (j=0, j< sampleRate, j++){
            setVoltage(0,sine[j]); //1 for B, 0 for A
            setVoltage(1,tri[j]);
        }
        
        
    }
}
//the DAC outputs a 10Hz sine wave on VoutA and a 5Hz triangle wave on VoutB, updating the values 1000 times a second 
char make_sine(){
    char i=0;
    for (i=0, i<sampleRate, i++) {
        sine[i] = 3*sin(freq * (2 * pi) * i /sampleRate);
    }
}
char make_tri(){
    char i=0;
    for (i=0, i<sampleRate, i++){
        tri[i]= 3*(i/sampleRate);
    }
    
}