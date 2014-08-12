#include <msp430g2553.h>
/*
 * SPI.h
 *
 *  Created on: Nov 22, 2012
 *      Author: Admin
 */

#ifndef SPI_H_
#define SPI_H_

/**
 * SPI Clock at P1.5
 */
#define SCLK    BIT5

/**
 * SPI SOMI (Slave Out, Master In) at P1.6
 */
#define SOMI    BIT6

/**
 * SPI SIMO (Slave In, Master Out) at P1.7
 */
#define SIMO    BIT7

/**
 * CSN (Chip Select) at P1.3
 */
#define CSN      BIT3
/**
 * CE (Chip Select) at P1.4
 */
#define CE 		BIT4


void spi_csh(void);
void spi_csl(void);
void SPI_Init(void);
void Send_byte(int data, int time_ms);
unsigned char SPI_Xfer_byte(unsigned char data);

#endif /* SPI_H_ */
