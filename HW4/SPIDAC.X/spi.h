#ifndef _EXAMPLE_SPI_H    /* Guard against multiple inclusion */
#define _EXAMPLE_SPI_H

void setVoltage(unsigned char cha, unsigned char volt);
void spi_init();
unsigned char spi_io(unsigned char o);

#endif

