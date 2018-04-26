/*************************************************************************************************
* Sistema de telemetria por rede GSM/GPRS.
* Arquivo: rs485.c > responsável pela implementação das funções de sim900.h
* Programador: Jean Luiz Zanatta
* Data: 07/08/2014
*************************************************************************************************/

//includes
#include <LPC17xx.h>
#include <string.h>
#include "sim900.h"
#include "uart.h" 
#include "misc.h"
#include "lcd.h"

unsigned short _resPoint = 0;

//configura pinos de IO para interface com o modulo GSM.
void sim900_io_config(void) {
	GPIO_set_dir(DCDPIN, 0);
	GPIO_set_dir(POWERPIN, 1);
	GPIO_set_dir(RESETPIN, 1);
	GPIO_set_value(RESETPIN, 0);
	GPIO_set_value(POWERPIN, 0);
	return;
}

//liga modulo GSM.
int Sim900PwOn(void) {
	char n, timeOut;				
	for(n=0;n<5;n++) {				
		//se modulo estiver desligado, ligue.
	  if(!GPIO_get_value(DCDPIN)) { 
			GPIO_set_value(POWERPIN,1);
	    delay_ms(1500);
	    GPIO_set_value(POWERPIN,0);
	    delay_ms(2000);
	  } else { //se modulo estiver ligado, reinicie.
	    GPIO_set_value(RESETPIN,1);
	    delay_ms(500);
	    GPIO_set_value(RESETPIN,0);
	    delay_ms(2000);
	  }		
		//envia o comando AT duas vezes para reconhecer o baud rate.
		Sim900SendCmd("AT",2000);
		Sim900SendCmd("AT",2000);

		if(!RespSearch("OK")) // Procura pela resposta do comando AT
		{						
			TimeOut = 10;			// Envia o comando para desabilitar o 
			while(--TimeOut)		// eco dos comandos AT.
			{		
				Sim900SendCmd("ATE0",2000);
			
				if(!RespSearch("ATE0\r\n\r\nOK"))
				{				
					if(Sim900SendCmd("AT",2000)==6)
						return(0);	
				}						
			}				
		}								
	}
	
	return (-1);						// Se não conseguir resposta retorna falha.
}

//-----------------------------------------------------------------------------
// Envia comandos para o modulo GSM.

int Sim900SendCmd(const char* Cmd, int TimeOut)
{
	int RetNum;
	
	UARTGSMCOUNT = 0;									// Zera contagem do buffer da uart.
	memset( (void *)UARTGSMBUFFER, '\0', BUFSIZE );		// Limpa Buffer.
		
	UARTSend(UARTGSM,(uint8_t *)Cmd, strlen(Cmd));		// Envia string de comando.	
	UARTSend(UARTGSM,(uint8_t *)"\r\n", 2);				// Envia CR(move p/ inicio da linha) LF(nova linha) apos o comando.
	
	RetNum = UARTReceivingWait(UARTGSM, TimeOut);		// Aguarda resposta ou time out de 1s.		
	
	return(RetNum);										// Retorna o numero de bytes recebido.
}

//-----------------------------------------------------------------------------
// Inicializa modulo GSM.

int sim900WarmUp(void)
{
	int8_t TimeOut, ret;
	
	//------------------------------
	// Liga modulo GSM.
	
	if(Sim900PwOn())	
		return(-1);
		
	//------------------------------
	// Verifica nivel de sinal.		
	TimeOut = 20;
	while(--TimeOut)
	{			
		ret = sim900GetCSQ();
		
		LcdGotoXy(1,4);
		LcdPuts("CSQ: ");
		LcdPrintNum(ret,2);
		
		if( ret >= 10)
			break;		
		
		delay_ms(1000);
	}		
	
	if(!TimeOut)
		return(-2);
	
	//-------------------------------
	// Define o tipo de acesso a rede.
	
	TimeOut = 5;
	while(--TimeOut)
	{	
			
		if(Sim900SendCmd("AT+CREG=0",1000)==6) //CREG:
			break;		
	}
	
	if(!TimeOut)
		return(-3);
	
	//-----------------------------
	// Aguarda o registro na rede.
	
	TimeOut = 20;
	while(--TimeOut)
	{		
		ret = sim900CREGStatus();
		
		LcdGotoXy(1,4);
		LcdPuts("CREG: ");
		LcdPrintNum(ret,2);
				
		if(ret==01)
			break;
		
		delay_ms(1000);
	}
	
	if(!TimeOut)
		return(-4);
	
	return(0);
}

//-----------------------------------------------------------------------------
// Retorna nivel de sinal GSM.

int sim900GetCSQ(void)
{
	uint8_t TimeOut = 4, CSQ = 0;
	
	while(--TimeOut)
	{	
		if(Sim900SendCmd("AT+CSQ",1000)>7)
		{
			if(UARTGSMBUFFER[UARTGSMCOUNT-6]<0x30 || UARTGSMBUFFER[UARTGSMCOUNT-6]>0x39)
			{
				CSQ = ((UARTGSMBUFFER[UARTGSMCOUNT-5]-0x30));
				return(CSQ);
			}
			else
			{			
				CSQ =  ((UARTGSMBUFFER[UARTGSMCOUNT-6]-0x30)*10);
				CSQ += ((UARTGSMBUFFER[UARTGSMCOUNT-5]-0x30));
				return(CSQ);
			}
		}		
	}
	return(-1);
}

//-----------------------------------------------------------------------------
// Retorna status de registro na rede GSM.

int sim900CREGStatus(void)
{
	uint8_t TimeOut = 4, STS = 0;
	
	while(--TimeOut)
	{	
		if(Sim900SendCmd("AT+CREG?",1000)==14)
		{			
			STS =  ((UARTGSMBUFFER[UARTGSMCOUNT-5]-0x30)*10);
			STS += ((UARTGSMBUFFER[UARTGSMCOUNT-3]-0x30));
						
			return(STS);
		}
	}
	return(-1);
}

//-----------------------------------------------------------------------------
// Procura uma string na resposta do modulo.
// Parametros.
// Resp: Resposta a ser esperada do modulo.

int RespSearch(const char* resp)
{
	int RespLen, n, b;
	char compflag = 0;
		
	RespLen = strlen(resp);
	
	n=0;
	b=0;
	
	while( (n<UARTGSMCOUNT)	&& 	(RespLen) )
	{
		if(resp[b] == UARTGSMBUFFER[n] && !compflag)
			compflag = 1;
		
		if(resp[b] != UARTGSMBUFFER[n] && compflag)
		{
			b = 0;
			compflag = 0;			
			RespLen = strlen(resp);			
		}
		
		if(resp[b] == UARTGSMBUFFER[n] && compflag)			
			RespLen--,b++;
			
		n++;
	}

	RespPoint = n;
	
	return(RespLen);
}