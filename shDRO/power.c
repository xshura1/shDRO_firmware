/*
 * power.c
 *
 * Created: 14.10.2018 11:04:10
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 

#include "power.h"
#include "dro_config.h"

void check_power(){
	if (POWER_VALUE){ 
		dro_config.is_shutdown = 1;
		shutdown_dro();
	}
	else
		dro_config.is_shutdown = 0;
}
