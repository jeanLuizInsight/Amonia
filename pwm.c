//includes
#include "pwm.h"
#include <LPC17xx.h>

//----inicialização canal pwm
void pwm_init(void) {
	// ensure PWM peripheral is powered on (it is powered on by default)
    LPC_SC->PCONP |= 1<<6;
    LPC_PWM1->TCR = 2;                      // bring PWM module into reset
    LPC_PWM1->IR = 0xff;                    // clear any pending interrupts   
    // configure P2.0 for PWM1.1 - O - Pulse Width Modulator 1, channel 3 output.
    LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 &~ (0x3<<4)) | (0x1<<4);
	  //configure P2.0 for PWM1.1 - O - Pulse Width Modulator 1, channel 5 output.
    LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 &~ (0x3<<8)) | (0x1<<8);    
    // Disable pullups for P2.2
		LPC_PINCON->PINMODE4 = (LPC_PINCON->PINMODE4 &~ (0x3<<4)) | (0x2<<4);
    // Disable pullups for P2.4
		LPC_PINCON->PINMODE4 = (LPC_PINCON->PINMODE4 &~ (0x3<<8)) | (0x2<<8);    
    // Set prescale so we have a resolution of 1us
    LPC_PWM1->PR = SystemCoreClock / (4 * 1000000) - 1; 
    LPC_PWM1->MR0 = PWM_PERIOD;             // set the period in us. 50Hz rate
    LPC_PWM1->MR3 = 0;                   // set duty of 2ms
    LPC_PWM1->MR5 = 0;                  // set a match that occurs 1ms
                                            // before the TC resets.
    LPC_PWM1->LER = 0x7;					// set latch-enable register
    LPC_PWM1->MCR = 0x2;          // reset on MR0
    LPC_PWM1->PCR |= 1<<11;                 // enable PWM1 with single-edge operation
		LPC_PWM1->PCR |= 1<<13;                 // enable PWM1 with single-edge operation
    LPC_PWM1->TCR = (1<<3) | 1;             // enable PWM mode and counting	
		return;
}

//----configuração do canal PWM
void pwm_channel_config(char channel) {
	// configure P2.0 for PWM1.1 - O - Pulse Width Modulator 1, channel 3 output.
	LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 &~ (0x3<<(2 * (channel - 1)))) | (0x1<<(2 * (channel - 1)));
  //LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(0x3 << 8)) | (0x1 << 8);	
	//Disable pullups for P2.2
	LPC_PINCON->PINMODE4 = (LPC_PINCON->PINMODE4 &~ (0x3<<(2 * (channel - 1)))) | (0x2<<(2 * (channel - 1)));
  //LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(0x3 << 8)) | (0x1 << 8);
	return;
}

//
void pwm_set_dutycycle(char channel, char dc) {
	if(channel == 3) { 
		LPC_PWM1->MR3 = (dc * PWM_PERIOD) / 100; 
	}
	if(channel == 5) { 
		LPC_PWM1->MR5 = (dc * PWM_PERIOD) / 100;
	}
	LPC_PWM1->LER |= (1 | (1<<channel));	// Load Enable Register. Enables use of new PWM match values. //before the TC resets.
  //LPC_PWM1->TCR = 2;                      // bring PWM module into reset
  //LPC_PWM1->TCR = (1 << 3) | 1;             // enable PWM mode and counting
	return;
}




