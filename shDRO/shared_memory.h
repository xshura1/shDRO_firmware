/*
 * shared_memory.h
 *
 * Created: 15.01.2019 14:19:56
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef SHARED_MEMORY_H_
#define SHARED_MEMORY_H_

#include "dro_types.h"
#include "messages.h"

#define DATA_BUF_SIZE			512
#define MEMORY_BUF_SIZE			256

#define CONSOLE_TX_BUFFER_SIZE	DATA_BUF_SIZE / 4
#define CONSOLE_RX_BUFFER_SIZE	DATA_BUF_SIZE / 4

#define UART_BUFFER_SIZE		64
#define UART_TX_BUFFER_SIZE		UART_BUFFER_SIZE / 2
#define UART_RX_BUFFER_SIZE		UART_BUFFER_SIZE / 2


typedef struct {
	unsigned char * buf;
	unsigned char size;
	unsigned char tail;
	unsigned char head;
} fifo_s;


uint8_t data_1_2[DATA_BUF_SIZE];

volatile uint8_t * data_1;
volatile uint8_t * data_2;
volatile uint8_t	pos_1;
volatile uint8_t	pos_2;

volatile uint16_t * time_buf;


volatile uint8_t uart_buf[UART_BUFFER_SIZE];

uint8_t memory_buf[MEMORY_BUF_SIZE];

fifo_s fifo_tx;
fifo_s fifo_rx;
fifo_s fifo_ctx;
fifo_s fifo_crx;

#define FIFO_TX (*((dro_config.is_cmd && !dro_config.is_logic_analyzer) ? &fifo_ctx : &fifo_tx))
#define FIFO_RX (*((dro_config.is_cmd && !dro_config.is_logic_analyzer) ? &fifo_crx : &fifo_rx))

void init_shared_memory(void);

//fifo заполнено?
#define FIFO_IS_FULL(fifo)   ((fifo.head-fifo.tail)==fifo.size)

//fifo пусто?
#define FIFO_IS_EMPTY(fifo)  (fifo.tail==fifo.head)

//количество элементов в очереди
#define FIFO_COUNT(fifo)     (fifo.head-fifo.tail)

//размер fifo
#define FIFO_SIZE(fifo)      ( fifo.size )

//количество свободного места в fifo
#define FIFO_SPACE(fifo)     (FIFO_SIZE(fifo)-FIFO_COUNT(fifo))

//поместить элемент в fifo
#define FIFO_PUSH(fifo, byte) \
{\
	fifo.buf[fifo.head & (fifo.size-1)]=byte;\
	fifo.head++;\
}

//взять первый элемент из fifo
#define FIFO_FRONT(fifo) (fifo.buf[(fifo).tail & (fifo.size-1)])

#define FIFO_FRONT_PTR(fifo) ((char*)(&fifo.buf[(fifo).tail & (fifo.size-1)]))

#define FIFO_PTR(fifo, offset) ((char*)(&fifo.buf[((fifo).tail + offset) & (fifo.size-1)]))

#define FIFO_HEAD_PTR(fifo) ((char*)(&fifo.buf[(fifo).head & (fifo.size-1)]))

#define FIFO_DATA(fifo, i) (fifo.buf[((fifo).tail + i) & (fifo.size-1)])

//уменьшить количество элементов в очереди
#define FIFO_POP(fifo)   \
{\
	fifo.tail++; \
}

//очистить fifo
#define FIFO_FLUSH(fifo)   \
{\
	fifo.tail=0;\
	fifo.head=0;\
}


#endif /* SHARED_MEMORY_H_ */
