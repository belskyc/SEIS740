/*
 * irSensors.c
 *
 *  Created on: Mar 16, 2014
 *      Author: Jeff
 */

#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "irSensors.h"

extern xQueueHandle xDisplayQueue;
extern dispReq DisplayRequests[];
extern uint8_t IR_IRQ_errors[];

void
IR_IRQ_handler_1_1()
{
	portBASE_TYPE nQStatus;
    // copy the timer value
	DisplayRequests[IR_ID_1_1].tVal = LPC_TIM0->TC;
	// Enqueue the Request
	nQStatus = xQueueSendToBackFromISR(xDisplayQueue, &DisplayRequests[IR_ID_1_1], 0);
	if (nQStatus != pdPASS)
	{
		IR_IRQ_errors[IR_ID_1_1] = 1;
	}
}

void
IR_IRQ_handler_1_2()
{
	portBASE_TYPE nQStatus;
    // copy the timer value
	DisplayRequests[IR_ID_1_2].tVal = LPC_TIM0->TC;
	// Enqueue the Request
	nQStatus = xQueueSendToBackFromISR(xDisplayQueue, &DisplayRequests[IR_ID_1_2], 0);
	if (nQStatus != pdPASS)
	{
		IR_IRQ_errors[IR_ID_1_2] = 1;
	}
}

void
IR_IRQ_handler_1_3()
{
	portBASE_TYPE nQStatus;
    // copy the timer value
	DisplayRequests[IR_ID_1_3].tVal = LPC_TIM0->TC;
	// Enqueue the Request
	nQStatus = xQueueSendToBackFromISR(xDisplayQueue, &DisplayRequests[IR_ID_1_3], 0);
	if (nQStatus != pdPASS)
	{
		IR_IRQ_errors[IR_ID_1_3] = 1;
	}
}

void
IR_IRQ_handler_1_4()
{
	portBASE_TYPE nQStatus;
    // copy the timer value
	DisplayRequests[IR_ID_1_4].tVal = LPC_TIM0->TC;
	// Enqueue the Request
	nQStatus = xQueueSendToBackFromISR(xDisplayQueue, &DisplayRequests[IR_ID_1_4], 0);
	if (nQStatus != pdPASS)
	{
		IR_IRQ_errors[IR_ID_1_4] = 1;
	}
}

void
IR_IRQ_handler_2_1()
{
	portBASE_TYPE nQStatus;
    // copy the timer value
	DisplayRequests[IR_ID_2_1].tVal = LPC_TIM0->TC;
	// Enqueue the Request
	nQStatus = xQueueSendToBackFromISR(xDisplayQueue, &DisplayRequests[IR_ID_2_1], 0);
	if (nQStatus != pdPASS)
	{
		IR_IRQ_errors[IR_ID_2_1] = 1;
	}
}

void
IR_IRQ_handler_2_2()
{
	portBASE_TYPE nQStatus;
    // copy the timer value
	DisplayRequests[IR_ID_2_2].tVal = LPC_TIM0->TC;
	// Enqueue the Request
	nQStatus = xQueueSendToBackFromISR(xDisplayQueue, &DisplayRequests[IR_ID_2_2], 0);
	if (nQStatus != pdPASS)
	{
		IR_IRQ_errors[IR_ID_2_2] = 1;
	}
}

void
IR_IRQ_handler_2_3()
{
	portBASE_TYPE nQStatus;
    // copy the timer value
	DisplayRequests[IR_ID_2_3].tVal = LPC_TIM0->TC;
	// Enqueue the Request
	nQStatus = xQueueSendToBackFromISR(xDisplayQueue, &DisplayRequests[IR_ID_2_3], 0);
	if (nQStatus != pdPASS)
	{
		IR_IRQ_errors[IR_ID_2_3] = 1;
	}
}

void
IR_IRQ_handler_2_4()
{
	portBASE_TYPE nQStatus;
    // copy the timer value
	DisplayRequests[IR_ID_2_4].tVal = LPC_TIM0->TC;
	// Enqueue the Request
	nQStatus = xQueueSendToBackFromISR(xDisplayQueue, &DisplayRequests[IR_ID_2_4], 0);
	if (nQStatus != pdPASS)
	{
		IR_IRQ_errors[IR_ID_2_4] = 1;
	}
}

/*
 * Initialize all GPIO for IR sensors
 */
int
IR_init()
{

	// Multiplexer GPIO
	// GPIO on P2.0 and P2.1
	LPC_PINCON->PINSEL2 &= (~0x0000000f);
	// GPIO on P2.0 and P2.1 as output
	LPC_GPIO2->FIODIR0 |= (0x03);
    // Set P2.0 and P2.1 to output 0
    LPC_GPIO2->FIOCLR0 |= (0x03);

    return 0;
}
