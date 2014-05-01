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

#include "irSensors.h"
#include "timer.h"

// ADXL345 interrupt semaphores: defined in main.c
extern xSemaphoreHandle xCountingSemaphore;    // Used in EINT1 handling ADXL345 readings.
extern xSemaphoreHandle xADXLActiveSemaphore;  // Used in EINT0 handling ADXL345 "Active" interrupts.
extern xQueueHandle xDisplayQueue;
extern xQueueHandle xUARTQueue;
extern dispReq DisplayRequests[];
extern uint8_t MUX_IR_index2;
extern uint8_t MUX_IR_index1;

uint8_t IR_IRQ_errors[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// Junk variables used for debugging.
char junkEINT0 = 0;
char junkEINT1 = 0;
char junkEINT2 = 0;
char junkEINT3 = 0;

// IR sensor IDs for each lane
static uint32_t IR_LANE1_ID = IR_ID_1_1;
static uint32_t IR_LANE2_ID = IR_ID_2_1;

// The NXP LPC1769 uC has four (4) external interrupts.
// The EINT interrupts are active LOW.
/******* External Interrupt EINT0 *******/
// Used for ADXL345 "Activity" Interrupt.
void EINT0_Init()
{
	// Ensure the Interrupt is DISABLED for configuring:
	NVIC_DisableIRQ( EINT0_IRQn );

	// Set P2[10] GPIO pin 53 to EINT0 (pin J6-51 of LPCXpresso board)
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

	timer0_reference = timer0_counter;
	//portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	// Give Semaphore to trigger the ADXL task to process the ADXL reading.
	//xSemaphoreGiveFromISR( xADXLActiveSemaphore, &xHigherPriorityTaskWoken );

	/************************************************************************/
	/* Clear the software interrupt bit using the interrupt controllers
	Clear Pending register. */
	NVIC_ClearPendingIRQ( EINT0_IRQn );
	LPC_SC->EXTINT |= 0x01;
	/************************************************************************/
	// Clear the Activity Interrupt by reading the INT_SOURCE register from the ADXL345.
	//k = readADXL345(REG_INT_SOURCE);
	//printf("EINT0: REG_INT_SOURCE = %x\n", k);
	/************************************************************************/

	//junkEINT0++;  // DEBUG variable.

	/* Giving the semaphore may have unblocked a task - if it did and the
	unblocked task has a priority equal to or above the currently executing
	task then xHigherPriorityTaskWoken will have been set to pdTRUE and
	portEND_SWITCHING_ISR() will force a context switch to the newly unblocked
	higher priority task.

	NOTE: The syntax for forcing a context switch within an ISR varies between
	FreeRTOS ports.  The portEND_SWITCHING_ISR() macro is provided as part of
	the Cortex-M3 port layer for this purpose.  taskYIELD() must never be called
	from an ISR! */
	//portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

	return;
}

/******* External Interrupt EINT1 *******/
// Used for ADXL345 "Watermark" (reading) interrupt.
void EINT1_Init()
{
	// Ensure the Interrupt is DISABLED for configuring:
	NVIC_DisableIRQ( EINT1_IRQn );

	// Set P2[11] GPIO pin 52 to EINT1 (pin J6-52 of LPCXpresso board)
	LPC_PINCON->PINSEL4 &= ~(0x3 << 22);
	LPC_PINCON->PINSEL4 |= (0x1 << 22);

	// Clear the interrupt flag:
	LPC_SC->EXTINT |= (0x1 << 1); // this can only be cleared when the ADXL345 has cleared the INT2 line

	// Set to EDGE sensitive triggering (vs. LEVEL triggering)
	LPC_SC->EXTMODE |= (0x1 << 1);  // Only do this when the NVIC is disabled

	// Set to interrupt on RISING edge (vs. FALLING edge)
	// LPC_SC->EXTPOLAR |= (0x1 << 1);
	// Set to interrupt on FALLING edge (vs. RISING edge)
	LPC_SC->EXTPOLAR &= ~(0x1 << 1);

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
	printf("Entered EINT1_IRQHandler().\n;");
	uint8_t k = 0;

	timer0_reference = timer0_counter;
	//portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	// Give Semaphore to trigger the ADXL task to process the ADXL reading.
	//xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );

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
	//portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

	return;
}

/******* External Interrupt EINT2 *******/
// Used for IR-Sensor
void EINT2_Init()
{
	// Ensure the Interrupt is DISABLED for configuring:
	NVIC_DisableIRQ( EINT2_IRQn );

	// Set P2[12] GPIO pin 51 to EINT2 (pin J6-53 of LPCXpresso board)
	LPC_PINCON->PINSEL4 &= ~(0x3 << 24);
	LPC_PINCON->PINSEL4 |= (0x1 << 24);

	// Clear the interrupt flag:
	LPC_SC->EXTINT |= (0x1 << 2);  // this can only be cleared when the device has cleared the INT line

	// Set to EDGE sensitive triggering (vs. LEVEL triggering)
	LPC_SC->EXTMODE |= (0x1 << 2);  // Only do this when the NVIC is disabled

	// Set to interrupt on FALLING edge (vs. RISING edge)
	LPC_SC->EXTPOLAR &= ~(0x1 << 2);

	/* The interrupt service routine uses an (interrupt safe) FreeRTOS API
	function so the interrupt priority must be at or below the priority defined
	by configSYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority( EINT2_IRQn, 5 );

	/* Enable the interrupt. */
	// NVIC_EnableIRQ( EINT2_IRQn ); // Enable is later in main.c
}

// EINT2 ISR
void EINT2_IRQHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	junkEINT2++;  // DEBUG variable.
	//printf("Entered EINT2_IRQHandler()... junkEINT2 = %d\n", junkEINT2);

	// Put the timer value on the queue from the appropriate IR sensor
	portBASE_TYPE nQStatus;
    // copy the timer value
	DisplayRequests[IR_LANE2_ID].tVal = timer0_counter - timer0_reference;
	// Enqueue the Request
	nQStatus = xQueueSendToBackFromISR(xDisplayQueue, &DisplayRequests[IR_LANE2_ID], &xHigherPriorityTaskWoken);
	if (nQStatus != pdPASS)
	{
		IR_IRQ_errors[IR_LANE2_ID] = 1;
	}
	nQStatus = xQueueSendToBackFromISR(xUARTQueue, &DisplayRequests[IR_LANE2_ID], &xHigherPriorityTaskWoken);
	if (nQStatus != pdPASS)
	{
		IR_IRQ_errors[IR_LANE2_ID] = 1;
	}

	IR_LANE2_ID++;
	MUX_IR_index2++;

	if (IR_LANE2_ID > IR_ID_2_4)
	{
		IR_LANE2_ID = IR_ID_2_1;
		MUX_IR_index2 = 0;
	}

	// Advance the MUX
    // Set P2.0 and P2.1 to output 0
    LPC_GPIO2->FIOCLR0 |= (0x03);
    LPC_GPIO2->FIOSET0 |= (MUX_IR_index2 & 0x03);


	/************************************************************************/
	/* Clear the software interrupt bit using the interrupt controllers
	Clear Pending register. */
	NVIC_ClearPendingIRQ( EINT2_IRQn );
	LPC_SC->EXTINT |= 0x04;
	/************************************************************************/

	return;
}

/******* External Interrupt EINT3 *******/
// Used for ??
void EINT3_Init()
{
	// Ensure the Interrupt is DISABLED for configuring:
	NVIC_DisableIRQ( EINT3_IRQn );

	// Set P2[13] GPIO pin 50 to EINT3 (pin J6-27 of LPCXpresso board)
	LPC_PINCON->PINSEL4 &= ~(0x3 << 26);
	LPC_PINCON->PINSEL4 |= (0x1 << 26);

	// Clear the interrupt flag:
	LPC_SC->EXTINT |= (0x1 << 3);  // this can only be cleared when the device has cleared the INT line

	// Set to EDGE sensitive triggering (vs. LEVEL triggering)
	LPC_SC->EXTMODE |= (0x1 << 3);  // Only do this when the NVIC is disabled

	// Set to interrupt on FALLING edge (vs. RISING edge)
	LPC_SC->EXTPOLAR &= ~(0x1 << 3);

	/* The interrupt service routine uses an (interrupt safe) FreeRTOS API
	function so the interrupt priority must be at or below the priority defined
	by configSYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority( EINT3_IRQn, 5 );

	/* Enable the interrupt. */
	// NVIC_EnableIRQ( EINT3_IRQn ); // Enable is later in main.c
}

// EINT3 ISR
void EINT3_IRQHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	junkEINT3++;  // DEBUG variable.
	//printf("Entered EINT3_IRQHandler()... junkEINT3 = %d\n", junkEINT3);

	// Put the timer value on the queue from the appropriate IR sensor
	portBASE_TYPE nQStatus;
    // copy the timer value
	DisplayRequests[IR_LANE1_ID].tVal = timer0_counter - timer0_reference;
	// Enqueue the Request
	nQStatus = xQueueSendToBackFromISR(xDisplayQueue, &DisplayRequests[IR_LANE1_ID], &xHigherPriorityTaskWoken);
	if (nQStatus != pdPASS)
	{
		IR_IRQ_errors[IR_LANE1_ID] = 1;
	}
	nQStatus = xQueueSendToBackFromISR(xUARTQueue, &DisplayRequests[IR_LANE1_ID], &xHigherPriorityTaskWoken);
	if (nQStatus != pdPASS)
	{
		IR_IRQ_errors[IR_LANE1_ID] = 1;
	}

	IR_LANE1_ID++;
	MUX_IR_index1++;

	if (IR_LANE1_ID > IR_ID_1_4)
	{
		IR_LANE1_ID = IR_ID_1_1;
		MUX_IR_index1 = 0;
	}

	// Advance the MUX
    // Set P2.2 and P2.3 to output 0
    LPC_GPIO2->FIOCLR0 |= (0x0c);
    LPC_GPIO2->FIOSET0 |= ((MUX_IR_index1 << 2) & 0x0C);


	/************************************************************************/
	/* Clear the software interrupt bit using the interrupt controllers
	Clear Pending register. */
	NVIC_ClearPendingIRQ( EINT3_IRQn );
	LPC_SC->EXTINT |= 0x08;
	/************************************************************************/

	return;
}

