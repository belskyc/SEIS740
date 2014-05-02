/*
 * SEIS-740, Spring 2014, Real-Time-Systems
 * Class Project
 * Chris Belsky & Jeff Hatch
 * 
 * NXP LPC1769 uC running FreeRTOS - Receiver Task
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
#include "irSensors.h"
#include "display.h"


void vDisplayTask( void *pvParameters )
{
    /* Declare the structure that will hold the values received from the queue. */
    dispReq xReceivedRequest;
    portBASE_TYPE xStatus;
    portTickType xLastWakeTime;

	vPrintString( "vDisplayTask started\n" );
    /* The xLastWakeTime variable needs to be initialized with the current tick
    count.  Note that this is the only time we access this variable.  From this
    point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
    API function. */
    xLastWakeTime = xTaskGetTickCount();

	/* This task is also defined within an infinite loop. */
	for( ;; )
	{
		/* As this task only runs when the sending tasks are in the Blocked state, 
		and the sending tasks only block when the queue is full, this task should
		always find the queue to be full.  3 is the queue length. */
		//if( uxQueueMessagesWaiting( xDisplayQueue ) != 3 )
		//{
		//	vPrintString( "Queue should have been full!\n" );
		//}

		/* The first parameter is the queue from which data is to be received.  The
		queue is created before the scheduler is started, and therefore before this
		task runs for the first time.

		The second parameter is the buffer into which the received data will be
		placed.  In this case the buffer is simply the address of a variable that
		has the required size to hold the received structure. 

		The last parameter is the block time - the maximum amount of time that the
		task should remain in the Blocked state to wait for data to be available 
		should the queue already be empty.  A block time is not necessary as this
		task will only run when the queue is full so data will always be available. */
		//vPrintString("vDisplayTask checking queue");
		xStatus = xQueueReceive( xDisplayQueue, &xReceivedRequest, portMAX_DELAY );

		if( xStatus == pdPASS )
		{
			//vPrintStringAndNumber("Display Request received from ", xReceivedRequest.irID);
			//vPrintStringAndNumber("Display Request timer value ", xReceivedRequest.tVal);

			Display_displayNumber(xReceivedRequest.irID, xReceivedRequest.tVal, 0, 0);
		}
		else
		{
			/* We did not receive anything from the queue.  This must be an error 
			as this task should only run when the queue is full. */
			vPrintString( "Could not receive from the display queue.\n" );
			if (xStatus == errQUEUE_EMPTY)
			{
				vPrintString( "The queue is empty.\n");
			}
		}
	}
}
/*------END of vDisplayTask-----------------------------------------------------*/
