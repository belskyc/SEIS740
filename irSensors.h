/*
 * irSensors.h
 *
 *  Created on: Mar 16, 2014
 *      Author: Jeff
 */

#define IR_ID_1_1	0
#define IR_ID_1_2   1
#define IR_ID_1_3   2
#define IR_ID_1_4   3
#define IR_ID_2_1   4
#define IR_ID_2_2   5
#define IR_ID_2_3	6
#define IR_ID_2_4   7

typedef struct
{
	uint8_t  irID;    // IR sensor ID
	uint32_t tVal;    // timer value
} dispReq;

static dispReq DisplayRequests[] =
{
		{ IR_ID_1_1, 0 },
		{ IR_ID_1_2, 0 },
		{ IR_ID_1_3, 0 },
		{ IR_ID_1_4, 0 },
		{ IR_ID_2_1, 0 },
		{ IR_ID_2_2, 0 },
		{ IR_ID_2_3, 0 },
		{ IR_ID_2_4, 0 },
};

static uint8_t IR_IRQ_errors[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

void
IR_IRQ_handler_1_1();

void
IR_IRQ_handler_1_2();

void
IR_IRQ_handler_1_3();

void
IR_IRQ_handler_1_4();

void
IR_IRQ_handler_2_1();

void
IR_IRQ_handler_2_2();

void
IR_IRQ_handler_2_3();

void
IR_IRQ_handler_2_4();

/*
 * Initialize all GPIO for IR sensors
 */
int
IR_init();

