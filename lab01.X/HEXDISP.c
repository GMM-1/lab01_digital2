#include "HEXDISP.h"

uint8_t ArregloHex (uint8_t numero){
    switch(numero){
        case 0:
            return 0x3F;
            break;
        case 1:
            return 0x06;
            break;
        case 2:
            return 0x5B;
            break;
        case 3:
            return 0x4F;
            break;
        case 4:
            return 0x66;
            break;
        case 5:
            return 0x6D;
            break;
        case 6:
            return 0x7D;
            break;
        case 7:
            return 0x07;
            break;
        case 8:
            return 0x7F;
            break;
        case 9:
            return 0x6F;
            break;
        case 10: // A
            return 0x77;
            break;
        case 11: // B b
            return 0x7C;
            break;
        case 12: // C
            return 0x39;
            break;
        case 13: // d
            return 0x5E;
            break;
        case 14: // E
            return 0x79;
            break;
        case 15: // F
            return 0x71;
            break;

        default:
            return 0b11101011;
            break;
    }
}
