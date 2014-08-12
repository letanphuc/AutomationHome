/*****************************************************************************
 *
 * File: maintutorial1local.c
 *
 * Copyright S. Brennen Ball, 2007
 *
 * The author provides no guarantees, warantees, or promises, implied or
 *	otherwise.  By using this software you agree to indemnify the author
 * 	of any damages incurred by using it.
 *
 *****************************************************************************/

//This code depends on a clock frequency of 40 MHz (I use a 10 MHz crystal and
//  the PLL.  If you are using a slower clock, then you must change the SPI and
//  UART rates, as well as the delay lengths in delays.c
#include <msp430G2553.h>
#include "stdio.h"
#include "UART.h"
#include "SPI.h"
#include "nrf24l01.h"
void uart_printi(int n);
void printreg(char *name, int n);
void Initialize(void);

//main routine
void main(void) {
	unsigned char data; //register to hold letter sent and received
	unsigned int count; //counter for for loop
	Initialize(); //initialize IO, UART, SPI, set up nRF24L01 as TX
	uart_puts((char *) "*** MSP430 SPI Example ***\n\r\n\r");
	unsigned char stat;
	unsigned char conf;

	uart_puts("Hello world");

	while (1) {
		nrf24l01_clear_csn();
		// send command
		stat = SPI_Xfer_byte(nrf24l01_R_REGISTER | nrf24l01_CONFIG);
		// read response
		conf = SPI_Xfer_byte(nrf24l01_NOP);
		nrf24l01_set_csn();
		uart_puts((char *) "NRF24l01 registers:\n\r");
		printreg((char *) "status", stat);
		printreg((char *) "config", conf);

		__delay_cycles(5000000);
	}
}

//initialize routine
void Initialize(void) {
	WDTCTL = WDTPW + WDTHOLD; 	// Stop WDT
	BCSCTL1 = CALBC1_1MHZ;      // Set DCO
	DCOCTL = CALDCO_1MHZ;
//	InitializeIO(); //set up IO (directions and functions)
//	OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_LOW, 64); //open UART
//	OpenSPI(SPI_FOSC_16, MODE_00, SMPMID); //open SPI1
	uart_init();
	SPI_Init();
}
void uart_printi(int n) {
	int buffer[16];
	int i;
	int j;

	if (n == 0) {
		uart_putc('0');
		return;
	}

	for (i = 15; i > 0 && n > 0; i--) {
		buffer[i] = (n % 10) + '0';
		n /= 10;
	}

	for (j = i + 1; j <= 15; j++) {
		uart_putc(buffer[j]);
	}
}
void printreg(char *name, int n) {
	int size = 8;
	int i;

	uart_puts((char *) "- ");
	uart_puts(name);
	uart_puts((char *) ": ");

	int mask = 1 << (size - 1);

	for (i = 0; i < size; i++) {
		if ((n & (mask >> i)) != 0) {
			uart_putc('1');
		} else {
			uart_putc('0');
		}
	}
	uart_puts((char *) " (");
	uart_printi(n);
	uart_puts((char *) ")\n\r");
}
/*
 {
 uart_putc(data);
 uart_puts((char *)"\n\r");
 txstt = nrf24l01_irq_tx_ds_active();
 irqstt = nrf24l01_irq_pin_active();
 conf = nrf24l01_get_config();
 stat = nrf24l01_get_status();
 printreg((char *)"irqstt", irqstt);
 printreg((char *)"txstt", txstt);
 printreg((char *)"status", stat);
 printreg((char *)"config", conf);
 _delay_cycles(5000000);
 }
 */
