#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "adc.h"

#define setbit(x,y) x |= _BV(y)
#define clearbit(x,y) x &= ~(_BV(y))
#define togglebit(x,y) x ^= _BV(y)
#define is_high(x,y) ((x & _BV(y)) == _BV(y))
#define is_low(x,y) ((x & _BV(y)) == 0)

#define SYSTEM_LED 0x00

//TODO
// select arch:
//  il faut 4 ports analogiques
//  il faut 1 port i2c 

//TODO
// IRQ sur capteur sound analog0 (compare value)
// IRQ sur capteur sound analog1 (compare value)
// IRQ sur capteur PIR (compare value)
// IRQ sur capteur IR (compare value ou autre)

// chaque IRQ declenche une command I2C vers $HOST

// un compteur demarre a la mise sous tension
// et authorise le syst. apres 5min

void avr_init_platform(void);
void avr_init_gpio(void);

int main(void){
	avr_init_platform();

	for(;;){
		
	}

	return 0;
}

void avr_init_platform( void ){
	avr_init_gpio();
	avr_init_adc();
	
	setbit( PORTB, SYSTEM_LED );
}

void avr_init_gpio( void ){
	DDRD = 0x00;
	DDRB = 0xff;
}
