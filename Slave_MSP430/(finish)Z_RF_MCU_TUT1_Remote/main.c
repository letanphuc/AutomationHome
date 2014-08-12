/*****************************************************************************
 *
 * File: maintutorial1remote.c
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
#include <msp430g2553.h>
#include <stdio.h>
#include "UART.h"
#include "SPI.h"
#include "nrf24l01.h"
#include "stdint.h"

void Initialize(void);

void ToggleLED(void); //toggle the current state of the on-board LED

void main(void) {
//	uint8_t data[4] ;
//	data[0] = 10;
	uint8_t data = 0;

	Initialize(); //initialize PLL, IO, SPI, set up nRF24L01 as RX

	printf("Remote now run\n\r");
	while (1) {
		//wait until a packet has been received
		while (!(nrf24l01_irq_pin_active() && nrf24l01_irq_rx_dr_active()))
			;

		nrf24l01_read_rx_payload(&data, 1); //read the packet into data
		nrf24l01_irq_clear_all(); //clear all interrupts in the 24L01
		//printf ("Have received: %d\n\r", data);
		_delay_cycles(130); //wait for the other 24L01 to come from standby to RX

		nrf24l01_set_as_tx(); //change the device to a TX to send back from the other 24L01
		nrf24l01_write_tx_payload(&data, 1, true); //transmit received char over RF

		//wait until the packet has been sent
		while (!(nrf24l01_irq_pin_active() && nrf24l01_irq_tx_ds_active()))
			;

		nrf24l01_irq_clear_all(); //clear interrupts again
		nrf24l01_set_as_rx(true); //resume normal operation as an RX

		nrf24l01_flush_rx();
		nrf24l01_flush_tx();

		printf("Command: %d", nrf24l01_execute_command(nrf24l01_R_RX_PL_WID, &data, 1, 0));
		printf("REsu = %d \n\r", data);
		ToggleLED(); //toggle the on-board LED as visual indication that the loop has completed
	}
}

//initialize routine
void Initialize(void) {
	WDTCTL = WDTPW + WDTHOLD; 	// Stop WDT
	BCSCTL1 = CALBC1_1MHZ;      // Set DCO
	DCOCTL = CALDCO_1MHZ;
	P1DIR |= BIT0;
	P1OUT |= BIT0;
	P2DIR &= ~BIT1;
	uart_init();
	uart_puts("Now u see me\n\r");
	SPI_Init();

	nrf24l01_initialize_debug(true, 1, false); //initialize the 24L01 to the debug configuration as RX, 1 data byte, and auto-ack disabled

}

//toggles on-board LED
void ToggleLED(void) {
	P1OUT ^= BIT0;
}
