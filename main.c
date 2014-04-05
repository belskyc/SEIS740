/*
 * SEIS-740, Spring 2014, Real-Time-Systems
 * Class Project
 * Chris Belsky & Jeff Hatch *
*/

#include "stdio.h"

// FreeRTOS.org includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
// FreeRTOS Demo includes.
#include "basic_io.h"

// Project Includes
#include "system_LPC17xx.h"
#include "LPC17xx.h"			/* LPC17xx Peripheral Registers */
#include "irSensors.h"
#include "display.h"
#include "timer.h"
#include "EINT_Setup.h"
#include "ssp.h"
#include "ADXL345_init.h"
#include "ADXL345.h"
#include "externs.h"
#include "uart.h"
#include "vTasks.h"
#include "leds.h"


#define mainSENDER_1		1
#define mainSENDER_2		2

// --- GLOBAL Variable DEFINITIONS: --- //
uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];

XYZ_t xyzRaw_buffer[32];
XYZCon_t xyzCon_buffer[32];
int32_t xyzAvg[3];
uint8_t xyz_data[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66}; // Global variable to hold XYZ data.

int Angle = 0;
int ControllerSt = 0;
int ControlCounter = 0;
int NVIC_TIMER1_IRQ_State = 0;
char s[10];

/* Declare a variable of type xSemaphoreHandle.  This is used to reference the
semaphore that is used to synchronize a task with an interrupt. */
xSemaphoreHandle xCountingSemaphore;

/* Counting semaphore for handling the UART interrupt */
xSemaphoreHandle xUARTCountSemaphore;

// MUTEX semaphore for handling the ADXL345 "Active" interrupt.
xSemaphoreHandle xADXLActiveSemaphore;

/* Declare a variable of type xQueueHandle.  This is used to store the queue
that is accessed by IR sensor interrupt handlers and the Display task */
xQueueHandle xDisplayQueue;

/* Array of display requests for each IR handler.  This is used to
 * pass from the IR sensor interrupt handler into the queue and then
 * is pulled off of the queue in the receiver task
 */
dispReq DisplayRequests[] =
{
		{ IR_ID_1_1, 0 },
		{ IR_ID_1_2, 0 },
		{ IR_ID_1_3, 0 },
		{ IR_ID_1_4, 0 },
		{ IR_ID_2_1, 0 },
		{ IR_ID_2_2, 0 },
		{ IR_ID_2_3, 0 },
		{ IR_ID_2_4, 0 },
};

/*-----------------------------------------------------------*/

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

static void setupHardware(void)
{
	// Warning: If you do not initialize the hardware clock, the timings will be inaccurate
	vPrintString("setupHardware\n");

	/* SystemClockUpdate() updates the SystemFrequency variable */
	SystemCoreClockUpdate();

	/* Initialized SSP1 for SPI to talk to ADXL345 */
	SSP1InitADXL345();

	/* Initialize the External-Interrupt EINT GPIO pins for interrupt */
	EINT0_Init();
	EINT1_Init();
	EINT2_Init();
	EINT3_Init();

	/* Intialize the accel */
	initialize_accel();
}

int main( void )
{
	// Initialization routines.
	clkcfg_init();
	leds_init();
	Display_init();
	IR_init();

    /* The queue is created to hold a maximum of 8 structures of type xDisplayQueue. */
	xDisplayQueue = xQueueCreate( 8, sizeof( dispReq ) );

	timer0_counter = 0; // Clear the counter used for the 7-segment display.
	init_timer( 0, TIMER0_INTERVAL );  // Used to drive queue
	enable_timer(0);

	timer1_counter = 0; // Clear the counter used for the 7-segment display.
	init_timer( 1, TIMER1_INTERVAL );  // Used to drive queue
	enable_timer(1);

	// Initialize UART3:
	RxIRQ_Fired = 0;
	UART3_Init(57600); // Setup UART3 to 57600 baud
	UART3_PrintString ("SEIS-740 Project:\r");  // Send string over UART3.
	UART3_PrintString ("UART3 waiting.  Input 1st CHAR command = ");  // Send string over UART3.

	/* Before a semaphore is used it must be explicitly created.  In this example
	a counting semaphore is created.  The semaphore is created to have a maximum
	count value of 10, and an initial count value of 0. */
	xCountingSemaphore = xSemaphoreCreateCounting( 10, 0 );
	/* Check the semaphore was created successfully. */
	if( xCountingSemaphore == NULL ) { printf("FAILED: could not create xCountingSemaphore!\n"); }

	xUARTCountSemaphore = xSemaphoreCreateCounting( 10, 0 );
	/* Check the semaphore was created successfully. */
	if( xCountingSemaphore == NULL ) { printf("FAILED: could not create xUARTCountSemaphore!\n"); }

	vSemaphoreCreateBinary(xADXLActiveSemaphore);
	/* Check the semaphore was created successfully. */
	if( xCountingSemaphore == NULL ) { printf("FAILED: could not create xADXLActiveSemaphore!\n"); }

#if 1
	if( xDisplayQueue != NULL )
	{
		/* Create two instances of the task that will write to the queue.  The
		parameter is used to pass the structure that the task should write to the
		queue, so one task will continuously send xStructsToSend[ 0 ] to the queue
		while the other task will continuously send xStructsToSend[ 1 ].  Both
		tasks are created at priority 2 which is above the priority of the . */
		//xTaskCreate( vSenderTask, "Sender1", 240, ( void * ) &( xStructsToSend[ 0 ] ), 2, NULL );
		//xTaskCreate( vSenderTask, "Sender2", 240, ( void * ) &( xStructsToSend[ 1 ] ), 2, NULL );
		xTaskCreate( vSenderTask, "Sender1", 240, NULL, 1, NULL );

		/* Create the task that will read from the queue.  The task is created with
		priority 1, so below the priority of the sender tasks. */

		xTaskCreate( vDisplayTask, "Display", 512, NULL, 1, NULL );

		/* Create the ADXL task. */
#if 0
		xTaskCreate( vADXLTask,		/* Pointer to the function that implements the task. */
					 "ADXL Task",	/* Text name for the task.  This is to facilitate debugging only. */
					 1024,			/* Stack depth in words. */
					 NULL,			/* We are not using the task parameter. */
					 1,				/* This task will run at priority 1. */
					 NULL );		/* We are not using the task handle. */
#endif

		/* Create the UART task */
		xTaskCreate( vUARTTask, "UART Task", 512, NULL, 1, NULL );

		/* Create the ADXL345 "Active" interrupt task */
		xTaskCreate( vADXLActiveTask, "ADXL Active Task", 512, NULL, 1, NULL );

		// Disable the External interrupts to the ADXL while setting up HW.
		NVIC_DisableIRQ( EINT0_IRQn );
		NVIC_DisableIRQ( EINT1_IRQn );
		NVIC_DisableIRQ( EINT2_IRQn );
		NVIC_DisableIRQ( EINT3_IRQn );
		setupHardware();  // Write all registers to to ADXL345 to setup its configuration.
		// Only enable the external interrupts after HW has been configured.
		NVIC_EnableIRQ( EINT0_IRQn );
		NVIC_EnableIRQ( EINT1_IRQn );
		NVIC_EnableIRQ( EINT2_IRQn );
		// NVIC_EnableIRQ( EINT3_IRQn );

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
/*------END of main()-----------------------------------------------------*/


void vApplicationMallocFailedHook( void )
{
	/* This function will only be called if an API call to create a task, queue
	or semaphore fails because there is too little heap RAM remaining. */
	for( ;; )
		printf("FAILED: fell into vApplicationMallocFailedHook()!\n");
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	/* This function will only be called if a task overflows its stack.  Note
	that stack overflow checking does slow down the context switch
	implementation. */
	for( ;; )
		printf("FAILED: fell into vApplicationStackOverflowHook()!\n");
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







