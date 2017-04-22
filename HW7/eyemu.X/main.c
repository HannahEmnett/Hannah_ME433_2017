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

#define SLAVE_ADDR 0b1101011 

void initIMU();
void setIMU(unsigned char add, unsigned char reg, unsigned char value);
unsigned char getIMU(unsigned char add, unsigned char reg);
void getMult(unsigned char add, unsigned char reg, unsigned char * data, int length);
void drawBar(signed int xlen, signed int ylen, unsigned short c1, unsigned short c2);
void dispstr(char * str, unsigned short xpos, unsigned short ypos, unsigned short c1, unsigned short c2);
void dispc(char c, unsigned short xpos, unsigned short ypos, unsigned short c1, unsigned short c2);
void procIMU(unsigned char * data, signed short * procdat, int length);
void drawArrow(unsigned short xpos, unsigned short ypos, unsigned short c1, unsigned short c2, char xside);
void drawBarChar(unsigned short xpos, unsigned short ypos, unsigned short c1, unsigned short c2,char xside);

signed char cury=0;
signed char curx=0;

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
    unsigned char r=0;
    r= getIMU(SLAVE_ADDR, 0x0F);
    if (r==0b01101001) LATAbits.LATA4=1;
    else LATAbits.LATA4=0;
    char rawval[14];
    signed short buffed[7];
    signed short accval[7];
    char buff[100];
    char barbuff[100];

    
    while (1) {
        
            //if (num>=50){dispc(barbuff[0],mid+num-50,45,WHITE,BLACK);} 
            //else {dispc(barbuff[0],mid-(50-num),45,BLACK,WHITE);}
            //if (num==0){initBar();}
        
        _CP0_SET_COUNT(0);
//        char h=0;
//        for (h=0; h<5;h++){
//            getMult(SLAVE_ADDR,0x20,rawval,14);
//            procIMU(rawval,accval,14);
//            char n=0;
//            for (n=0;n<7;n++){
//            accval[n]=accval[n]+rawval[n];
//            }
//        }
//        char n=0;
//        for (n=0;n<7;n++){
//            accval[n]=accval[n]/4;
//            }
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
        
        while (_CP0_GET_COUNT()<24000000/2);
        //drawBar(xlen1,ylen1, BLACK, BLACK);
        
    }
}

void drawBar(signed int xlen, signed int ylen, unsigned short c1, unsigned short c2){
    char xstart=63; char ystart=63; char buff3[2]; signed char i=0;
    //sprintf(buff3, "_|"); 
    //drawArrow(curx+5+xstart,ystart,c2,c2, 1);
    //drawArrow(xstart,cury+8+ystart,c2,c2, 0);
//        if (xlen> curx){
//            for (i=curx; i<(xlen-curx); i=i+5){
//                if (i<0) dispc(buff3[0],xstart+i,ystart,c2,c2);
//                else dispc(buff3[0],xstart+i,ystart,c1,c2);
//            }
//        }
//        else {
//            for (i=curx; i<(xlen-curx); i=i-5){
//                if (i<0)dispc(buff3[0],xstart+i,ystart,c1,c2);
//                else dispc(buff3[0],xstart+i,ystart,c2,c2);
//            }
//        }
//        if (ylen> cury){
//            for (i=cury; i<(ylen-cury); i=i+8){
//                if (i<0) dispc(buff3[1],xstart,ystart+i,c2,c2);
//                else dispc(buff3[1],xstart,ystart+i,c1,c2);
//            }
//        }
//        else {
//            for (i=cury; i<(ylen-cury); i=i-8){
//                if (i<0) dispc(buff3[1],xstart,ystart+i,c1,c2);
//                else dispc(buff3[1],xstart,ystart+i,c2,c2);
//            }
//        }
//        curx=xlen; cury=ylen;
        if (ylen > 0) {
            if (ylen> 16) drawArrow(xstart, ystart-ylen,c1,c2,1);
            for (i = 0; i < ylen; i=i+8) {
                drawBarChar(xstart, ystart - i, c1,c2,0);
                //dispc(buff3[1],xstart + 1, ystart - i, c1,c2);
            }
            for (i = ylen+8; i < 100; i=i+8) {
                drawBarChar(xstart, ystart - i, c2, c2,0);
                //dispc(buff3[1],xstart + 1, ystart - i, c2,c2);
            }
            for (i = 0; i > -100; i=i-8) {
                drawBarChar(xstart, ystart - i, c2,c2,0);
                //dispc(buff3[1],xstart + 1, ystart - i, c2,c2);
            }
        } else {
            if (ylen< -16) drawArrow(xstart, ystart-ylen,c1,c2,3);
            for (i = 0; i > ylen; i=i-8) {
                drawBarChar(xstart, ystart - i, c1,c2,0);
                //dispc(buff3[1],xstart + 1, ystart - i, c1,c2);
            }
            drawArrow(xstart, ystart+ylen,c1,c2,0);
            for (i = ylen-8; i > -100; i=i-8) {
                drawBarChar(xstart, ystart - i, c2,c2,0);
                //dispc(buff3[1],xstart + 1, ystart - i, c2,c2);
            }
            for (i = 0; i < 100; i=i+8) {
                drawBarChar(xstart, ystart - i, c2,c2,0);
                //dispc(buff3[1],xstart + 1, ystart - i, c2,c2);
            }
        }
    if (xlen > 0) {
            if (xlen>10) drawArrow(xstart-xlen, ystart,c1,c2,0);
            for (i = 0; i < xlen; i=i+5) {
                drawBarChar(xstart-i, ystart, c1,c2,1);
                //dispc(buff3[1],xstart + 1, ystart - i, c1,c2);
            }
            for (i = xlen+5; i < 100; i=i+5) {
                drawBarChar(xstart-i, ystart, c2, c2,1);
                //dispc(buff3[1],xstart + 1, ystart - i, c2,c2);
            }
            for (i = 0; i > -100; i=i-5) {
                drawBarChar(xstart-i, ystart, c2,c2,1);
                //dispc(buff3[1],xstart + 1, ystart - i, c2,c2);
            }
        } else {
            if (xlen<-10) drawArrow(xstart-xlen, ystart,c1,c2,2);
            for (i = 0; i > xlen; i=i-5) {
                drawBarChar(xstart-i, ystart, c1,c2,1);
                //dispc(buff3[1],xstart + 1, ystart - i, c1,c2);
            }
            for (i = ylen-5; i > -100; i=i-5) {
                drawBarChar(xstart-i, ystart, c2,c2,1);
                //dispc(buff3[1],xstart + 1, ystart - i, c2,c2);
            }
            for (i = 0; i < 100; i=i+5) {
                drawBarChar(xstart-i, ystart, c2,c2,1);
                //dispc(buff3[1],xstart + 1, ystart - i, c2,c2);
            }
        }
    //drawArrow(curx+5,ystart,c1,c2, 1);
    //drawArrow(xstart,cury+8,c1,c2, 0);
}
void drawBarChar(unsigned short xpos, unsigned short ypos, unsigned short c1, unsigned short c2,char xside){
    char arrx[5]= {0x08, 0x08, 0x08, 0x08, 0x08};
    char arry[5]= {0x00, 0x00, 0xff, 0x00, 0x00};
    int i=0, j=0;
    for (i=0; i<=4; i++) {
        if ((xpos+i) <128) {
            for (j=0; j<=7; j++) {
                if ((ypos+j)< 128){
                    if (xside){
                        if (arrx[i]>> j & 1 == 1) {LCD_drawPixel(xpos+i, ypos+j, c1);}
                        else {LCD_drawPixel(xpos+i, ypos+j, c2);}
                    }
                    else{
                        if (arry[i]>> j & 1 == 1) {LCD_drawPixel(xpos+i, ypos+j, c1);}
                        else {LCD_drawPixel(xpos+i, ypos+j, c2);}
                    }
                }
            }
        }
    }
}
void drawArrow(unsigned short xpos, unsigned short ypos, unsigned short c1, unsigned short c2, char xside){
    char arrx[5]= {0x00, 0x08, 0x1C, 0x3E, 0x7F};
    char arry[5]= {0xC0, 0xF0, 0xFC, 0xF0, 0xC0};
    char arrnx[5]={0x7F, 0x3E, 0x1C, 0x08, 0x00};
    char arrny[5]= {0x03,0x0F,0x3F, 0x0F, 0x03};
    int i=0, j=0;
    for (i=0; i<=4; i++) {
        if ((xpos+i) <128) {
            for (j=0; j<=7; j++) {
                if ((ypos+j)< 128){
                    if (xside==0){
                        if (arrx[i]>> j & 1 == 1) {LCD_drawPixel(xpos+i, ypos+j, c1);}
                        else {LCD_drawPixel(xpos+i, ypos+j, c2);}
                    }
                    else if (xside== 1){
                        if (arry[i]>> j & 1 == 1) {LCD_drawPixel(xpos+i, ypos+j, c1);}
                        else {LCD_drawPixel(xpos+i, ypos+j, c2);}
                    }
                    else if (xside==2){
                        if (arrnx[i]>> j & 1 == 1) {LCD_drawPixel(xpos+i, ypos+j, c1);}
                        else {LCD_drawPixel(xpos+i, ypos+j, c2);}
                    }
                    else {
                        if (arrny[i]>> j & 1 == 1) {LCD_drawPixel(xpos+i, ypos+j, c1);}
                        else {LCD_drawPixel(xpos+i, ypos+j, c2);}
                    }
                }
            }
        }
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

void initIMU(){
    i2c_master_setup(); // init I2C2, which we use as a master
    setIMU(SLAVE_ADDR,0x10, 0b10000010); //CTRL1_XL
    setIMU(SLAVE_ADDR,0x11, 0b10001000); //CTRL2_G
    setIMU(SLAVE_ADDR,0x12, 0b00000100); //CTRL3_C
}

void setIMU(unsigned char add, unsigned char reg, unsigned char value){  
    i2c_master_start();
    i2c_master_send(add<<1 | 0);
    i2c_master_send(reg);
    i2c_master_send(value);
    i2c_master_stop(); 
}

unsigned char getIMU(unsigned char add, unsigned char reg){
    unsigned char r=0;
    i2c_master_start();
    i2c_master_send(add<<1 | 0);
    i2c_master_send(reg);
    i2c_master_restart();
    i2c_master_send((add<<1) | 1);
    r=i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return r;
} 

void getMult(unsigned char add, unsigned char reg, unsigned char * data, int length){
    i2c_master_start();
    i2c_master_send(add<<1 | 0);
    i2c_master_send(reg);
    i2c_master_restart();
    i2c_master_send((add<<1) | 1);
    int i=0;
    for (i=0; i<=length-1; i++){
        data[i]=i2c_master_recv();
        if (i==length-1) i2c_master_ack(1);
        else i2c_master_ack(0);
    }
    i2c_master_stop();
}

void procIMU(unsigned char * data, signed short * procdat, int length) {
    int i=0,j=0;
    while (i<=length-1){
        procdat[j]= (data[i+1] << 8) | data[i];
        i=i+2;
        j++;
    }
}