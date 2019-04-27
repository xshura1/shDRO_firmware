/*
 * protocol.h
 *
 * Created: 26.07.2018 17:13:42
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "dro_types.h"
#include "timers.h"
#include "shared_memory.h"

#define DATA_BUF(device_no)		(uint8_t*)(device_no > 1 ? &data_2 : &data_1)
#define POS_BUF(device_no)		(device_no > 1 ? pos_2 : pos_1)
#define BUFFER_OFFSET(p1, p2)	(p1 <= p2 ? p2-p1 : (256 - p1) + p2)


volatile uint8_t*		data_buf;
volatile uint8_t		pos_buf;

volatile uint8_t		is_test_channels;

void initProtocol();
void change_buf(device_ex_t * device);
uint8_t get_int_mask(device_ex_t* device);

arguments_size_s get_arguments_size(device_protocol_e protocol, device_function_e func);

void check_config_device_ex(device_ex_t * device);
void get_data();
void tracsmission_data();

void check_chinese_caliper(device_ex_t* device);
void check_quad(device_ex_t* device);

#endif /* PROTOCOL_H_ */
