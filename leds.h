/*
===============================================================================
 SEIS-742: Project 1
 Name        : leds.h
 Author      : Chris Belsky
 Version     :
 Description : LED2 (on NGX base board) routines.  Based on RDB sample code.
===============================================================================
*/

#ifndef LEDS_H_
#define LEDS_H_

// For the LPC1769 board: made the following modifications:
#define LED2 (1 << 22)  // LED2 is P0[22]
#define GPIOP019 (1 << 19) // P0[19]

// Set of defines for RDB1768 board user LEDs:
// Led 2 is P1_24
#define LED_2 (1 << 24)
// Led 3 is P1_25
#define LED_3 (1 << 25)
// Led 4 is P1_28
#define LED_4 (1 << 28)
// Led 5 is P1_29
#define LED_5 (1 << 29)
// All Leds
#define LED_ALL (LED_2 | LED_3 | LED_4 | LED_5)

extern void leds_init (void);
extern void leds_invert (unsigned int whichled);
extern void led_on (unsigned int ledstate);
extern void led_off (unsigned int ledstate);

#endif /*LEDS_H_*/
