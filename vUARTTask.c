/*
 * SEIS-740, Spring 2014, Real-Time-Systems
 * Class Project
 * Chris Belsky & Jeff Hatch
 *
 * NXP LPC1769 uC running FreeRTOS - UART Task
*/

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
#include "uart.h"



void vUARTTask( void *pvParameters )
{
	const char *pcTaskName = "UART task\n";
	volatile unsigned long ul;
	char command;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{

		/* Use the semaphore to wait for the event.  The semaphore was created
		before the scheduler was started so before this task ran for the first
		time.  The task blocks indefinitely meaning this function call will only
		return once the semaphore has been successfully obtained - so there is no
		need to check the returned value. */
		xSemaphoreTake( xUARTCountSemaphore, portMAX_DELAY );

		/* Print out the name of this task. */
		vPrintString( pcTaskName );

		if(RxIRQ_Fired)
		{
			// Process UART3
			command = RxBuf[0];
			printf("RxBuf[0] = 0x%x \n", RxBuf[0]);
			printf("RxBuf[1] = 0x%x \n", RxBuf[1]);
			printf("command = 0x%x \n", command);
			switch (command)
			{
			case 'C':
				printf("Understood the command = %x\r", command);
				break;
			default:
				printf("Did NOT understand the command = %x\r", command);
				break;
			}
			UART3_PrintString ("UART3 waiting.  Input next CHAR command = ");  // Send string over UART3.
			RxIRQ_Fired = 0; // Clear Flag.
		}
	}
}
/*---------END of UART Task--------------------------------------------------*/
