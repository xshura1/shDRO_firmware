/*
 * dro_config.c
 *
 * Created: 08.09.2018 9:29:51
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 

#include <stddef.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "hardware.h"
#include "dro_config.h"
#include "dro_types.h"
#include "protocol.h"
#include "transmission_data.h"
#include "timers.h"
#include "keyboard.h"
#include "power.h"
#include "eeprom.h"
#include "cmd.h"
#include "utils.h"
#include "buzzer.h"


void load_devices(void);


uint8_t load_channels_from_eeprom(void);
void save_channels_to_eeprom(void);

void save_devices_to_eeprom(void);
uint8_t load_devices_from_eeprom(void);


int freeRam (void) {
	extern int __heap_start;
	extern int *__brkval;
	return  (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

uint16_t stack_p(void){
	return SP;
}

void clear_mem_manager(void){
	mem_data_s *	mem_data		= (mem_data_s *)&memory_buf[MEM_DATA_POS];
	ch_mem_data_s * ch_data			= (ch_mem_data_s *)&memory_buf[CH_MEM_DATA_POS];
	
	for(uint8_t i=0; i<SIZE_CH_MEM_DATA; i++){
		ch_data[i].ptr = NULL;
		ch_data[i].size = 0;
	}
	
	mem_data->bit_busy_arguments = 0;
	mem_data->size_devices = 0;
}

void alloc_mem(mem_type_e mem_type, uint8_t size, void ** out_ptr){	
	
	mem_data_s *	mem_data		= (mem_data_s *)&memory_buf[MEM_DATA_POS];	
	ch_mem_data_s * ch_data			= (ch_mem_data_s *)&memory_buf[CH_MEM_DATA_POS];
	ch_mem_data_s * curr_ch_data	= NULL;
	
	
	uint8_t	*		l_channels	= (uint8_t *)&memory_buf[MAX_SIZE_DEVICES + MAX_SIZE_ARG];
	uint8_t			m_size		= 0;
	uint8_t			m_size_dev	= 0;
	int8_t			shift_no	= 0;
	uint8_t			begin_ind	= 0;
	uint8_t			end_ind		= 0;
	int8_t			i_step		= 0;
	
		
	switch (mem_type){
		case MEM_DEVICES:
			if (size > MAX_SIZE_DEVICES)
				*out_ptr = NULL;
			else{	
				*out_ptr = (void *)&memory_buf[0];
				mem_data->size_devices = size;
			}
		break;
		
		case MEM_ARGUMENTS:
			if (size == 0) {
				if (*out_ptr != NULL){
					mem_data->bit_busy_arguments &= ~( 1 << ((*out_ptr - (void *)&memory_buf[MAX_SIZE_DEVICES])/SIZE_ARG) );
					*out_ptr = NULL;
				}
				break;
			}
			else			
				if (*out_ptr == NULL)
					for (uint8_t i=0; i<COUNT_DEVICES; i++)
						if (((mem_data->bit_busy_arguments >> i) & 0x1) == 0){
							*out_ptr = (void *)&memory_buf[(i * SIZE_ARG) + MAX_SIZE_DEVICES];
							mem_data->bit_busy_arguments |= (1 << i);
							break;
						}
		break;
		
		case MEM_CHANNELS:
			
			for (uint8_t i=0; i<COUNT_DEVICES; i++)
				if (ch_data[i].ptr != NULL){
					m_size += ch_data[i].size;
					if (ch_data[i].ptr == out_ptr){
						m_size_dev = m_size;
						shift_no = size - ch_data[i].size;
						curr_ch_data = &ch_data[i];
						
					}
				}
				else
					if (curr_ch_data == NULL)
						curr_ch_data = &ch_data[i];
			
			
			if ((m_size + size) > MAX_SIZE_CHANNELS){
				*out_ptr = NULL;
				break;
			}
			
				
			if (shift_no < 0){
				begin_ind	= m_size_dev;
				end_ind		= m_size;	
				i_step		= 1;
			}
			else
				if (shift_no > 0){
					begin_ind	= m_size - 1;
					end_ind		= m_size_dev - 1;
					i_step = -1;	
				}
				
			while (begin_ind != end_ind){
				l_channels[begin_ind + shift_no] = l_channels[begin_ind];
				begin_ind += i_step;
			}
				
			curr_ch_data->size = size;
			if (size == 0){
				curr_ch_data->ptr = NULL;
				for (uint8_t i=0; i<COUNT_DEVICES-1; i++)
					if (ch_data[i].ptr == NULL){
						ch_data[i] = ch_data[i+1];
						ch_data[i+1].ptr = NULL;
						ch_data[i+1].size = 0;
					}
			}
			else
				curr_ch_data->ptr = out_ptr;
				
			m_size = 0;	
			for (uint8_t i=0; i<COUNT_DEVICES; i++)
				if (ch_data[i].ptr != NULL){
					*ch_data[i].ptr = (uint8_t *)&l_channels[m_size];
					m_size += ch_data[i].size;
				}	
		break;
	}	
}

uint8_t dro_load_config(void){
	uint8_t result = 0;
	result = load_channels_from_eeprom();
	if (result)
		result = load_devices_from_eeprom();
	return result;
}

void dro_save_config(void){
	save_channels_to_eeprom();
	save_devices_to_eeprom();
}


void free_devices(void){
	
	if (dro_config.count_devices > 0){
		for(uint8_t i=0; i<dro_config.count_devices; i++){
			free_device((device_ex_t *)&dro_config.devices[i]);
		}
		
		alloc_mem(MEM_DEVICES,	0, (void**)&dro_config.devices);
		dro_config.devices = NULL;
		dro_config.count_devices = 0;
	}
}

void free_device(device_ex_t * device){
	alloc_mem(MEM_CHANNELS,		0, (void**)&device->channel_numbers);
	alloc_mem(MEM_ARGUMENTS,	0, (void**)&device->arguments);
	device->arguments		= NULL;
	device->channel_numbers = NULL;
	device->count_channels	= 0;
}

uint8_t load_devices_from_eeprom(void){
	uint8_t			result = 0;
	device_memory_s dev_mem;
	device_ex_t	*	device;
	uint8_t			crc = 0;
	uint16_t		eeprom_adr = EEPROM_DEVICES_ADDRESS_BEGIN;
	uint8_t			count_devices = 0;
	
	
	free_devices();
	clear_mem_manager();
	
	eeprom_read_array(eeprom_adr, (uint8_t *)&count_devices, sizeof(count_devices), &crc, &eeprom_adr);
	result = (crc = calc_crc((uint8_t *)&count_devices, sizeof(count_devices)));
	eeprom_adr++;
		
	alloc_mem(MEM_DEVICES, sizeof(device_ex_t) * count_devices, (void **)&dro_config.devices);
	result = (dro_config.devices != NULL);
	
	if (result){
		dro_config.count_devices	= count_devices;	
		
		for (uint8_t i=0; i<count_devices; i++){
			result = 0;
			eeprom_read_array(eeprom_adr, (uint8_t *)&dev_mem, sizeof(dev_mem), &crc, &eeprom_adr);
			result = (crc = calc_crc((uint8_t *)&dev_mem, sizeof(dev_mem)));
			eeprom_adr++;
			if (!result) break;
			
			device = (device_ex_t *)&dro_config.devices[i];
			device->protocol	= dev_mem.protocol;
			device->led			= dev_mem.led;
			device->function	= dev_mem.function;
			device->unit		= dev_mem.unit;
			device->state		= CHECK_STATE;
			device->is_reset	= 1;
			device->arguments	= NULL;
			device->channel_numbers = NULL;
			device->count_channels	= 0;
			alloc_mem(MEM_ARGUMENTS, dev_mem.arguments_size, (void **)&device->arguments); 
						
			eeprom_read_array(eeprom_adr, (uint8_t *)device->arguments, dev_mem.stored_arguments_size, &crc, &eeprom_adr);
			result = (crc = calc_crc((uint8_t *)device->arguments, dev_mem.stored_arguments_size));
			eeprom_adr++;
			if (!result) break;
			
			device->count_channels = dev_mem.count_channels;
						
			alloc_mem(MEM_CHANNELS, dev_mem.count_channels, (void**)&device->channel_numbers);
			eeprom_read_array(eeprom_adr, (uint8_t *)device->channel_numbers, dev_mem.count_channels, &crc, &eeprom_adr);
			result = (crc = calc_crc((uint8_t *)device->channel_numbers, dev_mem.count_channels));
			eeprom_adr++;
		}
	}
	return result;
}

void save_devices_to_eeprom(void){
	device_memory_s dev_mem;
	device_ex_t	*	device;
	uint8_t			crc = 0;
	arguments_size_s arg_size;
	uint16_t		eeprom_adr = EEPROM_DEVICES_ADDRESS_BEGIN;
	
	
	crc = calc_crc((uint8_t *)&dro_config.count_devices, sizeof(dro_config.count_devices));
	eeprom_write_array(eeprom_adr, (uint8_t *)&dro_config.count_devices, sizeof(dro_config.count_devices), crc, &eeprom_adr);
	eeprom_adr++;
		
	for (uint8_t i=0; i<dro_config.count_devices; i++){
		device = (device_ex_t *)&dro_config.devices[i];
		arg_size = get_arguments_size(device->protocol, device->function);
		
		dev_mem.protocol				= device->protocol;
		dev_mem.led						= device->led;
		dev_mem.function				= device->function;
		dev_mem.unit					= device->unit;
		dev_mem.stored_arguments_size	= arg_size.stored_arguments_size;
		dev_mem.arguments_size			= arg_size.arguments_size;
		dev_mem.count_channels			= device->count_channels;
		crc = calc_crc((uint8_t *)&dev_mem, sizeof(dev_mem));
		
		// данные устройства		
		eeprom_write_array(eeprom_adr, (uint8_t *)&dev_mem, sizeof(dev_mem), crc, &eeprom_adr);
		eeprom_adr++;
		
		// данные аргументов устройства
		crc = calc_crc((uint8_t *)&device->arguments, dev_mem.stored_arguments_size);
		eeprom_write_array(eeprom_adr, (uint8_t *)&device->arguments, dev_mem.stored_arguments_size, crc, &eeprom_adr);
		eeprom_adr++;
		
		// каналы устройства
		crc = calc_crc(device->channel_numbers, device->count_channels);
		eeprom_write_array(eeprom_adr, device->channel_numbers, device->count_channels, crc, &eeprom_adr);
		eeprom_adr++;
		
	}
}

uint8_t load_channels_from_eeprom(void){
	uint8_t			result = 0;
	uint8_t			crc = 0;
	uint16_t		eeprom_adr = EEPROM_CHANNELS_ADDRESS_BEGIN;
	
	eeprom_read_array(eeprom_adr, (uint8_t *)&channels[0], sizeof(channels), &crc, &eeprom_adr);
	result = (crc = calc_crc((uint8_t *)&channels[0], sizeof(channels)));
	
	return result;
}

void save_channels_to_eeprom(void){
	uint16_t	eeprom_adr	= EEPROM_CHANNELS_ADDRESS_BEGIN;
	uint8_t		crc			= 0;
	
	crc = calc_crc((uint8_t *)&channels[0], sizeof(channels));
	eeprom_write_array(eeprom_adr, (uint8_t *)&channels[0], sizeof(channels), crc, &eeprom_adr);
}

// проверка конфигурации устройств
void check_devices(void){
	
	for (uint8_t i=0; i<dro_config.count_devices; i++)
		if (dro_config.devices[i].state == CHECK_STATE)
			check_config_device_ex(&dro_config.devices[i]);
			
}

void disable_channels_interrupts(void){
	dro_config.stored_channel_interrupts.CI_MSK1 = PCMSK1;
	dro_config.stored_channel_interrupts.CI_MSK2 = PCMSK2;
	dro_config.stored_channel_interrupts.CI_ICR = PCICR;
	
	PCMSK1	= 0;
	PCMSK2	= 0;
	PCICR	= 0;
}

void enable_channels_interrupts(void){
	PCMSK1	= dro_config.stored_channel_interrupts.CI_MSK1;
	PCMSK2	= dro_config.stored_channel_interrupts.CI_MSK2;
	PCICR	= dro_config.stored_channel_interrupts.CI_ICR;
}

void set_channel_interrupt(uint8_t ch_no){
	switch (INTERRUPT_CH_NO(ch_no)){
		case INTERRUPT_1:
			PCMSK1	|= (1<<(NO_BIT_CH(ch_no)));
			PCICR	|= (1<<PCIE1);
		break;
						
		case INTERRUPT_2:
			PCMSK2	|= (1<<(NO_BIT_CH(ch_no)));
			PCICR	|= (1<<PCIE2);
		break;
		
		default:
			disable_channels_interrupts();
		break;
	}
}

void init_channels(void){
	disable_channels_interrupts();
	device_ex_t*	device;
	uint8_t			ch_no = 0;
		
	for (uint8_t i=0; i<dro_config.count_devices; i++){
		device = dro_config.devices + i;
			for (uint8_t j=0; j<device->count_channels; j++)
				if (!device->error){
					ch_no = device->channel_numbers[j];
					if (channels[ch_no].enable_interrupt)
						set_channel_interrupt(ch_no);
				
					switch (channels[device->channel_numbers[j]].type){
						case TCH_CLK_OUT:
						case TCH_SET_ZERO_OUT:
						case TCH_CHANGE_UNIT_OUT:
						case TCH_SET_FAST_MODE_OUT:
							*DDR_CH(ch_no) |= (1 << (NO_BIT_CH(ch_no)));
						break;
						
						case TCH_CLK_IN:
						case TCH_DATA_IN:
						case TCH_A_IN:
						case TCH_B_IN:
						case TCH_Z_IN:
						case TCH_UNDEFINED:
							*DDR_CH(ch_no) &= ~( 1 << (NO_BIT_CH(ch_no)));
						break;	
					}	
				}
				else{
					// если у устройства есть ошибки, то все его каналы переключаем в режим PullUp
					*DDR_CH(ch_no) &= ~( 1 << (NO_BIT_CH(ch_no)));
					*PORT_CH(ch_no) |= (1 << (NO_BIT_CH(ch_no)));
					
				}
	}
		
	// все неиспользуемые каналы, или каналы, настроенные на прерывания, переводим в режим PullUp
	for (uint8_t i=0; i<COUNT_CHANNELS; i++)
		if ((channels[i].type == TCH_UNDEFINED) || (channels[i].enable_interrupt)){
			*DDR_CH(i) &= ~( 1 << (NO_BIT_CH(i)));
			*PORT_CH(i) |= (1 << (NO_BIT_CH(i)));
		}
}

void init_dro(void){
	dro_config.state				= DRO_INFO; //  DRO_BEGIN_REPAIR_KEYBOARD;
	dro_config.count_devices		= 0;
	dro_config.devices				= NULL;
	dro_config.transmission_sources = LED | UART;
	dro_config.transmission_type	= TRANSMISSION_DATA;
	dro_config.is_shutdown			= 0;
	dro_config.is_test_keyboard		= 0;
	dro_config.is_cmd				= 0;
	dro_config.is_test_channels		= 0;
	dro_config.is_logic_analyzer	= 0;
	
	for (uint8_t i=0; i<SIZE_ALL; i++)
		memory_buf[i] = 0;
	
	// инициализация памяти
	init_shared_memory();
	
	// инициализация приёмников
	init_transmission(dro_config.transmission_sources);
	
	// инициализация динамика
	init_buzzer(); 
		
	// загрузка конфигурации устройств
	dro_load_config();
		
	// проверка конфигурации устройств
	check_devices();
	
	// инициализация каналов: выставление прерываний, настройка на вход/выход
	init_channels();
	
	// инициализация клавиатуры
	// если ошибка, то инициируем обучение клавиатуры
	if (!init_keyboard())
		dro_config.state = DRO_BEGIN_REPAIR_KEYBOARD;
	
	// инициализация таймера
	init_timer0(1000);
		
	// инициализация протоколов
	initProtocol();
}

// функция возвращает кол-во занятых каналов
uint8_t get_count_busy_channels(){
	uint8_t result = 0;
	device_ex_t * device;
	
	for (uint8_t i=0; i<dro_config.count_devices; i++){
		device = dro_config.devices + i;
		result += device->count_channels;
	}
	return result;
}

int8_t channel_is_attach_device(uint8_t channel_no){
	int8_t result = -1;
	
	for (uint8_t i=0; i<(dro_config.count_devices); i++){
		for (uint8_t j=0; j<dro_config.devices[i].count_channels; j++)
			if (dro_config.devices[i].channel_numbers[j] == channel_no){
				result = i;
				goto BREAK_LOOP;
			}
	}
	
	BREAK_LOOP:
	
	return result;
}


void shutdown_dro(void){
	shutdown_transmission();
	cli();
	
}
