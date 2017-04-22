#include "helpfunc.h"

void drawBar(signed int xlen, signed int ylen, unsigned short c1, unsigned short c2){
    char xstart=63; char ystart=63; signed char i=0;
        if (ylen > 0) {
            if (ylen> 16) drawArrow(xstart, ystart-ylen,c1,c2,1);
            for (i = 0; i < ylen; i=i+8) drawBarChar(xstart, ystart - i, c1,c2,0);
            for (i = ylen+8; i < 100; i=i+8) drawBarChar(xstart, ystart - i, c2, c2,0);
            for (i = 0; i > -100; i=i-8) drawBarChar(xstart, ystart - i, c2,c2,0);
        } else {
            if (ylen< -16) drawArrow(xstart, ystart-ylen,c1,c2,3);
            for (i = 0; i > ylen; i=i-8) drawBarChar(xstart, ystart - i, c1,c2,0);
            for (i = ylen-8; i > -100; i=i-8) drawBarChar(xstart, ystart - i, c2,c2,0);
            for (i = 0; i < 100; i=i+8) drawBarChar(xstart, ystart - i, c2,c2,0);
        }
        if (xlen > 0) {
            if (xlen>10) drawArrow(xstart-xlen, ystart,c1,c2,0);
            for (i = 0; i < xlen; i=i+5) drawBarChar(xstart-i, ystart, c1,c2,1);
            for (i = xlen+5; i < 100; i=i+5) drawBarChar(xstart-i, ystart, c2, c2,1);
            for (i = 0; i > -100; i=i-5) drawBarChar(xstart-i, ystart, c2,c2,1);
        } else {
            if (xlen<-10) drawArrow(xstart-xlen, ystart,c1,c2,2);
            for (i = 0; i > xlen; i=i-5) drawBarChar(xstart-i, ystart, c1,c2,1);
            for (i = xlen-5; i > -100; i=i-5) drawBarChar(xstart-i, ystart, c2,c2,1);
            for (i = 0; i < 100; i=i+5) drawBarChar(xstart-i, ystart, c2,c2,1);
        }
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