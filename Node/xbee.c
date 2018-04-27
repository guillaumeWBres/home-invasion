#include <string.h>
#include <stdio.h>
#include <msp430g2553.h>

#include "xbee.h"

#define RX 	BIT1
#define TX 	BIT2
#define BUFSIZE 64

char rx_buffer[BUFSIZE];
volatile int rx_ptr;
volatile int rx_size;
volatile char rx_done;

char tx_buffer[BUFSIZE];
volatile int tx_ptr;
volatile int tx_size;

void xbee_init(void){
	P1DIR |= RX+TX;
	P1SEL |= RX+TX;
	P1SEL2 |= RX+TX;
	
	UCA0CTL1 |= UCSSEL_2; // SMCLK
	UCA0BR0 = 0x08; // 115200b/s @1M
	UCA0BR1 = 0x00; // 115200b/s @1M
	UCA0MCTL = UCBRS2 + UCBRS0; // 5% modulation
	UCA0CTL1 &= ~UCSWRST;
	UC0IE &= ~UCA0TXIE; // disable TX ISR
}

void xbee_send_command(const char *cmd){
	tx_ptr &= 0;
	strcpy(tx_buffer, cmd);
	tx_size = strlen(cmd);
	while (UCA0STAT & UCBUSY);
}

int xbee_confirmed_command(const char *cmd, const char *confirm){
	xbee_send_command(cmd);

	rx_ptr &= 0;
	rx_done &= 0x00;
	UC0IE |= UCA0RXIE;
	while (!rx_done);

	if (strcmp(rx_buffer, confirm) == 0)
		return 0;
	else
		return -1;
}

int xbee_answered_command(const char *cmd, char *answer){
	xbee_send_command(cmd);

	rx_ptr &= 0;
	rx_done &= 0x00;
	UC0IE |= UCA0RXIE;
	while (!rx_done);

	strcpy(answer, rx_buffer);
	return strlen(rx_buffer);
}

void xbee_setup(const char *ID, const char *MY, const char *DL){
	char buffer[16];
	
	P1DIR |= XBEE_SLEEP;
	P1OUT &= ~XBEE_SLEEP;

	xbee_send_command("+++"); // enter CMD mode
	xbee_send_command("ATCT32\r"); // CMD timeout
	sprintf(buffer, "ATID%s\r", ID); xbee_send_command(buffer);
	sprintf(buffer, "ATMY%s\r", MY); xbee_send_command(buffer);
	xbee_send_command("ATDH0\r");
	sprintf(buffer, "ATDL%s\r", DL); xbee_send_command(buffer);
// ATSM: sleep mode [0:disabled;1:Pin sleep enabled;4:Cyclic;5:Cyclic+pin wakeup]
// ATSP: sleep period ([0x20-0xAF0] x 10ms)
// ATST: time before sleep; timer resets on SERIAL/XBEE data (*x1ms)
// ATSO: sleep option: 0: always wake up for ST time, 1: sleep entire SN*SP time
	xbee_send_command("ATCN\r");
}

void xbee_broadcast(const char *payload){
	xbee_confirmed_command("+++", "OK\r"); // enter CMD mode
	xbee_confirmed_command("ATDH0\r", "OK\r"); // clear ADDR hi
	xbee_confirmed_command("ATDHFFFF\r", "OK\r"); // set broadcast DEST
	xbee_send_command("ATCN\r");
	xbee_send_command(payload);
}

void xbee_unicast(const char *payload, const char *DH, const char *DL){
	char buffer[16];
	xbee_confirmed_command("+++", "OK\r"); // enter CMD mode
	// set dest ADDR
	sprintf(buffer, "ATDH%s\r", DH); xbee_send_command(buffer);
	sprintf(buffer, "ATDL%s\r", DL); xbee_send_command(buffer);
	xbee_send_command("ATCN\r");
	xbee_send_command(payload);
}

void xbee_sleep(char sleep){
	if (sleep)
		P1OUT |= XBEE_SLEEP;
	else
		P1OUT &= ~XBEE_SLEEP;
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void){
	UCA0TXBUF = tx_buffer[tx_ptr++];
	if (tx_ptr == tx_size) // tx done
		UC0IE &= ~UCA0TXIE; // disable TX ISR
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
	rx_buffer[rx_ptr++] = UCA0RXBUF;
	if (rx_ptr == BUFSIZE-1) // avoid overflow
		rx_ptr = 0;

	if (UCA0RXBUF == '\r'){
		rx_done |= 0x01;
		UC0IE &= ~UCA0RXIE;
	} else
		rx_done &= 0x00;
}
