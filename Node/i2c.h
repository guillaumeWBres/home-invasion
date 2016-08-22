#ifndef __I2C_H__
#define __I2C_H__

#define I2C_TSL2561_CHIP_ADDR	0x29 // GND: 0x29, 0x39 0x49

void avr_init_i2c( void );
void avr_i2c_start(void);
void avr_i2c_stop(void);

void avr_i2c_write(char data);
void avr_i2c_write_reg(char addr, char data);

char avr_i2c_read_ack(void);
char avr_i2c_read_nack(void);
char avr_i2c_status(void);

#endif
