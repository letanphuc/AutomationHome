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
#include "stdint.h"
#include "UART.h"
#include "SPI.h"
#include "nrf24l01.h"




void Initialize(void);
void ToggleLED(void); //toggle the current state of the on-board LED
void CheckErrorsUSART(void); //fix any framing or overrun errors in the USART


void configRF_txAddress (uint8_t * add){
	printf("configRF_txAddress\n\r");
	nrf24l01_set_tx_addr(add,5);
	_delay_cycles(100);
}

void RF_sendData(uint8_t * data, uint8_t len){
	uint8_t i = 0;
	for (i = 0; i < len; i++){
		nrf24l01_write_tx_payload(data + i, 1, true);
		while (!(nrf24l01_irq_pin_active() && nrf24l01_irq_tx_ds_active()));
	}
}
uint8_t RF_sendByte(uint8_t byte){
	uint8_t reply = 0;
	uint8_t count = 0;

	nrf24l01_write_tx_payload(&byte, 1, true);
	while (!(nrf24l01_irq_pin_active() && nrf24l01_irq_tx_ds_active()));
	nrf24l01_irq_clear_all();
	nrf24l01_set_as_rx(true);

	for (count = 0; count < 100; count++) {

		if ((nrf24l01_irq_pin_active() && nrf24l01_irq_rx_dr_active())) {
			nrf24l01_read_rx_payload(&reply, 1); //get the payload into data
			break;
		}

	}

	nrf24l01_irq_clear_all(); //clear interrupts again


	_delay_cycles(130); //wait for receiver to come from standby to RX
	nrf24l01_set_as_tx(); //resume normal operation as a TX

	return reply;
}

uint8_t* RF_sendArray(uint8_t* data, uint8_t len){
	uint8_t i = 0;
	static uint8_t reply[32] = {0};

	for (i = 0; i < len; i++)
		reply[i] = RF_sendByte(data[i]);

	return reply;
}

void main(void) {
	uint8_t data = 32;
	uint8_t new_data = 21;
	uint8_t destIP[5] = { 192, 168, 1, 1, 111 };

	Initialize(); //initialize IO, UART, SPI, set up nRF24L01 as TX

	printf("Local host now run\n\r");

	configRF_txAddress(destIP);
	while (1) {

		data = uart_getc();
		new_data = RF_sendByte(data);


		printf("\n\rData send: %d -> %d\n\r", data, new_data);
		if (new_data != data)
			printf("Erororrororororr\n\r");

		ToggleLED(); //toggle the on-board LED as visual indication that the loop has completed
	}
}

//initialize routine
void Initialize(void) {
	WDTCTL = WDTPW + WDTHOLD; 	// Stop WDT
	BCSCTL1 = CALBC1_1MHZ;      // Set DCO
	DCOCTL = CALDCO_1MHZ;
	P1DIR |= BIT0;
	P2DIR |= BIT2;
	P2OUT &= ~BIT2;

	uart_init();

	uart_puts("Hello i'm running\n\r");

	SPI_Init();
	nrf24l01_initialize_debug(false, 1, false); //initialize the 24L01 to the debug configuration as TX, 1 data byte, and auto-ack disabled
}
void ToggleLED(void) {
	P1OUT ^= BIT0; //invert the bit that controls the LED
}
