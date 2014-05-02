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

// RESET Push-Button Task.
void vSenderTask( void *pvParameters )
{
	uint8_t k = 0;
	portBASE_TYPE xStatus;
    const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

    int GPIO0state = 0;
	uint8_t buttonPressEvent = 0;
	vPrintString( "vSenderTask started\n" );

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
		    vPrintString( "Button press event detected\n" );
		    buttonPressEvent = 0; // Clear the button event.

		    // Reset the IR-sensor indexes.
		    IR_LANE1_ID = IR_ID_1_1;
		    IR_LANE2_ID = IR_ID_2_1;

		    // Reset the MUX indexes & addresses.
		    MUX_IR_index2 = 0;
		    MUX_IR_index1 = 0;
		    LPC_GPIO2->FIOCLR0 |= (0x0f); // Reset the MUX address

		    // Clear the displays
		    Display_displayNumber(11, 100000000, 0, 0);
		    Display_displayNumber(12, 100000000, 0, 0);
		    Display_displayNumber(13, 100000000, 0, 0);
		    Display_displayNumber(14, 100000000, 0, 0);
		    Display_displayNumber(21, 100000000, 0, 0);
		    Display_displayNumber(22, 100000000, 0, 0);
		    Display_displayNumber(23, 100000000, 0, 0);
		    Display_displayNumber(24, 100000000, 0, 0);

		    timer0_reference = timer0_counter; // Update the timer reference value.

		    k = readADXL345(REG_INT_SOURCE); // Clear the ADXL345 interrupt.
			// printf("EINT1: REG_INT_SOURCE = %x\n", k);
		}

		/* Allow the other sender task to execute. */
		// taskYIELD();
	}
}
/*-----------------------------------------------------------*/
