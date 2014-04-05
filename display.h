
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
	5 ACDFG     1 0 1 1 0 1 1 ?  6/E B
	6 ACDEFG    1 0 0 0 1 1 1 ?  7/F 1
	7 ABC       1 1 1 0 0 0 0 ?  0/8 7
	8 ABCDEFG   1 1 1 1 1 1 1 ?  7/F F
	9 ABCFG     1 1 1 0 0 1 1 ?  6/E 7
   10 G         0 0 0 0 0 0 1 ?  4/C 0  (hyphen)
   11           0 0 0 0 0 0 0 ?  0/8 0  (blank)
*/

#define UNIT_TEST	0

/* lane #1 */
#define DISP_1_1	11
#define DISP_1_2	12
#define DISP_1_3    13
#define DISP_1_4    14

/* lane #2 */
#define DISP_2_1    21
#define DISP_2_2    22
#define DISP_2_3    23
#define DISP_2_4    24

/* Digits with decimal point turned off */
//extern const int numbers[];

/* Digits with decimal point turned on */
//extern const int numbers_dp[];

/**
    Initialize all eight displays
	
	@return None
*/
void
Display_init(void);

/**
    Given the display ID and number, convert the number to a value
    that can be shifted into the display.

    @param dispID    the display ID (1 - 8)
    @param timerVal  the 32 bit value read from the timer TC register
    @param LED1      boolean value signifying turn on LED1
    @param LED2      boolean value signifying turn on LED2	
	
	@return None
*/
void
Display_displayNumber(const uint8_t dispID,
                      const uint32_t timerVal,
					  const uint8_t LED1,
					  const uint8_t LED2);
