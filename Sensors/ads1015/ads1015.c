#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "i2c.h"
#include "ads1015.h"

int ads1015_init(struct ads1015_t *ads, char i2cChan, char chipAddr){
	unsigned int fd = -1; char k = 0;
	switch(i2cChan){
		case 0:
			fd = open("/dev/i2c-1", O_RDWR);
			break;
		case 1:
			fd = open("/dev/i2c-1", O_RDWR);
			break;
		default:
			printf("wrong /dev/i2c-%d value\r\n", i2cChan);
			goto fail;
			break;
	}
	if (fd < 0){
		printf("failed to open /dev/i2c-%d\r\n", i2cChan);
		goto fail;
	} else ads->i2cFd = fd;
	
	i2c_settings(ads->i2cFd, chipAddr);
	ads->raw = 0x00;
	return 0;
fail:
	return -1;
}

void ads1015_exit(struct ads1015_t *ads){
	if(close(ads->i2cFd) < 0)
		printf("error closing /dev/i2c-%d\r\n", ads->i2cFd);
}

void ads1015_adc_settings(struct ads1015_t *ads, char conv_mode, char channel){
	char buff[3];
	buff[0] = CONFIG_REG; 
	
	if(conv_mode == ADC_CONTINUOUS_MODE){
		ads->captureMode = ADC_CONTINUOUS_MODE;
		buff[1] = ADC_CONTINUOUS_MODE;
	} else if (conv_mode == ADC_SINGLESHOT){
		ads->captureMode = ADC_SINGLESHOT;
		buff[1] = ADC_SINGLESHOT;
	} else { 
		printf("wrong capture Mode %d\r\n", conv_mode);
		return;
	}

  buff[1] |= DEFAULT_GAIN|channel;
	buff[2] = SPS1600|DEFAULT_COMP|RDY_LO|DEFAULT_LATCH|DEFAULT_COMP_QUEUE; // <!> ajouter gestion IRQ
	i2c_write(ads->i2cFd,	buff, 3);
}

int ads1015_done(struct ads1015_t *ads){
	char buff[2];
	buff[0] = CONFIG_REG;
	i2c_write(ads->i2cFd, buff, 1);
	i2c_read(ads->i2cFd, buff, 2);
	return (buff[0]&0x80)>>7;
}

int ads1015_single_read(struct ads1015_t *ads)
{
	char buff[4];
	int raw = 0; uint32_t k = 0;	
	if(ads->captureMode == ADC_SINGLESHOT){
		ads1015_startConv(ads);
		while(ads1015_done(ads) != 0x01); 
	}

	buff[0] = CONV_REG;
	i2c_write(ads->i2cFd, buff, 1);
	i2c_read(ads->i2cFd, buff, 2);
	raw = (buff[0] << 4) | ((buff[1]&0xF0) >> 4);
	ads->raw = raw;
	return raw;
}

void ads1015_print(struct ads1015_t *ads){
	printf("%d\r\n", ads->raw);
}

int ads1015_i2c_testComm(struct ads1015_t *ads){
	char buff[3];
	short t_;
	buff[0] = LO_THRESH_REG; 
	i2c_write(ads->i2cFd, buff, 1);
	i2c_read(ads->i2cFd, buff, 2);
	t_ = (buff[0]<<8)|buff[1];
	if(t_ != 0x8000)
		goto fail;
	buff[0] = HI_THRESH_REG;
	i2c_write(ads->i2cFd, buff, 1);
	i2c_read(ads->i2cFd, buff, 2);
	t_ = (buff[0]<<8)|buff[1];
	if(t_ != 0x7fff)
		goto fail;
	printf("ads1015-test success\r\n");
	return 0;
fail:
	printf("ads1015-test failed\r\n");
	return -1;
}

void ads1015_startConv(struct ads1015_t *ads){
	char buff[3];
	buff[0] = CONFIG_REG;	
	i2c_write(ads->i2cFd, buff, 1);
	i2c_read(ads->i2cFd, buff, 2);
	buff[2] = buff[1]; 
	buff[1] = (buff[0]| 1<<7); //start bit
	buff[0] = CONFIG_REG;
	i2c_write(ads->i2cFd, buff, 3);
}
