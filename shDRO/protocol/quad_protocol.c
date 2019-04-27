/*
 * quad_protocol.c
 *
 * Created: 27.03.2019 21:42:49
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
#include "..\timers.h"

void quad_init(device_ex_t* device){
	optical_arguments_s * arg = (optical_arguments_s *)(device->arguments);
	
	device->state		= READY_STATE;
	device->curr_value.dp_pos = 3;	
	arg->cnt_pulse		= 0;
	arg->old_state		= 0;
	device->pos_buffer	= 0;
	arg->cnt_z			= 0;
};

void quad_get(device_ex_t* device){
	optical_arguments_s * arg = (optical_arguments_s *)(device->arguments);
	uint8_t no_bit = 0;
				
	device->state = PROCESSING_DATA_STATE;
				
	no_bit = NO_BIT_CH(device->channel_numbers[0]);
	uint8_t new_state = (data_buf[device->pos_buffer] >> no_bit) & 0x03;
			
	no_bit = NO_BIT_CH(device->channel_numbers[1]);
	new_state |= (data_buf[device->pos_buffer] >> (no_bit - 1)) & 0x03;
			
	uint8_t full_state = new_state | (arg->old_state << 2);
			
	switch(full_state){
		case 0x2: case 0x4: case 0xB: case 0xD:
		arg->cnt_pulse++;
		device->state = TRANSMISSION_DATA_STATE;
		break;
				
		case 0x1: case 0x7: case 0x8: case 0xE:
		arg->cnt_pulse--;
		device->state = TRANSMISSION_DATA_STATE;
		break;
	}
			
	arg->old_state = new_state;
}

void quad_transmit(device_ex_t* device){
	optical_arguments_s * arg = (optical_arguments_s *)(device->arguments);
	
	device->curr_value.value = (int32_t)(arg->cnt_pulse * 5);
	transmit_value(&device->curr_value, device->led, LED);
}