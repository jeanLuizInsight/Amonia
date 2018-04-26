/*************************************************************************************************
* Sistema de telemetria por rede GSM/GPRS.
* Arquivo: rs485.h > respons�vel por todos os procedimentos relacionados ao m�dulo GSM/GPRS SIM900
* Programador: Jean Luiz Zanatta
* Data: 07/08/2014
*************************************************************************************************/
#ifndef	SIM900D_H
#define SIM900D_H

//includes
#include <LPC17xx.h>
#include <stdint.h>
#include "uart.h"

//defines:
#define POWERPIN	0,18    //pino power
#define RESETPIN	0,17		//pino reset
#define DCDPIN		2,3			//pino dcd
#define UARTGSM		1			  //sele��o da UART comunica��o com modulo GSM.
#define BAUDGSM		57600		//baudrate para comunica��o com modulo GSM.

#if UARTGSM
	#define UARTGSMBUFFER	UART1Buffer
	#define UARTGSMCOUNT	UART1Count
#else
	#define UARTGSMBUFFER	UART0Buffer
	#define UARTGSMCOUNT	UART0Count
#endif

//vari�veis globais
extern unsigned short _respPoint;

//prototipo das fun��es
void sim900_io_conf(void);
int sim900_Power_on(void);
int sim900_warm_up(void);
int	sim900_get_CSQ(void);
int	sim900_CREG_status(void);
int resp_search(const char* resp);
int	sim900_send_cmd(const char* cmd, int timeOut);

#endif
