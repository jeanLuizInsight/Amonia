/*************************************************************************************************
* Sistema de telemetria por rede GSM/GPRS.
* Arquivo: rs485.c
* Programador: Jean Luiz Zanatta
* Data: 06/08/2014
*************************************************************************************************/
//includes
#include <LPC17xx.h>
#include <string.h>
#include "rs485.h"
#include "uart.h"
#include "misc.h"
#include "tgs2444.h"

//variaveis globais
unsigned short _CRC16;
unsigned short _indx = 0;
unsigned short _jndx = 0;
unsigned char _rs485Adress = RS485_DEFADDRESS;

//função de configuração dos pinos da interafe RS485.
void RS485_io_conf(void) {
	config_output(TX_EN_PIN); 
	config_output(RX_EN_PIN);
	//GPIO_set_dir(2,9,1);  //p2[8]
	//GPIO_set_dir(2,8,1);	//p2[9]
	REN_485();	
	return;
}

//habilita pino de TX e desabilita pino RX no transceiver RS485.
void DE_485(void) {		
	output_high(RX_EN_PIN); //desativa, ativo baixo
	output_high(TX_EN_PIN); //habilita envio pelo drive 485
	//GPIO_set_value(2,9,1);
	//GPIO_set_value(2,8,1);														
}                                               

//habilita pino de RX e desabilita pino TX no transceiver RS485.
void REN_485(void) {	
	while(!_UART1TxEmpty){}//espera término da transmissão
	output_low(TX_EN_PIN); //desativa, ativo alto (desabilita transmissão)
	output_low(RX_EN_PIN); //ativa, ativo baixo   (re-habilita recepção)
 	//GPIO_set_value(2,9,0);
	//GPIO_set_value(2,8,0);
}	

//calcula CRC baseado no polinômio CRC16 reverso, 0xA001.  Modifica: CRC16, UART485BUFFER. 
void Processa_CRC_offline(void) {		
	_CRC16 = 0xFFFF;									
	for (_indx = 0; _indx < _UART1Count - 2; _indx++) {
		_CRC16 ^= _UART1Buffer[_indx];					
   	_jndx = 0;						
   	while (_jndx < 8) {								
       	if ( (_CRC16 & 0x01) ) {							
           	_CRC16  = _CRC16>>1;	
           	_CRC16 ^= 0xA001;		
       	} else {
            _CRC16  = _CRC16>>1;
       	}		
			  _jndx++;				
		}											
	}										
}

//verifica se o CRC está OK. Subtrai o valor recebido de CRC do calculado de CRC. Modifica:	CRC16.
//1 se CRC OK ou 0 se CRC not OK.
char Verifica_CRC(void) {
	if(_UART1Count > 0) {
		Processa_CRC_offline();							//
		if ((_CRC16 - (((unsigned short)_UART1Buffer[_UART1Count - 1])<<8) - ((unsigned short)_UART1Buffer[_UART1Count - 2]) ) == 0 ) {												//
			return 1;	    // CRC check OK
		} else {												
			return 0;		  // CRC not ok
		}    											
	} else {
		return 0;
	}
}						

//geraCRC (código verificador de erro). Gera CRC de 16 bits. Modifica: 	CRC16. 
void Gera_CRC(void) {
	_UART1Count +=  2;					// aloca 2 caracteres para o valor do CRC
 	Processa_CRC_offline();			  // gera CRC
	_UART1Buffer[_UART1Count - 2] = (char)(_CRC16&0x00FF);// separa byte menos significativo e posiciona parte do CRC no buffer
	_UART1Buffer[_UART1Count - 1] = (char)(_CRC16>>8);   	// posiciona o byte mais significativo do CRC no buffer
}

//dados vindos do GSM
void Trata_pacotes(void) {
	//primeiro verifica se CRC ok!
	if(Verifica_CRC()) {	
		//primeiro verifica bytes vindos na Uart depois o comando. GSM envia 4bytes para todos os comandos	
		//16 bits guarda 1byte (1byte = 2bytes na memória) = numeros base 16 hexadecimal
		//trata comando de handshake. 
		if((_UART1Count == 4) && (_UART1Buffer[1] == CMDHANDSHAKE)) {	
			_UART1Count = 2;
			_UART1Buffer[_UART1Count++] = 0x6F;					//posição 2:"o"
			_UART1Buffer[_UART1Count++] = 0x6B;					//posição 3:"k"
		}	
		//trata comando de leitura de NH3.
		else if((_UART1Count == 4) && (_UART1Buffer[1] == CMDLERNH3)) {		
			_UART1Count = 2;
			_UART1Buffer[_UART1Count++] = ((_vlrNH3>>8)&0xFF);       //posição 2: ppm
			_UART1Buffer[_UART1Count++] = (_vlrNH3&0xFF);
		}			
		//responde somente se CRC bateu e se for endereço do dispositivo (e não broadcast)
		
		if(_UART1Buffer[0] == _rs485Adress) {
			delay_us(1750);			
			NVIC_EnableIRQ(UART1_IRQn);		
			Gera_CRC();
			UART_send(RS485PORT,(uint8_t*)_UART1Buffer, _UART1Count);	
			_UART1Count = 0;
		} else {
			NVIC_EnableIRQ(UART1_IRQn);		
			_UART1Count = 0;
		}
	}
	//NVIC_EnableIRQ(UART1_IRQn);		
	//_UART1Count = 0;	
	return;	
}

//envia strings pela interface RS485.
void Send485_string(const char *buffer) {	
	_UART1Count = 0;									              // Zera contagem do buffer da uart.
	memset( (void *)_UART1Buffer, '\0', BUFSIZE);		// Limpa Buffer.
	//DE_485();		
	UART_send(RS485PORT, (uint8_t *)buffer, strlen(buffer));
	//UART_send(1, (uint8_t *)"\r\n", 2);
	//REN_485();	
	return;
}

