/*
 * uart.c
 *
 * Created: 29.07.2018 19:54:51
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "dro_config.h"
#include "shared_memory.h"
#include "transmission_data.h"
#include "hardware.h"

#define BAUDDIVIDER ((((F_CPU + UART_BAUDRATE * 8UL) / (UART_BAUDRATE * 16UL))) - 1)
#define HI(x) ((x)>>8)
#define LO(x) ((x)& 0xFF)


ISR(USART_RX_vect){
	uint8_t rxbyte = UDR0;
	if( !FIFO_IS_FULL( FIFO_RX ) ) 
		FIFO_PUSH( FIFO_RX, rxbyte );
}

ISR( USART_UDRE_vect ){
	uint8_t txbyte = 0;
	
	if( FIFO_IS_EMPTY( FIFO_TX ) ) {
		//если данных в fifo больше нет то запрещаем прерывание
		UCSR0B &= ~(1 << UDRIE0);
	}
	else{
		//иначе передаем следующий байт
		txbyte = FIFO_FRONT( FIFO_TX );
		FIFO_POP( FIFO_TX );
		UDR0 = txbyte;
	}
}

// TODO Нужно запрещать/разрешать только прерывания UART а не глобально
int uart_put_byte(char b, FILE *file ){
	int result;
	uint8_t time_10ms = timer_cnt_10ms;
	
	while (FIFO_IS_FULL(FIFO_TX)){}
		if (TIME_OFFSET_8(time_10ms, timer_cnt_10ms)>10)
			return -1;
			
	cli(); //запрещаем прерывания
	if( !FIFO_IS_FULL( FIFO_TX ) ) {
		FIFO_PUSH( (FIFO_TX), b );
		//и разрешаем прерывание по освобождению передатчика
		UCSR0B |= ( 1 << UDRIE0 );
		result = 0;
	}
	
	sei(); //разрешаем прерывания
		
	return result;
}

int uart_get_byte( FILE* file ){
	int result;
	
	cli(); //запрещаем прерывания
	if( !FIFO_IS_EMPTY( FIFO_RX ) ) {
		//если в буфере есть данные, то извлекаем их
		result = FIFO_FRONT( FIFO_RX );
		FIFO_POP( FIFO_RX );
	}
	else 
		result = _FDEV_EOF; //данных нет
	
	sei(); //разрешаем прерывания
	
	return result;
}


FILE uart_stream	= FDEV_SETUP_STREAM(uart_put_byte, uart_get_byte, _FDEV_SETUP_RW);

FILE * uart_s(){
	return &uart_stream;	
};

void init_uart(){
	stdout = &uart_stream;
	stdin = &uart_stream;
	
	UBRR0L = LO(BAUDDIVIDER);
	UBRR0H = HI(BAUDDIVIDER);
	UCSR0A = 0;
	UCSR0B = 1<<RXEN0|1<<TXEN0|1<<RXCIE0|0<<TXCIE0;
	UCSR0C = 1<<UCSZ00|1<<UCSZ01;
}


void transmit_uart(char * str_data){
		
	while (*str_data != 0){
		putchar(*str_data);
		str_data++;
	}
	
	puts("\r\n");
}