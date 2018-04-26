/******************************************************************************
* Funções de leitura e escrita dos pinos GPIO
* Microcontrolador LPC1766
* Programador: Jean Luiz Zanatta
* Data: 26/02/2014
*******************************************************************************/
/*
  Para que funcione corretamente os pinos devem estar configurados como GPIO (ver registrador PINSEL) 
	e no caso de escrita como pino de saída (ver IODIR)
	-FIOPIN: registro que le o estado atual da porta, se FIOPIN é lido sua parte mascarada com 1 no registro FIOMASK é lido com 0
	-FIOSET: controla o estado dos pinos de saída, retorna o conteudo atual do registro da saida da porta, escrevendo 0s não tem efeito e 1s produz HIGHS nos respectivos pinos da porta
	-FIOCLR: controla o estado dos pinos de saída, retorna o conteudo atual do registro da saida da porta, escrevendo 0s não tem efeito e 1s produz LOWS nos respectivos pinos da porta
	-FIODIR: controla individualmente direção de cada pino da porta GPIO
*/
//includes
#include "misc.h"
#include <LPC17xx.h>

char bit_test(int var, char j) { 
	return ((var>>j)&0x01);  
}	//return ((var & (1 << j)) >> j); 

void bit_set(int *var, char j) { 
	*var |= (1<<j); 
	return;
}	  

void bit_clear(int *var, char j) { 
	*var &= ~(1<<j);
	return;
}
//----------------------------------------------------------------------------//
/* Notação: Os bits mais significativos de pin_number sempre contém a Porta (0 ou 1) do pino (necessario no LPC214X) */
// Retorna o valor no pino especificado (0 ou 1)
char input(char pin_number) {
	if((pin_number>>5)==0) {
		return ((LPC_GPIO0->FIOPIN >> (pin_number&0x1F)) & 0x00000001);
	} else if((pin_number>>5)==1) {
		return ((LPC_GPIO1->FIOPIN >> (pin_number&0x1F)) & 0x00000001);
	} else if((pin_number>>5)==2) {
		return ((LPC_GPIO2->FIOPIN >> (pin_number&0x1F)) & 0x00000001);
	} else if((pin_number>>5)==3) {
		return ((LPC_GPIO3->FIOPIN >> (pin_number&0x1F)) & 0x00000001);
	} else if((pin_number>>5)==4) {
		return ((LPC_GPIO4->FIOPIN >> (pin_number&0x1F)) & 0x00000001);
	}
	return 0;
}
//função que produz saída como nivel alto(FIOSET) nos pinos da porta controlada como saída configurada como GPIO
void output_high(char pin_number) {
	if((pin_number>>5)==0) {
		LPC_GPIO0->FIOSET |= (1 << (pin_number&0x1F));
	} else if((pin_number>>5)==1) {
		LPC_GPIO1->FIOSET |= (1 << (pin_number&0x1F));   //seta bit = 1 especificado por pin_number (saída alto), 0 mantem-se inalterada
	} else if((pin_number>>5)==2) {
		LPC_GPIO2->FIOSET |= (1 << (pin_number&0x1F));
	} else if((pin_number>>5)==3) {
		LPC_GPIO3->FIOSET |= (1 << (pin_number&0x1F));
	} else if((pin_number>>5)==4) {
		LPC_GPIO4->FIOSET |= (1 << (pin_number&0x1F));
	}
	return;
}
//função que produz saída como nivel baixo(FIOCLR) nos pinos da porta controlada como saída configurada como GPIO
void output_low(char pin_number) {
	if((pin_number>>5) == 0){
		LPC_GPIO0->FIOCLR |= (1 << (pin_number&0x1F));
	} else if((pin_number>>5) == 1) {
		LPC_GPIO1->FIOCLR |= (1 << (pin_number&0x1F));  //seta bit = 1 especificado por pin_number (saída baixo), e limpa o bit de FIOSET(saida alto)
	} else if((pin_number>>5) == 2) {
		LPC_GPIO2->FIOCLR |= (1 << (pin_number&0x1F));
	} else if((pin_number>>5) == 3) {
		LPC_GPIO3->FIOCLR |= (1 << (pin_number&0x1F));
	} else if((pin_number>>5) == 4) {
		LPC_GPIO4->FIOCLR |= (1 << (pin_number&0x1F));
	}
	return;
}

void toggle_pin(char pin_number) {
	if (input(pin_number)) { 
		output_low(pin_number); 
	} else { 
		output_high(pin_number); 
	}
	return;
}
//função seta pino controlado como saída
void config_output(char pin_number) {
	if((pin_number>>5) == 0){
		LPC_GPIO0->FIODIR |= (1 << (pin_number&0x1F));
	} else if((pin_number>>5) == 1) {
		LPC_GPIO1->FIODIR |= (1 << (pin_number&0x1F));  //seta bit = 1 especificado por pin_number (61(bit 29) e 60(bit 28)) (pino P1.29 e P1.28 controlado é de saída)
	} else if((pin_number>>5) == 2) {
		LPC_GPIO2->FIODIR |= (1 << (pin_number&0x1F));
	} else if((pin_number>>5) == 3) {
		LPC_GPIO3->FIODIR |= (1 << (pin_number&0x1F));
	} else if((pin_number>>5) == 4) {
		LPC_GPIO4->FIODIR |= (1 << (pin_number&0x1F));
	}
	return;
}
//função seta pino controlado como entrada
void config_input(char pin_number) {
	if((pin_number>>5) == 0){
		LPC_GPIO0->FIODIR &= ~(1 << (pin_number&0x1F));
	} else if((pin_number>>5) == 1) {
		LPC_GPIO1->FIODIR &= ~(1 << (pin_number&0x1F));  //seta bit = 0 especificado por pin_number (pino controlado é de entrada)
	} else if((pin_number>>5) == 2) {
		LPC_GPIO2->FIODIR &= ~(1 << (pin_number&0x1F));
	} else if((pin_number>>5) == 3) {
		LPC_GPIO3->FIODIR &= ~(1 << (pin_number&0x1F));
	} else if((pin_number>>5) == 4) {
		LPC_GPIO4->FIODIR &= ~(1 << (pin_number&0x1F));
	}
	return;
}

void delay_config(void) {
	LPC_TIM0->TCR = 0x02;   // Timer Counter reset
	LPC_TIM0->PR  = ((SystemCoreClock / 4) / 1000000) - 1; // contador de microssegundos
  //LPC_TIM1->TCR = 0x01; // Timer Counter Enable
	LPC_TIM0->MCR = 0x04;                /* stop timer on match */
	return;
}
void Timer0Init(void){		
	LPC_SC->PCLKSEL0 &= ~(1<<3);
	LPC_SC->PCLKSEL0 |= (1<<2);	
	return;
}
void delay_us(uint32_t us) {
		LPC_TIM0->TCR = 0x02;                /* reset timer */
    LPC_TIM0->PR  = 99;                  /* set prescaler */
    LPC_TIM0->MR0 = us;
    LPC_TIM0->IR  = 0xff;                /* reset all interrrupts */
    LPC_TIM0->MCR = 0x04;                /* stop timer on match */
    LPC_TIM0->TCR = 0x01;                /* start timer */

    /* wait until delay time has elapsed */
    while (LPC_TIM0->TCR & 0x01);
    return;
}

void delay_ms(int ms) {
	int cnt2 = 0;  
	for (cnt2 = ms;cnt2 > 0;cnt2--) {
		delay_us(1000);
  } 
	return;
}
//para LCD
void timer_interrupt_config(void) {
	/** Definições de interrupção usando Repetitive Interrupt Timer (RIT) */	
	LPC_RIT->RICOMPVAL = (T_interrupt * (SystemCoreClock / 4)) / 1000; //RICOMPVAL = 2666666
	LPC_RIT->RICTRL |= (1<<1); 	// Contador é zerado quando contador atinge valor igual ao RICOMPVAL seta RITENCLR = 1
	LPC_RIT->RICTRL &= ~(1<<3); // Desabilita interrupção (até que seja re-habilitada) seta RITEN = 0
	return;
}
//para LCD
void timer_interrupt_enable(void) {
	LPC_RIT->RICTRL |= (1<<3); //reabilita interrupção seta RITEN = 1
	NVIC_EnableIRQ(RIT_IRQn);  //(habilita interrupção externa)função habilita interrupção dos dispositivos especificos no controlador de interrupção NVIC
	return;
}
//-----------------------macros para configurações GPIO telemetria GSM/GPRS-------------------
//variaveis constantes.
static LPC_GPIO_TypeDef (*const LPC_GPIO[5]) = { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4 };

//função para inicialização do GPIO. Habilita clock no PCONP para o GPIO.
 void GPIO_init(void){
	LPC_SC->PCONP |= (1<<15);
	return;
}

//função para ler o valor de um pino do GPIO. Retorna o estado logico do pino de GPIO.
//mesma input(retorna o valor do pino especificado)
uint8_t GPIO_get_value(uint8_t porta, uint8_t pino){
	return((LPC_GPIO[porta]->FIOPIN & (1<<pino)) ? 1 : 0);
}

//função para escrita de valor logico em um pino do GPIO. Escreve o valor logico no pino indicado.
//mesma output_low, output_high facilitada!!!
void GPIO_set_value(uint8_t porta, uint8_t pino, uint8_t value){
	if(value){
		LPC_GPIO[porta]->FIOSET=(1<<pino);  //nivel alto
		return;
	}
	LPC_GPIO[porta]->FIOCLR=(1<<pino);	  //nivel baixo
	return;
}

//função para configurar a direção dos pinos do GPIO. Configura os pinos do GPIO como entrada ou saida.
//dir = 1 true: Saida, Dir = 0: Entrada.
//mesma config input, output facilitada!!
void GPIO_set_dir(uint8_t porta, uint8_t pino, uint8_t dir){
	if(dir){
		LPC_GPIO[porta]->FIODIR |= (1<<pino);  //saida
		return;
	}
	LPC_GPIO[porta]->FIODIR &= ~(1<<pino);   //entrada
	return;
}



