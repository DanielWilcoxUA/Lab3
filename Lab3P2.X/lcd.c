/*
 * File:   lcd.c
 * Authors:
 *
 * Created on December 31, 2014, 1:39 PM
 */

#include "p24fj64ga002.h"
#include "lcd.h"
#include "timer.h"
#include "math.h"
#include <string.h>


#define LCD_DATA   LATB
#define LCD_RS  LATBbits.LATB1 //Pin 5
#define LCD_E   LATBbits.LATB0 //Pin 4

#define TRIS_D7  TRISBbits.TRISB15
#define TRIS_D6  TRISBbits.TRISB14
#define TRIS_D5  TRISBbits.TRISB13
#define TRIS_D4  TRISBbits.TRISB12
#define TRIS_RS  TRISBbits.TRISB1
#define TRIS_E   TRISBbits.TRISB0

#define LOWER 1
#define UPPER 0

typedef enum stateTypeEnum{
    RUNLED,
    STOPLED
} stateType;

extern volatile stateType LEDState;
extern unsigned int time;
extern unsigned int displayTime;

/* This function should take in a two-byte word and writes either the lower or upper
 * byte to the last four bits of LATB. Additionally, according to the LCD data sheet
 * It should set LCD_RS and LCD_E with the appropriate values and delays.
 * After these commands are issued, there should be a delay.
 * The command type is a simplification. From the data sheet, the RS is '1'
 * when you are simply writing a character. Otherwise, RS is '0'.
 */
void writeFourBits(unsigned char word, unsigned int commandType, unsigned int delayAfter, unsigned int lower){
    //TODO: BIT MASKING AND SHIFTING??!?!?!?!
    if(lower){
        LCD_DATA = (LCD_DATA & 0x0FFF)|((word & 0x000F) << 12);
    }
    else{
        LCD_DATA = (LCD_DATA & 0x0FFF)|((word & 0x00F0) << 8);
    }
    LCD_RS = commandType; delayUs(1);
    LCD_E = 1;  delayUs(1); //minimum 230 ns
    LCD_E = 0;  delayUs(1);
    if(delayAfter > 0)  {
        delayUs(delayAfter);
    }
}

/* Using writeFourBits, this function should write the two bytes of a character
 * to the LCD.
 */
void writeLCD(unsigned char word, unsigned int commandType, unsigned int delayAfter){
    writeFourBits(word, commandType, delayAfter, UPPER);
    writeFourBits(word, commandType, delayAfter, LOWER);
}

/*Initialize the LCD
 */
void initLCD(void) {
    // Setup D, RS, and E to be outputs (0).
    TRIS_D7 = 0;
    TRIS_D6 = 0;
    TRIS_D5 = 0;
    TRIS_D4 = 0;
    TRIS_RS = 0;
    TRIS_E = 0;

    // Initilization sequence utilizes specific LCD commands before the general configuration
    // commands can be utilized. The first few initilition commands cannot be done using the
    // WriteLCD function. Additionally, the specific sequence and timing is very important.

    // Enable 4-bit interface
    writeFourBits(0b0011, 0, 5, LOWER);

    // Function Set (specifies data width, lines, and font.
    writeFourBits(0b0011, 0, 1, LOWER);

    // 4-bit mode initialization is complete. We can now configure the various LCD
    // options to control how the LCD will function.

    writeFourBits(0b0011, 0, 1, LOWER);   //before function set 4bit
    writeFourBits(0b0010, 0, 1, LOWER);     //after function set 4bit

    writeLCD(0b00101000,0,0);   //function set, n is 1, f is 0
    writeLCD(0b00001000,0,0);   //display off
    writeLCD(0b00000001,0,0);   //display clear
    writeLCD(0b00000110,0,0);   //entry mode set, i/d =1, s=0

    writeLCD(0b00001111,0,0);   //display on, cursor blink

    // TODO: Display On/Off Control
        // Turn Display (D) Off
    // TODO: Clear Display (The delay is not specified in the data sheet at this point. You really need to have the clear display delay here.
    // TODO: Entry Mode Set
        // Set Increment Display, No Shift (i.e. cursor move)
    // TODO: Display On/Off Control
        // Turn Display (D) On, Cursor (C) Off, and Blink(B) Off
}

/* Given a character, write it to the LCD. RS should be set to the appropriate value.
 */
void printCharLCD(char c) {
    writeLCD(c, 1, 0);  //writes ascii in binary to lcd
}

/*
 * You can use printCharLCD here. Note that every time you write a character
 * the cursor increments its position automatically.
 * Since a string is just a character array, try to be clever with your use of pointers.
 */
void printStringLCD(const char* s) {
    int i;

    for(i=0; s[i]; i++) {   //loops til s[i] is a null pointer
        printCharLCD(s[i]);
    }
}

/*
 * Clear the display.
 */
void clearLCD(){
    writeLCD(0b0000001,0,0);    //clear command
}

/*
 Use the command for changing the DD RAM address to put the cursor somewhere.
 */
void moveCursorLCD(unsigned char x, unsigned char y){

    unsigned int DDRamAddress;

    if(y == 2)  {
        DDRamAddress = 40 + x - 1;
    }
    else    {
        DDRamAddress = x - 1;
    }
    //now convert to 7 digit binary
    DDRamAddress &= 0b01111111; //leaves 7 digit address
    DDRamAddress |= 0b10000000; //force the 8th to be 1

    //write to LCD
    writeLCD(DDRamAddress,0,0);
}

void resetCursorLCD() {
    moveCursorLCD((unsigned char) 1, (unsigned char) 1);
}

/*
 * This function is called in lab1p2.c for testing purposes.
 * If everything is working properly, you should get this to look like the video on D2L
 * However, it is suggested that you test more than just this one function.
 */
/*void testLCD(){

    int i = 0;
    printCharLCD('c');
    for(i = 0; i < 1000; i++) delayUs(1);
    clearLCD();
    printStringLCD("Hello!");
    moveCursorLCD(3, 2);
    for(i = 0; i < 1000; i++) delayUs(1);
    printStringLCD("Hello!");
    for(i = 0; i < 1000; i++) delayUs(1);
    clearLCD();
}
*/