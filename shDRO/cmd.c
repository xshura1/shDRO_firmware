/*
 * cmd.c
 *
 * Created: 11.02.2019 14:05:03
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 

#include <ctype.h>

#include "cmd.h"
#include "cmd_func.h"
#include "shared_memory.h"
#include "transmission_data.h"
#include "dro_config.h"
#include "utils.h"


#define TOKEN(str, token) ( (strcmp_P(str, token) == 0) && (dro_config.is_cmd) )

const void * const sys_func[] PROGMEM  = {
	cmd_help,
	cmd_setup,
	cmd_exit,
	cmd_add_device,
	//cmd_arduino_info,
	cmd_attach_channel,
	cmd_buzzer,
	cmd_channels_info,
	cmd_edit_channel,
	cmd_del_device,
	cmd_detach_channel,
	cmd_devices_info,
	cmd_about,
	cmd_keyboard_info,
	cmd_led_blink,
	cmd_led_clr,
	cmd_led_msg,
	cmd_leds_info,
	cmd_load,
	cmd_save,
	cmd_edit_device,
	cmd_test_channels,
	cmd_led_lumi
	
};

void cmd_send_result(cmd_result_func_e result, char * command, cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_parse_str(char * str, uint8_t is_parse_arg, char ** command, int8_t * index_command, cmd_argument_s * args, uint8_t * args_num);
void cmd_parse_arg(cmd_argument_s * arg);
void cmd_parse_val(cmd_argument_s * arg);
cmd_result_func_e cmd_exec_func(int8_t func_no, cmd_argument_s * args, uint8_t args_num);


void cmd_check(void){
#ifndef DEBUG	
	
	FILE *				file			= uart_s();	
	char *				command			= NULL;
	int8_t				command_index	= -1;
	cmd_argument_s *	args			= (cmd_argument_s *)uart_buf;
	uint8_t				args_num		= 0;
	cmd_result_func_e	result			= CRF_OK;
	

	if (FIFO_IS_EMPTY(FIFO_RX))	return;
		
	command = (char *)FIFO_HEAD_PTR(FIFO_RX);
	command--;
	if ((*command != 13) && (*command != 10) && (*command != 0)) return;
	
	
	// в конец подставляем символ окончания строки
	if (!FIFO_IS_FULL(FIFO_RX))
		FIFO_PUSH(FIFO_RX, 0);
		
	
	fprintf(file, "%s", (char *)FIFO_FRONT_PTR(FIFO_RX));
	fprintf_P(file, MSG_CLRF);
															
			
			
	result = cmd_parse_str((char *)FIFO_FRONT_PTR(FIFO_RX), 
							dro_config.is_cmd, 
							&command, 
							&command_index, 
							args, 
							&args_num);
						
	if (!dro_config.is_cmd && (command_index >= CMD_CONSOLE_FUNC_BEGIN_INDEX))
		result = CRF_COMMAND_NOT_FOUND;
									
	if (result != CRF_OK)
		cmd_send_result(result, command, args, args_num);
	else{
		result = cmd_exec_func(command_index, args, args_num);
		if (result != CRF_EMPTY)
			cmd_send_result(result, command, args, args_num);
	}
			
	if (dro_config.is_cmd)
		fprintf_P(file, CMD_SETUP);
			
	FIFO_FLUSH(FIFO_RX);

#endif	
}


cmd_result_func_e cmd_exec_func(int8_t func_no, cmd_argument_s * args, uint8_t args_num){
	if (func_no < 0) return CRF_COMMAND_NOT_FOUND;
	cmd_result_func_e result = CRF_OK;
	cmd_result_func_e(* handler)	(cmd_argument_s * args, uint8_t args_num) = NULL;
	handler = (void*)pgm_read_word(&(sys_func[func_no]));
	if (handler != NULL)
		result = handler(args, args_num);
	else
		result = CRF_COMMAND_NOT_FOUND;	
	return result;
}


void cmd_send_result(cmd_result_func_e result, char * command, cmd_argument_s * args, uint8_t args_num){
	
	FILE * file = uart_s();	
	uint8_t	is_clrf	= 0;
	
	if (result == CRF_OK)
		fprintf_P(file, MSG_CRF_OK, command);
	else
		if ((result == CRF_INVALID_ARG) || (result == CRF_INVALID_VALUE)){
			for (uint8_t i=0; i<args_num; i++){
				is_clrf = 0;
				if (args[i].val_type == CTV_ERROR_ARG){
					fprintf_P(file, MSG_CRF_ERROR, command);
					fprintf_P(file, MSG_CRF_INVALID_ARG, args[i].parse_arg);
					is_clrf = 1;
				}
				else
					if (args[i].val_type == CTV_ERROR_VAL){
						fprintf_P(file, MSG_CRF_ERROR, command);
						fprintf_P(file, MSG_CRF_INVALID_VALUE, args[i].parse_arg, args[i].parse_val);
						is_clrf = 1;
					}
					
				if (is_clrf)
					fprintf_P(file, MSG_CLRF);
					
			}
		}
		else{
			fprintf_P(file, MSG_CRF_ERROR, command);
			fprintf_P(file, (char*)pgm_read_word(&(cmd_messages[result - 1])));
		}
		
	
	if (!is_clrf)
		fprintf_P(file, MSG_CLRF);
}

cmd_result_func_e cmd_parse_str(char * str, uint8_t is_parse_arg, char ** command, int8_t * index_command, cmd_argument_s * args, uint8_t * args_num){
	typedef enum  {
		find_command,
		parse_command,
		find_slash,
		find_arg,
		parse_arg,
		find_val,
		parse_val
	} state_e;
		
	cmd_result_func_e	result			= CRF_COMMAND_NOT_FOUND;
	state_e				state			= find_command;
						*command		= NULL;
						*index_command	= -1;
						*args_num		=  0;
	uint8_t				is_quotes		=  0;					
	
	if (is_parse_arg)
		for (uint8_t i=0; i<CMD_MAX_ARGS; i++){
			args[i].parse_arg		= NULL;
			args[i].parse_val		= NULL;
			args[i].val_t.value		= 0;
			args[i].val_t.dp_pos	= 0;
			
		}
	
	while (*str != 0){
		if ((state != find_val) && (state != parse_val)) // значения аргументов к нижнему регистру не приводим
			*str = tolower(*str);
		if (*str < 32)
			*str = ' ';
			
		switch (state){
			case find_command:
			if (*str != ' '){
				*command = str;
				state = parse_command;
			}
			break;
				
			case parse_command:
			if (*str == ' '){
				*str = 0;
				if (!is_parse_arg) goto BREAK_LOOP;
				state = find_slash;
			}
			break;
				
			case find_slash:
			if (*str == '/')
			state = find_arg;
				
			break;
				
			case find_arg:
				if (*str != ' '){
					if (*args_num == CMD_MAX_ARGS) return CRF_TOO_MANY_ARGUMENTS;
					(*args_num)++;
					
					args[(*args_num) - 1].parse_arg = str;
					state = parse_arg;
				}
				
			break;
				
			case parse_arg:
				if (*str == ' '){
					*str = 0;
					state = find_val;
					is_quotes = 0;
				}
			break;
				
			case find_val:
				if (*str != ' '){
					args[(*args_num) - 1].parse_val = str;
					state = parse_val;
					is_quotes = ((*str == 34) || (*str == 39));
				}
				
			break;
				
			case parse_val:
				if (is_quotes){
					if ( (*str == 34) || (*str == 39) )
						is_quotes = 0;
				}
				else
					if (*str == ' '){
						*str = 0;
						state = find_slash;
					}
			break;
		}
			
		str++;
	}	
	
	BREAK_LOOP:
	
	if (*command != NULL)
		result = CRF_OK;
	
	if (result == CRF_OK){
		
		for(uint8_t i=0; i<TOKENS_COMMAND_SIZE; i++)
			if (strcmp_P(*command, (char*)pgm_read_word(&(tokens_command[i]))) == 0 ){
				*index_command = i;
				break;
			}
		
		if (*index_command < 0)
			result = CRF_COMMAND_NOT_FOUND;
				
		if (result == CRF_OK)
			for (uint8_t i=0; i<(*args_num); i++){
				cmd_parse_arg(&args[i]);
				if (args[i].arg != T_ARG_NULL){
					cmd_parse_val(&args[i]);
			
					if (args[i].val_type == CTV_ERROR_VAL)
						result = CRF_INVALID_VALUE;	
				}
				else{
					args[i].val_type = CTV_ERROR_ARG;
					result = CRF_INVALID_ARG;
				}
			}
	}
		
	return result;
}


void cmd_parse_arg(cmd_argument_s * arg){
	arg->arg = T_ARG_NULL;
	for(uint8_t i=0; i<TOKENS_ARG_SIZE; i++)
		if (strcmp_P(arg->parse_arg, (char*)pgm_read_word(&(args_map[i].str))) == 0 ){
			arg->arg = pgm_read_byte(&(args_map[i].arg_e));
			break;
		}
	
}

void cmd_parse_val(cmd_argument_s * arg){
	arg->val_type = CTV_UNDEFINED;
	uint8_t is_expected_enum = 0;
	unit_value_ex_e unit_value = UV_NONE;
	
	for(uint8_t i=0; i<TOKENS_VAL_SIZE; i++)
		if (arg->arg == pgm_read_byte(&(vals_map[i].arg_e))){
			is_expected_enum = 1;
			if (strcmp_P(arg->parse_val, (char*)pgm_read_word(&(vals_map[i].str))) == 0 ){
				arg->val_type = CTV_ENUM;
				arg->val_enum = pgm_read_word(&(vals_map[i].val));
				break;
			}
		}
	
	if (is_expected_enum && (arg->val_type != CTV_ENUM))
		arg->val_type = CTV_ERROR_VAL;
			
	if (arg->val_type == CTV_UNDEFINED){
		if (str_is_empty(arg->parse_val))
			arg->val_type = CTV_EMPTY;
		else
			if (str_is_quotes(arg->parse_val, &arg->val_str))
				arg->val_type = CTV_STR;
			else
				if (str_is_bits_enum(arg->parse_val, &arg->val_bit))
					arg->val_type = CTV_BIT;	
				else
					if (str_to_int32_t(arg->parse_val, &arg->val_t.value, &arg->val_t.dp_pos, &unit_value)){
						arg->val_type = CTV_VALUE_T;
						arg->unit_val = unit_value;
					}
					else
						arg->val_type = CTV_ERROR_VAL;	
		
	}
}
