/**
  * @file jagged_array.h
  * @description A definition of functions offered by the jagged_array
  */

#include <stdio.h>
#include <stdlib.h>
#include "array.h"

#ifndef _JAGGED_array_H
#define _JAGGED_array_H

/**
 * Jaggedarray struct
*/
struct Jaggedarray {
    size_t* size;
    struct array** jarray;
};

/**
 * Create new Jaggedarray
 * RETURNS:
 *  - Jaggedarray struct pointer
 * INPUTE:
 *  - size
*/
struct Jaggedarray* new_ja(size_t* size);

/**
 * free Jaggedarray
 * RETURNS:
 *  - None
 * INPUTE:
 *  - Jaggedarray Pointer
*/
void free_ja(struct Jaggedarray* jagged_array);

/**
 * set array in Jaggedarray
 * RETURNS:
 *  - None
 * INPUTE:
 *  - struct Jaggedarray pointer
 *  - struct array pointer
 *  - index
 * NOTE:
 *  - A copy of the array will be made.
*/
void set_ja(struct Jaggedarray* jagged_array, struct array* array, size_t* index);

/**
 * get array in Jaggedarray
 * RETURNS:
 *  - struct array pointer
 * INPUTE:
 *  - struct Jaggedarray pointer
 *  - index
 * NOTE:
 *  - A copy of the array will be made.
*/
struct array* get_ja(struct Jaggedarray* jagged_array, size_t* index);

/**
 * save Jaggedarray
 * RETURNS:
 *  - None
 * INPUTE:
 *  - Jaggedarray
 *  - directory
 *  - filename
*/
void save_to_file_ja(struct Jaggedarray* jagged_array, char* directory, char* filename);

/**
 * Read Jaggedarray
 * RETURNS:
 *  - Jaggedarray struct pointer
 * INPUTE:
 *  - filepath
*/
struct Jaggedarray* read_from_file_ja(char* filepath);

/**
 * create and save Jaggedarray
 * RETURNS:
 *  - None
 * INPUTE:
 *  - Jaggedarray
*/
void new_save_ja(struct Jaggedarray* jagged_array);

/**
 * print Jaggedarray
 * RETURNS:
 *  - None
 * INPUTE:
 *  - Jaggedarray
*/
void print_ja(struct Jaggedarray* jagged_array);

/**
 * Average of Jaggedarray
 * RETURNS:
 *  - average
 * INPUTE:
 *  - Jaggedarray
*/
void* average_ja(struct Jaggedarray* jagged_array);

/**
 * Norm of Jaggedarray
 * RETURNS:
 *  - norm
 * INPUTE:
 *  - Jaggedarray
*/
void* norm_ja(struct Jaggedarray* jagged_array);

#endif