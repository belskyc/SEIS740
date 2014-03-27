/*
 * SEIS-740 Project
 * NXP LPC1769 uC - External Interrupts
 *
 * Authors: Chris Belsky & Jeff Hatch
 */
#include "LPC17xx.h"			/* LPC17xx Peripheral Registers */
#include "FreeRTOS.h"
#include "semphr.h"

// ADXL345 interrupt semaphore
extern xSemaphoreHandle xCountingSemaphore;

void
EINT1_Init()
{

	// Set P2[11] GPIO pin 52 to EINT1
	LPC_PINCON->PINSEL4 &= ~(0x3<<22);
	LPC_PINCON->PINSEL4 |= (0x1<<22);

	/* Set to interrupt rising edge */
	LPC_SC->EXTINT |= (0x1<<1); // this can only be cleared when the ADXL345 has cleared the INT2 line

	/* Set to Level Sensitive */
	LPC_SC->EXTMODE |= (0x1<<1);  // Only do this when the NVIC is disabled

	/* Set to Active High */
	LPC_SC->EXTPOLAR |= (0x1<<1);

	/* The interrupt service routine uses an (interrupt safe) FreeRTOS API
	function so the interrupt priority must be at or below the priority defined
	by configSYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority( EINT1_IRQn, 5 );

	/* Enable the interrupt. */
	NVIC_EnableIRQ( EINT1_IRQn );
}


//
void EINT1_IRQHandler(void)
{
	// printf("Entered EINT1_IRQHandler().\n;");

	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	// Give Semaphore to trigger the ADXL task to process the ADXL reading.
	xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );

	/************************************************************************/
	/* Clear the software interrupt bit using the interrupt controllers
	Clear Pending register. */
	NVIC_ClearPendingIRQ( EINT1_IRQn );
	LPC_SC->EXTINT |= 0x02;
	/************************************************************************/

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
	return;
}
