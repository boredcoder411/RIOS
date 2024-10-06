#include "defs.h"
#include <stdint.h>

struct UartRingBuffer
{
    uint8_t buffer[UartRingBufferSize];
    uint8_t head;
    uint8_t tail;
};

void uart_init();

void uart_put_char(char cData);

void uart_put_string(char *pData);

uint8_t uart_get_char(void);

uint8_t uart_ring_buffer_get(struct UartRingBuffer *pRingBuffer);

void uart_ring_buffer_put(struct UartRingBuffer *pRingBuffer, uint8_t ui8Data);

uint8_t uart_ring_buffer_full(const struct UartRingBuffer *pRingBuffer);

uint8_t uart_ring_buffer_empty(const struct UartRingBuffer *pRingBuffer);
