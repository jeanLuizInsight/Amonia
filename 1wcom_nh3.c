/*
Este barramento 1-wire é utilizado apenas como uma forma de comunicar o sensor com o módulo master a distância
no protótipo 
*/
//includes
#include <LPC17xx.h>
#include "1wcom_nh3.h"
#include "misc.h"
#include "tgs2444.h"
//variaveis 
char _codes[MAX_NUM_DE_SENSORES][8];	// codigos dos sensores internos
char _codeSensext[8]; // codigo do sensor externo
signed int _temps[MAX_NUM_DE_SENSORES];
char _umidades[MAX_NUM_DE_SENSORES];
int _resposta[9] = { 0,0,0,0,0,0,0,0,0 };
//char num_sensores_total = 0;
char _numSensoresInternos = 0;
char _numSensoresExternos = 0;
char _numSensUmidade = 0;
char _sensorIndex = 0;
char _extSensorIndex = 0xFF;
int _vccSensor = 0; // [mV]
char _presenca[2];
char _idBit;
char _cmpIdBit;
int _idBitNumber;
char _noDeviceFlag;
char _lastDeviceFlag;
int _lastDiscrepancy;
int _lastZero;
char _searchDirection;
int _n1;
int _n2;

void init_1wire(void) { 
	LPC_PINCON->PINMODE_OD1 |= (1<<(PULLUP & 0x1F));
	config_output(PULLUP);
	config_output(PULLDOWN);
	SPUoff();
	SPDoff();
	return;
}

void w_reset(void) {
	_presenca[0] = 0x01;
	_presenca[1] = 0x01;
	SPUoff();
	SPD();
	delay_us(500);
	SPDoff();
	delay_us(30);
	_presenca[0] = input(LEITURA_1W);
	delay_us(30);
	_presenca[1] = input(LEITURA_1W);
	delay_us(450);
	return;
}

void write1(void) {
	SPUoff();
	SPD();
	delay_us(6);
	SPDoff();      
	SPU(); 		// hmmm, só é usado 1 vez
	delay_us(64);
	SPUoff();
	return;
}

void write0(void) {
	SPUoff();
	SPD();
	delay_us(60);
	SPDoff();
	delay_us(10);
	return;
}

void write_byte(int cmd) {
   char ind;
   for(ind = 0;ind < 8;ind++) {
      if(bit_test(cmd, ind) == 1) { 
				write1();  
			} else { 
				write0(); 
			}
   }
   delay_us(10);
	 return;
   //delay_us(5);
}

int read_bit(void) {
   SPD();
	 //delay_us(3);
	 delay_us(5);
   SPDoff();
   //delay_us(13);
	 delay_us(9);
   return input(LEITURA_1W);                              //retorna o valor no pino especificado 0 ou 1
}

void read_byte(char n) {
   char ind1, ind2;
   SPUoff();
   for (ind1 = 0;ind1 < n;ind1++) {
      _resposta[ind1] = 0;
      for(ind2 = 0;ind2 < 8;ind2++)   {
/*         if (ReadBit() == 0) { bit_clear(&resposta[j],k); }
         else { bit_set(&resposta[j],k); }*/
				_resposta[ind1] |= read_bit()<<ind2; 
        delay_us(60);
      }
   }
	 return;
}

void reset_and_match(void) {  
   char aux1;
   w_reset();
   write_byte(0x55);
   for ( aux1 = 0 ;aux1 < 8;aux1++) { 
      write_byte(_codes[_sensorIndex][aux1]);     
   }
	 return;
}

void reset_and_match1_sens(void) {  
	w_reset();
	write_byte(0xCC); //204
	return;
}

//int shift_reg;
void search_rom_step(void) {
   w_reset();
   write_byte(0xF0);
   _lastZero = 0;
   _noDeviceFlag = 0;
   for ( _idBitNumber = 1;_idBitNumber < 65;_idBitNumber++ ) {
      _n1 = (_idBitNumber - 1) / 8;
      _n2 = (_idBitNumber - 1) % 8;
      _idBit = read_bit();
      delay_us(50);
      _cmpIdBit = read_bit();
      delay_us(50);
			delay_us(500);
			//putchar(0,id_bit);
			//putchar(0,cmp_id_bit);
      if ((_idBit == 1) && (_cmpIdBit == 1) ) { 
				_noDeviceFlag = 1; 
				break; 
			}
      if ((_idBit == 0) && (_cmpIdBit == 0) ) {
         if (_idBitNumber == _lastDiscrepancy ) {
            _searchDirection = 1;
         } else {
            if (_idBitNumber > _lastDiscrepancy ) {
               _searchDirection = 0;
            } else {
               _searchDirection = bit_test(_resposta[_n1],_n2);
            }
         }
         if (_searchDirection == 0) {
            _lastZero = _idBitNumber;
         }
      } else {
         _searchDirection = _idBit;
      }
      if (_searchDirection == 0) {
         write0();
         bit_clear(&_resposta[_n1],_n2);
      } else {
         write1();
         bit_set(&_resposta[_n1],_n2);
      }
   }
   _lastDiscrepancy = _lastZero;
   if (_lastDiscrepancy == 0 ) {
			_lastDeviceFlag = 1;
	 }
	 return;
}
// void GravaTipoSensor(char tipo_do_sensor){
// 	resetandmatch();
// 	WriteByte(0x4E);	 	// Write Scratchpad
//     WriteByte(0x03);	 	// Scratchpad's page 3
// 	WriteByte(tipo_do_sensor);
//     resetandmatch();
// 	WriteByte(0x48);   // Grava na EEPROM
// 	WriteByte(0x03);
// 	delay_ms(10);
// }
void grava_tipo_sens(char sind, char tipo) {
	if (_codes[sind][0] == 0x26) {
		_sensorIndex = sind;
		reset_and_match();
		write_byte(0x4E);	 // Write Scratchpad
		write_byte(0x03);	 // Scratchpad's page 3		
		if(tipo=='e') {
			write_byte('e');
			write_byte('x');
			write_byte('t');
		} else if(tipo=='i') {
			write_byte(0);
			write_byte(0);
			write_byte(0);
		}
		reset_and_match();
		write_byte(0x48);   // Grava na EEPROM
		write_byte(0x03);
		delay_ms(10);
	}
	return;
}
char le_tipo_sensor(char sensind) {
	_sensorIndex = sensind;
	reset_and_match();
	write_byte(0xB8); 	// Recall memory (tranfers data from EEPROM to Scratchpad)
	write_byte(0x03);	  // Page number
	reset_and_match();
	write_byte(0xBE); 	// Read Scratchpad
	write_byte(0x03);   // Page number
	read_byte(1);
	return(_resposta[0]);
}

char verifica_crc(char codelength) {
   char shift_reg = 0;
   char fb;
   for(_i = 1;_i < (codelength - 1);_i++) {
   		if(_resposta[_i]!=0) {
				break; 
			}	
   }
   if(_i == (codelength - 1)) { 
			return(0); // Se os dados forem todos = 0 retorna 0 
	 } else {  
	    for (_i = 0;_i < codelength - 1;_i++ ) {
					for (_j = 0;_j < 8;_j++) {
						fb = (shift_reg & 0x01) ^ ((_resposta[_i]>>_j)&0x01);
						shift_reg = shift_reg>>1;
						if (fb == 1) { 
							shift_reg = shift_reg ^ 0x8c; 
						}
	   		}
	   }
	   if (shift_reg == _resposta[codelength-1]) { 
				return 1; 
		 } else { 
				return 0; 
		 }
   }
}		
void procura_sensores(void) {
	char nsens = 0;	
	_numSensoresInternos = 0;
	_numSensoresExternos = 0;
	_numSensUmidade = 0;	
	_extSensorIndex = 0xFF;
	_lastDiscrepancy = 0;   
	_lastDeviceFlag = 0;
	for (nsens = 0; nsens < (MAX_NUM_DE_SENSORES); nsens++) {
      search_rom_step(); 
      if (!_noDeviceFlag && verifica_crc(8)) {
         for (_i = 0;_i < 8;_i++) {	
					 _codes[nsens][_i] = _resposta[_i]; 
				 }
				 if(_codes[nsens][0]==0x26) { 
						_numSensUmidade++;
				 }
      }	else {
				 break; 
			}
      if ( _lastDeviceFlag ) { 
				 nsens++;
				 break; 
			}
   }
   for(_j = 0;_j < nsens;_j++) {
   		if(le_tipo_sensor(_j)=='e') {			
			_numSensoresExternos = 1; 
			_extSensorIndex = _j;
			break;
		}
   }     
   _numSensoresInternos = nsens - _numSensoresExternos;
	 return;
}

void convert_temp_DS18B20(void) {
	reset_and_match();
	write_byte(0x4E);
	write_byte(0x7F);
	write_byte(0x94);
	write_byte(0x3F);	// 0x3F = 10 bits, 5F = 11 bits, 7F = 12 bits
	reset_and_match();
	write_byte(0x44);
	return;
//    SPU();
//    delay_ms(190); //////// verificar isso
//    SPUoff();
}

void le_DS18B20(void) {
	reset_and_match();
	write_byte(0xBE);
	read_byte(9); 
	return;
}
//void levoltB20() {
//   resetandmatch(); 
//   WriteByte(0xB4);
//   ReadByte(1);
//   resposta[1] = 0;
//}

void convert_temp_DS2438(void) {
	reset_and_match();
	write_byte(0x44);
	return;
}

void convert_volt_DS2438(char flagvdd) {
   reset_and_match();
   write_byte(0x4E);	 				// Write Scratchpad
   write_byte(0x00);	 				// Scratchpad's page 0
   if (flagvdd) { 
			write_byte(0x08); // Selects Vdd pin as A/D input
	 } else { 
			write_byte(0x00); // Selects Vad pin as A/D input
	 }	
   reset_and_match();
   write_byte(0xB4);					// Convert V command
	 return;
}

void le_DS2438(void) {
	reset_and_match();
	write_byte(0xB8); // Recall memory (tranfers data from EEPROM to Scratchpad)
	write_byte(0x00);	// Page 0
	reset_and_match();
	write_byte(0xBE);	// Read Scratchpad
	write_byte(0x00);	// Page 0
	read_byte(9);
	return;
}
					  
char calcula_umidade(void) {
	int umidtemp = 0xFF;	
// 	UARTputchar(0,vcc_sensor>>8);
// 	UARTputchar(0,vcc_sensor>>0);
// 	UARTputchar(0,resposta[4]);
// 	UARTputchar(0,resposta[3]);
	
	if((_vccSensor<270)||(_vccSensor>600)) { 
			return(0xFF); // Retorna umidade inválida
	} 
	umidtemp = ((_resposta[4]*256) + _resposta[3]); // tensão [centésimos de V]
	umidtemp = (((15723*umidtemp) / _vccSensor-2382) / 100);  // Cálculo da umidade (sem compensação por temperatura)
	
	if ((umidtemp > 100) && (umidtemp <= 120) ) { 
			return(100); // <-- satura umidade em 100%
	} else if (umidtemp>=120) { 
			return(0xFF); 
	} else { 
			return(umidtemp); 
	}
}

//função de leitura do sensor
void le_sensor(void) {  
	if(_codes[_sensorIndex][0] == 0x26) {
		convert_volt_DS2438(1);
	  delay_ms(15);
		le_DS2438();
		_vccSensor = ((_resposta[4] * 256 + _resposta[3])); // [mV]
		delay_ms(1);
		convert_volt_DS2438(0);
	  delay_ms(15);
		convert_temp_DS2438();
    delay_ms(20);	  
    le_DS2438();
	} else if (_codes[_sensorIndex][0] == 0x28) {
		convert_temp_DS18B20();		
		delay_ms(12);	  
		le_DS18B20();
	}  
	return;
}
//função que retorna o 3º valor da concentração de amônia NH3_3 no display
char ad_val_to_NH3(void) {
	 int VDD_val = 0;
	 int VAD_val = 0;
	 int ADval = 0;
	 char concCalc = 0;	
	 //Converte tensão da alimentação
	 reset_and_match1_sens();
	 write_byte(0x4E); //78	 			               	// Write Scratchpad
	 write_byte(0x00); //0	 				              // Scratchpad's page 0
	 write_byte(0x08); //8
	 reset_and_match1_sens();
	 write_byte(0xB4); //180				              // Convert V command
	 delay_ms(15);		
	 //Le DS2438
	 reset_and_match1_sens();
   write_byte(0xB8); //184                       // Recall memory (tranfers data from EEPROM to Scratchpad)
   write_byte(0x00); //0                         // Page 0
   reset_and_match1_sens();
   write_byte(0xBE); //190	                     // Read Scratchpad
   write_byte(0x00); //0	                       // Page 0
   read_byte(9);	   //9
	 VDD_val = _resposta[4] * 256 + _resposta[3];	 
	 // Converte tensão do AD
	 reset_and_match1_sens();
   write_byte(0x4E); //78	  				               // Write Scratchpad
   write_byte(0x00); //0	  				               // Scratchpad's page 0
	 write_byte(0x00); 
   reset_and_match1_sens();
   write_byte(0xB4); //180					               // Convert V command	 
	 delay_ms(15);		
	 // Le DS2438
	 reset_and_match1_sens();
   write_byte(0xB8); //184                          // Recall memory (tranfers data from EEPROM to Scratchpad)
   write_byte(0x00); //	                            // Page 0
   reset_and_match1_sens();
   write_byte(0xBE); //190                        	// Read Scratchpad
   write_byte(0x00);	                              // Page 0
   read_byte(9); //9
	 VAD_val = _resposta[4] * 256 + _resposta[3];	 
	 ADval = (VAD_val * 1024) / VDD_val;
	 if((VAD_val == VDD_val) || (VDD_val == 0) || (VAD_val == 0) || (VDD_val > 520)) { 
			return(0xFF); 
   }	 
	 if((ADval > VAL_DAC_MIN) && (ADval < VAL_DAC_MAX)) {
			concCalc = ((ADval - VAL_DAC_MIN) * 100) / (VAL_DAC_MAX - VAL_DAC_MIN);
	 } else if(ADval <= VAL_DAC_MIN) {
		  concCalc = 0;
	 } else {
		  concCalc = 100;
	 }	 
	 return(concCalc);
}
	 




