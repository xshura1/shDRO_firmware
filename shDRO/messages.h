/*
 * messages.h
 *
 * Created: 21.10.2018 16:55:09
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef MESSAGES_H_
#define MESSAGES_H_

#include <avr/pgmspace.h>


const static char MSG_PRODUCT_NAME[]	PROGMEM = " shDRO"; 
const static char MSG_VERSION_NO[]		PROGMEM = "ver. 0.2";
const static char MSG_VERSION_DATA[]	PROGMEM = "01.11.2019";

const static char MSG_EXIT[]			PROGMEM = "Exit";

//------------------------обучение и тестирование клавиатуры-------------------------------------------------

const static char MSG_PRESS[]			PROGMEM	= "Press %02d";
const static char MSG_DOUBLE[]			PROGMEM = "Double.%02d";
const static char MSG_WAIT[]			PROGMEM = "Wait";
const static char MSG_ADC_VALUE[]		PROGMEM = "Adc.%d=%03d";
const static char MSG_ADC_EMPTY[]		PROGMEM = "Adc.%d    ";
const static char MSG_TEST[]			PROGMEM = "Test ? %d";
const static char MSG_CODE[]			PROGMEM = "Code  %02d";

const static char MSG_DOTS[]			PROGMEM = "...";
const static char MSG_CLRF[]			PROGMEM = "\r\n";

//-----------------------------------------------------------------------------------------------------------
//----------------------для индикаторов----------------------------------------------------------------------
const static char LMSG_SCAN[]			PROGMEM = "SCAn  ";
const static char LMSG_SCAN_CH[]		PROGMEM = "SCAn.Ch %d";
//-----------------------------------------------------------------------------------------------------------

#endif /* MESSAGES_H_ */
