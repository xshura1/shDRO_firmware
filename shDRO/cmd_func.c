/*
 * cmd_func.c
 *
 * Created: 02.03.2019 10:23:41
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 

#include <avr/pgmspace.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "hardware.h"
#include "protocol.h"
#include "cmd.h"
#include "dro_config.h"
#include "keyboard.h"
#include "transmission_data.h"
#include "led.h"
#include "LedControl.h"
#include "buzzer.h"

cmd_argument_s * cmd_get_argument(token_arg_e arg, cmd_argument_s * args, uint8_t args_num);
void cmd_get_str_arg_P(token_arg_e arg, char ** out_str);
void cmd_get_str_val_P(token_arg_e arg, uint8_t val, char ** out_str);
void cmd_send_arg_P(token_arg_e arg, int32_t val, uint8_t val_is_string, uint8_t is_clrf);

cmd_result_func_e cmd_edit_device_ex(int8_t dev_no, cmd_argument_s * args, uint8_t args_num);

cmd_argument_s * cmd_get_argument(token_arg_e arg, cmd_argument_s * args, uint8_t args_num){
	for (uint8_t i=0; i<args_num; i++)
		if (args[i].arg == arg)
			return &args[i];
	
	return NULL;
}

void cmd_get_str_arg_P(token_arg_e arg, char ** out_str){
	for (uint8_t i=0; i<TOKENS_ARG_SIZE; i++)
		if (arg == pgm_read_byte(&(args_map[i].arg_e))){
			strcpy_P(*out_str, (char*)pgm_read_word(&(args_map[i].str)));
			break;
		}
}

void cmd_send_arg_P(token_arg_e arg, int32_t val, uint8_t val_is_string, uint8_t is_clrf){
	char arg_buf[10];
	char val_buf[10];
	char * parg = (char*)&arg_buf;
	char * pval = (char*)&val_buf;
	FILE * file = uart_s();
	
	cmd_get_str_arg_P(arg, &parg);
	if (val_is_string){
		cmd_get_str_val_P(arg, val, &pval);
		fprintf_P(file, CMD_MSG_ARG_INFO_S, parg, pval);
	}
	else
		fprintf_P(file, CMD_MSG_ARG_INFO_D, parg, val);
	
	if (is_clrf)	
		fprintf_P(file, MSG_CLRF);
	
}

void cmd_get_str_val_P(token_arg_e arg, uint8_t val, char ** out_str){
	for (uint8_t i=0; i<TOKENS_VAL_SIZE; i++)
		if (arg == pgm_read_byte(&(vals_map[i].arg_e)))
			if (val == pgm_read_byte(&(vals_map[i].val))){
				strcpy_P(*out_str, (char*)pgm_read_word(&(vals_map[i].str)));
				break;
			}
			
	
}

cmd_result_func_e cmd_setup(cmd_argument_s * args, uint8_t args_num){
	if (dro_config.is_cmd) return 0;
	dro_config.is_cmd = 1;
	fprintf_P(led_s(LED_MAIN), TOKEN_COMMAND_SETUP);
	return CRF_OK;
}

cmd_result_func_e cmd_exit(cmd_argument_s * args, uint8_t args_num){
	dro_config.is_cmd = 0;
	return CRF_OK;
}

cmd_result_func_e cmd_help(cmd_argument_s * args, uint8_t args_num){
	FILE * file = uart_s();
	
	for(uint8_t i=1; i<TOKENS_COMMAND_SIZE; i++){
		fprintf_P(file, (char*)pgm_read_word(&(tokens_command[i])));
		fprintf_P(file, MSG_CLRF);
	}
	
	return CRF_OK;
}



cmd_result_func_e cmd_about(cmd_argument_s * args, uint8_t args_num){
	fprintf_P(uart_s(), MSG_PRODUCT_NAME);
	fprintf_P(uart_s(), MSG_CLRF);
	fprintf_P(uart_s(), MSG_VERSION_NO);
	fprintf_P(uart_s(), MSG_CLRF);
	fprintf_P(uart_s(), MSG_VERSION_DATA);
	fprintf_P(uart_s(), MSG_CLRF);
	return CRF_OK;
}



cmd_result_func_e cmd_devices_info(cmd_argument_s * args, uint8_t args_num){
	FILE * file = uart_s();
	device_ex_t * device = NULL;

	for (uint8_t i=0; i<dro_config.count_devices; i++){
		device = &dro_config.devices[i];
		fprintf_P(file, CMD_MSG_DEVICE_INFO, i);
		cmd_send_arg_P(T_ARG_PROTOCOL, device->protocol, 1, 0);
		cmd_send_arg_P(T_ARG_FUNCTION, device->function, 1, 0);
		cmd_send_arg_P(T_ARG_LED, device->led, 0, 0);
		cmd_send_arg_P(T_ARG_UNIT, device->unit, 1, 1);
			
	}
	return CRF_OK;
}

cmd_result_func_e cmd_keyboard_info(cmd_argument_s * args, uint8_t args_num){
	FILE * file = uart_s();
	for (uint8_t i=0; i<KEY_COUNT; i++){
		fprintf_P(file, CMD_MSG_KEYBOARD_INFO,
					keyboard.adc_mappings[i].key_code,
					keyboard.adc_mappings[i].channel+ADC_CH_BEGIN,
					keyboard.adc_mappings[i].adc_value);
		fprintf_P(file, MSG_CLRF);
	}
	
	return CRF_OK;
}

cmd_result_func_e cmd_channels_info(cmd_argument_s * args, uint8_t args_num){
	FILE * file = uart_s();
	int8_t dev_no = -1;
	
	for (uint8_t i=0; i<COUNT_CHANNELS; i++){
		if (i<6)
			fprintf_P(file, CMD_MSG_CHANNEL_PD, i+2);
		else
			fprintf_P(file, CMD_MSG_CHANNEL_PC, i-6);	
			
		fprintf_P(file, CMD_MSG_CHANNEL_INFO, i);
		cmd_send_arg_P(T_ARG_TYPE, channels[i].type, 1, 0);
		cmd_send_arg_P(T_ARG_ENABLE_INTERRUPT, channels[i].enable_interrupt, 0, 0);
		cmd_send_arg_P(T_ARG_INTERRUPT_LEVEL, channels[i].int_level, 1, 0);
		dev_no = channel_is_attach_device(i);
		cmd_send_arg_P(T_ARG_INVERT, channels[i].is_invert, 0, (dev_no < 0));
		
		if (dev_no >= 0)
			cmd_send_arg_P(T_ARG_DEVICE, dev_no, 0, 1);
		
	}
	return CRF_OK;
}

cmd_result_func_e cmd_edit_channel(cmd_argument_s * args, uint8_t args_num){
	cmd_argument_s * arg_ch = cmd_get_argument(T_ARG_CH, args, args_num);
	if (arg_ch == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	if (arg_ch->val_type != CTV_VALUE_T){
		arg_ch->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	
	if ((arg_ch->val_t.value > 11) || (arg_ch->val_t.value < 0)){
		arg_ch->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	channel_t * channel = &channels[arg_ch->val_t.value];
	
	arg_ch = cmd_get_argument(T_ARG_TYPE, args, args_num);
	if (arg_ch != NULL)
		channel->type = arg_ch->val_enum;
	
	arg_ch = cmd_get_argument(T_ARG_ENABLE_INTERRUPT, args, args_num);
	if (arg_ch != NULL)
		channel->enable_interrupt = arg_ch->val_t.value;	
		
	arg_ch = cmd_get_argument(T_ARG_INTERRUPT_LEVEL, args, args_num);
	if (arg_ch != NULL)
		channel->int_level = arg_ch->val_enum;
		
	arg_ch = cmd_get_argument(T_ARG_INVERT, args, args_num);
	if (arg_ch != NULL)
		channel->is_invert = arg_ch->val_t.value;	
	
	return CRF_OK;		

}

cmd_result_func_e cmd_leds_info(cmd_argument_s * args, uint8_t args_num){
	return CRF_OK;
}


cmd_result_func_e cmd_edit_device_ex(int8_t dev_no, cmd_argument_s * args, uint8_t args_num){
	cmd_argument_s *	arg_dev = NULL;
	device_ex_t *		device = NULL;
	
	if (dev_no < 0){
		arg_dev = cmd_get_argument(T_ARG_DEVICE, args, args_num);
		if (arg_dev == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
		if (arg_dev->val_type != CTV_VALUE_T){
			arg_dev->val_type = CTV_ERROR_VAL;
			return CRF_INVALID_VALUE;
		}
		if ((arg_dev->val_t.value > (dro_config.count_devices - 1)) || (arg_dev->val_t.value < 0)){
			arg_dev->val_type = CTV_ERROR_VAL;
			return CRF_INVALID_VALUE;
		}
		device = &dro_config.devices[arg_dev->val_t.value];	
	}
	else
		device = &dro_config.devices[dev_no];
	
	arg_dev = cmd_get_argument(T_ARG_FUNCTION, args, args_num);
	if (arg_dev != NULL)
		device->function = arg_dev->val_enum;
		
	
	arg_dev = cmd_get_argument(T_ARG_LED, args, args_num);
	if (arg_dev != NULL)
		device->led = arg_dev->val_t.value;
		
	
	arg_dev = cmd_get_argument(T_ARG_UNIT, args, args_num);
	if (arg_dev != NULL)
		device->unit = arg_dev->val_enum;

		
	arg_dev = cmd_get_argument(T_ARG_PROTOCOL, args, args_num);
	if (arg_dev != NULL){
		if (device->protocol != arg_dev->val_enum){
			device->protocol = arg_dev->val_enum;
			if (device->arguments != NULL){
				alloc_mem(MEM_ARGUMENTS, 0, (void **)&device->arguments);
				device->arguments = NULL;
			}
		
			arguments_size_s arg_size = get_arguments_size(device->protocol, device->function);
			alloc_mem(MEM_ARGUMENTS, arg_size.arguments_size, (void **)&device->arguments);
			if (device->arguments == NULL)
				return CRF_OUT_OF_MEMORY;
			
		}
	}
		
	return CRF_OK;
	
}

cmd_result_func_e cmd_add_device(cmd_argument_s * args, uint8_t args_num){
	void * ptr = NULL;
	alloc_mem(MEM_DEVICES, sizeof(device_ex_t) * (dro_config.count_devices + 1), &ptr);
	// распределение памяти для устройства
	if (ptr == NULL) return CRF_OUT_OF_MEMORY;
	
	dro_config.devices = (device_ex_t*)ptr;
	dro_config.count_devices++;
	dro_config.devices[dro_config.count_devices-1].arguments		= NULL;
	dro_config.devices[dro_config.count_devices-1].channel_numbers	= NULL;
	dro_config.devices[dro_config.count_devices-1].count_channels	= 0;
			
	return cmd_edit_device_ex(dro_config.count_devices-1, args, args_num);
}

cmd_result_func_e cmd_edit_device(cmd_argument_s * args, uint8_t args_num){
	return cmd_edit_device_ex(-1, args, args_num);
}

cmd_result_func_e cmd_del_device(cmd_argument_s * args, uint8_t args_num){
	cmd_argument_s *	arg_dev = cmd_get_argument(T_ARG_DEVICE, args, args_num);
	int8_t				dev_no	= -1;
	device_ex_t			tmp_device;
	
	if (arg_dev == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	if (arg_dev->val_type != CTV_VALUE_T){
		arg_dev->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	if ((arg_dev->val_t.value > (dro_config.count_devices - 1)) || (arg_dev->val_t.value < 0)){
		arg_dev->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	
	dev_no = arg_dev->val_t.value;
	// освобождаем память каналов, аргументов и самого устройства
	free_device(&dro_config.devices[dev_no]);
	
	// удаляемое устройство должно быть в конце массива
	// если это не так, то данные устройства в конце массива
	// записываем на место удаляемого устройства
	
	if (dev_no != (dro_config.count_devices - 1)){
		tmp_device = dro_config.devices[dev_no];
		dro_config.devices[dev_no] = dro_config.devices[dro_config.count_devices - 1];
		dro_config.devices[dro_config.count_devices - 1] = tmp_device;
	}
	
																					
	dro_config.count_devices--;
	alloc_mem(MEM_DEVICES, sizeof(device_ex_t) * dro_config.count_devices, (void**)&dro_config.devices);
		
	if (dro_config.count_devices == 0)
		dro_config.devices = NULL;
		
	return CRF_OK;
}

cmd_result_func_e cmd_attach_channel(cmd_argument_s * args, uint8_t args_num){
	cmd_argument_s*	arg_dev	= NULL;
	cmd_argument_s*	arg_ch	= NULL;
	device_ex_t*	device	= NULL;
		
	arg_dev	= cmd_get_argument(T_ARG_DEVICE, args, args_num);
	if (arg_dev == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	if (arg_dev->val_type != CTV_VALUE_T){
		arg_dev->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	if ((arg_dev->val_t.value > (dro_config.count_devices - 1)) || (arg_dev->val_t.value < 0)){
		arg_dev->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	
	device = &dro_config.devices[arg_dev->val_t.value];
	
	arg_ch	= cmd_get_argument(T_ARG_CH, args, args_num);
	if (arg_ch == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	if (arg_ch->val_type != CTV_VALUE_T){
		arg_ch->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	if ((arg_ch->val_t.value > 11) || (arg_ch->val_t.value < 0)){
		arg_ch->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	if (channel_is_attach_device(arg_ch->val_t.value) > -1) return CRF_CHANNEL_IS_BUSY;
		
	alloc_mem(MEM_CHANNELS, device->count_channels + 1, (void**)&device->channel_numbers);
	if (device->channel_numbers == NULL) return CRF_OUT_OF_MEMORY;
	device->count_channels++;
	device->channel_numbers[device->count_channels - 1] = arg_ch->val_t.value;

	return CRF_OK;
}

cmd_result_func_e cmd_detach_channel(cmd_argument_s * args, uint8_t args_num){

	cmd_argument_s*	arg_dev	= NULL;
	cmd_argument_s*	arg_ch	= NULL;
	device_ex_t*	device	= NULL;
	uint8_t			tmp_ch	= 0;
	
	arg_dev	= cmd_get_argument(T_ARG_DEVICE, args, args_num);
	if (arg_dev == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	if (arg_dev->val_type != CTV_VALUE_T){
		arg_dev->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	if ((arg_dev->val_t.value > (dro_config.count_devices - 1)) || (arg_dev->val_t.value < 0)){
		arg_dev->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	
	device = &dro_config.devices[arg_dev->val_t.value];
	
	arg_ch	= cmd_get_argument(T_ARG_CH, args, args_num);
	if (arg_ch == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	if (arg_ch->val_type != CTV_VALUE_T){
		arg_ch->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	if ((arg_ch->val_t.value > 11) || (arg_ch->val_t.value < 0)){
		arg_ch->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	
	for (uint8_t i=0; i<device->count_channels; i++)
		if (device->channel_numbers[i] == arg_ch->val_t.value){
			tmp_ch = device->channel_numbers[device->count_channels - 1];
			device->channel_numbers[device->count_channels - 1] = device->channel_numbers[i];
			device->channel_numbers[i] = tmp_ch;
			break;
		}
	
	alloc_mem(MEM_CHANNELS, device->count_channels - 1, (void**)&device->channel_numbers);
	device->count_channels--;
	if (device->count_channels == 0)
		device->channel_numbers = NULL;	
	
	
	return CRF_OK;
}



cmd_result_func_e cmd_test_channels(cmd_argument_s * args, uint8_t args_num){
	
	cmd_argument_s *	arg_ch				= cmd_get_argument(T_ARG_CH, args, args_num);
	uint8_t				samples				= 255;
	int32_t				time_us				= 0;
	uint8_t				test_interrupted	= 0;
	volatile uint8_t *	pos_n				= NULL;
	
	if (arg_ch == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	if (arg_ch->val_type != CTV_BIT){
		arg_ch->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	// TODO по умолчанию сделать 8us
	cmd_argument_s * arg_per = cmd_get_argument(T_ARG_PERIOD, args, args_num);
	if (arg_per == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	if (get_timer_mode(arg_per->val_t.value) == 0) return CRF_INVALID_VALUE;
	time_us = arg_per->val_t.value;
	
	
	cmd_argument_s * arg_smp = cmd_get_argument(T_ARG_SAMPLES, args, args_num);
	if ((arg_smp != NULL) && (arg_smp->val_type == CTV_VALUE_T)){
		if ((arg_smp->val_t.value > 255) && (arg_smp->val_t.value < 1)){
			arg_smp->val_type = CTV_ERROR_VAL;
			return CRF_INVALID_VALUE;
		}
		else
			samples = arg_smp->val_t.value;	
	}
	
	// TODO добавить проверку на группу каналов
	uint16_t val_bit = arg_ch->val_bit;
	
	cli();
	disable_channels_interrupts();
	
	data_1		= &memory_buf[0];
	data_2		= &memory_buf[0];
	data_buf	= &memory_buf[0];
	pos_1		= 0;
	pos_2		= 0;
		
	for (uint8_t i=0; i<COUNT_CHANNELS; i++)
		if (((val_bit >> i) & 0x01))
			if (channels[i].enable_interrupt){
				set_channel_interrupt(i);
				// TODO выдавать ошибку, если больше одного канала настроено на прерывание
				if (GROUP_NO(i) == 1)
					pos_n = &pos_1;
				else
					pos_n = &pos_2;	
			}
	
	for(uint16_t i=0; i<(DATA_BUF_SIZE / 2); i++)
		time_buf[i] = 0;
	
	for(uint16_t i=0; i<MEMORY_BUF_SIZE; i++)
		data_buf[i] = 0;
		
	init_timer0(0);					// отключаем головной таймер, чтобы не мешал
	dro_config.is_logic_analyzer = 1;
	dro_config.is_test_channels = 1;
	is_test_channels = 1;
	init_timer1_for_logic_analizer(time_us);
	
	sei();
	fprintf_P(uart_s(), MSG_WAIT);
	fprintf_P(uart_s(), MSG_DOTS);
	fprintf_P(uart_s(), CMD_MSG_PRESS_ANY_KEY);
	fprintf_P(uart_s(), MSG_CLRF);
	
	while (*pos_n < samples){
		if (keyboard.is_pressed_key){
			test_interrupted = 1;
			break;
		}
	}
	
	cli();
	disable_channels_interrupts();
	is_test_channels = 0;
	init_timer0(1000);					// включаем головной таймер // TODO нужно ли отключать его?
	sei();
	
	if (test_interrupted){
		fprintf_P(uart_s(), CMD_MSG_TEST_CHANNELS_INT);
		goto END_TEST_CHANNELS;
	}
		
	uint8_t bit			= 0;
	uint16_t time_Nus	= 0;
	uint8_t i = 0;
	while (i<COUNT_CHANNELS){
		if (val_bit & 0x01){
			fprintf_P(uart_s(), CMD_MSG_TEST_CHANNELS, (uint32_t)1000000/time_us, i);
			for(uint16_t j=0; j<MEMORY_BUF_SIZE; j++){
				time_Nus = (uint16_t)time_buf[j];
				if (time_Nus == 0)
					time_Nus = 1;				// если время было меньше разрешения таймера, то принимаем его за 1 (разрешение таймера)
				bit = (data_buf[j] >> NO_BIT_CH(i)) & 0x01;
				do{
					fprintf(uart_s(), "%d", bit);
					time_Nus--;
				}while (time_Nus > 0);
			}
			fprintf_P(uart_s(), MSG_CLRF);
		}
		val_bit = (val_bit >> 1);
		i++;
	}
	
	END_TEST_CHANNELS:
		fprintf_P(uart_s(), MSG_CLRF);
		dro_config.is_logic_analyzer = 0;
		dro_load_config();
		
		FIFO_FLUSH(FIFO_TX);
		FIFO_FLUSH(FIFO_RX);
	
	return CRF_EMPTY;
}

cmd_result_func_e cmd_buzzer(cmd_argument_s * args, uint8_t args_num){
	if (args_num == 0){
		stop_buzzer();
		return CRF_OK;
	}
		
	cmd_argument_s * arg_note = cmd_get_argument(T_ARG_NOTE, args, args_num);
	if (arg_note == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	if (arg_note->val_type != CTV_VALUE_T){
		arg_note->val_type = CTV_ERROR_VAL;
		return CRF_INVALID_VALUE;
	}
	
	
	uint8_t d_on	= 255;
	uint8_t d_off	= 0;
	uint8_t cnt		= 255;
		
	cmd_argument_s * arg_d_on = cmd_get_argument(T_ARG_DURATION_ON, args, args_num);
	if (arg_d_on != NULL){
		if (arg_d_on->val_type != CTV_VALUE_T){
			arg_d_on->val_type = CTV_ERROR_VAL;
			return CRF_INVALID_VALUE;
		}
		d_on = arg_d_on->val_t.value;
	}
	
	cmd_argument_s * arg_d_off = cmd_get_argument(T_ARG_DURATION_OFF, args, args_num);
	if (arg_d_off != NULL){
		if (arg_d_off->val_type != CTV_VALUE_T){
			arg_d_off->val_type = CTV_ERROR_VAL;
			return CRF_INVALID_VALUE;
		}
		d_off = arg_d_off->val_t.value;
	}
	
	cmd_argument_s * arg_cnt = cmd_get_argument(T_ARG_CNT, args, args_num);
	if (arg_cnt != NULL){
		if (arg_cnt->val_type != CTV_VALUE_T){
			arg_cnt->val_type = CTV_ERROR_VAL;
			return CRF_INVALID_VALUE;
		}
		cnt = arg_cnt->val_t.value;
	}
		
	start_buzzer(arg_note->val_t.value, d_on, d_off, cnt);
		
	return CRF_OK;
}

cmd_result_func_e cmd_led_blink(cmd_argument_s * args, uint8_t args_num){
	cmd_argument_s * arg_led = cmd_get_argument(T_ARG_LED, args, args_num);
	if (arg_led == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	select_led(arg_led->val_t.value);
	return CRF_OK;
	
}

cmd_result_func_e cmd_led_clr(cmd_argument_s * args, uint8_t args_num){
	cmd_argument_s * arg_led = cmd_get_argument(T_ARG_LED, args, args_num);
	if (arg_led == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	clearDisplay((device_led_e)arg_led->val_t.value);
	return CRF_OK;
}
	
cmd_result_func_e cmd_led_msg(cmd_argument_s * args, uint8_t args_num){
	cmd_argument_s * arg_led = cmd_get_argument(T_ARG_LED, args, args_num);
	if (arg_led == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	cmd_argument_s * arg_msg = cmd_get_argument(T_ARG_MSG, args, args_num);
	if (arg_msg == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	fprintf(led_s((device_led_e)arg_led->val_t.value), "%.16s", arg_msg->val_str);
	return CRF_OK;
}

cmd_result_func_e cmd_led_lumi(cmd_argument_s * args, uint8_t args_num){
	cmd_argument_s * arg_led = cmd_get_argument(T_ARG_LED, args, args_num);
	if (arg_led == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	cmd_argument_s * arg_lumi = cmd_get_argument(T_ARG_LUMI, args, args_num);
	if (arg_lumi == NULL) return CRF_REQUIRED_ARG_NOT_FOUND;
	setIntensity(arg_led->val_t.value, arg_lumi->val_t.value);
	return CRF_OK;
}

cmd_result_func_e cmd_load(cmd_argument_s * args, uint8_t args_num){
	if (dro_load_config())
		return CRF_OK;
	else
		return CRF_OUT_OF_MEMORY;	
}

cmd_result_func_e cmd_save(cmd_argument_s * args, uint8_t args_num){
	dro_save_config();
	return CRF_OK;
}
