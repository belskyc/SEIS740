/*
===============================================================================
 SEIS-740: Real-Time Systems Project
 Name        : uart.c
 Author      : Chris Belsky
 Version     :
 Description : UART routines.  Based on RDB sample code.
===============================================================================
*/


#include "LPC17xx.h"
#include "externs.h"
#include "uart.h"
#include <stdio.h>

#include "FreeRTOS.h"
#include "semphr.h"

// --- UART GLOBAL Variable DEFINITIONS: --- //
xSemaphoreHandle xUARTCountSemaphore;
uint8_t RxBuf[2];
uint8_t UART3_LSR, UART3_IIR;
char RxIRQ_Fired;

/* HYPERTERMINAL NOTES:
 * Use the following serial port settings:
 * - 57600 baud
 * - 8 data bits
 * - No parity
 * - 1 stop bit
 * - No flow control
 * Use the following Hyperterminal settings:
 * File -> Properties -> Settings Tab -> ASCII Setup ->
 * - Check "Echo typed characters locally"
 * - Check "Send line ends with line feeds"
 */

// *******************************************************************//
// UART3
// *******************************************************************//
// Function to set up UART3
void UART3_Init(int baudrate)
{
	/* OLD:
	int pclk;
	unsigned long int Fdiv;
	// PCLK_UART3 is being set to 1/4 of SystemCoreClock
	pclk = SystemCoreClock / 4;
	*/

	// Assumes CCLK = 12MHz

	// Turn on power to UART3
	LPC_SC->PCONP |= PCUART3_POWERON;

	// Turn on UART3 peripheral clock
	// LPC_SC->PCLKSEL1 &= ~(PCLK_UART3_MASK);
	LPC_SC->PCLKSEL1 &= ~(3 << PCLK_UART3);		// PCLK_periph = CCLK/4
	LPC_SC->PCLKSEL1 |= (2 << PCLK_UART3);		// PCLK_periph = CCLK/2

	// Set PINSEL0 so that P0.0 = TXD3, P0.1 = RXD3
	LPC_PINCON->PINSEL0 &= ~0x0F;
	LPC_PINCON->PINSEL0 |= 0x0A;  // ((1 << 1) | (1 << 3));
	// Ensure that the Rx pin P0.1 has NEITHER pull-up or pull-down resistors enabled.
	LPC_PINCON->PINMODE0 &= ~(3 << 2);
	LPC_PINCON->PINMODE0 |= (1 << 3);

	LPC_UART3->LCR = 0x83;		// 8 bits, 1 Stop bit, no Parity, DLAB=1
    /* OLD:
	Fdiv = ( pclk / 16 ) / baudrate ;	// Set baud rate
    LPC_UART3->DLM = Fdiv / 256;
    LPC_UART3->DLL = Fdiv % 256;
    */
	// Calculated to get 57600 by flowchart on p.314 of LPC17xx User Guide.
	// !!Important!!: If the fractional divider is active (DIVADDVAL > 0) and DLM = 0,
	//  the value of the DLL register must be GREATER THAN 2.
    LPC_UART3->DLM = 0x00;
	LPC_UART3->DLL = 0x04;
	LPC_UART3->FDR = 0x85;  // DIVADDVAL = 5, MULVAL = 8
    LPC_UART3->LCR &= ~(0x80);	  // DLAB = 0

    LPC_UART3->FCR &= ~(3 << 6);  // Rx Trigger Level = 1.
    LPC_UART3->FCR |= 0x07;	      // Enable and reset TX and RX FIFO.
    NVIC_EnableIRQ(UART3_IRQn);	  // Enable the UART3 interrupt.
    LPC_UART3->IER |= 0x01;       // Enable Rx (RBR) Interrupt.
}

// *******************************************************************//
// Function to send character over UART3
void UART3_Sendchar(char c)
{
	while((LPC_UART3->LSR & LSR_THRE) == 0);	// Block until tx empty

	LPC_UART3->THR = c;
}

// *******************************************************************//
// Function to get character from UART3
// char UART3_Getchar()
uint8_t UART3_Getchar()
{
	// char c;
	uint8_t c;
	while((LPC_UART3->LSR & LSR_RDR) == 0);  // Nothing received so just block
	UART3_IIR = LPC_UART3->IIR;  // Clear IIR
	UART3_LSR = LPC_UART3->LSR;  // Clear LSR
	c = LPC_UART3->RBR; // Read Receiver buffer register
	// LPC_UART3->FCR |= 0x02;  // Reset RX FIFO to clear remaining chars.
	return c;
}

// *******************************************************************//
// Function to print the string out over the UART3
void UART3_PrintString(char *pcString)
{
	int i = 0;
	// loop through until reach string's zero terminator
	while (pcString[i] != 0)
	{
		UART3_Sendchar(pcString[i]); // print each character
		i++;
	}
}

//
void UART3_IRQHandler(void)
{
	// Assumes two bytes in the Rx buffer.
	RxBuf[0] = UART3_Getchar();
	// RxBuf[1] = UART3_Getchar();
	//RxBuf[0] = LPC_UART3->RBR;
	//RxBuf[1] = LPC_UART3->RBR;
	RxIRQ_Fired = 1; // Set Flag.

	// Give the semaphore
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* 'Give' the semaphore multiple times.  The first will unblock the handler
	task, the following 'gives' are to demonstrate that the semaphore latches
	the events to allow the handler task to process them in turn without any
	events getting lost.  This simulates multiple interrupts being taken by the
	processor, even though in this case the events are simulated within a single
	interrupt occurrence.*/
	xSemaphoreGiveFromISR( xUARTCountSemaphore, &xHigherPriorityTaskWoken );


    /* Giving the semaphore may have unblocked a task - if it did and the
    unblocked task has a priority equal to or above the currently executing
    task then xHigherPriorityTaskWoken will have been set to pdTRUE and
    portEND_SWITCHING_ISR() will force a context switch to the newly unblocked
    higher priority task.

    NOTE: The syntax for forcing a context switch within an ISR varies between
    FreeRTOS ports.  The portEND_SWITCHING_ISR() macro is provided as part of
    the Cortex-M3 port layer for this purpose.  taskYIELD() must never be called
    from an ISR! */
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

	// uint8_t x;
	/*
	char command;
	command = RxBuf[0];
	printf("RxBuf[0] = %x \n", RxBuf[0]);
	printf("RxBuf[1] = %x \n", RxBuf[1]);
	printf("command = %x \n", command);
	switch (command)
	{
	case 'C':
		printf("Understood the command = %x\r", command);
		break;
	default:
		printf("Did NOT understand the command = %x\r", command);
		break;
	}
	UART3_PrintString ("\rUART3 waiting.  Input next CHAR command = ");  // Send string over UART3.
	*/
	return;
}


// *******************************************************************//
// UART0 : NOT USED WITH NGX base board!
// *******************************************************************//
// Function to set up UART0
void UART0_Init(int baudrate)
{
	int pclk;
	unsigned long int Fdiv;

	// PCLK_UART0 is being set to 1/4 of SystemCoreClock
	pclk = SystemCoreClock / 4;	
	
	// Turn on power to UART0
	LPC_SC->PCONP |=  PCUART0_POWERON;
		
	// Turn on UART0 peripheral clock
	LPC_SC->PCLKSEL0 &= ~(PCLK_UART0_MASK);
	LPC_SC->PCLKSEL0 |=  (0 << PCLK_UART0);		// PCLK_periph = CCLK/4
	
	// Set PINSEL0 so that P0.2 = TXD0, P0.3 = RXD0
	LPC_PINCON->PINSEL0 &= ~0xf0;
	LPC_PINCON->PINSEL0 |= ((1 << 4) | (1 << 6));
	
	LPC_UART0->LCR = 0x83;		// 8 bits, no Parity, 1 Stop bit, DLAB=1
    Fdiv = ( pclk / 16 ) / baudrate ;	// Set baud rate
    LPC_UART0->DLM = Fdiv / 256;							
    LPC_UART0->DLL = Fdiv % 256;
    LPC_UART0->LCR = 0x03;		// 8 bits, no Parity, 1 Stop bit DLAB = 0
    LPC_UART0->FCR = 0x07;		// Enable and reset TX and RX FIFO
}

// ***********************
// Function to send character over UART
void UART0_Sendchar(char c)
{
	while( (LPC_UART0->LSR & LSR_THRE) == 0 );	// Block until tx empty
	
	LPC_UART0->THR = c;
}

// ***********************
// Function to get character from UART
char UART0_Getchar()
{
	char c;
	while( (LPC_UART0->LSR & LSR_RDR) == 0 );  // Nothing received so just block 	
	c = LPC_UART0->RBR; // Read Receiver buffer register
	return c;
}

// ***********************
// Function to prints the string out over the UART
void UART0_PrintString(char *pcString)
{
	int i = 0;
	// loop through until reach string's zero terminator
	while (pcString[i] != 0) {	
		UART0_Sendchar(pcString[i]); // print each character
		i++;
	}
}
