#define _XTAL_FREQ 4000000
//ELEGIR UN ANSEL en channel.

#ifndef ADC_H
#define	ADC_H

#include <xc.h>
#include <stdint.h>
//Fosc/8 canal variable y status completed y ADC disabled
//combinaciones para puertos analogicos
#define AN0          0b01000001
#define AN1          0b01000101
#define AN2          0b01001001
#define AN3          0b01001101
#define AN4          0b01010001
#define AN5          0b01010101
#define AN6          0b01011001
#define AN7          0b01011101
#define AN8          0b01100001
#define AN9          0b01100101
#define AN10         0b01101001
#define AN11         0b01101101
#define AN12         0b01110001
#define AN13         0b01110101

void Init_ADC (unsigned char channel);
#endif
