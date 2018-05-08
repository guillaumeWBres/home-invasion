#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <msp430g2553.h>

#include "xbee.h"

#define SELF_ID	"5101"
volatile int status;

#define TA1_MOD  13733
volatile int ta1_count;

#define WDT_MOD	0xffff
volatile int wdt_count;

#define MIC 			BIT5 // P1.5: AN
#define DER_THRESH	(20*1024/100) // 20% variation will trigger
volatile int 			pVal;

#define RED					BIT0 // P1.0
#define GREEN				BIT6 // P2.6

void adc_init(void);
void uart_init(void);
void timers_init(void);
void platform_init(void);

// initializes 1st derv. calculator 
void restart_calculator(void);

int main(void){
	platform_init();
	
	// PAN ID: 5110
	// MY ID: 5100+node(id)
	// default DEST: BS, ie. 5100
	//xbee_setup("5110", "5101", "5100"); 

	xbee_unicast(
		"<Node|1|STS|0>\r\n", 
			"5100",
				strlen("<Node|1|STS|0>\r\n")+2
	);

	status = NODE_STAND_BY;

	while(1){
		
		switch (status){
			case NODE_ACTIVE:
				P2OUT &= ~GREEN;

				status = xbee_wait_instructions();
				if (status == NODE_HIBERNATE){
					xbee_unicast(
						"<Node|1|STS|2>\r\n", 
							"5100",
								strlen("<Node|1|STS|2>\r\n")+2
					);
					
					UC0IE |= UCA0RXIE; // will wake up on BS orders
					_BIS_SR(LPM0_bits+GIE); // hibernate

				} else if (status == NODE_STAND_BY)
					xbee_unicast(
						"<Node|1|STS|0>\r\n", 
							"5100",
								strlen("<Node|1|STS|0>\r\n")+2
					);

				else
					xbee_unicast(
						"<Node|1|STS|-1|Active>\r\n", 
							"5100",
								strlen("<Node|1|STS|-1|Active>\r\n")+2
					);

				break;
			
			case NODE_HIBERNATE:
				P2OUT |= GREEN;
				
				status = xbee_wait_instructions();
				if (status == NODE_ACTIVE)
					xbee_unicast(
						"<Node|1|STS|1>\r\n", 
							"5100",
								strlen("<Node|1|STS|1>\r\n")+2
					);

				else if (status == NODE_STAND_BY)
					xbee_unicast(
						"<Node|1|STS|0>\r\n", 
							"5100",
								strlen("<Node|1|STS|0>\r\n")+2
					);
				break;

			case NODE_NOTIFY_EVENT:
				// notify BS then stand by
				xbee_unicast(
					"<Node|1|STS|0|DetectionEvent>\r\n",
						"5100",
							strlen("<Node|1|STS|0|DetectionEvent>\r\n")+2
				);
				status = NODE_STAND_BY;
				P1OUT &= ~RED;
				break;

			case NODE_STAND_BY:
			default:
				P2OUT |= GREEN;

				status = xbee_wait_instructions();
				if (status == NODE_ACTIVE){
					xbee_unicast(
						"<Node|1|STS|1>\r\n", 
							"5100",
								strlen("<Node|1|STS|1>\r\n")+2
					);

					restart_calculator(); // going hot

				} else if (status == NODE_HIBERNATE){
					xbee_unicast(
						"<Node|1|STS|2>\r\n", 
							"5100",
								strlen("<Node|1|STS|2>\r\n")+2
					);

					UC0IE |= UCA0RXIE; // will wake up on BS orders
					_BIS_SR(LPM0_bits+GIE); // hibernate

				} else
					xbee_unicast(
						"<Node|1|STS|-1|StandBy>\r\n", 
							"5100",
								strlen("<Node|1|STS|-1|StandBy>\r\n")+2
					);
				break;
		}
	}
	return 0;
}

void platform_init(void){
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
	
	P1DIR |= RED;
	P1OUT &= 0x00;
	
	P2DIR |= GREEN;
	P2OUT &= 0x00;
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

void restart_calculator(void){
	ADC10CTL0 |= ENC + ADC10SC; 
	while (ADC10CTL1 & ADC10BUSY);
	pVal = ADC10MEM;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void CCR0_ISR(void){
	int nVal, der;
	
	if (status == NODE_ACTIVE){
		ADC10CTL0 |= ENC + ADC10SC; 
		while (ADC10CTL1 & ADC10BUSY);
		nVal = ADC10MEM;

		der = nVal-pVal;
		if (der < 0) 
			der *= (-1); // |abs|: only care about changes

		if (der >= DER_THRESH){ // major change
			status = NODE_NOTIFY_EVENT;
			P1OUT |= RED;
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
			case NODE_IDLE:
			case NODE_WORKING:
				_BIS_SR(LPM3_bits + GIE); 
				WDTCTL = WDTPW + WDTHOLD;
				wdt_cont &= 0;
				break;
			
			case NODE_NOT_NOTIFIED:
			case NODE_NOTIFIED_WAIT_ACK:
			default:
				break;
		}
	} else
		wdt_cont++;
}
*/
