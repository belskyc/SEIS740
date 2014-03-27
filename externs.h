/*
 * SEIS-740, Spring 2014, Real-Time-Systems
 * Class Project
 * Chris Belsky & Jeff Hatch
 *
 * Global Variable Declarations
*/

// FreeRTOS.org includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "ADXL345.h"
#include "ssp.h"

/* Be careful with the port number and location number, because
some of the location may not exist in that port. */
#define PORT_NUM			1
#define LOCATION_NUM		0

// FreeRTOS GLOBAL Variables:
/* Declare a variable of type xSemaphoreHandle.  This is used to reference the
semaphore that is used to synchronize a task with an interrupt. */
extern xSemaphoreHandle xCountingSemaphore;

/* Counting semaphore for handling the UART interrupt */
extern xSemaphoreHandle xUARTCountSemaphore;

/* Declare a variable of type xQueueHandle.  This is used to store the queue
that is accessed by IR sensor interrupt handlers and the Display task */
xQueueHandle xDisplayQueue;

extern char str[30];

// SSP GLOBAL Variables:
extern uint8_t src_addr[SSP_BUFSIZE];
extern uint8_t dest_addr[SSP_BUFSIZE];

// ADXL GLOBAL Variables:
extern XYZ_t xyzRaw_buffer[32];
extern XYZCon_t xyzCon_buffer[32];
extern int32_t xyzAvg[3];
extern uint8_t xyz_data[6]; // Global variable to hold XYZ data.

extern int Angle;
extern int ControllerSt;
extern int ControlCounter;
extern int NVIC_TIMER1_IRQ_State;
extern char s[10];

// UART GLOBAL Variables:
extern uint8_t RxBuf[2];
extern uint8_t UART3_LSR, UART3_IIR;
extern char RxIRQ_Fired;
