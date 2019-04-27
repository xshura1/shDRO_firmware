/*
 * transmission_data.c
 *
 * Created: 09.09.2018 9:00:06
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 

#include "dro_config.h"
#include "dro_types.h"
#include "transmission_data.h"
#include "utils.h"


void init_transmission(transmission_source_e transmission_sources){
	if ((transmission_sources & LED) == LED)
		init_led();
	
	if ((transmission_sources & UART) == UART)
		init_uart();
			
}

void shutdown_transmission(){
	shutdown_led();
	
}

void transmit_value(value_t* value, device_led_e led, transmission_source_e sources){
	char buf[20];
	char * b = (char *)&buf;
	
	
	b = value_t_to_str(b, value);
	
	if ((sources & LED) == LED)
		fprintf(led_s(led), "%.8s", b);
	
	if ((sources & UART) == UART)
		transmit_uart((char*)b);
	
}
