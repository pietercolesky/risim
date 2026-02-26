/**
  * @file progress_bar.h
  * @description A definition of functions offered by the progress_bar
  */

#include <stdio.h>
#include <pthread.h>

#ifndef _PROGRESS_BAR_H
#define _PROGRESS_BAR_H

/**
 *ProgressBar struct
*/
struct ProgressBar {
    size_t* barWidth;
    size_t* totalSteps;
    float* progressRatio;
    size_t* progress;
    pthread_mutex_t* mutex;

};

/**
 * This function creates a progress bar struct.
 * RETURNS:
 *  - ProgressBar struct pointer
 * INPUTS:
 *  - barWidth
 *  - totalSteps
*/
struct ProgressBar* new_pb(size_t* barWidth, size_t* totalSteps);

/**
 * This frees a ProgressBar struct
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - ProgressBar struct pointer
 *  - message - stop message
*/
void stop_free_pb(struct ProgressBar* progress_bar, char* message);

/**
 * This function will update the progress bar
 * RETURNS:
 *  - NONE
 * INPUT:
 *  -
*/
void update_pb(struct ProgressBar* progress_bar, size_t* progress);

/**
 * This function will lock the progressbar when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct ProgressBar pointer
*/
void lock_pb(struct ProgressBar* progress_bar);

/**
 * This function will unlock the progressbar when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct ProgressBar pointer
*/
void unlock_pb(struct ProgressBar* progress_bar);



#endif