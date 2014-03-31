/*
 * SEIS-740, Spring 2014, Real-Time-Systems
 * Class Project
 * Chris Belsky & Jeff Hatch
 *
 * NXP LPC1769 uC running FreeRTOS - ADXL Task
*/

#include "stdlib.h"

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



void vADXLTask( void *pvParameters )
{
    const char *pcTaskName = "ADXL Task is running\n";
    volatile unsigned long ul;
    uint32_t i;
    uint32_t portnum = PORT_NUM;
    uint8_t Dummy = 0;
    XYZ_t xyz;

    /* Print out the name of this task. */
    vPrintString( pcTaskName );

   	// printf("Done initializing accel\n");

    while(1)
    {
		/* Use the semaphore to wait for the event.  The semaphore was created
		before the scheduler was started so before this task ran for the first
		time.  The task blocks indefinitely meaning this function call will only
		return once the semaphore has been successfully obtained - so there is no
		need to check the returned value. */
		xSemaphoreTake( xCountingSemaphore, portMAX_DELAY );

	   	// Clear the Rx FIFO
	   	for (i = 0; i < 8; i++)
	   		Dummy = LPC_SSP1->DR;

		for (i = 0; i < 32; i++)
		{
			readADXL345XYZ(xyzRaw_buffer[i].xyz);
		}

		ConvertRawXYZ(xyzRaw_buffer, xyzCon_buffer);

		CalcMAvg(xyzCon_buffer, xyzAvg, 4);

		//printf("xyzAvg[0] = %d, xyzAvg[1] = %d\n", xyzAvg[0], xyzAvg[1]);
		int neg = 0;
#if 1
		if (xyzAvg[0] < 1)
		{
			//printf("Angle = %d\n", (-1 * GetAngle((-1 * xyzAvg[0]), xyzAvg[1])));
			Angle = (-1 * GetAngle((-1 * xyzAvg[0]), xyzAvg[1]));
			itoa(Angle, s, 10);
			UART3_PrintString ("Angle = ");  // Send string over UART3.
			UART3_PrintString (s);  // Send string over UART3.
			UART3_PrintString ("\r");  // Send string over UART3.
		}
		else
		{
			//printf("Angle = %d\n", GetAngle(xyzAvg[0], xyzAvg[1]));
			Angle = GetAngle(xyzAvg[0], xyzAvg[1]);
			itoa(Angle, s, 10);
			UART3_PrintString ("Angle = ");  // Send string over UART3.
			UART3_PrintString (s);  // Send string over UART3.
			UART3_PrintString ("\r");  // Send string over UART3.
		}
#endif

    } // END main while 1
}
/*--------END of ADXL Task---------------------------------------------------*/
