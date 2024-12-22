#ifndef TIMER_H
#define TIMER_H

#ifndef _CRT_SECURE_NO_WARNINGS
#   define _CRT_SECURE_NO_WARNINGS
#endif 

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "CustomTypes.h"


struct Timer
{
    char* name;
    clock_t start;
    clock_t stop;
};
typedef struct Timer Timer;


Timer* initTimer(const char* name)
{
    // Allocate the limited lifetime timer object.
    Timer* timer = (Timer *)malloc(sizeof(Timer));
    
    // Give a name to the timer's recorded process.
    timer->name = strBuild(name);

    // Record the start moment.
    timer->start = clock();

    // Instantiate the object.
    return timer;
}

void endTimer(Timer* timer)
{
    uint64_t duration_ms;
    uint64_t duration_sec;
    uint64_t duration_min;
    uint64_t duration_h;

    // Record the finishing moment.
    timer->stop = clock();

    // Calculate process elapsed time in milliseconds.
    duration_ms = (uint64_t)(1000.0 * (double)(timer->stop - timer->start) / CLOCKS_PER_SEC);

    duration_sec = duration_ms / 1000;
    duration_ms = duration_ms % 1000;

    duration_min = duration_sec / 60;
    duration_sec = duration_sec % 60;

    duration_h = duration_min / 60;
    duration_min = duration_min % 60;

    // Convert and print the elpased time as milliseconds.
    printf(
        "Process \"%s\" finished after %02zuh : %02zumin : %02zusec : %03zums\n", 
        timer->name, 
        duration_h,
        duration_min,
        duration_sec,
        duration_ms
    );

    free(timer->name);
    free(timer);
}

#endif // TIMER_H