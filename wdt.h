/*********************************************************************************
* Medidor comcentração amônia.
* wdt.h					
* Microcontrolador LPC1766
* Programador: Jean Luiz Zanatta
* Data: 28/02/2014						
**********************************************************************************/
//defines
#define		WDLOCK		0x80000000
#define		WDEN		  0x01
#define		WDRESET		0x02
#define		WDTOF		  0x04
#define   WDTC_1SEG 1000000
//prototipo de funções
extern void wdt_config(char nseg);
void wdt_feed(void);
// // Valor para 1 segundo antes do estouro
// #define		WDCONSTANT	0x98968
// Estouro em Twdclk * 4 * WDTC
// Com IRC como fonte do WDCLK (4MHz), para 1 segundo temos:
// void WdtInit(void);
//void WDT_Feed(void);
