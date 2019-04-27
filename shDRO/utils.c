/*
 * utils.c
 *
 * Created: 09.09.2018 12:32:04
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <avr/pgmspace.h>
#include <math.h>

#include "dro_types.h"
#include "cmd_res.h"


typedef struct {
	uint64_t quot;
	uint8_t rem;
} divmod10_t;

inline static divmod10_t divmodu10(int32_t n){
	divmod10_t res;
	// умножаем на 0.8
	res.quot = n >> 1;
	res.quot += res.quot >> 1;
	res.quot += res.quot >> 4;
	res.quot += res.quot >> 8;
	res.quot += res.quot >> 16;
	uint32_t qq = res.quot;
	// делим на 8
	res.quot >>= 3;
	// вычисляем остаток
	res.rem = (uint8_t)(n - ((res.quot << 1) + (qq & ~7ull)));
	// корректируем остаток и частное
	if(res.rem > 9)
	{
		res.rem -= 10;
		res.quot++; 
	}
	return res;
}


char * utoa_fast_div(int32_t value, char *buffer){
	buffer += 16;
	*--buffer = 0;
	do {
		
		divmod10_t res = divmodu10(value);
		*--buffer = res.rem + '0';
		value = res.quot;
		
	} while (value != 0);
	return buffer;
}


char * value_t_to_str(char * buffer, value_t * value){
	uint8_t i = 0;
	buffer += 16;
	*--buffer = 0;
	int32_t val = labs(value->value);
	uint8_t is_negative = (value->value < 0);
	
	do {
		if ((i == value->dp_pos) && (value->dp_pos != 0xF)){
			*--buffer = '.';
		}
		else{
			divmod10_t res = divmodu10(val);
			*--buffer = res.rem + '0';
			val = res.quot;
		}
		i++;
		
	} while (val != 0);
	
	if (value->dp_pos != 0xF){
		while (i< value->dp_pos){
			*--buffer = '0';
			i++;
		}
		
		if (i == value->dp_pos){
			*--buffer = '.';
			*--buffer = '0';
		}
	}
	
	if (is_negative)
		*--buffer = '-';
	
	
	return buffer;
}

uint32_t pow_10(uint8_t power){
	uint32_t result = 1;
	switch (power){
		case 1:
			result = 10;
			break;
		
		case 2:
			result = 100;
			break;
			
		case 3:
			result = 1000;
			break;
			
		case 4:
			result = 10000;
			break;
			
		case 5:
			result = 100000;
			break;
			
		case 6:
			result = 1000000;
			break;

		default:
			result = 1;	
	}
	return result;
}

uint8_t normalize_value(int32_t * number, uint8_t * dp_pos, unit_value_ex_e * unit_value){
	uint8_t result = 1;
	if (dp_pos > 0){
		if ((*unit_value == UV_TIME_US) || (*unit_value == UV_FREQ_HZ)) return 0;
		if (*dp_pos < 4){
			*number *= pow_10(3-*dp_pos);
			*unit_value -= 1;
			*dp_pos = 0;
		}
		else
			if (*dp_pos < 7){
				*number *= pow_10(6-*dp_pos);
				*unit_value -= 2;
				*dp_pos = 0;
			}
			else
				result = 0;
	}
	
	return result;
}

uint8_t str_to_int32_t(char * str, int32_t * number, uint8_t * dp_pos, unit_value_ex_e * unit_value){
	uint8_t result		=  0;
	uint8_t is_negative =  0;
	int8_t	dp			= -1;
	int32_t tmp			=  0;
	char * unit_str		= NULL;
	
	
	dp			= -1;
	*dp_pos		=  0;
	*unit_value	= UV_NONE;
	
	// TODO добавить проверку на 234ввв например - результат должен быть отрицательным	
	while (*str != 0){
		if ((*str >= '0') && (*str <= '9')){
			tmp += (*str - '0');
			tmp *= 10;
			if (dp > -1)
				dp++;
			result = 1;
		}
		else{
			if (*str == '-'){
				if (is_negative){
					result = 0;
					break;
				}
				is_negative = 1;
			}else
				if ((*str == '.') || (*str == ',')){
					if (dp > -1) {
						result = 0;
						break;
					}
					dp++;
				}
				else
					if (*str > ' '){
						if (unit_str == NULL)
							unit_str = str;
					}
					else{
						*str = 0;
						break;
					}
		}
		
		str++;
	}
	
	if (result) {
		tmp /= 10;
		if (is_negative)
		tmp = -tmp;
		*number = tmp;
		if (dp > 0)
			*dp_pos = dp;
	
		if (unit_str != NULL){
			result = 0;
			for(uint8_t i=0; i<TOKENS_UNIT_SIZE; i++)
				if (strcmp_P(unit_str, (char*)pgm_read_word(&(tokens_unit[i]))) == 0 ){
					result = 1;
					*unit_value = i+1;
					break;
				}
		}
	}
	
	
	
	return result;
}

uint8_t str_is_empty(char * str){
	uint8_t result = 1;
	while (*str != 0){
		if (*str > ' '){
			result = 0;
			break;
		}
		str++;
	}
	return result;
}

uint8_t str_is_quotes(char * str, char ** out_str){
	
	typedef enum  {
		find_quote_begin,
		find_str,
		find_quote_end
	} state_e;
	
	uint8_t result	= 0;
	state_e state	= find_quote_begin;
	
	while (*str != 0){
		switch (state){
			case find_quote_begin:
				if ((*str == 34) || (*str == 39))
					state = find_str;
			break;
			
			case find_str:
				if (*str >= ' '){
					*out_str = str;
					state = find_quote_end;
				}
			
			break;	
			
			case find_quote_end:
				if ((*str == 34) || (*str == 39)){
					*str = 0;
					result = 1;
				}
			break;
		
		}
		
		if (result)
			break;
			
		str++;
	}
	
	return result;
}

uint8_t str_is_bits_enum(char * str,  uint16_t * out_bits){
	uint8_t		result	= 0;
	int16_t		bit		= 0;
	
	*out_bits = 0;
	
	while (*str != 0){
		if ((*str >= '0') && (*str <= '9')){
			if (bit < 0) bit = 0;
			bit += (*str - '0');
			bit *= 10;
			result = 0;
		}
		else
			if (*str == ';'){
				bit /= 10;
				
				if (bit > 15)
					return 0;
				
				*out_bits |= (1<<bit);
				bit = 0;
				result = 1;
			}
			else
				if (*str != ' '){
					result = 0;
					break;
				}
		str++;		
	}
	
	return result;
}


uint8_t calc_crc(uint8_t * data, uint16_t size){
	uint8_t crc = 0x5A;

	for (uint16_t i=0; i<size; i++)
		crc = crc ^ data[i];
	
	return crc;
}


