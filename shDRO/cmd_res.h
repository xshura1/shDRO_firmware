/*
 * cmd_res.h
 *
 * Created: 28.02.2019 10:27:50
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef CMD_RES_H_
#define CMD_RES_H_

#include <avr/pgmspace.h>

#include "dro_types.h"


//-----------------------------------Сообщения консоли-------------------------------------------------------



const static char CMD_SETUP[]					PROGMEM = ">";


const static char CMD_MSG_ARG_INFO_S[]			PROGMEM = "/%s %s	";
const static char CMD_MSG_ARG_INFO_D[]			PROGMEM = "/%s %d	";

const static char CMD_MSG_DEVICE_INFO[]			PROGMEM = "/dev %d	";
const static char CMD_MSG_CHANNEL_INFO[]		PROGMEM = "/ch %d	";
const static char CMD_MSG_CHANNEL_PD[]			PROGMEM = "[PD%d]	";
const static char CMD_MSG_CHANNEL_PC[]			PROGMEM = "[PC%d]	";

const static char CMD_MSG_TEST_CHANNELS[]		PROGMEM = "Freq (HZ) = %ld Ch%d =	";
const static char CMD_MSG_PRESS_ANY_KEY[]		PROGMEM = "(press any key to stop test)";
const static char CMD_MSG_TEST_CHANNELS_INT[]	PROGMEM = "Test was interrupted";

const static char CMD_MSG_KEYBOARD_INFO[]		PROGMEM = "Key code = %02d ADC Channel = %d	ADC Value = %d";
//-----------------------------------------------------------------------------------------------------------

// команды---------------------------------------------------------------------------------------------------
#define TOKENS_COMMAND_SIZE				22
#define CMD_CONSOLE_FUNC_BEGIN_INDEX	2

const static char TOKEN_COMMAND_HELP[]				PROGMEM = "help";
const static char TOKEN_COMMAND_SETUP[]				PROGMEM = "setup";

const static char TOKEN_COMMAND_EXIT[]				PROGMEM = "exit";					
const static char TOKEN_COMMAND_ADD_DEVICE[]		PROGMEM = "adddev";				
const static char TOKEN_COMMAND_ATTACH_CHANNEL[]	PROGMEM = "addch";			
const static char TOKEN_COMMAND_BUZZER[]			PROGMEM = "buz";				
const static char TOKEN_COMMAND_CHANNELS_INFO[]		PROGMEM = "chinfo";			
const static char TOKEN_COMMAND_EDIT_CHANNEL[]		PROGMEM = "editch";
const static char TOKEN_COMMAND_DEL_DEVICE[]		PROGMEM = "deldev";				
const static char TOKEN_COMMAND_DETACH_CHANNEL[]	PROGMEM = "delch";			
const static char TOKEN_COMMAND_DEVICES_INFO[]		PROGMEM = "devinfo";			
const static char TOKEN_COMMAND_DRO_INFO[]			PROGMEM = "about";				
const static char TOKEN_COMMAND_KEYBOARD_INFO[]		PROGMEM = "keyinfo";				
const static char TOKEN_COMMAND_LED_BLINK[]			PROGMEM = "ledblink";				
const static char TOKEN_COMMAND_LED_CLR[]			PROGMEM = "ledclr";					
const static char TOKEN_COMMAND_LED_MSG[]			PROGMEM = "ledmsg";					
const static char TOKEN_COMMAND_LEDS_INFO[]			PROGMEM = "ledinfo";				
const static char TOKEN_COMMAND_LOAD[]				PROGMEM = "load";
const static char TOKEN_COMMAND_SAVE[]				PROGMEM = "save";
const static char TOKEN_COMMAND_EDIT_DEVICE[]		PROGMEM = "editdev";
const static char TOKEN_COMMAND_TEST_CHANNELS[]		PROGMEM = "testch";
const static char TOKEN_COMMAND_LED_LUMI[]			PROGMEM = "ledlumi";

const static char * const  tokens_command[TOKENS_COMMAND_SIZE] PROGMEM  =
{
	TOKEN_COMMAND_HELP,									//	cmd_help-
	TOKEN_COMMAND_SETUP,								//	cmd_setup-
	
	TOKEN_COMMAND_EXIT,									//	cmd_exit-
	TOKEN_COMMAND_ADD_DEVICE,							//	cmd_adddevice-
	TOKEN_COMMAND_ATTACH_CHANNEL,						//	cmd_attachchannel-
	TOKEN_COMMAND_BUZZER,								//	cmd_buzzer-
	TOKEN_COMMAND_CHANNELS_INFO,						//	cmd_channelsinfo-
	TOKEN_COMMAND_EDIT_CHANNEL,							//	cmd_editchannel-
	TOKEN_COMMAND_DEL_DEVICE,							//	cmd_deldevice-
	TOKEN_COMMAND_DETACH_CHANNEL,						//	cmd_detachchannel-
	TOKEN_COMMAND_DEVICES_INFO,							//	cmd_devicesinfo-
	TOKEN_COMMAND_DRO_INFO,								//	cmd_droinfo-
	TOKEN_COMMAND_KEYBOARD_INFO,						//	cmd_keyboardinfo-
	TOKEN_COMMAND_LED_BLINK,							//	cmd_ledblink-
	TOKEN_COMMAND_LED_CLR,								//	cmd_ledclr-
	TOKEN_COMMAND_LED_MSG,								//	cmd_ledmsg-
	TOKEN_COMMAND_LEDS_INFO,							//	cmd_ledinfo-
	TOKEN_COMMAND_LOAD,									//	cmd_load-
	TOKEN_COMMAND_SAVE,									//	cmd_save-
	TOKEN_COMMAND_EDIT_DEVICE,							//	cmd_edit_device-
	TOKEN_COMMAND_TEST_CHANNELS,						//	cmd_test_channels-
	TOKEN_COMMAND_LED_LUMI								//	cmd_led_lumi-
};
//-----------------------------------------------------------------------------------------------------------

// аргументы-------------------------------------------------------------------------------------------------

const static char TOKEN_ARG_DEV[]			PROGMEM = "dev";
const static char TOKEN_ARG_EI[]			PROGMEM = "ei";
const static char TOKEN_ARG_FUNC[]			PROGMEM = "func";
const static char TOKEN_ARG_FREQ[]			PROGMEM = "freq";
const static char TOKEN_ARG_IL[]			PROGMEM = "il";
const static char TOKEN_ARG_INV[]			PROGMEM = "inv";
const static char TOKEN_ARG_LED[]			PROGMEM = "led";
const static char TOKEN_ARG_MSG[]			PROGMEM = "msg";
const static char TOKEN_ARG_CH[]			PROGMEM = "ch";
const static char TOKEN_ARG_PROTOCOL[]		PROGMEM = "p";
const static char TOKEN_ARG_TIME[]			PROGMEM = "time";
const static char TOKEN_ARG_UNIT[]			PROGMEM = "unit";
const static char TOKEN_ARG_TYPE[]			PROGMEM = "type";
const static char TOKEN_ARG_EX[]			PROGMEM = "ex";
const static char TOKEN_ARG_PERIOD[]		PROGMEM = "pr";
const static char TOKEN_ARG_SAMPLES[]		PROGMEM = "smp";
const static char TOKEN_ARG_NOTE[]			PROGMEM = "note";
const static char TOKEN_ARG_DURATION_ON[]	PROGMEM = "don";
const static char TOKEN_ARG_DURATION_OFF[]	PROGMEM = "doff";
const static char TOKEN_ARG_CNT[]			PROGMEM = "cnt";
const static char TOKEN_ARG_LUMI[]			PROGMEM = "lumi";

#define TOKENS_ARG_SIZE 21

typedef enum {
	T_ARG_NULL				= 0,
	T_ARG_DEVICE			= 1,
	T_ARG_ENABLE_INTERRUPT	= 2,
	T_ARG_FUNCTION			= 3,
	T_ARG_INTERRUPT_LEVEL	= 4,
	T_ARG_INVERT			= 5,
	T_ARG_TYPE				= 6,
	T_ARG_LED				= 7,
	T_ARG_MSG				= 8,
	T_ARG_CH				= 9,
	T_ARG_PROTOCOL			= 10,
	T_ARG_TIME_MS			= 11,
	T_ARG_UNIT				= 12,
	T_ARG_FREQUENCE			= 13,
	T_ARG_EX				= 14,
	T_ARG_PERIOD			= 15,
	T_ARG_SAMPLES			= 16,
	T_ARG_NOTE				= 17,
	T_ARG_DURATION_ON		= 18,
	T_ARG_DURATION_OFF		= 19,
	T_ARG_CNT				= 20,
	T_ARG_LUMI				= 21
	
} token_arg_e;

const static struct PROGMEM {
	const char * str;
	token_arg_e	arg_e;
} args_map[TOKENS_ARG_SIZE] =
{
	{TOKEN_ARG_DEV,				T_ARG_DEVICE			},
	{TOKEN_ARG_EI,				T_ARG_ENABLE_INTERRUPT	},
	{TOKEN_ARG_FUNC,			T_ARG_FUNCTION			},
	{TOKEN_ARG_IL,				T_ARG_INTERRUPT_LEVEL	},
	{TOKEN_ARG_INV,				T_ARG_INVERT			},
	{TOKEN_ARG_TYPE,			T_ARG_TYPE				},
	{TOKEN_ARG_LED,				T_ARG_LED				},
	{TOKEN_ARG_MSG,				T_ARG_MSG				},
	{TOKEN_ARG_CH,				T_ARG_CH				},
	{TOKEN_ARG_PROTOCOL,		T_ARG_PROTOCOL			},
	{TOKEN_ARG_TIME,			T_ARG_TIME_MS			},
	{TOKEN_ARG_UNIT,			T_ARG_UNIT				},
	{TOKEN_ARG_FREQ,			T_ARG_FREQUENCE			},
	{TOKEN_ARG_EX,				T_ARG_EX				},
	{TOKEN_ARG_PERIOD,			T_ARG_PERIOD			},
	{TOKEN_ARG_SAMPLES,			T_ARG_SAMPLES			},
	{TOKEN_ARG_NOTE,			T_ARG_NOTE				},
	{TOKEN_ARG_DURATION_ON,		T_ARG_DURATION_ON		},
	{TOKEN_ARG_DURATION_OFF,	T_ARG_DURATION_OFF		},
	{TOKEN_ARG_CNT,				T_ARG_CNT				},
	{TOKEN_ARG_LUMI,			T_ARG_LUMI				}
};

//-----------------------------------------------------------------------------------------------------------

// значения--------------------------------------------------------------------------------------------------
#define TOKENS_VAL_SIZE 28

const static char TOKEN_VAL_01[]				PROGMEM = "01";		// прерывания
const static char TOKEN_VAL_10[]				PROGMEM = "10";
const static char TOKEN_VAL_0_1[]				PROGMEM = "0|1";

const static char TOKEN_VAL_24BIT[]				PROGMEM = "24bit";	// протоколы
const static char TOKEN_VAL_21BIT[]				PROGMEM = "21bit";
const static char TOKEN_VAL_BCD7[]				PROGMEM = "bcd7";
const static char TOKEN_VAL_BIN6[]				PROGMEM = "bin6";	
const static char TOKEN_VAL_QUAD[]				PROGMEM = "quad";
const static char TOKEN_VAL_PULSE[]				PROGMEM = "pulse";

const static char TOKEN_VAL_X[]					PROGMEM = "x";		// назначение устройства
const static char TOKEN_VAL_Y[]					PROGMEM = "y";
const static char TOKEN_VAL_Z[]					PROGMEM = "z";
const static char TOKEN_VAL_ANGLE[]				PROGMEM = "angle";
const static char TOKEN_VAL_TACHO[]				PROGMEM = "tacho";
const static char TOKEN_VAL_EDGE[]				PROGMEM = "edge";
const static char TOKEN_VAL_OTHER[]				PROGMEM = "other";

const static char TOKEN_VAL_INCH[]				PROGMEM = "inch";	// единицы измерения
const static char TOKEN_VAL_MM[]				PROGMEM = "mm";

const static char TOKEN_VAL_UNDEFINED[]			PROGMEM = "undef";	// тип канала
const static char TOKEN_VAL_CLK_IN[]			PROGMEM = "clkin";
const static char TOKEN_VAL_CLK_OUT[]			PROGMEM = "clkout";
const static char TOKEN_VAL_DATA_IN[]			PROGMEM = "datain";
const static char TOKEN_VAL_A_IN[]				PROGMEM = "ain";
const static char TOKEN_VAL_B_IN[]				PROGMEM = "bin";
const static char TOKEN_VAL_Z_IN[]				PROGMEM = "zin";
const static char TOKEN_VAL_ZERO_OUT[]			PROGMEM = "zeroout";
const static char TOKEN_VAL_CHANGE_UNIT_OUT[]	PROGMEM = "unitout";
const static char TOKEN_VAL_FASTMODE_OUT[]		PROGMEM = "fastout";


const static struct PROGMEM {
	const char * str;
	uint16_t	val;
	token_arg_e arg_e;
} vals_map[TOKENS_VAL_SIZE] =
{
	// прерывания
	{TOKEN_VAL_01,				INT_LEVEL_01,			T_ARG_INTERRUPT_LEVEL},
	{TOKEN_VAL_10,				INT_LEVEL_10,			T_ARG_INTERRUPT_LEVEL},
	{TOKEN_VAL_0_1,				INT_LEVEL_ALL,			T_ARG_INTERRUPT_LEVEL},
	// протоколы
	{TOKEN_VAL_24BIT,			BIT24_PROTOCOL,			T_ARG_PROTOCOL},
	{TOKEN_VAL_21BIT,			BIT21_PROTOCOL,			T_ARG_PROTOCOL},
	{TOKEN_VAL_BCD7,			BCD7_PROTOCOL,			T_ARG_PROTOCOL},
	{TOKEN_VAL_BIN6,			BIN6_PROTOCOL,			T_ARG_PROTOCOL},
	{TOKEN_VAL_QUAD,			QUAD_PROTOCOL,			T_ARG_PROTOCOL},
	{TOKEN_VAL_PULSE,			PULSE_PROTOCOL,			T_ARG_PROTOCOL},
	// назначение
	{TOKEN_VAL_X,				DA_AXIS_X,				T_ARG_FUNCTION},
	{TOKEN_VAL_Y,				DA_AXIS_Y,				T_ARG_FUNCTION},
	{TOKEN_VAL_Z,				DA_AXIS_Z,				T_ARG_FUNCTION},
	{TOKEN_VAL_ANGLE,			DA_ANGLE_SENSOR,		T_ARG_FUNCTION},
	{TOKEN_VAL_TACHO,			DA_TACHOMETER,			T_ARG_FUNCTION},
	{TOKEN_VAL_EDGE,			DA_EDGE_FINDER,			T_ARG_FUNCTION},
	{TOKEN_VAL_OTHER,			DA_OTHER,				T_ARG_FUNCTION},
	// единицы измерения
	{TOKEN_VAL_INCH,			UV_INCH,				T_ARG_UNIT},
	{TOKEN_VAL_MM,				UV_MM,					T_ARG_UNIT},
	// тип канала	
	{TOKEN_VAL_UNDEFINED,		TCH_UNDEFINED,			T_ARG_TYPE},		
	{TOKEN_VAL_CLK_IN,			TCH_CLK_IN,				T_ARG_TYPE},
	{TOKEN_VAL_CLK_OUT,			TCH_CLK_OUT,			T_ARG_TYPE},
	{TOKEN_VAL_DATA_IN,			TCH_DATA_IN,			T_ARG_TYPE},
	{TOKEN_VAL_A_IN,			TCH_A_IN,				T_ARG_TYPE},
	{TOKEN_VAL_B_IN,			TCH_B_IN,				T_ARG_TYPE},
	{TOKEN_VAL_Z_IN,			TCH_Z_IN,				T_ARG_TYPE},
	{TOKEN_VAL_ZERO_OUT,		TCH_SET_ZERO_OUT,		T_ARG_TYPE},
	{TOKEN_VAL_CHANGE_UNIT_OUT,	TCH_CHANGE_UNIT_OUT,	T_ARG_TYPE},
	{TOKEN_VAL_FASTMODE_OUT,	TCH_SET_FAST_MODE_OUT,	T_ARG_TYPE}
	
};

//-----------------------------------------------------------------------------------------------------------
//единицы измерения
#define TOKENS_UNIT_SIZE 10

const static char TOKEN_UNIT_INCH[]				PROGMEM = "inch";
const static char TOKEN_UNIT_MM[]				PROGMEM = "mm";
const static char TOKEN_UNIT_RPM[]				PROGMEM = "rpm";
const static char TOKEN_UNIT_DEGREE[]			PROGMEM = "deg";
const static char TOKEN_UNIT_US[]				PROGMEM = "us";
const static char TOKEN_UNIT_MS[]				PROGMEM = "ms";
const static char TOKEN_UNIT_S[]				PROGMEM = "s";
const static char TOKEN_UNIT_HZ[]				PROGMEM = "hz";
const static char TOKEN_UNIT_KHZ[]				PROGMEM = "khz";
const static char TOKEN_UNIT_MHZ[]				PROGMEM = "mhz";

const static char * const  tokens_unit[TOKENS_UNIT_SIZE] PROGMEM  =
{
	TOKEN_UNIT_INCH,
	TOKEN_UNIT_MM,
	TOKEN_UNIT_RPM,
	TOKEN_UNIT_DEGREE,
	TOKEN_UNIT_US,
	TOKEN_UNIT_MS,
	TOKEN_UNIT_S,
	TOKEN_UNIT_HZ,
	TOKEN_UNIT_KHZ,
	TOKEN_UNIT_MHZ
};

#endif /* CMD_RES_H_ */