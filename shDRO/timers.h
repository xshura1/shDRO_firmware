/*
 * timers.h
 *
 * Created: 27.07.2018 15:03:24
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>

#define PRESCALER_TIMER_0		(0<<CS01)|(1<<CS00)	//F_CPU
#define PRESCALER_TIMER_8		(1<<CS01)|(0<<CS00)	//F_CPU/8
#define PRESCALER_TIMER_64		(1<<CS01)|(1<<CS00)	//F_CPU/64
#define PRESCALER_TIMER_1024	(1<<CS02)|(1<<CS00)	//F_CPU/1024




#define TIME_OFFSET_8(cnt1, cnt2) (cnt1 <= cnt2 ? cnt2-cnt1 : (UINT8_MAX - cnt1) + cnt2)

#define CLK_SHIFT 13

/*typedef enum {
	MODE_NONE	= 0,
	MODE_4_US	= (7	| (0b010<<CLK_SHIFT)),	//250000 HZ
	MODE_5_US	= (9	| (0b010<<CLK_SHIFT)),	//200000 HZ
	MODE_8_US	= (15	| (0b010<<CLK_SHIFT)),	//125000 HZ
	MODE_10_US	= (19	| (0b010<<CLK_SHIFT)),	//100000 HZ
	MODE_20_US	= (39	| (0b010<<CLK_SHIFT)),	//50000 HZ
	MODE_40_US	= (9	| (0b011<<CLK_SHIFT)),	//25000 HZ
	MODE_50_US	= (99	| (0b010<<CLK_SHIFT)),	//20000 HZ
	MODE_80_US	= (19	| (0b011<<CLK_SHIFT)),	//12500 HZ
	MODE_100_US	= (24	| (0b011<<CLK_SHIFT)),	//10000 HZ
	MODE_200_US	= (49	| (0b011<<CLK_SHIFT)),	//5000 HZ
	MODE_400_US	= (99	| (0b011<<CLK_SHIFT)),	//2500 HZ
	MODE_500_US	= (124	| (0b011<<CLK_SHIFT)),	//2000 HZ
	MODE_800_US	= (199	| (0b011<<CLK_SHIFT)),	//1250 HZ
	MODE_1_MS	= (249	| (0b011<<CLK_SHIFT)),	//1000 HZ
	MODE_2_MS	= (124	| (0b100<<CLK_SHIFT)),	//500 HZ
	MODE_4_MS	= (249	| (0b100<<CLK_SHIFT)),	//250 HZ
	MODE_5_MS	= (1249	| (0b011<<CLK_SHIFT)),	//200 HZ
	MODE_8_MS	= (124	| (0b101<<CLK_SHIFT)),	//125 HZ
	MODE_10_MS	= (624	| (0b100<<CLK_SHIFT)),	//100 HZ
	MODE_20_MS	= (1249	| (0b100<<CLK_SHIFT))	//50 HZ
	
} timer_mode_e;*/

// 16000000, 
//	2000000,	250000,	62500,	15625 = freq
//	010			011		100		101
//	5,			40,		160,	640 a
// HZ = freq/(mode+1)
//mode = (us / a * 10)  - 1



volatile uint8_t timer_cnt_1ms;
volatile uint8_t timer_cnt_10ms;

volatile uint16_t	timer1_cnt_8us;


uint8_t init_timer0(uint32_t time_us);

uint8_t init_timer1_for_logic_analizer(uint16_t time_us);
void init_timer1_for_buzzer(uint16_t sound);

uint16_t get_timer_mode(uint32_t time_us);

#endif /* TIMERS_H_ */
