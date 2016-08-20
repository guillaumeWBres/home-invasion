#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include "spi.h"
#include "mcp3008.h"

int mcp3008_init(struct mcp3008_t *mcp, int chipselect){
	unsigned int fd;
	switch(chipselect){
		case 0:
			fd = open("/dev/spidev0.0", O_RDWR);
		break;
		case 1:
			fd = open("/dev/spidev0.1", O_RDWR);
		break;
		default:
			printf("wrong spidev0.%d value\r\n", chipselect);
			return -1;
		break;
	}
	
	if(fd < 0){
		printf("error opening spidev0.%d\r\n", fd);
		return -1;
	} 
	
	mcp->fd = fd;
	open_spi( mcp->fd, SPI_MODE_0, 1000000 );
	return 0;

}

void mcp3008_exit(struct mcp3008_t *mcp){
	close(mcp->fd);
}

int mcp3008_capture(struct mcp3008_t *mcp, uint8_t channel, uint8_t capture_mode){
	char buff[3];
	int ret;
	buff[0] = START_MASK;
	buff[1] = channel|capture_mode;
	buff[2] = 0x00;
	spi_write_then_read(mcp->fd, buff, 3);	
	ret = ((buff[1]&0x03)<<8); // attention cast char > uint16t
	ret |= (buff[2]&0xff);
	return ret;
}
