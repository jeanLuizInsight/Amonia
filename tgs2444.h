/******************************************************************************
* Aquisi��o de sinal do sensor TGS2444 Figaro
* Microcontrolador LPC1766
* Programador: Jean Luiz Zanatta				
* Data: 26/02/2014
*******************************************************************************/
//defines
#define MAX_RS 90000
#define Rin2 560000    //resistor 560k
#define Rpd2 1000000   //resistor 1M
#define Rl   10000     //resistor 10k
#define T_VCH 2000            //tempo us para Vc leitura
#define T_VCL (T_VCH + 5000)  //tempo us para Vc leitura
#define T_READ (T_VCH + 4000) //tempo us necess�rio para ent�o efetuar a leitura do A/D 
#define T_VHL 14000           //tempo us para Vh aquecimento
#define T_VHH 250000          //tempo us ciclo funcionamento do sensor (aquecimento e detec��o)
#define STARTUP_NH3_SECONDS 60
#define STARTUP_NH3_NREADS ((1000000 / T_VHH) * STARTUP_NH3_SECONDS)
#define NPONTOS 102
//Para Convers�o de concentra��o de amonia para o DAC >>
#define V_MIN 1500 // mV
#define V_MAX 3200 // mV
#define VAL_DAC_MIN ((V_MIN * 1024) / 3300) //465.45
#define VAL_DAC_MAX ((V_MAX * 1024) / 3300) //992.96
//prot�tipo de fun��es
void tgs2444_config(void);
void zera_contagem(void);
void le_tensoes(void);
int calcula_rs(char formula);
char NH3_conc(int res);
void NH3_to_DAC(char conc);
//void NH3_maior_menor(char conc);
//variaveis globais
extern int _nad1;
extern int _nad2;
extern int _rs;
extern int _rs1;
extern int _rs2;
//char NH3level;
extern long double _nom;
extern long double _den;
// validReadCnt � usada como vari�vel auxiliar para fazer com que leituras
// s� sejam v�lidas ap�s aproximadamente STARTUP_NH3_SECONDS, possibilitando
// assim o aquecimento inicial do sensor, necess�rio para seu funcionamento adequado.
extern unsigned char _validReadCnt;
extern const int rs_res[NPONTOS];
extern const char _pinosSinalizacao[2];
extern unsigned int _i;
extern unsigned int _j;
extern unsigned int _k;
extern int _t1;
extern volatile short _vlrNH3;
//extern int _vlr2NH3;
