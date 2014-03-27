/*
===============================================================================
 SEIS-742: Project 1
 Name        : uart.h
 Author      : Chris Belsky
 Version     :
 Description : UART routines.  Based on RDB sample code.
===============================================================================
*/

#ifndef UART_H_
#define UART_H_

#include "stdint.h"

// PCUART0 : NOT USED WITH NGX base board!
#define PCUART0_POWERON (1 << 3)  // Default is ON (=1).
#define PCLK_UART0 6
#define PCLK_UART0_MASK (3 << 6)  // Set in PCLKSEL0.

// LPCXpresso LPC1769 board w/ NGX base board:
// PCUART3: TXD3 = P0.0, RXD3 = P0.1
#define PCUART3_POWERON (1 << 25) // Default is OFF (=0).
#define PCLK_UART3 18
#define PCLK_UART3_MASK (3 << 18) // Set in PCLKSEL1.

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

// ***********************
// Function to set up UART
void UART0_Init(int baudrate);
void UART3_Init(int baudrate);

// ***********************
// Function to send character over UART
void UART0_Sendchar(char c);
void UART3_Sendchar(char c);

// ***********************
// Function to get character from UART
char UART0_Getchar();
// char UART3_Getchar();
uint8_t UART3_Getchar();

// ***********************
// Function to prints the string out over the UART
void UART0_PrintString(char *pcString);
void UART3_PrintString(char *pcString);

#endif /*UART_H_*/
