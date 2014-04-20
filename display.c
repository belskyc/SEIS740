//

#include "stdio.h"

#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "task.h"

#include "display.h"
#include "timer.h"

/*
    The number arrays are based off of the following table:
	
	                  BITS
	            A B C D E F G DP
	# Segments	1 2 3 4 5 6 7 8  Value (LED ON/OFF)
	0 ABCDEF    1 1 1 1 1 1 0 ?  3/B F
	1 BC		0 1 1 0 0 0 0 ?  0/8 6
	2 ABDEG     1 1 0 1 1 0 1 ?  5/D B 
	3 ABCDG     1 1 1 1 0 0 1 ?  4/C F
	4 BCFG      0 1 1 0 0 1 1 ?  6/E 6
	5 ACDFG     1 0 1 1 0 1 1 ?  6/E D
	6 ACDEFG    1 0 1 1 1 1 1 ?  7/F D
	7 ABC       1 1 1 0 0 0 0 ?  0/8 7
	8 ABCDEFG   1 1 1 1 1 1 1 ?  7/F F
	9 ABCFG     1 1 1 0 0 1 1 ?  6/E 7
   10 G         0 0 0 0 0 0 1 ?  4/C 0  (hyphen)
   11           0 0 0 0 0 0 0 ?  0/8 0  (blank)
*/
static const int numbers[] =
{
    0x3F,
	0x06,
	0x5B,
	0x4F,
	0x66,
	0x6D,
	0x7D,
	0x07,
	0x7F,
	0x67,
	0x40,
	0x00,
};

static const int numbers_dp[] = 
{
    0xBF,
	0x86,
	0xDB,
	0xCF,
	0xE6,
	0xED,
	0xFD,
	0x87,
	0xFF,
	0xE7,
	0xC0,
	0x80,
};

#define NUM_DIGITS 		7

#define THOUSANDS_IDX	0
#define HUNDREDS_IDX	1
#define TENS_IDX		2
#define ONES_IDX		3
#define TENTHS_IDX		4
#define HUNDREDTHS_IDX	5
#define THOUSANDTHS_IDX	6

/*
    Return 0xFFFF on error, display bit values otherwise

    @param  timerVal value of timer counter in .1ms

    @return dsBits bit value to shift into display
*/
unsigned int
create_display_val(unsigned int timerVal)
{
    unsigned int dsBits = 0;
	unsigned int remainder = 0;
	unsigned int seconds = 0;
	int digits[NUM_DIGITS] = { 0, 0, 0, 0, 0, 0, 0 };
	
	seconds = timerVal / 10000;    // seconds
	// Check for out of range
	if (seconds > 9999)
	{
	    return 0x40404040;  // hyphens
	}
	
    digits[THOUSANDS_IDX] = timerVal / 10000000;  // thousands
	remainder = timerVal % 10000000;
	//printf("remainder = %d\n", remainder);
	
	digits[HUNDREDS_IDX] = remainder / 1000000; // Hundreds
	remainder = remainder % 1000000;
	//printf("remainder = %d\n", remainder);
	
	digits[TENS_IDX] = remainder / 100000; // Tens
	remainder = remainder % 100000;
	//printf("remainder = %d\n", remainder);
	
	digits[ONES_IDX] = remainder / 10000; // ones
	remainder = remainder % 10000;
	//printf("remainder = %d\n", remainder);
	
	digits[TENTHS_IDX] = remainder / 1000;     // tenths of a second
	remainder = remainder % 1000;  // remainder after tenths
	//printf("remainder = %d\n", remainder);
	
	digits[HUNDREDTHS_IDX] = remainder / 100;    // hundredths of a second
	remainder = remainder % 100;   // remainder after hundredths
	//printf("remainder = %d\n", remainder);
	
	digits[THOUSANDTHS_IDX] = remainder / 10; // milliseconds
	
	int i = 0;
	for (i = 0; i < NUM_DIGITS; i++)
	{
	    if (digits[i] > 0)
		{
		    break;
		}
	}
	
	//printf("first digit found at %d\n", i);
	if (i > 3)
	{
	    i = 3;  // less than 4 digits, set back to seconds
	}
	
	int j = 0;
	for (j = i + 3; j >= i; j--)
	{
	    dsBits = dsBits << 8;
		//printf("digits[%d] = %d\n", i, digits[i]);
		if (3 == j)
		{
		    dsBits |= numbers_dp[digits[j]];
		}
		else
		{
		    dsBits |= numbers[digits[j]];
		}

	}
	
	return dsBits;
}

void pushBit(const uint32_t bit, const uint8_t dispID)
{
	uint8_t i = 0;
	uint8_t k = 0;

	taskENTER_CRITICAL();
	// Wait for 2 timer events to ensure one full timer period has elapsed.
	for(i = 0; i < 2; i++)
	{
		timer1_runStatus = 0; // Clear timer flag.
		while(!timer1_runStatus)
			__NOP;  // Do nothing while waiting for timer event.
	}

	switch(dispID)
	{
	    case DISP_1_1:
	    	//LPC_GPIO1->FIOCLR |= GPIO_P1_29;  // ENABLE the 7SegDisp.
	    	LPC_GPIO1->FIOCLR |= GPIO_P1_19;  // ENABLE the 7SegDisp.
	    	break;
	    case DISP_2_1:
	    	LPC_GPIO1->FIOCLR |= GPIO_P1_23;  // ENABLE the 7SegDisp.
	    	break;
	    case DISP_1_2:
	    	LPC_GPIO1->FIOCLR |= GPIO_P1_20;  // ENABLE the 7SegDisp.
	    	break;
	    case DISP_2_2:
	    	LPC_GPIO1->FIOCLR |= GPIO_P1_24;  // ENABLE the 7SegDisp.
	    	break;
	    case DISP_1_3:
	    	LPC_GPIO1->FIOCLR |= GPIO_P1_21;  // ENABLE the 7SegDisp.
	        break;
	    case DISP_2_3:
	    	LPC_GPIO1->FIOCLR |= GPIO_P1_25;  // ENABLE the 7SegDisp.
	    	break;
	    case DISP_1_4:
	    	LPC_GPIO1->FIOCLR |= GPIO_P1_22;  // ENABLE the 7SegDisp.
	    	break;
	    case DISP_2_4:
	    	LPC_GPIO1->FIOCLR |= GPIO_P1_26;  // ENABLE the 7SegDisp.
	    	break;
	    default:
			__NOP;  // Do nothing.
	}

	if(bit) // Push "1" bit
	{
		LPC_GPIO1->FIOSET |= GPIO_P1_28;  // DATA bit = 1.
		__NOP; // Wait one clock cycle (3MHz => 333.33nSec)
		__NOP; // Wait one clock cycle (3MHz => 333.33nSec)
		LPC_GPIO1->FIOCLR |= GPIO_P1_29;  // FALLING-EDGE the CLOCK to the 7SegDisp.
	}
	else // Push "0" bit
	{
		LPC_GPIO1->FIOCLR |= GPIO_P1_28;  // DATA bit = 0.
		__NOP; // Wait one clock cycle (3MHz => 333.33nSec)
		__NOP; // Wait one clock cycle (3MHz => 333.33nSec)
		LPC_GPIO1->FIOCLR |= GPIO_P1_29;  // FALLING-EDGE the CLOCK to the 7SegDisp.
	}

	// Wait for 2 timer events to ensure one full timer period has elapsed.
	for(k = 0; k < 2; k++)
	{
		timer1_runStatus = 0; // Clear timer flag.
		while(!timer1_runStatus)
			__NOP;  // Do nothing while waiting for timer event.
	}

	LPC_GPIO1->FIOSET |= GPIO_P1_29;  // RISING-EDGE the CLOCK to the 7SegDisp.
	taskEXIT_CRITICAL();

	return;
}

void
display(uint32_t id,
		uint32_t val,
        uint8_t LED1,
	    uint8_t LED2)
{
	uint32_t dispBit;
	uint8_t i = 0;
	uint8_t k = 0;

	// 36 total bits to push out to the 7-Segment Display.

	vPrintStringAndNumber("display id", id);
	pushBit(1, id); // Push a START-bit (bit-1).
	//vPrintStringAndNumber("pushing bit", 1);

	for(i = 0; i < 32; i++) // Push 32 digit bits (bits 2-33).
	{
		dispBit = (val >> i) & 0x1;  // Mask bit
		//vPrintStringAndNumber("pushing bit", dispBit);
		pushBit(dispBit, id);
	}

	// Push LED bits (bits 34 & 35).
	if(LED1)
	{
		pushBit(1, id);
		//vPrintStringAndNumber("pushing bit", 1);
	}
	else
	{
		pushBit(0, id);
		//vPrintStringAndNumber("pushing bit", 0);
	}

	if (LED2)
	{
		pushBit(1, id);
		//vPrintStringAndNumber("pushing bit", 1);
	}
	else
	{
		pushBit(0, id);
		//vPrintStringAndNumber("pushing bit", 0);
	}

	pushBit(1, id); // Push a LOAD-bit.
	//vPrintStringAndNumber("pushing bit", 1);
	// Wait for 2 timer events to ensure one full timer period has elapsed.
	for(k = 0; k < 2; k++)
	{
		timer1_runStatus = 0; // Clear timer flag.
		while(!timer1_runStatus)
			__NOP;  // Do nothing while waiting for timer event.
	}

	//LPC_GPIO1->FIOCLR |= GPIO_P1_26;  // FALLING-EDGE the CLOCK to the 7SegDisp.
	LPC_GPIO1->FIOCLR |= GPIO_P1_29;  // FALLING-EDGE the CLOCK to the 7SegDisp.

	switch(id)
	{
	    case DISP_1_1:
	    	//LPC_GPIO1->FIOSET |= GPIO_P1_29;  // DISABLE the 7SegDisp.
	    	LPC_GPIO1->FIOSET |= GPIO_P1_19;  // DISABLE the 7SegDisp.
	    	break;
	    case DISP_2_1:
	    	LPC_GPIO1->FIOSET |= GPIO_P1_23;  // DISABLE the 7SegDisp.
	    	break;
	    case DISP_1_2:
	    	LPC_GPIO1->FIOSET |= GPIO_P1_20;  // DISABLE the 7SegDisp.
	    	break;
	    case DISP_2_2:
	    	LPC_GPIO1->FIOSET |= GPIO_P1_24;  // DISABLE the 7SegDisp.
	    	break;
	    case DISP_1_3:
	    	LPC_GPIO1->FIOSET |= GPIO_P1_21;  // DISABLE the 7SegDisp.
	        break;
	    case DISP_2_3:
	    	LPC_GPIO1->FIOSET |= GPIO_P1_25;  // DISABLE the 7SegDisp.
	    	break;
	    case DISP_1_4:
	    	LPC_GPIO1->FIOSET |= GPIO_P1_22;  // DISABLE the 7SegDisp.
	    	break;
	    case DISP_2_4:
	    	LPC_GPIO1->FIOSET |= GPIO_P1_26;  // DISABLE the 7SegDisp.
	    	break;
	    default:
	    	__NOP;  // Do nothing.
	}

	return;
}

void
Display_init(void)
{
	// Configuration for the LPC1769 board:
	// int tempInt1, tempInt2;
#if 0
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
#endif

/* ***************************************************************** */

	/*
	 * Enable Pins (on PAD) for 7-Segment Displays
	 */

	// Set P_19 to value '00' - GPIO; Used for ENABLE of 7-Segment Display 1_1
	LPC_PINCON->PINSEL3	&= ~(3 << 6);
	LPC_GPIO1->FIODIR |= GPIO_P1_19;  // Set GPIO - P1_19 - to be output (= 1).

	// Set P_20 to value '00' - GPIO; Used for ENABLE of 7-Segment Display 1_2
	LPC_PINCON->PINSEL3	&= ~(3 << 8);
	LPC_GPIO1->FIODIR |= GPIO_P1_20;  // Set GPIO - P1_20 - to be output (= 1).

	// Set P_28 to value '00' - GPIO; Used for ENABLE of 7-Segment Display 1_3
	LPC_PINCON->PINSEL3	&= ~(3 << 10);
	LPC_GPIO1->FIODIR |= GPIO_P1_21;  // Set GPIO - P1_21 - to be output (= 1).

	// Set P_28 to value '00' - GPIO; Used for ENABLE of 7-Segment Display 1_4
	LPC_PINCON->PINSEL3	&= ~(3 << 12);
	LPC_GPIO1->FIODIR |= GPIO_P1_22;  // Set GPIO - P1_22 - to be output (= 1).

	// Set P_28 to value '00' - GPIO; Used for ENABLE of 7-Segment Display 2_1
	LPC_PINCON->PINSEL3	&= ~(3 << 14);
	LPC_GPIO1->FIODIR |= GPIO_P1_23;  // Set GPIO - P1_23 - to be output (= 1).

	// Set P_28 to value '00' - GPIO; Used for ENABLE of 7-Segment Display 2_2
	LPC_PINCON->PINSEL3	&= ~(3 << 16);
	LPC_GPIO1->FIODIR |= GPIO_P1_24;  // Set GPIO - P1_25 - to be output (= 1).

	// Set P_28 to value '00' - GPIO; Used for ENABLE of 7-Segment Display 2_3
	LPC_PINCON->PINSEL3	&= ~(3 << 18);
	LPC_GPIO1->FIODIR |= GPIO_P1_25;  // Set GPIO - P1_25 - to be output (= 1).

	// Set P_28 to value '00' - GPIO; Used for ENABLE of 7-Segment Display 2_4
	LPC_PINCON->PINSEL3	&= ~(3 << 20);
	LPC_GPIO1->FIODIR |= GPIO_P1_26;  // Set GPIO - P1_26 - to be output (= 1).

	/*
	 * Data and clock for 7-Segment Displays
	 */

	// Set P_28 to value '00' - GPIO; Used for DATA of 7-Segment Displays
	LPC_PINCON->PINSEL3	&= ~(3 << 24);
	LPC_GPIO1->FIODIR |= GPIO_P1_28;  // Set GPIO - P1_28 - to be output (= 1).

	// Set P_29 to value '00' - GPIO; Used for CLOCK of 7-Segment Displays
	LPC_PINCON->PINSEL3	&= ~(3 << 26);
	LPC_GPIO1->FIODIR |= GPIO_P1_29;  // Set GPIO - P1_29 - to be output (= 1).

	/* ***************************************************************** */

	__NOP;
}

void
Display_displayNumber(const uint8_t dispID,
                      const uint32_t timerVal,
					  const uint8_t LED1,
					  const uint8_t LED2)
{
    uint32_t dispVal = 0;
	
	dispVal = create_display_val(timerVal);
	//vPrintStringAndNumber("Display_displayNumber: dispVal = ", dispVal);
	display(dispID, dispVal, LED1, LED2);

}

#if UNIT_TEST
int
main(int argc, char **argv[])
{
    unsigned int dispVal;

    dispVal = create_display_val(100000000);  // too high
	printf("10000 Expected 40404040, dispVal = %x\n", dispVal);
	
	dispVal = create_display_val(99990000); // 9999
	printf(" 9999 Expected 67676767, dispVal = %x\n", dispVal);
	
	dispVal = create_display_val(8765000); // 876.5
	printf("876.5 Expected 7f07fd6b, dispVal = %x\n", dispVal);

    dispVal = create_display_val(765400); // 76.54
	printf("76.54 Expected 07fd6b66, dispVal = %x\n", dispVal);
	
	dispVal = create_display_val(65430); // 6.543
	printf("6.543 Expected fd6b664f, dispVal = %x\n", dispVal);

	dispVal = create_display_val(5430); // 0.543
	printf("0.543 Expected bf6b664f, dispVal = %x\n", dispVal);
	
	dispVal = create_display_val(432);  // 0.043
	printf("0.043 Expected bf3f664f, dispVal = %x\n", dispVal);
	
	dispVal = create_display_val(32);   // 0.003
	printf("0.003 Expected bf3f3f4f, dispVal = %x\n", dispVal);
	
	dispVal = create_display_val(1);    // 0.000
	printf("0.000 Expected bf3f3f3f, dispVal = %x\n", dispVal);
		
	dispVal = create_display_val(0);    // 0.000
	printf("0,000 Expected bf3f3f3f, dispVal = %x\n", dispVal);
}
#endif
