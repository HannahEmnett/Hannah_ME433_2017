/* 
 * File:   main.c
 * Author: Han
 *
 * Created on April 13, 2017, 4:40 PM
 */
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "ILI9163C.h" 



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

void initBar();
void dispstr(char * str, unsigned short xpos, unsigned short ypos, unsigned short c1, unsigned short c2);
void dispc(char c, unsigned short xpos, unsigned short ypos, unsigned short c1, unsigned short c2);

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
    
    SPI1_init();
    LCD_init();
    LCD_clearScreen(BLACK);
    
    __builtin_enable_interrupts();
    
    char buff[128];
    char barbuff[5];
    int num =0,mid=64;
    sprintf(buff, "hello world!");
    dispstr(buff,28,32,WHITE,BLACK);
    sprintf(buff, "              ");
    sprintf(buff, "FPS: ");
    dispstr(buff,45,65,WHITE,BLACK);
    sprintf(buff, "      ");
    sprintf(barbuff, " ");
    initBar();
    float time=0;
    while (1) {
        num=100;
        while(num>=0){
            sprintf(buff, "%d", num);
            dispstr(buff,100,32,WHITE,BLACK);
            if (num>=50){dispc(barbuff[0],mid+num-50,45,WHITE,BLACK);} 
            else {dispc(barbuff[0],mid-(50-num),45,BLACK,WHITE);}
            if (num==0){initBar();}
            time=(float) 24000000.0/_CP0_GET_COUNT();
            sprintf(buff, "%4.2f", time);
            dispstr(buff,75,65,WHITE,BLACK);
            while (_CP0_GET_COUNT()<48000000/2/5); //5hz
            _CP0_SET_COUNT(0);
            dispstr(buff,100,32,BLACK, BLACK);
            sprintf(buff, "    ");
            dispstr(buff,75,75,BLACK,BLACK);
            num--;
        }
    }
}

void initBar(){
    char bar1=0;
    char buff3[2];
    sprintf(buff3, " "); 
    for (bar1 =0; bar1<=50; bar1++){
        dispc(buff3[0],bar1+64,45,BLACK,WHITE);
        dispc(buff3[0],-bar1+64,45,BLACK,BLACK);
    }
}

void dispstr(char * str, unsigned short xpos, unsigned short ypos, unsigned short c1, unsigned short c2) {
    int k=0;
    while (str[k]){
        dispc(str[k], xpos+k*5, ypos, c1, c2);
        k++;
    }
}

void dispc(char c, unsigned short xpos, unsigned short ypos, unsigned short c1, unsigned short c2) {
    int i=0, j=0, row=0;
    row= c - 0x20;
    for (i=0; i<=4; i++) {
        if ((xpos+i) <128) {
            for (j=0; j<=7; j++) {
                if ((ypos+j)< 128){
                    if (ASCII[row][i]>> j & 1 == 1) {LCD_drawPixel(xpos+i, ypos+j, c1);}
                    else {LCD_drawPixel(xpos+i, ypos+j, c2);}
                }
            }
        }
    }
}
  