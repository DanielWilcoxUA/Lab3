#include "p24fj64ga002.h"
#include "adc.h"
#include "pwm.h"
#include "lcd.h"
#include "timer.h"
#include "sw1.h"
#include <stdio.h>
#include <stdlib.h>

_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & BKBUG_ON & COE_ON & ICS_PGx1 &
          FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS32768 )

_CONFIG2( IESO_OFF & SOSCSEL_SOSC & WUTSEL_LEG & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_OFF &
          IOL1WAY_OFF & I2C1SEL_PRI & POSCMOD_XT )

#define VDD 5;
#define VSS 0;

// ******************************************************************************************* //
// ******************************************************************************************* //
volatile int val = 0;
volatile int done = 0;
volatile int adcVal = 0;
volatile float curVoltage = 0;

typedef enum stateTypeEnum{
    forward,
    idle,
    backward,
    idle2
} stateType;

volatile stateType curState = forward;

int main(void)
{
        initADC();
        initPWM();
        initLCD();
        initSW1();

	while(1)
	{
            resetCursorLCD();
            char str[5];
            sprintf(str,"%f",curVoltage);
            str[5] = '\0';
            printStringLCD(str);

            if(adcVal < 511) {
                //Steer left
                OC1RS = PR2*adcVal/511.0;
                OC2RS = PR2;
            }
            else {
                //Steer right
                OC1RS = PR2;
                OC2RS = PR2*(1023-adcVal)/511.0;
            }
	}

	return 0;
}

// ******************************************************************************************* //

void _ISR _ADC1Interrupt(void){
    IFS0bits.AD1IF = 0;

    int i = 0;
    unsigned int *adcPtr;
    adcVal = 0;
    adcPtr = (unsigned int *) (&ADC1BUF0);

    for(i = 0; i < 16; i++){
        adcVal = adcVal + *adcPtr/16;
        adcPtr++;
    }
    
    float m = VDD-VSS;
    curVoltage = adcVal*m/1023.0 + VSS;
    
    done = 1;
}

void _ISR _CNInterrupt(void){
    IFS1bits.CNIF = 0;
    if(PORTBbits.RB5 == PRESSED){
        switch (curState) {
            case forward:
                curState = idle;
                //Turn off and ground pins 6 and 15
                RPOR1bits.RP2R = 0;
                TRISBbits.TRISB2 = 0;
                LATBbits.LATB2 = 0;
                RPOR3bits.RP6R = 0;
                TRISBbits.TRISB6 = 0;
                LATBbits.LATB6 = 0;
                break;
            case idle:
                curState = backward;
                //Assign OCRs to pins 7 and 16
                RPOR1bits.RP3R = 18;
                RPOR3bits.RP7R = 19;
                break;
            case backward:
                curState = idle2;
                //Turn off and ground pins 7 and 16
                RPOR1bits.RP3R = 0;
                TRISBbits.TRISB3 = 0;
                LATBbits.LATB3 = 0;
                RPOR3bits.RP7R = 0;
                TRISBbits.TRISB7 = 0;
                LATBbits.LATB7 = 0;
                break;
            case idle2:
                curState = backward;
                //Assign OCRs to pins 6 and 15
                RPOR1bits.RP2R = 18;
                RPOR3bits.RP6R = 19;
                break;
        }
    }
}