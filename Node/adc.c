#include <avr/io.h>
#include "adc.h"

void avr_init_adc( void ){
	ADCSRA = _BV(ADEN) | _BV(ADPS2); // ADC prescaler FCPU/16
}	

void avr_adc_start_conversion( void ){
	ADCSRA |= _BV(ADSC);
}

uint16_t avr_adc_read( uint8_t mux ){
	ADMUX = mux;
	avr_adc_start_conversion();
	while( ADCSRA & _BV(ADSC));
	return ADCW;
}
