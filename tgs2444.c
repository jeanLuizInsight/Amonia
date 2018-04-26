/******************************************************************************
* Aquisição de sinal do sensor TGS2444 Figaro
* Microcontrolador LPC1766
* Programador: Jean Luiz Zanatta				
* Data: 26/02/2014
*******************************************************************************/
//includes
#include <LPC17xx.h>
//#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "tgs2444.h"
#include "misc.h"
#include "adc.h"
#include "dac.h"
#include "rs485.h"
#include "uart.h"

//variaveis
int subTensao = 0;
int _nad1 = 0;
int _nad2 = 0;
int _rs = MAX_RS;
//int _rs1 = MAX_RS;
int _rs2 = MAX_RS;
//char NH3level = 0;
long double _nom = 0;
long double _den = 1;
//variavel armazena concentração NH3
volatile short _vlrNH3 = 0;
//int _vlr2NH3 = 0;
// validReadCnt é usada como variável auxiliar para fazer com que leituras
// só sejam válidas após aproximadamente STARTUP_NH3_SECONDS, possibilitando
// assim o aquecimento inicial do sensor, necessário para seu funcionamento adequado.
unsigned char _validReadCnt = STARTUP_NH3_NREADS;
//tabela com os valores gerados no matlab dos gráficos obtidos do datasheet do sensor(Kohm) (para comparar a resistencia vinda do sensor e verificar sua precisão) RS em ohm  
const int rs_res[NPONTOS] = {
	50400, 47250, 37800, 31500, 26250, 
	23100, 21000, 18900, 17850, 16800, 
	15750, 14785, 13897, 13083, 12341, 
	11665, 11053, 10500, 10002, 9551, 
	9135, 8746, 8382, 8043, 7727, 
	7435, 7165, 6917, 6691, 6485, 
	6300, 6134, 5986, 5853, 5733, 
	5623, 5521, 5424, 5331, 5239, 
	5145, 5048, 4949, 4848, 4746, 
	4646, 4548, 4453, 4363, 4278, 
	4200, 4130, 4066, 4009, 3956, 
	3906, 3860, 3814, 3769, 3723, 
	3675, 3624, 3571, 3517, 3461, 
	3405, 3350, 3297, 3245, 3196, 
	3150, 3108, 3070, 3035, 3003, 
	2973, 2944, 2917, 2890, 2863, 
	2835, 2806, 2777, 2747, 2716, 
	2684, 2652, 2619, 2586, 2553, 
	2520, 2487, 2454, 2421, 2388, 
	2356, 2324, 2293, 2263, 2234, 
	2205, 2178 
}; //102 elementos de 0 a 101
const char _pinosSinalizacao[2] = { (1<<5)|29, (1<<5)|28 }; //{ P1.29 = 61, P.1.28 = 60 }
unsigned int _i = 0;
unsigned int _j = 0;
unsigned int _k = 0;
int _t1 = 0;

void tgs2444_config(void) {
	//----Determina pinos de sinalizacao como saída----
	for(_i= 0;_i < 2;_i++) {
		config_output(_pinosSinalizacao[_i]);                           //controla direção dos pinos especificados sendo como saída  
		//modo de controle do registro 1 (p1.29(pino 45) e p1.28(pino 44)) = open-drain, lógica invertida
		LPC_PINCON->PINMODE_OD1 |= (1<<(_pinosSinalizacao[_i] & 0x1F)); 
		output_high(_pinosSinalizacao[_i]);                             //define a saída dos pinos como nivel alto
	}
}
void zera_contagem(void) {
	//contador de microssegundos (timer2 do uC utilizado para contagem de tempo necessária para funcionamento e leitura do sensor)
	LPC_TIM2->PR = ((SystemCoreClock / 4) / 1000000) - 1;       //Valor setado p/ prescaler register 25 (250ms = 250000us)
	//LPC_TIM2->IR  = 0xff;                                       // Limpa todas as interrupções.
	LPC_TIM2->TCR = 0x02;                                       //Timer Counter reset
	LPC_TIM2->TCR = 0x01;                                       //Timer Counter Enable
	return;
}

void le_tensoes(void) {		
	while(LPC_TIM2->TC < T_VHH) {}         //espera 250ms o ciclo total
	zera_contagem();	                     //zera contador
	output_low(_pinosSinalizacao[1]);      //drena corrente para p1.28 (saida nivel baixo) - Vh 
	while(LPC_TIM2->TC < T_VCH) {}         //espera 2ms
	output_low(_pinosSinalizacao[0]);      //drena corrente para p1.29 (saida nivel baixo) - Vc 
	while(LPC_TIM2->TC < T_READ) {}        //espera 6ms tempo ideal(datasheet sensor) para efetuar leitura
	_nad1 = adc_read(2);                   //(V passa pelo res. 10k)atribui para variavel valor de conversão a/d canal 2 [ad02] 
	_nad2 = adc_read(1);                   //(V ñ passa pelo res. 10k)                                           canal 1 [ad01]
	_nad1 += adc_read(2);
	_nad2 += adc_read(1);
	while(LPC_TIM2->TC < T_VCL) {}         //espera 7ms
	output_high(_pinosSinalizacao[0]);     //cessa drena corrente p1.29 (saida nivel alto) - Vc
	while(LPC_TIM2->TC < T_VHL) {}         //espera 14ms
	output_high(_pinosSinalizacao[1]);     //cessa drena corrente p1.28 (saida nivel alto) - Vh
	return;
}

int calcula_rs(char formula) {	
	//se a relação subtraida das duas resistencias lidas do A/D for menos que 400(muito baixa) considera-se máxima resistência do sensor
	//subTensao = _nad1 - _nad2;
	if(abs(_nad1 - _nad2) > 400) {     //se a sub tensão for menor que 500 evita calculos amônia = 0ppm
		if(formula == 1) {
			_rs = (Rl * _nad2) / (_nad1 - _nad2);                    //resistencia calculada
		}
		else if(formula == 2) {
			_nom = ((long double)(Rin2 + Rpd2) * (long double)Rl * (long double)_nad2);
			_den = ((long double)(Rin2 + Rpd2) * (long double)(_nad1 - _nad2) - (long double)Rl * _nad2);
			_rs = _nom / _den;                                       //resistencia calculada
		}
	} else {
		_rs = MAX_RS;                        //90000
	}	
  //NH3level = NH3conc(Rs);
	return(_rs); 
}

//----função que calcula a resistencia do sensor e converte para concentração de amônia real 0 a 100ppm
//----no vetor a posição 0 corresponde a 0ppm e a posição 100 a 100ppm 
char NH3_conc(int res) {
	char temp;	
	if(res > rs_res[0]) {                                        //se a resistencia calculada for maior que a maxima 
			return(0);                                               //retorna valor minimo
	}
	for(temp = 0;temp < (NPONTOS - 1);temp++) {                  //percorre todas as posições do vetor 
		if(abs(res - rs_res[temp]) < abs(res - rs_res[temp+1])) {  //até encontrar posição correspondente a Rs mais próxima
			return(temp);                                            //retorna concentração da amônia
		}
	}
	return(101);                                                 //retorna valor maximo
}

//----Conversão de concentração de amonia para o DAC (converte o valor real novamente para analógico para verificar se está correta )  
void NH3_to_DAC(char conc) {
	if(conc > 100) {                                     //se a concentração for maior que 100ppm
		conc = 100;                                        //considera conc. igual a 100 (valor max.)
	}	
	//---valores min. e max. são atribuidos pelas regras de precisão do DAC do uC
	dac_set_value(VAL_DAC_MIN + ((VAL_DAC_MAX - VAL_DAC_MIN) * conc) / 100); //a fórmula converte D/A e a função seta no uC
	//465.45min
  //992.96max
	//ex: 162.9 valor analógico para se a concentração for igual a 30ppm
	return;
}




