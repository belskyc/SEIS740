/****************************************************************************
 *   $Id:: ssp.c 5804 2010-12-04 00:32:12Z usb00423                         $
 *   Project: NXP LPC17xx SSP example
 *
 *   Description:
 *     This file contains SSP code example which include SSP initialization, 
 *     SSP interrupt handler, and APIs for SSP access.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/

#include "stdio.h"

#include "LPC17xx.h"			/* LPC17xx Peripheral Registers */
#include "ssp.h"
#include "ADXL345_init.h"

/* statistics of all the interrupts */
volatile uint32_t interrupt0RxStat = 0;
volatile uint32_t interrupt0OverRunStat = 0;
volatile uint32_t interrupt0RxTimeoutStat = 0;
volatile uint32_t interrupt1RxStat = 0;
volatile uint32_t interrupt1OverRunStat = 0;
volatile uint32_t interrupt1RxTimeoutStat = 0;

/*****************************************************************************
** Function name:		SSP_IRQHandler
**
** Descriptions:		SSP port is used for SPI communication.
**						SSP interrupt handler
**						The algorithm is, if RXFIFO is at least half full, 
**						start receive until it's empty; if TXFIFO is at least
**						half empty, start transmit until it's full.
**						This will maximize the use of both FIFOs and performance.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void SSP0_IRQHandler(void) 
{
  uint32_t regValue;

  regValue = LPC_SSP0->MIS;
  if ( regValue & SSPMIS_RORMIS )	/* Receive overrun interrupt */
  {
	interrupt0OverRunStat++;
	LPC_SSP0->ICR = SSPICR_RORIC;		/* clear interrupt */
  }
  if ( regValue & SSPMIS_RTMIS )	/* Receive timeout interrupt */
  {
	interrupt0RxTimeoutStat++;
	LPC_SSP0->ICR = SSPICR_RTIC;		/* clear interrupt */
  }

  /* please be aware that, in main and ISR, CurrentRxIndex and CurrentTxIndex
  are shared as global variables. It may create some race condition that main
  and ISR manipulate these variables at the same time. SSPSR_BSY checking (polling)
  in both main and ISR could prevent this kind of race condition */
  if ( regValue & SSPMIS_RXMIS )	/* Rx at least half full */
  {
	interrupt0RxStat++;		/* receive until it's empty */		
  }
  return;
}

/*****************************************************************************
** Function name:		SSP_IRQHandler
**
** Descriptions:		SSP port is used for SPI communication.
**						SSP interrupt handler
**						The algorithm is, if RXFIFO is at least half full, 
**						start receive until it's empty; if TXFIFO is at least
**						half empty, start transmit until it's full.
**						This will maximize the use of both FIFOs and performance.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void SSP1_IRQHandler(void) 
{
  uint32_t regValue;

  regValue = LPC_SSP1->MIS;
  if ( regValue & SSPMIS_RORMIS )	/* Receive overrun interrupt */
  {
	interrupt1OverRunStat++;
	LPC_SSP1->ICR = SSPICR_RORIC;		/* clear interrupt */
  }
  if ( regValue & SSPMIS_RTMIS )	/* Receive timeout interrupt */
  {
	interrupt1RxTimeoutStat++;
	LPC_SSP1->ICR = SSPICR_RTIC;		/* clear interrupt */
  }

  /* please be aware that, in main and ISR, CurrentRxIndex and CurrentTxIndex
  are shared as global variables. It may create some race condition that main
  and ISR manipulate these variables at the same time. SSPSR_BSY checking (polling)
  in both main and ISR could prevent this kind of race condition */
  if ( regValue & SSPMIS_RXMIS )	/* Rx at least half full */
  {
	interrupt1RxStat++;		/* receive until it's empty */		
  }
  return;
}

/*****************************************************************************
** Function name:		SSP0_SSELToggle
**
** Descriptions:		SSP0 CS manual set
**				
** parameters:			port num, toggle(1 is high, 0 is low)
** Returned value:		None
** 
*****************************************************************************/
void SSP_SSELToggle( uint32_t portnum, uint32_t toggle )
{
  if ( portnum == 0 )
  {
    if ( !toggle )
	  LPC_GPIO0->FIOCLR |= (0x1<<16);
    else
	  LPC_GPIO0->FIOSET |= (0x1<<16);	  
  }
  else if ( portnum == 1 )
  {
    if ( !toggle )
	  LPC_GPIO0->FIOCLR |= (0x1<<6);
    else
	  LPC_GPIO0->FIOSET |= (0x1<<6);	  
  }
  return;
}

#if  0
/*****************************************************************************
** Function name:		SSPInitADXL345
**
** Descriptions:		SSP port initialization routine
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void SSP0InitADXL345( void )
{
  uint8_t i, Dummy=Dummy;

  /* Enable AHB clock to the SSP0. ??? */
  LPC_SC->PCONP |= (0x1<<21);  // Power on SSP0

  /* Further divider is needed on SSP0 clock. Using default divided by 4 ??? */
  LPC_SC->PCLKSEL1 &= ~(0x3<<10);  // peripheral clock select for SSP0, PCLK_peripheral = CCLK/8

  /* P0.15 thru P0.18 as SSP0 */
  LPC_PINCON->PINSEL0 &= ~(0x3UL<<30);  // clear bits 30 & 31
  LPC_PINCON->PINSEL0 |= (0x2UL<<30);   // set bits 30 & 31 to 10 (SCK0)
  /* set SSEL0, MISO0, and MOSI0 respectively */
  LPC_PINCON->PINSEL1 &= ~((0x3<<0)|(0x3<<2)|(0x3<<4));   // clear bits 0, 1, 2, 3, 4 & 5
  LPC_PINCON->PINSEL1 |= ((0x2<<0)|(0x2<<2)|(0x2<<4));    // set (0,1), (2,3) & (4,5) -> 10

#if !USE_CS  // This would be for SEEPROM test (we don't want to driver the clock ourselves so USE_CS = 1)
  LPC_PINCON->PINSEL1 &= ~(0x3<<0);     // Change pin out from SSEL0 to GPIO
  LPC_GPIO0->FIODIR |= (0x1<<16);		/* P0.16 defined as GPIO and Outputs */
#endif

  /* Set DSS data to 8-bit, Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 15 */
  //LPC_SSP0->CR0 = 0x0707;

  /* Set DSS data to 16-bit, Frame format SPI, CPOL = 1, CPHA = 1, and SCR is 0 */
  LPC_SSP0->CR0 = 0x00CF;

  /* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
  LPC_SSP0->CPSR = 0x2;

  for ( i = 0; i < FIFOSIZE; i++ )
  {
	Dummy = LPC_SSP0->DR;		/* clear the RxFIFO */
  }

  /* Enable the SSP Interrupt */
  NVIC_EnableIRQ(SSP0_IRQn);

  /* Set SSPINMS registers to enable interrupts */
  /* enable all error related interrupts */
  //LPC_SSP0->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;

  /* Device select as master, SSP Enabled */
#if LOOPBACK_MODE
  LPC_SSP0->CR1 = SSPCR1_LBM | SSPCR1_SSE;
#else
#if SSP_SLAVE
  /* Slave mode */
  if ( LPC_SSP0->CR1 & SSPCR1_SSE )
  {
	/* The slave bit can't be set until SSE bit is zero. */
	LPC_SSP0->CR1 &= ~SSPCR1_SSE;
  }
  LPC_SSP0->CR1 = SSPCR1_MS;		/* Enable slave bit first */
  LPC_SSP0->CR1 |= SSPCR1_SSE;	/* Enable SSP */
#else
  /* Master mode */
  LPC_SSP0->CR1 = SSPCR1_SSE;
#endif
#endif

  return;
}
#endif

#if 1
/*****************************************************************************
** Function name:		SSP1InitADXL345
**
** Descriptions:		SSP port initialization routine
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void SSP1InitADXL345( void )
{
  uint8_t i, Dummy=Dummy;

  /* Enable AHB clock to the SSP0. ??? */
  LPC_SC->PCONP |= (0x1<<10);  // Power on SSP1

  /* Further divider is needed on SSP0 clock. Using default divided by 4 ??? */
  LPC_SC->PCLKSEL0 &= ~(0x3<<20);  // peripheral clock select for SSP1, PCLK_peripheral = CCLK/8

  /* P0.6~0.9 as SSP1 */
  LPC_PINCON->PINSEL0 &= ~((0x3<<12)|(0x3<<14)|(0x3<<16)|(0x3<<18));
  LPC_PINCON->PINSEL0 |= ((0x2<<12)|(0x2<<14)|(0x2<<16)|(0x2<<18));


#if !USE_CS
  LPC_PINCON->PINSEL0 &= ~(0x3<<12);
  LPC_GPIO0->FIODIR |= (0x1<<6);		/* P0.6 defined as GPIO and Outputs */
#endif

  /* Set DSS data to 8-bit, Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 15 */
  //LPC_SSP0->CR0 = 0x0707;

  /* Set DSS data to 16-bit frame, Frame format SPI, CPOL = 1, CPHA = 1, and SCR is 0 */
  //LPC_SSP1->CR0 = 0x01CF;  // 16 bit frame, SCR = 1
  LPC_SSP1->CR0 = 0x01C7; // 8 bit frame, SCR = 1

  /* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
  LPC_SSP1->CPSR = 0x0F;

  for ( i = 0; i < FIFOSIZE; i++ )
  {
	Dummy = LPC_SSP1->DR;		/* clear the RxFIFO */
  }

  /* Enable the SSP Interrupt */
  NVIC_EnableIRQ(SSP1_IRQn);

  /* Set SSPINMS registers to enable interrupts */
  /* enable all error related interrupts */
  //LPC_SSP0->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;

  /* Device select as master, SSP Enabled */
#if LOOPBACK_MODE
  LPC_SSP1->CR1 = SSPCR1_LBM | SSPCR1_SSE;
#else
#if SSP_SLAVE
  /* Slave mode */
  if ( LPC_SSP1->CR1 & SSPCR1_SSE )
  {
	/* The slave bit can't be set until SSE bit is zero. */
	LPC_SSP1->CR1 &= ~SSPCR1_SSE;
  }
  LPC_SSP1->CR1 = SSPCR1_MS;		/* Enable slave bit first */
  LPC_SSP1->CR1 |= SSPCR1_SSE;	/* Enable SSP */
#else
  /* Master mode */
  LPC_SSP1->CR1 = SSPCR1_SSE;
#endif
#endif

  return;
}
#endif
/*****************************************************************************
** Function name:		SSPInit
**
** Descriptions:		SSP port initialization routine
**				
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void SSP0Init( void )
{
  uint8_t i, Dummy=Dummy;

  /* Enable AHB clock to the SSP0. */
  LPC_SC->PCONP |= (0x1<<21);

  /* Further divider is needed on SSP0 clock. Using default divided by 4 */
  LPC_SC->PCLKSEL1 &= ~(0x3<<10);

  /* P0.15~0.18 as SSP0 */
  LPC_PINCON->PINSEL0 &= ~(0x3UL<<30);  // SCK0?
  LPC_PINCON->PINSEL0 |= (0x2UL<<30);
  LPC_PINCON->PINSEL1 &= ~((0x3<<0)|(0x3<<2)|(0x3<<4));
  LPC_PINCON->PINSEL1 |= ((0x2<<0)|(0x2<<2)|(0x2<<4));
  
#if !USE_CS
  LPC_PINCON->PINSEL1 &= ~(0x3<<0);
  LPC_GPIO0->FIODIR |= (0x1<<16);		/* P0.16 defined as GPIO and Outputs */
#endif
		
  /* Set DSS data to 8-bit, Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 15 */
  LPC_SSP0->CR0 = 0x0707;

  /* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
  LPC_SSP0->CPSR = 0x2;

  for ( i = 0; i < FIFOSIZE; i++ )
  {
	Dummy = LPC_SSP0->DR;		/* clear the RxFIFO */
  }

  /* Enable the SSP Interrupt */
  NVIC_EnableIRQ(SSP0_IRQn);
	
  /* Device select as master, SSP Enabled */
#if LOOPBACK_MODE
  LPC_SSP0->CR1 = SSPCR1_LBM | SSPCR1_SSE;
#else
#if SSP_SLAVE
  /* Slave mode */
  if ( LPC_SSP0->CR1 & SSPCR1_SSE )
  {
	/* The slave bit can't be set until SSE bit is zero. */
	LPC_SSP0->CR1 &= ~SSPCR1_SSE;
  }
  LPC_SSP0->CR1 = SSPCR1_MS;		/* Enable slave bit first */
  LPC_SSP0->CR1 |= SSPCR1_SSE;	/* Enable SSP */
#else
  /* Master mode */
  LPC_SSP0->CR1 = SSPCR1_SSE;
#endif
#endif
  /* Set SSPINMS registers to enable interrupts */
  /* enable all error related interrupts */
  LPC_SSP0->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;
  return;
}

/*****************************************************************************
** Function name:		SSPInit
**
** Descriptions:		SSP port initialization routine
**				
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void SSP1Init( void )
{
  uint8_t i, Dummy=Dummy;

  /* Enable AHB clock to the SSP1. */
  LPC_SC->PCONP |= (0x1<<10);

  /* Further divider is needed on SSP1 clock. Using default divided by 4 */
  /* pclk = cclk / 4 -> 12MHz / 4 */
  LPC_SC->PCLKSEL0 &= ~(0x3<<20);

  /* P0.6~0.9 as SSP1 */
  LPC_PINCON->PINSEL0 &= ~((0x3<<12)|(0x3<<14)|(0x3<<16)|(0x3<<18));
  LPC_PINCON->PINSEL0 |= ((0x2<<12)|(0x2<<14)|(0x2<<16)|(0x2<<18));
  
#if !USE_CS
  LPC_PINCON->PINSEL0 &= ~(0x3<<12);
  LPC_GPIO0->FIODIR |= (0x1<<6);		/* P0.6 defined as GPIO and Outputs */
#endif
		
  /* Set DSS data to 16-bit, Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 1 */
  // TODO: fix the above comment
  //LPC_SSP1->CR0 = 0x01C8;  // 9 bit frames?
  //LPC_SSP1->CR0 = 0x01C7;  // 8 bit frames
  LPC_SSP1->CR0 = 0x00C7; // 8 bit frames, SCR is 0

  /* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
  LPC_SSP1->CPSR = 0x0f;

  for ( i = 0; i < FIFOSIZE; i++ )
  {
	Dummy = LPC_SSP1->DR;		/* clear the RxFIFO */
  }

  /* Enable the SSP Interrupt */
  NVIC_EnableIRQ(SSP1_IRQn);
	
  /* Device select as master, SSP Enabled */
#if LOOPBACK_MODE
  LPC_SSP1->CR1 = SSPCR1_LBM | SSPCR1_SSE;
#else
#if SSP_SLAVE
  /* Slave mode */
  if ( LPC_SSP1->CR1 & SSPCR1_SSE )
  {
	/* The slave bit can't be set until SSE bit is zero. */
	LPC_SSP1->CR1 &= ~SSPCR1_SSE;
  }
  LPC_SSP1->CR1 = SSPCR1_MS;		/* Enable slave bit first */
  LPC_SSP1->CR1 |= SSPCR1_SSE;	/* Enable SSP */
#else
  /* Master mode */
  LPC_SSP1->CR1 = SSPCR1_SSE;
#endif
#endif
  /* Set SSPINMS registers to enable interrupts */
  /* enable all error related interrupts */
  LPC_SSP1->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;
  return;
}

void SSPSendADXL34516bits( uint32_t portnum, uint8_t *buf, uint32_t Length )
{
	uint16_t write_val = 0;
	uint8_t Dummy = Dummy;

	write_val = buf[0];
	write_val = (write_val << 8);
	write_val |= buf[1];

	//printf("SSPSendAXL34516bits: writing %x\n",write_val);
    if ( portnum == 0 )
	{
      /* Move on only if NOT busy and TX FIFO not full. */
	  while ( (LPC_SSP0->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	  LPC_SSP0->DR = write_val;
	  //LPC_SSP0->DR = buf[0];

	 // while ( (LPC_SSP0->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	  //LPC_SSP0->DR = buf[1];

	  while ( (LPC_SSP0->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  /* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO
	  on MISO. Otherwise, when SSP0Receive() is called, previous data byte
	  is left in the FIFO. */
	  //Dummy = LPC_SSP0->DR;
    }
    else if ( portnum == 1 )
	{
	  /* Move on only if NOT busy and TX FIFO not full. */
	  while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	  LPC_SSP1->DR = write_val;

	  //printf("SSPSendAXL34516bits: SSP1->SR = %x\n", LPC_SSP1->SR);
	  while ( (LPC_SSP1->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );

	  //printf("SSPSendAXL34516bits: SSP1->SR = %x\n", LPC_SSP1->SR);
	  //    Dummy = LPC_SSP1->DR;
	  //    printf("SSPSendAXL34516bits: read %x from DR\n", Dummy);
	  //printf("SSPSendAXL34516bits: SSP1->SR = %x\n", LPC_SSP1->SR);


    }
}

void
SSPSendADXL345XYZRead()
{

	uint16_t write_val = 0;
	int i = 0;
	uint16_t dummy = 0;

	for (i = 0; i < FIFOSIZE; i++)
		dummy = LPC_SSP1->DR;

	// Send 0x55 as our dummy bytes while MISO is being written by slave
	write_val = 0xF255;  // 0x32 | 0xC0 high byte, 0x55 low
	//write_val = 0xF2F2;
	//write_val = 0xB255;
	//write_val = 0xF155;

	//printf("SSPSendADXL345XYZRead(): sending 0x%x\n", write_val);
	//printf("SSPSendADXL345XYZRead(): SR = 0x%x\n", LPC_SSP1->SR);

    /* Move on only if NOT busy and TX FIFO not full. */
	while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	LPC_SSP1->DR = write_val;

#if 1
	// Send 5 more bytes to receive the remaining 5 bytes from ADXL345
	for (i = 0; i < 5; i++)
	{
		while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
		//printf("SSPSendADXL345XYZRead(): sending 0x55\n");
		LPC_SSP1->DR = 0x5555;
	}
#endif
	//printf("SSPSendADXL345XYZRead(): SR = 0x%x\n", LPC_SSP1->SR);
}

/*****************************************************************************
** Function name:		SSPSend
**
** Descriptions:		Send a block of data to the SSP port, the 
**						first parameter is the buffer pointer, the 2nd 
**						parameter is the block length.
**
** parameters:			buffer pointer, and the block length
** Returned value:		None
** 
*****************************************************************************/
void SSPSend( uint32_t portnum, uint8_t *buf, uint32_t Length )
{
  uint32_t i;
  uint8_t Dummy = Dummy;
    
  for ( i = 0; i < Length; i++ )
  {
    if ( portnum == 0 )
	{
      /* Move on only if NOT busy and TX FIFO not full. */
	  while ( (LPC_SSP0->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	  LPC_SSP0->DR = *buf;
	  buf++;
#if !LOOPBACK_MODE
	  while ( (LPC_SSP0->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  /* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO 
	  on MISO. Otherwise, when SSP0Receive() is called, previous data byte
	  is left in the FIFO. */
	  //Dummy = LPC_SSP0->DR;
#else
	  /* Wait until the Busy bit is cleared. */
	  while ( LPC_SSP0->SR & SSPSR_BSY );
#endif
    }
    else if ( portnum == 1 )
	{
	  /* Move on only if NOT busy and TX FIFO not full. */
	  while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	  LPC_SSP1->DR = *buf;  // Send register address.
	  buf++;
	  while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	  LPC_SSP1->DR = *buf;  // Send data to write to register.

#if !LOOPBACK_MODE
	  // WAIT until SR indicates NOT busy and Rx FIFO is NOT empty.
	  while ( (LPC_SSP1->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  /* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO 
	  on MISO. Otherwise, when SSP0Receive() is called, previous data byte
	  is left in the FIFO. */
	  //printf("SSPSend: SSP1->SR = %x\n", LPC_SSP1->SR);
	  for (i = 0; i < FIFOSIZE; i++)
	  {
		  Dummy = LPC_SSP1->DR;
		  //printf("SSPSend: read %x from DR\n", Dummy);
		  //printf("SSPSend: SSP1->SR = %x\n", LPC_SSP1->SR);
	  }

#else
	  /* Wait until the Busy bit is cleared. */
	  while ( LPC_SSP1->SR & SSPSR_BSY );
#endif
    }
  }
  return; 
}


//
void SSPWriteSingleByte( uint32_t portnum, uint8_t *buf, uint32_t Length )
{
  uint32_t i;
  uint8_t Dummy;

  for ( i = 0; i < Length; i++ )
  {
    if ( portnum == 0 )
	{
    	// ... SSP0 NOT implemented...
    }
    else if ( portnum == 1 )
	{
	  /* Move on only if NOT busy and TX FIFO not full. */
	  while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	  LPC_SSP1->DR = *buf;  // Send register address.
	  buf++;
	  while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	  LPC_SSP1->DR = *buf;  // Send data to write to register.

	  // WAIT until SR indicates NOT busy and Rx FIFO is NOT empty.
	  while ( (LPC_SSP1->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  /* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO
	  on MISO. Otherwise, when SSP0Receive() is called, previous data byte
	  is left in the FIFO. */
	  //printf("SSPSend: SSP1->SR = %x\n", LPC_SSP1->SR);
	  for (i = 0; i < FIFOSIZE; i++)
	  {
		  Dummy = LPC_SSP1->DR;
		  //printf("SSPSend: read %x from DR\n", Dummy);
		  //printf("SSPSend: SSP1->SR = %x\n", LPC_SSP1->SR);
	  }
    }
  }
  return;
}


//
void SSPMultByteRead( uint32_t portnum, uint8_t *buf)
{
	uint8_t i;
	uint8_t Reg = REG_DATAX0;  // For MultByte Read, always start reading at REG_DATAX0.
	uint8_t Junk = 0x55;
	// uint32_t Temp32 = 0x12345678;
	Reg |= 0xC0;  // Set READ and MULTIBYTE bits.


    if ( portnum == 0 )
	{
    	// ... SSP0 NOT implemented...
    }
    else if ( portnum == 1 )
	{
		// Now read all six data registers (X0, X1, Y0, Y1, Z0, Z1).
		for (i = 0; i < 7; i++)
		{
			// Move on only if NOT busy and TX FIFO not full.
			while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
			if(i == 0) LPC_SSP1->DR = Reg;  // First, write out REG to read from.
			else LPC_SSP1->DR = Junk;  // Then just clock bits through SPI.
			// WAIT until SR indicates NOT busy and Rx FIFO is NOT empty.
			while ( (LPC_SSP1->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
			if( i == 0) buf[0] = LPC_SSP1->DR;  // Read garbage after writing Reg address.
			else buf[i-1] = LPC_SSP1->DR;  // Now getting good XYZ data.
			// buf[i] = Temp32++;
			// printf("5: i = %d AND buf[%d] = 0x%x.\n", i, i, buf[i]);
		}
    }
  return;
}


/*****************************************************************************
** Function name:		SSPReceive
** Descriptions:		the module will receive a block of data from 
**						the SSP, the 2nd parameter is the block 
**						length.
** parameters:			buffer pointer, and block length
** Returned value:		None
** 
*****************************************************************************/
// SSPReceive(1, write_buf[1], read_buf[0], 1);
// OLD: void SSPReceive( uint32_t portnum, uint8_t *buf, uint32_t Length )
void SSPReceive( uint32_t portnum, uint8_t *buf, uint32_t Length )
{
  uint32_t i;
  // uint32_t d = 0;

  for ( i = 0; i < Length; i++ )
  {

    if ( portnum == 0 )
    {
    	// ... SSP0 NOT implemented...
    }
	else if ( portnum == 1 )
	{
		// WAIT until SR indicates NOT busy and Rx FIFO is NOT empty.
		while ( (LPC_SSP1->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
		*buf = LPC_SSP1->DR;
		*buf++;
	}
  }
  return; 
}

// Reads the contents of a single-byte register from the ADXL345.
// This function assumes that immediately before it's executed that a
//  SSPSend() routine was executed informing the ADXL345 of which
//  register to read from.
void SSPReceiveSingleByte( uint32_t portnum, uint8_t *wbuf, uint8_t *rbuf)
{
	// uint32_t i;
	// uint32_t d = 0;
	uint16_t dummy;

	if ( portnum == 0 )
	{
		// ... SSP0 NOT implemented...
	}
	else if ( portnum == 1 )
	{
		// First, write out REG to read from.
		// Move on only if NOT busy and TX FIFO not full.
		while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
		LPC_SSP1->DR = *wbuf;  // Send register address.
		// WAIT until SR indicates NOT busy and Rx FIFO is NOT empty.
		while ( (LPC_SSP1->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
		dummy = LPC_SSP1->DR;  // Read FIFO.
		printf("dummy=0x%x\n", dummy);
		wbuf++;
		while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
		LPC_SSP1->DR = *wbuf;  // Send dummy data to clock out data into FIFO.
		// WAIT until SR indicates NOT busy and Rx FIFO is NOT empty.
		while ( (LPC_SSP1->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
		*rbuf = LPC_SSP1->DR;  // Read FIFO.
	}

	return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

