#include <avr/io.h>
#include "i2c.h"

void avr_init_i2c( void ){
	TWSR = 0x00;
	TWBR = 0x0C; // FCPU/(16+2TWBR*PRESCALER)
	TWCR = (1<<TWEN);
}

void avr_i2c_start(void){
	TWCR = (1<<TWINT);
	TWCR |= (1<<TWSTA);
	TWCR |= (1<<TWEN);
	while( (TWCR & (1<<TWINT)) == 0);
}

void avr_i2c_stop(void){
	TWCR = (1<<TWINT);
	TWCR |= (1<<TWSTO);
	TWCR |= (1<<TWEN);
}

void avr_i2c_write( char data ){
	TWDR = data;
	TWCR = (1<<TWINT);
	TWCR |= (1<<TWEN);
	while((TWCR & (1<<TWINT)) == 0);
}

char avr_i2c_read_ack( void ){
	TWCR = (1<<TWINT);
	TWCR |= (1<<TWEA);
	TWCR |= (1<<TWEN);
	while((TWCR &(1<<TWINT)) == 0);
	return TWDR;
}

char avr_i2c_read_nack( void ){
	TWCR = (1<<TWINT);
	TWCR |= (1<<TWEN);
	while((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

char avr_i2c_status( void ){
	return (TWSR & 0xF8);
}

char avr_i2c_write_reg( uint16_t addr, char data ){
	avr_i2c_start();

	if( avr_i2c_status() != 0x08 )
		return -1;
	
	avr_i2c_write((I2C_TSL2561_CHIP_ADDR)|(uint8_t)((addr & 0x0700)>>7));
	if( avr_i2c_status() != 0x18 )
		return -1;

	avr_i2c_write((uint8_t)addr);
	if( avr_i2c_status() != 0x28 )
		return -1;
	
	avr_i2c_write(data);
	if( avr_i2c_status() != 0x28 )
		return -1;

	avr_i2c_stop();
	return 0x00;
}
