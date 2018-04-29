#include <string.h>
#include <stdio.h>
#include <msp430g2553.h>

#include "xbee.h"

#define RX 	BIT1
#define TX 	BIT2
#define SLEEP_PIN	BIT3
#define BUFSIZE 64

char rx_buffer[BUFSIZE];
volatile int rx_ptr;
volatile int rx_size;
volatile char rx_done;

char tx_buffer[BUFSIZE];
volatile int tx_ptr;
volatile int tx_size;
volatile uint8_t tx_done;

void xbee_init(void){
	P1DIR |= RX+TX+SLEEP_PIN;
	P1SEL |= RX+TX;
	P1SEL2 |= RX+TX;
	
	xbee_wakeup();
	
	UCA0CTL1 |= UCSSEL_2; // SMCLK
	UCA0BR0 = 0x08; // 115200 b/s @1M
	UCA0BR1 = 0x00; // 115200 b/s @1M
	UCA0MCTL = UCBRS2 + UCBRS0; // 5% modulation
	UCA0CTL1 &= ~UCSWRST;
	UC0IE &= ~UCA0TXIE; // disable TX ISR
	UC0IE &= ~UCA0RXIE; // disable RX ISR
}

void xbee_send_command(const char *cmd, const int size){
	tx_ptr &= 0;
	tx_done &= 0x00;
	strcpy(tx_buffer, cmd);
	tx_size = size;
	UC0IE |= UCA0TXIE;
	while (!tx_done);
}

int xbee_confirmed_command(const char *cmd, const int size, const char *confirm){
	xbee_send_command(cmd, size);

	rx_ptr &= 0;
	rx_done &= 0x00;
	UC0IE |= UCA0RXIE;
	while (!rx_done);

	if (strcmp(rx_buffer, confirm) == 0)
		return 0;
	else
		return -1;
}

int xbee_answered_command(const char *cmd, const int size, char *answer){
	xbee_send_command(cmd, size);

	rx_ptr &= 0;
	rx_done &= 0x00;
	UC0IE |= UCA0RXIE;
	while (!rx_done);

	strcpy(answer, rx_buffer);
	return strlen(rx_buffer);
}

void xbee_setup(const char *ID, const char *MY){
	char buffer[16];
	xbee_confirmed_command("+++\r", 4, "OK\r"); // enter CMD mode
	xbee_confirmed_command("ATCT32\r", 7, "OK\r"); // CMD timeout
	
	sprintf(buffer, "ATID%s\n", ID); 
	xbee_confirmed_command(buffer, 9, "OK\r");
	
	sprintf(buffer, "ATMY%s\r", MY); 
	xbee_confirmed_command(buffer, 9, "OK\r");

// ATSM: sleep mode [0:disabled;1:Pin sleep enabled;4:Cyclic;5:Cyclic+pin wakeup]
// ATSP: sleep period ([0x20-0xAF0] x 10ms)
// ATST: time before sleep; timer resets on SERIAL/XBEE data (*x1ms)
// ATSO: sleep option: 0: always wake up for ST time, 1: sleep entire SN*SP time
	
	xbee_confirmed_command("ATCN\n", 5, "OK\r");
}

void xbee_broadcast(const char *payload, const int size){
	xbee_confirmed_command("+++", 3, "OK\r"); // enter CMD mode
	xbee_confirmed_command("ATDH0\r", 6, "OK\r"); // clear ADDR hi
	xbee_confirmed_command("ATDHFFFF\r", 9, "OK\r"); // set broadcast DEST
	xbee_confirmed_command("ATCN\r", 5, "OK\r");
	xbee_send_command(payload, size);
}

void xbee_unicast(const char *payload, const char *DH, const char *DL, const int size){
	char buffer[16];
	xbee_confirmed_command("+++", 3, "OK\r"); // enter CMD mode
	// set dest ADDR
	sprintf(buffer, "ATDH%s\r", DH); xbee_confirmed_command(buffer, 9, "OK\r");
	sprintf(buffer, "ATDL%s\r", DL); xbee_confirmed_command(buffer, 9, "OK\r");
	xbee_confirmed_command("ATCN\r", 5, "OK\r");
	xbee_send_command(payload, size);
}

void xbee_sleep(void){
	P1OUT |= SLEEP_PIN;
}

void xbee_wakeup(void){
		P1OUT &= ~SLEEP_PIN;
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void){
	UCA0TXBUF = tx_buffer[tx_ptr];
	if (tx_ptr == tx_size-1){ 
		UC0IE &= ~UCA0TXIE; // disable TX ISR
		tx_done |= 0x01;
	} else
		tx_ptr++;
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
