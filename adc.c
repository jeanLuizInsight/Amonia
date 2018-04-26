/****************************************************************************
 *   $Id:: adc.c 6089 2011-01-06 04:38:09Z nxp12832                         $
 *   Project: NXP LPC17xx ADC example
 *
 *   Description:
 *     This file contains ADC code example which include ADC 
 *     initialization, ADC interrupt handler, and APIs for ADC
 *     reading.
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
#include "adc.h"
#include <LPC17xx.h>

/*****************************************************************************
** Function name:		ADCInit
**
** Descriptions:		initialize ADC channel
**
** parameters:			ADC clock rate
** Returned value:		None
** 
*****************************************************************************/
void adc_init( int adc_Clk ) {
  /* Enable CLOCK into ADC controller */
	//PCONP: permite habilitar/desabilitar funções de periféricos (1 = habilita, 0 = desabilita)
  LPC_SC->PCONP |= (1 << 12); //registro PCADC: bit de controle power/clock conversor A/D 
  /* AD0.2~3 */
	//pinsel1 controla as funções da metade superior da porta 0 (de P0.16 a P0.30)
  LPC_PINCON->PINSEL1 &= ~0x000F0000;	/* P0.23~26, A0.0~3, function 01  ~11110000000000000000 */  
  LPC_PINCON->PINSEL1 |= 0x00050000;  //  1010000000000000000
  /* No pull-up no pull-down (function 10) on these ADC pins. */
  LPC_PINCON->PINMODE1 &= ~0x000F0000;
  LPC_PINCON->PINMODE1 |= 0x000A0000;
  LPC_ADC->ADCR = ( 0x01 << 2 ) |  /* SEL=1,select channel 0~7 on ADC0 */
		( ( ( (SystemCoreClock/4)  / adc_Clk ) - 1 ) << 8 ) |  /* CLKDIV = Fpclk / ADC_Clk - 1 */ 
		( 0 << 16 ) | 		/* BURST = 0, no BURST, software controlled */
		( 0 << 17 ) |  		/* CLKS = 0, 11 clocks/10 bits */
		( 1 << 21 ) |  		/* PDN = 1, normal operation */
		( 0 << 24 ) |  		/* START = 0 A/D conversion stops */
		( 0 << 27 );		/* EDGE = 0 (CAP/MAT singal falling,trigger A/D conversion) */ 
  
	LPC_ADC->ADINTEN = 0x00;		/* Disable all interrupts */
  return;
}

/*****************************************************************************
** Function name:		ADCRead
**
** Descriptions:		Read ADC channel
**
** parameters:			Channel number
** Returned value:		Value read, if interrupt driven, return channel #
** 
*****************************************************************************/
int adc_read( char channelNum ) {
  int regVal, ADC_Data;
  /* channel number is 0 through 7 */
  if ( channelNum >= ADC_NUM ) {
		channelNum = 0;		/* reset channel number to 0 */
  }  
  LPC_ADC->ADCR &= 0xFFFFFF00; 
  LPC_ADC->ADCR |= (1 << 24) | (1 << channelNum);					
  /* switch channel,start A/D convert */  
  while ( 1 ) {			/* wait until end of A/D convert */
		regVal = LPC_ADC->ADGDR;
		/* read result of A/D conversion */
		if ( regVal & ADC_DONE ) {
			break;
		}
  }	        
  LPC_ADC->ADCR &= 0xF8FFFFFF;	/* stop ADC now */    
  if ( regVal & ADC_OVERRUN ) {	/* save data when it's not overrun, otherwise, return zero */
	return ( 0 );
  }  
  ADC_Data = ( regVal >> 4 ) & 0xFFF;
  return ( ADC_Data );	/* return A/D conversion value */
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
