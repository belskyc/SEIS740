/*
    FreeRTOS V6.1.1 - Copyright (C) 2011 Real Time Engineers Ltd.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public 
    License and the FreeRTOS license exception along with FreeRTOS; if not it 
    can be viewed here: http://www.freertos.org/a00114.html and also obtained 
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "irSensors.h"
#include "display.h"
#include "timer.h"

/* Demo includes. */
#include "basic_io.h"

#define mainSENDER_1		1
#define mainSENDER_2		2

/* The tasks to be created.  Two instances are created of the sender task while
only a single instance is created of the receiver task. */
static void vSenderTask( void *pvParameters );
static void vReceiverTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* Declare a variable of type xQueueHandle.  This is used to store the queue
that is accessed by IR sensor interrupt handlers and the Display task */
xQueueHandle xDisplayQueue;

#if 0
/* Define the structure type that will be passed on the queue. */
typedef struct
{
	unsigned char ucValue;
	unsigned char ucSource;
} xData;

/* Declare two variables of type xData that will be passed on the queue. */
static const xData xStructsToSend[ 2 ] =
{
	{ 100, mainSENDER_1 }, /* Used by Sender1. */
	{ 200, mainSENDER_2 }  /* Used by Sender2. */
};
#endif

// Function to initialize the clock & external clock output pin.
void clkcfg_init (void)
{
	// Configuration for the LPC1769 board:
	// NOTE!  The project files from RDB1768cmsis2_LedFlash use the DEFAULT Clock configuration in the "system_LPC17xx.c" file.
	//  Thus, when it's desired to have a custom clock configuration, it's necessary to copy the "system_LPC17xx.c" file to
	//  the local project directory, and set the #define CLOCK_SETUP, #define PLL0_SETUP and #define PLL1_SETUP to value 0.

	// Setup and start the MAIN OSCILLATOR: (see p.28 of LPC17xx Users Manual.)
	LPC_SC->SCS &= ~(1 << 4);	// Ensure the OSCRANGE bit of SCS register is cleared (set freq range 1-20MHz).
	LPC_SC->SCS |= (1 << 5);    // Enable main oscillator bit 5 (OSCEN) of SCS register.
	while ((LPC_SC->SCS & (1 << 6)) == 0);  // Wait for Oscillator to be ready (bit OSCSTAT of SCS = 1 when ready).

	// AFTER the MAIN OSC is started, configure the Clock:
	LPC_SC->CLKSRCSEL &= ~(0x00000003);  // sysclk = irc_clk: Setup Internal RC Oscillator = 4Mhz.
	LPC_SC->CLKSRCSEL |=  (0x00000001);  // sysclk = osc_clk: Setup External Oscillator = 12Mhz.
	LPC_SC->CCLKCFG   &= ~(0x000000FF);  // Clock Divide = 1.
	LPC_SC->PLL0CON   &= ~(0x00000003);  // cclk = pllclk = sysclk: Disable and bypass PLL.

	// EXTERNAL CLOCK OUTPUT PIN:
	// Port P1.27 (pin-43 on LQFP100 pkg).
	// Setup Pin Connect Block for P1.27 => CLKOUT.
	LPC_PINCON->PINSEL3 &= ~(3<<22);
	LPC_PINCON->PINSEL3 |= (1<<22);
	// Select CLK type to output.
	LPC_SC->CLKOUTCFG |= 0x00000100; // Enables cclk output on P1.27
	// LPC_SC->CLKOUTCFG |= 0x00000101; // Enables osc_clk output on P1.27
	// LPC_SC->CLKOUTCFG |= 0x00000102; // Enables irc_clk output on P1.27
}

int main( void )
{

	clkcfg_init();
	leds_init();
	Display_init();
	IR_init();

    /* The queue is created to hold a maximum of 8 structures of type xDisplayQueue. */
	xDisplayQueue = xQueueCreate( 8, sizeof( dispReq ) );

	timer0_counter = 0; // Clear the counter used for the 7-segment display.
	init_timer( 0, TIMER0_INTERVAL );  // Used to drive queue
	enable_timer(0);

#if 1
	if( xDisplayQueue != NULL )
	{
		/* Create two instances of the task that will write to the queue.  The
		parameter is used to pass the structure that the task should write to the 
		queue, so one task will continuously send xStructsToSend[ 0 ] to the queue
		while the other task will continuously send xStructsToSend[ 1 ].  Both 
		tasks are created at priority 2 which is above the priority of the receiver. */
		//xTaskCreate( vSenderTask, "Sender1", 240, ( void * ) &( xStructsToSend[ 0 ] ), 2, NULL );
		//xTaskCreate( vSenderTask, "Sender2", 240, ( void * ) &( xStructsToSend[ 1 ] ), 2, NULL );

		/* Create the task that will read from the queue.  The task is created with
		priority 1, so below the priority of the sender tasks. */
		xTaskCreate( vReceiverTask, "Display", 600, NULL, 1, NULL );

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}
	else
	{
		/* The queue could not be created. */
		vPrintString( "Could NOT create the display queue.\n" );
	}
#endif

		
    /* If all is well we will never reach here as the scheduler will now be
    running the tasks.  If we do reach here then it is likely that there was 
    insufficient heap memory available for a resource to be created. */
	for( ;; );
	return 0;
}
/*-----------------------------------------------------------*/

#if 0
static void vSenderTask( void *pvParameters )
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

static void vReceiverTask( void *pvParameters )
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
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* This function will only be called if an API call to create a task, queue
	or semaphore fails because there is too little heap RAM remaining. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	/* This function will only be called if a task overflows its stack.  Note
	that stack overflow checking does slow down the context switch
	implementation. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* This example does not use the idle hook to perform any processing. */
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This example does not use the tick hook to perform any processing. */
}







