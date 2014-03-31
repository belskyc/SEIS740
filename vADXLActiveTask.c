/*
 * SEIS-740, Spring 2014, Real-Time-Systems
 * Class Project
 * Chris Belsky & Jeff Hatch
 *
 * NXP LPC1769 uC running FreeRTOS - ADXL "Active" Interrupt Task
*/

#include "stdlib.h"
#include "stdio.h"

// FreeRTOS.org includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
// FreeRTOS Demo includes.
#include "basic_io.h"

#include "system_LPC17xx.h"
#include "LPC17xx.h"			/* LPC17xx Peripheral Registers */

#include "vTasks.h"
#include "externs.h"
#include "ssp.h"
#include "ADXL345_init.h"
#include "ADXL345.h"
#include "uart.h"

void vADXLActiveTask( void *pvParameters )
{
	const char *pcTaskName = "ADXL-Active Task...\n";
	//uint8_t k = 0;

	/* Print out the name of this task. */
	vPrintString( pcTaskName );

	while(1)
	{
		/* Use the semaphore to wait for the event.  The semaphore was created
		before the scheduler was started so before this task ran for the first
		time.  The task blocks indefinitely meaning this function call will only
		return once the semaphore has been successfully obtained - so there is no
		need to check the returned value. */
		xSemaphoreTake( xADXLActiveSemaphore, portMAX_DELAY );

		vPrintString( pcTaskName );  // Do something like printing the task name.

		// Clear the Activity Interrupt by reading the INT_SOURCE register from the ADXL345.
		//k = readADXL345(REG_INT_SOURCE);
		//printf("REG_INT_SOURCE = %x\n", k);

		itoa(junkEINT0, s, 10);
		UART3_PrintString ("junkEINT0 = ");  // Send string over UART3.
		UART3_PrintString (s);  // Send string over UART3.
		UART3_PrintString ("\r");  // Send string over UART3.
	}
}
/*--------END of ADXL "Active" Interrupt Task---------------------------------------------------*/
