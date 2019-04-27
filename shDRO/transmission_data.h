/*
 * transmission_data.h
 *
 * Created: 09.09.2018 8:59:48
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef TRANSMISSION_DATA_H_
#define TRANSMISSION_DATA_H_

#include <stdio.h>
#include "dro_types.h"


FILE * uart_s();
FILE * led_s(device_led_e led);
FILE * led_s_ex(device_led_e led, uint8_t clear_display);


void init_transmission(transmission_source_e sources);
void shutdown_transmission();

void init_led();
void shutdown_led();
void clear_all_led();

void init_uart();

void transmit_led_ex(device_led_e led, uint8_t clear_display, char * str_data);
void transmit_led(device_led_e led, char * str_data);
void transmit_uart(char * str_data);

void transmit_value(value_t* value, device_led_e led, transmission_source_e sources);

#endif /* TRANSMISSION_DATA_H_ */
