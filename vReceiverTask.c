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


void vReceiverTask( void *pvParameters )
{
    /* Declare the structure that will hold the values received from the queue. */
    dispReq xReceivedRequest;
    portBASE_TYPE xStatus;
    portTickType xLastWakeTime;

    /* The xLastWakeTime variable needs to be initialized with the current tick
    count.  Note that this is the only time we access this variable.  From this
    point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
    API function. */
    xLastWakeTime = xTaskGetTickCount();

	/* This task is also defined within an infinite loop. */
	for( ;; )
	{
#if 0
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
		xStatus = xQueueReceive( xDisplayQueue, &xReceivedRequest, portMAX_DELAY );

		if( xStatus == pdPASS )
		{
			//vPrintStringAndNumber("Display Request received from ", xReceivedRequest.irID);
			vPrintStringAndNumber("Display Request timer value ", xReceivedRequest.tVal);

			//Display_displayNumber(xReceivedRequest.irID, xReceivedRequest.tVal, 0, 0);
		}
		else
		{
			/* We did not receive anything from the queue.  This must be an error 
			as this task should only run when the queue is full. */
			vPrintString( "Could not receive from the dispaly queue.\n" );
			if (xStatus == errQUEUE_EMPTY)
			{
				vPrintString( "The queue is empty.\n");
			}
		}
#endif

	    unsigned int dispVal;

	    dispVal = create_display_val(100000000);  // too high
		//printf("10000 Expected 40404040, dispVal = %x\n", dispVal);
	    //vPrintStringAndNumber("10000 Expected 40404040, dispVal = ", dispVal);
	    //Display_displayNumber(DISP_1_1, dispVal, 0, 0);

		/* We want this task to execute exactly every 250 milliseconds.  As per
		the vTaskDelay() function, time is measured in ticks, and the
		portTICK_RATE_MS constant is used to convert this to milliseconds.
		xLastWakeTime is automatically updated within vTaskDelayUntil() so does not
		have to be updated by this task code. */
		vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_RATE_MS ) );

		dispVal = create_display_val(99990000); // 9999
		//printf(" 9999 Expected 67676767, dispVal = %x\n", dispVal);
	    //vPrintStringAndNumber(" 9999 Expected 67676767, dispVal = ", dispVal);
	    //Display_displayNumber(DISP_1_1, dispVal, 0, 0);

		vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_RATE_MS ) );

		dispVal = create_display_val(8765000); // 876.5
		//printf("876.5 Expected 7f07fd6b, dispVal = %x\n", dispVal);
	    //vPrintStringAndNumber("876.5 Expected 7f07fd6b, dispVal = ", dispVal);
	    //Display_displayNumber(DISP_1_1, dispVal, 0, 0);

		vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_RATE_MS ) );

	    dispVal = create_display_val(765400); // 76.54
		//printf("76.54 Expected 07fd6b66, dispVal = %x\n", dispVal);
	    //vPrintStringAndNumber("76.54 Expected 07fd6b66, dispVal = ", dispVal);
	    //Display_displayNumber(DISP_1_1, dispVal, 0, 0);

		vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_RATE_MS ) );

		dispVal = create_display_val(65430); // 6.543
		//printf("6.543 Expected fd6b664f, dispVal = %x\n", dispVal);
	    //vPrintStringAndNumber("6.543 Expected fd6b664f, dispVal = ", dispVal);
	    //Display_displayNumber(DISP_1_1, dispVal, 0, 0);

		vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_RATE_MS ) );

		dispVal = create_display_val(5430); // 0.543
		//printf("0.543 Expected bf6b664f, dispVal = %x\n", dispVal);
	    //vPrintStringAndNumber("0.543 Expected bf6b664f, dispVal = ", dispVal);
	    //Display_displayNumber(DISP_1_1, dispVal, 0, 0);

		vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_RATE_MS ) );

		dispVal = create_display_val(432);  // 0.043
		//printf("0.043 Expected bf3f664f, dispVal = %x\n", dispVal);
	    //vPrintStringAndNumber("0.043 Expected bf3f664f, dispVal = ", dispVal);
	    //Display_displayNumber(DISP_1_1, dispVal, 0, 0);

		vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_RATE_MS ) );

		dispVal = create_display_val(32);   // 0.003
		//printf("0.003 Expected bf3f3f4f, dispVal = %x\n", dispVal);
	    //vPrintStringAndNumber("0.003 Expected bf3f3f4f, dispVal = ", dispVal);
	    //Display_displayNumber(DISP_1_1, dispVal, 0, 0);

		vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_RATE_MS ) );

		dispVal = create_display_val(1);    // 0.000
		//printf("0.000 Expected bf3f3f3f, dispVal = %x\n", dispVal);
	    //vPrintStringAndNumber("0.000 Expected bf3f3f3f, dispVal = ", dispVal);
	    //Display_displayNumber(DISP_1_1, dispVal, 0, 0);

		vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_RATE_MS ) );

		dispVal = create_display_val(0);    // 0.000
		//printf("0,000 Expected bf3f3f3f, dispVal = %x\n", dispVal);
	    vPrintStringAndNumber("0,000 Expected bf3f3f3f, dispVal = ", dispVal);
	    //Display_displayNumber(DISP_1_1, dispVal, 0, 0);

		vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_RATE_MS ) );
	}
}
/*------END of vReceiverTask-----------------------------------------------------*/
