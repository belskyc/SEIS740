/*
===============================================================================
 SEIS-742: Project 1
 Name        : timer.c
 Author      : Chris Belsky
 Version     :
 Description : Timer routines.  Based on RDB sample code.
===============================================================================
*/

#include "LPC17xx.h"
#include "timer.h"

// External Timer Variables.
volatile uint32_t timer0_counter = 0;
volatile uint32_t timer1_counter = 0;
volatile uint32_t timer2_counter = 0;
volatile uint32_t timer3_counter = 0;

volatile uint32_t timer0_runStatus = 0;
volatile uint32_t timer1_runStatus = 0;
volatile uint32_t timer2_runStatus = 0;
volatile uint32_t timer3_runStatus = 0;

volatile uint32_t timer1_clkStatus = 0;

/*************************************************
 * enable_timer()
 *
 * Enables appropriate timer, based on 'timer_num'.
 * TCR: Timer Control Register.
 * bit-0: when set '1' enables the Timer Counter &
 *  Prescale Counter for counting.  When cleared '0'
 *  they are disabled.
 *************************************************/
void enable_timer( uint8_t timer_num )
{
	if ( timer_num == 0 )
	{
		timer0_runStatus = 1;
		LPC_TIM0->TCR = 1;
	}
	else if ( timer_num == 1 )
	{
		timer1_runStatus = 1;
		LPC_TIM1->TCR = 1;
	}
	else if ( timer_num == 2 )
	{
		timer2_runStatus = 1;
		LPC_TIM2->TCR = 1;
	}
	else if ( timer_num == 3 )
	{
		timer3_runStatus = 1;
		LPC_TIM3->TCR = 1;
	}
}

/**************************************************
 * disable_timer()
 *
 * Disables appropriate timer, based on 'timer_num'.
 * TCR: Timer Control Register.
 * bit-0: when set '1' enables the Timer Counter &
 *  Prescale Counter for counting.  When cleared '0'
 *  they are disabled.
 **************************************************/
void disable_timer( uint8_t timer_num )
{
	if ( timer_num == 0 )
	{
		timer0_runStatus = 0;
		LPC_TIM0->TCR = 0;
	}
	else if ( timer_num == 1 )
	{
		timer1_runStatus = 0;
		LPC_TIM1->TCR = 0;
	}
	else if ( timer_num == 2 )
	{
		timer2_runStatus = 0;
		LPC_TIM2->TCR = 0;
	}
	else if ( timer_num == 3 )
	{
		timer3_runStatus = 0;
		LPC_TIM3->TCR = 0;
	}
}

/**************************************************
 * reset_timer()
 *
 * Resets appropriate timer, based on 'timer_num'.
 * TCR: Timer Control Register.
 * bit-1: when set '1' the Timer Counter & Prescale
 *  Counters are synchronously reset on the next
 *  positive edge of PCLK.  The counters remain reset
 *  until TCR[1] is returned to value '0'.
 **************************************************/
void reset_timer( uint8_t timer_num )
{
	uint32_t regVal;

	if ( timer_num == 0 )
	{
		regVal = LPC_TIM0->TCR;
		regVal |= 0x02;
		LPC_TIM0->TCR = regVal;
	}
	else if ( timer_num == 1 )
	{
		regVal = LPC_TIM1->TCR;
		regVal |= 0x02;
		LPC_TIM1->TCR = regVal;
	}
	else if ( timer_num == 2 )
	{
		regVal = LPC_TIM2->TCR;
		regVal |= 0x02;
		LPC_TIM2->TCR = regVal;
	}
	else if ( timer_num == 3 )
	{
		regVal = LPC_TIM3->TCR;
		regVal |= 0x02;
		LPC_TIM3->TCR = regVal;
	}
}

/******************************************************
 * init_timer()
 *
 * Initializes appropriate timer, based on 'timer_num',
 * setting timer interval, resetting timer and
 * enabling timer interrupt.
 ******************************************************/
void init_timer ( uint8_t timer_num, uint32_t TimerInterval )
{
	if ( timer_num == 0 )
	{
		timer0_counter = 0;
		LPC_TIM0->MR0 = TimerInterval;  // Match value when Timer Counter TC = MR then do something.
		LPC_TIM0->MCR = 3;				// Interrupt and Reset the TC on MR0.
		NVIC_EnableIRQ(TIMER0_IRQn);	// Enable the TIM0 interrupt.
	}
	else if ( timer_num == 1 )
	{
		timer1_counter = 0;
		LPC_TIM1->MR0 = TimerInterval;
		LPC_TIM1->MCR = 3;				/* Interrupt and Reset on MR1 */
		NVIC_EnableIRQ(TIMER1_IRQn);    // TIMER1 used to control 7-segment display; Disable if Switch used to enable TIMER1.
	}
	else if ( timer_num == 2 )
	{
		timer2_counter = 0;
		LPC_TIM2->MR0 = TimerInterval;
		LPC_TIM2->MCR = 3;				/* Interrupt and Reset on MR1 */
		NVIC_EnableIRQ(TIMER2_IRQn);
	}
	else if ( timer_num == 3 )
	{
		timer3_counter = 0;
		LPC_TIM3->MR0 = TimerInterval;
		LPC_TIM3->MCR = 3;				/* Interrupt and Reset on MR1 */
		NVIC_EnableIRQ(TIMER3_IRQn);
	}
}

/******************************************************
 * GPIO Configuration for Stepper Motor Drive:
 *
 * Initializes appropriate GPIO to control the
 *  stepper motor.
 ******************************************************/
void GPIOcfg_init (void)
{
	// Configuration for the LPC1769 board:
	// int tempInt1, tempInt2;

	// Set P0_19 to value '00' - GPIO:  Used for CLOCK of 7-Segment Display.
	// LPC_PINCON->PINSEL1	&= (~0x000000C0);
	LPC_PINCON->PINSEL1	&= ~(3 << 6);
	LPC_GPIO0->FIODIR |= GPIO_P0_19;  // Set GPIO - P0_19 - to be output (= 1).

	// Set P0_20 to value '00' - GPIO:  Used for DATA of 7-Segment Display.
	// LPC_PINCON->PINSEL1	&= (~0x000000C0);
	LPC_PINCON->PINSEL1	&= ~(3 << 8);
	LPC_GPIO0->FIODIR |= GPIO_P0_20;  // Set GPIO - P0_20 - to be output (= 1).

	// Setup P0_18 as input switch:
	LPC_PINCON->PINSEL1	&= ~(3 << 4);
	LPC_GPIO0->FIODIR &= ~GPIO_P0_18;  // Set GPIO - P0_18 - to be input (= 0).
	// tempInt1 = LPC_PINCON->PINSEL1;
	// printf("LPC_PINCON->PINSEL1 = %d\n", tempInt1);

	/*
	// Set P_24 to value '00' - GPIO:  Used for USM0 of DRV8811EVM.
	LPC_PINCON->PINSEL3	&= ~(3 << 16);
	LPC_GPIO1->FIODIR |= GPIO_P1_24;  // Set GPIO - P1_24 - to be output (= 1).
	*/

	// Set P_25 to value '00' - GPIO:  Used for DATA of 7-Segment Display. (BROWN)
	LPC_PINCON->PINSEL3	&= ~(3 << 18);
	LPC_GPIO1->FIODIR |= GPIO_P1_25;  // Set GPIO - P1_25 - to be output (= 1).

	// Set P_26 to value '00' - GPIO:  Used for CLOCK of 7-Segment Display. (RED)
	LPC_PINCON->PINSEL3	&= ~(3 << 20);
	LPC_GPIO1->FIODIR |= GPIO_P1_26;  // Set GPIO - P1_26 - to be output (= 1).


	// Set P_29 to value '00' - GPIO:  Used for ENABLE of 7-Segment Display. (BLACK)
	LPC_PINCON->PINSEL3	&= ~(3 << 26);
	LPC_GPIO1->FIODIR |= GPIO_P1_29;  // Set GPIO - P1_29 - to be output (= 1).

	/*
	// Set P_28 to value '00' - GPIO:  Used for ENABLEn of DRV8811EVM.
	LPC_PINCON->PINSEL3	&= ~(3 << 24);
	LPC_GPIO1->FIODIR |= GPIO_P1_28;  // Set GPIO - P1_28 - to be output (= 1).
	*/
	__NOP;
}

