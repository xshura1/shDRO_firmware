/*
 * hardware.h
 *
 * Created: 07.02.2019 21:36:05
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef HARDWARE_H_
#define HARDWARE_H_


#define F_CPU			16000000L
#define UART_BAUDRATE	250000

#include <util/delay.h>

// доступ к порту канала
#define DDR_CH(ch_no)	(ch_no < 6 ? &DDRD	: &DDRC)
#define PORT_CH(ch_no)	(ch_no < 6 ? &PORTD	: &PORTC)
#define PIN_CH(ch_no)	(ch_no < 6 ? &PIND	: &PINC)

// номер бита канала
#define NO_BIT_CH(ch_no) (ch_no < 6 ? ch_no + 2 : ch_no - 6)

// к какой группе принадлежит канал
// каналы разбиты на две группы: 6 каналов порта B (группа 1)
// и 6 каналов порта D (группа 2)
#define GROUP_NO(ch_no) (ch_no < 6 ? 1 : 2)

// группы (порты) каналов
#define DATA_1	PIND
#define DATA_2	PINC

// прерывание канала
#define INTERRUPT_CH_NO(ch_no) (ch_no < 6 ? INTERRUPT_2 : INTERRUPT_1)

// индикаторы
#define SPI_MOSI	PB3	/* Data is shifted out of this pin*/
#define SPI_CLK		PB5	/* The clock is signaled on this pin */
#define SPI_CS		PB2 /* This one is driven LOW for chip selectzion */

#define PORT_SPI	PORTB
#define DDR_SPI		DDRB


#define MAX_LEDS	4 /* The maximum number of devices to be used */
#define LED_MAIN	LED_3

// динамик
#define DDR_BUZZER	DDRB
#define PIN_BUZZER	PB1

// клавиатура
#define KEYBOARD_ADC1	6
#define KEYBOARD_ADC2	7
#define CHANGE_ADC(adc_no) (adc_no == KEYBOARD_ADC1 ? KEYBOARD_ADC2 : KEYBOARD_ADC1)

// EEPROM
#define EEPROM_KEYBOARD_ADDRESS_BEGIN 0x0100
#define EEPROM_KEYBOARD_ADDRESS_END		EEPROM_KEYBOARD_ADDRESS_BEGIN + SIZE_MEMORY_KEYBOARD - 1

#define EEPROM_CHANNELS_ADDRESS_BEGIN	EEPROM_KEYBOARD_ADDRESS_END + 1
#define EEPROM_CHANNELS_ADDRESS_END		EEPROM_CHANNELS_ADDRESS_BEGIN + SIZE_MEMORY_CHANNELS - 1

#define EEPROM_DRO_CONFIG_ADDRESS_BEGIN	EEPROM_CHANNELS_ADDRESS_END + 1
#define EEPROM_DRO_CONFIG_ADDRESS_END	EEPROM_DRO_CONFIG_ADDRESS_BEGIN + SIZE_DRO_CONFIG - 1

#define EEPROM_DEVICES_ADDRESS_BEGIN	EEPROM_DRO_CONFIG_ADDRESS_END + 1



#endif /* HARDWARE_H_ */