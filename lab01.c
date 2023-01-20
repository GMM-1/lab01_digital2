/*
 ********************************************************************************
Universidad del Valle de Guatemala
 * curso: Electrónica Digital 2
Autor: Gabriel Andrade
compilador: XC8
proyecto: laboratorio 01
hardware: PIC 16F887
creado: 18-01-2023
última modificación: 19-01-2023
 ********************************************************************************
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#define _XTAL_FREQ 4000000
#include <xc.h>
#include <stdio.h>
#include "ADC.h"      //libreria para el manejo del ADC

////////////////////////////////////////////////////////////////////////////////
// VARIABLES
////////////////////////////////////////////////////////////////////////////////

int flag_antirrebote = 0;
int flag_antirrebote2 = 0;
int valor_adc;

////////////////////////////////////////////////////////////////////////////////
// PROTOTIPOS DE FUNCIONES
////////////////////////////////////////////////////////////////////////////////

void setupINTOSC(void);
void setupPORTS(void);
void setupINTERRUPT(void);
void contador(void);
void conversion_adc(void);

////////////////////////////////////////////////////////////////////////////////
// Rutina de INTERRUPCIONES
////////////////////////////////////////////////////////////////////////////////

void __interrupt() isr(void)
{
  //INTERRUPCION DEL PORTB
    if (INTCONbits.RBIF == 1)//revisar bandera de interrupcion
    {
        contador();
        INTCONbits.RBIF = 0; //limpiar bandera
    }
  //INTERRUPCION DEL ADC
    if (PIR1bits.ADIF) //revisar bandera de interrupcion
    {
      conversion_adc();

    }
}

////////////////////////////////////////////////////////////////////////////////
// CODIGO PRINCIPAL
////////////////////////////////////////////////////////////////////////////////

void main(void)
{
    setupINTOSC();    //configuracion del reloj oscilador
    setupPORTS();     //configuracion de los puertos
    setupINTERRUPT(); //configuracion de interrupt on change
    Init_ADC(AN12);   //Inicializacion del ADC para canal AN12


    // bucle infinito
    while(1)
    {
    ADCON0bits.GO = 1; // El ciclo A/D esta en progreso
    PORTA = valor_adc;
    }
}

////////////////////////////////////////////////////////////////////////////////
// FUNCIONES
////////////////////////////////////////////////////////////////////////////////

void setupINTOSC(void)
{
    //Seleccion de Oscilador interno
    OSCCONbits.SCS = 1;
    // oscilador a 4MHz
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;
}

void setupPORTS(void)
{
  //configuracion de tipo de entrada analogica/digital
  ANSEL = 0;
  ANSELH = 0b00100000;

  //salidas
  TRISD = 0;
  PORTD = 0;
  TRISA = 0;
  PORTA = 0;

  //entradas
  TRISBbits.TRISB7 = 1;
  TRISBbits.TRISB6 = 1;
  TRISBbits.TRISB0 = 1;
  PORTB = 0;
}

void setupINTERRUPT(void)
{
  //interrupcion del puerto B (push buttons)
  INTCONbits.RBIE = 1; //interrupciones del puerto B
  OPTION_REGbits.nRBPU = 0; // habilitar pull ups individuales en B
  WPUBbits.WPUB7 = 1;//pullup en B7
  WPUBbits.WPUB6 = 1;//pullup en B6
  IOCBbits.IOCB7 = 1;//interrupt on change en B7
  IOCBbits.IOCB6 = 1;//interruot on change en B6

  INTCONbits.GIE = 1; //interrupciones globales activadas
  INTCONbits.PEIE = 1; //interrupciones de perifericos

  PIR1bits.ADIF = 0;      // A/D conversion no ha empezado o completado
}

void contador(void)
{
  if (PORTBbits.RB6 == 0) //antirrebotes
  {
    flag_antirrebote = 1;
  }
  if (PORTBbits.RB6 == 1 && flag_antirrebote == 1) //si se presiona el PB RB6...
  {
      __delay_ms(10);
      PORTD = PORTD + 1; //incrementamos en uno el puerto D
      flag_antirrebote = 0;
  }
  if (PORTBbits.RB7 == 0)//antirrebotes
  {
      flag_antirrebote2 = 1;
  }
  if (PORTBbits.RB7 == 1 && flag_antirrebote2 == 1)//si se presiona el PB RB7...
  {
      __delay_ms(10);
      PORTD = PORTD - 1;//decrementamos en uno el puerto D
      flag_antirrebote2 = 0;
  }
}

void conversion_adc(void)
{
  PIR1bits.ADIF = 0; //conversion A/D sin empezar
  valor_adc = ADRESH; //registro de resultado del ADC

  //delay de funcionamiento
  __delay_ms(20);
}
