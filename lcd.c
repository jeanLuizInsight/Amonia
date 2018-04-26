//includes
#include "lcd.h"
#include "misc.h"
#include "tgs2444.h"
//variaveis
char _useBusyFlag = 0;
char _lcdText[4][20] = 
{ {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
	{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
	{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
	{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
};
char _linhaLcd = 1;
char _colunaLcd = 1;
const char _endBaseLinha[4] = {0x80, 0xC0, 0x94, 0xD4};

void lcd_out_data4(char val) {
	if(bit_test(val,0)) { 
		output_high(LCD_D4_PIN);
	} else { 
		output_low(LCD_D4_PIN); 
	}
	if(bit_test(val,1)) { 
		output_high(LCD_D5_PIN);
	} else { 
		output_low(LCD_D5_PIN);
	}
	if(bit_test(val,2)) { 
		output_high(LCD_D6_PIN);
	}	else { 
		output_low(LCD_D6_PIN); 
	}
	if(bit_test(val,3)) {
		output_high(LCD_D7_PIN); 
	}	else { 
		output_low(LCD_D7_PIN);
	}
	return;
}

void wait_busy_flag(void) {
	int cnt = 0;	
	lcd_rs_clr();                                    //seleciona memoria de configuração
	lcd_rw_set();                                    //leitura
	config_input(LCD_D7_PIN);
	delay_us(10);	
	for (cnt = 2000;cnt > 0 ;cnt--){
		lcd_en_set();
		delay_us(1);
		if (!input(LCD_D7_PIN)){ 
			lcd_en_clr();
			break;
		}
		lcd_en_clr();
		delay_us(10);
	}		
	lcd_rs_set();                                    //memoria de dados
	lcd_rw_clr();	                                   //escrita
	config_output(LCD_D7_PIN);
	output_low(LCD_D7_PIN);	
	return;
}

void lcd_write_nibbles(char val) {
	//higher-order byte
	lcd_en_set();
	lcd_out_data4((val>>4) & 0x0F);
	lcd_en_clr();
	delay_us(10);
	//lower-order byte
	lcd_en_set();
	lcd_out_data4((val) & 0x0F);
	lcd_en_clr();
	if(_useBusyFlag) { 
		wait_busy_flag(); 
	}	else { 
		delay_us(700);
	}
	return;
}

void lcd_write_control(unsigned char val) {
  lcd_rs_clr();
	lcd_rw_clr();
	delay_us(10);
	lcd_write_nibbles(val);
	return;
}

void lcd_putchar(unsigned char c) {
	lcd_rs_set();
	lcd_rw_clr();
	_useBusyFlag = 1;
	delay_us(10);
	lcd_write_nibbles(c);
	return;
}
// TODO: Melhorar muito isso:
void lcd_print(unsigned char *str) {
    char indp;
	// limit 1 line display for prints
	for (indp = 0;indp < 20 && str[indp] != 0;indp++){
		lcd_putchar(str[indp]);
  }
	return;
}

void lcd_defposxy(char x, char y) {
	_linhaLcd = y;
	_colunaLcd = x;
	return;
}

void lcd_defchar(unsigned char c) {
	_lcdText[_linhaLcd - 1][_colunaLcd - 1] = c;
	_colunaLcd = (_colunaLcd) % 20 + 1;  									 // Incrementa proxima posição de escrita (coluna)
	if(_colunaLcd == 1) { 
			_linhaLcd = (_linhaLcd % 4) + 1; // Vai para próxima linha 
	} 
	return;
}

void lcd_deftext(unsigned char* str) {
	char indp;
	//limit 1 line display for prints
	for (indp = 0;indp < 20 && str[indp] != 0;indp++){
		lcd_defchar(str[indp]);
  }
	return;
}

void lcd_printall(void) {
	char aux1, aux2 = 0;	
	for (aux1 = 0;aux1 < 4 ;aux1++){
		lcd_gotoxy(1, aux1 + 1);
		for (aux2 = 0;aux2 < 20 ;aux2++){
			lcd_putchar(_lcdText[aux1][aux2]);
		}
	}
	lcd_cursor_off();	
	return;
}

char lcd_read_nibbles(void) {
	char retval = 0;	
	//higher-order byte
  lcd_en_set();
	delay_us(1);
	retval |= (input(LCD_D7_PIN)<<7);
	retval |= (input(LCD_D6_PIN)<<6);
	retval |= (input(LCD_D5_PIN)<<5);
	retval |= (input(LCD_D4_PIN)<<4);
  lcd_en_clr();
  delay_us(10);
  //lower-order byte
  lcd_en_set();
  delay_us(1);
  retval |= (input(LCD_D7_PIN)<<3);
	retval |= (input(LCD_D6_PIN)<<2);
	retval |= (input(LCD_D5_PIN)<<1);
	retval |= (input(LCD_D4_PIN)<<0);
	lcd_en_clr();
  delay_us(100); 
	return(retval);
}

char lcd_getchar(void) {
	char retval = 0;	
	lcd_rs_set();                                               //seleciona memoria de dados
	lcd_rw_set();                                               //leitura
	delay_us(10);
	config_input(LCD_D4_PIN);
	config_input(LCD_D5_PIN);
	config_input(LCD_D6_PIN);
	config_input(LCD_D7_PIN);
	delay_us(10);  
	retval = lcd_read_nibbles();	
	lcd_rw_clr();
	config_output(LCD_D4_PIN);
	config_output(LCD_D5_PIN);
	config_output(LCD_D6_PIN);
	config_output(LCD_D7_PIN);	
	return(retval);			
}

char lcd_checktext(void) {
	static char aux1, aux2 = 0;	
	for (aux1 = 0; aux1 < 4 ; aux1++) {
		lcd_gotoxy(1, aux1 + 1);	
		lcd_rs_set();                                               //seleciona memoria de dados
		lcd_rw_set();                                               //leitura
		config_input(LCD_D4_PIN);
		config_input(LCD_D5_PIN);
		config_input(LCD_D6_PIN);
		config_input(LCD_D7_PIN);
		delay_us(10);	
		for (aux2 = 0; aux2 < 20 ; aux2++) {
			if(lcd_read_nibbles() != _lcdText[aux1][aux2]) { 
					aux2 = 22;                                            //força saída do loop, verificação a seguir entende como divergência e retorna 0
			} 
		}
		lcd_rw_clr();
		config_output(LCD_D4_PIN);
		config_output(LCD_D5_PIN);
		config_output(LCD_D6_PIN);
		config_output(LCD_D7_PIN);
		if(aux2 > 20) { 
			return(0); 
		}                                                           //Se houve divergência aux2 foi forçado maior que 20, retorna 0		
	}
	return(1);
}

void lcd_readtext(void) {
	char aux1, aux2 = 0;	
	for (aux1 = 0; aux1 < 4 ; aux1++) {
		lcd_defposxy(1,aux1+1);	
		lcd_rs_set();                                               //seleciona memoria de dados
		lcd_rw_set();                                               //leitura
		config_input(LCD_D4_PIN);
		config_input(LCD_D5_PIN);
		config_input(LCD_D6_PIN);
		config_input(LCD_D7_PIN);
		delay_us(10);	
		for (aux2 = 0; aux2 < 20 ; aux2++) {
			_lcdText[aux1][aux2] = lcd_read_nibbles();
		}
		lcd_rw_clr();
		config_output(LCD_D4_PIN);
		config_output(LCD_D5_PIN);
		config_output(LCD_D6_PIN);
		config_output(LCD_D7_PIN);			
	}
	return;
// 	if((aux1<4) || (aux2<20)){ return(0); }
// 	return(1);
}

void lcd_init(void)	{
	char setas[8] = {0x04, 0x0E, 0x1F, 0x00, 0x00, 0x1F, 0x0E, 0x04 };
	char graus[8] = {0x0C, 0x12, 0x12, 0x0C, 0x00, 0x00, 0x00, 0x00 };
	//PINSEL2  = 0x00000000; // Funções de debug e trace NÃO usadas.
  /* we only work on OUTPUT so far */
	config_output(LCD_D4_PIN);
	config_output(LCD_D5_PIN);
	config_output(LCD_D6_PIN);
	config_output(LCD_D7_PIN);
	config_output(LCD_EN_PIN);
	config_output(LCD_RS_PIN);
	config_output(LCD_RW_PIN);
	output_low(LCD_D4_PIN);
	output_low(LCD_D5_PIN);
	output_low(LCD_D6_PIN);
	output_low(LCD_D7_PIN);
	output_low(LCD_EN_PIN);
	output_low(LCD_RS_PIN);
	output_low(LCD_RW_PIN);         
  /* IO init complete, init LCD */   
  /* init 4-bit ops*/
  lcd_rs_clr();
  lcd_en_clr();
	//wait VDD raise > 4.5V
	delay_ms(15);
	//lcd_wait();
	_useBusyFlag = 0;
   //dummy inst 
  lcd_write_nibbles(0x30);
	delay_ms(5);	  //////////////
  lcd_write_nibbles(0x30);
	delay_us(100); ////////////
	lcd_write_nibbles(0x30);	
	delay_ms(5);	  /////////////
	 //FUNCTION SET
	 //001DL  N F XX
	 //DL=1: 8bit
	 //DL=0: 4bit
	 //N=0: 1 line display
	 //N=1: 2 line display
	 //F=0: 5x7 dots
	 //F=1: 5x10 dots
	 //our case:
	 //0010 1000
	 lcd_en_set();
	 lcd_out_data4(0x2);
	 lcd_en_clr();
	 delay_ms(1);
	 delay_us(40);	//////////
	 lcd_write_nibbles(0x28);		 
	 //LCD ON
	 delay_us(40);	//////////
	 lcd_write_nibbles(0x0E);
	 //Clear Display
	 delay_us(40);   ////////////
	 lcd_write_nibbles(0x01);  // LCD CLEAR
	 //Entry mode
	 delay_ms(2);	   ////////////
	 lcd_write_nibbles(0x06);
	// Grava caracteres
	delay_ms(2);
	lcd_write_nibbles(0x40);
	delay_ms(2);
	for (_i = 0; _i < 8;_i++) {
       lcd_putchar(setas[_i]);
    }
	for (_i = 0; _i < 8;_i++) {
       lcd_putchar(graus[_i]);
    }	
	return;
}

void print_date(char dia, char mes, int ano) {
	lcd_defchar(0x30 + (dia / 10));          //Dia (dezena)
	lcd_defchar(0x30 + (dia % 10));          //Dia (unidade)
	lcd_defchar('/');	
	lcd_defchar(0x30 + (mes / 10));          //Mes (dezena)
	lcd_defchar(0x30 + (mes % 10));          //Mes (unidade)
	lcd_defchar('/');	
	lcd_defchar(0x30 + (ano / 1000)); 			 //Ano (milhar)
	ano = ano - (ano / 1000) * 1000;
	lcd_defchar(0x30 + (ano / 100)); 				 //Ano (centena)
	ano = ano - (ano / 100) * 100;
	lcd_defchar(0x30 + (ano / 10)); 				 //Ano (dezena)
	ano = ano - (ano / 10) * 10;
	lcd_defchar(0x30 + (ano / 1)); 				 	 //Ano (unidade)
	return;
}

void print_hour(char hora, char minuto, char segundo) {
	lcd_defchar(0x30 + (hora / 10));         //Hora (dezena)
	lcd_defchar(0x30 + (hora % 10));         //Hora (unidade)
	lcd_defchar(':');	
	lcd_defchar(0x30 + (minuto / 10));       //minuto (dezena)
	lcd_defchar(0x30 + (minuto % 10));       //minuto (unidade)	
	lcd_defchar(':');	
	lcd_defchar(0x30 + (minuto / 10));       //segundo (dezena)
	lcd_defchar(0x30 + (minuto % 10));       //segundo (unidade)	
	return;
}

void print_blank_until_NL(void) {
	while(_colunaLcd != 1) { 
			lcd_defchar(' '); 
	}
	return;
}

void lcd_deftexttext(void) {
	char aux1, aux2 = 0;	
	for (aux1 = 0; aux1 < 4 ; aux1++) {
		for (aux2 = 0; aux2 < 20 ; aux2++) {
		}
	}
	return;
}

void print_number(int numero, char caracteres) {            //maximo 5 caracteres
	int auxnum = 0;	
	auxnum = numero;	
	if(caracteres >= 5) {
		if(numero >= 10000) { 
				lcd_defchar(0x30 + auxnum / 10000); 
		} else { 
				lcd_defchar(' '); 
		}
		auxnum -= (auxnum / 10000) * 10000;
	}
	if(caracteres >= 4) {
		if(numero >= 1000) { 
				lcd_defchar(0x30 + auxnum / 1000);
		} else { 
				lcd_defchar(' ');
		}
		auxnum -= (auxnum / 1000) * 1000;
	}
	if(caracteres >= 3) {
		if(numero >= 100) { 
				lcd_defchar(0x30 + auxnum / 100); 
		} else { 
				lcd_defchar(' '); 
		}
		auxnum -= (auxnum / 100) * 100;
	}
	if(caracteres >= 2) {
		if(numero >= 10) { 
				lcd_defchar(0x30 + auxnum / 10); 
		} else { 
				lcd_defchar(' ');
		}
		auxnum -= (auxnum / 10) * 10;
	}
	lcd_defchar(0x30 + auxnum);
	return;
}

/******************************************************
	Atualização do LCD
*******************************************************/
void refresh_lcd(void) {
	lcd_defposxy(1,2);         //(posiciona vlr inicial em x, posiciona valor inicial em y)	
	print_blank_until_NL();
	//lcd_deftext("NH3_1:");
	//print_number(_vlr1NH3, 3); //(valor, espaço no display)
	//lcd_deftext("ppm ");
	//lcd_defchar('(');
	//print_number(_rs1, 5);
	//lcd_defchar(')');
	//print_blank_until_NL();	
	lcd_deftext("NH3:");
	print_number(_vlrNH3, 3);
	lcd_deftext("ppm ");
	lcd_defchar('(');
	print_number(_rs2, 5);
	lcd_defchar(')');
	while(!((_linhaLcd == 1) && (_colunaLcd == 1))) { 
			lcd_defchar(' '); // Imprime espaços até voltar p/ posição (1,1) do display (quebra de linhas e colunas são automáticas na função lcd_defchar
	} 
	lcd_cursor_off();	
	return;
}

void show_splash_screen(void) {
	lcd_defposxy(1,1); // necessário somente 1 posicionamento, função lcd_defchar(c) se responsabiliza pela quebra de linhas
	lcd_deftext("                    ");
	lcd_deftext("  ALTEM Tecnologia  ");
	lcd_deftext("  www.altem.com.br  ");
	lcd_deftext("                    ");
	return;
}



