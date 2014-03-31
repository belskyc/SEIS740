/*
 * SEIS-740 Project
 * NXP LPC1769 uC - External Interrupts
 *
 * Authors: Chris Belsky & Jeff Hatch
 */

#ifndef EINT1_SETUP_H_
#define EINT1_SETUP_H_

// The NXP LPC1769 uC has four (4) external interrupts.
void EINT0_Init();
void EINT1_Init();
void EINT2_Init();
void EINT3_Init();

#endif /* EINT1_SETUP_H_ */
