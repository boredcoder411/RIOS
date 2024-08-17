#include <stdbool.h>

typedef struct task
{
    unsigned char running;
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
    unsigned char priority;
    bool nokill;
} task;

extern task tasks[30];
extern const unsigned char tasksNum;
extern const unsigned long tasksPeriodGCD;
extern unsigned long priorityPeriods[5];
extern unsigned char runningTasks[4];
extern const unsigned long idleTask;
extern unsigned char currentTask;
extern unsigned schedule_time;

void addTaskWithPriority(int (*TickFct)(int), unsigned char priority);
