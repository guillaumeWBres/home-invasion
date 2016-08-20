#include <fcntl.h>			
#include <sys/ioctl.h>			
#include <linux/spi/spidev.h>	
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "spi.h"

int spi_cs0_fd;				
int spi_cs1_fd;			
	
unsigned char spi_mode;
unsigned char spi_bitsPerWord;
unsigned int spi_speed;

int open_spi (int slave_select, int mode, int speed_hz){

    int status_value = -1;
    int *spi_cs_fd;
 
    switch(mode){
		case 0: spi_mode = SPI_MODE_0; break; // CPOL 0 CPHA 0
		case 1: spi_mode = SPI_MODE_1; break; // CPOL 0 CPHA 1
		case 2: spi_mode = SPI_MODE_2; break; // CPOL 1 CPHA 0
		case 3: spi_mode = SPI_MODE_3; break; // CPOL 1 CPHA 1
		default: perror("MODE VALUE[0:3]"); exit(EXIT_FAILURE); break; 
    }
    
    spi_bitsPerWord = 8;
    spi_speed = speed_hz;	 

    if (slave_select) 
	 	spi_cs_fd = &spi_cs1_fd;
    else 
	 	spi_cs_fd = &spi_cs0_fd;

    if (slave_select)
    	*spi_cs_fd = open("/dev/spidev0.1", O_RDWR);
    else
    	*spi_cs_fd = open("/dev/spidev0.0", O_RDWR);

    if (*spi_cs_fd < 0)
    {
        perror("Error - Could not open SPI device");
        exit(1);
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_MODE, &spi_mode);
    if(status_value < 0)
    {
        perror("Could not set SPIMode (WR)...ioctl fail");
        exit(1);
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_RD_MODE, &spi_mode);
    if(status_value < 0)
    {
      perror("Could not set SPIMode (RD)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
    if(status_value < 0)
    {
      perror("Could not set SPI bitsPerWord (WR)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bitsPerWord);
    if(status_value < 0)
    {
      perror("Could not set SPI bitsPerWord(RD)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    if(status_value < 0)
    {
      perror("Could not set SPI speed (WR)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
    if(status_value < 0)
    {
      perror("Could not set SPI speed (RD)...ioctl fail");
      exit(1);
    }
    return(status_value);
}

int close_spi(int slave_select)
{
    int status_value = -1;
    int *spi_cs_fd;

    if (slave_select)
    	spi_cs_fd = &spi_cs1_fd;
    else
    	spi_cs_fd = &spi_cs0_fd;

    status_value = close(*spi_cs_fd);
    if(status_value < 0)
    {
    	perror("Error - Could not close SPI device");
    	exit(1);
    }
    return(status_value);
}

int spi_write_then_read (int slave_select, char *buff, int num_of_byte)
{
	struct spi_ioc_transfer spi[num_of_byte]; 
	int i =0;
	int retVal = -1;
	int *spi_cs_fd;

    if (slave_select)
    	spi_cs_fd = &spi_cs1_fd;
    else
    	spi_cs_fd = &spi_cs0_fd;

	for (i=0; i < num_of_byte ;i++)
	{
		spi[i].tx_buf = (unsigned long)(buff + i); 
		spi[i].rx_buf = (unsigned long)(buff + i);
		spi[i].len = sizeof(*(buff + i)) ;
		spi[i].delay_usecs = 0;
		spi[i].speed_hz = spi_speed ;
		spi[i].bits_per_word = spi_bitsPerWord ;
		spi[i].cs_change = 0;
	}

	retVal = ioctl(*spi_cs_fd, SPI_IOC_MESSAGE(num_of_byte), &spi) ;

	if(retVal < 0)
	{
		perror("Error - Problem transmitting spi data..ioctl");
		exit(1);
	}

	return retVal;
}
