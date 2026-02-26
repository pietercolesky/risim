/**
  * @file stopwatch.c
  * This is the implementation of the functions of stopwatch.h
  */

#include <stdio.h>
#include <stdlib.h>
#include "stopwatch.h"
#include <sys/time.h>

/**
 * Create new stopwatch
 * RETURNS:
 *  - Stopwatch struct pointer
 * INPUTE:
 *  - NONE
*/
struct Stopwatch* new_sw() {
    struct Stopwatch* stopwatch = malloc(sizeof(struct Stopwatch));
    stopwatch->start_time = malloc(sizeof(struct timeval));
    stopwatch->lap_time = malloc(sizeof(struct timeval));
    
    gettimeofday(stopwatch->start_time, NULL);
    *(stopwatch->lap_time) = *(stopwatch->start_time);
    
    return stopwatch;
}

/**
 * Free the stopwatch
 * RETURNS:
 *  - Stopwatch struct pointer
 * INPUTE:
 *  - NONE
*/
void free_sw(struct Stopwatch* stopwatch) {
    if (stopwatch != NULL) {
        if (stopwatch->start_time != NULL) {
            free(stopwatch->start_time);
        }
        if (stopwatch->lap_time != NULL) {
            free(stopwatch->lap_time);
        }
        free(stopwatch);
    }
}

/**
 * Get laptime
 * RETURNS:
 *  - Stopwatch struct pointer
 * INPUTE:
 *  - double time elapsed
*/
double elapsed_sw(struct Stopwatch* stopwatch) {
    if (stopwatch == NULL || stopwatch->start_time == NULL || stopwatch->lap_time == NULL) {
        return -1.0;
    }
    
    gettimeofday(stopwatch->lap_time, NULL);
    double elapsed_time = (double)(stopwatch->lap_time->tv_sec - stopwatch->start_time->tv_sec) +
                         (double)(stopwatch->lap_time->tv_usec - stopwatch->start_time->tv_usec) / 1.0e6;
    return elapsed_time;
}


/**
 * Print laptime
 * RETURNS:
 *  - NONE
 * INPUTE:
 *  - NONE
*/
void print_sw(struct Stopwatch* stopwatch) {
    double elapsed_time = elapsed_sw(stopwatch);
    printf("Time elapsed: %.6f seconds\n", elapsed_time);
}
