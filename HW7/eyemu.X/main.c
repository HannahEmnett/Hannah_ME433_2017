/* 
 * File:   main.c
 * Author: Han
 *
 * Created on April 18, 2017, 11:13 AM
 */
#include "setup.h"
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "ILI9163C.h" 
#include "i2c_master_noint.h"
#include "helpfunc.h"

#define SLAVE_ADDR 0b1101011 

int main() {
    __builtin_disable_interrupts();
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583); // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    BMXCONbits.BMXWSDRM = 0x0; // 0 data RAM access wait states
    INTCONbits.MVEC = 0x1;// enable multi vector interrupts
    DDPCONbits.JTAGEN = 0;// disable JTAG to get pins back
    
    TRISAbits.TRISA4=0;
    TRISBbits.TRISB4=1;
    ANSELBbits.ANSB2=0;
    ANSELBbits.ANSB3=0;
    
    
    SPI1_init();
    LCD_init();
    LCD_clearScreen(BLACK);
    initIMU();
    __builtin_enable_interrupts();
    
    
    unsigned char r=0; //WhoAmI?
    r= getIMU(SLAVE_ADDR, 0x0F);
    if (r==0b01101001) LATAbits.LATA4=1;
    else LATAbits.LATA4=0;
    
    
    char rawval[14];
    signed short accval[7];
    char buff[100];

    
    while (1) {
        _CP0_SET_COUNT(0);
        signed int xlen1=0, ylen1=0;
        getMult(SLAVE_ADDR,0x20,rawval,14);
        procIMU(rawval,accval,14);
        xlen1=  (55.0*accval[4])/15000.0; 
        ylen1= (55.0*accval[5])/15000.0; 
        drawBar(xlen1,ylen1, WHITE, BLACK);
//        sprintf(buff, "T: %5.2d, GX: %5.2d", accval[0],accval[1]);
//        dispstr(buff, 10, 10, WHITE, BLACK);
//        sprintf(buff, "GY: %5.2d, GZ: %5.2d", accval[2],accval[3]);
//        dispstr(buff, 10, 20, WHITE, BLACK);
        sprintf(buff, "AX: %5.2d, AY: %5.2d", accval[4],accval[5]);
        dispstr(buff, 10, 10, WHITE, BLACK);
//        sprintf(buff, "AZ: %5.2d", accval[6]);
//        dispstr(buff, 10, 40, WHITE, BLACK);
        
        while (_CP0_GET_COUNT()<24000000/10);
        
    }
}

