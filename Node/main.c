#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <msp430g2553.h>

#include "xbee.h"

#define MIC 	BIT5 // P1.5

char status[16];

#define TA1_MOD  13733
volatile int ta1_count;

#define WDT_MOD	0xffff
volatile int wdt_count;

void adc_init(void);
void uart_init(void);
void timers_init(void);
void platform_init(void);

int main(void){
	platform_init();
	while(1);
	return 0;
}

void platform_init(void){
	if (CALBC1_1MHZ == 0xff || CALDCO_1MHZ == 0xff) while(1);
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	WDTCTL = WDTPW + WDTHOLD;

	adc_init();
	timers_init();
	xbee_init();

	_BIS_SR(GIE);
	xbee_send_base("node 0 up\r\n");
}

void adc_init(void){
	ADC10CTL0 = SREF_0; // VCC/VSS as ref.
	ADC10CTL0 |= ADC10SHT_1; // Sample and hold for 8 clock cycles
	ADC10CTL0 |= ADC10IE; // ISR
	ADC10CTL0 |= ADC10ON; // ON

	ADC10CTL1 = INCH_5; // P1.5
	P1SEL |= MIC;
	ADC10AE0 |= MIC;
	
	//ADC10CTL1 |= CONSEQ_2; // repeat single chan 
	//ADC10CTL1 |= SHS_1; // triggered by TA0
	//ADC10CTL1 |= ADC10DIV_3; // ACLK/3
}

void timers_init(void){
	TACTL |= MC_0; // stop
	TACTL = TASSEL_2 + ID_2 + TAIE; // SMCLK
	
	// TA0: AN. conversion
	TA0R = 0;
	CCR0 = 0xff;
	CCTL0 |= CCIE; // CCR0 IE

	// TA1: 15 min status report
	TA1R = 0;
	CCR1 = 0xffff;
	TACTL |= MC_2; // continuous op. mode
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void CCR0_ISR(void){
	ADC10CTL0 |= ENC + ADC10SC; // sampling+start
	//while (ADC10CTL1 & ADC10BUSY);
	sprintf(status, "sts: %d\r\n", ADC10MEM);
	xbee_send_base(status);
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
