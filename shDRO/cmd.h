/*
 * cmd.h
 *
 * Created: 11.02.2019 14:05:17
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef CMD_H_
#define CMD_H_

#include "cmd_res.h"

#define CMD_MAX_ARGS sizeof(uart_buf) / sizeof(cmd_argument_s)

typedef enum {
	CTV_UNDEFINED,
	CTV_EMPTY,
	CTV_ENUM,
	CTV_VALUE_T,
	CTV_STR,
	CTV_BIT,
	CTV_ERROR_ARG,
	CTV_ERROR_VAL
} cmd_type_val_e;


typedef struct {
	union{
		value_t		val_t;
		uint16_t	val_bit;
		char *		val_str;
		uint16_t	val_enum;
	};
	char *			parse_arg;
	char *			parse_val;
	token_arg_e		arg;
	cmd_type_val_e	val_type : 4;
	unit_value_ex_e	unit_val : 4;	
} cmd_argument_s;


void cmd_check(void);

#endif /* CMD_H_ */
