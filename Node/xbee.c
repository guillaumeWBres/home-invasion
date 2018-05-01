#include <string.h>
#include <stdio.h>
#include <msp430g2553.h>

#include "xbee.h"

#define RX 	BIT1
#define TX 	BIT2
#define SLEEP_PIN	BIT3

char rx_buffer[BUFSIZE];
volatile int rx_ptr;
volatile int rx_size;
volatile char rx_done;

char tx_buffer[BUFSIZE];
volatile int tx_ptr;
volatile int tx_size;
volatile uint8_t tx_done;

static char DL[4];

void xbee_init(void){
	P1DIR |= RX+TX; // XBEE uart
	P1DIR |= SLEEP_PIN; // XBEE ctrl
	P1SEL |= RX+TX;
	P1SEL2 |= RX+TX;
	
	xbee_wakeup();
	
	UCA0CTL1 |= UCSSEL_2; // SMCLK
	UCA0BR0 = 104; // 9600 B/s @ 1M
	UCA0BR1 = 0x00; // 9600 B/s @ 1M
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

int xbee_confirmed_command(const char *cmd, const int size){
	xbee_send_command(cmd, size);
	rx_ptr &= 0;
	rx_done &= 0x00;
	UC0IE |= UCA0RXIE;
	while (!rx_done);
	return 0;
}

void xbee_wait_instructions(char *buffer){
	rx_ptr &= 0;
	rx_done &= 0x00;
	UC0IE |= UCA0RXIE;
	_BIS_SR(LPM0_bits + GIE);
	strcpy(buffer, rx_buffer);
}

void xbee_setup(const char *id, const char *my, const char *dl){
	char buffer[16];

	__delay_cycles(1000000);
	xbee_send_command("+++", 3); // enter CMD mode
	rx_ptr &= 0;
	rx_done &= 0x00;
	UC0IE |= UCA0RXIE;
	while (!rx_done);

	// set Network (PAN) ID
	strcpy(buffer, "ATID");
	strcpy(buffer+4, id);
	buffer[8] = '\r';
	xbee_confirmed_command(buffer, strlen(buffer)+1);
	
	// set ATMY
	strcpy(buffer, "ATMY");
	strcpy(buffer+4, my);
	buffer[8] = '\r';
	xbee_confirmed_command(buffer, strlen(buffer)+1);

	// set default ADDR
	// ADDRH will not be used
	xbee_confirmed_command("ATDH0\r", 6);
	strcpy(buffer, "ATDL");
	strcpy(buffer+4, dl);
	strcpy(DL, dl); // store internally for next unicast/broadcast
	buffer[8] = '\r';
	xbee_confirmed_command(buffer, strlen(buffer)+1); 

// ATSM: sleep mode [0:disabled;1:Pin sleep enabled;4:Cyclic;5:Cyclic+pin wakeup]
// ATSP: sleep period ([0x20-0xAF0] x 10ms)
// ATST: time before sleep; timer resets on SERIAL/XBEE data (*x1ms)
// ATSO: sleep option: 0: always wake up for ST time, 1: sleep entire SN*SP time

	xbee_confirmed_command("ATCN\r", 5);
}

void xbee_broadcast(const char *payload, const int size){
	if (strcmp(DL, "FFFF") != 0){
		// configure for broadcast 
		__delay_cycles(1000000);
		xbee_send_command("+++", 3); // enter CMD mode
		rx_ptr &= 0;
		rx_done &= 0x00;
		UC0IE |= UCA0RXIE;
		while (!rx_done);

		strcpy(DL, "FFFF"); // store internally for next unicast/broadcast
		xbee_send_command("ATDLFFFF\r", 9); 

		rx_ptr &= 0;
		rx_done &= 0x00;
		UC0IE |= UCA0RXIE;
		while (!rx_done);

		xbee_send_command("ATCN\r", 5);
		rx_ptr &= 0;
		rx_done &= 0x00;
		UC0IE |= UCA0RXIE;
		while (!rx_done);
	} 
	xbee_send_command(payload, size);
}

void xbee_unicast(const char *payload, const char *dl, const int size){
	char buffer[16];
	if (strcmp(DL, dl) != 0){
		// need to program correct DEST ADDR
		__delay_cycles(1000000);
		xbee_confirmed_command("+++", 3); // enter CMD mode

		strcpy(buffer, "ATDL");
		strcpy(buffer+4, dl);
		strcpy(DL, dl); // store internally for next unicast/broadcast
		buffer[8] = '\r';
		xbee_confirmed_command(buffer, 9); 

		xbee_confirmed_command("ATCN\r", 5);
	} 
	xbee_send_command(payload, size);
}

uint8_t checksum(const char *API_frame){
	int i;
	int sum = 0;
	for (i=2; i<strlen(API_frame); i++)
		sum += API_frame[i];
	sum &= 0xFF;
	return (uint8_t)(0xFF-sum);
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
	rx_buffer[rx_ptr] = UCA0RXBUF;
	if (rx_ptr == BUFSIZE-1) // avoid overflow
		rx_ptr = 0;
	else
		rx_ptr++;

	if (UCA0RXBUF == '\r'){
		rx_done |= 0x01;
		UC0IE &= ~UCA0RXIE;
		_BIC_SR(LPM0_bits); // wakeup if needed
	} else
		rx_done &= 0x00;
}
