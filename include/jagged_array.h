/**
  * @file jagged_array.h
  * @description A definition of functions offered by the jagged_array
  */

#include <stdio.h>
#include <stdlib.h>
#include "array.h"

#ifndef _JAGGED_ARRAY_H
#define _JAGGED_ARRAY_H

/**
 * JaggedArray struct
*/
struct JaggedArray {
    size_t* size;
    struct array** jarray;
};

/**
 * Create new JaggedArray
 * RETURNS:
 *  - JaggedArray struct pointer
 * INPUTE:
 *  - size
*/
struct JaggedArray* new_ja(size_t* size);

/**
 * free JaggedArray
 * RETURNS:
 *  - None
 * INPUTE:
 *  - JaggedArray Pointer
*/
void free_ja(struct JaggedArray* jagged_array);

/**
 * set array in JaggedArray
 * RETURNS:
 *  - None
 * INPUTE:
 *  - struct JaggedArray pointer
 *  - struct array pointer
 *  - index
 * NOTE:
 *  - A copy of the array will be made.
*/
void set_ja(struct JaggedArray* jagged_array, struct array* array, size_t* index);

/**
 * get array in JaggedArray
 * RETURNS:
 *  - struct array pointer
 * INPUTE:
 *  - struct JaggedArray pointer
 *  - index
 * NOTE:
 *  - A copy of the array will be made.
*/
struct array* get_ja(struct JaggedArray* jagged_array, size_t* index);

/**
 * save JaggedArray
 * RETURNS:
 *  - None
 * INPUTE:
 *  - JaggedArray
 *  - directory
 *  - filename
*/
void save_to_file_ja(struct JaggedArray* jagged_array, char* directory, char* filename);

/**
 * Read JaggedArray
 * RETURNS:
 *  - JaggedArray struct pointer
 * INPUTE:
 *  - filepath
*/
struct JaggedArray* read_from_file_ja(char* filepath);

/**
 * create and save JaggedArray
 * RETURNS:
 *  - None
 * INPUTE:
 *  - JaggedArray
*/
void new_save_ja(struct JaggedArray* jagged_array);

/**
 * print JaggedArray
 * RETURNS:
 *  - None
 * INPUTE:
 *  - JaggedArray
*/
void print_ja(struct JaggedArray* jagged_array);

/**
 * Average of JaggedArray
 * RETURNS:
 *  - average
 * INPUTE:
 *  - JaggedArray
*/
void* average_ja(struct JaggedArray* jagged_array);

/**
 * Norm of JaggedArray
 * RETURNS:
 *  - norm
 * INPUTE:
 *  - JaggedArray
*/
void* norm_ja(struct JaggedArray* jagged_array);

#endif