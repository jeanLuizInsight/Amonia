/******************************************************************************
* Inicializando e setando valor para DAC(digital/analogico)
* Microcontrolador LPC1766
* Programador: Jean Luiz Zanatta				
* Data: 26/02/2014
*******************************************************************************/
//includes
#include "dac.h"
#include <LPC17xx.h>

void dac_init(void) {
	LPC_PINCON->PINSEL1 &= ~0x00300000;
	LPC_PINCON->PINSEL1 |= 0x00200000;
	LPC_DAC->DACR = 0;
	return;
}

//----função seta valor DAC do uC(pega valor já convertido para analógico de 0 a 1023)
void dac_set_value(int val){
	if((val>>10) != 0) { 
		val = 1023; //valor maximo permitido para val
	}
	LPC_DAC->DACR = (val<<6); 
	return;
}






