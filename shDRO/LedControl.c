/*
  LedControl.cpp - A library for controling Led Digits or 
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

#include <avr/io.h>

#include "hardware.h"
#include "LedControl.h"

#include <util/delay.h>

//the opcodes the MAX7221 and MAX7219 understand
#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15


 /* We keep track of the led-status for 8 devices in this array */
 uint8_t status[MAX_LEDS * 8];
 /* This is the array we send the data with 2 commandbytes for 8 devices */
 uint8_t spidata[MAX_LEDS * 2];

 /* Send out a single command to the device */
void spiTransfer(uint8_t addr, uint8_t opcode, uint8_t data);

void LedControlInit() {
    
    DDR_SPI		|= (1<<SPI_MOSI)|(1<<SPI_CLK)|(1<<SPI_CS);
	PORT_SPI	|= (1<<SPI_CS);
	
	/*разрешение spi,старший бит вперед,мастер, режим 0*/
	SPCR = (1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0);
	SPSR = (0<<SPI2X);    
	
    for(uint8_t i=0;i<(MAX_LEDS * 8);i++) 
		status[i]=0x00;
		
    for(uint8_t i=0;i<MAX_LEDS;i++) {
		spiTransfer(i,OP_DISPLAYTEST,0);
		
		//scanlimit is set to max on startup
		setScanLimit(i,7);
		
		//decode is done in source
		spiTransfer(i,OP_DECODEMODE,0);
		clearDisplay(i);
		
		//we go into shutdown-mode on startup
		shutdown(i,1);
		
    }
}

uint8_t getDeviceCount() {
    return MAX_LEDS;
}

void shutdown(uint8_t addr, uint8_t b) {
    if(addr<0 || addr>=MAX_LEDS) return;
	
    if(b)
		spiTransfer(addr, OP_SHUTDOWN,0);
    else
		spiTransfer(addr, OP_SHUTDOWN,1);
}
	
void setScanLimit(uint8_t addr, uint8_t limit) {
    if(addr<0 || addr>=MAX_LEDS) return;
	
    if(limit>=0 || limit<8)
    	spiTransfer(addr, OP_SCANLIMIT,limit);
}

void setIntensity(uint8_t addr, uint8_t intensity) {
    if(addr<0 || addr>=MAX_LEDS) return;
	
    if(intensity>=0 || intensity<16)	
		spiTransfer(addr, OP_INTENSITY,intensity);
    
}

void clearDisplay(uint8_t addr) {
    uint8_t offset;

    if(addr<0 || addr>=MAX_LEDS) return;
	
    offset=addr*8;
    
	for(uint8_t i=0;i<8;i++) {
		status[offset+i]=0;
		spiTransfer(addr, i+1,status[offset+i]);
    }
}



void setLed(uint8_t addr, uint8_t row, uint8_t column, uint8_t state) {
    uint8_t offset;
    uint8_t val=0x00;

    if(addr<0 || addr>=MAX_LEDS) return;
	
    if(row<0 || row>7 || column<0 || column>7)	return;
	
    offset=addr*8;
    val=0b10000000 >> column;
    
	if(state)
		status[offset+row]=status[offset+row]|val;
    else {
		val=~val;
		status[offset+row]=status[offset+row]&val;
    }
    spiTransfer(addr, row+1,status[offset+row]);
}
	
void setRow(uint8_t addr, uint8_t row, uint8_t value) {
    if(addr<0 || addr>=MAX_LEDS) return;
	if(row<0 || row>7)	return;
	
    uint8_t offset=addr*8;
    status[offset+row]=value;
    spiTransfer(addr, row+1,status[offset+row]);
}
    
void setColumn(uint8_t addr, uint8_t col, uint8_t value) {
    if(addr<0 || addr>=MAX_LEDS) return;
    if(col<0 || col>7) 	return;
	
    uint8_t val;
	for(uint8_t row=0;row<8;row++) {
		val=value >> (7-row);
		val=val & 0x01;
		setLed(addr,row,col,val);
    }
}

void setDigit(uint8_t addr, uint8_t digit, uint8_t value, uint8_t dp) {
    uint8_t offset;
    uint8_t v;

    if(addr<0 || addr>=MAX_LEDS) return;
    if(digit<0 || digit>7 || value>15) return;
	
    offset=addr*8;
    //v=charTable[value];
	v = pgm_read_byte(&(charTable[value]));
    
	if(dp)
		v|=0b10000000;
    
	status[offset+digit]=v;
    spiTransfer(addr, digit+1,v);
    
}

void setChar(uint8_t addr, uint8_t digit, char value, uint8_t dp) {
    uint8_t offset;
    uint8_t index,v;

    if(addr<0 || addr>=MAX_LEDS) return;
    if(digit<0 || digit>7) return;
	
    offset=addr*8;
    index=(uint8_t)value;
    
	if(index >127) {
	//nothing define we use the space char
		value=32;
    }
	
    //v=charTable[index];
	v = pgm_read_byte(&(charTable[index]));
	
    if(dp)
		v|=0b10000000;
    
	status[offset+digit]=v;
    spiTransfer(addr, digit+1,v);
}

inline void spiTransfer(uint8_t addr, volatile uint8_t opcode, volatile uint8_t data) {
	//spiTransfer_ex(addr, opcode, data, 0, 7);
	//Create an array with the data to shift out
	uint8_t offset=addr*2;
	uint8_t maxbytes=MAX_LEDS*2;

	for(uint8_t i=0; i<maxbytes; i++)
		spidata[i]= 0;
      
      //put our device data into the array
	spidata[offset+1]=opcode;
	spidata[offset]=data;
     
	PORT_SPI &= ~(1 << SPI_CS);
      
	for(int i=maxbytes;i>0;i--){
		SPDR = spidata[i-1];
		while( !(SPSR & (1<<SPIF)) ){}
	}
	
	PORT_SPI |= (1<<SPI_CS);
}    




