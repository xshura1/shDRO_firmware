/*
 * protocol.c
 *
 * Created: 26.07.2018 16:36:17
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

#include "protocol.h"
#include "timers.h"
#include "dro_config.h"
#include "hardware.h"
#include "error.h"
#include "transmission_data.h"


void caliper_init(device_ex_t* device);
void caliper_get(device_ex_t* device);
void caliper_transmit(device_ex_t* device);

void quad_init(device_ex_t* device);
void quad_get(device_ex_t* device);
void quad_transmit(device_ex_t* device);

// количество протоколов 
#define COUNT_PROTOCOLS				6
// количество обработчиков событий для каждого устройства
#define COUNT_PROTOCOLS_HANDLERS	7



const static void* protocol_handlers[COUNT_PROTOCOLS][COUNT_PROTOCOLS_HANDLERS] =
{/*	init_device_handler		get_data_handler		set_data_handler		transmission_handler		set_zero_handler		change_unit_handler		set_fast_mode_handler*/
	{	caliper_init,			caliper_get,			NULL,					caliper_transmit,			NULL,					NULL,					NULL},
	{	NULL,					NULL,					NULL,					NULL,						NULL,					NULL,					NULL},
	{	caliper_init,			caliper_get,			NULL,					caliper_transmit,			NULL,					NULL,					NULL},
	{	caliper_init,			caliper_get,			NULL,					caliper_transmit,			NULL,					NULL,					NULL},
	{	quad_init,				quad_get,				NULL,					quad_transmit,				NULL,					NULL,					NULL},
	{	NULL,					NULL,					NULL,					NULL,						NULL,					NULL,					NULL}
};

arguments_size_s get_arguments_size(device_protocol_e protocol, device_function_e func){
	arguments_size_s result;
	result.arguments_size			= 0;
	result.stored_arguments_size	= 0;
	
	switch(protocol){
		case BIT24_PROTOCOL:
		case BCD7_PROTOCOL:
		case BIN6_PROTOCOL:
			result.arguments_size = sizeof(capacity_arguments_s);
			result.stored_arguments_size = sizeof(stored_capacity_arguments_s);
		break;
		
		case QUAD_PROTOCOL:
			switch(func){
				case DA_AXIS_X:
				case DA_AXIS_Y:
				case DA_AXIS_Z:
					result.arguments_size = sizeof(optical_arguments_s);
					result.stored_arguments_size = sizeof(stored_optical_arguments_s);
				break;
				
				case DA_TACHOMETER:
					result.arguments_size = sizeof(tachometer_arguments_s);
					result.stored_arguments_size = sizeof(stored_tachometer_arguments_s);
				break;
				
				default:
				break;
			}
		break;
		
		default:
		break;
	}
	
	return result;
}

void execute_handler(device_handler_type_e handler_type, device_ex_t * device){
	void(* handler)	(device_ex_t * device) = NULL;
	if (device == NULL) return;
	
	handler = protocol_handlers[device->protocol][handler_type];
	if (handler == NULL) return;
	
	handler(device);
	
	
}

ISR(PCINT1_vect){
	data_2[pos_2] = DATA_2;
	
	if (is_test_channels){
		time_buf[pos_2] = timer1_cnt_8us;
		timer1_cnt_8us = 0;
	}
	
	pos_2++;
}


// время выполнения ~1.4мкс.
ISR(PCINT2_vect){
	data_1[pos_1] = DATA_1;
	
	if (is_test_channels){
		time_buf[pos_1] = timer1_cnt_8us;
		timer1_cnt_8us = 0;
	}
	
	pos_1++;
	
}



/*
Т.к. есть два буфера данных, то для удобства работы
присваиваем ссылки на буфер данных и позицию в буфере 
в зависимости от того, на какие прерывания настроено устройство.
*/
inline void change_buf(device_ex_t * device){
	if (device->int_group_no == 1){	
		data_buf	= data_1;
		pos_buf		= pos_1;
	}
	else{
		data_buf	= data_2;
		pos_buf		= pos_2;
	}
		
}

// TODO нужно отслеживать время простоя, и, если, время больше какого-то значения, то выдавать ошибку (для разных протоколов по разному)

void get_data(){
	device_ex_t * device = NULL;
	void(* handler)	(device_ex_t * device) = NULL;
	
	for (uint8_t i = 0; i<dro_config.count_devices; i++)
		if (!dro_config.devices[i].error){ // некорректно работающие устройства не обрабатываем
			device = (device_ex_t *)&dro_config.devices[i];
			
			handler = protocol_handlers[device->protocol][DHT_GET_DATA];
			if (handler == NULL) continue;
			
			change_buf(device);
						
			while (device->pos_buffer != pos_buf){
				if ((data_buf[device->pos_buffer] ^ data_buf[device->old_pos_buffer]) & get_int_mask(device)){
					
					handler(device);							
					
					device->old_pos_buffer = device->pos_buffer;
				}
				device->pos_buffer++;
			}
		}
}


void tracsmission_data(){
	for (uint8_t i =0; i<dro_config.count_devices; i++)
		if (dro_config.devices[i].state == TRANSMISSION_DATA_STATE){
			execute_handler(DHT_TRANSMISSION, &dro_config.devices[i]);
			dro_config.devices[i].state = READY_STATE;
		}
	
}

// проверка конфигурации каналов устройства
// сначала производится общая проверка каналов, независимо от заданного протокола обмена:
//		если в рамках одного устройства есть каналы, настроенные на прерывание, то данные каналы
//		должны принадлежать одной группе
// затем производится проверка каналов в зависимости от заданного протокола обмена
void check_config_device_ex(device_ex_t * device){
	uint8_t group_no = 0;
	
	for (uint8_t i=0; i<device->count_channels; i++)
		if (channels[device->channel_numbers[i]].enable_interrupt){
			if ((group_no != GROUP_NO(device->channel_numbers[i])) && (group_no > 0)){
				device->error = CRF_INVALID_CHANNEL_TYPE;
				break;
			}
			else
				device->int_group_no = GROUP_NO(device->channel_numbers[i]);
	}
	
	if (device->error) return;
		
	switch (device->protocol){
				
		case BIT24_PROTOCOL:
		case BIN6_PROTOCOL:
		case BCD7_PROTOCOL:
			check_chinese_caliper(device);
		break;
		
		case QUAD_PROTOCOL:
			check_quad(device);
		break;
		
		default:
			device->error = CRF_INVALID_PROTOCOL;
	}
}

/*
 для китайских штангенциркулей должны быть выделены как минимум два канала с типами: TCH_CLK_IN, TCH_DATA_IN
 при этом, первым должен быть TCH_CLK_IN, вторым - TCH_DATA_IN (если не так, то сортируем)
 так же данные каналы должны быть из одной группы, т.е. их номера должны быть или меньше 6 или больше 5
 канал TCH_CLK_IN должен быть настроен на прерывание
*/
void check_chinese_caliper(device_ex_t* device){
	uint8_t clk_no		= 0xFF;
	uint8_t data_no		= 0xFF;
	channel_t *	channel	= NULL;	
	
	// определяем наличие тех каналов устройства, работа без которых невозможна
	// так же к устройству могут быть добавлены другие вспомогательные каналы
	for (uint8_t i=0; i<device->count_channels; i++){
		channel = (channel_t *)&channels[device->channel_numbers[i]];
		if (channel->type == TCH_CLK_IN)
			clk_no = device->channel_numbers[i];
		
		if (channel->type == TCH_DATA_IN)
			data_no = device->channel_numbers[i];	
		
		if ((clk_no < 0xFF) && (data_no < 0xFF))
			break;
	}
	
	// если не найдены каналы типа: TCH_CLK_IN и TCH_DATA_IN, то ошибка конфигурации каналов
	if ((clk_no == 0xFF) || (data_no == 0xFF)){
		device->error = CRF_INVALID_CHANNEL_TYPE;
		return;
	}
	
		
	// каналы типа:TCH_CLK_IN и TCH_DATA_IN должны быть в одной группе
	// если не так, то ошибка конфигурации каналов
	if (GROUP_NO(clk_no) != GROUP_NO(data_no)){
		device->error = CRF_INVALID_CHANNEL_TYPE;
		return;
	}
	
	
	// TCH_CLK_IN - прерывание разрешено, TCH_DATA_IN - прерывание запрещено
	// если не так, то ошибка конфигурации каналов
	if (!channels[clk_no].enable_interrupt || channels[data_no].enable_interrupt){
		device->error = CRF_INVALID_CHANNEL_TYPE;
		return;
	}
	
	
	// необходимо отсортировать ключевые каналы, без которых работа устройства невозможна
	// для китайских штангенциркулей это каналы TCH_CLK_IN, TCH_DATA_IN - они должны идти первыми и именно в таком порядке
	// чтобы при анализе состояния каналов была возможность быстро найти нужный канал
	
	uint8_t i = 0;
	channel_type_e ch_type = TCH_CLK_IN;
	while (i<device->count_channels){
		if (ch_type == TCH_CLK_IN)
			if (channels[device->channel_numbers[i]].type == ch_type){
				if (i != 0){
					device->channel_numbers[i] ^= (device->channel_numbers[0] ^= device->channel_numbers[i]);
					device->channel_numbers[0] ^= device->channel_numbers[i];
				}
				ch_type = TCH_DATA_IN;
				i = 1;
			}
			
			
		if (ch_type == TCH_DATA_IN)
			if (channels[device->channel_numbers[i]].type == ch_type){
				if (i != 1){
					device->channel_numbers[i] ^= (device->channel_numbers[1] ^= device->channel_numbers[i]);
					device->channel_numbers[1] ^= device->channel_numbers[i];
				}
				
				i = device->count_channels;
			}
				
		
		i++;
			
	}
}

void check_quad(device_ex_t* device){
	uint8_t		a_no	= 0xFF;
	uint8_t		b_no	= 0xFF;
	channel_t	channel;
	uint8_t *	channel_numbers = device->channel_numbers;
	uint8_t		count_channels = device->count_channels;
	
	// определяем наличие тех каналов устройства, работа без которых невозможна
	// так же к устройству могут быть добавлены другие вспомогательные каналы
	for (uint8_t i=0; i<count_channels; i++){
		channel = channels[channel_numbers[i]];
		if (channel.type == TCH_A_IN)
			a_no = channel_numbers[i];
		
		if (channel.type == TCH_B_IN)
			b_no = channel_numbers[i];
		
		
		if ((a_no < 0xFF) && (b_no < 0xFF))
			break;
	}
	
	// если не найдены каналы типа: TCH_A_IN, TCH_B_IN, то ошибка конфигурации каналов
	if ((a_no == 0xFF) || (b_no == 0xFF)){
		device->error = CRF_INVALID_CHANNEL_TYPE;
		return;
	}
	
	// каналы:TCH_A_IN, TCH_B_IN, должны быть в одной группе
	// если не так, то ошибка конфигурации каналов
	if (!(((a_no < 6) && (b_no < 6)) || ((a_no > 5) && (b_no > 5)))){
		//if ((GROUP_NO(a_no) != GROUP_NO(b_no)) && (GROUP_NO(b_no) != GROUP_NO(z_no)) ){
		device->error = CRF_INVALID_CHANNEL_TYPE;
		return;
	}
		
	// необходимо отсортировать ключевые каналы в следующем порядке: TCH_A_IN, TCH_B_IN
	// чтобы при анализе состояния каналов была возможность быстро найти нужный канал
	
	uint8_t i = 0;
	channel_type_e ch_type = TCH_A_IN;
	while (i<count_channels){
		channel = channels[channel_numbers[i]];
		if (ch_type == TCH_A_IN)
		if (channel.type == ch_type){
			if (i != 0){
				channel_numbers[i] ^= (channel_numbers[0] ^= channel_numbers[i]);
				channel_numbers[0] ^= channel_numbers[i];
			}
			ch_type = TCH_B_IN;
			i = 1;
		}
		
		
		if (ch_type == TCH_B_IN)
		if (channel.type == ch_type){
			if (i != 1){
				channel_numbers[i] ^= (channel_numbers[1] ^= channel_numbers[i]);
				channel_numbers[1] ^= channel_numbers[i];
			}
			
			i = count_channels;
		}

		i++;
	}
}

void initProtocol(){
	
	for (uint8_t i=0; i<dro_config.count_devices; i++){
		dro_config.devices[i].is_reset = 1;
		execute_handler(DHT_INIT_DEVICE, &dro_config.devices[i]);
	}

	data_1[0]	= 0;
	pos_1		= 0;
	
	data_2[0]	= 0;
	pos_2		= 0;
	
	is_test_channels = 0;
}

// маска для определения прерывания на текущем устройстве
inline uint8_t get_int_mask(device_ex_t* device){
	uint8_t result = 0;
		
	for (uint8_t i=0; i<device->count_channels; i++)
		if (channels[device->channel_numbers[i]].enable_interrupt)
			result |= (1<<(NO_BIT_CH(device->channel_numbers[i])));

	return result;
}





