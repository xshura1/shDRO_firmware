/*
 * dro_config.h
 *
 * Created: 03.08.2018 15:35:26
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef DRO_CONFIG_H_
#define DRO_CONFIG_H_

#include <avr/io.h>
#include <stdlib.h>

#include "dro_types.h"

#define SIZE_DRO_CONFIG			sizeof(dro_config) + 1 // TODO нужно пересчитать размер dro_config
#define SIZE_MEMORY_CHANNELS	sizeof(channels) + 1

#define COUNT_CHANNELS			12
#define COUNT_DEVICES			6

#define SIZE_DEVICE				sizeof(device_ex_t)
#define SIZE_ARG				sizeof(capacity_arguments_s)
#define SIZE_CH_MEM_DATA		sizeof(ch_mem_data_s) * COUNT_DEVICES
#define SIZE_MEM_DATA			sizeof(mem_data_s)

#define MAX_SIZE_DEVICES		SIZE_DEVICE * COUNT_DEVICES
#define MAX_SIZE_ARG			SIZE_ARG * COUNT_DEVICES
#define MAX_SIZE_CHANNELS		COUNT_CHANNELS
#define CH_MEM_DATA_POS			MAX_SIZE_DEVICES + MAX_SIZE_ARG + MAX_SIZE_CHANNELS
#define MEM_DATA_POS			CH_MEM_DATA_POS + SIZE_CH_MEM_DATA
#define SIZE_ALL				MAX_SIZE_DEVICES + MAX_SIZE_ARG + MAX_SIZE_CHANNELS + SIZE_CH_MEM_DATA + SIZE_MEM_DATA

typedef struct {
	uint8_t	CI_MSK1;
	uint8_t	CI_MSK2;
	uint8_t	CI_ICR;
} channel_interrrupts_s;

typedef enum {
	DRO_INFO,
	DRO_REQUEST_REPAIR_KEYBOARD,
	DRO_NORMAL,
	DRO_BEGIN_REPAIR_KEYBOARD,
	DRO_REPAIR_KEYBOARD,
	DRO_END_REPAIR_KEYBOARD,
	DRO_BEGIN_CMD,
	DRO_CMD,
	DRO_SHUTDOWN
		
} dro_state_e;


// конфигурация dro
typedef struct {
	channel_interrrupts_s	stored_channel_interrupts; // сохраненные регистры прерываний портов
	device_ex_t * 			devices;
	uint8_t 				count_devices;
	dro_state_e				state;
	dro_state_e				old_state;
		
	transmission_source_e	transmission_sources;
	transmission_type_e		transmission_type;
	
	unsigned int			is_check_power		: 1;
	unsigned int			is_shutdown			: 1;
	unsigned int			is_test_keyboard	: 1;
	unsigned int			is_cmd				: 1;
	unsigned int			is_test_channels	: 1;
	unsigned int			is_logic_analyzer	: 1;
} dro_config_t;

typedef enum {
	MEM_DEVICES,
	MEM_ARGUMENTS,
	MEM_CHANNELS
} mem_type_e;

typedef struct {
	void **	ptr;
	uint8_t	size;
} ch_mem_data_s;

typedef struct {
	uint8_t	size_devices;
	uint8_t	bit_busy_arguments;
} mem_data_s;

void clear_mem_manager(void);
void alloc_mem(mem_type_e mem_type, uint8_t size, void ** out_ptr);
void free_devices(void);
void free_device(device_ex_t * device);

int freeRam (void);
uint16_t stack_p(void);

channel_t		channels[COUNT_CHANNELS];
volatile dro_config_t  dro_config;

void init_dro(void);

uint8_t dro_load_config(void);
void dro_save_config(void);

int8_t channel_is_attach_device(uint8_t channel_no);

void disable_channels_interrupts(void);
void enable_channels_interrupts(void);

void set_channel_interrupt(uint8_t ch_no);

void shutdown_dro(void);


#endif /* DRO_CONFIG_H_ */
