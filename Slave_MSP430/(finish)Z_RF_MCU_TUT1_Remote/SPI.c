
#include "SPI.h"
//int b[10]={0x40,0xF9,0x24,0x30,0x19,0x12,0x02,0xF8,0x80,0x10};
/*
 * SPI.c
 *
 *  Created on: Nov 22, 2012
 *      Author: Admin
 */
void SPI_Init(void)
{
	/**
	 * From TIs users manual
	 *
     * The recommended USCI initialization/re-configuration process is:
 	 * 1. Set UCSWRST (BIS.B #UCSWRST,&UCxCTL1)
 	 * 2. Initialize all USCI registers with UCSWRST=1 (including UCxCTL1)
 	 * 3. Configure ports
 	 * 4. Clear UCSWRST via software (BIC.B #UCSWRST,&UCxCTL1)
 	 * 5. Enable interrupts (optional) via UCxRXIE and/or UCxTXIE
 	 */

	// (1)
	UCB0CTL1 = UCSWRST;

	// (2)
	P2DIR  &= ~BIT1;
	P1DIR  |= CE;
    P1DIR  |= CSN;
  	P1SEL  |= SOMI + SIMO + SCLK;
  	P1SEL2 |= SOMI + SIMO + SCLK;

    // (3) 3-pin, 8-bit SPI master
    UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC;
	UCB0CTL1 |= UCSSEL_2;   // SMCLK

	// (4)
	UCB0CTL1 &= ~UCSWRST;

	spi_csh();
}


void Send_byte(int data, int time_ms)
{
	  int i_delay;
	//  while (!(IFG2 & UCB0TXIFG));              // USCI_A0 TX buffer ready?
	  UCB0TXBUF = data;                     // Send next value
	  for (i_delay = 0; i_delay<time_ms; i_delay++ )
	  {
		_delay_cycles(1000);
	  }
}
unsigned char SPI_Xfer_byte(unsigned char data)
{
  	UCB0TXBUF = data;
	// wait for TX
	while (!(IFG2 & UCB0TXIFG));
	return UCB0RXBUF;
}
void spi_csh(void)
{
    P1OUT |= CSN;
}
void spi_csl(void)
{
    P1OUT &= ~CSN;
}
