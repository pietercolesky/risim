/**
  * @file array.c
  * This is the implementation of the functions of array.h
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "logger.h"
#include "array.h"
#include "complex.h"
#include "utils.h"
#include <math.h>
#include <stdbool.h>
#include "constants.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include <lapacke.h>

/**
 * This function creates a new array with a specific type.
*/
struct array* new_a(size_t* num_dimensions, size_t* dimensions, enum DataType* type){
    struct array* array = malloc(sizeof(struct array));

    size_t elementSize = 0;

    switch (*type) {
      case DOUBLE:
        elementSize = sizeof(double);
        break;
      case INT:
        elementSize = sizeof(int);
        break;
      case FLOAT:
        elementSize = sizeof(float);
        break;
      case COMPLEX:
        elementSize = sizeof(double complex);
        break;
      default:
        log_unsupported_type(__FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    //Allocating for the specific array
    switch (*num_dimensions){
      case 1:{
        size_t* dim = malloc(*num_dimensions * sizeof(size_t));
        dim[0] = dimensions[0];
        validate_dimension_a(dim[0], __FILE__, __LINE__);
        array->dimensions = dim;
        array->array = malloc(dim[0] * elementSize);
        break;
      }
      case 2:{
        size_t* dim = malloc(*num_dimensions * sizeof(size_t));
        dim[0] = dimensions[0];
        validate_dimension_a(dim[0], __FILE__, __LINE__);
        dim[1] = dimensions[1];
        validate_dimension_a(dim[1], __FILE__, __LINE__);
        array->dimensions = dim;
        array->array = malloc(dimensions[0] * dimensions[1] * elementSize);
        break;
      }
      case 3:{
        size_t* dim = malloc(*num_dimensions * sizeof(size_t));
        dim[0] = dimensions[0];
        validate_dimension_a(dim[0], __FILE__, __LINE__);
        dim[1] = dimensions[1];
        validate_dimension_a(dim[1], __FILE__, __LINE__);
        dim[2] = dimensions[2];
        validate_dimension_a(dim[2], __FILE__, __LINE__);
        array->dimensions = dim;
        array->array = malloc(dimensions[0] * dimensions[1] * dimensions[2] * elementSize);
        break;
      }
      default:
        log_invalid_dimensions(__FILE__, __LINE__);
        break;
    }

    if (array->array == NULL) {
        log_mem_aloc_fail(__FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    array->num_dimensions = malloc(sizeof(size_t));
    *array->num_dimensions = *num_dimensions;
    array->type = malloc(sizeof(enum DataType));
    *array->type = *type;
    array->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(array->mutex, NULL);

    
    return array;
}

/**
 * This function creates a new array and then set with the preset values.
*/
struct array* new_set_a(size_t* num_dimensions, size_t* dimensions, enum DataType* type, void* set_array) {
    struct array* array = new_a(num_dimensions, dimensions, type);

    switch(*num_dimensions) {
        case 1: {
            switch (*array->type) {
                case DOUBLE: {
                    double* set_array_double = (double*)set_array;
                    for (size_t i = 0; i < array->dimensions[0]; ++i) {
                        ((double*)array->array)[i] = set_array_double[i];
                    }
                    break;
                }
                case INT: {
                    int* set_array_int = (int*)set_array;
                    for (size_t i = 0; i < array->dimensions[0]; ++i) {
                        ((int*)array->array)[i] = set_array_int[i];
                    }
                    break;
                }
                case FLOAT: {
                    float* set_array_double = (float*)set_array;
                    for (size_t i = 0; i < array->dimensions[0]; ++i) {
                        ((float*)array->array)[i] = set_array_double[i];
                    }
                    break;
                }
                case COMPLEX: {
                    double complex* set_array_double = (double complex*)set_array;
                    for (size_t i = 0; i < array->dimensions[0]; ++i) {
                        ((double complex*)array->array)[i] = set_array_double[i];
                    }
                    break;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            break;
        }
        case 2: {
            switch (*array->type) {
                case DOUBLE: {
                    double (*set_array_double)[dimensions[1]] = (double(*)[dimensions[1]])set_array;
                    for (size_t i = 0; i < array->dimensions[0]; ++i) {
                        for (size_t j = 0; j < array->dimensions[1]; ++j) {
                            set_element_a(array, (size_t[]){i,j}, &set_array_double[i][j]);
                        }
                    }
                    break;
                }
                case INT: {
                    int (*set_array_int)[dimensions[1]] = (int(*)[dimensions[1]])set_array;
                    for (size_t i = 0; i < array->dimensions[0]; ++i) {
                        for (size_t j = 0; j < array->dimensions[1]; ++j) {
                            set_element_a(array, (size_t[]){i,j}, &set_array_int[i][j]);
                        }
                    }
                    break;
                }
                case FLOAT: {
                    float (*set_array_float)[dimensions[1]] = (float(*)[dimensions[1]])set_array;
                    for (size_t i = 0; i < array->dimensions[0]; ++i) {
                        for (size_t j = 0; j < array->dimensions[1]; ++j) {
                            set_element_a(array, (size_t[]){i,j}, &set_array_float[i][j]);
                        }
                    }
                    break;
                }
                case COMPLEX: {
                    double complex (*set_array_complex)[dimensions[1]] = (complex(*)[dimensions[1]])set_array;
                    for (size_t i = 0; i < array->dimensions[0]; ++i) {
                        for (size_t j = 0; j < array->dimensions[1]; ++j) {
                            set_element_a(array, (size_t[]){i,j}, &set_array_complex[i][j]);
                        }
                    }
                    break;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            break;
        }
        case 3: {
            switch (*array->type) {
                case DOUBLE: {
                    double (*set_array_double)[dimensions[1]][dimensions[2]] = (double(*)[dimensions[1]][dimensions[2]])set_array;
                    for (size_t i = 0; i < array->dimensions[0]; ++i) {
                        for (size_t j = 0; j < array->dimensions[1]; ++j) {
                            for (size_t k = 0; k < array->dimensions[2]; ++k) {
                                set_element_a(array, (size_t[]){i,j,k}, &set_array_double[i][j][k]);
                            }
                        }
                    }
                    break;
                }
                case INT: {
                    int (*set_array_int)[dimensions[1]][dimensions[2]] = (int(*)[dimensions[1]][dimensions[2]])set_array;
                    for (size_t i = 0; i < array->dimensions[0]; ++i) {
                        for (size_t j = 0; j < array->dimensions[1]; ++j) {
                            for (size_t k = 0; k < array->dimensions[2]; ++k) {
                                set_element_a(array, (size_t[]){i,j,k}, &set_array_int[i][j][k]);
                            }
                        }
                    }
                    break;
                }
                case FLOAT: {
                    float (*set_array_float)[dimensions[1]][dimensions[2]] = (float(*)[dimensions[1]][dimensions[2]])set_array;
                    for (size_t i = 0; i < array->dimensions[0]; ++i) {
                        for (size_t j = 0; j < array->dimensions[1]; ++j) {
                            for (size_t k = 0; k < array->dimensions[2]; ++k) {
                                set_element_a(array, (size_t[]){i,j,k}, &set_array_float[i][j][k]);
                            }
                        }
                    }
                    break;
                }
                case COMPLEX: {
                    double complex (*set_array_complex)[dimensions[1]][dimensions[2]] = (double complex(*)[dimensions[1]][dimensions[2]])set_array;
                    for (size_t i = 0; i < array->dimensions[0]; ++i) {
                        for (size_t j = 0; j < array->dimensions[1]; ++j) {
                            for (size_t k = 0; k < array->dimensions[2]; ++k) {
                                set_element_a(array, (size_t[]){i,j,k}, &set_array_complex[i][j][k]);
                            }
                        }
                    }
                    break;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            break;
        }
        default: {

        }
    }
    
    return array;
}

/**
 * This function returns an item at the specific index int the array.
*/
void* get_element_a(struct array* array, size_t* indices){
    switch (*array->num_dimensions){
      case 1:{
        switch (*array->type){
          case DOUBLE:{
            double* arr_int = (double*)get_a(array);
            return (void*)&arr_int[indices[0]];
          }
          case INT:{
            int* arr_int = (int*)get_a(array);
            return (void*)&arr_int[indices[0]];
          } 
          case FLOAT:{
            float* arr_float = (float*)get_a(array);
            return (void*)&arr_float[indices[0]];
          }
          case COMPLEX:{
            double complex* arr_complex = (double complex*)get_a(array);
            return (void*)&arr_complex[indices[0]];
          }   
          default:
            log_incompatible_types(__FILE__, __LINE__);
            break;
        }
        break;
      }
      case 2:{
        switch (*array->type){
          case DOUBLE:{
            double* arr_double = (double*)get_a(array);
            size_t index = get_2d_index_a(array, indices);
            return (void*)&arr_double[index];
          }
          case INT:{
            int* arr_int = (int*)get_a(array);
            size_t index = get_2d_index_a(array, indices);
            return (void*)&arr_int[index];
          } 
          case FLOAT:{
            float* arr_float = (float*)get_a(array);
            size_t index = get_2d_index_a(array, indices);
            return (void*)&arr_float[index];
          }
          case COMPLEX:{
            double complex* arr_complex = (double complex*)get_a(array);
            size_t index = get_2d_index_a(array, indices);
            return (void*)&arr_complex[index];
          }   
          default:
            log_incompatible_types(__FILE__, __LINE__);
            break;
        }
        break;
      }
      case 3:{
        switch (*array->type){
          case DOUBLE:{
          double* arr_double = (double*)get_a(array);
            size_t index = get_3d_index_a(array, indices);
            return (void*)&arr_double[index];
          }
          case INT:{
            int* arr_int = (int*)get_a(array);
            size_t index = get_3d_index_a(array, indices);
            return (void*)&arr_int[index];
          }
          case FLOAT:{
            float* arr_float = (float*)get_a(array);
            size_t index = get_3d_index_a(array, indices);
            return (void*)&arr_float[index];
          }
          case COMPLEX:{
            double complex* arr_complex = (double complex*)get_a(array);
            size_t index = get_3d_index_a(array, indices);
            return (void*)&arr_complex[index];
          }  
          default:
            log_incompatible_types(__FILE__, __LINE__);
            break;
        }
        break;
      }
      default:
        log_invalid_dimensions(__FILE__, __LINE__);
        break;
    }

    return NULL;
}

/**
 * Getting the array
*/
void* get_a(struct array* array) {
    switch (*array->type){
        case DOUBLE:{
            return array->array;
        }
        case INT:{
            return array->array;
        }
        case FLOAT:{
            return array->array;
        }
        case COMPLEX:{
            return array->array;
        }
        default:{
            log_unsupported_type(__FILE__, __LINE__);
            return NULL;
        }
    }
}

/**
 * This function prints the array.
*/
void print_a(struct array* array){
    switch (*array->num_dimensions){
      case 1:{
        switch (*array->type){
          case DOUBLE:{
            size_t dim1 = array->dimensions[0];
            double* arr_double = (double*)get_a(array);
            printf("[ ");
            for (size_t i = 0; i < dim1; i++) {
                printf("%lf ", arr_double[i]);
            }
            printf("]\n");
            break;
          }
          case INT:{
            int* arr_int = (int*)get_a(array);
            printf("[ ");
            for (size_t i = 0; i < array->dimensions[0]; i++) {
                printf("%d ", arr_int[i]);
            }
            printf("]\n");
            break;
          } 
          case FLOAT:{
            float* arr_float = (float*)get_a(array);
            printf("[ ");
            for (size_t i = 0; i < array->dimensions[0]; i++) {
                printf("%f ", arr_float[i]);
            }
            printf("]\n");
            break;
          }
          case COMPLEX:{
            double complex* arr_complex = (double complex*)get_a(array);
            printf("[ ");
            for (size_t i = 0; i < array->dimensions[0]; i++) {
                print_complex(&arr_complex[i]);
            }
            printf(" ]\n");
            break;
          }   
          default:
            log_incompatible_types(__FILE__, __LINE__);
            break;
        }
        break;
      }
      case 2:{
        switch (*array->type){
          case DOUBLE:{
            double* data = array->array;
            printf("[\n");
            for (size_t i = 0; i < array->dimensions[0]; i++){
                printf("  [");
                for (size_t j = 0; j < array->dimensions[1]; j++){
                  size_t index = get_2d_index_a(array, (size_t[]){i,j});
                  printf("%f ",data[index]);
                }
                printf("]\n");
            }
            printf("]\n");
            break;
          }
          case INT:{
            int* data = array->array;
            printf("[\n");
            for (size_t i = 0; i < array->dimensions[0]; i++){
                printf("  [");
                for (size_t j = 0; j < array->dimensions[1]; j++){
                  size_t index = get_2d_index_a(array, (size_t[]){i,j});
                  printf("%d ",data[index]);
                }
                printf("]\n");
            }
            printf("]\n");
            break;
          } 
          case FLOAT:{
            float* data = array->array;
            printf("[\n");
            for (size_t i = 0; i < array->dimensions[0]; i++){
                printf("  [");
                for (size_t j = 0; j < array->dimensions[1]; j++){
                  size_t index = get_2d_index_a(array, (size_t[]){i,j});
                  printf("%f ",data[index]);
                }
                printf("]\n");
            }
            printf("]\n");
            break;
          }
          case COMPLEX:{
            double complex* data = array->array;
            printf("[\n");
            for (size_t i = 0; i < array->dimensions[0]; i++){
                printf("  [");
                for (size_t j = 0; j < array->dimensions[1]; j++){
                  size_t index = get_2d_index_a(array, (size_t[]){i,j});
                  print_complex(&data[index]);
                }
                printf("]\n");
            }
            printf("]\n");
            break;
          }   
          default:
            log_incompatible_types(__FILE__, __LINE__);
            break;
        }
        break;
      }
      case 3:{
        switch (*array->type){
          case DOUBLE:{
              double* data = array->array;
              printf("[\n");
              for (size_t i = 0; i < array->dimensions[0]; i++){
                  printf("  [\n");
                  for (size_t j = 0; j < array->dimensions[1]; j++){
                      printf("    [");
                      for (size_t k = 0; k < array->dimensions[2]; k++){
                          size_t index = get_3d_index_a(array, (size_t[]){i,j,k});
                          printf("%f ",data[index]);
                      }
                      printf("]\n");
                  }
                  printf("  ]\n");
              }
              printf("]\n");
              break;
          }
          case INT:{
              int* data = array->array;
              printf("[\n");
              for (size_t i = 0; i < array->dimensions[0]; i++){
                  printf("  [\n");
                  for (size_t j = 0; j < array->dimensions[1]; j++){
                      printf("    [");
                      for (size_t k = 0; k < array->dimensions[2]; k++){
                          size_t index = get_3d_index_a(array, (size_t[]){i,j,k});
                          printf("%d ",data[index]);
                      }
                      printf("]\n");
                  }
                  printf("  ]\n");
              }
              printf("]\n");
              break;
          }
          case FLOAT:{
              float* data = array->array;
              printf("[\n");
              for (size_t i = 0; i < array->dimensions[0]; i++){
                  printf("  [\n");
                  for (size_t j = 0; j < array->dimensions[1]; j++){
                      printf("    [");
                      for (size_t k = 0; k < array->dimensions[2]; k++){
                          size_t index = get_3d_index_a(array, (size_t[]){i,j,k});
                          printf("%f ",data[index]);
                      }
                      printf("]\n");
                  }
                  printf("  ]\n");
              }
              printf("]\n");
              break;
          }
          case COMPLEX:{
              double complex* data = array->array;
              printf("[\n");
              for (size_t i = 0; i < array->dimensions[0]; i++){
                  printf("  [\n");
                  for (size_t j = 0; j < array->dimensions[1]; j++){
                      printf("    [");
                      for (size_t k = 0; k < array->dimensions[2]; k++){
                          size_t index = get_3d_index_a(array, (size_t[]){i,j,k});
                          print_complex(&data[index]);
                      }
                      printf("]\n");
                  }
                  printf("  ]\n");
              }
              printf("]\n");
              break;
          }  
          default:
            log_incompatible_types(__FILE__, __LINE__);
            break;
        }
        break;
      }
      default:
        log_invalid_dimensions(__FILE__, __LINE__);
        break;
    }
}

/**
 * Returning initialized array with 0's
*/
struct array* zeros_a(size_t* num_dimensions, size_t* dimentions, enum DataType* type) {
    switch (*type) {
        case DOUBLE: {
            struct array* array = new_a(num_dimensions, dimentions, type);
            double* arr_double = (double*)get_a(array);

            for (size_t i = 0; i < get_size_a(array); i++) {
                arr_double[i] = 0.0;
            }
            return array;
        }
        case INT: {
            struct array* array = new_a(num_dimensions, dimentions, type);
            int* arr_int = (int*)get_a(array);

            for (size_t i = 0; i < get_size_a(array); i++) {
                arr_int[i] = 0;
            }
            return array;
        }
        case FLOAT: {
            struct array* array = new_a(num_dimensions, dimentions, type);
            float* arr_float = (float*)get_a(array);

            for (size_t i = 0; i < get_size_a(array); i++) {
                arr_float[i] = 0.0;
            }
            return array;
        }
        case COMPLEX: {
            struct array* array = new_a(num_dimensions, dimentions, type);
            double complex* arr_complex = (double complex*)get_a(array);

            for (size_t i = 0; i < get_size_a(array); i++) {
                arr_complex[i] = 0.0 + 0.0*I;
            }
            return array;
        }
        default: {
            log_unsupported_type(__FILE__, __LINE__);
            break;
        }
    };
    return NULL;
}

/**
 * Returning initialized array with 1's
*/
struct array* ones_a(size_t* num_dimensions, size_t* dimentions, enum DataType* type) {

    switch (*type) {
        case DOUBLE: {
            struct array* array = new_a(num_dimensions, dimentions, type);
            
            double* arr_double = (double*)get_a(array);

            for (size_t i = 0; i < get_size_a(array); i++) {
                arr_double[i] = 1.0;
            }
            return array;
        }
        case INT: {
            struct array* array = new_a(num_dimensions, dimentions, type);
            int* arr_int = (int*)get_a(array);

            for (size_t i = 0; i < get_size_a(array); i++) {
                arr_int[i] = 1;
            }
            return array;
        }
        case FLOAT: {
            struct array* array = new_a(num_dimensions, dimentions, type);
            float* arr_float = (float*)get_a(array);
            for (size_t i = 0; i < get_size_a(array); i++) {
                arr_float[i] = 1.0;
            }
            return array;
        }
        case COMPLEX: {
            struct array* array = new_a(num_dimensions, dimentions, type);
            double complex* arr_complex = (double complex*)get_a(array);

            for (size_t i = 0; i < get_size_a(array); i++) {
                arr_complex[i] = 1.0 + 0.0*I;
            }
            return array;
        }
        default:
            log_unsupported_type(__FILE__, __LINE__);
            break;
    };
    return NULL;
}

/**
 * free all the elements in the array
*/
void free_a(struct array* array) {
    free(array->array);
    free(array->dimensions);
    free(array->num_dimensions);
    free(array->type);
    pthread_mutex_destroy(array->mutex);
    free(array->mutex);
    free(array);
}

/**
 * Element wise multiplication of two arrays
*/
struct array* multiply_a(struct array* arr1, struct array* arr2 ) {
    if (*arr1->type != COMPLEX) {
        if (*arr1->type != *arr2->type) {
            log_incompatible_types(__FILE__, __LINE__);
        }
    }
    
    if (*arr1->num_dimensions != *arr2->num_dimensions) {
        log_incompatible_size(__FILE__, __LINE__);
    }

    switch (*arr1->num_dimensions){
        case 1:{
            if (arr1->dimensions[0] != arr2->dimensions[0]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
            }
            break;
        }
        case 2:{
            if (arr1->dimensions[0] != arr2->dimensions[0] || arr1->dimensions[1] != arr2->dimensions[1]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
            }
            break;
        }
        case 3:{
            if (arr1->dimensions[0] != arr2->dimensions[0] || arr1->dimensions[1] != arr2->dimensions[1] || arr1->dimensions[2] != arr2->dimensions[2]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
            }
            break;
        }
        default:{
            log_invalid_dimensions(__FILE__, __LINE__);
            break;
        }
    }

    struct array* arrayMul = new_a(arr1->num_dimensions, arr1->dimensions, arr1->type);
    switch (*arr1->type){
        case DOUBLE: {
            double* arr1_array = arr1->array;
            double* arr2_array = arr2->array;
            double* mul_array = arrayMul->array;
            for (size_t i = 0; i < get_size_a(arr1); i++){
                mul_array[i] = arr1_array[i] * arr2_array[i];
            }
            break;
        }
        case INT: {
            int* arr1_array = arr1->array;
            int* arr2_array = arr2->array;
            int* mul_array = arrayMul->array;
            for (size_t i = 0; i < get_size_a(arr1); i++){
                mul_array[i] = arr1_array[i] * arr2_array[i];
            }
            break;
        }
        case FLOAT: {
            float* arr1_array = arr1->array;
            float* arr2_array = arr2->array;
            float* mul_array = arrayMul->array;
            for (size_t i = 0; i < get_size_a(arr1); i++){
                mul_array[i] = arr1_array[i] * arr2_array[i];
            }
            break;
        }
        case COMPLEX: {
            switch (*arr2->type) {
                case DOUBLE: {
                    double complex* arr1_array = arr1->array;
                    double* arr2_array = arr2->array;
                    double complex* mul_array = arrayMul->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        mul_array[i] = arr1_array[i] * arr2_array[i];
                    }
                    break;
                }
                case INT: {
                    double complex* arr1_array = arr1->array;
                    int* arr2_array = arr2->array;
                    double complex* mul_array = arrayMul->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        mul_array[i] = arr1_array[i] * arr2_array[i];
                    }
                    break;
                }
                case FLOAT: {
                    double complex* arr1_array = arr1->array;
                    float* arr2_array = arr2->array;
                    double complex* mul_array = arrayMul->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        mul_array[i] = arr1_array[i] * arr2_array[i];
                    }
                    break;
                }
                case COMPLEX: {
                    double complex* arr1_array = arr1->array;
                    double complex* arr2_array = arr2->array;
                    double complex* mul_array = arrayMul->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        mul_array[i] = arr1_array[i] * arr2_array[i];
                    }
                    break;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            break;
        }
        default: {
            free_a(arrayMul);
            log_unsupported_type(__FILE__, __LINE__);
            break;
        }
    }
    return arrayMul;
}

/**
 * Element wise division of two arrays
*/
struct array* divide_a(struct array* arr1, struct array* arr2 ) {
    if (*arr1->type != COMPLEX) {
        if (*arr1->type != *arr2->type) {
            log_incompatible_types(__FILE__, __LINE__);
        }
    }

    if (*arr1->num_dimensions != *arr2->num_dimensions) {
        log_incompatible_size(__FILE__, __LINE__);
    }

    switch (*arr1->num_dimensions){
        case 1:{
            if (arr1->dimensions[0] != arr2->dimensions[0]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
            }
            break;
        }
        case 2:{
            if (arr1->dimensions[0] != arr2->dimensions[0] || arr1->dimensions[1] != arr2->dimensions[1]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
            }
            break;
        }
        case 3:{
            if (arr1->dimensions[0] != arr2->dimensions[0] || arr1->dimensions[1] != arr2->dimensions[1] || arr1->dimensions[2] != arr2->dimensions[2]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
            }
            break;
        }
        default:{
            log_invalid_dimensions(__FILE__, __LINE__);
            break;
        }
    }


    struct array* arrayDiv = new_a(arr1->num_dimensions, arr1->dimensions, arr1->type);
    switch (*arr1->type){
        case DOUBLE: {
            double* arr1_array = arr1->array;
            double* arr2_array = arr2->array;
            double* div_array = arrayDiv->array;
            for (size_t i = 0; i < get_size_a(arr1); i++){
                div_array[i] = arr1_array[i] / arr2_array[i];
            }
            break;
        }
        case INT: {
            int* arr1_array = arr1->array;
            int* arr2_array = arr2->array;
            int* div_array = arrayDiv->array;
            for (size_t i = 0; i < get_size_a(arr1); i++){
                div_array[i] = arr1_array[i] / arr2_array[i];
            }
            break;
        }
        case FLOAT: {
            float* arr1_array = arr1->array;
            float* arr2_array = arr2->array;
            float* div_array = arrayDiv->array;
            for (size_t i = 0; i < get_size_a(arr1); i++){
                div_array[i] = arr1_array[i] / arr2_array[i];
            }
            break;
        }
        case COMPLEX: {
            switch (*arr2->type) {
                case DOUBLE: {
                    double complex* arr1_array = arr1->array;
                    double* arr2_array = arr2->array;
                    double complex* div_array = arrayDiv->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        div_array[i] = arr1_array[i] / arr2_array[i];
                    }
                    break;
                }
                case INT: {
                    double complex* arr1_array = arr1->array;
                    int* arr2_array = arr2->array;
                    double complex* div_array = arrayDiv->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        div_array[i] = arr1_array[i] / arr2_array[i];
                    }
                    break;
                }
                case FLOAT: {
                    double complex* arr1_array = arr1->array;
                    float* arr2_array = arr2->array;
                    double complex* div_array = arrayDiv->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        div_array[i] = arr1_array[i] / arr2_array[i];
                    }
                    break;
                }
                case COMPLEX: {
                    double complex* arr1_array = arr1->array;
                    double complex* arr2_array = arr2->array;
                    double complex* div_array = arrayDiv->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        div_array[i] = arr1_array[i] / arr2_array[i];
                    }
                    break;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            break;
        }
        default: {
            free_a(arrayDiv);
            log_unsupported_type(__FILE__, __LINE__);
            break;
        }
    }
    return arrayDiv;
}

/**
 * Element wise addition of two arrays
*/
struct array* addition_a(struct array* arr1, struct array* arr2 ) {
    if (*arr1->type != COMPLEX) {
        if (*arr1->type != *arr2->type) {
            log_incompatible_types(__FILE__, __LINE__);
        }
    }

    if (*arr1->num_dimensions != *arr2->num_dimensions) {
        log_incompatible_size(__FILE__, __LINE__);
    }

    switch (*arr1->num_dimensions){
        case 1:{
            if (arr1->dimensions[0] != arr2->dimensions[0]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
            }
            break;
        }
        case 2:{
            if (arr1->dimensions[0] != arr2->dimensions[0] || arr1->dimensions[1] != arr2->dimensions[1]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
            }
            break;
        }
        case 3:{
            if (arr1->dimensions[0] != arr2->dimensions[0] || arr1->dimensions[1] != arr2->dimensions[1] || arr1->dimensions[2] != arr2->dimensions[2]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
            }
            break;
        }
        default:{
            log_invalid_dimensions(__FILE__, __LINE__);
            break;
        }
    }

    struct array* arrayAdd = new_a(arr1->num_dimensions, arr1->dimensions, arr1->type);
    switch (*arr1->type){
        case DOUBLE: {
            double* arr1_array = arr1->array;
            double* arr2_array = arr2->array;
            double* add_array = arrayAdd->array;
            for (size_t i = 0; i < get_size_a(arr1); i++){
                add_array[i] = arr1_array[i] + arr2_array[i];
            }
            break;
        }
        case INT: {
            int* arr1_array = arr1->array;
            int* arr2_array = arr2->array;
            int* add_array = arrayAdd->array;
            for (size_t i = 0; i < get_size_a(arr1); i++){
                add_array[i] = arr1_array[i] + arr2_array[i];
            }
            break;
        }
        case FLOAT: {
            float* arr1_array = arr1->array;
            float* arr2_array = arr2->array;
            float* add_array = arrayAdd->array;
            for (size_t i = 0; i < get_size_a(arr1); i++){
                add_array[i] = arr1_array[i] + arr2_array[i];
            }
            break;
        }
        case COMPLEX: {
            switch (*arr2->type) {
                case DOUBLE: {
                    double complex* arr1_array = arr1->array;
                    double* arr2_array = arr2->array;
                    double complex* add_array = arrayAdd->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        add_array[i] = arr1_array[i] + arr2_array[i];
                    }
                    break;
                }
                case INT: {
                    double complex* arr1_array = arr1->array;
                    int* arr2_array = arr2->array;
                    double complex* add_array = arrayAdd->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        add_array[i] = arr1_array[i] + arr2_array[i];
                    }
                    break;
                }
                case FLOAT: {
                    double complex* arr1_array = arr1->array;
                    float* arr2_array = arr2->array;
                    double complex* add_array = arrayAdd->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        add_array[i] = arr1_array[i] + arr2_array[i];
                    }
                    break;
                }
                case COMPLEX: {
                    double complex* arr1_array = arr1->array;
                    double complex* arr2_array = arr2->array;
                    double complex* add_array = arrayAdd->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        add_array[i] = arr1_array[i] + arr2_array[i];
                    }
                    break;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            break;
        }
        default: {
            free_a(arrayAdd);
            log_unsupported_type(__FILE__, __LINE__);
            break;
        }
    }
    return arrayAdd;
}

/**
 * Element wise minus of two arrays
*/
struct array* minus_a(struct array* arr1, struct array* arr2 ) {
    if (*arr1->type != COMPLEX) {
        if (*arr1->type != *arr2->type) {
            log_incompatible_types(__FILE__, __LINE__);
        }
    }

    if (*arr1->num_dimensions != *arr2->num_dimensions) {
        log_incompatible_size(__FILE__, __LINE__);
    }

    switch (*arr1->num_dimensions){
        case 1:{
            if (arr1->dimensions[0] != arr2->dimensions[0]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
            }
            break;
        }
        case 2:{
            if (arr1->dimensions[0] != arr2->dimensions[0] || arr1->dimensions[1] != arr2->dimensions[1]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
            }
            break;
        }
        case 3:{
            if (arr1->dimensions[0] != arr2->dimensions[0] || arr1->dimensions[1] != arr2->dimensions[1] || arr1->dimensions[2] != arr2->dimensions[2]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
            }
            break;
        }
        default:{
            log_invalid_dimensions(__FILE__, __LINE__);
            break;
        }
    }

    struct array* arrayMinus = new_a(arr1->num_dimensions, arr1->dimensions, arr1->type);
    switch (*arr1->type){
        case DOUBLE: {
            double* arr1_array = arr1->array;
            double* arr2_array = arr2->array;
            double* minus_array = arrayMinus->array;
            for (size_t i = 0; i < get_size_a(arr1); i++){
                minus_array[i] = arr1_array[i] - arr2_array[i];
            }
            break;
        }
        case INT: {
            int* arr1_array = arr1->array;
            int* arr2_array = arr2->array;
            int* minus_array = arrayMinus->array;
            for (size_t i = 0; i < get_size_a(arr1); i++){
                minus_array[i] = arr1_array[i] - arr2_array[i];
            }
            break;
        }
        case FLOAT: {
            float* arr1_array = arr1->array;
            float* arr2_array = arr2->array;
            float* minus_array = arrayMinus->array;
            for (size_t i = 0; i < get_size_a(arr1); i++){
                minus_array[i] = arr1_array[i] - arr2_array[i];
            }
            break;
        }
        case COMPLEX: {
            switch (*arr2->type) {
                case DOUBLE: {
                    double complex* arr1_array = arr1->array;
                    double* arr2_array = arr2->array;
                    double complex* minus_array = arrayMinus->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        minus_array[i] = arr1_array[i] - arr2_array[i];
                    }
                    break;
                }
                case INT: {
                    double complex* arr1_array = arr1->array;
                    int* arr2_array = arr2->array;
                    double complex* minus_array = arrayMinus->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        minus_array[i] = arr1_array[i] - arr2_array[i];
                    }
                    break;
                }
                case FLOAT: {
                    double complex* arr1_array = arr1->array;
                    float* arr2_array = arr2->array;
                    double complex* minus_array = arrayMinus->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        minus_array[i] = arr1_array[i] - arr2_array[i];
                    }
                    break;
                }
                case COMPLEX: {
                    double complex* arr1_array = arr1->array;
                    double complex* arr2_array = arr2->array;
                    double complex* minus_array = arrayMinus->array;
                    for (size_t i = 0; i < get_size_a(arr1); i++){
                        minus_array[i] = arr1_array[i] - arr2_array[i];
                    }
                    break;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            break;
        }
        default: {
            free_a(arrayMinus);
            log_unsupported_type(__FILE__, __LINE__);
            break;
        }
    }
    return arrayMinus;
}

/**
 * Set the value at a specific index in the array.
 */
void set_element_a(struct array* array, size_t* indices, void* value) {
    size_t size = get_size_a(array);
    switch (*array->num_dimensions){
      case 1:{
        size_t index = indices[0];
        switch (*array->type){
          case DOUBLE:{
            double* arr = (double*)get_a(array);
            if (index < size) {
                arr[index] = *(double*)value;
            } else {
                log_index_out_of_bounds(__FILE__, __LINE__);
            }
            break;
          }
          case INT:{
            int* arr = get_a(array);
            if (index < size) {
                arr[index] = *(int*)value;
            } else {
                log_index_out_of_bounds(__FILE__, __LINE__);
            }
            break;
          } 
          case FLOAT:{
            float* arr = get_a(array);
            if (index < size) {
                arr[index] = *(float*)value;
            } else {
                log_index_out_of_bounds(__FILE__, __LINE__);
            }
            break;
          }
          case COMPLEX:{
            double complex* arr = get_a(array);
            if (index < size) {
                arr[index] = *(double complex*)value;
            } else {
                log_index_out_of_bounds(__FILE__, __LINE__);
            }
            break;
          }   
          default:
            log_incompatible_types(__FILE__, __LINE__);
            break;
        }
        break;
      }
      case 2:{
        size_t index = get_2d_index_a(array, indices);
        switch (*array->type){
          case DOUBLE:{
            double* arr = get_a(array);
            if (index < size) {
                arr[index] = *(double*)value;
            } else {
                log_index_out_of_bounds(__FILE__, __LINE__);
            }
            break;
          }
          case INT:{
            int* arr = get_a(array);
            if (index < size) {
                arr[index] = *(int*)value;
            } else {
                log_index_out_of_bounds(__FILE__, __LINE__);
            }
            break;
          } 
          case FLOAT:{
            float* arr = get_a(array);
            if (index < size) {
                arr[index] = *(float*)value;
            } else {
                log_index_out_of_bounds(__FILE__, __LINE__);
            }
            break;
          }
          case COMPLEX:{
            double complex* arr = get_a(array);
            if (index < size) {
                arr[index] = *(double complex*)value;
            } else {
                log_index_out_of_bounds(__FILE__, __LINE__);
            }
            break;
          }   
          default:
            log_incompatible_types(__FILE__, __LINE__);
            break;
        }
        break;
      }
      case 3:{
        size_t index = get_3d_index_a(array, indices);
        switch (*array->type){
          case DOUBLE:{
            double* arr = get_a(array);
            if (index < size) {
                arr[index] = *(double*)value;
            } else {
                log_index_out_of_bounds(__FILE__, __LINE__);
            }
            break;
          }
          case INT:{
            int* arr = get_a(array);
            if (index < size) {
                arr[index] = *(int*)value;
            } else {
                log_index_out_of_bounds(__FILE__, __LINE__);
            }
            break;
          }
          case FLOAT:{
            float* arr = get_a(array);
            if (index < size) {
                arr[index] = *(float*)value;
            } else {
                log_index_out_of_bounds(__FILE__, __LINE__);
            }
            break;
          }
          case COMPLEX:{
            double complex* arr = get_a(array);
            if (index < size) {
                arr[index] = *(double complex*)value;
            } else {
                log_index_out_of_bounds(__FILE__, __LINE__);
            }
            break;
          }  
          default:
            log_invalid_type(__FILE__, __LINE__);
            break;
        }
        break;
      }
      default:
        log_invalid_dimensions(__FILE__, __LINE__);
        break;
    }
}

/**
 * Returns 1D index from 2D
*/
size_t get_2d_index_a(struct array* array, size_t* indices){
    size_t* dimensions = array->dimensions;
    return (size_t)(indices[0] * dimensions[1] + indices[1]);
}

/**
 * Returns 1D index from 3D
*/
size_t get_3d_index_a(struct array* array, size_t* indices){
    size_t* dimensions = array->dimensions;
    return (size_t)(indices[0] * (dimensions[1] * dimensions[2]) + indices[1] * dimensions[2] + indices[2]);
}

/**
 * Get array size
*/
size_t get_size_a(struct array* array){
    size_t* dimensions = array->dimensions;

    switch(*array->num_dimensions){
      case 1:{
        return (size_t)(dimensions[0]);
      }
      case 2:{
        return (size_t)(dimensions[0]*dimensions[1]);
      }
      case 3:{
        return (size_t)(dimensions[0]*dimensions[1]*dimensions[2]);
      }
      default:{
        log_invalid_dimensions(__FILE__, __LINE__);
        break;
      }
    }
    
    return 0;
}

/**
 * Returning an array with n amount of points evenly spaced between start and end
*/
struct array* linspace_a(void* start, void* end, size_t* num_points, enum DataType* type) {
    struct array* result = NULL;
    switch (*type) {
        case DOUBLE: {
            result = new_a(&(size_t){1}, (size_t[]){*num_points}, &(enum DataType){DOUBLE});
            // printarray(result);
            double* arr = (double*)result->array;
            if (*num_points == 1) {
                arr[0] = *(double*)start;
                break;
            }

            double step = (*(double*)end - *(double*)start) / (*num_points - 1);

            for (size_t j = 0; j < *num_points; j++){
                arr[j] = (*(double*)start + j * step);
            }
            // printarray(result);
            return result;
        }
        case FLOAT: {
            result = new_a(&(size_t){1}, (size_t[]){*num_points}, &(enum DataType){FLOAT});
            float* arr = (float*)result->array;
            if (*num_points == 1) {
                arr[0] = *(float*)start;
                break;
            }

            float step = (*(float*)end - *(float*)start) / (*num_points - 1);

            for (size_t j = 0; j < *num_points; j++){
                arr[j] = (*(float*)start + j * step);
            }
            return result;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }

    return NULL;
}

/**
 * array multiplied by constant
*/
struct array* multiply_constant_a(struct array* array, void* constant, enum DataType* constant_type) {
    struct array* result = zeros_a(array->num_dimensions, array->dimensions, array->type);
    switch (*array->type) {
        case DOUBLE: {
            if (*constant_type != DOUBLE) {
                log_incompatible_types(__FILE__, __LINE__);
            }
            double* result_double = (double*)result->array;
            double* arr_double = (double*)array->array;
            double value = *(double*)constant;
            for (size_t i = 0; i < get_size_a(array); i++){
                result_double[i] = arr_double[i] * value;
            }

            break;
        }
        case INT: {
            if (*constant_type != INT) {
                log_incompatible_types(__FILE__, __LINE__);
            }
            int* result_int = (int*)result->array;
            int* arr_int = (int*)array->array;
            int value = *(int*)constant;
            for (size_t i = 0; i < get_size_a(array); i++){
                result_int[i] = arr_int[i] * value;
            }
            break;
        }
        case FLOAT: {
            if (*constant_type != FLOAT) {
                log_incompatible_types(__FILE__, __LINE__);
            }
            float* result_float = (float*)result->array;
            float* arr_float = (float*)array->array;
            float value = *(float*)constant;
            for (size_t i = 0; i < get_size_a(array); i++){
                result_float[i] = arr_float[i] * value;
            }
            break;
        }
        case COMPLEX: {
            switch (*constant_type) {
                case DOUBLE: {
                    if (*constant_type != DOUBLE) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    double value = *(double*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] * value;
                    }
                    break;
                }
                case INT: {
                    if (*constant_type != INT) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    int value = *(int*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] * value;
                    }
                    break;
                }
                case FLOAT: {
                    if (*constant_type != FLOAT) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    float value = *(float*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] * value;
                    }
                    break;
                }
                case COMPLEX: {
                    if (*constant_type != COMPLEX) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    double complex value = *(double complex*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] * value;
                    }
                    break;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }

    return result;
}

/**
 * array divised by constant
*/
struct array* division_constant_a(struct array* array, void* constant, enum DataType* constant_type) {
    struct array* result = zeros_a(array->num_dimensions, array->dimensions, array->type);
    switch (*array->type) {
        case DOUBLE: {
            if (*constant_type != DOUBLE) {
                log_incompatible_types(__FILE__, __LINE__);
            }
            double* result_double = (double*)result->array;
            double* arr_double = (double*)array->array;
            double value = *(double*)constant;
            for (size_t i = 0; i < get_size_a(array); i++){
                result_double[i] = arr_double[i] / value;
            }

            break;
        }
        case INT: {
            if (*constant_type != INT) {
                log_incompatible_types(__FILE__, __LINE__);
            }
            int* result_int = (int*)result->array;
            int* arr_int = (int*)array->array;
            int value = *(int*)constant;
            for (size_t i = 0; i < get_size_a(array); i++){
                result_int[i] = arr_int[i] / value;
            }
            break;
        }
        case FLOAT: {
            if (*constant_type != FLOAT) {
                log_incompatible_types(__FILE__, __LINE__);
            }
            float* result_float = (float*)result->array;
            float* arr_float = (float*)array->array;
            float value = *(float*)constant;
            for (size_t i = 0; i < get_size_a(array); i++){
                result_float[i] = arr_float[i] / value;
            }
            break;
        }
        case COMPLEX: {
            switch (*constant_type) {
                case DOUBLE: {
                    if (*constant_type != DOUBLE) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    double value = *(double*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] / value;
                    }
                    break;
                }
                case INT: {
                    if (*constant_type != INT) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    int value = *(int*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] / value;
                    }
                    break;
                }
                case FLOAT: {
                    if (*constant_type != FLOAT) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    float value = *(float*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] / value;
                    }
                    break;
                }
                case COMPLEX: {
                    if (*constant_type != COMPLEX) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    double complex value = *(double complex*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] / value;
                    }
                    break;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }

    return result;
}

/**
 * array minus by constant
*/
struct array* minus_constant_a(struct array* array, void* constant, enum DataType* constant_type) {
    struct array* result = zeros_a(array->num_dimensions, array->dimensions, array->type);
    switch (*array->type) {
        case DOUBLE: {
            if (*constant_type != DOUBLE) {
                log_incompatible_types(__FILE__, __LINE__);
            }
            double* result_double = (double*)result->array;
            double* arr_double = (double*)array->array;
            double value = *(double*)constant;
            for (size_t i = 0; i < get_size_a(array); i++){
                result_double[i] = arr_double[i] - value;
            }

            break;
        }
        case INT: {
            if (*constant_type != INT) {
                log_incompatible_types(__FILE__, __LINE__);
            }
            int* result_int = (int*)result->array;
            int* arr_int = (int*)array->array;
            int value = *(int*)constant;
            for (size_t i = 0; i < get_size_a(array); i++){
                result_int[i] = arr_int[i] - value;
            }
            break;
        }
        case FLOAT: {
            if (*constant_type != FLOAT) {
                log_incompatible_types(__FILE__, __LINE__);
            }
            float* result_float = (float*)result->array;
            float* arr_float = (float*)array->array;
            float value = *(float*)constant;
            for (size_t i = 0; i < get_size_a(array); i++){
                result_float[i] = arr_float[i] - value;
            }
            break;
        }
        case COMPLEX: {
            switch (*constant_type) {
                case DOUBLE: {
                    if (*constant_type != DOUBLE) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    double value = *(double*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] - value;
                    }
                    break;
                }
                case INT: {
                    if (*constant_type != INT) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    int value = *(int*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] - value;
                    }
                    break;
                }
                case FLOAT: {
                    if (*constant_type != FLOAT) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    float value = *(float*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] - value;
                    }
                    break;
                }
                case COMPLEX: {
                    if (*constant_type != COMPLEX) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    double complex value = *(double complex*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] - value;
                    }
                    break;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }

    return result;
}

/**
 * array addition by constant
*/
struct array* addition_constant_a(struct array* array, void* constant, enum DataType* constant_type) {
    struct array* result = zeros_a(array->num_dimensions, array->dimensions, array->type);
    switch (*array->type) {
        case DOUBLE: {
            if (*constant_type != DOUBLE) {
                log_incompatible_types(__FILE__, __LINE__);
            }
            double* result_double = (double*)result->array;
            double* arr_double = (double*)array->array;
            double value = *(double*)constant;
            for (size_t i = 0; i < get_size_a(array); i++){
                result_double[i] = arr_double[i] + value;
            }

            break;
        }
        case INT: {
            if (*constant_type != INT) {
                log_incompatible_types(__FILE__, __LINE__);
            }
            int* result_int = (int*)result->array;
            int* arr_int = (int*)array->array;
            int value = *(int*)constant;
            for (size_t i = 0; i < get_size_a(array); i++){
                result_int[i] = arr_int[i] + value;
            }
            break;
        }
        case FLOAT: {
            if (*constant_type != FLOAT) {
                log_incompatible_types(__FILE__, __LINE__);
            }
            float* result_float = (float*)result->array;
            float* arr_float = (float*)array->array;
            float value = *(float*)constant;
            for (size_t i = 0; i < get_size_a(array); i++){
                result_float[i] = arr_float[i] + value;
            }
            break;
        }
        case COMPLEX: {
            switch (*constant_type) {
                case DOUBLE: {
                    if (*constant_type != DOUBLE) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    double value = *(double*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] + value;
                    }
                    break;
                }
                case INT: {
                    if (*constant_type != INT) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    int value = *(int*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] + value;
                    }
                    break;
                }
                case FLOAT: {
                    if (*constant_type != FLOAT) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    float value = *(float*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] + value;
                    }
                    break;
                }
                case COMPLEX: {
                    if (*constant_type != COMPLEX) {
                        log_incompatible_types(__FILE__, __LINE__);
                    }
                    double complex* result_complex = (double complex*)result->array;
                    double complex* arr_complex = (double complex*)array->array;
                    double complex value = *(double complex*)constant;
                    for (size_t i = 0; i < get_size_a(array); i++){
                        result_complex[i] = arr_complex[i] + value;
                    }
                    break;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }

    return result;
}

/**
 * Get the max value in an array.
 * RETURN:
 *  - Pointer to allocated space.
 * NOTE:
 *  - Remember to free the allocated space.
*/
void* max_a(struct array* array) {
    switch (*array->type) {
        case DOUBLE: {
            double* result = (double*)malloc(sizeof(double));
            double* arr = (double*)array->array;

            double max = arr[0];
            for (size_t i = 1; i < get_size_a(array); i++){
                if (arr[i] > max) {
                    max = arr[i];
                }
            }

            *result = max;
            return result;
        }
        case INT: {
            int* result = (int*)malloc(sizeof(int));
            int* arr = (int*)array->array;

            int max = arr[0];
            for (size_t i = 1; i < get_size_a(array); i++){
                if (arr[i] > max) {
                    max = arr[i];
                }
            }
            *result = max;
            return result;
        }
        case FLOAT: {
            float* result = (float*)malloc(sizeof(float));
            float* arr = (float*)array->array;

            float max = arr[0];
            for (size_t i = 1; i < get_size_a(array); i++){
                if (arr[i] > max) {
                    max = arr[i];
                }
            }
            *result = max;
            return result;
        }
        case COMPLEX: {
            // double complex* arr = (double complex*)array->array;

            // double complex max = arr[0];
            // for (size_t i = 1; i < getarraySize(array); i++){
            //     if (arr[i] > max) {
            //         max = arr[i];
            //     }
            // }
            log_not_implemented(__FILE__, __LINE__);
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;
}

/**
 * Get the absolute max in an array.
 * RETURN:
 *  - Pointer to allocated space.
 * NOTE:
 *  - Remember to free the allocated space.
*/
void* amax_a(struct array* array) {
    switch (*array->type) {
        case DOUBLE: {
            double* result = (double*)malloc(sizeof(double));
            double* arr = (double*)array->array;

            double max;
            if (arr[0] < 0) {
                max = -1 * arr[0];
            } else {
                max = arr[0];
            }
            
            for (size_t i = 1; i < get_size_a(array); i++){
                if (arr[i] < 0) {
                    if (-1*arr[i] > max) {
                        max = -1*arr[i];
                    }
                } else {
                    if (arr[i] > max) {
                        max = arr[i];
                    }
                }
            }

            *result = max;
            return result;
        }
        case INT: {
            int* result = (int*)malloc(sizeof(int));
            int* arr = (int*)array->array;

            int max;
            if (arr[0] < 0) {
                max = -1 * arr[0];
            } else {
                max = arr[0];
            }
            for (size_t i = 1; i < get_size_a(array); i++){
                if (arr[i] < 0) {
                    if (-1*arr[i] > max) {
                        max = -1*arr[i];
                    }
                } else {
                    if (arr[i] > max) {
                        max = arr[i];
                    }
                }
            }
            *result = max;
            return result;
        }
        case FLOAT: {
            float* result = (float*)malloc(sizeof(float));
            float* arr = (float*)array->array;

            float max;
            if (arr[0] < 0) {
                max = -1 * arr[0];
            } else {
                max = arr[0];
            }
            for (size_t i = 1; i < get_size_a(array); i++){
                if (arr[i] < 0) {
                    if (-1*arr[i] > max) {
                        max = -1*arr[i];
                    }
                } else {
                    if (arr[i] > max) {
                        max = arr[i];
                    }
                }
            }
            *result = max;
            return result;
        }
        case COMPLEX: {
            // double complex* arr = (double complex*)array->array;

            // double complex max = arr[0];
            // for (size_t i = 1; i < getarraySize(array); i++){
            //     if (arr[i] < 0) {
            //         if (-1*arr[i] > max) {
            //             max = -1*arr[i];
            //         }
            //     } else {
            //         if (arr[i] > max) {
            //             max = arr[i];
            //         }
            //     }
            // }
            log_not_implemented(__FILE__, __LINE__);
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;
}

/**
 * Get the min value in an array.
 * RETURN:
 *  - Pointer to allocated space.
 * NOTE:
 *  - Remember to free the allocated space.
*/
void* min_a(struct array* array) {
    switch (*array->type) {
        case DOUBLE: {
            double* result = (double*)malloc(sizeof(double));
            double* arr = (double*)array->array;

            double min = arr[0];
            for (size_t i = 1; i < get_size_a(array); i++){
                if (arr[i] < min) {
                    min = arr[i];
                }
            }
            *result = min;
            return result;
        }
        case INT: {
            int* result = (int*)malloc(sizeof(int));
            int* arr = (int*)array->array;

            int min = arr[0];
            for (size_t i = 1; i < get_size_a(array); i++){
                if (arr[i] < min) {
                    min = arr[i];
                }
            }
            *result = min;
            return result;
        }
        case FLOAT: {
            float* result = (float*)malloc(sizeof(float));
            float* arr = (float*)array->array;

            float min = arr[0];
            for (size_t i = 1; i < get_size_a(array); i++){
                if (arr[i] < min) {
                    min = arr[i];
                }
            }
            *result = min;
            return result;
        }
        case COMPLEX: {
            // double complex* arr = (double complex*)array->array;

            // double complex max = arr[0];
            // for (size_t i = 1; i < getarraySize(array); i++){
            //     if (arr[i] > max) {
            //         max = arr[i];
            //     }
            // }
            log_not_implemented(__FILE__, __LINE__);
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;
}

/**
 * This function copies a array with specific dimensions to another array
 * with different dimensions through slicing
*/
void copy_over_a(struct array* destination, struct array* source, int* des_slice, int* source_slice) {
    if (*destination->type != *source->type) {
        log_incompatible_types(__FILE__, __LINE__);
    }

    if (*destination->num_dimensions < *source->num_dimensions) {
        log_incompatible_size(__FILE__, __LINE__);
    }
    switch (*destination->num_dimensions) {
        case 1: {
            if (des_slice[0] != -1 || source_slice[0] != -1) {
                log_incompatible_slice(__FILE__, __LINE__);
            }
            if (*destination->num_dimensions != *source->num_dimensions) {
                log_incompatible_size(__FILE__, __LINE__);
            }
            if (destination->dimensions[0] != source->dimensions[0]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
            }
            switch (*destination->type) {
                case DOUBLE: {
                    double* dest_double = (double*)destination->array;
                    double* source_double = (double*)source->array;
                    for (size_t i = 0; i < get_size_a(source); i++){
                        dest_double[i] = source_double[i];
                    }
                    break;
                }
                case INT: {
                    int* dest_int = (int*)destination->array;
                    int* source_int = (int*)source->array;
                    for (size_t i = 0; i < get_size_a(source); i++){
                        dest_int[i] = source_int[i];
                    }
                    break;
                }
                case FLOAT: {
                    float* dest_float = (float*)destination->array;
                    float* source_float = (float*)source->array;
                    for (size_t i = 0; i < get_size_a(source); i++){
                        dest_float[i] = source_float[i];
                    }
                    break;
                }
                case COMPLEX: {
                    double complex* dest_complex = (double complex*)destination->array;
                    double complex* source_complex = (double complex*)source->array;
                    for (size_t i = 0; i < get_size_a(source); i++){
                        dest_complex[i] = source_complex[i];
                    }
                    break;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            break;
        }
        case 2: {
            switch (*source->num_dimensions) {
                case 1: {
                    // [-1,0]
                    if (des_slice[0] == -1 && des_slice[1] >= 0 && source_slice[0] == -1) {
                        //Checking dimensions
                        if (destination->dimensions[0] != source->dimensions[0]) {
                            log_incompatible_dimensions(__FILE__, __LINE__);
                        }
                        //Operation
                        for (size_t i = 0; i < destination->dimensions[0]; i++) {
                            set_element_a(destination,(size_t[]){i,des_slice[1]}, get_element_a(source,(size_t[]){i}));
                        }
                        break;
                    }
                    //[0,-1]
                    if (des_slice[0] >= 0 && des_slice[1] == -1 && source_slice[0] == -1) {
                        //Checking dimensions
                        if (destination->dimensions[1] != source->dimensions[0]) {
                            log_incompatible_dimensions(__FILE__, __LINE__);
                        }
                        //Operation
                        for (size_t i = 0; i < destination->dimensions[1]; i++) {
                            set_element_a(destination,(size_t[]){des_slice[0],i}, get_element_a(source,(size_t[]){i}));
                        }
                        break;
                    }
                    //[0,0]
                    if (des_slice[0] != -1 && des_slice[1] != -1) {
                        log_invalid_dimensions(__FILE__, __LINE__);
                    }
                    //[-1,-1]
                    if (des_slice[0] == -1 && des_slice[1] == -1) {
                        log_incompatible_dimensions(__FILE__, __LINE__);
                    }
                    log_incompatible_slice(__FILE__, __LINE__);
                    break;
                }
                case 2: {
                    //des_slice = [-1,-1], source_slice = [-1,-1] 
                    if (des_slice[0] == -1 && des_slice[1] == -1 && source_slice[0] == -1 && source_slice[1] == -1) {

                        //Checking if both arrays are of the same dimensions.
                        if (destination->dimensions[0] != source->dimensions[0] || destination->dimensions[1] != source->dimensions[1]) {
                            log_incompatible_dimensions(__FILE__, __LINE__);
                        }

                        switch (*destination->type) {
                            case DOUBLE: {
                                double* des_arr = destination->array;
                                double* source_arr = source->array;
                                for (size_t i = 0; i < get_size_a(destination); i++){
                                    des_arr[i] = source_arr[i];
                                }
                                break;
                            }
                            case INT: {
                                int* des_arr = destination->array;
                                int* source_arr = source->array;
                                for (size_t i = 0; i < get_size_a(destination); i++){
                                    des_arr[i] = source_arr[i];
                                }
                                break;
                            }
                            case FLOAT: {
                                float* des_arr = destination->array;
                                float* source_arr = source->array;
                                for (size_t i = 0; i < get_size_a(destination); i++){
                                    des_arr[i] = source_arr[i];
                                }
                                break;
                            }
                            case COMPLEX: {
                                double complex* des_arr = destination->array;
                                double complex* source_arr = source->array;
                                for (size_t i = 0; i < get_size_a(destination); i++){
                                    des_arr[i] = source_arr[i];
                                }
                                break;
                            }
                            default: {
                                log_invalid_type(__FILE__, __LINE__);
                            }
                        }
                        break;
                    }

                    //des_slice = [-1,0], source_slice = [-1,0] 
                    if (des_slice[0] == -1 && des_slice[1] >= 0 && source_slice[0] == -1 && source_slice[1] >= 0) {

                        //Checking if both arrays are of the same dimensions.
                        if (destination->dimensions[0] != source->dimensions[0] || des_slice[1] >= destination->dimensions[1] || source_slice[1] >= source->dimensions[1]) {
                            log_incompatible_dimensions(__FILE__, __LINE__);
                        }

                        for (int i = 0; i < destination->dimensions[0]; i++) {
                            set_element_a(destination,(size_t[]){i,des_slice[1]}, get_element_a(source, (size_t[]){i,source_slice[1]}));
                        }
                        break;
                    }
                    //des_slice = [0,-1], source_slice = [0,-1] 
                    if (des_slice[0] >= 0 && des_slice[1] == -1 && source_slice[0] >= 0 && source_slice[1] == -1) {
                        //Checking if both arrays are of the same dimensions.
                        if (destination->dimensions[1] != source->dimensions[1] || des_slice[0] >= destination->dimensions[0] || source_slice[0] >= source->dimensions[0]) {
                            log_incompatible_dimensions(__FILE__, __LINE__);
                        }

                        for (int i = 0; i < destination->dimensions[1]; i++) {
                            set_element_a(destination,(size_t[]){des_slice[0],i}, get_element_a(source, (size_t[]){source_slice[0],i}));
                        }
                        break;
                    }

                    //des_slice = [-1,0], source_slice = [0,-1] 
                    if (des_slice[0] == -1 && des_slice[1] >= 0 && source_slice[0] >= 0 && source_slice[1] == -1) {
                        //Checking if both arrays are of the same dimensions.
                        if (destination->dimensions[0] != source->dimensions[1] || des_slice[1] >= destination->dimensions[1] || source_slice[0] >= source->dimensions[0]) {
                            log_incompatible_dimensions(__FILE__, __LINE__);
                        }

                        for (int i = 0; i < destination->dimensions[1]; i++) {
                            set_element_a(destination,(size_t[]){i,des_slice[1]}, get_element_a(source, (size_t[]){source_slice[0],i}));
                        }
                        break;
                    }

                    //des_slice = [0,-1], source_slice = [-1,0] 
                    if (des_slice[0] >= 0 && des_slice[1] == -1 && source_slice[0] == -1 && source_slice[1] >= 0) {
                        //Checking if both arrays are of the same dimensions.
                        if (destination->dimensions[1] != source->dimensions[0] || des_slice[0] >= destination->dimensions[0] || source_slice[1] >= source->dimensions[1]) {
                            log_incompatible_dimensions(__FILE__, __LINE__);
                        }

                        for (int i = 0; i < destination->dimensions[1]; i++) {
                            set_element_a(destination,(size_t[]){des_slice[0],i}, get_element_a(source, (size_t[]){i,source_slice[1]}));
                        }
                        break;
                    }

                    log_incompatible_slice(__FILE__, __LINE__);
                    break;
                }
                default: {
                    log_invalid_dimensions(__FILE__, __LINE__);
                    break;
                }
            }
            break;
        }
        case 3: {
            switch (*source->num_dimensions) {
                case 1: {
                    if (source_slice[0] != -1) {
                        log_incompatible_slice(__FILE__, __LINE__);
                    }
                    //des_slice[-1,0,0], source_slice=[-1]
                    if (des_slice[0] == -1 && des_slice[1] >= 0 && des_slice[2] >= 0) {
                        if (destination->dimensions[0] != source->dimensions[0] || des_slice[1] >= destination->dimensions[1] || des_slice[2] >= destination->dimensions[2]) {
                            log_incompatible_slice(__FILE__, __LINE__);
                            break;
                        }
                        for (int i = 0; i < destination->dimensions[0]; i++) {
                            set_element_a(destination,(size_t[]){i, des_slice[1], des_slice[2]}, get_element_a(source, (size_t[]){i}));
                        }
                        break;
                    }

                    //des_slice[0,0,-1], source_slice=[-1]
                    if (des_slice[0] >= 0 && des_slice[1] >= 0 && des_slice[2] == -1) {
                        if (destination->dimensions[2] != source->dimensions[0] || des_slice[0] >= destination->dimensions[0] || des_slice[1] >= destination->dimensions[1]) {
                            log_incompatible_slice(__FILE__, __LINE__);
                            break;
                        }
                        for (int i = 0; i < destination->dimensions[2]; i++) {
                            set_element_a(destination,(size_t[]){des_slice[0], des_slice[1], i}, get_element_a(source, (size_t[]){i}));
                        }
                        break;
                    }

                    //des_slice[0,-1,0], source_slice=[-1]
                    if (des_slice[0] >= 0 && des_slice[1] == -1 && des_slice[2] >= 0) {
                        if (destination->dimensions[1] != source->dimensions[0] || des_slice[0] >= destination->dimensions[0] || des_slice[2] >= destination->dimensions[2]) {
                            log_incompatible_slice(__FILE__, __LINE__);
                            break;
                        }
                        for (int i = 0; i < destination->dimensions[1]; i++) {
                            set_element_a(destination,(size_t[]){des_slice[0], i, des_slice[2]}, get_element_a(source, (size_t[]){i}));
                        }
                        break;
                    }

                    log_incompatible_slice(__FILE__, __LINE__);
                    break;
                }
                case 2: {
                    //des_slice[-1,-1,0], source_slice=[-1,-1]
                    if (des_slice[0] == -1 && des_slice[1] == -1 && des_slice[2] >= 0) {
                        if (destination->dimensions[0] != source->dimensions[0] || destination->dimensions[1] != source->dimensions[1] || des_slice[2] >= destination->dimensions[2] ) {
                            log_incompatible_slice(__FILE__, __LINE__);
                            break;
                        }

                        for (int i = 0; i < destination->dimensions[0]; i++) {
                            for (int j = 0; j < destination->dimensions[1]; j++) {
                                set_element_a(destination,(size_t[]){i, j, des_slice[2]}, get_element_a(source, (size_t[]){i,j}));
                            }
                        }
                        break;
                    }

                    //des_slice[-1,0,-1], source_slice=[-1,-1]
                    if (des_slice[0] == -1 && des_slice[1] >= 0 && des_slice[2] == -1) {
                        if (destination->dimensions[0] != source->dimensions[0] || destination->dimensions[2] != source->dimensions[1] || des_slice[1] >= destination->dimensions[1] ) {
                            log_incompatible_slice(__FILE__, __LINE__);
                            break;
                        }

                        for (int i = 0; i < destination->dimensions[0]; i++) {
                            for (int j = 0; j < destination->dimensions[2]; j++) {
                                set_element_a(destination,(size_t[]){i, des_slice[1], j}, get_element_a(source, (size_t[]){i,j}));
                            }
                        }
                        break;
                    }

                    //des_slice[0,-1,-1], source_slice=[-1,-1]
                    if (des_slice[0] >= 0 && des_slice[1] == -1 && des_slice[2] == -1) {
                        if (destination->dimensions[1] != source->dimensions[0] || destination->dimensions[2] != source->dimensions[1] || des_slice[0] >= destination->dimensions[0] ) {
                            log_incompatible_slice(__FILE__, __LINE__);
                            break;
                        }

                        for (int i = 0; i < destination->dimensions[1]; i++) {
                            for (int j = 0; j < destination->dimensions[2]; j++) {
                                set_element_a(destination,(size_t[]){des_slice[0], i, j}, get_element_a(source, (size_t[]){i,j}));
                            }
                        }
                        break;
                    }

                    //des_slice[0,0,-1], source_slice=[0,-1]
                    //des_slice[0,0,-1], source_slice=[-1,0]
                    if (des_slice[0] >= 0 && des_slice[1] >= 0 && des_slice[2] == -1) {
                        if (source_slice[0] >= 0 && source_slice[1] == -1) {
                            if (destination->dimensions[2] != source->dimensions[1] || des_slice[0] >= destination->dimensions[0] || des_slice[1] >= destination->dimensions[1] || source_slice[0] >= source->dimensions[0]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[2]; i++) {
                                set_element_a(destination,(size_t[]){des_slice[0], des_slice[1], i}, get_element_a(source, (size_t[]){source_slice[0],i}));
                            }
                            break;
                        }
                        if (source_slice[0] == -1 && source_slice[1] >= 0) {
                            if (destination->dimensions[2] != source->dimensions[0] || des_slice[0] >= destination->dimensions[0] || des_slice[1] >= destination->dimensions[1] || source_slice[1] >= source->dimensions[1]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[2]; i++) {
                                set_element_a(destination,(size_t[]){des_slice[0], des_slice[1], i}, get_element_a(source, (size_t[]){i,source_slice[1]}));
                            }
                            break;
                        }
                        log_incompatible_slice(__FILE__, __LINE__);
                        break;
                    }

                    //des_slice[0,-1,0], source_slice=[0,-1]
                    //des_slice[0,-1,0], source_slice=[-1,0]
                    if (des_slice[0] >= 0 && des_slice[1] == -1 && des_slice[2] >= 0) {
                        if (source_slice[0] >= 0 && source_slice[1] == -1) {
                            if (destination->dimensions[1] != source->dimensions[1] || des_slice[0] >= destination->dimensions[0] || des_slice[2] >= destination->dimensions[2] || source_slice[0] >= source->dimensions[0]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[1]; i++) {
                                set_element_a(destination,(size_t[]){des_slice[0], i, des_slice[2]}, get_element_a(source, (size_t[]){source_slice[0],i}));
                            }
                            break;
                        }
                        if (source_slice[0] == -1 && source_slice[1] >= 0) {
                            if (destination->dimensions[1] != source->dimensions[0] || des_slice[0] >= destination->dimensions[0] || des_slice[2] >= destination->dimensions[2] || source_slice[1] >= source->dimensions[1]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[1]; i++) {
                                set_element_a(destination,(size_t[]){des_slice[0], i, des_slice[2]}, get_element_a(source, (size_t[]){i,source_slice[1]}));
                            }
                            break;
                        }
                        log_incompatible_slice(__FILE__, __LINE__);
                        break;
                    }

                    //des_slice[-1,0,0], source_slice=[0,-1]
                    //des_slice[-1,0,0], source_slice=[-1,0]
                    if (des_slice[0] == -1 && des_slice[1] >= 0 && des_slice[2] >= 0) {
                        if (source_slice[0] >= 0 && source_slice[1] == -1) {
                            if (destination->dimensions[0] != source->dimensions[1] || 
                                des_slice[1] >= destination->dimensions[1] || 
                                des_slice[2] >= destination->dimensions[2] || 
                                source_slice[0] >= source->dimensions[0]) 
                            {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[0]; i++) {
                                set_element_a(destination,(size_t[]){i, des_slice[1], des_slice[2]}, get_element_a(source, (size_t[]){source_slice[0],i}));
                            }
                            break;
                        }
                        if (source_slice[0] == -1 && source_slice[1] >= 0) {
                            if (destination->dimensions[0] != source->dimensions[0] || des_slice[1] >= destination->dimensions[1] || des_slice[2] >= destination->dimensions[2] || source_slice[1] >= source->dimensions[1]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[0]; i++) {
                                set_element_a(destination,(size_t[]){i, des_slice[1], des_slice[2]}, get_element_a(source, (size_t[]){i,source_slice[1]}));
                            }
                            break;
                        }
                        log_incompatible_slice(__FILE__, __LINE__);
                        break;
                    }

                    log_incompatible_slice(__FILE__, __LINE__);
                    break;
                }
                case 3: {
                    //des_slice[-1,-1,-1], source_slice=[-1,-1,-1]
                    if (des_slice[0] == -1 && des_slice[1] == -1 && des_slice[2] == -1) {
                        //Checking if both arrays are of the same dimensions.
                        if (destination->dimensions[0] != source->dimensions[0] || destination->dimensions[1] != source->dimensions[1] || destination->dimensions[2] != source->dimensions[2]) {
                            log_incompatible_dimensions(__FILE__, __LINE__);
                        }

                        switch (*destination->type) {
                            case DOUBLE: {
                                double* des_arr = destination->array;
                                double* source_arr = source->array;
                                for (size_t i = 0; i < get_size_a(destination); i++){
                                    des_arr[i] = source_arr[i];
                                }
                                break;
                            }
                            case INT: {
                                int* des_arr = destination->array;
                                int* source_arr = source->array;
                                for (size_t i = 0; i < get_size_a(destination); i++){
                                    des_arr[i] = source_arr[i];
                                }
                                break;
                            }
                            case FLOAT: {
                                float* des_arr = destination->array;
                                float* source_arr = source->array;
                                for (size_t i = 0; i < get_size_a(destination); i++){
                                    des_arr[i] = source_arr[i];
                                }
                                break;
                            }
                            case COMPLEX: {
                                double complex* des_arr = destination->array;
                                double complex* source_arr = source->array;
                                for (size_t i = 0; i < get_size_a(destination); i++){
                                    des_arr[i] = source_arr[i];
                                }
                                break;
                            }
                            default: {
                                log_invalid_type(__FILE__, __LINE__);
                            }
                        }
                        break;
                    }
                    
                    //des_slice[-1,-1,0], source_slice=[-1,-1,0]
                    //des_slice[-1,-1,0], source_slice=[-1,0,-1]
                    //des_slice[-1,-1,0], source_slice=[0,-1,-1]
                    if (des_slice[0] == -1 && des_slice[1] == -1 && des_slice[2] >= 0) {
                        if (source_slice[0] == -1 && source_slice[1] == -1 && source_slice[2] >= 0) {
                            if (destination->dimensions[0] != source->dimensions[0] || destination->dimensions[1] != source->dimensions[1] || des_slice[2] >= destination->dimensions[2] || source_slice[2] >= source->dimensions[2]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }

                            for (int i = 0; i < destination->dimensions[0]; i++) {
                                for (int j = 0; j < destination->dimensions[1]; j++) {
                                    set_element_a(destination,(size_t[]){i, j, des_slice[2]}, get_element_a(source, (size_t[]){i,j, source_slice[2]}));
                                }
                            }
                            break;
                        }
                        if (source_slice[0] == -1 && source_slice[1] >= 0 && source_slice[2] == -1) {
                            if (destination->dimensions[0] != source->dimensions[0] || destination->dimensions[1] != source->dimensions[2] || des_slice[2] >= destination->dimensions[2] || source_slice[1] >= source->dimensions[1]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }

                            for (int i = 0; i < destination->dimensions[0]; i++) {
                                for (int j = 0; j < destination->dimensions[1]; j++) {
                                    set_element_a(destination,(size_t[]){i, j, des_slice[2]}, get_element_a(source, (size_t[]){i,source_slice[1],j}));
                                }
                            }
                            break;
                        }
                        if (source_slice[0] >= 0 && source_slice[1] == -1 && source_slice[2] == -1) {
                            if (destination->dimensions[0] != source->dimensions[1] || destination->dimensions[1] != source->dimensions[2] || des_slice[2] >= destination->dimensions[2] || source_slice[0] >= source->dimensions[0]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }

                            for (int i = 0; i < destination->dimensions[0]; i++) {
                                for (int j = 0; j < destination->dimensions[1]; j++) {
                                    set_element_a(destination,(size_t[]){i, j, des_slice[2]}, get_element_a(source, (size_t[]){source_slice[0],i,j}));
                                }
                            }
                            break;
                        }
                        log_incompatible_slice(__FILE__, __LINE__);
                        break;
                    }

                    //des_slice[-1,0,-1], source_slice=[-1,-1,0]
                    //des_slice[-1,0,-1], source_slice=[-1,0,-1]
                    //des_slice[-1,0,-1], source_slice=[0,-1,-1]
                    if (des_slice[0] == -1 && des_slice[1] >= 0 && des_slice[2] == -1) {
                        if (source_slice[0] == -1 && source_slice[1] == -1 && source_slice[2] >= 0) {
                            if (destination->dimensions[0] != source->dimensions[0] || destination->dimensions[2] != source->dimensions[1] || des_slice[1] >= destination->dimensions[1] || source_slice[2] >= source->dimensions[2]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }

                            for (int i = 0; i < destination->dimensions[0]; i++) {
                                for (int j = 0; j < destination->dimensions[2]; j++) {
                                    set_element_a(destination,(size_t[]){i, des_slice[1], j}, get_element_a(source, (size_t[]){i,j, source_slice[2]}));
                                }
                            }
                            break;
                        }
                        if (source_slice[0] == -1 && source_slice[1] >= 0 && source_slice[2] == -1) {
                            if (destination->dimensions[0] != source->dimensions[0] || destination->dimensions[2] != source->dimensions[2] || des_slice[1] >= destination->dimensions[1] || source_slice[1] >= source->dimensions[1]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }

                            for (int i = 0; i < destination->dimensions[0]; i++) {
                                for (int j = 0; j < destination->dimensions[2]; j++) {
                                    set_element_a(destination,(size_t[]){i, des_slice[1], j}, get_element_a(source, (size_t[]){i,source_slice[1],j}));
                                }
                            }
                            break;
                        }
                        if (source_slice[0] >= 0 && source_slice[1] == -1 && source_slice[2] == -1) {
                            if (destination->dimensions[0] != source->dimensions[1] || destination->dimensions[2] != source->dimensions[2] || des_slice[1] >= destination->dimensions[1] || source_slice[0] >= source->dimensions[0]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }

                            for (int i = 0; i < destination->dimensions[0]; i++) {
                                for (int j = 0; j < destination->dimensions[2]; j++) {
                                    set_element_a(destination,(size_t[]){i, des_slice[1], j}, get_element_a(source, (size_t[]){source_slice[0],i,j}));
                                }
                            }
                            break;
                        }
                        log_incompatible_slice(__FILE__, __LINE__);
                        break;
                    }

                    //des_slice[0,-1,-1], source_slice=[-1,-1,0]
                    //des_slice[0,-1,-1], source_slice=[-1,0,-1]
                    //des_slice[0,-1,-1], source_slice=[0,-1,-1]
                    if (des_slice[0] >= 0 && des_slice[1] == -1 && des_slice[2] == -1) {
                        if (source_slice[0] == -1 && source_slice[1] == -1 && source_slice[2] >= 0) {
                            if (destination->dimensions[1] != source->dimensions[0] || destination->dimensions[2] != source->dimensions[1] || des_slice[0] >= destination->dimensions[0] || source_slice[2] >= source->dimensions[2]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }

                            for (int i = 0; i < destination->dimensions[1]; i++) {
                                for (int j = 0; j < destination->dimensions[2]; j++) {
                                    set_element_a(destination,(size_t[]){des_slice[0],i, j}, get_element_a(source, (size_t[]){i,j,source_slice[2]}));
                                }
                            }
                            break;
                        }
                        if (source_slice[0] == -1 && source_slice[1] >= 0 && source_slice[2] == -1) {
                            if (destination->dimensions[1] != source->dimensions[0] || destination->dimensions[2] != source->dimensions[2] || des_slice[0] >= destination->dimensions[0] || source_slice[1] >= source->dimensions[1]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }

                            for (int i = 0; i < destination->dimensions[1]; i++) {
                                for (int j = 0; j < destination->dimensions[2]; j++) {
                                    set_element_a(destination,(size_t[]){des_slice[0],i, j}, get_element_a(source, (size_t[]){i,source_slice[1],j}));
                                }
                            }
                            break;
                        }
                        if (source_slice[0] >= 0 && source_slice[1] == -1 && source_slice[2] == -1) {
                            if (destination->dimensions[1] != source->dimensions[1] || destination->dimensions[2] != source->dimensions[2] || des_slice[0] >= destination->dimensions[0] || source_slice[0] >= source->dimensions[0]) {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }

                            for (int i = 0; i < destination->dimensions[1]; i++) {
                                for (int j = 0; j < destination->dimensions[2]; j++) {
                                    set_element_a(destination,(size_t[]){des_slice[0],i, j}, get_element_a(source, (size_t[]){source_slice[0],i,j}));
                                }
                            }
                            break;
                        }
                        log_incompatible_slice(__FILE__, __LINE__);
                        break;
                    }

                    //des_slice[0,0,-1], source_slice=[0,-1,0]
                    //des_slice[0,0,-1], source_slice=[-1,0,0]
                    //des_slice[0,0,-1], source_slice=[0,0,-1]
                    if (des_slice[0] >= 0 && des_slice[1] >= 0 && des_slice[2] == -1) {
                        if (source_slice[0] >= 0 && source_slice[1] == -1 && source_slice[2] >= 0) {
                            if (destination->dimensions[2] != source->dimensions[1] || 
                                des_slice[0] >= destination->dimensions[0] || 
                                des_slice[1] >= destination->dimensions[1] || 
                                source_slice[0] >= source->dimensions[0] || 
                                source_slice[2] >= source->dimensions[2]) 
                            {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[2]; i++) {
                                set_element_a(destination,(size_t[]){des_slice[0], des_slice[1], i}, get_element_a(source, (size_t[]){source_slice[0],i,source_slice[2]}));
                            }
                            break;
                        }
                        if (source_slice[0] == -1 && source_slice[1] >= 0 && source_slice[2] >= 0) {
                            if (destination->dimensions[2] != source->dimensions[0] || 
                                des_slice[0] >= destination->dimensions[0] || 
                                des_slice[1] >= destination->dimensions[1] || 
                                source_slice[1] >= source->dimensions[1] || 
                                source_slice[2] >= source->dimensions[2]) 
                            {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[2]; i++) {
                                set_element_a(destination,(size_t[]){des_slice[0], des_slice[1], i}, get_element_a(source, (size_t[]){i,source_slice[1],source_slice[2]}));
                            }
                            break;
                        }
                        if (source_slice[0] >= 0 && source_slice[1] >= 0 && source_slice[2] == -1) {
                            if (destination->dimensions[2] != source->dimensions[2] || 
                                des_slice[0] >= destination->dimensions[0] || 
                                des_slice[1] >= destination->dimensions[1] || 
                                source_slice[0] >= source->dimensions[0] || 
                                source_slice[1] >= source->dimensions[1]) 
                            {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[2]; i++) {
                                set_element_a(destination,(size_t[]){des_slice[0], des_slice[1], i}, get_element_a(source, (size_t[]){source_slice[0],source_slice[1],i}));
                            }
                            break;
                        }
                        log_incompatible_slice(__FILE__, __LINE__);
                        break;
                    }

                    //des_slice[0,-1,0], source_slice=[0,-1,0]
                    //des_slice[0,-1,0], source_slice=[-1,0,0]
                    //des_slice[0,-1,0], source_slice=[0,0,-1]
                    if (des_slice[0] >= 0 && des_slice[1] == -1 && des_slice[2] >= 0) {
                        if (source_slice[0] >= 0 && source_slice[1] == -1 && source_slice[2] >= 0) {
                            if (destination->dimensions[1] != source->dimensions[1] || 
                                des_slice[0] >= destination->dimensions[0] || 
                                des_slice[2] >= destination->dimensions[2] || 
                                source_slice[0] >= source->dimensions[0] || 
                                source_slice[2] >= source->dimensions[2]) 
                            {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[1]; i++) {
                                set_element_a(destination,(size_t[]){des_slice[0], i, des_slice[2]}, get_element_a(source, (size_t[]){source_slice[0],i,source_slice[2]}));
                            }
                            break;
                        }
                        if (source_slice[0] == -1 && source_slice[1] >= 0 && source_slice[2] >= 0) {
                            if (destination->dimensions[1] != source->dimensions[0] || 
                                des_slice[0] >= destination->dimensions[0] || 
                                des_slice[2] >= destination->dimensions[2] || 
                                source_slice[1] >= source->dimensions[1] || 
                                source_slice[2] >= source->dimensions[2]) 
                            {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[1]; i++) {
                                set_element_a(destination,(size_t[]){des_slice[0], i, des_slice[2]}, get_element_a(source, (size_t[]){i,source_slice[1],source_slice[2]}));
                            }
                            break;
                        }
                        if (source_slice[0] >= 0 && source_slice[1] >= 0 && source_slice[2] == -1) {
                            if (destination->dimensions[1] != source->dimensions[2] || 
                                des_slice[0] >= destination->dimensions[0] || 
                                des_slice[2] >= destination->dimensions[2] || 
                                source_slice[0] >= source->dimensions[0] || 
                                source_slice[1] >= source->dimensions[2]) 
                            {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[1]; i++) {
                                set_element_a(destination,(size_t[]){des_slice[0], i, des_slice[2]}, get_element_a(source, (size_t[]){source_slice[0],source_slice[1],i}));
                            }
                            break;
                        }
                        log_incompatible_slice(__FILE__, __LINE__);
                        break;
                    }

                    //des_slice[-1,0,0], source_slice=[0,-1,0]
                    //des_slice[-1,0,0], source_slice=[-1,0,0]
                    //des_slice[-1,0,0], source_slice=[0,0,-1]
                    if (des_slice[0] == -1 && des_slice[1] >= 0 && des_slice[2] >= 0) {
                        if (source_slice[0] >= 0 && source_slice[1] == -1 && source_slice[2] >= 0) {
                            if (destination->dimensions[0] != source->dimensions[1] || 
                                des_slice[1] >= destination->dimensions[1] || 
                                des_slice[2] >= destination->dimensions[2] || 
                                source_slice[0] >= source->dimensions[0] || 
                                source_slice[2] >= source->dimensions[2]) 
                            {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[0]; i++) {
                                set_element_a(destination,(size_t[]){i,des_slice[1], des_slice[2]}, get_element_a(source, (size_t[]){source_slice[0],i,source_slice[2]}));
                            }
                            break;
                        }
                        if (source_slice[0] == -1 && source_slice[1] >= 0 && source_slice[2] >= 0) {
                            if (destination->dimensions[0] != source->dimensions[0] || 
                                des_slice[1] >= destination->dimensions[1] || 
                                des_slice[2] >= destination->dimensions[2] || 
                                source_slice[1] >= source->dimensions[1] || 
                                source_slice[2] >= source->dimensions[2]) 
                            {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[0]; i++) {
                                set_element_a(destination,(size_t[]){i,des_slice[1], des_slice[2]}, get_element_a(source, (size_t[]){i,source_slice[1],source_slice[2]}));
                            }
                            break;
                        }
                        if (source_slice[0] >= 0 && source_slice[1] >= 0 && source_slice[2] == -1) {
                            if (destination->dimensions[0] != source->dimensions[2] || 
                                des_slice[1] >= destination->dimensions[1] || 
                                des_slice[2] >= destination->dimensions[2] || 
                                source_slice[0] >= source->dimensions[0] || 
                                source_slice[1] >= source->dimensions[1]) 
                            {
                                log_incompatible_slice(__FILE__, __LINE__);
                                break;
                            }
                            for (int i = 0; i < destination->dimensions[0]; i++) {
                                set_element_a(destination,(size_t[]){i,des_slice[1], des_slice[2]}, get_element_a(source, (size_t[]){source_slice[0],source_slice[1],i}));
                            }
                            break;
                        }
                        log_incompatible_slice(__FILE__, __LINE__);
                        break;
                    }

                    log_incompatible_slice(__FILE__, __LINE__);
                    break;
                }
                default: {
                    log_invalid_dimensions(__FILE__, __LINE__);
                    break;
                }
            }
            break;
        }
        default: {
            log_invalid_dimensions(__FILE__, __LINE__);
            break;
        }
    }
}

/**
 * Validating a dimention
*/
void validate_dimension_a(size_t dimension, const char* file, int line) {
    if (dimension == 0) {
        log_invalid_dimensions(file, line);
    }
}

/**
 * This function gives the dot product of two arrays
 * with different dimensions through slicing
*/
struct array* dot_a(struct array* arr_1, struct array* arr_2) {

    switch(*arr_1->num_dimensions) {
        case 1: {
            switch(*arr_2->num_dimensions) {
                case 1: {
                    if (arr_1->dimensions[0] != arr_2->dimensions[0]) {
                        log_incompatible_dimensions(__FILE__, __LINE__);
                    }

                    //operation
                    switch (*arr_1->type) {
                        case DOUBLE: {
                            switch (*arr_2->type) {
                                case DOUBLE: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){DOUBLE});
                                    double* arr_1_double = arr_1->array;
                                    double* arr_2_double = arr_2->array;
                                    double sum = 0.0;
                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        sum = sum + (arr_1_double[i] * arr_2_double[i]);
                                    }
                                    set_element_a(dot_product, (size_t[]){0}, &sum);

                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){COMPLEX});
                                    double* arr_1_double = arr_1->array;
                                    double complex* arr_2_complex = arr_2->array;
                                    double complex sum = 0.0 + 0.0*I;
                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        sum = sum + (arr_1_double[i] * arr_2_complex[i]);
                                    }
                                    set_element_a(dot_product, (size_t[]){0}, &sum);

                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                            
                        }
                        case INT: {
                            switch (*arr_2->type) {
                                case INT: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){INT});
                                    int* arr_1_int = arr_1->array;
                                    int* arr_2_int = arr_2->array;
                                    int sum = 0;
                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        sum = sum + (arr_1_int[i] * arr_2_int[i]);
                                    }
                                    set_element_a(dot_product, (size_t[]){0}, &sum);
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){COMPLEX});
                                    int* arr_1_int = arr_1->array;
                                    double complex* arr_2_complex = arr_2->array;
                                    double complex sum = 0;
                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        sum = sum + (arr_1_int[i] * arr_2_complex[i]);
                                    }
                                    set_element_a(dot_product, (size_t[]){0}, &sum);
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                        }
                        case FLOAT: {
                            switch (*arr_2->type) {
                                case FLOAT: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){FLOAT});
                                    float* arr_1_float = arr_1->array;
                                    float* arr_2_float = arr_2->array;
                                    float sum = 0.0;
                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        sum = sum + (arr_1_float[i] * arr_2_float[i]);
                                    }
                                    set_element_a(dot_product, (size_t[]){0}, &sum);
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){COMPLEX});
                                    float* arr_1_float = arr_1->array;
                                    double complex* arr_2_complex = arr_2->array;
                                    double complex sum = 0.0;
                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        sum = sum + (arr_1_float[i] * arr_2_complex[i]);
                                    }
                                    set_element_a(dot_product, (size_t[]){0}, &sum);
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                        }
                        case COMPLEX: {
                            switch (*arr_2->type) {
                                case DOUBLE: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){COMPLEX});
                                    double complex* arr_1_complex = arr_1->array;
                                    double* arr_2_double = arr_2->array;
                                    double complex sum = 0 + 0*I;
                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        sum = sum + (arr_1_complex[i] * arr_2_double[i]);
                                    }
                                    set_element_a(dot_product, (size_t[]){0}, &sum);
                                    return dot_product;
                                }
                                case INT: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){COMPLEX});
                                    double complex* arr_1_complex = arr_1->array;
                                    int* arr_2_int = arr_2->array;
                                    double complex sum = 0 + 0*I;
                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        sum = sum + (arr_1_complex[i] * arr_2_int[i]);
                                    }
                                    set_element_a(dot_product, (size_t[]){0}, &sum);
                                    return dot_product;
                                }
                                case FLOAT: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){COMPLEX});
                                    double complex* arr_1_complex = arr_1->array;
                                    float* arr_2_float = arr_2->array;
                                    double complex sum = 0 + 0*I;
                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        sum = sum + (arr_1_complex[i] * arr_2_float[i]);
                                    }
                                    set_element_a(dot_product, (size_t[]){0}, &sum);
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){COMPLEX});
                                    double complex* arr_1_complex = arr_1->array;
                                    double complex* arr_2_complex = arr_2->array;
                                    double complex sum = 0 + 0*I;
                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        sum = sum + (arr_1_complex[i] * arr_2_complex[i]);
                                    }
                                    set_element_a(dot_product, (size_t[]){0}, &sum);
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                        }
                        default: {
                            log_invalid_type(__FILE__, __LINE__);
                        }
                    }
                    break;
                }
                case 2: {
                    if (arr_1->dimensions[0] != arr_2->dimensions[0]) {
                        log_incompatible_dimensions(__FILE__, __LINE__);
                    }

                    switch (*arr_1->type) {
                        case DOUBLE: {
                            switch (*arr_2->type) {
                                case DOUBLE: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_2->dimensions[1]}, &(enum DataType){DOUBLE});
                                    double* arr_1_double = arr_1->array;
                                    double sum = 0.0;

                                    for (int i = 0; i < arr_2->dimensions[1]; i++) {
                                        for (int j = 0; j < arr_2->dimensions[0]; j++) {
                                            sum = sum + (arr_1_double[j] * *(double*)get_element_a(arr_2, (size_t[]){j,i}));
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0.0;
                                    }
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    double* arr_1_double = arr_1->array;
                                    double complex sum = 0.0;

                                    for (int i = 0; i < arr_2->dimensions[1]; i++) {
                                        for (int j = 0; j < arr_2->dimensions[0]; j++) {
                                            sum = sum + (arr_1_double[j] * *(double complex*)get_element_a(arr_2, (size_t[]){j,i}));
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0.0;
                                    }
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                        }
                        case INT: {
                            switch (*arr_2->type) {
                                case INT: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_2->dimensions[1]}, &(enum DataType){INT});
                                    int* arr_1_int = arr_1->array;
                                    int sum = 0;

                                    for (int i = 0; i < arr_2->dimensions[1]; i++) {
                                        for (int j = 0; j < arr_2->dimensions[0]; j++) {
                                            sum = sum + (arr_1_int[j] * *(int*)get_element_a(arr_2, (size_t[]){j,i}));
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0;
                                    }
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    int* arr_1_int = arr_1->array;
                                    double complex sum = 0;

                                    for (int i = 0; i < arr_2->dimensions[1]; i++) {
                                        for (int j = 0; j < arr_2->dimensions[0]; j++) {
                                            sum = sum + (arr_1_int[j] * *(double complex*)get_element_a(arr_2, (size_t[]){j,i}));
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0;
                                    }
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                        }
                        case FLOAT: {
                            switch (*arr_2->type) {
                                case FLOAT: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_2->dimensions[1]}, &(enum DataType){FLOAT});
                                    float* arr_1_float = arr_1->array;
                                    float sum = 0.0;

                                    for (int i = 0; i < arr_2->dimensions[1]; i++) {
                                        for (int j = 0; j < arr_2->dimensions[0]; j++) {
                                            sum = sum + (arr_1_float[j] * *(float*)get_element_a(arr_2, (size_t[]){j,i}));
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0.0;
                                    }
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    float* arr_1_float = arr_1->array;
                                    double complex sum = 0.0;

                                    for (int i = 0; i < arr_2->dimensions[1]; i++) {
                                        for (int j = 0; j < arr_2->dimensions[0]; j++) {
                                            sum = sum + (arr_1_float[j] * *(double complex*)get_element_a(arr_2, (size_t[]){j,i}));
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0.0;
                                    }
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                        }
                        case COMPLEX: {
                            switch (*arr_2->type) {
                                case DOUBLE: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    double complex* arr_1_complex = arr_1->array;
                                    double complex sum = 0 + 0*I;

                                    for (int i = 0; i < arr_2->dimensions[1]; i++) {
                                        for (int j = 0; j < arr_2->dimensions[0]; j++) {
                                            sum = sum + (arr_1_complex[j] * *(double*)get_element_a(arr_2, (size_t[]){j,i}));
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0 + 0*I;
                                    }
                                    return dot_product;
                                }
                                case INT: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    double complex* arr_1_complex = arr_1->array;
                                    double complex sum = 0 + 0*I;

                                    for (int i = 0; i < arr_2->dimensions[1]; i++) {
                                        for (int j = 0; j < arr_2->dimensions[0]; j++) {
                                            sum = sum + (arr_1_complex[j] * *(int*)get_element_a(arr_2, (size_t[]){j,i}));
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0 + 0*I;
                                    }
                                    return dot_product;
                                }
                                case FLOAT: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    double complex* arr_1_complex = arr_1->array;
                                    double complex sum = 0 + 0*I;

                                    for (int i = 0; i < arr_2->dimensions[1]; i++) {
                                        for (int j = 0; j < arr_2->dimensions[0]; j++) {
                                            sum = sum + (arr_1_complex[j] * *(float*)get_element_a(arr_2, (size_t[]){j,i}));
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0 + 0*I;
                                    }
                                    return dot_product;                                    
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    double complex* arr_1_complex = arr_1->array;
                                    double complex sum = 0 + 0*I;

                                    for (int i = 0; i < arr_2->dimensions[1]; i++) {
                                        for (int j = 0; j < arr_2->dimensions[0]; j++) {
                                            sum = sum + (arr_1_complex[j] * *(double complex*)get_element_a(arr_2, (size_t[]){j,i}));
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0 + 0*I;
                                    }
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                        }
                        default: {
                            log_invalid_type(__FILE__, __LINE__);
                        }
                    }

                    //operation
                    break;
                }
                default: {
                    log_invalid_dimensions(__FILE__, __LINE__);
                }
            }
            log_invalid_dimensions(__FILE__, __LINE__);
            break;
        }
        case 2: {
            switch(*arr_2->num_dimensions) {
                case 1: {
                    if (arr_1->dimensions[1] != arr_2->dimensions[0]) {
                        log_incompatible_dimensions(__FILE__, __LINE__);
                    }
                    //operation
                    switch (*arr_1->type) {
                        case DOUBLE: {
                            switch (*arr_2->type) {
                                case DOUBLE: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_1->dimensions[0]}, &(enum DataType){DOUBLE});
                                    double* arr_2_double = arr_2->array;
                                    double sum = 0.0;

                                    for (int i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (int j = 0; j < arr_1->dimensions[1]; j++) {
                                            sum = sum + (*(double*)get_element_a(arr_1, (size_t[]){i,j}) * arr_2_double[j]);
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0.0;
                                    }
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_1->dimensions[0]}, &(enum DataType){COMPLEX});
                                    double complex* arr_2_complex = arr_2->array;
                                    double complex sum = 0.0 + 0.0*I;

                                    for (int i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (int j = 0; j < arr_1->dimensions[1]; j++) {
                                            sum = sum + (*(double*)get_element_a(arr_1, (size_t[]){i,j}) * arr_2_complex[j]);
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0.0 + 0.0*I;
                                    }
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                        }
                        case INT: {
                            switch (*arr_2->type) {
                                case INT: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_1->dimensions[0]}, &(enum DataType){INT});
                                    int* arr_2_int = arr_2->array;
                                    int sum = 0;

                                    for (int i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (int j = 0; j < arr_1->dimensions[1]; j++) {
                                            sum = sum + (*(int*)get_element_a(arr_1, (size_t[]){i,j}) * arr_2_int[j]);
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0;
                                    }
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_1->dimensions[0]}, &(enum DataType){COMPLEX});
                                    double complex* arr_2_complex = arr_2->array;
                                    double complex sum = 0 + 0*I;

                                    for (int i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (int j = 0; j < arr_1->dimensions[1]; j++) {
                                            sum = sum + (*(int*)get_element_a(arr_1, (size_t[]){i,j}) * arr_2_complex[j]);
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0 + 0*I;
                                    }
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }
                        }
                        case FLOAT: {
                            switch (*arr_2->type) {
                                case FLOAT: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_1->dimensions[0]}, &(enum DataType){FLOAT});
                                    float* arr_2_float = arr_2->array;
                                    float sum = 0.0;

                                    for (int i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (int j = 0; j < arr_1->dimensions[1]; j++) {
                                            sum = sum + (*(float*)get_element_a(arr_1, (size_t[]){i,j}) * arr_2_float[j]);
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0.0;
                                    }
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_1->dimensions[0]}, &(enum DataType){COMPLEX});
                                    double complex* arr_2_complex = arr_2->array;
                                    double complex sum = 0.0 + 0.0*I;

                                    for (int i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (int j = 0; j < arr_1->dimensions[1]; j++) {
                                            sum = sum + (*(float*)get_element_a(arr_1, (size_t[]){i,j}) * arr_2_complex[j]);
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0.0 + 0.0*I;
                                    }
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                        }
                        case COMPLEX: {
                            switch (*arr_2->type) {
                                case DOUBLE: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_1->dimensions[0]}, &(enum DataType){COMPLEX});
                                    double* arr_2_double = arr_2->array;
                                    double complex sum = 0 + 0*I;

                                    for (int i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (int j = 0; j < arr_1->dimensions[1]; j++) {
                                            sum = sum + (*(double complex*)get_element_a(arr_1, (size_t[]){i,j}) * arr_2_double[j]);
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0 + 0*I;
                                    }
                                    return dot_product;
                                }
                                case INT: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_1->dimensions[0]}, &(enum DataType){COMPLEX});
                                    int* arr_2_int = arr_2->array;
                                    double complex sum = 0 + 0*I;

                                    for (int i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (int j = 0; j < arr_1->dimensions[1]; j++) {
                                            sum = sum + (*(double complex*)get_element_a(arr_1, (size_t[]){i,j}) * arr_2_int[j]);
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0 + 0*I;
                                    }
                                    return dot_product;
                                }
                                case FLOAT: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_1->dimensions[0]}, &(enum DataType){COMPLEX});
                                    float* arr_2_float = arr_2->array;
                                    double complex sum = 0 + 0*I;

                                    for (int i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (int j = 0; j < arr_1->dimensions[1]; j++) {
                                            sum = sum + (*(double complex*)get_element_a(arr_1, (size_t[]){i,j}) * arr_2_float[j]);
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0 + 0*I;
                                    }
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){1}, (size_t[]){arr_1->dimensions[0]}, &(enum DataType){COMPLEX});
                                    double complex* arr_2_complex = arr_2->array;
                                    double complex sum = 0 + 0*I;

                                    for (int i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (int j = 0; j < arr_1->dimensions[1]; j++) {
                                            sum = sum + (*(double complex*)get_element_a(arr_1, (size_t[]){i,j}) * arr_2_complex[j]);
                                        }
                                        set_element_a(dot_product, (size_t[]){i}, &sum);
                                        sum = 0 + 0*I;
                                    }
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }
                        }
                        default: {
                            log_invalid_type(__FILE__, __LINE__);
                        }
                    }
                    break;
                }
                case 2: {
                    if (arr_1->dimensions[1] != arr_2->dimensions[0]) {
                        log_incompatible_dimensions(__FILE__, __LINE__);
                    }

                    //operation
                    switch (*arr_1->type) {
                        case DOUBLE: {
                            switch (*arr_2->type) {
                                case DOUBLE: {
                                    struct array* dot_product = zeros_a(&(size_t){2}, (size_t[]){arr_1->dimensions[0], arr_2->dimensions[1]}, &(enum DataType){DOUBLE});
                                    double sum = 0.0;

                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (size_t j = 0; j < arr_2->dimensions[1]; j++) {
                                            for (size_t h = 0; h < arr_1->dimensions[1]; h++) {
                                                sum = sum + (*(double*)get_element_a(arr_1, (size_t[]){i,h}) * *(double*)get_element_a(arr_2, (size_t[]){h,j}));
                                            }
                                            set_element_a(dot_product, (size_t[]){i,j}, &sum);
                                            sum = 0.0;
                                        }
                                    }
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){2}, (size_t[]){arr_1->dimensions[0], arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    double complex sum = 0.0 + 0.0*I;

                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (size_t j = 0; j < arr_2->dimensions[1]; j++) {
                                            for (size_t h = 0; h < arr_1->dimensions[1]; h++) {
                                                sum = sum + (*(double*)get_element_a(arr_1, (size_t[]){i,h}) * *(double complex*)get_element_a(arr_2, (size_t[]){h,j}));
                                            }
                                            set_element_a(dot_product, (size_t[]){i,j}, &sum);
                                            sum = 0.0 + 0.0*I;
                                        }
                                    }
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                        }
                        case INT: {
                            switch (*arr_2->type) {
                                case INT: {
                                    struct array* dot_product = zeros_a(&(size_t){2}, (size_t[]){arr_1->dimensions[0], arr_2->dimensions[1]}, &(enum DataType){INT});
                                    int sum = 0;

                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (size_t j = 0; j < arr_2->dimensions[1]; j++) {
                                            for (size_t h = 0; h < arr_1->dimensions[1]; h++) {
                                                sum = sum + (*(int*)get_element_a(arr_1, (size_t[]){i,h}) * *(int*)get_element_a(arr_2, (size_t[]){h,j}));
                                            }
                                            set_element_a(dot_product, (size_t[]){i,j}, &sum);
                                            sum = 0;
                                        }
                                    }
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){2}, (size_t[]){arr_1->dimensions[0], arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    double complex sum = 0 + 0*I;

                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (size_t j = 0; j < arr_2->dimensions[1]; j++) {
                                            for (size_t h = 0; h < arr_1->dimensions[1]; h++) {
                                                sum = sum + (*(int*)get_element_a(arr_1, (size_t[]){i,h}) * *(double complex*)get_element_a(arr_2, (size_t[]){h,j}));
                                            }
                                            set_element_a(dot_product, (size_t[]){i,j}, &sum);
                                            sum = 0 + 0*I;
                                        }
                                    }
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                        }
                        case FLOAT: {
                            switch (*arr_2->type) {
                                case FLOAT: {
                                    struct array* dot_product = zeros_a(&(size_t){2}, (size_t[]){arr_1->dimensions[0], arr_2->dimensions[1]}, &(enum DataType){FLOAT});
                                    float sum = 0.0;

                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (size_t j = 0; j < arr_2->dimensions[1]; j++) {
                                            for (size_t h = 0; h < arr_1->dimensions[1]; h++) {
                                                sum = sum + (*(float*)get_element_a(arr_1, (size_t[]){i,h}) * *(float*)get_element_a(arr_2, (size_t[]){h,j}));
                                            }
                                            set_element_a(dot_product, (size_t[]){i,j}, &sum);
                                            sum = 0.0;
                                        }
                                    }
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){2}, (size_t[]){arr_1->dimensions[0], arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    double complex sum = 0.0 + 0.0*I;

                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (size_t j = 0; j < arr_2->dimensions[1]; j++) {
                                            for (size_t h = 0; h < arr_1->dimensions[1]; h++) {
                                                sum = sum + (*(float*)get_element_a(arr_1, (size_t[]){i,h}) * *(double complex*)get_element_a(arr_2, (size_t[]){h,j}));
                                            }
                                            set_element_a(dot_product, (size_t[]){i,j}, &sum);
                                            sum = 0.0 + 0.0*I;
                                        }
                                    }
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                        }
                        case COMPLEX: {
                            switch (*arr_2->type) {
                                case DOUBLE: {
                                    struct array* dot_product = zeros_a(&(size_t){2}, (size_t[]){arr_1->dimensions[0], arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    double complex sum = 0 + 0*I;

                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (size_t j = 0; j < arr_2->dimensions[1]; j++) {
                                            for (size_t h = 0; h < arr_1->dimensions[1]; h++) {
                                                sum = sum + (*(double complex*)get_element_a(arr_1, (size_t[]){i,h}) * *(double*)get_element_a(arr_2, (size_t[]){h,j}));
                                            }
                                            set_element_a(dot_product, (size_t[]){i,j}, &sum);
                                            sum = 0 + 0*I;
                                        }
                                    }
                                    return dot_product;
                                }
                                case INT: {
                                    struct array* dot_product = zeros_a(&(size_t){2}, (size_t[]){arr_1->dimensions[0], arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    double complex sum = 0 + 0*I;

                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (size_t j = 0; j < arr_2->dimensions[1]; j++) {
                                            for (size_t h = 0; h < arr_1->dimensions[1]; h++) {
                                                sum = sum + (*(double complex*)get_element_a(arr_1, (size_t[]){i,h}) * *(int*)get_element_a(arr_2, (size_t[]){h,j}));
                                            }
                                            set_element_a(dot_product, (size_t[]){i,j}, &sum);
                                            sum = 0 + 0*I;
                                        }
                                    }
                                    return dot_product;
                                }
                                case FLOAT: {
                                    struct array* dot_product = zeros_a(&(size_t){2}, (size_t[]){arr_1->dimensions[0], arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    double complex sum = 0 + 0*I;

                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (size_t j = 0; j < arr_2->dimensions[1]; j++) {
                                            for (size_t h = 0; h < arr_1->dimensions[1]; h++) {
                                                sum = sum + (*(double complex*)get_element_a(arr_1, (size_t[]){i,h}) * *(float*)get_element_a(arr_2, (size_t[]){h,j}));
                                            }
                                            set_element_a(dot_product, (size_t[]){i,j}, &sum);
                                            sum = 0 + 0*I;
                                        }
                                    }
                                    return dot_product;
                                }
                                case COMPLEX: {
                                    struct array* dot_product = zeros_a(&(size_t){2}, (size_t[]){arr_1->dimensions[0], arr_2->dimensions[1]}, &(enum DataType){COMPLEX});
                                    double complex sum = 0 + 0*I;

                                    for (size_t i = 0; i < arr_1->dimensions[0]; i++) {
                                        for (size_t j = 0; j < arr_2->dimensions[1]; j++) {
                                            for (size_t h = 0; h < arr_1->dimensions[1]; h++) {
                                                sum = sum + (*(double complex*)get_element_a(arr_1, (size_t[]){i,h}) * *(double complex*)get_element_a(arr_2, (size_t[]){h,j}));
                                            }
                                            set_element_a(dot_product, (size_t[]){i,j}, &sum);
                                            sum = 0 + 0*I;
                                        }
                                    }
                                    return dot_product;
                                }
                                default: {
                                    log_invalid_type(__FILE__, __LINE__);
                                }
                            }

                        }
                        default: {
                            log_invalid_type(__FILE__, __LINE__);
                        }
                    }
                    break;
                }
                default: {
                    log_invalid_dimensions(__FILE__, __LINE__);
                }
            }
            log_invalid_dimensions(__FILE__, __LINE__);
            break;
        }
        default: {
            log_invalid_dimensions(__FILE__, __LINE__);
        }
    }
    return NULL;
}

/**
 * Generates a diagonal matrix from an 1d array
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - array struct pointer
 *  - slice array 
*/
struct array* diag_a(struct array* array, int* slice) {
    switch (*array->num_dimensions) {
        case 1: {
            //Just make a diagonal matrix
            switch (*array->type) {
                case DOUBLE: {
                    struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[0]}, &(enum DataType){DOUBLE});
                    for (size_t i = 0; i < array->dimensions[0]; i++) {
                        set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){i}));
                    }
                    return result;
                }
                case INT: {
                    struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[0]}, &(enum DataType){INT});
                    for (size_t i = 0; i < array->dimensions[0]; i++) {
                        set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){i}));
                    }
                    return result;
                }
                case FLOAT: {
                    struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[0]}, &(enum DataType){FLOAT});
                    for (size_t i = 0; i < array->dimensions[0]; i++) {
                        set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){i}));
                    }
                    return result;
                }
                case COMPLEX: {
                    struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[0]}, &(enum DataType){COMPLEX});
                    for (size_t i = 0; i < array->dimensions[0]; i++) {
                        set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){i}));
                    }
                    return result;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            
            break;
        }
        case 2: {
            //check which one is -1
            //[-1,0]
            if (slice[0] == -1 && slice[1] >= 0) {
                switch (*array->type) {
                    case DOUBLE: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[0]}, &(enum DataType){DOUBLE});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){i, slice[1]}));
                        }
                        return result;
                    }
                    case INT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[0]}, &(enum DataType){INT});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){i, slice[1]}));
                        }
                        return result;
                    }
                    case FLOAT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[0]}, &(enum DataType){FLOAT});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){i, slice[1]}));
                        }
                        return result;
                    }
                    case COMPLEX: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[0]}, &(enum DataType){COMPLEX});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){i, slice[1]}));
                        }
                        return result;
                    }
                    default: {
                        log_invalid_type(__FILE__, __LINE__);
                    }
                }
                break;
            }
            //[0,-1]
            if (slice[0] >= 0 && slice[1] == -1) {
                switch (*array->type) {
                    case DOUBLE: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[1], array->dimensions[1]}, &(enum DataType){DOUBLE});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){slice[0],i}));
                        }
                        return result;
                    }
                    case INT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[1], array->dimensions[1]}, &(enum DataType){INT});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){slice[0],i}));
                        }
                        return result;
                    }
                    case FLOAT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[1], array->dimensions[1]}, &(enum DataType){FLOAT});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){slice[0],i}));
                        }
                        return result;
                    }
                    case COMPLEX: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[1], array->dimensions[1]}, &(enum DataType){COMPLEX});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){slice[0],i}));
                        }
                        return result;
                    }
                    default: {
                        log_invalid_type(__FILE__, __LINE__);
                    }
                }
                break;
            }
            log_incompatible_slice(__FILE__, __LINE__);
            break;
        }
        case 3: {
            //check which one is -1
            //[-1,0,0]
            if (slice[0] == -1 && slice[1] >= 0 && slice[2] >= 0) {
                switch (*array->type) {
                    case DOUBLE: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[0]}, &(enum DataType){DOUBLE});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){i, slice[1], slice[2]}));
                        }
                        return result;
                    }
                    case INT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[0]}, &(enum DataType){INT});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){i, slice[1], slice[2]}));
                        }
                        return result;
                    }
                    case FLOAT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[0]}, &(enum DataType){FLOAT});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){i, slice[1], slice[2]}));
                        }
                        return result;
                    }
                    case COMPLEX: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[0]}, &(enum DataType){COMPLEX});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){i, slice[1], slice[2]}));
                        }
                        return result;
                    }
                    default: {
                        log_invalid_type(__FILE__, __LINE__);
                    }
                }
            }
            //[0.-1,0]
            if (slice[0] >= 0 && slice[1] == -1 && slice[2] >= 0) {
                switch (*array->type) {
                    case DOUBLE: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[1], array->dimensions[1]}, &(enum DataType){DOUBLE});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){slice[0], i, slice[2]}));
                        }
                        return result;
                    }
                    case INT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[1], array->dimensions[1]}, &(enum DataType){INT});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){slice[0], i, slice[2]}));
                        }
                        return result;
                    }
                    case FLOAT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[1], array->dimensions[1]}, &(enum DataType){FLOAT});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){slice[0], i, slice[2]}));
                        }
                        return result;
                    }
                    case COMPLEX: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[1], array->dimensions[1]}, &(enum DataType){COMPLEX});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){slice[0], i, slice[2]}));
                        }
                        return result;
                    }
                    default: {
                        log_invalid_type(__FILE__, __LINE__);
                    }
                }
            }
            //[0,0,-1]
            if (slice[0] >= 0 && slice[1] >= 0 && slice[2] == -1) {
                switch (*array->type) {
                    case DOUBLE: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[2], array->dimensions[2]}, &(enum DataType){DOUBLE});
                        for (size_t i = 0; i < array->dimensions[2]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){slice[0], slice[1], i}));
                        }
                        return result;
                    }
                    case INT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[2], array->dimensions[2]}, &(enum DataType){INT});
                        for (size_t i = 0; i < array->dimensions[2]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){slice[0], slice[1], i}));
                        }
                        return result;
                    }
                    case FLOAT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[2], array->dimensions[2]}, &(enum DataType){FLOAT});
                        for (size_t i = 0; i < array->dimensions[2]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){slice[0], slice[1], i}));
                        }
                        return result;
                    }
                    case COMPLEX: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[2], array->dimensions[2]}, &(enum DataType){COMPLEX});
                        for (size_t i = 0; i < array->dimensions[2]; i++) {
                            set_element_a(result, (size_t[]){i,i}, get_element_a(array, (size_t[]){slice[0], slice[1], i}));
                        }
                        return result;
                    }
                    default: {
                        log_invalid_type(__FILE__, __LINE__);
                    }
                }
            }

            log_incompatible_slice(__FILE__, __LINE__);
            break;
        }
        default: {
            log_invalid_dimensions(__FILE__, __LINE__);
            break;
        }
    }
    return NULL;
}

/**
 * This will return a array struct pointer with the given slice parameters
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - array struct pointer
 *  - slice array 
*/
struct array* slice_a(struct array* array, int* slice) {
    switch (*array->num_dimensions) {
        case 2: {
            //check which one is -1
            //[-1,0]
            if (slice[0] == -1 && slice[1] >= 0) {
                switch (*array->type) {
                    case DOUBLE: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[0]}, &(enum DataType){DOUBLE});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){i, slice[1]}));
                        }
                        return result;
                    }
                    case INT: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[0]}, &(enum DataType){INT});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){i, slice[1]}));
                        }
                        return result;
                    }
                    case FLOAT: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[0]}, &(enum DataType){FLOAT});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){i, slice[1]}));
                        }
                        return result;
                    }
                    case COMPLEX: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[0]}, &(enum DataType){COMPLEX});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){i, slice[1]}));
                        }
                        return result;
                    }
                    default: {
                        log_invalid_type(__FILE__, __LINE__);
                    }
                }
                break;
            }
            //[0,-1]
            if (slice[0] >= 0 && slice[1] == -1) {
                switch (*array->type) {
                    case DOUBLE: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[1]}, &(enum DataType){DOUBLE});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){slice[0],i}));
                        }
                        return result;
                    }
                    case INT: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[1]}, &(enum DataType){INT});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){slice[0],i}));
                        }
                        return result;
                    }
                    case FLOAT: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[1]}, &(enum DataType){FLOAT});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){slice[0],i}));
                        }
                        return result;
                    }
                    case COMPLEX: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[1]}, &(enum DataType){COMPLEX});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){slice[0],i}));
                        }
                        return result;
                    }
                    default: {
                        log_invalid_type(__FILE__, __LINE__);
                    }
                }
                break;
            }
            log_incompatible_slice(__FILE__, __LINE__);
            break;
        }
        case 3: {
            //check which one is -1
            //[-1,0,0]
            if (slice[0] == -1 && slice[1] >= 0 && slice[2] >= 0) {
                switch (*array->type) {
                    case DOUBLE: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[0]}, &(enum DataType){DOUBLE});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){i, slice[1], slice[2]}));
                        }
                        return result;
                    }
                    case INT: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[0]}, &(enum DataType){INT});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){i, slice[1], slice[2]}));
                        }
                        return result;
                    }
                    case FLOAT: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[0]}, &(enum DataType){FLOAT});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){i, slice[1], slice[2]}));
                        }
                        return result;
                    }
                    case COMPLEX: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[0]}, &(enum DataType){COMPLEX});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){i, slice[1], slice[2]}));
                        }
                        return result;
                    }
                    default: {
                        log_invalid_type(__FILE__, __LINE__);
                    }
                }
            }
            //[0.-1,0]
            if (slice[0] >= 0 && slice[1] == -1 && slice[2] >= 0) {
                switch (*array->type) {
                    case DOUBLE: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[1]}, &(enum DataType){DOUBLE});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){slice[0], i, slice[2]}));
                        }
                        return result;
                    }
                    case INT: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[1]}, &(enum DataType){INT});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){slice[0], i, slice[2]}));
                        }
                        return result;
                    }
                    case FLOAT: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[1]}, &(enum DataType){FLOAT});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){slice[0], i, slice[2]}));
                        }
                        return result;
                    }
                    case COMPLEX: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[1]}, &(enum DataType){COMPLEX});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){slice[0], i, slice[2]}));
                        }
                        return result;
                    }
                    default: {
                        log_invalid_type(__FILE__, __LINE__);
                    }
                }
            }
            //[0,0,-1]
            if (slice[0] >= 0 && slice[1] >= 0 && slice[2] == -1) {
                switch (*array->type) {
                    case DOUBLE: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[2]}, &(enum DataType){DOUBLE});
                        for (size_t i = 0; i < array->dimensions[2]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){slice[0], slice[1], i}));
                        }
                        return result;
                    }
                    case INT: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[2]}, &(enum DataType){INT});
                        for (size_t i = 0; i < array->dimensions[2]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){slice[0], slice[1], i}));
                        }
                        return result;
                    }
                    case FLOAT: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[2]}, &(enum DataType){FLOAT});
                        for (size_t i = 0; i < array->dimensions[2]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){slice[0], slice[1], i}));
                        }
                        return result;
                    }
                    case COMPLEX: {
                        struct array* result = zeros_a(&(size_t){1}, (size_t[]){array->dimensions[2]}, &(enum DataType){COMPLEX});
                        for (size_t i = 0; i < array->dimensions[2]; i++) {
                            set_element_a(result, (size_t[]){i}, get_element_a(array, (size_t[]){slice[0], slice[1], i}));
                        }
                        return result;
                    }
                    default: {
                        log_invalid_type(__FILE__, __LINE__);
                    }
                }
            }

            //[-1,-1,0]
            if (slice[0] == -1 && slice[1] == -1 && slice[2] >= 0) {
                switch (*array->type) {
                    case DOUBLE: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[1]}, &(enum DataType){DOUBLE});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            for (size_t j = 0; j < array->dimensions[1]; j++) {
                                set_element_a(result, (size_t[]){i,j}, get_element_a(array, (size_t[]){i, j, slice[2]}));
                            }
                        }
                        return result;
                    }
                    case INT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[1]}, &(enum DataType){INT});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            for (size_t j = 0; j < array->dimensions[1]; j++) {
                                set_element_a(result, (size_t[]){i,j}, get_element_a(array, (size_t[]){i, j, slice[2]}));
                            }
                        }
                        return result;
                    }
                    case FLOAT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[1]}, &(enum DataType){FLOAT});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            for (size_t j = 0; j < array->dimensions[1]; j++) {
                                set_element_a(result, (size_t[]){i,j}, get_element_a(array, (size_t[]){i, j, slice[2]}));
                            }
                        }
                        return result;
                    }
                    case COMPLEX: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[1]}, &(enum DataType){COMPLEX});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            for (size_t j = 0; j < array->dimensions[1]; j++) {
                                set_element_a(result, (size_t[]){i,j}, get_element_a(array, (size_t[]){i, j, slice[2]}));
                            }
                        }
                        return result;
                    }
                    default: {
                        log_invalid_type(__FILE__, __LINE__);
                    }
                }
            }
            //[-1,0,-1]
            if (slice[0] == -1 && slice[1] >= 0 && slice[2] == -1) {
                switch (*array->type) {
                    case DOUBLE: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[2]}, &(enum DataType){DOUBLE});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            for (size_t j = 0; j < array->dimensions[2]; j++) {
                                set_element_a(result, (size_t[]){i,j}, get_element_a(array, (size_t[]){i, slice[1], j}));
                            }
                        }
                        return result;
                    }
                    case INT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[2]}, &(enum DataType){INT});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            for (size_t j = 0; j < array->dimensions[2]; j++) {
                                set_element_a(result, (size_t[]){i,j}, get_element_a(array, (size_t[]){i, slice[1], j}));
                            }
                        }
                        return result;
                    }
                    case FLOAT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[2]}, &(enum DataType){FLOAT});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            for (size_t j = 0; j < array->dimensions[2]; j++) {
                                set_element_a(result, (size_t[]){i,j}, get_element_a(array, (size_t[]){i, slice[1], j}));
                            }
                        }
                        return result;
                    }
                    case COMPLEX: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[0], array->dimensions[2]}, &(enum DataType){COMPLEX});
                        for (size_t i = 0; i < array->dimensions[0]; i++) {
                            for (size_t j = 0; j < array->dimensions[2]; j++) {
                                set_element_a(result, (size_t[]){i,j}, get_element_a(array, (size_t[]){i, slice[1], j}));
                            }
                        }
                        return result;
                    }
                    default: {
                        log_invalid_type(__FILE__, __LINE__);
                    }
                }
            }
            //[0,-1,-1]
            if (slice[0] >= 0 && slice[1] == -1 && slice[2] == -1) {
                switch (*array->type) {
                    case DOUBLE: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[1], array->dimensions[2]}, &(enum DataType){DOUBLE});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            for (size_t j = 0; j < array->dimensions[2]; j++) {
                                set_element_a(result, (size_t[]){i,j}, get_element_a(array, (size_t[]){slice[0], i, j}));
                            }
                        }
                        return result;
                    }
                    case INT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[1], array->dimensions[2]}, &(enum DataType){INT});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            for (size_t j = 0; j < array->dimensions[2]; j++) {
                                set_element_a(result, (size_t[]){i,j}, get_element_a(array, (size_t[]){slice[0], i, j}));
                            }
                        }
                        return result;
                    }
                    case FLOAT: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[1], array->dimensions[2]}, &(enum DataType){FLOAT});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            for (size_t j = 0; j < array->dimensions[2]; j++) {
                                set_element_a(result, (size_t[]){i,j}, get_element_a(array, (size_t[]){slice[0], i, j}));
                            }
                        }
                        return result;
                    }
                    case COMPLEX: {
                        struct array* result = zeros_a(&(size_t){2}, (size_t[]){array->dimensions[1], array->dimensions[2]}, &(enum DataType){COMPLEX});
                        for (size_t i = 0; i < array->dimensions[1]; i++) {
                            for (size_t j = 0; j < array->dimensions[2]; j++) {
                                set_element_a(result, (size_t[]){i,j}, get_element_a(array, (size_t[]){slice[0], i, j}));
                            }
                        }
                        return result;
                    }
                    default: {
                        log_invalid_type(__FILE__, __LINE__);
                    }
                }
            }

            log_incompatible_slice(__FILE__, __LINE__);
            break;
        }
        default: {
            log_invalid_dimensions(__FILE__, __LINE__);
            break;
        }
    }
    return NULL;
}

/**
 * Computing the conjugate of an array  +
 * RETURNS:
 *  - Conjugate array struct pointer
 * INPUTS:
 *  - array struct pointer
*/
struct array* conjugate_a(struct array* array) {
    if (*array->type == COMPLEX) {
        struct array* complex_conjugate = zeros_a(array->num_dimensions, array->dimensions, array->type);
        double complex* complex_arr = (double complex*) complex_conjugate->array;
        double complex* array_arr = (double complex*) array->array;

        for (size_t i = 0; i < get_size_a(array); i++) {
            complex_arr[i] = conj(array_arr[i]);
        }

        return complex_conjugate;
    } else {
        log_invalid_type(__FILE__, __LINE__);
    }
    return NULL;
}

/**
 * Print shape of array
 * RETURNS:
 *  - NONE
 * INPUTS: 
 *  - array struct pointer
*/
void print_shape_a(struct array* array) {
    switch (*array->num_dimensions) {
        case 1: {
            printf("[%ld]\n", array->dimensions[0]);
            break;
        }
        case 2: {
            printf("[%ld, %ld]\n", array->dimensions[0], array->dimensions[1]);
            break;
        }
        case 3: {
            printf("[%ld, %ld, %ld]\n", array->dimensions[0], array->dimensions[1], array->dimensions[2]);
            break;
        }
        default: {
            log_invalid_dimensions(__FILE__, __LINE__);
        }
    }
}

/**
 * Fill the diagonal of array with desired value
 * Only 2d arrays are acceptable
 * RETURNS:
 *  - None
 * INPUTS: 
 *  - array struct pointer
 *  - Value pointer
*/
void fill_diagonal_a(struct array* array, void* value) {
    switch (*array->num_dimensions) {
        case 2: {
            if (array->dimensions[0] != array->dimensions[1]) {
                log_incompatible_dimensions(__FILE__, __LINE__);
                break;
            }
            switch (*array->type) {
                case DOUBLE: {
                    double* value_double = (double*)value;
                    for (size_t i = 0; i < array->dimensions[0]; i++) {
                        set_element_a(array, (size_t[]){i,i}, value_double);
                    }
                    break;
                }
                case INT: {
                    int* value_int = (int*)value;
                    for (size_t i = 0; i < array->dimensions[0]; i++) {
                        set_element_a(array, (size_t[]){i,i}, value_int);
                    }
                    break;
                }
                case FLOAT: {
                    float* value_float = (float*)value;
                    for (size_t i = 0; i < array->dimensions[0]; i++) {
                        set_element_a(array, (size_t[]){i,i}, value_float);
                    }
                    break;
                }
                case COMPLEX: {
                    double complex* value_complex = (double complex*)value;
                    for (size_t i = 0; i < array->dimensions[0]; i++) {
                        set_element_a(array, (size_t[]){i,i}, value_complex);
                    }
                    break;
                }
                default: {
                    log_invalid_type(__FILE__, __LINE__);
                }
            }
            break;
        }
        default: {
            log_invalid_dimensions(__FILE__, __LINE__);
        }
    }
}

/**
 * Calculates the norm of an array.
 * RETURNS:
 *  - norm value pointer.
 * INPUTS: 
 *  - array struct pointer.
 * NOTE:
 *  - Need to free the return value.
 *  - If Complex, return value is double.
*/
void* norm_a(struct array* array) {
    if (get_size_a(array) == 0) {
        return NULL;
    }
    switch (*array->type) {
        case DOUBLE: {
            double* norm = malloc(sizeof(double));
            if (!norm) {
                return NULL;
            }
            *norm = 0; 
            double* arr = (double*)array->array;
            for (size_t i = 0; i < get_size_a(array); i++) {
                *norm += pow(arr[i],2);
            }
            *norm = sqrt(*norm);

            return norm;
        }
        case INT: {
            double* norm = malloc(sizeof(double));
            if (!norm) {
                return NULL;
            }
            *norm = 0; 
            int* arr = (int*)array->array;
            for (size_t i = 0; i < get_size_a(array); i++) {
                *norm += pow(arr[i],2);
            }
            *norm = sqrt(*norm);

            return norm;
        }
        case FLOAT: {
            float* norm = malloc(sizeof(float));
            if (!norm) {
                return NULL;
            }
            *norm = 0; 
            float* arr = (float*)array->array;
            for (size_t i = 0; i < get_size_a(array); i++) {
                *norm += pow(arr[i],2);
            }
            *norm = sqrt(*norm);

            return norm;
        }
        case COMPLEX: {
            double* norm = malloc(sizeof(double));
            if (!norm) {
                return NULL;
            }
            *norm = 0; 
            double complex* arr = (double complex*)array->array;
            for (size_t i = 0; i < get_size_a(array); i++) {
                *norm += pow(cabs(arr[i]),2);
            }
            *norm = sqrt(*norm);

            return norm;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;
}

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
void* mean_a(struct array* array) {
    if (get_size_a(array) == 0) {
        return NULL;
    }
    switch (*array->type) {
        case DOUBLE: {
            double* mean = malloc(sizeof(double));
            if (!mean) {
                return NULL;
            }
            *mean = 0; 
            double* arr = (double*)array->array;
            for (size_t i = 0; i < get_size_a(array); i++) {
                *mean += arr[i];
            }
            *mean = *mean / get_size_a(array);

            return mean;
        }
        case INT: {
            double* mean = malloc(sizeof(double));
            if (!mean) {
                return NULL;
            }
            *mean = 0; 
            int* arr = (int*)array->array;
            for (size_t i = 0; i < get_size_a(array); i++) {
                *mean += arr[i];
            }
            *mean = *mean / get_size_a(array);

            return mean;
        }
        case FLOAT: {
            float* mean = malloc(sizeof(float));
            if (!mean) {
                return NULL;
            }
            *mean = 0; 
            float* arr = (float*)array->array;
            for (size_t i = 0; i < get_size_a(array); i++) {
                *mean += arr[i];
            }
            *mean = *mean / get_size_a(array);

            return mean;
        }
        case COMPLEX: {
            double* mean = malloc(sizeof(double));
            if (!mean) {
                return NULL;
            }
            *mean = 0; 
            double complex* arr = (double complex*)array->array;
            for (size_t i = 0; i < get_size_a(array); i++) {
                *mean += cabs(arr[i]);
            }
            *mean = *mean / get_size_a(array);

            return mean;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;
}

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
void* std_a(struct array* array) {
    if (get_size_a(array) == 0) {
        return NULL;
    }
    switch (*array->type) {
        case DOUBLE: {
            double* std = malloc(sizeof(double));
            double* norm = mean_a(array);
            if (!std) {
                return NULL;
            }

            *std = 0; 
            double* arr = (double*)array->array;
            //calculating the sum
            double sum = 0;
            for (size_t i = 0; i < get_size_a(array); i++) {
                double value = (double)(pow((arr[i]- *norm),2));
                sum += value;
            }

            *std = sum / get_size_a(array);
            *std = sqrt(*std);

            free(norm);
            return std;
        }
        case INT: {
            double* std = malloc(sizeof(double));
            double* norm = mean_a(array);
            if (!std) {
                return NULL;
            }

            *std = 0;
            int* arr = (int*)array->array;
            //calculating the sum
            double sum = 0;
            for (size_t i = 0; i < get_size_a(array); i++) {
                double value = (double)(pow((arr[i]- *norm),2));
                sum += value;
            }

            *std = sqrt(sum / get_size_a(array));
            
            free(norm);
            return std;
        }
        case FLOAT: {
            float* std = malloc(sizeof(float));
            float* norm = mean_a(array);
            if (!std) {
                return NULL;
            }

            *std = 0; 
            float* arr = (float*)array->array;
            //calculating the sum
            float sum = 0;
            for (size_t i = 0; i < get_size_a(array); i++) {
                float value = (float)(pow((arr[i]- *norm),2));
                sum += value;
            }

            *std = sqrt(sum / get_size_a(array));
            
            free(norm);
            return std;
        }
        case COMPLEX: {
            double* std = malloc(sizeof(double));
            double* norm = mean_a(array);
            if (!std) {
                return NULL;
            }

            *std = 0; 
            double complex* arr = (double complex*)array->array;
            //calculating the sum
            double sum = 0;
            for (size_t i = 0; i < get_size_a(array); i++) {
                double value = (double)(pow((cabs(arr[i])- *norm),2));
                sum += value;
            }

            *std = sqrt(sum / get_size_a(array));
            
            free(norm);
            return std;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;
}

/**
 * This fuction does the outer product of two 1d arrays
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - array struct pointer
*/
struct array* outer_a(struct array* array1, struct array* array2) {
    if (*array1->num_dimensions != 1 || *array2->num_dimensions != 1) {
        log_incompatible_dimensions(__FILE__, __LINE__);
    }
    if (array1->dimensions[0] != array2->dimensions[0]) {
        log_incompatible_size(__FILE__, __LINE__);
    }
    if (*array1->type != *array2->type) {
        log_incompatible_types(__FILE__, __LINE__); 
    }

    switch (*array1->type) {
        case DOUBLE: {
            struct array* result = zeros_a(&(size_t){2}, (size_t[]){array1->dimensions[0],array1->dimensions[0]}, &(enum DataType){DOUBLE});
            double* array1_arr = array1->array;
            double* array2_arr = array2->array;
            for (size_t r = 0; r < array1->dimensions[0]; r++) {
                for (size_t c = 0; c < array1->dimensions[0]; c++) {
                    set_element_a(result, (size_t[]){r,c}, &(double){array1_arr[r] * array2_arr[c]});
                }
            }
            return result;
        }
        case INT: {
            struct array* result = zeros_a(&(size_t){2}, (size_t[]){array1->dimensions[0],array1->dimensions[0]}, &(enum DataType){INT});
            int* array1_arr = array1->array;
            int* array2_arr = array2->array;
            for (size_t r = 0; r < array1->dimensions[0]; r++) {
                for (size_t c = 0; c < array1->dimensions[0]; c++) {
                    set_element_a(result, (size_t[]){r,c}, &(int){array1_arr[r] * array2_arr[c]});
                }
            }
            return result;
        }
        case FLOAT: {
            struct array* result = zeros_a(&(size_t){2}, (size_t[]){array1->dimensions[0],array1->dimensions[0]}, &(enum DataType){FLOAT});
            float* array1_arr = array1->array;
            float* array2_arr = array2->array;
            for (size_t r = 0; r < array1->dimensions[0]; r++) {
                for (size_t c = 0; c < array1->dimensions[0]; c++) {
                    set_element_a(result, (size_t[]){r,c}, &(float){array1_arr[r] * array2_arr[c]});
                }
            }
            return result;
        }
        case COMPLEX: {
            struct array* result = zeros_a(&(size_t){2}, (size_t[]){array1->dimensions[0],array1->dimensions[0]}, &(enum DataType){COMPLEX});
            double complex* array1_arr = array1->array;
            double complex* array2_arr = array2->array;
            for (size_t r = 0; r < array1->dimensions[0]; r++) {
                for (size_t c = 0; c < array1->dimensions[0]; c++) {
                    set_element_a(result, (size_t[]){r,c}, &(double complex){array1_arr[r] * array2_arr[c]});
                }
            }
            return result;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;
}

/**
 * Hadamard Inverse of a complex matrix
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - array struct pointer
*/
struct array* hadamard_inverse_a(struct array* array) {
    
    if (*array->type == COMPLEX) {
        struct array* result = zeros_a(array->num_dimensions, array->dimensions, array->type);
        double complex* arr = array->array;
        double complex* arr_result = result->array;
        for (size_t i = 0; i < get_size_a(array); i++) {
            if (arr[i] != 0.0 + 0.0*I) {
                arr_result[i] = 1.0 / arr[i];
            } 
        }
        return result;
    } else {
        log_invalid_type(__FILE__, __LINE__);
    }

    return NULL;
}

/**
 * Diagonal Hadamard Inverse of a complex matrix
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - array struct pointer
*/
struct array* diag_hadamard_inverse_a(struct array* array) {
    if (*array->num_dimensions != 2) {
        log_incompatible_size(__FILE__, __LINE__);
    }
    if (array->dimensions[0] != array->dimensions[1]) {
        log_incompatible_dimensions(__FILE__, __LINE__);
    }

    if (*array->type == COMPLEX) {
        struct array* result = zeros_a(array->num_dimensions, array->dimensions, array->type);
        for (size_t i = 0; i < array->dimensions[0]; i++) {
            double complex value = *(double complex*)get_element_a(array, (size_t[]){i,i});
            if (value != 0.0 + 0.0*I) {
                value = 1.0 / value;
            }
            set_element_a(result, (size_t[]){i,i}, &value);
        }
        return result;
    } else {
        log_invalid_type(__FILE__, __LINE__);
    }

    return NULL;

}

/**
 * Computes the transpose of a matrix
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
*/
struct array* transpose_a(struct array* array) {

    if (*array->num_dimensions != 2) {
        log_incompatible_size(__FILE__, __LINE__);
    }

    switch (*array->type) {
        case DOUBLE: {
            struct array* result = zeros_a(array->num_dimensions, (size_t[]){array->dimensions[1], array->dimensions[0]}, array->type);
            for (size_t r = 0; r < array->dimensions[0]; r++) {
                for (size_t c = 0; c < array->dimensions[1]; c++) {
                    set_element_a(result, (size_t[]){c,r}, get_element_a(array, (size_t[]){r,c}));
                }
            }
            return result;
        }
        case INT: {
            struct array* result = zeros_a(array->num_dimensions, (size_t[]){array->dimensions[1], array->dimensions[0]}, array->type);
            for (size_t r = 0; r < array->dimensions[0]; r++) {
                for (size_t c = 0; c < array->dimensions[1]; c++) {
                    set_element_a(result, (size_t[]){c,r}, get_element_a(array, (size_t[]){r,c}));
                }
            }
            return result;
            break;
        }
        case FLOAT: {
            struct array* result = zeros_a(array->num_dimensions, (size_t[]){array->dimensions[1], array->dimensions[0]}, array->type);
            for (size_t r = 0; r < array->dimensions[0]; r++) {
                for (size_t c = 0; c < array->dimensions[1]; c++) {
                    set_element_a(result, (size_t[]){c,r}, get_element_a(array, (size_t[]){r,c}));
                }
            }
            return result;
            break;
        }
        case COMPLEX: {
            struct array* result = zeros_a(array->num_dimensions, (size_t[]){array->dimensions[1], array->dimensions[0]}, array->type);
            for (size_t r = 0; r < array->dimensions[0]; r++) {
                for (size_t c = 0; c < array->dimensions[1]; c++) {
                    set_element_a(result, (size_t[]){c,r}, get_element_a(array, (size_t[]){r,c}));
                }
            }
            return result;
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }

    return NULL;
}

/**
 * Computes the hermitian transpose of a complex matrix
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
*/
struct array* hermitian_transpose_a(struct array* array) {

    if (*array->num_dimensions != 2) {
        log_incompatible_size(__FILE__, __LINE__);
    }

    if(*array->type != COMPLEX) {
        log_invalid_type(__FILE__, __LINE__);
    }

    struct array* array_conjugate = conjugate_a(array);
    struct array* hermitian_transpose_array = transpose_a(array_conjugate);
    free_a(array_conjugate);
    return hermitian_transpose_array;
}

/**
 * Save to file (.array)
 * RETURNS:
 *  -NONE
 * INPUTS:
 *  -NONE
*/
void save_to_file_a(struct array* array, char* directory, char* filename) {
    if (array == NULL) {
        return;
    }
    char full_path[256];

    if (strcmp(directory, "") != 0) {
        snprintf(full_path, sizeof(full_path), "%s/%s.array", directory, filename);
        create_directory(directory);
    } else {
        snprintf(full_path, sizeof(full_path), "%s.array",filename);
    }

    // printf("\n");
    // printf("%s\n", directory);
    // printf("%s\n", full_path);

    // bool path_check = check_file_path(directory);
    // bool path_full_path = check_file_path(full_path);
    // printf("Directory Exists: %d\n", path_check);
    // printf("Full Path Exists: %d\n", path_full_path);

    FILE *combinedDataFile = fopen(full_path, "w");
    if (!combinedDataFile) {
        // printf("%s\n", full_path);
        // printf("%d\n",path_check);
        // printf("%d\n",path_full_path);
        // perror("Error opening file");
        log_error_opening_file(__FILE__, __LINE__);
        return;
    }

    fprintf(combinedDataFile, "%d\n", *array->type);
    fprintf(combinedDataFile, "%ld\n", *array->num_dimensions);
    for (int i = 0; i < *array->num_dimensions; i++) {
        fprintf(combinedDataFile, "%ld\n", array->dimensions[i]);
    }

    switch (*array->type) {
        case DOUBLE: {
            double* arr = array->array;
            for (int h = 0; h < get_size_a(array); h++) {
                fprintf(combinedDataFile, "%.20f\n", arr[h]);
            }
            break;
        }
        case INT: {
            int* arr = array->array;
            for (int h = 0; h < get_size_a(array); h++) {
                fprintf(combinedDataFile, "%d\n", arr[h]);
            }
            break;
        }
        case FLOAT: {
            float* arr = array->array;
            for (int h = 0; h < get_size_a(array); h++) {
                fprintf(combinedDataFile, "%.20f\n", arr[h]);
            }
            break;
        }
        case COMPLEX: {
            double complex* arr = array->array;
            for (int h = 0; h < get_size_a(array); h++) {
                fprintf(combinedDataFile, "%.20f,%.20f\n", creal(arr[h]), cimag(arr[h]));
            }
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    fclose(combinedDataFile);
}

/**
 * Read .array file
*/
struct array* read_from_file_a(char* filepath) {
    if(!ends_with_string(filepath, ".array")) {
        log_invalid_file_extension(__FILE__,__LINE__);
    }

    FILE *combinedDataFile = fopen(filepath, "r");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return NULL;
    }

    enum DataType type = 0;
    int type_int = 0;
    size_t num_dimensions = 0;
    size_t dimensions [3] = {0,0,0};

    if (fscanf(combinedDataFile, "%d", &type_int) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    type = type_int;
    if (fscanf(combinedDataFile, "%lu", &num_dimensions) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }

    for (size_t i = 0; i < num_dimensions; i++) {
        if (fscanf(combinedDataFile, "%lu", &dimensions[i]) != 1) {
            log_error_fscan(__FILE__,__LINE__);
        }
    }

    struct array* result = zeros_a(&num_dimensions, dimensions, &type);

    switch (*result->type) {
        case DOUBLE: {
            double* arr = result->array;
            for (int h = 0; h < get_size_a(result); h++) {
                if (fscanf(combinedDataFile, "%lf", &arr[h]) != 1) {
                    log_error_fscan(__FILE__,__LINE__);
                }
            }
            break;
        }
        case INT: {
            int* arr = result->array;
            for (int h = 0; h < get_size_a(result); h++) {
                if (fscanf(combinedDataFile, "%d", &arr[h]) != 1) {
                    log_error_fscan(__FILE__,__LINE__);
                }
            }
            break;
        }
        case FLOAT: {
            float* arr = result->array;
            for (int h = 0; h < get_size_a(result); h++) {
                if (fscanf(combinedDataFile, "%f", &arr[h]) != 1) {
                    log_error_fscan(__FILE__,__LINE__);
                }
            }
            break;
        }
        case COMPLEX: {
            double complex* arr = result->array;
            double real, imag;
            for (int h = 0; h < get_size_a(result); h++) {
                if (fscanf(combinedDataFile, "%lf,%lf", &real, &imag) != 2) {
                    log_error_fscan(__FILE__,__LINE__);
                }
                arr[h] = real + imag * I;
            }
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    fclose(combinedDataFile);
    return result;
}

/**
 * This function will lock the array when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct array pointer
*/
void lock_a(struct array* array) {
    pthread_mutex_lock(array->mutex);
}

/**
 * This function will unlock the array when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct array pointer
*/
void unlock_a(struct array* array) {
    pthread_mutex_unlock(array->mutex);
}

/**
 * This function will check if two arrays are equal
 * RETURNS:
 *  - bool
 * INPUTS:
 *  - two struct array pointer
*/
bool equal_a(struct array* array1, struct array* array2) {
    if (*array1->num_dimensions != *array2->num_dimensions) {
        return false;
    }
    if (*array1->type != *array2->type) {
        return false;
    }
    switch (*array1->num_dimensions) {
        case 1: {
            if (array1->dimensions[0] != array2->dimensions[0]) {
                return false;
            }
            break;
        }
        case 2: {
            if (array1->dimensions[0] != array2->dimensions[0] || array1->dimensions[1] != array2->dimensions[1]) {
                return false;
            }
            break;
        }
        case 3: {
            if (array1->dimensions[0] != array2->dimensions[0] || array1->dimensions[1] != array2->dimensions[1] || array1->dimensions[2] != array2->dimensions[2]) {
                return false;
            }
            break;
        }
        default: {
            return false;
            break;
        }
    }

    switch (*array1->type) {
        case DOUBLE: {
            double* arr1 = get_a(array1);
            double* arr2 = get_a(array2);
            for (int i = 0; i < get_size_a(array1); i++) {
                if (arr1[i] != arr2[i]) {
                    return false;
                }
            }
            break;
        }
        case INT: {
            int* arr1 = get_a(array1);
            int* arr2 = get_a(array2);
            for (int i = 0; i < get_size_a(array1); i++) {
                if (arr1[i] != arr2[i]) {
                    return false;
                }
            }
            break;
        }
        case FLOAT: {
            float* arr1 = get_a(array1);
            float* arr2 = get_a(array2);
            for (int i = 0; i < get_size_a(array1); i++) {
                if (arr1[i] != arr2[i]) {
                    return false;
                }
            }
            break;
        }
        case COMPLEX: {
            double complex* arr1 = get_a(array1);
            double complex* arr2 = get_a(array2);
            for (int i = 0; i < get_size_a(array1); i++) {
                if (arr1[i] != arr2[i]) {
                    return false;
                }
            }
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }

    return true;
}

/**
 * This function computes the reciprocal of an array
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
*/
struct array* reciprocal_a(struct array* array) {
    switch (*array->type) {
        case DOUBLE: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, array->type);
            double* arr = get_a(array);
            double* arr_result = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                if (arr[i] > EPSILON_RECIPROCAL) {
                    arr_result[i] = 1.0 / arr[i];
                } else {
                    arr_result[i] = 0.0;
                }
            }
            return result;
        }
        case INT: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, array->type);
            int* arr = get_a(array);
            int* arr_result = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                if (arr[i] > EPSILON_RECIPROCAL) {
                    arr_result[i] = 1.0 / arr[i];
                } else {
                    arr_result[i] = 0.0;
                }
            }
            return result;
        }
        case FLOAT: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, array->type);
            float* arr = get_a(array);
            float* arr_result = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                if (arr[i] > EPSILON_RECIPROCAL) {
                    arr_result[i] = 1.0 / arr[i];
                } else {
                    arr_result[i] = 0.0;
                }
            }
            return result;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;
}

/**
 * This function computes the diagonal reciprocal of an array
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
*/
struct array* diag_reciprocal_a(struct array* array) {
    if (*array->num_dimensions != 2) {
        log_incompatible_size(__FILE__, __LINE__);
    }
    if (array->dimensions[0] != array->dimensions[1]) {
        log_incompatible_dimensions(__FILE__, __LINE__);
    }
    switch (*array->type) {
        case DOUBLE: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, array->type);
            for (size_t i = 0; i < array->dimensions[0]; i++) {
                double value = *(double*)get_element_a(array, (size_t[]){i,i});
                value = 1.0 / value;
                set_element_a(result, (size_t[]){i,i}, &value);
            }
            return result;
        }
        case INT: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, array->type);
            for (size_t i = 0; i < array->dimensions[0]; i++) {
                int value = *(int*)get_element_a(array, (size_t[]){i,i});
                value = 1.0 / value;
                set_element_a(result, (size_t[]){i,i}, &value);
            }
            return result;
        }
        case FLOAT: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, array->type);
            for (size_t i = 0; i < array->dimensions[0]; i++) {
                float value = *(float*)get_element_a(array, (size_t[]){i,i});
                value = 1.0 / value;
                set_element_a(result, (size_t[]){i,i}, &value);
            }
            return result;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;
}

/**
 * This function computes the real part of an array
 * RETURNS:
 *  - struct array pointer (DOUBLE)
 * INPUTS:
 *  - struct array pointer
 *
*/
struct array* real_a(struct array* array) {
    if (*array->type == COMPLEX) {
        struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){DOUBLE});
        double complex* arr = array->array;
        double* result_arr = result->array;

        for (size_t i = 0; i < get_size_a(array); i++) {
            result_arr[i] = creal(arr[i]);
        }

        return result;
    } else {
        log_invalid_type(__FILE__, __LINE__);
    }

    return NULL;
}

/**
 * This function computes imaginary part of an array
 * RETURNS:
 *  - struct array pointer (DOUBLE)
 * INPUTS:
 *  - struct array pointer
*/
struct array* imag_a(struct array* array) {
    if (*array->type == COMPLEX) {
        struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){DOUBLE});
        double complex* arr = array->array;
        double* result_arr = result->array;

        for (size_t i = 0; i < get_size_a(array); i++) {
            result_arr[i] = cimag(arr[i]);
        }

        return result;
    } else {
        log_invalid_type(__FILE__, __LINE__);
    }

    return NULL;
}

/**
 * This function checks if a specific element is in an array.
 * RETURNS:
 *  - bool
 * INPUTS:
 *  - struct array pointer
*/
bool contains_element_a(struct array* array, void* element) {
    bool check = false;
    switch (*array->type) {
        case DOUBLE: {
            double element_double = *(double*){element};
            double* arr_double = get_a(array);
            for (size_t i = 0; i < get_size_a(array); i++) {
                if (arr_double[i] == element_double) {
                    check = true;
                    break;
                }
            }
            break;
        }
        case INT: {
            int element_double = *(int*){element};
            int* arr_double = get_a(array);
            for (size_t i = 0; i < get_size_a(array); i++) {
                if (arr_double[i] == element_double) {
                    check = true;
                    break;
                }
            }
            break;
        }
        case FLOAT: {
            float element_double = *(float*){element};
            float* arr_double = get_a(array);
            for (size_t i = 0; i < get_size_a(array); i++) {
                if (arr_double[i] == element_double) {
                    check = true;
                    break;
                }
            }
            break;
        }
        case COMPLEX: {
            double complex element_double = *(double*){element};
            double complex* arr_double = get_a(array);
            for (size_t i = 0; i < get_size_a(array); i++) {
                if (arr_double[i] == element_double) {
                    check = true;
                    break;
                }
            }
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return check;
}

/**
 * This function checks if a specific element is in an array.
 * RETURNS:
 *  - index or -1 if not found
 * INPUTS:
 *  - struct array pointer
*/
int index_of_element_a(struct array* array, void* element) {
    int index = -1;
    switch (*array->type) {
        case DOUBLE: {
            double element_double = *(double*){element};
            double* arr_double = get_a(array);
            for (size_t i = 0; i < get_size_a(array); i++) {
                if (arr_double[i] == element_double) {
                    index = i;
                    break;
                }
            }
            break;
        }
        case INT: {
            int element_double = *(int*){element};
            int* arr_double = get_a(array);
            for (size_t i = 0; i < get_size_a(array); i++) {
                if (arr_double[i] == element_double) {
                    index = i;
                    break;
                }
            }
            break;
        }
        case FLOAT: {
            float element_double = *(float*){element};
            float* arr_double = get_a(array);
            for (size_t i = 0; i < get_size_a(array); i++) {
                if (arr_double[i] == element_double) {
                    index = i;
                    break;
                }
            }
            break;
        }
        case COMPLEX: {
            double complex element_double = *(double*){element};
            double complex* arr_double = get_a(array);
            for (size_t i = 0; i < get_size_a(array); i++) {
                if (arr_double[i] == element_double) {
                    index = i;
                    break;
                }
            }
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return index;
}

/**
 * This function calculates the svd of a 2d array.
 * Jacobi SVD algorithm from scratch 
 * see github.com/ampl/gsl/blob/master/linalg/svd.c
 * RETURNS:
 *  - struct array* U, s, Vh
 * INPUTS:
 *  - struct array pointer
*/
struct array** svd_a(struct array* M) {
    if (*M->num_dimensions != 2) {
        log_invalid_dimensions(__FILE__, __LINE__);
    }

    switch (*M->type) {
        case DOUBLE: {
            log_not_implemented(__FILE__, __LINE__);
            break;
        }
        case INT: {
            log_not_implemented(__FILE__, __LINE__);
            break;
        }
        case FLOAT: {
            log_not_implemented(__FILE__, __LINE__);
            break;
        }
        case COMPLEX: {
            size_t m = M->dimensions[0];
            size_t n = M->dimensions[1];
            int lda = m;

            lapack_complex_double* A = malloc(m * n * sizeof(lapack_complex_double));
            if (A == NULL) {
                log_mem_aloc_fail(__FILE__, __LINE__);
            }

            //Copying values from M to A
            for (int i = 0; i < m; i++) {
                for (int j = 0; j < n; j++) {
                    A[i * n + j] = *(double complex*)get_element_a(M, (size_t[]){i,j});  // Example: (i+1) + (j+1)*I
                }
            }
            
            lapack_complex_double U[m * m];     // Matrix U (m x m)
            lapack_complex_double VT[n * n];    // Matrix VT (n x n)
            double S[m];                        // Vector of singular values (size min(m, n))
            double superb[m]; 

            lapack_int LAPACKE_zgesvd( int matrix_layout, char jobu, char jobvt, lapack_int m, lapack_int n,
                                lapack_complex_double* a, lapack_int lda, double* s, lapack_complex_double* u,
                                lapack_int ldu, lapack_complex_double* vt, lapack_int ldvt, double* superb );

            lapack_int info;
            info = LAPACKE_zgesvd(LAPACK_ROW_MAJOR, 'A', 'A', m, n, A, lda, S, U, lda, VT, n, superb);

            if (info > 0) {
                // log_svd_not_converge(__FILE__, __LINE__);
                // printf("SVD did not converge.\n");
            }

            struct array* U_array = zeros_a(&(size_t){2}, (size_t[]){m,m}, &(enum DataType){COMPLEX});
            struct array* VT_array = zeros_a(&(size_t){2}, (size_t[]){n,n}, &(enum DataType){COMPLEX});
            struct array* S_array = zeros_a(&(size_t){1}, (size_t[]){m}, &(enum DataType){DOUBLE});

            //copy over U to U_array
            for (int i = 0; i < m; i++) {
                for (int j = 0; j < m; j++) {
                    set_element_a(U_array, (size_t[]){i,j}, &(double complex){U[i * m + j]});  // Example: (i+1) + (j+1)*I
                }
            }
            //copy over VT to VT_array
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    set_element_a(VT_array, (size_t[]){i,j}, &(double complex){VT[i * n + j]});  // Example: (i+1) + (j+1)*I
                }
            }
            //copy over S to S_array
            for (int i = 0; i < m; i++) {
                set_element_a(S_array, (size_t[]){i}, &(double){S[i]});
            }

            struct array** svd_elements = malloc(3 * sizeof(struct array*));
            svd_elements[0] = U_array;
            svd_elements[1] = S_array;
            svd_elements[2] = VT_array;

            free(A);

            return svd_elements;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }

    return NULL;
}

/**
 * This function calculates the pseudo inverse of a 2d array.
 * RETURNS:
 *  - struct array* array
 * INPUTS:
 *  - struct array pointer
*/
struct array* p_inv_a(struct array* M) {
    if (*M->num_dimensions != 2) {
        log_invalid_dimensions(__FILE__, __LINE__);
    }

    switch (*M->type) {
        case DOUBLE: {
            log_not_implemented(__FILE__, __LINE__);
            break;
        }
        case INT: {
            log_not_implemented(__FILE__, __LINE__);
            break;
        }
        case FLOAT: {
            log_not_implemented(__FILE__, __LINE__);
            break;
        }
        case COMPLEX: {
            struct array** svd_values = svd_a(M);
            //do not have to free this arrays, will be freed in for loop
            struct array* U = svd_values[0];
            struct array* S = svd_values[1];
            struct array* VH = svd_values[2];
            struct array* S_matrix = diag_a(S, (int[]){-1});
            struct array* S_reciprocal = reciprocal_a(S_matrix);
            struct array* U_hermitian_transpose = hermitian_transpose_a(U);
            struct array* VH_hermitian_transpose = hermitian_transpose_a(VH);
            struct array* VH_hermitian_transpose_dot_S_hadamard_inverse = dot_a(VH_hermitian_transpose, S_reciprocal);
            struct array* p_inv = dot_a(VH_hermitian_transpose_dot_S_hadamard_inverse, U_hermitian_transpose);

            free_a(VH_hermitian_transpose_dot_S_hadamard_inverse);
            free_a(VH_hermitian_transpose);
            free_a(U_hermitian_transpose);
            free_a(S_reciprocal);
            free_a(S_matrix);

            for (size_t i = 0; i < 3; i++) {
                free_a(svd_values[i]);
            }
            free(svd_values);

            return p_inv;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;

}


/**
 * This function calculates the absolute value of an array
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
 */
struct array* abs_a(struct array* array) {
    switch (*array->type) {
        case DOUBLE: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){DOUBLE});
            double* arr = get_a(array);
            double* result_arr = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                result_arr[i] = fabs(arr[i]);
            }
            return result;
        }
        case INT: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){INT});
            int* arr = get_a(array);
            int* result_arr = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                result_arr[i] = abs(arr[i]);
            }
            return result;
        }
        case FLOAT: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){FLOAT});
            float* arr = get_a(array);
            float* result_arr = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                result_arr[i] = fabsf(arr[i]);
            }
            return result;
        }
        case COMPLEX: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){DOUBLE});
            double complex* arr = get_a(array);
            double* result_arr = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                result_arr[i] = cabs(arr[i]);
            }
            return result;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;
}

/**
 * This function calculates the power of an array
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
 *  - double power
 */
struct array* power_a(struct array* array, double power) {
    switch (*array->type) {
        case DOUBLE: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){DOUBLE});
            double* arr = get_a(array);
            double* result_arr = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                result_arr[i] = pow(arr[i], power);
            }
            return result;
        }
        case INT: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){INT});
            int* arr = get_a(array);
            int* result_arr = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                result_arr[i] = (int)pow(arr[i], power);
            }
            return result;
        }
        case FLOAT: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){FLOAT});
            float* arr = get_a(array);
            float* result_arr = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                result_arr[i] = powf(arr[i], power);
            }
            return result;
        }
        case COMPLEX: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){COMPLEX});
            double complex* arr = get_a(array);
            double complex* result_arr = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                result_arr[i] = cpow(arr[i], power);
            }
            return result;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;
}

/**
 * This function calculates the square root of an array
 * RETURNS:
 *  - struct array pointer
 * INPUTS:
 *  - struct array pointer
 */
struct array* sqrt_a(struct array* array) {
    switch (*array->type) {
        case DOUBLE: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){DOUBLE});
            double* arr = get_a(array);
            double* result_arr = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                result_arr[i] = sqrt(arr[i]);
            }
            return result;
        }
        case INT: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){INT});
            int* arr = get_a(array);
            int* result_arr = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                result_arr[i] = (int)sqrt(arr[i]);
            }
            return result;
        }
        case FLOAT: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){FLOAT});
            float* arr = get_a(array);
            float* result_arr = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                result_arr[i] = sqrtf(arr[i]);
            }
            return result;
        }
        case COMPLEX: {
            struct array* result = zeros_a(array->num_dimensions, array->dimensions, &(enum DataType){COMPLEX});
            double complex* arr = get_a(array);
            double complex* result_arr = get_a(result);
            for (size_t i = 0; i < get_size_a(array); i++) {
                result_arr[i] = csqrt(arr[i]);
            }
            return result;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;
}

    // switch (*array->type) {
    //     case DOUBLE: {
    //         break;
    //     }
    //     case INT: {
    //         break;
    //     }
    //     case FLOAT: {
    //         break;
    //     }
    //     case COMPLEX: {
    //         break;
    //     }
    //     default: {
    //         log_invalid_type(__FILE__, __LINE__);
    //     }
    // }