/**
  * @file array.h
  * @description A definition of functions offered by the array
  */

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <pthread.h>
#include <stdbool.h>

#ifndef _array_H
#define _array_H

/**
 * Generic array struct
*/
struct array {
    void* array;
    size_t* num_dimensions;
    size_t* dimensions;
    enum DataType* type;
    pthread_mutex_t* mutex;
};

/**
 * This function creates a new array with a specific type.  +
*/
struct array* new_a(size_t* num_dimensions, size_t* dimensions, enum DataType* type);

/**
 * This function creates a new array and then set with the preset values. +
*/
struct array* new_set_a(size_t* num_dimensions, size_t* dimensions, enum DataType* type, void* set_array);

/**
 * This function returns an element at the specific index int the array. +
*/
void* get_element_a(struct array* array, size_t* index);

/**
 * This function prints the array.
*/
void print_a(struct array* array);

/**
 * Getting the array  +
*/
void* get_a(struct array* array);

/**
 * Returning initialized array with 0's +
*/
struct array* zeros_a(size_t* num_dimensions, size_t* dimensions, enum DataType* type);

/**
 * Returning initialized array with 1's +
*/
struct array* ones_a(size_t* num_dimensions, size_t* dimensions, enum DataType* type);

/**
 * free all the elements in the array +
*/
void free_a(struct array* array);

/**
 * Element wise multiplication of two arrays  +
*/
struct array* multiply_a(struct array* arr1, struct array* arr2 );

/**
 * Element wise division of two arrays  +
*/
struct array* divide_a(struct array* arr1, struct array* arr2 );

/**
 * Set the value at a specific index in the array. +
 */
void set_element_a(struct array* array, size_t* indices, void* value);

/**
 * Element wise addition of two arrays  +
*/
struct array* addition_a(struct array* arr1, struct array* arr2 );

/**
 * Element wise multiplication of two arrays  +
*/
struct array* minus_a(struct array* arr1, struct array* arr2 );

/**
 * Returns 1D index from 2D +
*/
size_t get_2d_index_a(struct array* array, size_t* indices);

/**
 * Returns 1D index from 3D +
*/
size_t get_3d_index_a(struct array* array, size_t* indices);

/**
 * Get array size +
*/
size_t get_size_a(struct array* array);

/**
 * Returning an array with n amount of points evenly spaced between start and end +
*/
struct array* linspace_a(void* start, void* end, size_t* num_points, enum DataType* type);

/**
 * array multiplied by constant +
*/
struct array* multiply_constant_a(struct array* array, void* constant, enum DataType* constant_type);

/**
 * array divised by constant  +
*/
struct array* division_constant_a(struct array* array, void* constant, enum DataType* constant_type);

/**
 * array minus by constant +
*/
struct array* minus_constant_a(struct array* array, void* constant, enum DataType* constant_type);

/**
 * array addition by constant +
*/
struct array* addition_constant_a(struct array* array, void* constant, enum DataType* constant_type);

/**
 * Get the max value in an array. +
*/
void* max_a(struct array* array);

/**
 * Get the absolute max in an array.  +  
*/
void* amax_a(struct array* array);

/**
 * Get the min value in an array. +
*/
void* min_a(struct array* array);

/**
 * This function copies a array with specific dimensions to another array +
 * with different dimensions through slicing
*/
void copy_over_a(struct array* destination, struct array* source, int* des_slice, int* source_slice);

/**
 * Validating a dimention
*/
void validate_dimension_a(size_t dimension, const char* file, int line);

/**
 * This function gives the dot product of two arrays  +
 * with different dimensions through slicing
*/
struct array* dot_a(struct array* arr_1, struct array* arr_2);

/**
 * Generates a diagonal matrix from an 1d array +
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - array struct pointer
 *  - slice array 
*/
struct array* diag_a(struct array* array, int* slice);

/**
 * This will return a array struct pointer with the given slice parameters  +
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - array struct pointer
 *  - slice array 
*/
struct array* slice_a(struct array* array, int* slice);

/**
 * Computing the conjugate of an array  +
 * RETURNS:
 *  - Conjugate array struct pointer
 * INPUTS:
 *  - array struct pointer
*/
struct array* conjugate_a(struct array* array);

/**
 * Print shape of array
 * RETURNS:
 *  - NONE
 * INPUTS: 
 *  - array struct pointer
*/
void print_shape_a(struct array* array);

/**
 * Fill the diagonal of array with desired value +
 * RETURNS:
 *  - Diagonally filled array
 * INPUTS: 
 *  - array struct pointer
 *  - Value pointer
*/
void fill_diagonal_a(struct array* array, void* value);

/**
 * Calculates the norm of an array +
 * RETURNS:
 *  - norm value pointer
 * INPUTS: 
 *  - array struct pointer
 * NOTE:
 *  - Need to free the return value.
 *  - If Complex, return value is double.
*/
void* norm_a(struct array* array);

/**
 * Calculates the mean of an array.
 * RETURNS:
 *  - mean value pointer.
 * INPUTS: 
 *  - array struct pointer.
 * NOTE:
 *  - Need to free the return value.
 *  - If Complex, return value is double.
*/
void* mean_a(struct array* array);

/**
 * Calculates the standard deviation of an array.
 * RETURNS:
 *  - norm value pointer.
 * INPUTS: 
 *  - array struct pointer.
 * NOTE:
 *  - Need to free the return value.
 *  - If Complex, return value is double.
*/
void* std_a(struct array* array);

/**
 * This fuction does the outer product of two 1d arrays +
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - array struct pointer
*/
struct array* outer_a(struct array* array1, struct array* array2);

/**
 * Hadamard Inverse of a complex matrix  +
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - array struct pointer
*/
struct array* hadamard_inverse_a(struct array* array);

/**
 * Diagonal Hadamard Inverse of a complex matrix +
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - array struct pointer
*/
struct array* diag_hadamard_inverse_a(struct array* array);

/**
 * Computes the transpose of a matrix +
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
*/
struct array* transpose_a(struct array* array);

/**
 * Computes the hermitian transpose of a complex matrix
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
*/
struct array* hermitian_transpose_a(struct array* array);

/**
 * Save to file
 * RETURNS:
 *  -NONE
 * INPUTS:
 *  -NONE
*/
void save_to_file_a(struct array* array, char* directory, char* filename);

/**
 * Read .array file
*/
struct array* read_from_file_a(char* filepath);

/**
 * This function will lock the array when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct array pointer
*/
void lock_a(struct array* array);

/**
 * This function will unlock the array when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct array pointer
*/
void unlock_a(struct array* array);

/**
 * This function will check if two arrays are equal
 * RETURNS:
 *  - bool
 * INPUTS:
 *  - two struct array pointer
*/
bool equal_a(struct array* array1, struct array* array2);

/**
 * This function computes the reciprocal of an array
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
*/
struct array* reciprocal_a(struct array* array);

/**
 * This function computes the diagonal reciprocal of an array
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
*/
struct array* diag_reciprocal_a(struct array* array);

/**
 * This function computes the real part of an array
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
*/
struct array* real_a(struct array* array);

/**
 * This function computes imaginary part of an array
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
*/
struct array* imag_a(struct array* array);

/**
 * This function checks if a specific element is in an array.
 * RETURNS:
 *  - bool
 * INPUTS:
 *  - struct array pointer
*/
bool contains_element_a(struct array* array, void* element);

/**
 * This function checks if a specific element is in an array.
 * RETURNS:
 *  - index or -1 if not found
 * INPUTS:
 *  - struct array pointer
*/
int index_of_element_a(struct array* array, void* element);

/**
 * This function calculates the svd of a 2d array.
 * RETURNS:
 *  - struct array* U, s, Vh
 * INPUTS:
 *  - struct array pointer
*/
struct array** svd_a(struct array* array);

/**
 * This function calculates the pseudo inverse of a 2d array.
 * RETURNS:
 *  - struct array* array
 * INPUTS:
 *  - struct array pointer
*/
struct array* p_inv_a(struct array* array);

/**
 * This function calculates the absolute value of an array
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
 */
struct array* abs_a(struct array* array);

/**
 * This function calculates the power of an array
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
 *  - double power
 */
struct array* power_a(struct array* array, double power);

/**
 * This function calculates the square root of an array
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
 */
struct array* sqrt_a(struct array* array);

#endif