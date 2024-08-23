#include "UART.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static struct UartRingBuffer UART_RING_BUFFER = {.head = 0, .tail = 0};

static void uart_tx_start(void) {
    uint8_t data;

    data = uart_ring_buffer_get(&UART_RING_BUFFER);

    while (!( UCSR0A & (1<<UDRE0)));

    UDR0 = data;
}

void uart_init() {
    UCSR0C &= ~_BV(UMSEL00) | ~_BV(UMSEL01);
    
    UBRR0H = (uint8_t)((UBRR - 8) >> 8);
    UBRR0L = (uint8_t)(UBRR);

    UCSR0C |= _BV(UCSZ00) | _BV(UCSZ01) | _BV(USBS0);
    
    UCSR0B |= _BV(TXEN0) | _BV(RXEN0);

    UCSR0B |= _BV(TXCIE0);

    UCSR0B |= _BV(RXCIE0);
}

void uart_put_char(char data) {
    const uint8_t is_tx_ongoing = !uart_ring_buffer_empty(&UART_RING_BUFFER);

    uart_ring_buffer_put(&UART_RING_BUFFER, data);

    if (!is_tx_ongoing) {
        uart_tx_start();
    }
}

void uart_put_string(char *data) {
    while (*data != '\0') {
        uart_put_char(*data);
        data++;
    }
}

uint8_t uart_get_char(void) {
    while (!(UCSR0A & (1<<RXC0)));

    return UDR0;
}

uint8_t uart_ring_buffer_get(struct UartRingBuffer *ring_buffer) {
    const uint8_t data = ring_buffer->buffer[ring_buffer->tail];
    ring_buffer->tail++;

    if (ring_buffer->tail == UartRingBufferSize) {
        ring_buffer->tail = 0;
    }

    return data;
}

void uart_ring_buffer_put(struct UartRingBuffer *ring_buffer, uint8_t data) {
    ring_buffer->buffer[ring_buffer->head] = data;
    ring_buffer->head++;

    if (ring_buffer->head == UartRingBufferSize) {
        ring_buffer->head = 0;
    }
}

uint8_t uart_ring_buffer_full(const struct UartRingBuffer *ring_buffer) {
    uint8_t is_ring_buffer_full;

    uint8_t index_after_head = ring_buffer->head;

    if (index_after_head == UartRingBufferSize) {
        index_after_head = 0;
    }

    is_ring_buffer_full = index_after_head == ring_buffer->tail;

    return is_ring_buffer_full;
}

uint8_t uart_ring_buffer_empty(const struct UartRingBuffer *ring_buffer) {
    uint8_t is_ring_buffer_empty;

    is_ring_buffer_empty = ring_buffer->head == ring_buffer->tail;

    return is_ring_buffer_empty;
}

ISR(USART_RX_vect) {
    uint8_t data = UDR0;
    uart_ring_buffer_put(&UART_RING_BUFFER, data);
}

ISR(USART_TX_vect) {
    // Empty ISR for TX complete, can be filled if needed
}
