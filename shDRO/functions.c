/*
 * functions.c
 *
 * Created: 19.09.2018 16:33:25
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 

#include <avr/pgmspace.h>
#include <string.h>

#include "hardware.h"
#include "dro_types.h"
#include "dro_config.h"
#include "keyboard.h"
#include "utils.h"
#include "transmission_data.h"
#include "LedControl.h"
#include "led.h"
#include "messages.h"
#include "shared_memory.h"
#include "cmd.h"

// инвертировать значение
void invert_sign(uint8_t device_no){
	// если номер устройства больше чем текущее кол-во устройств,
	// то инвертируем знак у каждого устройства
	/*if (device_no > COUNT_DEVICES){
		for (uint8_t i = 0; i<COUNT_DEVICES; i++){
			devices[i].dev_value.svalue.is_negative	= !devices[i].dev_value.svalue.is_negative;
			devices[i].start_value.is_negative	= devices[i].dev_value.svalue.is_negative;
		}
	}
	else{
		devices[device_no].dev_value.svalue.is_negative	= !devices[device_no].dev_value.svalue.is_negative;
		devices[device_no].start_value.is_negative	= devices[device_no].dev_value.svalue.is_negative;
	}*/
}
// установить значение в ноль
void set0(uint8_t device_no){
	// если номер устройства больше чем текущее кол-во устройств,
	// то устанавливаем в ноль значение каждого устройства
	/*if (device_no > COUNT_DEVICES){
		for (uint8_t i = 0; i<COUNT_DEVICES; i++){
			devices[i].start_value.is_negative	= devices[i].dev_value.svalue.is_negative;
			devices[i].start_value.value		= devices[i].dev_value.svalue.value;
			
			devices[i].dev_value.svalue.value		= 0;
			devices[i].dev_value.svalue.is_negative	= 0;
			devices[i].start_value.is_negative		= 0;
			
		}
	}
	else{
		
		devices[device_no].start_value.value		= devices[device_no].dev_value.svalue.value;
		
		devices[device_no].dev_value.svalue.value		= 0;
		devices[device_no].dev_value.svalue.is_negative	= 0;
		devices[device_no].start_value.is_negative		= 0; 
	}*/
}

void show_dro_info(transmission_source_e transmission_sources){
	
	if ((transmission_sources & LED) == LED){
		fprintf_P(led_s(LED_MAIN), MSG_PRODUCT_NAME);
		fprintf_P(led_s(LED_2), MSG_VERSION_NO);
		fprintf_P(led_s(LED_1), MSG_VERSION_DATA);
		#ifndef DEBUG
		_delay_ms(1000);
		#endif
		clear_all_led();
	}
}

void execute_func(uint8_t key_code){
	uint8_t is_F = key_code & 0x80;
	uint8_t k_code = key_code & 0x3f;
	
	if (is_F){

	}
	else{
		switch (k_code){
			case KEY_X:
				select_led(LED_2);
			break;
			
			case KEY_Y:
				select_led(LED_1);
			break;
			
			case KEY_Z:
				select_led(LED_0);
			break;
		}
	}
}

void scan_keyboard(){
		
	fprintf(led_s_ex(LED_MAIN, 0), "%02d F %d", keyboard.key_code, keyboard.is_F);
	
	if (!keyboard.is_processed){
		if (keyboard.key_code != KEY_NULL){
			if (keyboard.key_code == KEY_F)
				keyboard.is_F = !keyboard.is_F;
			else{
				execute_func(keyboard.key_code | (keyboard.is_F << 7));
				keyboard.is_F = 0;	
			}
						
			keyboard.is_processed = 1;
		}
	}
}






