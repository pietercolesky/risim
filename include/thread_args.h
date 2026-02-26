/**
  * @file thread_args.h
  * @description A definition of functions offered by the thread_args
  */

#include <stdio.h>
#include <stdlib.h>

#ifndef _THREAD_ARGS_H
#define _THREAD_ARGS_H

/**
 * Stopwatch struct
*/
struct ThreadArgs {
    size_t* worker_id;
    void** args;
    size_t* num_args;
};

/**
 * Initialize ThreadArgs
 * RETURNS:
 *  - struct ThreadArgs pointer
 * INPUTS:
 *  - worker id
 *  - number of arguments
*/
struct ThreadArgs* init_ta(size_t* worker_id, size_t* num_args);

/**
 * Free ThreadArgs
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct ThreadArgs pointer
 * NOTE:
 *  - This function only frees the argument array not the arguments itself.
*/
void free_ta(struct ThreadArgs* thread_args);


#endif