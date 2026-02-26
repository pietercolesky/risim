/**
  * @file utils.h
  * @description A definition of functions offered by the utils
  */
#include "complex.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#ifndef _UTILS_H
#define _UTILS_H

/**
 * Enum to represent the data type
*/
enum DataType{
    DOUBLE,
    INT,
    FLOAT,
    COMPLEX
};

/**
 * Enum to represent layout
*/
enum LayoutType{
    HEX,
    SQR,
    REG,
    NONE
};

/**
 * Enum to represent an algorithm
*/
enum AlgorithmType {
    GRADIENT_DESENT,
    GRADIENT_DESENT_MOMENTUM,
    NESTROVS_ACCELERATED_GRADIENT,
    ADAGRAD,
    RMSPROP,
    ADAM,
    GAUSS_NEWTON,
    LEVENBERG_MARQUARDT,
    LEVENBERG_MARQUARDT_DIAGONAL_HESSIAN_LAMBDA,
    STEFCAL,
    STEFCAL_LAMBDA,
    STEFCAL_LM_LAMBDA,
    LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2,
    STEFCAL_IMPROVED,
    LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2_REDUCED_TIMESTEPS,
    LEVENBERG_MARQUARDT_REDUCED_TIMESTEPS,
};

/**
 * Format a complex value
*/
char* format_complex(double complex* z);

/**
 * Print a complex value
*/
void print_complex(double complex* z);

/**
 * Create a directory
*/
void create_directory(const char* path);

/**
 * Create all directories in path
*/
int create_directories(const char *path);

/**
 * string ends with
*/
int ends_with_string(char* str, char* suffix);


/**
 * Lock the file for exclusive access
 */
int lock_file(int fd);

/**
 * Unlock the file
 */
int unlock_file(int fd);

/**
 * This function extracts the filename from the filepath
*/
char* extract_filename_from_filepath(char* filepath);

/**
 * check if a filepath exists
*/
int check_file_path(char* filepath);

/**
 * compare doubles to the fifth decimal
*/
bool compare_doubles(double* a, double* b);

/**
 * This function will remove the file extention from a filepath
 */
char* remove_extension_from_filepath(char* filepath);

#endif