#include "p24FJ64GA002.h"
#include "pwm.h"

void initPWM(){
    T2CONbits.TCKPS = 3;
    TMR2 = 0;
    PR2 = 56;
    OC1CONbits.OCTSEL = 0; // using timer 2
    OC1CONbits.OCM = 6;
    OC2CONbits.OCTSEL = 0;
    OC2CONbits.OCM = 6;
    RPOR1bits.RP2R = 18; // map OC1 to pin 6
    RPOR3bits.RP6R = 19; // map OC2 to pin 15

    TRISBbits.TRISB3 = 0; // map pin 7 to ground
    LATBbits.LATB3 = 0;
    TRISBbits.TRISB7 = 0; // map pin 16 to ground
    LATBbits.LATB7 = 0;

    OC1R = 0; //very important
    OC1RS = 0;
    OC2R = 0;
    OC2RS = 0;
    T2CONbits.TON = 1;
}
