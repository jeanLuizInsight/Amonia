#ifndef __LCD4X20V3_H 
#define __LCD4X20V3_H
/* DEFINE IOMASK */
#define  LCD_D4_PIN  ((0<<5)|22)
#define  LCD_D5_PIN  ((2<<5)|11)
#define  LCD_D6_PIN  ((2<<5)|12)
#define  LCD_D7_PIN  ((2<<5)|13)
#define  LCD_EN_PIN  ((0<<5)|21)
#define  LCD_RS_PIN  ((0<<5)|20)
#define  LCD_RW_PIN  ((0<<5)|19)
/* Functions Header */
/* internal I/O functions */
#define lcd_rs_set() output_high(LCD_RS_PIN) 
#define lcd_rs_clr() output_low(LCD_RS_PIN)  
#define lcd_en_set() output_high(LCD_EN_PIN) 
#define lcd_en_clr() output_low(LCD_EN_PIN)  
#define lcd_rw_set() output_high(LCD_RW_PIN) 
#define lcd_rw_clr() output_low(LCD_RW_PIN)  
/* initialize both the GPIO of lpc and LCD */
#define lcd_clear()         lcd_write_control(0x01)
#define lcd_cursor_home()   lcd_write_control(0x02)
#define lcd_display_on()    lcd_write_control(0x0E)
#define lcd_display_off()   lcd_write_control(0x08)
#define lcd_cursor_blink()  lcd_write_control(0x0F)
#define lcd_cursor_on()     lcd_write_control(0x0E)
#define lcd_cursor_off()    lcd_write_control(0x0C)
#define lcd_cursor_left()   lcd_write_control(0x10)
#define lcd_cursor_right()  lcd_write_control(0x14)
#define lcd_display_sleft() lcd_write_control(0x18)
#define lcd_display_sright() lcd_write_control(0x1C)
#define lcd_gotoxy(x,y) lcd_write_control(_endBaseLinha[y - 1] + x - 1);
#define mostrasetas() lcd_defchar(0x00)
#define mostragraus() lcd_defchar(0x01)
//defines para atualização do LCD
#define MENU_TEMPS 0
#define MENU_ESTADO 1
#define MENU_STATS 2
#define MENU_VENTMIN 3

//prototipo de funções
void lcd_out_data4(char val);
void wait_busy_flag(void);
void lcd_write_nibbles(char val);
void lcd_write_control(unsigned char val);
void lcd_putchar(unsigned char c);
void lcd_print(unsigned char * str);
void lcd_defposxy(char x, char y);
void lcd_defchar(unsigned char c);
void lcd_deftext(unsigned char* str);
void lcd_printall(void);
char lcd_read_nibbles(void);
char lcd_getchar(void);
char lcd_checktext(void);
void lcd_readtext(void);
extern void lcd_init(void);
void print_date(char dia, char mes, int ano);
void print_hour(char hora, char minuto, char segundo);
void print_blank_until_NL(void);
void lcd_deftexttext(void);
void print_number(int numero, char caracteres);
extern void refresh_lcd(void);
extern void show_splash_screen(void);
//variaveis globais
extern char _useBusyFlag;
extern char _lcdText[4][20];
extern char _linhaLcd;
extern char _colunaLcd;
extern const char _endBaseLinha[4];
#endif

