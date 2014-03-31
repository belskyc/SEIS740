/*
 * ADXL345_init.c
 *
 *  Created on: Mar 9, 2013
 *      Author: jhatch
 */

#include "stdio.h"

#include "LPC17xx.h"
#include "ssp.h"
#include "ADXL345_init.h"
#include "ADXL345.h"

// Negative bit (0x8000 for left justified, 0x0200 for right)
#define NEGATIVITY_BIT 0x8000

// Convert the raw DATA*0 and DATA*1 registers into a 16
// bit value
void
ConvertRawXYZ(XYZ_t *XYZRaw, XYZCon_t *XYZCon)
{
	int i = 0;

	// Convert to 16 bit
	for (i = 0; i < 32; i++)
	{
		// X
		XYZCon[i].xyz[0] = ((uint16_t)XYZRaw[i].xyz[1] << 8) | XYZRaw[i].xyz[0];
		XYZCon[i].xyz[0] &= 0xFFC0;  // change this if left justified to 0xFFC0, 0x03FF if right
		// Y
		XYZCon[i].xyz[1] = ((uint16_t)XYZRaw[i].xyz[3] << 8) | XYZRaw[i].xyz[2];
		XYZCon[i].xyz[1] &= 0xFFC0;  // change this if left justified to 0xFFC0, 0x03FF if right
	}
}

// NOTE: This is for left justified only
int
convert_negative( uint16_t accel_val)
{
	int outval = 0;
	int neg = 1;
	uint16_t temp = accel_val;

    if (temp & NEGATIVITY_BIT)
    {
    	neg = -1;
    	temp = ~(accel_val);
    	temp = temp >> 6;  // Drop the 6 unused bits
    	temp += 1;
    }
    else
    {
    	temp = temp >> 6;
    }

    outval = temp;
    outval = outval * neg;

	return outval;
}

void
CalcMAvg(XYZCon_t *XYZCon, int32_t output[], int samples)
{
	uint8_t sample_interval = 32 / samples;
	int i = 0; int xSum = 0; int ySum = 0;

	for (i = 0; i < 32; i += sample_interval)
	{
		xSum += convert_negative(XYZCon[i].xyz[0]);
		ySum += convert_negative(XYZCon[i].xyz[1]);
	}

	//printf("xSum = %d, ySum = %d\n", xSum, ySum);
	output[0] = xSum / samples;
	output[1] = ySum / samples;
}

void
writeADXL345(uint8_t reg, uint8_t val)
{
    uint8_t write_buf[2];
    // uint8_t read_buf[1];
    int i = 0;

    write_buf[0] = reg;
    write_buf[1] = val;

    //printf("writeADXL345 sending: 0x%x 0x%x\n", write_buf[0], write_buf[1]);

    SSP_SSELToggle( 1, 0 );

    for ( i = 0; i < 0x100; i++ );	/* delay */
    SSPWriteSingleByte(1, write_buf, 2); // Write both reg & data bytes.

    for ( i = 0; i < 0x100; i++ );	/* delay */

    SSP_SSELToggle( 1, 1 );
}

uint8_t
readADXL345(uint8_t reg)
{
	uint8_t tmp_reg = reg;
	uint8_t write_buf[1];
	uint8_t read_buf[1];
	int i = 0;
	uint8_t dummy = 0;

	tmp_reg |= 0x80;		     // set the read bit
	write_buf[0] = tmp_reg;
	write_buf[1] = 0x55;
	read_buf[0] = 0xAA; // Start with known value for DEBUG.

	for (i = 0; i < FIFOSIZE; i++)
		dummy = LPC_SSP1->DR;

    SSP_SSELToggle( 1, 0 );

    for ( i = 0; i < 0x100; i++ );	/* delay */

    SSPReceiveSingleByte(1, write_buf, read_buf);   // Read the data byte of ADXL345.

	for ( i = 0; i < 0x100; i++ );	/* delay */

	SSP_SSELToggle( 1, 1 );

	return(read_buf[0]);
}

void
readADXL345XYZ( uint8_t *xyz_data)
{
	int i = 0;

    SSP_SSELToggle( 1, 0 );
    for ( i = 0; i < 0x100; i++ );	/* delay */

    SSPMultByteRead(1, xyz_data);

	for ( i = 0; i < 0x100; i++ );  /* delay */

	SSP_SSELToggle( 1, 1 );
}

// Initialize the accelerometer and start the sampling
void
initialize_accel()
{
	uint8_t k = 0;

	/*
	 * 	As a general rule, configure the digital accelerometers in the following order:
	 * 	1. Set data parameters such as data rate, measurement range, data format, and offset adjustment.
	 * 	2. Configure interrupts (do not enable): thresholds and timing values, and map interrupts to pins.
	 * 	3. Configure FIFO (if in use): mode, trigger interrupt if using trigger mode, and samples bits.
	 * 	4. Enable interrupts: INT_ENABLE register.
	 * 	5. Place part into measurement mode: POWER_CTL register.
	 */

	// Go into standby mode to configure the device.
	writeADXL345(REG_POWER_CTL, CONFIG_POWER_CTL);  // = 0x00

	// Data format control: set first to setup 4-wire SPI.
	writeADXL345(REG_DATA_FORMAT, INIT_DATA_FORMAT);
	// Data Rate and power mode control
	writeADXL345(REG_BW_RATE, INIT_BW_RATE);
	// X-axis offset
	writeADXL345(REG_OFSX, INIT_OFSX);
	// Y-axis offset
	writeADXL345(REG_OFSY, INIT_OFSY);
	// Z-axis offset
	writeADXL345(REG_OFSZ, INIT_OFSZ);

	// Activity Threshold
	writeADXL345(REG_THRESH_ACT, INIT_THRESH_ACT);
	// Axis enable control for activity and inactivity detection
	writeADXL345(REG_ACT_INACT_CTL, INIT_ACT_INACT_CTL);
	// Inactivity Threshold
	writeADXL345(REG_THRESH_INACT, INIT_THRESH_INACT);
	// Inactivity Time
	writeADXL345(REG_TIME_INACT, INIT_TIME_INACT);

	// Tap Threshold register
	writeADXL345(REG_THRESH_TAP, INIT_THRESH_TAP);
	// Tap Duration
	writeADXL345(REG_DUR, INIT_DUR);
	// Tap Latency
	writeADXL345(REG_LATENT, INIT_LATENT);
	// Tap Window
	writeADXL345(REG_WINDOW, INIT_WINDOW);
	// Axis control for single tap/double tap
	writeADXL345(REG_TAP_AXES, INIT_TAP_AXES);

	// Free-Fall threshold
	writeADXL345(REG_THRESH_FF, INIT_THRESH_FF);
	// Free-Fall time
	writeADXL345(REG_TIME_FF, INIT_TIME_FF);

	// Interrupt mapping control
	writeADXL345(REG_INT_MAP, INIT_INT_MAP);

	// FIFO control : STREAM mode & # samples to trigger IRQ.
	writeADXL345(REG_FIFO_CTL, INIT_FIFO_CTL);

	// Interrupt enable control
	writeADXL345(REG_INT_ENABLE, INIT_INT_ENABLE);
	// Power-saving features control
	writeADXL345(REG_POWER_CTL, INIT_POWER_CTL);  // starts measurement mode

	// Now read out the settings:
	k = readADXL345(REG_DATA_FORMAT);
	printf("REG_DATA_FORMAT = %x\n", k);
	k = readADXL345(REG_BW_RATE);
	printf("REG_BW_RATE = %x\n", k);
	k = readADXL345(REG_OFSX);
	printf("REG_OFSX = %x\n", k);
	k = readADXL345(REG_OFSY);
	printf("REG_OFSY = %x\n", k);
	k = readADXL345(REG_OFSZ);
	printf("REG_OFSZ = %x\n", k);
	k = readADXL345(REG_THRESH_TAP);
	printf("REG_THRESH_TAP = %x\n", k);
	k = readADXL345(REG_DUR);
	printf("REG_DUR = %x\n", k);
	k = readADXL345(REG_LATENT);
	printf("REG_LATENT = %x\n", k);
	k = readADXL345(REG_WINDOW);
	printf("REG_WINDOW = %x\n", k);
	k = readADXL345(REG_THRESH_ACT);
	printf("REG_THRESH_ACT = %x\n", k);
	k = readADXL345(REG_THRESH_INACT);
	printf("REG_THRESH_INACT = %x\n", k);
	k = readADXL345(REG_TIME_INACT);
	printf("REG_TIME_INACT = %x\n", k);
	k = readADXL345(REG_ACT_INACT_CTL);
	printf("REG_ACT_INACT_CTL = %x\n", k);
	k = readADXL345(REG_THRESH_FF);
	printf("REG_THRESH_FF = %x\n", k);
	k = readADXL345(REG_TIME_FF);
	printf("REG_TIME_FF = %x\n", k);
	k = readADXL345(REG_TAP_AXES);
	printf("REG_TAP_AXES = %x\n", k);
	k = readADXL345(REG_INT_MAP);
	printf("REG_INT_MAP = %x\n", k);
	k = readADXL345(REG_FIFO_CTL);
	printf("REG_FIFO_CTL = %x\n", k);
	k = readADXL345(REG_INT_ENABLE);
	printf("REG_INT_ENABLE = %x\n", k);
	k = readADXL345(REG_POWER_CTL);
	printf("REG_POWER_CTL = %x\n", k);
	printf("------------------\n");
	// Clear the Activity Interrupt by reading the INT_SOURCE register from the ADXL345.
	k = readADXL345(REG_INT_SOURCE);
	printf("INIT: REG_INT_SOURCE = %x\n", k);
	printf("------------------\n");
}
