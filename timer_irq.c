/*
===============================================================================
 SEIS-740:
 Name        : timer_irq.c
 Author      : Chris Belsky
 Version     :
 Description : IRQ handlers.  Based on RDB sample code.
===============================================================================
*/

#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "leds.h"
#include "timer.h"
#include "irSensors.h"
//#include "SevenSegmentDecode.h"

//extern int Angle;
extern int ControllerSt;
extern int ControlCounter;
extern int NVIC_TIMER1_IRQ_State;
//extern char s[10];

extern xQueueHandle xDisplayQueue;

// TIMER0 handles the LED2 indication.
void TIMER0_IRQHandler (void)
{
	LPC_TIM0->IR = 1;	  // CLEAR interrupt flag.
	timer0_counter++;	  // Increment the external counter.

	leds_invert(GPIOP019);	// Toggle the LED state.
	leds_invert(LED2);	    // Toggle the LED state.
	return;
}

// TIMER1 handles clocking the 7-segment display.
void TIMER1_IRQHandler (void)
{
	LPC_TIM1->IR = 1;	   // CLEAR interrupt flag.
	// timer0_counter++;		  // Increment the external counter.
	timer1_runStatus = 1;  // Flag that timer has occurred.
	return;
}

void TIMER2_IRQHandler (void)
{
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  timer2_counter++;
  // ...do something...
  return;
}

void TIMER3_IRQHandler (void)
{
  LPC_TIM3->IR = 1;			/* clear interrupt flag */
  timer3_counter++;
  // ...do something...
  return;
}
