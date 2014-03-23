/*
===============================================================================
 SEIS-742: Project 1
 Name        : timer.h
 Author      : Chris Belsky
 Version     :
 Description : Timer routines.  Based on RDB sample code.
===============================================================================
*/

#ifndef __TIMER_H 
#define __TIMER_H

#include <stdint.h>

// Interrupt interval times: assumes using OSC_CLK = 12MHz = CCLK.
// PCLKSEL0 register: PCLK_TIMERn bits = '00' =>  PCLK = CCLK/4
//  Thus, PCLK = 3MHz.
#define TIMER0_INTERVAL  3000000  // 1 second
#define TIMER1_INTERVAL  60	      // 2uSec * 3MHz = 6 => 500kHz; 2mSec * 3MHz = 6000 => 500Hz

// PCONP Register: Power Control for Peripherals Register.
// Turns ON power to the PCTIMn peripherals.
#define PCTIM0_POWERON (1 << 1)
#define PCTIM1_POWERON (1 << 2)
#define PCTIM2_POWERON (1 << 22)
#define PCTIM3_POWERON (1 << 23)

#define GPIO_P0_20 (1 << 20)  // GPIO P0[20]
#define GPIO_P0_19 (1 << 19)  // GPIO P0[19]
#define GPIO_P0_18 (1 << 18)  // GPIO P0[18]
#define GPIO_P1_24 (1 << 24)  // USM0
#define GPIO_P1_25 (1 << 25)  // USM1
#define GPIO_P1_26 (1 << 26)  // STEP
#define GPIO_P1_29 (1 << 29)  // DIR
#define GPIO_P1_28 (1 << 28)  // ENABLEn

extern volatile uint32_t timer0_counter;
extern volatile uint32_t timer1_counter;
extern volatile uint32_t timer2_counter;
extern volatile uint32_t timer3_counter;

extern volatile uint32_t timer0_runStatus;
extern volatile uint32_t timer1_runStatus;
extern volatile uint32_t timer2_runStatus;
extern volatile uint32_t timer3_runStatus;

extern volatile uint32_t timer1_clkStatus;

extern void init_timer( uint8_t timer_num, uint32_t timerInterval );
extern void enable_timer( uint8_t timer_num );
extern void disable_timer( uint8_t timer_num );
extern void reset_timer( uint8_t timer_num );
// extern void StepperMotor_GPIOcfg_init (void);
extern void GPIOcfg_init (void);

#endif /* end __TIMER_H */

