#include "../../defs.h"

#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>

char rx_buffer[64];
int rx_buffer_head = 0;

// Send byte to the UART
static void uart_putchar(char c) {
    while(!(UCSR0A & _BV(UDRE0)));
    UDR0 = c;
}

// Read a byte from the UART
char uart_getchar() {
    while(!(UCSR0A & _BV(RXC0)));
    return UDR0;
}

// Initialize the UART
void uart_init() {
    // Operation mode: Asynchronous USART
    UCSR0C &= ~_BV(UMSEL00) | ~_BV(UMSEL01);
    
    // Set baudrate
    UBRR0H = (uint8_t)((UBRR_VAL - 8) >> 8);
    UBRR0L = (uint8_t)(UBRR_VAL);

    // Set frame format: 8data, 1stop bit
    UCSR0C |= _BV(UCSZ00) | _BV(UCSZ01) | _BV(USBS0);
    
    // Enable transmitter and receiver
    UCSR0B |= _BV(TXEN0) | _BV(RXEN0);

    // Enable TX interrupt
    UCSR0B |= _BV(TXCIE0);

    // Enable RX interrupt
    UCSR0B |= _BV(RXCIE0);
}

// Write len bytes to the UART
int uart_write(char *buf, int len) {
    int i = 0;
    while(i < len) {
        uart_putchar(buf[i]);
        ++i;
    }
    return i;
}

// Read len bytes from the UART
int uart_read(char *buf, int len) {
    int i = 0;
    while(i < len) {
        buf[i] = uart_getchar();
        ++i;
    }
    return i;
}

// Check if there is data available to read
bool uart_available() {
    return (UCSR0A & _BV(RXC0));
}

char* uart_get_buffer() {
    return rx_buffer;
}

// Setup the interrupt for receiving data
ISR(USART_RX_vect)
{
    cli();
    rx_buffer[rx_buffer_head] = UDR0;
    rx_buffer_head = (rx_buffer_head + 1) % 64;
    sei();
}