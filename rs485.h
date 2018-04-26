/*************************************************************************************************
* Sistema de telemetria por rede GSM/GPRS.
* Arquivo: rs485.h > respons�vel por todos os procedimentos relacionados a comunica��o serial RS-485
* Programador: Jean Luiz Zanatta
* Data: 06/08/2014
*************************************************************************************************/
#ifndef	RS485_H
#define RS485_H

//includes
#include <stdint.h>

//defines
#define RS485PORT 1						 //Porta UART usada na comunicacao RS485
#define TX_EN_PIN ((2<<5) | 9) //Porta de IO do LPC usada como Tx enable (DE do SN65LBC185D, ativo ALTO)
#define RX_EN_PIN ((2<<5) | 8) //Porta de IO do LPC usada como Rx enable (REn do SN65LBC185D, ativo BAIXO)
//endere�o do dispositivo no barramento 485.
#define RS485_BROADCAST  0x00
#define RS485_DEFADDRESS 0x0B    //11 = 0B lab
//#define RS485_DEFADDRESS 0x03  //03 campo
//defines do protocolo de comunica��o
//comandos de status.
#define CMDHANDSHAKE     0x42
//comandos para leitura de NH�.
#define CMDLERNH3        0x64

//vari�veis globais
extern unsigned short _CRC16;
extern unsigned short _indx;
extern unsigned short _jndx;
extern unsigned char _rs485Adress;
extern unsigned char _idDisp;

//prot�tipo de fun��es
void Gera_CRC(void);
char Verifica_CRC(void);
void Processa_CRC_offline(void);
void Trata_pacotes(void);
void RS485_io_conf(void);
void DE_485(void);
void REN_485(void);
void Send485_string(const char *buffer);

#endif
