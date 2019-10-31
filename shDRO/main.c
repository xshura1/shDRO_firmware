/*
 * shDRO.c
 *
 * Created: 26.07.2018 16:17:32
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 



#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>

#include "hardware.h"
#include "dro_config.h"
#include "protocol.h"
#include "keyboard.h"
#include "power.h"
#include "led.h"
#include "cmd.h"
#include "functions.h"

#include <util/delay.h>

int main(void){
	// инициализация
	init_dro();
		
	sei();
	start_keyboard(1);
		
	while (1){
		
		if (!FIFO_IS_EMPTY(FIFO_RX)){
			if (!dro_config.is_cmd){
				cmd_check();
				if (dro_config.is_cmd){
					dro_config.old_state = dro_config.state;
					dro_config.state = DRO_BEGIN_CMD;
				}	
			}
		}
			
		switch (dro_config.state){
			case DRO_INFO:
				show_dro_info(LED | UART);
				dro_config.state = DRO_REQUEST_REPAIR_KEYBOARD;	
			break;
			
			case DRO_REQUEST_REPAIR_KEYBOARD:
				
				for (uint8_t i=0; i<10; i++){
					#ifndef DEBUG
						_delay_ms(10);
					#endif	
					if (keyboard.is_pressed_key){
						dro_config.state = DRO_BEGIN_REPAIR_KEYBOARD;
						break;
					}
					else
						dro_config.state = DRO_NORMAL;
				}
							
			break;
			
			case DRO_NORMAL:
				get_data();
				tracsmission_data();
				scan_keyboard();
				check_led();
				//check_power();
			break;
									
			case DRO_BEGIN_REPAIR_KEYBOARD:
				disable_channels_interrupts();
				cli();
				repair_keyboard(1);
				dro_config.state = DRO_REPAIR_KEYBOARD;
				sei();
			break;	
			
			case DRO_REPAIR_KEYBOARD:
				if (!keyboard.is_repair)
					dro_config.state = DRO_END_REPAIR_KEYBOARD;
			break;
			
			case DRO_END_REPAIR_KEYBOARD:
				save_keyboard_to_eeprom();
				cli();
				enable_channels_interrupts();
				dro_config.state = DRO_NORMAL;
				sei();
			break;	
			
			case DRO_BEGIN_CMD:
				// TODO проанализировать, нужно ли отключать прерывания при входе в консоль
				disable_channels_interrupts();
				dro_config.state = DRO_CMD;
			break;
			
			case DRO_CMD:
				cmd_check();
				if (!dro_config.is_cmd){
					enable_channels_interrupts();
					dro_config.state = dro_config.old_state;
				}
				
				check_led();
				#ifndef DEBUG
				_delay_ms(2);
				#endif
					
			break;
			
			case DRO_SHUTDOWN:
			break;
					
		}
	}
	
	return 1;
}
