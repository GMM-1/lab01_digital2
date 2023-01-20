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
#pragma config FOSC = INTRC_NOCLKOUT  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF             // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF            // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF            // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF               // Code Protection bit (Program memory code protection is disabled)
      #pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF            // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF             // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = OFF            // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF              // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V         // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF              // Flash Program Memory Self Write Enable bits (Write protection off)

#define _XTAL_FREQ 4000000
#include <xc.h>
#include <stdio.h>
#include "ADC.h"      //libreria para la configuracion del ADC
#include "HEXDISP.h" //libreria para el manejo de displays

////////////////////////////////////////////////////////////////////////////////
// VARIABLES
////////////////////////////////////////////////////////////////////////////////

//variables para contador
int flag_antirrebote = 0;
int flag_antirrebote2 = 0;

//variables para conversion adc
int valor_adc;

//variables para multiplexeo
uint8_t turno_multiplexeo;

//variables para conversion a hexadecimal
uint8_t display_1;
uint8_t display_2;
uint8_t unidades_disp;
uint8_t decenas_disp;

////////////////////////////////////////////////////////////////////////////////
// PROTOTIPOS DE FUNCIONES
////////////////////////////////////////////////////////////////////////////////

//configuraciones
void setupINTOSC(void);
void setupPORTS(void);
void setupINTERRUPT(void);
void setupTMR1(void);

void contador(void);
void conversion_adc(void);
void reseteo_TMR1(void);
void conversion_hexa(void);
void multiplexeo(void);


////////////////////////////////////////////////////////////////////////////////
// Rutina de INTERRUPCIONES
////////////////////////////////////////////////////////////////////////////////

void __interrupt() isr(void)
{
  //INTERRUPCION DEL TMR1H
  if(PIR1bits.TMR1IF) //se enciende la bandera cada segundo
  {
    reseteo_TMR1();

    //multiplexeo
    switch (turno_multiplexeo)
    {
        case 0:
            PORTC = display_1;
            PORTEbits.RE0 = 1;
            PORTEbits.RE1 = 0;
            break;
        case 1:
            PORTC = display_2;
            PORTEbits.RE0 = 0;
            PORTEbits.RE1 = 1;
            break;
        default:
            turno_multiplexeo = 0;
            break;
  }
}

  //INTERRUPCION DEL PORTB
    if (INTCONbits.RBIF == 1)//revisar bandera de interrupcion
    {
        contador();
        INTCONbits.RBIF = 0; //limpiar bandera
    }

  //INTERRUPCION DEL ADC
  if (PIR1bits.ADIF){
      if (ADCON0bits.CHS ){
         valor_adc = ADRESH;
         //PORTD = cuenta;
      }
      PIR1bits.ADIF = 0;
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
    setupTMR1();      //configuracion para el timer
    Init_ADC(AN12);   //Inicializacion del ADC para canal AN12


    // bucle infinito
    while(1)
    {
      //mandamos el valor del adc al puerto
      ADCON0bits.GO = 1; // El ciclo A/D esta en progreso
      PORTA = valor_adc;

      //despliegue del ADC en los 7 segmentos
      conversion_hexa();   //hacemos la conversion del adc a hexadecimal
      multiplexeo();       //hacemos los turnos para cada display

      //alarma
      if(PORTD <= valor_adc)
      {
          PORTEbits.RE2 = 1;
      }
      else{
          PORTEbits.RE2 = 0;
      }
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
  TRISC = 0;
  PORTC = 0;
  TRISE = 0;
  PORTE = 0;


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
  WPUBbits.WPUB7 = 1;       //pullup en B7
  WPUBbits.WPUB6 = 1;       //pullup en B6
  IOCBbits.IOCB7 = 1;       //interrupt on change en B7
  IOCBbits.IOCB6 = 1;       //interruot on change en B6
  INTCONbits.RBIF = 0;      //bandera apagada

  INTCONbits.GIE = 1; //interrupciones globales activadas
  INTCONbits.PEIE = 1; //interrupciones de perifericos

  PIR1bits.ADIF = 0;      // A/D conversion no ha empezado o completado

  //interrupcion del timer 1
  PIE1bits.TMR1IE = 1;    // interrupcion del TMR1H
  PIR1bits.TMR1IF = 0;    //bandera apagada

}

void setupTMR1(void)
{
// Timer1 Registers Prescaler= 1 - TMR1 Preset = 65036 - Freq = 500.00 Hz - Period = 0.002000 seconds
  T1CONbits.T1CKPS1 = 0;   // bits 5-4  Prescaler Rate Select bits
  T1CONbits.T1CKPS0 = 0;   // bit 4
  T1CONbits.T1OSCEN = 0;   // bit 3 Timer1 Oscillator Enable Control bit 1 = on
  T1CONbits.T1SYNC = 1;    // bit 2 Timer1 External Clock Input Synchronization Control bit...1 = Do not synchronize external clock input
  T1CONbits.TMR1CS = 0;    // bit 1 Timer1 Clock Source Select bit...0 = Internal clock (FOSC/4)
  T1CONbits.TMR1ON = 1;    // bit 0 enables timer
  // Reset del tmr1
  reseteo_TMR1();
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

void reseteo_TMR1(void)
{
  TMR1H = 254;
  TMR1L = 12;
  PIR1bits.TMR1IF = 0; //apagamos la bandera
}

void conversion_hexa(void)
{
    // para el display1 desplegamos las unidades
    unidades_disp = valor_adc%16;
    // para el display2 desplegamos las decenas
    decenas_disp = valor_adc/16;

    //hacemos la conversion a hexadecimal
    display_1 = ArregloHex(unidades_disp);
    display_2 = ArregloHex(decenas_disp);
    return;
}

void multiplexeo(void)
{
    // Actualiza el valor de turno para realizar multiplexeo
    switch(turno_multiplexeo)
    {
        case 0:
            turno_multiplexeo = 1 ;
            break;
        case 1:
            turno_multiplexeo = 0 ;
            break;
        default:
            turno_multiplexeo = 0 ;
            break;
    }
    return;
}
