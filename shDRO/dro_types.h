/*
 * dro_types.h
 *
 * Created: 26.07.2018 16:20:22
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef DRO_TYPES_H_
#define DRO_TYPES_H_


#include <stdint.h>
#include "timers.h"
#include "error.h"

typedef enum {
	DHT_INIT_DEVICE		= 0,
	DHT_GET_DATA		= 1,
	DHT_SET_DATA		= 2,
	DHT_TRANSMISSION	= 3,
	DHT_SET_ZERO		= 4,
	DHT_CHANGE_UNIT		= 5,
	DHT_SET_FAST_MODE	= 6
} device_handler_type_e;

// тип данных, передаваемых на устройства отображения
typedef enum {
	TRANSMISSION_DATA,	// передача полученных с устройства данных
	TRANSMISSION_ERROR,	// передача кода ошибки
	TRANSMISSION_CONFIG	// передача текущей конфигурации устройства (только для uart)
} transmission_type_e;

// протоколы обмена устройств
typedef enum {
	BIT24_PROTOCOL	= 0, 
	BIT21_PROTOCOL	= 1,
	BCD7_PROTOCOL	= 2,
	BIN6_PROTOCOL	= 3,
	QUAD_PROTOCOL	= 4,
	PULSE_PROTOCOL	= 5
} device_protocol_e;

// номера прерываний
typedef enum {
	//INTERRUPT_0		= 0,
	INTERRUPT_1		= 1,
	INTERRUPT_2		= 2,
	INTERRUPT_NONE	= 3	
} interrupt_e;

// отслеживаемые уровни прерываний
// TODO переименовать в interrupt_level_e
typedef enum {
	INT_LEVEL_10	= 0b00, // отслеживать прерывание при переходе с 1 на 0
	INT_LEVEL_01	= 0b01, // отслеживать прерывание при переходе с 0 на 1
	INT_LEVEL_ALL	= 0b11  // отслеживать прерывание при обоих случаях
} interrupt_value_e;

// статусы устройств
typedef enum {
	CHECK_STATE,			// проверка конфигурации
	READY_STATE,			// готов к приему новой порции данных
	PROCESSING_DATA_STATE,	// обработка принятых данных
	TRANSMISSION_DATA_STATE	// передача данных на устройства отображения
} device_state_e;

// устройства отображения (может быть задано несколько устройств отображения)
typedef enum {
	LED			= 1,	// индикатор
	UART		= 2,	// последовательный порт
} transmission_source_e;

// индикатор отображения значения для устройства
typedef enum {
	LED_0		= 0,
	LED_1		= 1,
	LED_2		= 2,		
	LED_3		= 3,
	LED_NONE	= 4
} device_led_e;

// назначение устройства
typedef enum {
	DA_AXIS_X		= 0,	// ось X
	DA_AXIS_Y		= 1,	// ось Y
	DA_AXIS_Z		= 2,	// ось Z
	DA_ANGLE_SENSOR	= 3,	// датчик угла
	DA_TACHOMETER	= 4,	// тахометр
	DA_EDGE_FINDER	= 5,	// кромкоискатель
	DA_OTHER		= 6		// другое
} device_function_e;

typedef struct {
	uint8_t				stored_arguments_size;			// размер сохраняемых в EEPROM значений
	uint8_t				arguments_size;					// общий размер структуры аргументов (включая stored_arguments_size)
} arguments_size_s;

// единицы измерения
typedef enum {
	UV_NONE,
	UV_INCH,
	UV_MM,
	UV_RPM,
	UV_DEGREE,
	UV_TIME_US,
	UV_TIME_MS,
	UV_TIME_S,
	UV_FREQ_HZ,
	UV_FREQ_KHZ,
	UV_FREQ_MHZ 
} unit_value_ex_e;

// логическое назначение канала
typedef enum {
	TCH_UNDEFINED,			// не назначен
	TCH_CLK_IN,				// синхросигнал
	TCH_CLK_OUT,			// синхросигнал
	TCH_DATA_IN,			// данные
	TCH_A_IN,				// квадратурный сигнал
	TCH_B_IN,				// квадратурный сигнал
	TCH_Z_IN,				// сигнал с реперной метки
	TCH_SET_ZERO_OUT,		// аппаратный сброс в ноль
	TCH_CHANGE_UNIT_OUT,	// аппаратное переключение единиц измерения
	TCH_SET_FAST_MODE_OUT	// аппаратное включение "быстрого режима"
} channel_type_e;

// параметры канала
// номер прерывания (INTERRUPT_0 или INTERRUPT_2) определяется порядковым номером канала
// регистры порта так же определяются номером канала
typedef struct {
	channel_type_e		type				: 4; // назначение канала
	interrupt_value_e	int_level			: 2; // уровень прерывания
	unsigned int		enable_interrupt	: 1; // разрешение прерывания
	unsigned int		is_invert			: 1; // инвертированный сигнал

} channel_t;

// целочисленное знаковое значение
typedef struct {
	int32_t	value;
	uint8_t	dp_pos;
} value_t;

/*
	В зависимости от типа подключенного устройства,
	используются различные структуры для хранения данных устройства.
	Какую именно структуру использовать, определяется протоколом обмена.
	Ниже перечислены структуры, для устройств, поддерживаемых текущей версией shDRO.
*/

//----------------- китайский штангенциркуль ----------------------------------
typedef struct {
	value_t	correction_value;
	value_t	start_value;
} stored_capacity_arguments_s;

typedef struct {
	stored_capacity_arguments_s	stored_arg;
	uint32_t	buffer;
	uint32_t	transmit_buffer;
	uint8_t		timer_value_10ms;
	uint8_t		cnt_bits;
} capacity_arguments_s;
//-----------------------------------------------------------------------------

//----------------- оптическая линейка ----------------------------------------
typedef struct {
	value_t		correction_value;
	value_t		start_value;
	uint8_t		accuracy;
} stored_optical_arguments_s;

typedef struct {
	stored_optical_arguments_s stored_arg;
	int32_t		cnt_pulse;
	uint8_t		old_state;
	uint8_t		cnt_z;
} optical_arguments_s;
//-----------------------------------------------------------------------------

//----------------- датчик угла -----------------------------------------------
typedef struct {
	value_t		pulses_per_degree;
} stored_angle_sensor_arguments_s;

typedef struct {
	stored_angle_sensor_arguments_s	stored_arg;
	uint32_t	cnt_pulse;
} angle_sensor_arguments_s;
//-----------------------------------------------------------------------------

//----------------- тахометр --------------------------------------------------
typedef struct {
	value_t		pulses_per_round;
} stored_tachometer_arguments_s;

typedef struct {
	stored_tachometer_arguments_s	stored_arg;
	uint32_t	cnt_pulse;
	uint32_t	time_10ms;
} tachometer_arguments_s;
//-----------------------------------------------------------------------------

//----------------- кромкоискатель --------------------------------------------
typedef struct {
	//uint8_t is_on;	
} edge_finder_arguments_s;
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

typedef struct {
	value_t		curr_value;
	uint8_t*			channel_numbers;
	uint8_t				count_channels;
	
	void*				arguments;				// одна из структур _arguments_s
	
	device_led_e		led;					// индикатор, закрепленный за устройством
	uint8_t				int_group_no ;			// номер группы каналов с прерываниями
	
	uint8_t		pos_buffer;
	uint8_t		old_pos_buffer;
	
	device_protocol_e	protocol			: 4; // протокол обмена
	unsigned int		dp_pos				: 4; // кол-во отображаемых знаков после запятой
	
	device_function_e	function			: 4; // функциональное назначение устройства
	unit_value_ex_e		unit				: 4; // единицы измерения 
	device_state_e		state				: 4; // статус
	unsigned int		error				: 3; // ошибка
	unsigned int		is_reset			: 1; // был сброс процессора
	
} device_ex_t;

typedef struct {
	device_protocol_e	protocol	: 4; // протокол обмена
	device_function_e	function	: 4; // функциональное назначение устройства
	device_led_e		led;
	unit_value_ex_e		unit;
	uint8_t				stored_arguments_size;
	uint8_t				arguments_size;
	uint8_t				count_channels;
	
} device_memory_s;

#endif /* DRO_TYPES_H_ */
