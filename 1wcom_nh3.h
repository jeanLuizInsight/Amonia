/******************************************************************************
* Comunicação 1-W
* Microcontrolador LPC1766
* Programador: Jean Luiz Zanatta				
* Data: 26/02/2014
*******************************************************************************/
#ifndef __1WCOMNH3_H 
#define __1WCOMNH3_H

//defines
#define LEITURA_1W ((1<<5)|26) 
#define PULLUP 	   ((1<<5)|25)	
#define PULLDOWN 	 ((1<<5)|27) 				 
#define SPU()    output_low(PULLUP)       // Strong Pull-Up, ativo BAIXO
#define SPUoff() output_high(PULLUP)
#define SPD()    output_high(PULLDOWN)     // Strong Pull-Down, ativo ALTO
#define SPDoff() output_low(PULLDOWN)
#define MAX_NUM_DE_SENSORES 8
#define V_MIN 1500 // mV
#define V_MAX 3200 // mV
#define VAL_DAC_MIN ((V_MIN * 1024) / 3300)
#define VAL_DAC_MAX ((V_MAX * 1024) / 3300)

//prototipo funções
void init_1wire(void);
void w_reset(void);
void write1(void);
void write0(void);
void write_byte(int cmd);
int read_bit(void);
void read_byte(char n);
void reset_and_match(void);
void reset_and_match1_sens(void);
void search_rom_step(void);
void grava_tipo_sens(char sind, char tipo);
char le_tipo_sensor(char sensind);
char verifica_crc(char codelength);
void procura_sensores(void);
void convert_temp_DS18B20(void);
void le_DS18B20(void);
void convert_temp_DS2438(void);
void convert_volt_DS2438(char flagvdd);
void le_DS2438(void);
char calcula_umidade(void);
void le_sensor(void);
extern char ad_val_to_NH3(void);
//variaveis globais
extern char _codes[MAX_NUM_DE_SENSORES][8];	// codigos dos sensores internos
extern char _codeSensext[8]; // codigo do sensor externo
extern signed int _temps[MAX_NUM_DE_SENSORES];
extern char _umidades[MAX_NUM_DE_SENSORES];
extern int _resposta[9];
//char num_sensores_total = 0;
extern char _numSensoresInternos;
extern char _numSensoresExternos;
extern char _numSensUmidade;
extern char _sensorIndex;
extern char _extSensorIndex;
extern int _vccSensor; // [mV]
extern char _presenca[2];
extern char _idBit;
extern char _cmpIdBit;
extern int _idBitNumber;
extern char _noDeviceFlag;
extern char _lastDeviceFlag;
extern int _lastDiscrepancy;
extern int _lastZero;
extern char _searchDirection;
extern int _n1;
extern int _n2;
#endif
