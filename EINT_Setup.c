/*
 * SEIS-740 Project
 * NXP LPC1769 uC - External Interrupts
 *
 * Authors: Chris Belsky & Jeff Hatch
 */

#include "stdio.h"

#include "LPC17xx.h"			/* LPC17xx Peripheral Registers */

#include "FreeRTOS.h"
#include "semphr.h"

#include "ADXL345_init.h"
#include "ADXL345.h"

// ADXL345 interrupt semaphores: defined in main.c
extern xSemaphoreHandle xCountingSemaphore;    // Used in EINT1 handling ADXL345 readings.
extern xSemaphoreHandle xADXLActiveSemaphore;  // Used in EINT0 handling ADXL345 "Active" interrupts.

char junkEINT0 = 0; // Junk variable used for debugging.

// The NXP LPC1769 uC has four (4) external interrupts.
// The EINT interrupts are active LOW.
/******* External Interrupt EINT0 *******/
void EINT0_Init()
{
	// Ensure the Interrupt is DISABLED for configuring:
	NVIC_DisableIRQ( EINT0_IRQn );

	// Set P2[10] GPIO pin 53 to EINT0
	LPC_PINCON->PINSEL4 &= ~(0x3 << 20);
	LPC_PINCON->PINSEL4 |= (0x1 << 20);

	// Clear the interrupt flag:
	LPC_SC->EXTINT |= (0x1 << 0);  // this can only be cleared when the ADXL345 has cleared the INT line

	// Set to EDGE sensitive triggering (vs. LEVEL triggering)
	LPC_SC->EXTMODE |= (0x1 << 0);  // Only do this when the NVIC is disabled

	// Set to interrupt on FALLING edge (vs. RISING edge)
	LPC_SC->EXTPOLAR &= ~(0x1 << 0);

	/* The interrupt service routine uses an (interrupt safe) FreeRTOS API
	function so the interrupt priority must be at or below the priority defined
	by configSYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority( EINT0_IRQn, 5 );

	/* Enable the interrupt. */
	// NVIC_EnableIRQ( EINT0_IRQn ); // Enable is later in main.c
}

// EINT0 ISR
void EINT0_IRQHandler(void)
{
	printf("Entered EINT0_IRQHandler()...\n");
	uint8_t k = 0;

	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	// Give Semaphore to trigger the ADXL task to process the ADXL reading.
	xSemaphoreGiveFromISR( xADXLActiveSemaphore, &xHigherPriorityTaskWoken );

	/************************************************************************/
	/* Clear the software interrupt bit using the interrupt controllers
	Clear Pending register. */
	NVIC_ClearPendingIRQ( EINT0_IRQn );
	LPC_SC->EXTINT |= 0x01;
	/************************************************************************/
	// Clear the Activity Interrupt by reading the INT_SOURCE register from the ADXL345.
	k = readADXL345(REG_INT_SOURCE);
	printf("EINT0: REG_INT_SOURCE = %x\n", k);
	/************************************************************************/

	junkEINT0++;  // DEBUG variable.

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

/******* External Interrupt EINT1 *******/
void EINT1_Init()
{
	// Ensure the Interrupt is DISABLED for configuring:
	NVIC_DisableIRQ( EINT1_IRQn );

	// Set P2[11] GPIO pin 52 to EINT1
	LPC_PINCON->PINSEL4 &= ~(0x3 << 22);
	LPC_PINCON->PINSEL4 |= (0x1 << 22);

	// Clear the interrupt flag:
	LPC_SC->EXTINT |= (0x1 << 1); // this can only be cleared when the ADXL345 has cleared the INT2 line

	// Set to EDGE sensitive triggering (vs. LEVEL triggering)
	LPC_SC->EXTMODE |= (0x1 << 1);  // Only do this when the NVIC is disabled

	// Set to interrupt on RISING edge (vs. FALLING edge)
	LPC_SC->EXTPOLAR |= (0x1 << 1);

	/* The interrupt service routine uses an (interrupt safe) FreeRTOS API
	function so the interrupt priority must be at or below the priority defined
	by configSYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority( EINT1_IRQn, 5 );

	/* Enable the interrupt. */
	// NVIC_EnableIRQ( EINT1_IRQn );  // Enable is later in main.c
}


// EINT1 ISR
void EINT1_IRQHandler(void)
{
	// printf("Entered EINT1_IRQHandler().\n;");
	//uint8_t k = 0;

	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	// Give Semaphore to trigger the ADXL task to process the ADXL reading.
	xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );

	/************************************************************************/
	/* Clear the software interrupt bit using the interrupt controllers
	Clear Pending register. */
	NVIC_ClearPendingIRQ( EINT1_IRQn );
	LPC_SC->EXTINT |= 0x02;
	/************************************************************************/
	// Clear the Activity Interrupt by reading the INT_SOURCE register from the ADXL345.
	//k = readADXL345(REG_INT_SOURCE);
	//printf("EINT1: REG_INT_SOURCE = %x\n", k);
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

/******* External Interrupt EINT2 *******/
void EINT2_Init()
{

}

/******* External Interrupt EINT3 *******/
void EINT3_Init()
{

}

