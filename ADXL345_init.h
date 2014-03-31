/*
 * ADXL345_init.h
 *
 *  Created on: Mar 9, 2013
 *      Author: jhatch
 */

#include "LPC17xx.h"

#ifndef ADXL345_INIT_H_
#define ADXL345_INIT_H_

/* ***********************************************
 * Register Map
 * ***********************************************/

 #define REG_THRESH_TAP		0x1D	// Tap Threshold register
 #define REG_OFSX       	0x1E	// X-axis offset
 #define REG_OFSY			0x1F	// Y-axis offset
 #define REG_OFSZ			0x20	// Z-axis offset
 #define REG_DUR			0x21	// Tap Duration
 #define REG_LATENT			0x22	// Tap Latency
 #define REG_WINDOW			0x23	// Tap Window
 #define REG_THRESH_ACT		0x24	// Activity Threshold
 #define REG_THRESH_INACT	0x25	// Inactivity Threshold
 #define REG_TIME_INACT		0x26	// Inactivity Time
 #define REG_ACT_INACT_CTL	0x27	// Axis enable control for activity and inactivity detection
 #define REG_THRESH_FF		0x28	// Free-Fall threshold
 #define REG_TIME_FF		0x29	// Free-Fall time
 #define REG_TAP_AXES		0x2A	// Axis control for single tap/double tap
 #define REG_ACT_TAP_STATUS	0x2B	// Source of single tap/double tap. Read-Only
 #define REG_BW_RATE		0x2C	// Data Rate and power mode control
 #define REG_POWER_CTL		0x2D	// Power-saving features control
 #define REG_INT_ENABLE		0x2E	// Interrupt enable control
 #define REG_INT_MAP		0x2F	// Interrupt mapping control
 #define REG_INT_SOURCE		0x30	// Source of interrupts.  Read-Only
 #define REG_DATA_FORMAT	0x31	// Data format control
 #define REG_DATAX0			0x32	// X-Axis Data 0.
 #define REG_DATAX1			0x33	// X-Axis Data 1.
 #define REG_DATAY0			0x34	// Y-Axis Data 0.
 #define REG_DATAY1			0x35	// Y-Axis Data 1.
 #define REG_DATAZ0			0x36	// Z-Axis Data 0.
 #define REG_DATAZ1			0x37	// Z-Axis Data 1.
 #define REG_FIFO_CTL		0x38	// FIFO control
 #define REG_FIFO_STATUS	0x39	// FIFO status
// #define REG_TAP_SIGN		0x3A	// Sign and source for single tap/double tap

/* ***********************************************
 * Register Initialization Values
 * ***********************************************/

 #define INIT_THRESH_TAP	0xFF	// Single/double tap disabled
 #define INIT_OFSX       	0x00	// X-axis offset TBD
 #define INIT_OFSY			0x00	// Y-axis offset TBD
 #define INIT_OFSZ			0x00	// Z-axis offset TBD
 //#define INIT_DUR			0x00	// Tap Duration (disabled)
 //#define INIT_LATENT		0x00	// Tap Latency (disabled)
 //#define INIT_WINDOW		0x00	// Tap Window (disabled)
 #define INIT_DUR			0x64	// Tap Duration (enabled)
 #define INIT_LATENT		0x64	// Tap Latency (enabled)
 #define INIT_WINDOW		0x64	// Tap Window (enabled)

 //#define INIT_THRESH_ACT	0x03	// Activity Threshold ~.18g
 //#define INIT_THRESH_ACT	0x7F	// Activity Threshold - N/A
 //#define INIT_THRESH_ACT	0x01	// Activity Threshold 62.5mg
 //#define INIT_THRESH_ACT	0x10	// Activity Threshold set to 1g
 //#define INIT_THRESH_ACT	0x04	// Activity Threshold set to 0.25g
 #define INIT_THRESH_ACT	0x40	// Activity Threshold 4g
 //#define INIT_THRESH_ACT	0xFF	// Activity Threshold 16g

 #define INIT_THRESH_INACT	0x7F	// Inactivity Threshold - N/A
 #define INIT_TIME_INACT	0x01	// Inactivity Time 1s

 // #define INIT_ACT_INACT_CTL	0x00	// Axis enable control for activity and inactivity detection
 #define INIT_ACT_INACT_CTL	0x60	// Activity is DC-coupled; Enabled ACT_X & ACT_Y axis
 //#define INIT_ACT_INACT_CTL	0xE0	// Activity is AC-coupled; Enabled ACT_X & ACT_Y axis

 #define INIT_THRESH_FF		0x05	// Free-Fall threshold - desireable behavior
 #define INIT_TIME_FF		0x46	// Free-Fall time - desireable behavior
 #define INIT_TAP_AXES		0x00	// Axis control for single tap/double tap disabled
 #define INIT_ACT_TAP_STATUS 0x00	// Source of single tap/double tap. Read-Only
 #define INIT_BW_RATE		0x09	// Data Rate and power mode control 50Hz - sample rate 25Hz
 #define CONFIG_POWER_CTL	0x00	// Power-saving features control
 #define INIT_POWER_CTL		0x08	// Power-saving features control - init last?

 //#define INIT_INT_ENABLE	0x83	// Interrupt enable control DataReady, Watermark, Overrun
 //#define INIT_INT_ENABLE	0x02	// Interrupt enable Watermark
 //#define INIT_INT_ENABLE	0x12	// Interrupt enable Watermark, Activity
 #define INIT_INT_ENABLE	0x10	// Interrupt enable Activity

 #define INIT_INT_MAP		0x02	// Interrupt mapping Watermark -> interrupt pin INT2; all others -> pin INT1
 //#define INIT_INT_SOURCE	0x00	// Source of interrupts.  Read-Only
 #define INIT_INT_SOURCE	0x00	// Source of interrupts.  Read-Only
 //#define INIT_DATA_FORMAT	0x0C	// Data format control, SPI, Active HIGH, Full Res, Justify, +/- 2g
 #define INIT_DATA_FORMAT	0x2C	// Data format control, SPI, Active LOW, Full Res, Justify, +/- 2g
 #define INIT_DATAX0		0x00	// X-Axis Data 0.  Read-Only
 #define INIT_DATAX1		0x00	// X-Axis Data 1.  Read-Only
 #define INIT_DATAY0		0x00	// Y-Axis Data 0.  Read-Only
 #define INIT_DATAY1		0x00	// Y-Axis Data 1.  Read-Only
 #define INIT_DATAZ0		0x00	// Z-Axis Data 0.  Read-Only
 #define INIT_DATAZ1		0x00	// Z-Axis Data 1.  Read-Only
 //#define INIT_FIFO_CTL		0x90	// FIFO control.  Watermark at 16; 0x9F => Watermark at 32
 #define INIT_FIFO_CTL		0x88	// FIFO control. Stream mode & 8 samples watermark
 //#define INIT_FIFO_CTL		0x9F	// FIFO control. Stream mode & 31 samples watermark
 #define INIT_FIFO_STATUS	0x00	// FIFO status  Read-Only
 #define INIT_TAP_SIGN		0x00	// Sign and source for single tap/double tap Read-Only??

typedef struct adxl_init
{
	uint8_t	addr;
	uint8_t val;
} adxl_init_t;

/*
static adxl_init_t init_vals[] =
{
		{ REG_THRESH_TAP, INIT_THRESH_TAP },
		{ REG_OFSX, INIT_OFSX },
		{ REG_OFSY, INIT_OFSY },
		{ REG_OFSZ, INIT_OFSZ },
		{ REG_DUR, INIT_DUR },
		{ REG_LATENT, INIT_LATENT },
		{ REG_WINDOW, INIT_WINDOW },
		{ REG_THRESH_ACT, INIT_THRESH_ACT },
		{ REG_THRESH_INACT, INIT_THRESH_INACT },
		{ REG_ACT_INACT_CTL, INIT_ACT_INACT_CTL },
		{ REG_THRESH_FF, INIT_THRESH_FF },
		{ REG_TIME_FF, INIT_TIME_FF },
		{ REG_TAP_AXES, INIT_TAP_AXES },
		{ REG_BW_RATE, INIT_BW_RATE },
		{ REG_POWER_CTL, INIT_POWER_CTL },
		{ REG_INT_ENABLE, INIT_INT_ENABLE },
		{ REG_INT_MAP, INIT_INT_MAP },
		{ REG_DATA_FORMAT, INIT_DATA_FORMAT },
		{ REG_FIFO_CTL, INIT_FIFO_CTL }
};
*/

#endif /* ADXL345_INIT_H_ */
