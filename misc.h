/******************************************************************************
* Fubções de leitura e escrita dos pinos GPIO
* Microcontrolador LPC1766
* Programador: Jean Luiz Zanatta
* Data: 26/02/2014
*******************************************************************************/
//include
#include "type.h"

//defines
#define MAX_VALID_TEMPERATURE (80*16)
#define MIN_VALID_TEMPERATURE (-20*16)
#define T_interrupt 100 	///<Período (em ms) entre interrupções temporais

//prototipo de funções
void Timer0Init(void);
void Timer2Init(void);
char bit_test(int var, char j);   
void bit_set(int *var, char j);
void bit_clear(int *var, char j);
void GPIO_init(void);
char input(char pin_number);
void output_high(char pin_number);
void output_low(char pin_number);
void toggle_pin(char pin_number);
void config_output(char pin_number);
void config_input(char pin_number);
void delay_config(void);
void delay_us(uint32_t us);
void delay_ms(int ms);
void timer_interrupt_config(void);
void timer_interrupt_enable(void);

//prototipo das funções GPIO telemetria
void GPIO_init(void);
uint8_t GPIO_get_value(uint8_t porta, uint8_t pino);
void GPIO_set_dir(uint8_t porta, uint8_t pino, uint8_t dir);
void GPIO_set_value(uint8_t porta, uint8_t pino, uint8_t value);
