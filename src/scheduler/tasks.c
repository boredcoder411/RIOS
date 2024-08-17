#include "tasks.h"
#include "../drivers/uart/uart.h"
#include "../defs.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

task tasks[30];
const unsigned char tasksNum = 3;
const unsigned long tasksPeriodGCD = 25;
unsigned long priorityPeriods[5] = {100, 80, 60, 40, 20};

unsigned char runningTasks[4] = {255};
const unsigned long idleTask = 255;
unsigned char currentTask = 0;
unsigned schedule_time = 0;

ISR(TIMER1_COMPA_vect)
{
    unsigned char i;
    char buffer[10];
    char buffer2[10];

    for (i = 0; i < tasksNum; ++i)
    {                                                 
        if ((tasks[i].elapsedTime >= tasks[i].period)
            && (runningTasks[currentTask] > i)
            && (!tasks[i].running))
        {
            SREG &= 0x7F;
            tasks[i].elapsedTime = 0;
            tasks[i].running = 1;
            currentTask += 1;
            runningTasks[currentTask] = i;
            SREG |= 0x80;

            int returnValue = tasks[i].TickFct(tasks[i].state);

            itoa(returnValue, buffer, 10);
            //uart_write(buffer, strlen(buffer));
            //uart_write("\n\r", 2);

            itoa(i, buffer2, 10);
            //uart_write(buffer2, strlen(buffer2));
            //uart_write("\n\r", 2);

            SREG &= 0x7F;
            tasks[i].running = 0;
            runningTasks[currentTask] = idleTask;
            currentTask -= 1;
            SREG |= 0x80;
        }
        tasks[i].elapsedTime += tasksPeriodGCD;
    }
}

void addTaskWithPriority(int (*TickFct)(int), unsigned char priority)
{
    if (priority < 1 || priority > 5)
    {
        return;
    }

    unsigned char i;
    for (i = 0; i < tasksNum; i++)
    {
        if (tasks[i].TickFct == NULL)
        {
            tasks[i].state = -1;
            tasks[i].period = priorityPeriods[priority - 1];
            tasks[i].elapsedTime = tasks[i].period;
            tasks[i].running = 0;
            tasks[i].TickFct = TickFct;
            tasks[i].priority = priority;

            unsigned char j = i;
            while (j > 0 && tasks[j - 1].priority < tasks[j].priority)
            {
                task temp = tasks[j - 1];
                tasks[j - 1] = tasks[j];
                tasks[j] = temp;
                j--;
            }

            break;
        }
    }
}