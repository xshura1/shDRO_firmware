/*
 * led_transmission.c
 *
 * Created: 08.09.2018 9:37:33
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 

#include <string.h>
#include <stdio.h>

#include "hardware.h"
#include "LedControl.h"
#include "dro_types.h"
#include "eeprom.h"
#include "utils.h"
#include "led.h"
#include "timers.h"
#include "transmission_data.h"

#include <util/delay.h>

typedef struct {
	uint8_t			cnt_10ms;
	device_led_e	led;
	uint8_t			is_shutdown;
} selected_led_s;

selected_led_s selected_led;

device_led_e current_led;

#define LED_TIME_BLINK_x10ms	30	


void select_led(device_led_e led){
	shutdown(selected_led.led, 0);
	if (selected_led.led != led){
		selected_led.led = led;
		selected_led.cnt_10ms = timer_cnt_10ms;
	}
	else
		selected_led.led = LED_NONE;
}



void clear_all_led(){
	for (uint8_t i=0; i<MAX_LEDS; i++)
		clearDisplay(i);
}

void shutdown_all_led(){
	for (uint8_t i = 0; i<MAX_LEDS; i++){
		shutdown(i,0);		// энергосберегающий режим
		clearDisplay(i);
	}
}

void transmit_led_ex(device_led_e led, uint8_t clear_display, char * str_data){

	uint8_t digit_no = 8;
	uint8_t is_dp = 0;
	
	if (clear_display)
		clearDisplay(led);
	
	while (*str_data != 0){
		is_dp = (*(str_data + 1) == '.');
		setChar(led, digit_no-1, *str_data, is_dp);
		digit_no--;
		
		if (digit_no == 0) break;
		
		if (is_dp)
			str_data++;
		
		str_data++;
		
	}	
}

void transmit_led(device_led_e led, char * str_data){
	transmit_led_ex(led, 1, str_data);
}



int led_put_byte(char b, FILE *file )
{
	static int8_t led_pos;
	
	if (file->len == 0)
		led_pos = 7;
			
	// позиция точки должна быть на предыдущем сегменте индикатора
	if (b != '.'){
		if ((b == 'W') || (b == 'w')){ // W,w выводим в два символа
			setChar(current_led, led_pos, 'W', 0);
			led_pos--;
			setChar(current_led, led_pos, 'w', 0);
		}
		else
			if ((b == 'M') || (b == 'm')){ // M,m выводим в два символа
				setChar(current_led, led_pos, 'M', 0);
				led_pos--;
				setChar(current_led, led_pos, 'm', 0);
			}
			else
				setChar(current_led, led_pos, b, 0);
				
		led_pos--;	
	}
	else
		setLed(current_led, led_pos + 1, 0, 1);
	
	return 0;
}

FILE led_stream = FDEV_SETUP_STREAM(led_put_byte, NULL, _FDEV_SETUP_WRITE);

FILE * led_s_ex(device_led_e led, uint8_t clear_display){
	current_led = led;
	if (clear_display)
		clearDisplay(led);
	return &led_stream;
};

FILE * led_s(device_led_e led){
	return led_s_ex(led, 1);
};

void init_led(){

	LedControlInit();
	
	for (uint8_t i = 0; i<MAX_LEDS; i++){
		shutdown(i,0);		// энергосберегающий режим
		setIntensity(i,1);	// установка яркости (0-минимум, 15-максимум)
	}
	
	
	
	for (int8_t i = MAX_LEDS - 1; i>=0; i--)
		for (int8_t j = 7; j>=0; j--)
			for (uint8_t k = 0; k<8; k++){
				setLed(i, j, k, 1);	
				#ifndef DEBUG
				_delay_ms(1);
				#endif
			}
	
	for (uint8_t i = 0; i<MAX_LEDS; i++)
		clearDisplay(i);
			
	selected_led.led = LED_NONE;		
}

void check_led(){
	if (selected_led.led == LED_NONE) return;
	
	uint8_t cnt = TIME_OFFSET_8(selected_led.cnt_10ms, timer_cnt_10ms);
	if (cnt >= LED_TIME_BLINK_x10ms){
		selected_led.is_shutdown = !selected_led.is_shutdown;
		shutdown(selected_led.led, selected_led.is_shutdown);
		selected_led.cnt_10ms = timer_cnt_10ms;
	}
}