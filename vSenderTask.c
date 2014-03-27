/*
 * SEIS-740, Spring 2014, Real-Time-Systems
 * Class Project
 * Chris Belsky & Jeff Hatch
 *
 * NXP LPC1769 uC running FreeRTOS - Sender Task
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


#if 0
void vSenderTask( void *pvParameters )
{
portBASE_TYPE xStatus;
const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* The first parameter is the queue to which data is being sent.  The
		queue was created before the scheduler was started, so before this task
		started to execute.

		The second parameter is the address of the structure being sent.  The
		address is passed in as the task parameter.

		The third parameter is the Block time - the time the task should be kept
		in the Blocked state to wait for space to become available on the queue
		should the queue already be full.  A block time is specified as the queue
		will become full.  Items will only be removed from the queue when both
		sending tasks are in the Blocked state.. */
		xStatus = xQueueSendToBack( xQueue, pvParameters, xTicksToWait );

		if( xStatus != pdPASS )
		{
			/* We could not write to the queue because it was full - this must
			be an error as the receiving task should make space in the queue
			as soon as both sending tasks are in the Blocked state. */
			vPrintString( "Could not send to the queue.\n" );
		}

		/* Allow the other sender task to execute. */
		taskYIELD();
	}
}
/*-----------------------------------------------------------*/
#endif
