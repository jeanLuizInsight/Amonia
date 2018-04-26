/******************************************************************************
* Protótipo medição concentração de amônia
* Microcontrolador LPC1766
* Programador: Jean Luiz Zanatta				
* Data: 26/02/2014
*******************************************************************************/
// Includes
#include <LPC17xx.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "wdt.h"
#include "uart.h"
#include "adc.h"
#include "dac.h"
#include "tgs2444.h"
#include "lcd.h"
#include "rs485.h"

//variaveis de controle LCD
//char _flagLCDcheck = 1;
//char _flagLCD = 1;
//char _timerdivLCD = 10;
// Interrupção executada a cada T_interrupt(100) ms que atualiza o LCD.
/*void RIT_IRQHandler (void) { 
  //unsigned int i_int = 0;	 //Variável temporária usada principalmente para contagem em loops, exclusiva para uso dentro do tratador de interrupção do Timer0	
	LPC_RIT->RICTRL |= 0x01;   //escrevendo 1 no bit 1 de RICTRL
	if(--_timerdivLCD == 0){
		_timerdivLCD = 10;
		_flagLCD = 1;
	}
	_flagLCDcheck = 1;	
	return;
} */
/**
* @brief 	  Função Main - função principal do programa	
* @details  chama todas as funções do programa 
*/
int main(void) {	
	//----Inicializa sistema do uC----
	SystemInit();
	SystemCoreClockUpdate();	
	
	//----Inicializa configuração do watchdog timer----
	wdt_config(10); 
	
	//----Configura interrupção temporal (Repetitive Interrupt Timer - RIT) LCD----
	//timer_interrupt_config(); 	
	
	//----Configura timer usado em delays----
	//delay_config();
	Timer0Init();
	
	//configura pinos sensor
	tgs2444_config();
 	
	//-----inicializacao do LCD----	
	lcd_init();      
	show_splash_screen();	    //escreve empresa
	lcd_printall();           //atualiza LCD
	delay_ms(1000);
	
	//----inicialização UART----
	UART_init(RS485PORT, 57600);	
	
	//----configura pinos da interface RS485----
	RS485_io_conf();
	
	//delay_ms(200);
	//----inicialização do ADC----
	adc_init(ADC_CLK); 
	//----inicialização do DAC----
	//dac_init();	
	//----habilita interrupção temporal (Repetitive Interrupt Timer - RIT)----
	//timer_interrupt_enable(); //misc.c 
	//----Limpa bits válidos de RSID (RSID contém um bit p/ cada fonte de reset)----
	LPC_SC->RSID |= 0x0F; //seta bit 0, 1, 2, 3 = 1111 (POR, EXTR, WDTR, BODR)
	
	//----função seta valor para prescaler register, reseta e habilita timer counter
	zera_contagem();			
	
	////////////////////////////////
	// Loop principal do programa //
	////////////////////////////////
	while(1) {		
		//----alimenta whatchdog----
		wdt_feed();
		
		if(LPC_TIM2->TC >= T_VHH) { //timer counter(incrementado a cada PR+1 pulso de clock)>=250000us tempo total (ciclo para o sensor funcionar)
			//----leitura das tensões
			le_tensoes();		
			//----armazena nas variaveis resistencia calculada a partir de duas formulas 1 e 2----
			//_rs1 = calcula_rs(1); //_rs1:		
			_rs2 = calcula_rs(2); //_rs2:
			//----armazena nas variaveis a concentração obtida através da resistencia calculada----
			_vlrNH3 = NH3_conc(_rs2);
			//_vlr2NH3 = NH3_conc(_rs1);
			//----NH3_conc(rs): retorna a concentração real  da amônia(ppm)---- 
			//----NH3_to_DAC: converte novamente para um valor analógico----      
		}		
		
		//----envia dados pela RS-485----
		Trata_pacotes();      //verifica pedido do mestre(Micro GSM)
		
		//------------------------------------------ATUALIZAÇÃO DO LCD--------------------------------------------------------
		refresh_lcd();
		/*
		if (_flagLCD) {
			_flagLCD = 0;                                      //seta variavel para falso
			refresh_lcd();                                     //atualiza display
			_flagLCDcheck = 1;                                 //seta variavel para verdadeiro
		}
		if (_flagLCDcheck) {
			if(!lcd_checktext()) {                             //se a função retornar 0 (falso)			
				lcd_printall();
				if(!lcd_checktext()) {
					lcd_init();
					lcd_printall();
				}
			}
			_flagLCDcheck = 0;                                 //seta variavel para falso
		}*/
	}	//fim loop principal	
} //fim função principal




