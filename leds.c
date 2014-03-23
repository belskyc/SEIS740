/*
===============================================================================
 SEIS-742: Project 1
 Name        : leds.c
 Author      : Chris Belsky
 Version     :
 Description : LED2 (on NGX base board) routines.  Based on RDB sample code.
===============================================================================
*/

#include "LPC17xx.h"
#include "leds.h"
#include "stdio.h"

// Function to initialise GPIO to access the user LEDs
void leds_init (void)
{
	// Configuration for the LPC1769 board:
	// Port-0
	//  upper half is controlled by register PINSEL1.
	// Set P0_22 to 00 - GPIO
	LPC_PINCON->PINSEL1	&= (~0x00003000);
	//  Port-0 upper half
	// Set GPIO - P0_22 - to be outputs (= 1).
	LPC_GPIO0->FIODIR |= LED2;
}

// Function to invert current state of leds
void leds_invert (unsigned int whichleds)
{
	int ledstate;

	// Configuration for the LPC1769 board:
	// Read current state of GPIO P0_16..31, which includes LEDs
	ledstate = LPC_GPIO0->FIOPIN;
	// Turn off LEDs that are on
	// (ANDing to ensure we only affect the LED outputs)
	LPC_GPIO0->FIOCLR = ledstate & whichleds;
	// Turn on LEDs that are off
	// (ANDing to ensure we only affect the LED outputs)
	LPC_GPIO0->FIOSET = ((~ledstate) & whichleds);
}

// Function to turn on chosen led(s)
void led_on (unsigned int ledstate)
{
	// Configuration for the LPC1769 board:
	// Port-0
	LPC_GPIO0->FIOSET |= ledstate;
}

// Function to turn off chosen led(s)
void led_off (unsigned int ledstate)
{
	// Configuration for the LPC1769 board:
	// Port-0
	LPC_GPIO0->FIOCLR &= ~ledstate;
}


