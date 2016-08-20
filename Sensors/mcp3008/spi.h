#ifndef _SPI_H_
#define _SPI_H_

#define SLAVE_0			0x00
#define SLAVE_1			0x01

#define MODE_0			0x00
#define MODE_1			0x01
#define MODE_2			0x02
#define MODE_3			0x03

int spi_write_then_read(int slave_select, unsigned char *buff, int num_of_byte);
int open_spi(int slave_select, int mode, int speed_hz);
int close_spi(int slave_select);



#endif
