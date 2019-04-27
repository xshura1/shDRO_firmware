/*
 * shared_memory.c
 *
 * Created: 22.02.2019 0:48:32
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 

#include <stdint.h>

#include "shared_memory.h"

void init_fifo(fifo_s * fifo, uint8_t * buf, uint8_t size){
	fifo->buf	= buf;
	fifo->size	= size;
	fifo->head	= 0;
	fifo->tail	= 0;
	
}

void init_shared_memory(void){
	init_fifo(&fifo_tx, (uint8_t*)&uart_buf[0], UART_TX_BUFFER_SIZE);
	init_fifo(&fifo_rx, (uint8_t*)&uart_buf[UART_TX_BUFFER_SIZE], UART_RX_BUFFER_SIZE);
	
	init_fifo(&fifo_ctx, (uint8_t*)&data_1_2[0], CONSOLE_TX_BUFFER_SIZE);
	init_fifo(&fifo_crx, (uint8_t*)&data_1_2[CONSOLE_TX_BUFFER_SIZE], CONSOLE_RX_BUFFER_SIZE);
	
	data_1 = &data_1_2[0];
	data_2 = &data_1_2[DATA_BUF_SIZE / 2];
	time_buf = (uint16_t *)&data_1_2[0];
}
