#include "scheduler/tasks.h"
#include "drivers/uart/uart.h"
#include "defs.h"

#include <util/delay.h>
#include <avr/io.h>
#include <string.h>

int task1(int state);
int task2(int state);
int task3(int state);

void init_processor()
{
    /*Set up SPI*/
    PORTB = 0xff;
    
    /*Set up timer*/
    TCCR1B = (1<<WGM12)|(1<<CS11); // CTC mode (clear timer on compare). Prescaler=8
    // AVR output compare register OCR0.
    OCR1A = 25000;
    // AVR timer interrupt mask register         
    TIMSK1 = (1<<OCIE1A); //enables compare match interrupt
    TCNT1 = 0;
        
    /*Enable global interrupts*/
    SREG |= 0x80;

    uart_init();
}

int main(void)
{
    init_processor();

    //addTaskWithPriority(&task1, 3);
    //addTaskWithPriority(&task2, 5);
    //addTaskWithPriority(&task3, 1);

    while (1) {}
}

int task1(int state)
{
    uart_write("Task 1\n\r", 8);
    _delay_ms(500);
    return state;
}

int task2(int state)
{
    uart_write("Task 2\n\r", 8);
    if (uart_available()) {
        char received = uart_getchar();
        uart_write(&received, 1);
    }
    _delay_ms(500);
    return state;
}

int task3(int state)
{
    uart_write("Task 3\n\r", 8);
    _delay_ms(500);
    return state;
}
