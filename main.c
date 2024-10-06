#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

typedef struct Task {
   unsigned char running; // 1 indicates task is running
   int state;             // Current state of state machine
   unsigned long period; // Rate at which the task should tick
   unsigned long elapsedTime; // Time since task's previous tick
   int (*TickFct)(int); // Function to call for task's tick
} Task;

Task tasks[3];

ISR(TIMER1_COMPA_vect) {
    cli();
    uart_put_char('I');
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

    sei(); // Enable global interrupts
}

int main() {
    init_processor();
    uart_init();
    uart_put_char('H');
    while (1) {}
}