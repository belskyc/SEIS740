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
#include "irSensors.h"
#include "timer.h"
#include "display.h"
#include "ADXL345_init.h"

extern uint8_t MUX_IR_index1;
extern uint8_t MUX_IR_index2;

#if 1
void vSenderTask( void *pvParameters )
{
	uint8_t k = 0;
	portBASE_TYPE xStatus;
    const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

    int GPIO0state = 0;
	uint8_t buttonPressEvent = 0;
	vPrintString( "vSederTask started\n" );

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{

		// Check if the SW6-button on NGX board is pressed or not.
		GPIO0state = LPC_GPIO0->FIOPIN;
		if(!(GPIO0state & GPIO_P0_18)) // SW6-Button on NGX board is pressed.
		{
			buttonPressEvent = 1; // SET Flag that the SW6-button was pressed.
		}
		else // SW6-Button on NGX board is NOT pressed.
		{
			buttonPressEvent = 0; // CLEAR Flag that the SW6-button was pressed.
		}

		if(buttonPressEvent)
		{

	        //short_delay(300);

		    // copy the timer value
#if 0
	        uint32_t tval = timer0_counter;
			DisplayRequests[IR_IDX_1_1].tVal = timer0_counter;
			vPrintString( "Button press event detected\n" );
			vPrintStringAndNumber("Timer Value Captured = ", DisplayRequests[IR_IDX_1_1].tVal);

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

			xStatus = xQueueSendToBack(xDisplayQueue, &DisplayRequests[IR_IDX_1_1], 0);
		    //xStatus = xQueueSendToBack( xDisplayQueue, pvParameters, xTicksToWait );

		    if( xStatus != pdPASS )
		    {
			    /* We could not write to the queue because it was full - this must
			    be an error as the receiving task should make space in the queue
			    as soon as both sending tasks are in the Blocked state. */
			    vPrintString( "Could not send to the queue.\n" );
		    }
#endif

		    vPrintString( "Button press event detected\n" );
		    buttonPressEvent = 0;
		    MUX_IR_index2 = 0;
		    MUX_IR_index1 = 0;
		    LPC_GPIO2->FIOCLR0 |= (0x0f);
		    timer0_reference = timer0_counter;
		    k = readADXL345(REG_INT_SOURCE);
			// printf("EINT1: REG_INT_SOURCE = %x\n", k);

		    // Clear the displays
		    Display_displayNumber(11, 100000000, 0, 0);
		    Display_displayNumber(12, 100000000, 0, 0);
		    Display_displayNumber(13, 100000000, 0, 0);
		    Display_displayNumber(14, 100000000, 0, 0);
		    Display_displayNumber(21, 100000000, 0, 0);
		    Display_displayNumber(22, 100000000, 0, 0);
		    Display_displayNumber(23, 100000000, 0, 0);
		    Display_displayNumber(24, 100000000, 0, 0);

		}

		/* Allow the other sender task to execute. */
		taskYIELD();
	}
}
/*-----------------------------------------------------------*/
#endif
