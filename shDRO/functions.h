/*
 * functions.h
 *
 * Created: 19.09.2018 16:33:52
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "dro_types.h"

device_ex_t * add_device_f(void * args, uint8_t size);

// показать информацию о dro (версия, конфигурация и т.д.)
void show_dro_info(transmission_source_e transmission_sources);

// инвертировать значение
void invert_sign(uint8_t device_no);

// установить значение в ноль
void set0(uint8_t device_no);

void scan_keyboard();




#endif /* FUNCTIONS_H_ */