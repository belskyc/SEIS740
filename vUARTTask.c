/*
 * SEIS-740, Spring 2014, Real-Time-Systems
 * Class Project
 * Chris Belsky & Jeff Hatch
 *
 * NXP LPC1769 uC running FreeRTOS - UART Task
*/

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
#include "uart.h"
#include "irSensors.h"
#include "display.h"

extern xQueueHandle xUARTQueue;

/*
const char * const LANE1_1_LABEL = "Lane 1 Sensor 1: ";
const char * const LANE1_2_LABEL = "Lane 1 Sensor 2: ";
const char * const LANE1_3_LABEL = "Lane 1 Sensor 3: ";
const char * const LANE1_4_LABEL = "Lane 1 Sensor 4: ";
const char * const LANE2_1_LABEL = "Lane 2 Sensor 1: ";
const char * const LANE2_2_LABEL = "Lane 2 Sensor 2: ";
const char * const LANE2_3_LABEL = "Lane 2 Sensor 3: ";
const char * const LANE2_4_LABEL = "Lane 2 Sensor 4: ";
*/
char * LANE1_1_LABEL = "Lane 1 Sensor 1: ";
char * LANE1_2_LABEL = "Lane 1 Sensor 2: ";
char * LANE1_3_LABEL = "Lane 1 Sensor 3: ";
char * LANE1_4_LABEL = "Lane 1 Sensor 4: ";
char * LANE2_1_LABEL = "Lane 2 Sensor 1: ";
char * LANE2_2_LABEL = "Lane 2 Sensor 2: ";
char * LANE2_3_LABEL = "Lane 2 Sensor 3: ";
char * LANE2_4_LABEL = "Lane 2 Sensor 4: ";

void vUARTTask( void *pvParameters )
{
	const char *pcTaskName = "UART task\n";
	volatile unsigned long ul;
	char command;
	char timeStr[16];

    dispReq xReceivedRequest;
    portBASE_TYPE xStatus;
    portTickType xLastWakeTime;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task. */
		// vPrintString( pcTaskName );
		// vPrintString("vUARTTask checking queue.\n");

		// Get UART item off of the Queue.
		xStatus = xQueueReceive( xUARTQueue, &xReceivedRequest, portMAX_DELAY );

		if( xStatus == pdPASS )
		{
			// Write the time value for the appropriate lane and sensor:
			switch(xReceivedRequest.irID)
			{
				case DISP_1_1:
					UART3_PrintString(LANE1_1_LABEL);
					break;
				case DISP_2_1:
					UART3_PrintString(LANE2_1_LABEL);
					break;
				case DISP_1_2:
					UART3_PrintString(LANE1_2_LABEL);
					break;
				case DISP_2_2:
					UART3_PrintString(LANE2_2_LABEL);
					break;
				case DISP_1_3:
					UART3_PrintString(LANE1_3_LABEL);
					break;
				case DISP_2_3:
					UART3_PrintString(LANE2_3_LABEL);
					break;
				case DISP_1_4:
					UART3_PrintString(LANE1_4_LABEL);
					break;
				case DISP_2_4:
					UART3_PrintString(LANE2_4_LABEL);
					break;
				default:
				    __NOP;  // Do nothing.
		    }

		    snprintf(timeStr, 16, "%u\n", xReceivedRequest.tVal);
		    UART3_PrintString(timeStr);
		}
		else
		{
			/* We did not receive anything from the queue.  This must be an error
			as this task should only run when the queue is full. */
			vPrintString( "Could not receive from the UART queue.\n" );
			if (xStatus == errQUEUE_EMPTY)
			{
				vPrintString( "The queue is empty.\n");
			}
		}
	}
}
/*---------END of UART Task--------------------------------------------------*/
