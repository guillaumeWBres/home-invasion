#ifndef _ADS1015_H_
#define _ADS1015_H_
#include <stdint.h>

#define nCHAN	4
#define ADS1015_CHIP_ADDR	 0x48  // GND
/* 0x50 --VCC 0x52 --SDA  0x54 --SCL */

/* Pointer Reg (W_O) VAL */
#define CONV_REG 0x00
#define CONFIG_REG 0x01
#define LO_THRESH_REG	0x02
#define HI_THRESH_REG	0x03

/* CONFIG REG */
  /*ADC CHANN MUX*/
#define ADC_CHANNEL_0 (0x04<<4)
#define ADC_CHANNEL_1 (0x05<<4)
#define ADC_CHANNEL_2 (0x06<<4)
#define ADC_CHANNEL_3 (0x07<<4)
  /*PGM GAIN AMP.*/
#define DEFAULT_GAIN	(0x02<<1)
  /*MODE*/
#define ADC_CONTINUOUS_MODE	0x00
#define ADC_SINGLESHOT	0x01
  /*DATA RATE*/
#define SPS128 (0x00<<5)
#define SPS250 (0x01<<5)
#define SPS490 (0x02<<5)
#define SPS920 (0x03<<5)
#define SPS1600	(0x04<<5)
#define SPS2400	(0x05<<5)
#define SPS3300	(0x06<<5)
 /*COMP MODE*/
#define DEFAULT_COMP (0x00<<4)
#define WINDOW_COMP	(0x01<<4)
  /*COMP POL - ALRT/RDY SENS*/
#define RDY_HI (0x01<<3)
#define RDY_LO (0x00<<3)
  /*LATCHING COMP*/
#define DEFAULT_LATCH	(0x00<<2)
#define LATCH_COMP (0x01<<2)
  /*COMP_QUEUE*/
#define DEFAULT_COMP_QUEUE (0x03)
#define QUEUE_ONE_CONV (0x00)
#define QUEUE_TWO_CONV (0x01)
#define QUEUE_FOUR_CONV	(0x02)

typedef struct ads1015_t {
	unsigned int i2cFd;
	int raw;
	char captureMode; 
}ads1015_t;	

int ads1015_init(struct ads1015_t *ads, char i2cChan, char chipAddr);
void ads1015_exit(struct ads1015_t *ads);
void ads1015_adc_settings(struct ads1015_t *ads, char conv_mode, char channel);
void ads1015_startConv(struct ads1015_t *ads);
int ads1015_done(struct ads1015_t *ads);
int ads1015_single_read(struct ads1015_t *ads);
void ads1015_print(struct ads1015_t *ads);
void ads1015_shutdown(struct ads1015_t *ads);
int ads1015_i2c_testComm(struct ads1015_t *ads);
#endif
