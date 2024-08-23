#include "UART.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

int count = 0;

ISR(TIMER1_COMPA_vect) {
    cli();
    char buffer[10];
    itoa(count, buffer, 10);
    for (int i = 0; buffer[i] != '\0'; i++) {
        uart_put_char(buffer[i]);
    }
    count++;
    sei();
}

void init_processor() {
    cli();
    /*Set up timer*/
    TCCR1B = (1<<WGM12)|(1<<CS11); // CTC mode (clear timer on compare). Prescaler=8
    // AVR output compare register OCR0.
    OCR1A = 25000;
    // AVR timer interrupt mask register         
    TIMSK1 = (1<<OCIE1A); //enables compare match interrupt
    TCNT1 = 0;

    uart_init();
    uart_put_char('H');

    sei(); // Enable global interrupts
}

int main() {
    //init_processor();
    uart_init();
    uart_put_char('H');
    while (1) {}
}