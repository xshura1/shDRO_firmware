/*
 * eeprom.c
 *
 * Created: 02.10.2018 16:52:17
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 

#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include "hardware.h"

// Чтение из EEPROM
uint8_t eeprom_read_data(uint16_t address){
	while(EECR & (1 << EERE));	// Ждем готовности
	EEAR = address;				// Загружаем требуемый адрес в регистр EEAR
	EECR |= (1 << EERE);		// Устанавливаем в «1» разряд EERE регистра EECR
	return EEDR;				// Возвращаем данные из регистра данных
}

void eeprom_read_array(uint16_t begin_address, uint8_t * p_array, uint8_t size, uint8_t * crc, uint16_t * end_address){
	uint16_t address = begin_address;
	for(uint8_t i=0; i<size; i++){
		p_array[i] = eeprom_read_data(address);
		address++;
	}
	
	*crc = eeprom_read_data(address);
	*end_address = address;
}

// Запись в EEPROM
void eeprom_write_data(uint16_t address, uint8_t data){
	//uint8_t old_data = eeprom_read_data(address);
	
	// TODO некорректно отрабатывает сравнение считанного байта из eeprom и текущего
	//if (old_data != data){ // Проверяем данные
	
		while(EECR & (1 << EEPE)); // ожидание завершения предыдущей записи
		cli(); 
		//EECR |= (1<<EEPM0) | (1<<EEPM1);
		EEAR = address; 
		EEDR = data; 
		
		// ПРИМЕЧАНИЕ.
		// При отладке в AtmelStudio, нужно включить оптимизацию, т.к.
		// без включенной оптимизации между выставлением бита EEMPE и EEPE кол-во тактов превысит 4
		// в результате, в отладчике запись байта не произойдет
		EECR |= (1 << EEMPE);	// установка бита записи в EEPROM
		EECR |= (1 << EEPE);	// запись EEDR в EEPROM (запись произойдет только в случае, если прошло не более 4 тактов)
		sei(); // Разрешаем прерывания
	//}
}

void eeprom_write_array(uint16_t begin_address, uint8_t * p_array, uint8_t size, uint8_t crc, uint16_t * end_address){
	uint16_t address = begin_address;
	
	for (uint8_t i=0; i<size; i++){
		eeprom_write_data(address, p_array[i]);
		address++;
	}
	eeprom_write_data(address, crc);
	*end_address = address;
}


