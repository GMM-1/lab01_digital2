#include "ADC.h"
//libreria para configuracion del ADC para el PIC16F887
//Incluir frecuencia en FOSC y canal analogico en channel

void Init_ADC (unsigned char channel)
{
    PIE1bits.ADIE = 1;      // Activa la interrupcion de ADC

    // Configuracion del ADC
    ADCON1bits.ADFM  = 0;   // Justificado a la izquierda
    ADCON1bits.VCFG1 = 0;   // Referencia como tierra
    ADCON1bits.VCFG0 = 0;   // Referencia poder

    ADCON0 = channel;       //configuracion de puerto
    __delay_us(50);         // delay de funcionamiento 
}
