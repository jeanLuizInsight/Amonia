/*********************************************************************************
* Medidor comcentração amônia.
* wdt.c
* Microcontrolador LPC1766
* Programador: Jean Luiz Zanatta
* Data: 28/02/2014						
**********************************************************************************/
//includes
#include <LPC17xx.h>
#include "wdt.h"

//----função configuração do whatchdog
void wdt_config(char nseg) {	
	//----Configura fonte de clock e trava registrador----
	LPC_WDT->WDCLKSEL  = 0x00;    //seleciona RC oscilattor interno para o clock do whatchdog (default)
	LPC_WDT->WDCLKSEL |= WDLOCK;	//seta WDLOCK = 1 (os bits deste registro não podem ser mais modificados)
	//----Configura constante do watchdog----
	LPC_WDT->WDTC	= nseg * WDTC_1SEG;	 //valor do registrado definido por 1000000 * o parametro da função(10)
	//----Habilita watchdog e reset----
	LPC_WDT->WDMOD = WDEN | WDRESET;	//(bit 0 = 1 roda o timer do whatchdog, bit 1 = 1 se estourar tempo limite do timer whathdog reseta o chip) 
	//----Alimenta watchdog----
	LPC_WDT->WDFEED = 0xAA;
	LPC_WDT->WDFEED = 0x55;	
	return;
}
//----alimenta whatchdog
void wdt_feed() {
	//escrevendo 0xaa seguido por 0x55 este registro recarrega o timer do whatcdog com o valor contido em WDTC
	LPC_WDT->WDFEED = 0xAA; 
	LPC_WDT->WDFEED = 0x55;	
}




