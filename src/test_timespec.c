#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


int main(int argc, char* argv[])
{
    struct timespec ts;

    timespec_get(&ts, TIME_UTC);

    printf("%lld sec : %ld ns\n", ts.tv_sec, ts.tv_nsec);

    uint64_t sec = ((uint64_t)(ts.tv_sec)) * 1000;
    uint64_t ms = ((uint64_t)(ts.tv_nsec)) / 1000000;

    printf("%llu ms\n", sec + ms);

    return EXIT_SUCCESS;
}