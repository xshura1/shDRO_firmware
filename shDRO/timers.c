/*
 * timers.c
 *
 * Created: 27.07.2018 15:03:45
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

#include "hardware.h"
#include "timers.h"
#include "dro_config.h"
#include "shared_memory.h"
#include "buzzer.h"

ISR(TIMER0_COMPA_vect){
	static uint8_t i_10;
	timer_cnt_1ms++;
	i_10++;
	
	if (i_10 == 10){
		i_10 = 0;
		timer_cnt_10ms++;
		if (is_play){
			if (sound_cnt == 0){
				if ((TCCR1B & (1<<CS10))){
					if (sound_cycles < 255){
						if (sound_cycles > 0)
							sound_cycles --;
							
						if (sound_cycles == 0){
							is_play = 0;
							TCCR1B &= ~(1<<CS10);
							TCCR1A &= ~(1 << COM1A0);
						}
					}
						
					
					if (sound_pause_cnt > 0){
						sound_cnt = sound_pause_cnt;
						TCCR1B &= ~(1<<CS10);
						TCCR1A &= ~(1 << COM1A0);
					}
					else
						sound_cnt = sound_play_cnt;						
					return;
				}
				else{
					sound_cnt = sound_play_cnt;
					TCCR1A |= (1 << COM1A0);
					TCCR1B |= (1<<CS10);
					return;
				}
			}
				
			sound_cnt--;
		}
	}
}

ISR(TIMER1_COMPA_vect){
	timer1_cnt_8us++;
}


uint16_t get_timer_mode(uint32_t time_us){
	uint16_t	csN[4]	= {5, 40, 160, 640};
	uint32_t	res[4];	
	int8_t		cs_i	= -1;
	uint16_t	cnt		= UINT16_MAX;
	uint16_t	result	= 0;
	
	for (uint8_t i = 0; i<4; i++)
		if (!((time_us * 10) % csN[i]))		
			res[i] = ((time_us * 10)/csN[i]) - 1;
		else
			res[i] = 0;	

	for (uint8_t i = 0; i<4; i++)
		if (res[i] > 0)
			if (res[i] < cnt){
				cnt = res[i];
				cs_i = i;
			}
	if (cs_i > -1)	
		result = (cnt | ((cs_i + 2)<<CLK_SHIFT));
	return result;
}

uint8_t init_timer0(uint32_t time_us){
	uint16_t mode = get_timer_mode(time_us);
	timer_cnt_1ms	= 0;
	timer_cnt_10ms	= 0;
		
	TIMSK0 &= ~(1<<OCIE0A); 
	TCNT0 = 0; 
	
	if (mode == 0) return 0;
	
	TCCR0A |= (1<<WGM01); 
	TCCR0B |= (mode >> CLK_SHIFT) & 0x07;
		
	OCR0A = mode & 0xFF;
	
	TIMSK0 |= (1<<OCIE0A);
	return mode;
}

void init_timer1_for_buzzer(uint16_t sound){
	TIMSK1	&=	~(1<<OCIE1A);
	TCCR1A &= ~(1 << COM1A0);
	ICR1 = sound;	
	TCNT1 = 0x0000;	
	TCCR1B &= ~(1 << CS10);
	TCCR1B = (1 << WGM13)|(1 << WGM12);
}

uint8_t init_timer1_for_logic_analizer(uint16_t time_us){
	uint16_t mode = get_timer_mode(time_us);
	timer1_cnt_8us	= 0;
	
	TIMSK1	&=	~(1<<OCIE1A); 
	TCNT1	=	0x00;
	TCCR1A	&=	~(1 << COM1A0);
	
	if (mode == 0) return 0;
	
	TCCR1A = 0x00;
	TCCR1B |= (1 << WGM12); 
	TCCR1B &= ~(1 << WGM13);
	TCCR1B |= (mode >> CLK_SHIFT) & 0x07;
	
	OCR1A = mode & 0x1FFF;
	
	TIMSK1 |= (1 << OCIE1A); 
	return mode;
}

