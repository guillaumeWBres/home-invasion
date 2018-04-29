#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <msp430g2553.h>

#include "xbee.h"

char status[64];

#define TA1_MOD  13733
volatile int ta1_count;

#define WDT_MOD	0xffff
volatile int wdt_count;

#define MIC 			BIT5 // P1.5: AN
#define DER_THRESH	(1024/10) // 10% variation will trigger
volatile int 			pVal;
volatile uint8_t 		debouncing;
volatile int 			dbc_count;
#define DEBOUNCE_LIM	0xffff

#define LED				BIT0 // P1.0: debouncer
volatile uint8_t 		detected;

void adc_init(void);
void uart_init(void);
void timers_init(void);
void platform_init(void);

int main(void){
	platform_init();
	//xbee_setup("5110", "5101"); // set PAN ID & My ID	

	// notify BS
	//xbee_unicast("Hello from MSP430\n", "0000", "5100", 
	//	strlen("Hello from MSP430\n")+1
//	);

	xbee_send_command("up & running\n", strlen("up & running\n")+1);
	while(1);
	return 0;
}

void platform_init(void){
//	if (CALBC1_1MHZ == 0xff || CALDCO_1MHZ == 0xff) {while(1);};
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	WDTCTL = WDTPW + WDTHOLD;

	adc_init();
	timers_init();
	xbee_init();
	_BIS_SR(GIE);
}

void adc_init(void){
	ADC10CTL0 = SREF_0; // VCC/VSS as ref.
	ADC10CTL0 |= ADC10SHT_1; // Sample and hold for 8 clock cycles
	ADC10CTL0 |= ADC10IE; // ISR
	ADC10CTL0 |= ADC10ON; // ON

	P1SEL |= MIC;
	ADC10CTL1 = INCH_5; // P1.5
	ADC10AE0 |= MIC;
	ADC10CTL1 &= ~ADC10DF; // unsigned for thresh. validity
	
	ADC10CTL0 |= ENC + ADC10SC; // 1st start
	while (ADC10CTL1 & ADC10BUSY);
	pVal = ADC10MEM;
	debouncing &= 0x00;
	dbc_count = 0;

	P1DIR |= LED;
	P1OUT &= 0x00;
}

void timers_init(void){
	TACTL |= MC_0; // stop
	TACTL = TASSEL_2 + ID_2 + TAIE; // SMCLK
	
	// TA0: AN. conversion
	TA0R = 0;
	CCR0 = 0xfff;
	CCTL0 |= CCIE; // CCR0 IE

	// TA1: 15 min status report
	TA1R = 0;
	CCR1 = 0xffff;
	TACTL |= MC_2; // continuous op. mode
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void CCR0_ISR(void){
	int nVal, der;

	if (debouncing){
		P1OUT |= LED;

		if (dbc_count == DEBOUNCE_LIM){
			dbc_count = 0;
			debouncing &= 0x00;
			ADC10CTL0 |= ENC + ADC10SC;
			while (ADC10CTL1 & ADC10BUSY);
			pVal = ADC10MEM;
			detected &= 0x00;
			P1OUT &= ~LED;
		} else
			dbc_count++;

	} else {

		ADC10CTL0 |= ENC + ADC10SC; 
		while (ADC10CTL1 & ADC10BUSY);
		nVal = ADC10MEM;

		der = nVal-pVal;
		if (der < 0) der *= (-1); // |abs|: only care about changes

		if (der >= DER_THRESH)
			detection |= 0x01;
	
		pVal = nVal; 
	}

	TA0R = 0;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void CCR1_ISR(void){
	if (ta1_count == TA1_MOD-1){
		// xbee_status_report();
		ta1_count = 0;
	} else
		ta1_count++;

	TA0R = 0;
}

/* WDT is enabled on orders received by BS
	monitors activity: goes back to hibernation
	if no activity detected

#pragma vector = WDT_VECTOR
__interrupt void watchdog_timer(void){
	if (wdt_count == WDT_MOD-1){
		_BIS_SR(LPM3_bits + GIE); // LPM3 hibernation from now on
		WDTCTL = WDTPW + WDTHOLD; // kill WDT
		wdt_cont = 0;
	} else
		wdt_cont++;
}
*/
