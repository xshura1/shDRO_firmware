/*
 * eeprom.h
 *
 * Created: 02.10.2018 16:52:29
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

uint8_t eeprom_read_data(uint16_t address);
void eeprom_read_array(uint16_t begin_address, uint8_t * p_array, uint8_t size, uint8_t * crc, uint16_t * end_address);
void eeprom_write_data(uint16_t address, uint8_t data);
void eeprom_write_array(uint16_t begin_address, uint8_t * p_array, uint8_t size, uint8_t crc, uint16_t * end_address);

#endif /* EEPROM_H_ */