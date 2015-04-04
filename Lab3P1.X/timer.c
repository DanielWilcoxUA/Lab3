/*
 * File:   timer.c
 * Authors:
 *
 * Created on December 30, 2014, 8:07 PM
 */

#include "p24fj64ga002.h"
#include "timer.h"

#define F_CY 14745600
#define prescalar 256


void delayUs(unsigned int delay){

    //TODO: Create a delay for "delay" seconds using timer 1
    TMR1 = 0;
    PR1 = (F_CY*(delay/1000.0)/prescalar)-1;
    T1CONbits.TCKPS = 0b11; //prescalar 256
    IFS0bits.T1IF = 0;
    T1CONbits.TON = 1;
    //note flag will be put down in timer 2 interrupt
    while(IFS0bits.T1IF == 0);
    IFS0bits.T1IF = 0;
    T1CONbits.TON = 0;
}


