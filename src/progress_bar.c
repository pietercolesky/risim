/**
  * @file progress_bar.c
  * This is the implementation of the functions of progress_bar.h
  */

#include <stdio.h>
#include <stdlib.h>
#include "progress_bar.h"
#include <unistd.h>

/**
 * This function creates a progress bar struct.
 * RETURNS:
 *  - ProgressBar struct pointer
 * INPUTS:
 *  - barWidth
 *  - totalSteps
*/
struct ProgressBar* new_pb(size_t* barWidth, size_t* totalSteps) {
    struct ProgressBar* progress_bar = malloc(sizeof(struct ProgressBar));

    progress_bar->barWidth = malloc(sizeof(size_t));
    progress_bar->totalSteps = malloc(sizeof(size_t));
    progress_bar->progressRatio = malloc(sizeof(float));
    progress_bar->progress = malloc(sizeof(size_t));
    progress_bar->mutex = malloc(sizeof(pthread_mutex_t));

    *progress_bar->barWidth = *barWidth;
    *progress_bar->totalSteps = *totalSteps;
    *progress_bar->progressRatio = 0.0;
    *progress_bar->progress = 0;
    pthread_mutex_init(progress_bar->mutex, NULL);

    return progress_bar;
}

/**
 * This frees a ProgressBar struct
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - ProgressBar struct pointer
 *  - message - stop message
*/
void stop_free_pb(struct ProgressBar* progress_bar, char* message) {
    if (*progress_bar->progressRatio == 1) {
        printf("\nFINISHED: %s\n", message);
    } else {
        printf("\nERROR: %s\n", message);
    }

    if (progress_bar->barWidth != NULL) {
        free(progress_bar->barWidth);
    }
    if (progress_bar->totalSteps != NULL) {
        free(progress_bar->totalSteps);
    }
    if (progress_bar->progressRatio != NULL) {
        free(progress_bar->progressRatio);
    }
    if (progress_bar->mutex != NULL) {
        pthread_mutex_destroy(progress_bar->mutex);
        free(progress_bar->mutex);
    }
    if (progress_bar->progress != NULL) {
        free(progress_bar->progress);
    }
    if (progress_bar != NULL) {
        free(progress_bar);
    }
}

/**
 * This function will update the progress bar
 * RETURNS:
 *  - NONE
 * INPUT:
 *  -
*/
void update_pb(struct ProgressBar* progress_bar, size_t* progress) {
    if (*progress > *progress_bar->progress) {
        *progress_bar->progress = *progress;
        *progress_bar->progressRatio = (float)*progress / *progress_bar->totalSteps;
        // printf("%ld\n",*progress_bar->progressRatio);
        int barLength = *progress_bar->progressRatio * *progress_bar->barWidth;

        printf("[");
        for (int i = 0; i < *progress_bar->barWidth; ++i) {
            if (i < barLength) {
                printf("=");
            } else {
                printf(" ");
            }
        }
        printf("] %d%%\r", (int)(*progress_bar->progressRatio * 100));
        fflush(stdout);
    }
}

/**
 * This function will lock the progressbar when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct ProgressBar pointer
*/
void lock_pb(struct ProgressBar* progress_bar) {
    pthread_mutex_lock(progress_bar->mutex);
}

/**
 * This function will unlock the progressbar when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct ProgressBar pointer
*/
void unlock_pb(struct ProgressBar* progress_bar) {
    pthread_mutex_unlock(progress_bar->mutex);
}