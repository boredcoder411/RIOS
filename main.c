#include "uart.h"
#include "defs.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

typedef struct task
{
    unsigned char running;     // 1 indicates task is running
    int state;                 // Current state of state machine
    unsigned long period;      // Rate at which the task should tick
    unsigned long elapsedTime; // Time since task's previous tick
    int (*TickFct)(int);       // Function to call for task's tick
    unsigned char priority;    // Priority level (1 to 5)
} task;

task tasks[3];

const unsigned char tasksNum = 3;
const unsigned long tasksPeriodGCD = 25;

unsigned long priorityPeriods[5] = {100, 80, 60, 40, 20}; // Periods based on priority

int TickFct_1(int state);
int TickFct_2(int state);
int TickFct_3(int state);
int printTask(int state);

unsigned char runningTasks[4] = {255}; // Track running tasks, [0] always idleTask
const unsigned long idleTask = 255;    // 0 highest priority, 255 lowest
unsigned char currentTask = 0;         // Index of highest priority task in runningTasks

unsigned schedule_time = 0;
ISR(TIMER1_COMPA_vect)
{
    unsigned char i;
    for (i = 0; i < tasksNum; ++i)
    {                                                 // Heart of scheduler code
        if ((tasks[i].elapsedTime >= tasks[i].period) // Task ready
            && (runningTasks[currentTask] > i)        // Task priority > current task priority
            && (!tasks[i].running)                    // Task not already running (no self-preemption)
        )
        {
            SREG &= 0x7F;
            tasks[i].elapsedTime = 0; // Reset time since last tick
            tasks[i].running = 1;     // Mark as running
            currentTask += 1;
            runningTasks[currentTask] = i; // Add to runningTasks
            SREG |= 0x80;

            tasks[i].state = tasks[i].TickFct(tasks[i].state); // Execute tick

            SREG &= 0x7F;
            tasks[i].running = 0;                 // Mark as not running
            runningTasks[currentTask] = idleTask; // Remove from runningTasks
            currentTask -= 1;
            SREG |= 0x80;
        }
        tasks[i].elapsedTime += tasksPeriodGCD;
    }
}

void init_processor()
{
    /*Set up SPI*/
    PORTB = 0xff;

    /*Set up timer*/
    TCCR1B = (1 << WGM12) | (1 << CS11); // CTC mode (clear timer on compare). Prescaler=8
    // AVR output compare register OCR0.
    OCR1A = 25000;
    // AVR timer interrupt mask register
    TIMSK1 = (1 << OCIE1A); // enables compare match interrupt
    TCNT1 = 0;

    /*Enable global interrupts*/
    SREG |= 0x80;

    /*Set up UART*/
    uart_init();
}

void addTaskWithPriority(int (*TickFct)(int), unsigned char priority)
{
    if (priority < 1 || priority > 5)
    {
        return; // Invalid priority
    }

    // Find the next available task slot
    unsigned char i;
    for (i = 0; i < tasksNum; i++)
    {
        if (tasks[i].TickFct == NULL)
        {
            tasks[i].state = -1;
            tasks[i].period = priorityPeriods[priority - 1]; // Assign period based on priority
            tasks[i].elapsedTime = tasks[i].period;
            tasks[i].running = 0;
            tasks[i].TickFct = TickFct;
            tasks[i].priority = priority;

            // Insert task based on priority
            unsigned char j = i;
            while (j > 0 && tasks[j - 1].priority < tasks[j].priority)
            {
                // Swap the tasks
                task temp = tasks[j - 1];
                tasks[j - 1] = tasks[j];
                tasks[j] = temp;
                j--;
            }

            break;
        }
    }
}

int main(void)
{
    init_processor();

    // Add tasks with different priorities
    //addTaskWithPriority(&TickFct_1, 3); // Medium priority task
    //addTaskWithPriority(&TickFct_2, 5); // Highest priority task
    //addTaskWithPriority(&TickFct_3, 2); // Low priority task
    addTaskWithPriority(&printTask, 1);

    while (1) {}
}

int TickFct_1(int state)
{
    _delay_us(5000);
    return 0;
}

int TickFct_2(int state)
{
    _delay_us(5000);
    return 0;
}

int TickFct_3(int state)
{
    _delay_us(25000);
    return 0;
}

int printTask(int state)
{
    uart_write("Task\r\n", strlen("Task\r\n"));
    _delay_ms(1000);
    return 0;
}