#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <msp430g2553.h>

#include "xbee.h"

#define SELF_ID	"5101"

volatile uint8_t status;
#define STATUS_IDLE					0x00
#define STATUS_WORKING				0x01
#define STATUS_NOT_NOTIFIED		0x02
#define STATUS_NOTIFIED_WAIT_ACK	0x03

#define TA1_MOD  13733
volatile int ta1_count;

#define WDT_MOD	0xffff
volatile int wdt_count;

#define MIC 			BIT5 // P1.5: AN
#define DER_THRESH	(1024/10) // 10% variation will trigger
volatile int 			pVal;

#define LED				BIT0 // P1.0: debouncer

void adc_init(void);
void uart_init(void);
void timers_init(void);
void platform_init(void);

int main(void){
	char buffer[64];

	platform_init();
	//xbee_setup("5110", SELF_ID); // set PAN ID & My ID	

	// notify BS
	//xbee_unicast("Hello from MSP430\n", "0000", "5100", 
	//	strlen("Hello from MSP430\n")+1
//	);

	xbee_send_command("'5101' up & running\n", 
		strlen("up & running\n")+1
	);

	status = STATUS_WORKING; // will eventually be set by BS 

	while(1){
		switch (status){

			case STATUS_NOT_NOTIFIED:
				status = STATUS_NOTIFIED_WAIT_ACK;
				
				xbee_answered_command(
					"'5101' got something\n",
						strlen("'5101' got something\n")+1,
							buffer
				);

				break;

			case STATUS_NOTIFIED_WAIT_ACK:
				if (strcmp(buffer, "RESTART\r") == 0){

					xbee_send_command("'5101' going back to work\n",
						strlen("'5101' going back to work\n"+1)
					);

					status &= STATUS_IDLE;
					P1OUT &= ~LED;
	
					ADC10CTL0 |= ENC + ADC10SC; 
					while (ADC10CTL1 & ADC10BUSY);
					pVal = ADC10MEM;
				}
				break;

			default:
			case STATUS_IDLE:
			case STATUS_WORKING:
				// sleep?
				break;
		}
	}

	return 0;
}

void platform_init(void){
	if (CALBC1_1MHZ == 0xff || CALDCO_1MHZ == 0xff) {while(1);};
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

	if (status == STATUS_WORKING){
		ADC10CTL0 |= ENC + ADC10SC; 
		while (ADC10CTL1 & ADC10BUSY);
		nVal = ADC10MEM;

		der = nVal-pVal;
		if (der < 0) 
			der *= (-1); // |abs|: only care about changes

		if (der >= DER_THRESH){ // major change
			status = STATUS_NOT_NOTIFIED;
			P1OUT |= LED;
		} else
			pVal = nVal; 
	}

	TA0R = 0;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void CCR1_ISR(void){
	if (ta1_count == TA1_MOD-1){
		// xbee_status_report(status);
		ta1_count = 0;
	} else
		ta1_count++;

	TA0R = 0;
}

/* WDT is here to make sure app
	is power consumption optimized */

/*
#pragma vector = WDT_VECTOR
__interrupt void watchdog_timer(void){
	if (wdt_count == WDT_MOD-1){
		switch(status) {
			// extern rx_ptr
			rx_ptr &= 0; // will wake up() if sleeping?
			case STATUS_IDLE:
			case STATUS_WORKING:
				_BIS_SR(LPM3_bits + GIE); 
				WDTCTL = WDTPW + WDTHOLD;
				wdt_cont &= 0;
				break;
			
			case STATUS_NOT_NOTIFIED:
			case STATUS_NOTIFIED_WAIT_ACK:
			default:
				break;
		}
	} else
		wdt_cont++;
}
*/
