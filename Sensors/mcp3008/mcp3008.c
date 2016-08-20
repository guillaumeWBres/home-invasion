#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include "mcp3008.h"
#include "spi.h"

int mcp3008_init(struct mcp3008_t *mcp, int chipselect){
	unsigned int fd;
	unsigned char k = 0;
	switch(chipselect){
		case 0:
			fd = open("/dev/spidev0.0", O_RDWR);
		break;
		case 1:
			fd = open("/dev/spidev0.1", O_RDWR);
		break;
		default:
			printf("wrong spidev0.%d value\r\n", chipselect);
			return;
		break;
	}
	
	if(fd < 0){
		printf("error opening spidev0.%d\r\n", fd);
		goto fail;
	} 
	
	for(k=0;k<nCHAN;k++)
		mcp->raw[k] = 0x00;

	mcp->spiFd = fd;
	spi_settings(mcp->spiFd, 1000000, 8, SPI_MODE_0);
	return 0;

fail:
	return -1;
}

void mcp3008_exit(struct mcp3008_t *mcp){
	close(mcp->spiFd);
}

int mcp3008_capture(struct mcp3008_t *mcp, uint8_t channel, uint8_t capture_mode){
	uint8_t buff[3];
	int ret = -1;	
	buff[0] = START_MASK;
	buff[1] = channel|capture_mode;
	buff[2] = 0x00;
	spi_write_then_read(mcp->spiFd, buff, 3);	
	printf("buff[1]: 0x%x buff[2]: 0x%x\r\n", buff[1], buff[2]);
	ret = ((buff[1]&0x03)<<8)|(buff[2]&0xff);
	mcp->raw[channel] = (ret*3.3);
	return ret;
fail:
	return ret;
}

void mcp3008_print(struct mcp3008_t *mcp){
	unsigned char k = 0;
	for(k=0; k<nCHAN;k++)
		printf("CHAN%d: %d\r\n", k, mcp->raw[k]);
}
