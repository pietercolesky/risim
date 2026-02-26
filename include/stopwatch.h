/**
  * @file stopwatch.h
  * @description A definition of functions offered by the stopwatch
  */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#ifndef _STOPWATCH_H
#define _STOPWATCH_H

/**
 * Stopwatch struct
*/
struct Stopwatch {
    struct timeval* start_time;
    struct timeval* lap_time;
};

/**
 * Create new stopwatch
 * RETURNS:
 *  - Stopwatch struct pointer
 * INPUTE:
 *  - NONE
*/
struct Stopwatch* new_sw();

/**
 * Free the stopwatch
 * RETURNS:
 *  - Stopwatch struct pointer
 * INPUTE:
 *  - NONE
*/
void free_sw(struct Stopwatch* stopwatch);

/**
 * Get elapsed time
 * RETURNS:
 *  - Stopwatch struct pointer
 * INPUTE:
 *  - double time elapsed
*/
double elapsed_sw(struct Stopwatch* stopwatch);

/**
 * Print laptime
 * RETURNS:
 *  - NONE
 * INPUTE:
 *  - NONE
*/
void print_sw(struct Stopwatch* stopwatch);

#endif