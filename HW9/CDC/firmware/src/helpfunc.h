#ifndef _EXAMPLE_FILE_NAME_H    /* Guard against multiple inclusion */
#define _EXAMPLE_FILE_NAME_H


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

#endif