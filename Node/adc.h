#ifndef __ADC_H__
#define __ADC_H__

void avr_init_adc( void );
uint16_t avr_adc_read( uint8_t mux );
void avr_adc_start_conversion( void );

#endif
