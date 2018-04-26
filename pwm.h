/*********************************************************************************
* Medidor comcentração amônia.
* pwm.h					
* Microcontrolador LPC1766
* Programador: Jean Luiz Zanatta
* Data: 28/02/2014						
**********************************************************************************/
//defines
#define PWM_CH_CUBA2 5
#define PWM_CH_CUBA3 3
#define PWM_PERIOD 20000 // us
//prototipo funções
extern void pwm_channel_config(char channel);
extern void pwm_set_dutycycle(char channel, char dc);
extern void pwm_init(void);

