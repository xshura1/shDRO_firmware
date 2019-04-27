/*
 * buzzer.h
 *
 * Created: 23.03.2019 13:38:26
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef BUZZER_H_
#define BUZZER_H_

#define COUNT_NOTES 44

#define A2      9008 // Ля (2 октава)
#define Ais2    8580 // Ля диез
#define B2      8098 // Си
#define C3      7644 // До (3 октава)
#define Cis3    7214 // До диез
#define D3      6810 // Ре
#define Dis3    6427 // Ре диез
#define E3      6066 // Ми
#define F3      5726 // Фа
#define Fis3    5404 // Фа диез
#define G3      5101 // Соль
#define Gis3    4815 // Соль диез
#define A3      4544 // Ля
#define Ais3    4289 // Ля диез
#define B3      4049 // Си
#define C4      3821 // До (4 октава)
#define Cis4    3607 // До диез
#define D4      3404 // Ре
#define Dis4    3213 // Ре диез
#define E4      3033 // Ми
#define F4      2862 // Фа
#define Fis4    2702 // Фа диез
#define G4      2550 // Соль
#define Gis4    2407 // Соль диез
#define A4      2272 // Ля
#define Ais4    2144 // Ля диез
#define B4      2024 // Си
#define C5      1910 // До (5 октава)
#define Cis5    1803 // До диез
#define D5      1702 // Ре
#define Dis5    1606 // Ре диез
#define E5      1516 // Ми
#define F5      1431 // Фа
#define Fis5    1350 // Фа диез
#define G5      1275 // Соль
#define Gis5    1203 // Соль диез
#define A5      1135 // Ля
#define Ais5    1072 // Ля диез
#define B5      1011 // Си
#define C6      955 // До (6 октава)
#define Cis6    901 // До диез
#define D6      850 // Ре
#define Dis6    803 // Ре диез
#define E6      757 // Ми


volatile uint8_t sound_play_cnt;
volatile uint8_t sound_pause_cnt;
volatile uint8_t sound_cycles;
uint8_t sound_cnt;
volatile uint8_t is_play;

void init_buzzer(void);
void start_buzzer(uint16_t sound, uint8_t play_cnt, uint8_t pause_cnt, uint8_t cycles);
void stop_buzzer(void);

#endif /* BUZZER_H_ */