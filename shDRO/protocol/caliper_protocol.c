/*
 * caliper_protocol.c
 *
 * Created: 27.03.2019 0:07:02
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 

#include "..\hardware.h"
#include "..\dro_config.h"
#include "..\protocol.h"
#include "..\dro_types.h"
#include "..\utils.h"
#include "..\transmission_data.h"

#define TIME_WAIT_BIN6	8	// *10ms = 80ms
#define TIME_WAIT_24BIT	25	// *10ms = 250ms


void caliper_init(device_ex_t* device){
	capacity_arguments_s * arg = (capacity_arguments_s *)(device->arguments);
	
	device->state			= READY_STATE;
	device->pos_buffer		= 0;
	arg->timer_value_10ms	= 0;
}

void caliper_get(device_ex_t* device){
	capacity_arguments_s * arg = (capacity_arguments_s *)(device->arguments);
	
	uint8_t time_offset = TIME_OFFSET_8(timer_cnt_10ms, arg->timer_value_10ms);
	
	uint8_t		first_bit_no	= 0;
	uint8_t		end_bit_no		= 0;
	uint32_t	mask			= 0;
	uint8_t		time_wait		= 0;
	
	
	switch (device->protocol){
		case BIN6_PROTOCOL:
			first_bit_no	= 0;
			end_bit_no		= 23;
			mask			= 0xffffff;
			time_wait		= TIME_WAIT_BIN6;
		break;
		
		case BIT24_PROTOCOL:
			first_bit_no	= 24;
			end_bit_no		= 47;
			mask			= 0xfffff8;
			//mask			= 0x7FFFF8;
			time_wait		= TIME_WAIT_24BIT;
		break;
		
		default:
		break;
	}
	
	
			
	if (device->state == READY_STATE){
		if (time_offset > time_wait){
			device->state	= PROCESSING_DATA_STATE;
			arg->cnt_bits	= 0;
			arg->buffer		= 0;
		}
	}
			
	arg->timer_value_10ms = timer_cnt_10ms;
			
	uint8_t transform_data	= (data_buf[device->pos_buffer] >> NO_BIT_CH(device->channel_numbers[0]));
	uint8_t int_mask		= channels[device->channel_numbers[0]].int_level;
			
	if (((transform_data & 0b1) == int_mask) || ((transform_data | 0b11) == int_mask)){
		// приняли нужное прерывание
		if (device->state == PROCESSING_DATA_STATE){
			
			if (arg->cnt_bits >= first_bit_no)
				arg->buffer |= (uint32_t)((((data_buf[device->pos_buffer])) >> NO_BIT_CH(device->channel_numbers[1])) & 1) << (arg->cnt_bits - first_bit_no);
				
			arg->cnt_bits++;
			
			if (arg->cnt_bits > end_bit_no){
				arg->transmit_buffer	&= mask;
				arg->buffer				&= mask;
				
				// если значение буфера изменилось, то передаем информацию на устройства отображения
				// первый пакет данных, после сброса, обрабатываем всегда, далее сравниваются предыдущее и полученное значения
				if ((arg->transmit_buffer != arg->buffer) || device->is_reset){
					arg->transmit_buffer	= arg->buffer;
					device->state			= TRANSMISSION_DATA_STATE;
					device->is_reset		= 0;
				}
				else
					device->state = READY_STATE;
			
				arg->buffer	= 0;
				arg->cnt_bits	= 0;
			}
		}
	}
}

void caliper_transmit(device_ex_t* device){
	capacity_arguments_s * arg = (capacity_arguments_s *)(device->arguments);
	
	uint8_t data_is_invert = channels[device->channel_numbers[1]].is_invert;
	uint32_t val = arg->transmit_buffer;
	uint8_t is_negative = 0;
	
	device->curr_value.dp_pos = 0xF;
	device->curr_value.dp_pos = (device->unit == UV_MM ? 2 : 4);
	
	
	// bin6
		if (data_is_invert)
			val ^= 0xffffff;
		
		is_negative = ((val & 0x100000) != 0);
			
		if ((val & 0x800000) == 0){
			val &= 0xFFFFF;
			// считанное значение в мм.
			// в зависимости от заданной единицы измерения, пересчитываем
			device->curr_value.value = (device->unit == UV_MM ? (int32_t)(val) : (int32_t)( ((val*1000 + 127)/254)/5*5) );
		}
		else{
			val &= 0xFFFFF;
			// считанное значение в дюймах
			// в зависимости от заданной единицы измерения, пересчитываем
			device->curr_value.value = (device->unit == UV_MM ? (int32_t)((val*2540 + 1000)/2000) : (int32_t)(val*10000/2000) );
			
		}
		
		
		
		// 24bit
	/*	if (data_is_invert)
		val ^= 0xffffff;
		
		is_negative = ((val & 0x100000) != 0);
		
		
		
		if (is_negative){
			// РѕС‚СЂРёС†Р°С‚РµР»СЊРЅРѕРµ Р·РЅР°С‡РµРЅРёРµ
			val ^= 0xffffff;
			val = val + 1;
		}
		
		
		val &= 0xFFFFF;
		//fprintf(led_s(LED_2), "%ld", val);
		// СЃС‡РёС‚Р°РЅРЅРѕРµ Р·РЅР°С‡РµРЅРёРµ РІ РґСЋР№РјР°С…
		// РІ Р·Р°РІРёСЃРёРјРѕСЃС‚Рё РѕС‚ Р·Р°РґР°РЅРЅРѕР№ РµРґРёРЅРёС†С‹ РёР·РјРµСЂРµРЅРёСЏ, РїРµСЂРµСЃС‡РёС‚С‹РІР°РµРј
		
		if (device->unit == UV_INCH)
		device->curr_value.value = (int32_t)(val*10000/20480);
		else
		if (device->unit == UV_MM)
		device->curr_value.value = (int32_t)((val*2540 + 10240)/20480);
		else
		val = 0;*/
		
		
		//device->curr_value.value = val;
		
		if (is_negative)
			device->curr_value.value = -device->curr_value.value;

		transmit_value(&device->curr_value, device->led, LED);
		
		
}