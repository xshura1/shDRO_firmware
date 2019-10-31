/*
 * keyboard.c
 *
 * Created: 22.09.2018 14:29:05
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
*/ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>


#include "hardware.h"
#include "dro_config.h"
#include "keyboard.h"
#include "dro_types.h"
#include "LedControl.h"
#include "utils.h"
#include "transmission_data.h"
#include "messages.h"
#include "eeprom.h"



#define ADC_TOLERANCE			3	// минимальная разница между значениями соседними значениями ADC
									// если разница меньше, то считаем, что нажали уже ранее обработанную кнопку

#define MAX_COUNT_SAMPLES		64 

// номер опрашиваемого канала adc
volatile	uint8_t adc_ch			= 0;
#define ADC_CH (adc_ch - ADC_CH_BEGIN)



typedef enum {
	BEGIN_REPAIR_KEYBOARD,
	WAITING_RELEASE_KEY,
	BEGIN_SCAN_KEY,
	SCAN_KEY,
	END_SCAN_KEY,
	DUPLICATE_KEY,
	END_REPAIR_KEYBOARD
} test_keyboard_state_e;


uint8_t load_keyboard_from_eeprom(void);
uint8_t get_key_code(uint8_t adc_value);


// время выполнения ~5мкс.
ISR(ADC_vect){
	
	uint8_t new_adc			= ADCH;
	uint8_t a_ch			= ADC_CH;
	uint8_t new_code		= KEY_NULL;
	uint8_t is_need_change_channel = 1;
	uint8_t tmp_adc = 0;
		
	static uint8_t	old_code;
	static uint8_t	old_ch;
	static uint8_t	time_cnt_ms;
	static uint8_t	cnt_sampl;
	static uint8_t	cnt_sampl_code;
	static uint16_t sum_code;
	static uint8_t	cnt_repeat_code;
	
	
	is_need_change_channel = 0;
	
	if (new_adc < ADC_MAX_NULL_VALUE){
		new_adc = 0;
	}
	
	cnt_sampl++;
	
	if (cnt_sampl < MAX_COUNT_SAMPLES){
		sum_code+=new_adc;
	}
	
	if (cnt_sampl == MAX_COUNT_SAMPLES){
		
		tmp_adc = sum_code/cnt_sampl;
		sum_code = 0;
		cnt_sampl = 0;
		
		is_need_change_channel = 1;
		
		keyboard.adc_value = tmp_adc;
		keyboard.adc_channel = a_ch;
		
		if (tmp_adc > ADC_MAX_NULL_VALUE){
			keyboard.is_pressed_key |= (1<<a_ch);
		}
		else{
			keyboard.is_pressed_key &= ~( 1 << a_ch);
		}
		
		
		if (keyboard.is_repair){
			repair_keyboard(0);
		}
		else{
			if (!keyboard.is_processed){
				is_need_change_channel = 0;
				new_code = get_key_code(tmp_adc);
				
				if (old_code != new_code){
					cnt_sampl_code = 0;
				}
				else{
					cnt_sampl_code++;
				}
				
				old_code = new_code;
				
				if (cnt_sampl_code > 0){
					uint8_t time_10_ms = TIME_KEY_REPEAT_10_ms;
					
					if (cnt_repeat_code > 10)
						time_10_ms = time_10_ms / 10;
					else
						if (cnt_repeat_code > 4)	
							time_10_ms = time_10_ms / 2;
					
					
					if ( ((old_code == KEY_NULL) && (old_ch == a_ch)) || 
						 ((old_code != KEY_NULL) && (keyboard.key_code != old_code)) ||
						 ((TIME_OFFSET_8(time_cnt_ms, timer_cnt_10ms) > time_10_ms) && (keyboard.key_code == old_code)) ){
						
						if (keyboard.key_code == old_code)
							cnt_repeat_code++;
						else
							cnt_repeat_code = 0;	
							
						
						keyboard.key_code = old_code;
						time_cnt_ms = timer_cnt_10ms;
						old_ch = a_ch;
						old_code = KEY_NULL;
						keyboard.is_processed = 1;
					}
						
					
					is_need_change_channel = 1;
				}
			}
		}
	}
	
	start_keyboard(is_need_change_channel);
}


uint8_t get_key_code(uint8_t adc_value){
	uint8_t result = 0;
	for (uint8_t i=0; i<KEY_COUNT; i++){
		if (keyboard.adc_mappings[i].channel == ADC_CH){
			if (adc_value >= (keyboard.adc_mappings[i].adc_value - ADC_TOLERANCE)){
				result = keyboard.adc_mappings[i].key_code;
				break;
			}
		}
	}
	return result;
}

device_led_e get_ADC_led(uint8_t adc_ch_no){
	if (adc_ch_no <= LED_2){
		return LED_2 + ADC_CH_BEGIN - adc_ch_no;
	}
	else{
		return LED_2 - (adc_ch_no - ADC_CH_BEGIN);
	}
}

void set_leds_adc_value_empty(uint8_t curr_a_ch){
	for (uint8_t i=0; i<ADC_COUNT_CHANNELS; i++){
		fprintf_P(led_s_ex(get_ADC_led(i + ADC_CH_BEGIN), 0), MSG_ADC_EMPTY, i + ADC_CH_BEGIN);
	}
}

uint8_t init_keyboard(){
	
	uint8_t result = load_keyboard_from_eeprom();
	keyboard.is_repair	= 0;
	
	adc_ch = ADC_CH_BEGIN;
	
	//ион - напряжение питания, выравнивание влево
	ADMUX = (0<<REFS1)|(1<<REFS0)|(1<<ADLAR); 
	
	ADMUX |= 0x07;
	ADMUX &= ((0x1F << 3) | adc_ch);
	
	//вкл. ацп, режим одиночного преобр., разрешение прерывания,частота преобр. = FCPU/128
	ADCSRA = (1<<ADEN)|(0<<ADSC)|(0<<ADATE)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
	#ifdef DEBUG
		return 1;
	#endif
	
	return result;
}

void start_keyboard(uint8_t change_ch){
			
	// если нужно изменить канал adc для считывания
	// то, считываем текущий канал считывания и увеличиваем его
	// далее полученное значение записываем в регистр ADMUX
	if (change_ch){
		adc_ch = (ADMUX & 0x07);
		adc_ch = CHANGE_ADC(adc_ch);
						
		ADMUX |= 0x07;
		ADMUX &= ((0x1F << 3) | adc_ch);
	}
		
	ADCSRA |= (1<<ADSC);
}

void repair_keyboard(uint8_t is_init){
	adc_mapping_s  tmp_mapping;
	uint8_t fl = 0;
	
	static test_keyboard_state_e state;
	static uint8_t	request_key_code;
	static uint8_t is_scan_key_completed;
	
	if (is_init){
		keyboard.is_repair	= 1;
		state = BEGIN_REPAIR_KEYBOARD;
		return;
	}
	
	switch (state){
		case BEGIN_REPAIR_KEYBOARD:
		// перед тестом клавиатуры, очищаем все индикаторы
		for (uint8_t i=0; i<MAX_LEDS; i++){
			clearDisplay(i);
		}
		
		request_key_code = 1;
		
		for (uint8_t i=0; i<KEY_COUNT; i++){
			keyboard.adc_mappings[i].adc_value	= 0;
			keyboard.adc_mappings[i].key_code = 0;
			keyboard.adc_mappings[i].channel = 0;
		}
		
		state	= WAITING_RELEASE_KEY;
		break;
		
		case WAITING_RELEASE_KEY:
		// дожидаемся отжатия кнопки
		fprintf_P(led_s_ex(LED_MAIN, 0), MSG_WAIT);
		if (!keyboard.is_pressed_key){
			state = BEGIN_SCAN_KEY;
		}
		break;
		
		case BEGIN_SCAN_KEY:
		// кнопку отжали, можно начинать скан кнопки
		fprintf_P(led_s(LED_MAIN), MSG_PRESS, request_key_code);
		keyboard.adc_value = 0;
		is_scan_key_completed = 0;
		state = SCAN_KEY;
		break;
		
		case SCAN_KEY:
		// на определнном индикаторе отображаем значение ADC канала
		set_leds_adc_value_empty(keyboard.adc_channel);
		if (keyboard.is_pressed_key){
			fprintf_P(led_s_ex(LED_MAIN,0), LMSG_SCAN);
			
			
			if (keyboard.adc_value > ADC_MAX_NULL_VALUE){
				fprintf_P(led_s_ex(get_ADC_led(keyboard.adc_channel + ADC_CH_BEGIN), 0),
				MSG_ADC_VALUE,
				keyboard.adc_channel + ADC_CH_BEGIN,
				keyboard.adc_value);
				
				// в сохраненных значениях кодов ADC текущего канала ищем совпадения кодов
				// если нашли, значит была нажата ранее обработанная кнопка
				// TODO сохранение нового кода и определение дубля можно сделать в один проход
				for (uint8_t i=0; i<KEY_COUNT; i++){
					if (keyboard.adc_mappings[i].key_code != 0){
						if (keyboard.adc_mappings[i].key_code != request_key_code){
							if (keyboard.adc_mappings[i].channel == keyboard.adc_channel){
								if (abs(keyboard.adc_mappings[i].adc_value - keyboard.adc_value) <= ADC_TOLERANCE){
									fprintf_P(led_s(LED_MAIN), MSG_DOUBLE, keyboard.adc_mappings[i].key_code);
									state = DUPLICATE_KEY;
									break;
								}
							}
						}
					}
					else
					break;
				}
				
				is_scan_key_completed = 1;
				
				if (state == DUPLICATE_KEY) break;
				
				for (uint8_t i=0; i<KEY_COUNT; i++){
					if (keyboard.adc_mappings[i].key_code == request_key_code){
						if (keyboard.adc_mappings[i].adc_value < keyboard.adc_value){
							keyboard.adc_mappings[i].adc_value = keyboard.adc_value;
						}
						break;
					}
					else
					if (keyboard.adc_mappings[i].key_code == 0){
						keyboard.adc_mappings[i].channel =  keyboard.adc_channel;
						keyboard.adc_mappings[i].key_code = request_key_code;
						keyboard.adc_mappings[i].adc_value = keyboard.adc_value;
						break;
					}
				}
			}
		}
		
		if (is_scan_key_completed){
			state = END_SCAN_KEY;
		}
		
		break;
		
		
		case END_SCAN_KEY:
		if (!keyboard.is_pressed_key){
			request_key_code++;
			if (request_key_code > KEY_COUNT){
				state = END_REPAIR_KEYBOARD;
			}
			else{
				state = BEGIN_SCAN_KEY;
			}
		}
		else{
			state = SCAN_KEY;
		}
		break;
		
		case DUPLICATE_KEY:
		if (!keyboard.is_pressed_key){
			state = BEGIN_SCAN_KEY;
		}
		break;
		
		case END_REPAIR_KEYBOARD:

		do {
			fl = 0;
			for ( uint8_t i = 1; i < KEY_COUNT; i++) {
				uint16_t compI = 0;
				uint16_t compI1 = 0;
				compI	|= (keyboard.adc_mappings[i].channel << 8)	| keyboard.adc_mappings[i].adc_value;
				compI1	|= (keyboard.adc_mappings[i-1].channel << 8) | keyboard.adc_mappings[i-1].adc_value;
				if (compI > compI1){
					tmp_mapping = keyboard.adc_mappings[i];
					keyboard.adc_mappings[i] = keyboard.adc_mappings[i-1];
					keyboard.adc_mappings[i-1] = tmp_mapping;
					fl = 1;
				}
			}
		} while (fl);
		
		fprintf_P(led_s(LED_MAIN), MSG_EXIT);
		_delay_ms(1000);
		clear_all_led();
		keyboard.is_repair = 0;
		
		break;
	}
	
}


uint8_t load_keyboard_from_eeprom(void){
	
	uint8_t * keyboard_data = (uint8_t*)&keyboard.adc_mappings;
	for (uint16_t i=0; i<sizeof(keyboard.adc_mappings); i++)
		keyboard_data[i] = eeprom_read_data(i + EEPROM_KEYBOARD_ADDRESS_BEGIN);
	
	uint8_t crc_eeprom = eeprom_read_data(sizeof(keyboard.adc_mappings) + EEPROM_KEYBOARD_ADDRESS_BEGIN);
	uint8_t crc_curr = calc_crc(keyboard_data, sizeof(keyboard.adc_mappings));
	return (crc_curr == crc_eeprom);
}

void save_keyboard_to_eeprom(void){
		
	uint8_t * keyboard_data = (uint8_t*)&keyboard.adc_mappings;
	uint8_t crc = calc_crc(keyboard_data, sizeof(keyboard.adc_mappings));
	uint16_t size = sizeof(keyboard.adc_mappings);
	
	for (uint16_t i=0; i<size; i++)
		eeprom_write_data(i + EEPROM_KEYBOARD_ADDRESS_BEGIN, keyboard_data[i]);
	
	eeprom_write_data(sizeof(keyboard.adc_mappings) + EEPROM_KEYBOARD_ADDRESS_BEGIN, crc);	
	
}
