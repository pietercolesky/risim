/**
  * @file utils.c
  * This is the implementation of the functions of utils.h
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "logger.h"
#include "complex.h"
#include <math.h>
#include "constants.h"

/**
 * Format a complex value
*/
char* format_complex(double complex* z) {
    static char result[50]; // Adjust the size based on your needs
    if (cimag(*z) == 0.0) {
      snprintf(result, sizeof(result), "%.8f%c%.8fi",
              creal(*z), '+', 0.0);
    } else {
      snprintf(result, sizeof(result), "%.8f%c%.8fi",
              creal(*z), cimag(*z) >= 0 ? '+' : '-', cimag(*z) >= 0 ? cimag(*z) : -cimag(*z));
    }
    return result;
}

/**
 * Print a complex value
*/
void print_complex(double complex* z){
    printf(" %s",format_complex(z));
}

/**
 * Create a directory
*/
void create_directory(const char* path) {
    if (mkdir(path, 0777) != 0 && errno != EEXIST) {
        perror("Error creating directory");
        exit(EXIT_FAILURE);
    }
}

/**
 * Create all directories in path
*/
int create_directories(const char *path) {
    char temp_path[1024];
    char *pos = NULL;

    // Copy the path to a temporary buffer
    strncpy(temp_path, path, sizeof(temp_path));
    temp_path[sizeof(temp_path) - 1] = '\0'; // Ensure null-termination

    // Iterate through each part of the path
    for (pos = temp_path + 1; *pos; pos++) {
        if (*pos == '/') {
            *pos = '\0'; // Temporarily terminate the string

            // Try to create the directory
            if (mkdir(temp_path, S_IRWXU) && errno != EEXIST) {
                perror("Error creating directory");
                return -1;
            }

            *pos = '/'; // Restore the slash
        }
    }

    // Create the final directory in the path
    if (mkdir(temp_path, S_IRWXU) && errno != EEXIST) {
        perror("Error creating directory");
        return -1;
    }

    return 0; // Success
}

/**
 * string ends with
*/
int ends_with_string(char* str, char* suffix) {
    size_t len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if (len < suffix_len) {
        return 0;
    }

    const char* suffix_pos = strstr(str + len - suffix_len, suffix);
    return suffix_pos != NULL;
}

/**
 * This function extracts the filename from the filepath
*/
char* extract_filename_from_filepath(char* filepath) {
    char* filename = malloc(256 * sizeof(char));
    
    // Find the last occurrence of '/'
    char *last_slash = strrchr(filepath, '/');
    if (last_slash == NULL) {
        // No '/' found, use the entire string
        last_slash = filepath;
    } else {
        // Move past the '/'
        last_slash++;
    }

    // Find the last occurrence of '.'
    char *last_dot = strrchr(last_slash, '.');
    if (last_dot == NULL) {
        // No '.' found, use the remaining string
        strcpy(filename, last_slash);
    } else {
        // Copy the substring between last_slash and last_dot
        size_t length = last_dot - last_slash;
        strncpy(filename, last_slash, length);
        filename[length] = '\0'; // Null-terminate the string
    }

    return filename;
}

/**
 * check if a filepath exists
*/
int check_file_path(char* filepath) {
    if (access(filepath, F_OK) == 0) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * compare doubles to the fifth decimal
*/
bool compare_doubles(double* a, double* b) {
    return fabs(*a - *b) < EPSILON;
}

/**
 * This function will remove the file extention from a filepath
 */
char* remove_extension_from_filepath(char* filepath) {
    // Allocate memory for the output string
    char* new_filepath = malloc((strlen(filepath) + 1) * sizeof(char));
    if (!new_filepath) {
        log_mem_aloc_fail(__FILE__, __LINE__);
    }

    // Find the last occurrence of '/'
    const char *last_slash = strrchr(filepath, '/');
    if (last_slash == NULL) {
        // No '/' found, treat the entire filepath as the filename
        last_slash = filepath;
    } else {
        // Move past the '/'
        last_slash++;
    }

    // Find the last occurrence of '.'
    const char *last_dot = strrchr(last_slash, '.');
    
    if (last_dot == NULL) {
        // No '.' found, copy the entire filepath
        strcpy(new_filepath, filepath);
    } else {
        // Copy up to but not including the last '.'
        size_t length = last_dot - filepath;
        strncpy(new_filepath, filepath, length);
        new_filepath[length] = '\0';
    }
    
    return new_filepath;
}