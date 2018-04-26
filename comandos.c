/*****************************************************************************
 					COMANDOS UTILIZADOS (SEQUENCIAS)

******************************************************************************/
//includes
#include <LPC17xx.h>
#include "comandos.h"
#include "uart.h"
#include "adc.h"
#include "tgs2444.h"
#include "misc.h"
#include "pwm.h"
#include "dac.h"
//#include "1wcom_nh3.h"

void processa_pacote(char porta) {	
// 	int raddr = 0;
// 	int raddr2 = 0;
// 	char auxchar = 0;
// 	static int rx_msg_len = 0;
// 	char comando_conhecido = 0;
	
	uart_put_char(0,_UARTbuffer[0][0]);
		
	if(_UARTbuffer[0][0] == '2'){
		_i = adc_read(2);
		uart_put_char(0,_i>>8);		
		uart_put_char(0,_i>>0);		
	}
	if(_UARTbuffer[0][0] == '3'){
			LPC_TIM2->PR  = ((SystemCoreClock / 4) / 1000000) - 1; // contador de microssegundos
			LPC_TIM2->TCR = 0x02; // Timer Counter reset
			LPC_TIM2->TCR = 0x01; // Timer Counter Enable
			_i = 0;
			_i += adc_read(1);
			_t1 = LPC_TIM2->TC;
			uart_put_char(0,_t1>>24);
			uart_put_char(0,_t1>>16);
			uart_put_char(0,_t1>>8);
			uart_put_char(0,_t1>>0);
		
			uart_put_char(0,_i>>8);		
			uart_put_char(0,_i>>0);		
	}
	if(_UARTbuffer[0][0] == 'r'){
		le_tensoes();
		_i = calcula_rs(1);
		uart_put_char(0,_i>>8);
		uart_put_char(0,_i>>0);
	}
	if(_UARTbuffer[0][0] == 'a'){
		toggle_pin(_pinosSinalizacao[0]);
		uart_put_char(0,input(_pinosSinalizacao[0]));
	}
	if(_UARTbuffer[0][0] == 'b'){
		toggle_pin(_pinosSinalizacao[1]);
		uart_put_char(0,input(_pinosSinalizacao[1]));
	}
// 	if(UARTbuffer[0][0] == 'r'){
// 		Nad1 = ADCRead(2);
// 		Nad2 = ADCRead(1);
// 		Rs = (10000*Nad2)/(Nad1-Nad2);
// 		UARTputchar(0,Rs>>16);		
// 		UARTputchar(0,Rs>>8);		
// 		UARTputchar(0,Rs>>0);		
// 	}
	if(_UARTbuffer[0][0] == 'c'){
		_nad1 = adc_read(2);
		_nad2 = adc_read(1);
		_nom = ((long double)(Rin2 + Rpd2) * (long double)Rl * (long double)_nad2);
		_den = ((long double)(Rin2 + Rpd2) * (long double)(_nad1 - _nad2) - (long double)Rl * _nad2);
		_rs = _nom / _den;
		uart_put_char(0,_rs>>24);		
		uart_put_char(0,_rs>>16);		
		uart_put_char(0,_rs>>8);		
		uart_put_char(0,_rs>>0);		
	}
	if(_UARTbuffer[0][0] == 'm'){
		output_high(_pinosSinalizacao[0]);
		delay_ms(4);
		_nad1 = adc_read(2); _nad2 = adc_read(1); 
		_nad1 += adc_read(2); _nad2 += adc_read(1); 
		_nad1 += adc_read(2); _nad2 += adc_read(1); 
		_nad1 += adc_read(2); _nad2 += adc_read(1);
		output_low(_pinosSinalizacao[0]);
		if(_UARTbuffer[0][1] == '1'){
			_rs = (10000 * _nad2) / (_nad1 - _nad2);
		}
		else if(_UARTbuffer[0][1] == '2'){
			_nom = ((long double)(Rin2 + Rpd2) * (long double)Rl * (long double)_nad2);
			_den = ((long double)(Rin2 + Rpd2) * (long double)(_nad1 - _nad2) - (long double)Rl * _nad2);
			_rs = _nom / _den;
		}
		uart_put_char(0,_rs>>16);		
		uart_put_char(0,_rs>>8);		
		uart_put_char(0,_rs>>0);		
	}	
	if(_UARTbuffer[0][0] == 'p'){
		pwm_set_dutycycle(5,(_UARTbuffer[0][1] - 0x30) * 10);
		pwm_set_dutycycle(3,(_UARTbuffer[0][1] - 0x30) * 10);
	}
	if(_UARTbuffer[0][0] == 'd'){
		dac_set_value((_UARTbuffer[0][1] - 0x30) * 100);
	}
/*----comandos para comunicação 1-wire----
	if(_UARTbuffer[0][0] == 's'){
		procura_sensores();
		uart_put_char(0,_numSensoresInternos);
		uart_put_char(0,_numSensoresExternos);
	}
	if(_UARTbuffer[0][0] == 'v'){
		// Converte tensão
	 reset_and_match1_sens();
   write_byte(0x4E);	 				// Write Scratchpad
   write_byte(0x00);	 				// Scratchpad's page 0
//    if (flagvdd) { 
			write_byte(0x08); 
// 		}// Selects Vdd pin as A/D input
//    else { 
// 		WriteByte(0x00); 
// 	 }		// Selects Vad pin as A/D input
   reset_and_match1_sens();
   write_byte(0xB4);				// Convert V command		
		delay_ms(15);		
		// Le DS2438
		reset_and_match1_sens();
   write_byte(0xB8); // Recall memory (tranfers data from EEPROM to Scratchpad)
   write_byte(0x00);	// Page 0
   reset_and_match1_sens();
   write_byte(0xBE);	// Read Scratchpad
   write_byte(0x00);	// Page 0
   read_byte(9);	 
	 uart_put_char(0,_resposta[4]);
	 uart_put_char(0,_resposta[3]);	 
	 // Converte tensão
	 reset_and_match1_sens();
   write_byte(0x4E);	 				// Write Scratchpad
   write_byte(0x00);	 				// Scratchpad's page 0
//    if (flagvdd) { 
// 			WriteByte(0x08); 
// 		}// Selects Vdd pin as A/D input
//    else { 
		write_byte(0x00); 
// 	 }		// Selects Vad pin as A/D input
   reset_and_match1_sens();
   write_byte(0xB4);					// Convert V command	 
	 delay_ms(15);		
	 //Le DS2438
	 reset_and_match1_sens();
   write_byte(0xB8); // Recall memory (tranfers data from EEPROM to Scratchpad)
   write_byte(0x00);	// Page 0
   reset_and_match1_sens();
   write_byte(0xBE);	// Read Scratchpad
   write_byte(0x00);	// Page 0
   read_byte(9);	 
	 uart_put_char(0,_resposta[4]);
	 uart_put_char(0,_resposta[3]);
	}
	if(_UARTbuffer[0][0] == 'x'){
		uart_put_char(0,ad_val_to_NH3());
	}
	_bidx[porta] = 0;
------------------------------------- */	
}



