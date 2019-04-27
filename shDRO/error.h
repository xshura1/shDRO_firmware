/*
 * error.h
 *
 * Created: 11.03.2019 20:47:22
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef ERROR_H_
#define ERROR_H_

#include <avr/pgmspace.h>

#define MSG_CRF_ERROR_SIZE 9

typedef enum {
	CRF_OK						= 0,
	CRF_CHANNEL_IS_BUSY			= 1,
	CRF_INVALID_CHANNEL_TYPE	= 2,
	CRF_INVALID_PROTOCOL		= 3,
	
	CRF_INVALID_ARG				= 4,
	CRF_INVALID_VALUE			= 5,
	CRF_COMMAND_NOT_FOUND		= 6,
	CRF_REQUIRED_ARG_NOT_FOUND	= 7,
	CRF_OUT_OF_MEMORY			= 8,
	CRF_TOO_MANY_ARGUMENTS		= 9,
	CRF_EMPTY					= 255
	
} cmd_result_func_e;

const static char MSG_CRF_OK[]						PROGMEM = "%s - OK";
const static char MSG_CRF_ERROR[]					PROGMEM = "%s - Error: ";

#define CMD_MESSAGES_SIZE 9

const static char MSG_CRF_CHANNEL_IS_BUSY[]			PROGMEM = "channel is busy";
const static char MSG_CRF_INVALID_CHANNEL_TYPE[]	PROGMEM = "invalid channel type";
const static char MSG_CRF_INVALID_PROTOCOL[]		PROGMEM = "invalid protocol";
const static char MSG_CRF_INVALID_ARG[]				PROGMEM = "invalid argument: %s";
const static char MSG_CRF_INVALID_VALUE[]			PROGMEM = "invalid value for argument '%s': %s";
const static char MSG_CRF_COMMAND_NOT_FOUND[]		PROGMEM = "command not found";
const static char MSG_CRF_REQUIRED_ARG_NOT_FOUND[]	PROGMEM = "required argument not found";
const static char MSG_CRF_OUT_OF_MEMORY[]			PROGMEM = "out of memory";
const static char MSG_CRF_TOO_MANY_ARGUMENTS[]		PROGMEM = "too many arguments";

const static char * const  cmd_messages[CMD_MESSAGES_SIZE] PROGMEM  =
{
	MSG_CRF_CHANNEL_IS_BUSY,
	MSG_CRF_INVALID_CHANNEL_TYPE,
	MSG_CRF_INVALID_PROTOCOL,
	MSG_CRF_INVALID_ARG,
	MSG_CRF_INVALID_VALUE,
	MSG_CRF_COMMAND_NOT_FOUND,
	MSG_CRF_REQUIRED_ARG_NOT_FOUND,
	MSG_CRF_OUT_OF_MEMORY,
	MSG_CRF_TOO_MANY_ARGUMENTS
	
};
							


#endif /* ERROR_H_ */