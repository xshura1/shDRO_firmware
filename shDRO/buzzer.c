/*
 * buzzer.c
 *
 * Created: 23.03.2019 13:38:10
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "buzzer.h"
#include "timers.h"
#include "hardware.h"



void start_buzzer(uint16_t sound, uint8_t play_cnt, uint8_t pause_cnt, uint8_t cycles){
	sound_play_cnt = play_cnt;
	sound_pause_cnt = pause_cnt;
	sound_cycles = cycles;
	sound_cnt = 0;
	
	init_timer1_for_buzzer(sound);
	
	is_play = 1;
}

void stop_buzzer(void){
	is_play = 0;
	TCCR1B &= ~(1<<CS10);
	TCCR1A &= ~(1 << COM1A0);
}

void init_buzzer(void){
	DDR_BUZZER |= (1 << PIN_BUZZER);
	is_play = 0;
}

