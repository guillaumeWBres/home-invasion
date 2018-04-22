#include "xbee.h"
#include <string.h>
#include <msp430g2553.h>

#define RX 	BIT1
#define TX 	BIT2
#define BUFSIZE 64

char rx_buffer[BUFSIZE];
volatile int rx_ptr;
volatile int rx_size;

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

void xbee_send_base(const char *status){
	tx_ptr &= 0;
	strcpy(tx_buffer, status);
	tx_size = strlen(status)+2;
	UC0IE |= UCA0TXIE;
	while (UCA0STAT & UCBUSY);
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void){
	UCA0TXBUF = tx_buffer[tx_ptr++];
	if (tx_ptr == tx_size) // tx done
		UC0IE &= ~UCA0TXIE; // disable TX ISR
}

// receive orders
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
	rx_buffer[rx_ptr++] = UCA0RXBUF;
	if (rx_ptr == BUFSIZE-1)
		rx_ptr = 0;

	if (UCA0RXBUF == '\n'){
		// end of orders
		// wake up CPU
		// allow WDT
		// dans main on traite la commande
		// si c'est pas connu, on retournera en hibernation 
	}
}
