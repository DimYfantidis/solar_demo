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
#include <inttypes.h>

#include "CustomTypes.h"


struct Timer
{
    char* name;
    uint64_t start;
    uint64_t stop;
};
typedef struct Timer Timer;


uint64_t getAbsoluteTimeMillis()
{
    struct timespec ts;

    timespec_get(&ts, TIME_UTC);

    uint64_t sec = ((uint64_t)(ts.tv_sec)) * 1000;
    uint64_t ms = ((uint64_t)(ts.tv_nsec)) / 1000000;

    return sec + ms;
}


Timer* initTimer(const char* name)
{
    // Allocate the limited lifetime timer object.
    Timer* timer = (Timer *)malloc(sizeof(Timer));
    
    // Give a name to the timer's recorded process.
    timer->name = strBuild(name);

    // Record the start moment.
    timer->start = getAbsoluteTimeMillis();

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
    timer->stop = getAbsoluteTimeMillis();

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

void getTimeFormatStringFromMillis(char* buffer, size_t buffer_size, uint64_t duration_ms)
{
    uint64_t duration_sec;
    uint64_t duration_min;
    uint64_t duration_h;
    uint64_t duration_days;
    uint64_t duration_years;

    duration_sec = duration_ms / 1000;
    duration_ms = duration_ms % 1000;

    duration_min = duration_sec / 60;
    duration_sec = duration_sec % 60;

    duration_h = duration_min / 60;
    duration_min = duration_min % 60;
    
    duration_days = duration_h / 24;
    duration_h = duration_h % 24;

    duration_years = duration_days / 365;
    duration_days = duration_days % 365;

    snprintf(
        buffer, buffer_size, "(%"PRIu64"Y: %03"PRIu64"D : %02"PRIu64"h : %02"PRIu64"min : %02"PRIu64"sec : %03"PRIu64"ms)",
        duration_years, duration_days, duration_h, duration_min, duration_sec, duration_ms
    );
}

#endif // TIMER_H