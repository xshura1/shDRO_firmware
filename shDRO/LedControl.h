/*
  LedControl.h - A library for controling Led Digits or 
  Led arrays with a MAX7219/MAX7221
  Copyright (c) 2007 Eberhard Fahle

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef LedControl_h
#define LedControl_h


#include <stdint.h>
#include <avr/pgmspace.h>

#include "hardware.h"

/*
 * Here are the segments to be switched on for characters and digits on
 * 7-Segment Displays
 */

/* номера битов светодиодов индикатора
        
     _______
	|   6   |  
  1	|       | 5
	|_______|   
	|   0   |
  2	|       | 4
	|_______| .
        3     7  

*/

const static uint8_t charTable[128] PROGMEM  = {
		/*0*/		/*1*/		/*2*/		/*3*/		/*4*/	/*5*/		/*6*/		/*7*/		/*8*/	/*9*/
/*0*/	0b01111110,0b00110000,0b01101101,0b01111001,0b00110011,0b01011011,0b01011111,0b01110000,0b01111111,0b01111011,
/*1*/	0b01110111,0b00011111,0b00001101,0b00111101,0b01001111,0b01000111,0b00000000,0b00000000,0b00000000,0b00000000,
/*2*/	0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
/*3*/	0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
/*4*/	0b00000000,0b00000000,0b00000000,0b00000000,0b10000000,0b00000001,0b10000000,0b00000000,0b01111110,0b00110000,
/*5*/	0b01101101,0b01111001,0b00110011,0b01011011,0b01011111,0b01110000,0b01111111,0b01111011,0b00000000,0b00000000,
/*6*/	0b00000000,0b00001001,0b00000000,0b11100101,0b00000000,0b01110111,0b00011111,0b01001110,0b00111101,0b01001111,
/*7*/	0b01000111,0b00000000,0b00010111,0b00000110,0b00000000,0b00000000,0b00001110,0b01100110,0b00010101,0b00011101,
/*8*/   0b01100111,0b00000000,0b00000101,0b01011011,0b00001111,0b00011100,0b00111110,0b00011110,0b00110111,0b00000000,
/*9*/	0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001000,0b00000000,0b01110111,0b00011111,0b00001101,
/*10*/	0b00111101,0b01001111,0b01000111,0b00000000,0b00010111,0b00000110,0b00000000,0b00000000,0b00001110,0b01110010,
/*11*/	0b00010101,0b00011101,0b01100111,0b00000000,0b00000101,0b01011011,0b00001111,0b00011100,0b00111110,0b00111100,
/*12*/	0b00110111,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000
};


  
   
    
    /* 
     * Create a new controler 
     * Params :
     * int dataPin	The pin on the Arduino where data gets shifted out
     * int clockPin	The pin for the clock
     * int csPin	The pin for selecting the device when data is to be sent
     * int numDevices	The maximum number of devices that can be controled
     */
    void LedControlInit();

    /*
     * Gets the maximum number of devices attached to
     * this LedControl.
     * Returns :
     * int the number of devices attached to this LedControl
     */
    uint8_t getDeviceCount();

    /* 
     * Set the shutdown (power saving) mode for the device
     * Params :
     * int addr		The address of the display to control
     * boolean b	If true the device goes into power-down mode. If false
     *			device goes into normal operation
     */
    void shutdown(uint8_t addr, uint8_t b);

    /* 
     * Set the number of digits (or rows) to be displayed.
     * See datasheet for sideeffects of the scanlimit on the brightness
     * of the display.
     * Params :
     * int addr		The address of the display to control
     * int limit The number of digits to be displayed
     * Only values between 0 (only 1 digit) and 7 (all digits) are valid.
     */
    void setScanLimit(uint8_t addr, uint8_t limit);

    /* 
     * Set the brightness of the display.
     * Params:
     * int addr		The address of the display to control
     * int intensity the brightness of the display. 
     * Only values between 0(darkest) and 15(brightest) are valid.
     */
    void setIntensity(uint8_t addr, uint8_t intensity);
	//void setIntensity_tmp(uint8_t addr, uint8_t intensity);
    /* 
     * Switch all Leds on the display off. 
     * Params:
     * int addr		The address of the display to control
     */
    void clearDisplay(uint8_t addr);

    /* 
     * Set the status for a specific Led.
     * Params :
     * int addr		The address of the display to control
     * int row		the row in which the led is located 
     *			Only values between 0 and 7 are valid.
     * int col		the column in which the led is located
     *			Only values between 0 and 7 are valid.
     * boolean state	If true the led is switched on, if false it is switched off
     */
    void setLed(uint8_t addr, uint8_t row, uint8_t col, uint8_t state);

    /* 
     * Set the 8 Led's in a row to a new state
     * Params:
     * int addr		The address of the display to control
     * int row		The row on which the led's are to be set
     *			Only values between 0 and 7 are valid.
     * byte value	A bit set to 1 in this value will light up the
     *			corresponding led.
     */
    void setRow(uint8_t addr, uint8_t row, uint8_t value);

    /* 
     * Set the 8 Led's in a column to a new state
     * Params:
     * int addr		The address of the display to control
     * int col		The column on which the led's are to be set
     *			Only values between 0 and 7 are valid.
     * byte value	A bit set to 1 in this value will light up the
     *			corresponding led.
     */
    void setColumn(uint8_t addr, uint8_t col, uint8_t value);

    /* 
     * Display a hexadecimal digit on a 7-Segment Display
     * Params:
     * int addr		the address of the display to control
     * int digit	the position of the digit on the display
     * byte value	the value to be displayed. 
     *			Only values between 0x00 and 0x0F are valid.
     * boolean dp	If true also switches on the decimal point.
     */
    void setDigit(uint8_t addr, uint8_t digit, uint8_t value, uint8_t dp);

    /* 
     * Display a character on a 7-Segment display.
     * The char that can be acutally displayed are obviously quite limited.
     * Here is the whole set :	
     *	'0','1','2','3','4','5','6','7','8','9','0',
     *  'A','b','c','d','E','F','H','L','P',
     *  '.','-','_',' ' 
     * Params:
     * int addr		the address of the display to control
     * int digit	the position of the character on the display
     * char value	the character to be displayed. (See the limited set above!) 
     * boolean dp	If true also switches on the decimal point.
     */
    void setChar(uint8_t addr, uint8_t digit, char value, uint8_t dp);


#endif	//LedControl.h
