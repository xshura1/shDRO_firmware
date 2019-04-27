/*
 * utils.h
 *
 * Created: 09.09.2018 12:31:47
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef UTILS_H_
#define UTILS_H_

char * utoa_fast_div(int32_t value, char *buffer);
uint32_t pow_10(uint8_t power);
uint8_t str_to_int32_t(char * str, int32_t * number, uint8_t * dp_pos, unit_value_ex_e * unit_value);
uint8_t normalize_value(int32_t * number, uint8_t * dp_pos, unit_value_ex_e * unit_value);
char * value_t_to_str(char * buffer, value_t * value);
uint8_t str_is_empty(char * str);
uint8_t str_is_quotes(char * str, char ** out_str);
uint8_t str_is_bits_enum(char * str,  uint16_t * out_bits);

uint8_t calc_crc(uint8_t * data, uint16_t size);

#endif /* UTILS_H_ */