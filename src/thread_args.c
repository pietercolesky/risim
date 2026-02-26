/**
  * @file thread_args.c
  * This is the implementation of the functions of thread_args.h
  */

#include <stdio.h>
#include <stdlib.h>
#include "thread_args.h"

/**
 * Initialize ThreadArgs
 * RETURNS:
 *  - struct ThreadArgs pointer
 * INPUTS:
 *  - worker id
 *  - number of arguments
*/
struct ThreadArgs* init_ta(size_t* worker_id, size_t* num_args) {
    // Allocate memory for the ThreadArgs struct
    struct ThreadArgs* thread_args = malloc(sizeof(struct ThreadArgs));
    if (thread_args == NULL) {
        fprintf(stderr, "Memory allocation failed for ThreadArgs\n");
        return NULL;
    }

    // Allocate memory for worker_id and num_args
    thread_args->worker_id = malloc(sizeof(size_t));
    if (thread_args->worker_id == NULL) {
        fprintf(stderr, "Memory allocation failed for worker_id\n");
        free(thread_args); // Free previously allocated memory
        return NULL;
    }
    thread_args->num_args = malloc(sizeof(size_t));
    if (thread_args->num_args == NULL) {
        fprintf(stderr, "Memory allocation failed for num_args\n");
        free(thread_args->worker_id); // Free previously allocated memory
        free(thread_args); // Free previously allocated memory
        return NULL;
    }

    // Assign worker_id and num_args
    *(thread_args->worker_id) = *worker_id;
    *(thread_args->num_args) = *num_args;

    // Allocate memory for the args array
    thread_args->args = malloc((*num_args) * sizeof(void*));
    if (thread_args->args == NULL) {
        fprintf(stderr, "Memory allocation failed for args array\n");
        free(thread_args->num_args); // Free previously allocated memory
        free(thread_args->worker_id); // Free previously allocated memory
        free(thread_args); // Free previously allocated memory
        return NULL;
    }

    return thread_args;
}

/**
 * Free ThreadArgs
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct ThreadArgs pointer
 * NOTE:
 *  - This function only frees the argument array not the arguments itself.
*/
void free_ta(struct ThreadArgs* thread_args) {
    
    if (thread_args == NULL) {
        return; // Nothing to free
    }
    free(thread_args->worker_id);
    free(thread_args->num_args);
    // Free the args array
    free(thread_args->args);
    // Free the ThreadArgs struct itself
    free(thread_args);
}
