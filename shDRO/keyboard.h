/*
 * keyboard.h
 *
 * Created: 22.09.2018 14:29:21
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <avr/pgmspace.h>

// уровни на канале adc



/*#define KEY_1_LEVEL		122
#define KEY_2_LEVEL		128
#define KEY_3_LEVEL		134
#define KEY_4_LEVEL		141
#define KEY_5_LEVEL		152
#define KEY_6_LEVEL		162
#define KEY_7_LEVEL		172
#define KEY_8_LEVEL		184
#define KEY_9_LEVEL		203
#define KEY_10_LEVEL	220
#define KEY_11_LEVEL	241
#define KEY_12_LEVEL	254*/





// коды клавиатуры
// код клавиатуры формируется из номера входа adc * 8 + номер кнопки на входе


/*
//3
#define ADC_CH3 (3 << 3)
#define KEY_11	ADC_CH3 + 1
#define KEY_12	ADC_CH3 + 2
#define KEY_13	ADC_CH3 + 3
#define KEY_14	ADC_CH3 + 4
#define KEY_15	ADC_CH3 + 5

//4
#define ADC_CH4 (4 << 3)
#define KEY_21	ADC_CH4 + 1
#define KEY_22	ADC_CH4 + 2
#define KEY_23	ADC_CH4 + 3
#define KEY_24	ADC_CH4 + 4
#define KEY_25	ADC_CH4 + 5

//5
#define ADC_CH5 (5 << 3)
#define KEY_31	ADC_CH5 + 1
#define KEY_32	ADC_CH5 + 2
#define KEY_33	ADC_CH5 + 3
#define KEY_34	ADC_CH5 + 4
#define KEY_35	ADC_CH5 + 5*/

//6
/*
#define ADC_CH6 (6 << 4)
#define KEY_1	ADC_CH6 + 1		//97	
#define KEY_2	ADC_CH6 + 2		//98
#define KEY_3	ADC_CH6 + 3		//99
#define KEY_4	ADC_CH6 + 4		//100
#define KEY_5	ADC_CH6 + 5		//101
#define KEY_6	ADC_CH6 + 6		//102
#define KEY_7	ADC_CH6 + 7		//103
#define KEY_8	ADC_CH6 + 8		//104
#define KEY_9	ADC_CH6 + 9		//105
#define KEY_10	ADC_CH6 + 10	//106
#define KEY_11	ADC_CH6 + 11	//107
#define KEY_12	ADC_CH6 + 12	//108



// 7
#define ADC_CH7 (7 << 4)
#define KEY_17	ADC_CH7 + 1		//113
#define KEY_18	ADC_CH7 + 2		//114
#define KEY_19	ADC_CH7 + 3		//115
#define KEY_20	ADC_CH7 + 4		//116
#define KEY_21	ADC_CH7 + 5		//117
#define KEY_22	ADC_CH7 + 6		//118
#define KEY_23	ADC_CH7 + 7		//119
#define KEY_24	ADC_CH7 + 8		//120
#define KEY_25	ADC_CH7 + 9		//121
#define KEY_26	ADC_CH7 + 10	//122
#define KEY_27	ADC_CH7 + 11	//123
#define KEY_28	ADC_CH7 + 12	//124
*/

#define KEY_NULL	0

#define KEY_X		1
#define KEY_X0		2
#define KEY_Y		3
#define KEY_Y0		4
#define KEY_Z		5
#define KEY_Z0		6
#define KEY_ABS_INC		7
#define KEY_ZERO_ALL	8

#define KEY_CLR		9
#define KEY_ENTER	10
#define KEY_F		11

#define KEY_DOWN	12
#define KEY_UP		13

#define KEY_POINT	14
#define KEY_0		15
#define KEY_1		16
#define KEY_2		17
#define KEY_3		18
#define KEY_4		19
#define KEY_5		20
#define KEY_6		21
#define KEY_7		22
#define KEY_8		23
#define KEY_9		24

/*
#define KEY_Calc	2
#define KEY_Right	5
#define KEY_Left	6

#define KEY_Point	 20
#define KEY_Add		 21
#define KEY_Subtract 22
#define KEY_Multiply 23
#define KEY_Divide	 24
#define KEY_Reverse	 25

#define KEY_MSn			26
#define KEY_MRn			27

#define KEY_Mm_Inch		28
#define KEY_1div2		29
#define KEY_Mill_Lathe	30
*/


#define SIZE_MEMORY_KEYBOARD ((sizeof(adc_mapping_s) * (KEY_COUNT + 12))) + 1

#define ADC_COUNT_CHANNELS	2	// кол-во каналов ADC

#define KEY_COUNT			24 // общее количество кнопок

// диапазон входов adc
#define ADC_CH_BEGIN		6

#define ADC_MAX_NULL_VALUE	10 // максимальное значение ADC когда не нажата ни одна кнопка

typedef struct {
	uint8_t adc_value;
	unsigned int channel	:2;
	unsigned int key_code	:6;
} adc_mapping_s;


typedef struct {
	adc_mapping_s	adc_mappings[KEY_COUNT];
	uint8_t			key_code;
	uint8_t			adc_value;
	uint8_t			adc_channel;
	uint8_t			old_key_codes[ADC_COUNT_CHANNELS];
	uint8_t			cnt_ms;
	uint8_t			cnt_key;
	uint8_t			is_pressed_key; // нажата одна из кнопок
	
	unsigned int	is_processed	: 1; // кнопка обработана получателем
	unsigned int	is_repair		: 1; // происходит обновление кодов клавиатуры
	unsigned int	is_test			: 1; // происходит тест клавиатуры
	
	unsigned int	is_scan_key		: 1;// определение кода нажатой кнопки (подавления дребезга)
	
	unsigned int	is_CALC			: 1;
	unsigned int	is_F			: 1;
	
	
} keyboard_s;



volatile keyboard_s keyboard;

uint8_t init_keyboard();
void start_keyboard(uint8_t change_ch);
void repair_or_test_keyboard(uint8_t is_init);
void save_keyboard_to_eeprom(void);



#endif /* KEYBOARD_H_ */
