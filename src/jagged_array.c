/**
  * @file jagged_array.c
  * This is the implementation of the functions of jagged_array.h
  */

#include <stdio.h>
#include <stdlib.h>
#include "jagged_array.h"
#include "array.h"
#include "logger.h"
#include <string.h>
#include <math.h>

/**
 * Create new Jaggedarray
 * RETURNS:
 *  - Jaggedarray struct pointer
 * INPUTE:
 *  - size
*/
struct Jaggedarray* new_ja(size_t* size) {
    struct Jaggedarray* jagged_array = malloc(sizeof(struct Jaggedarray));

    jagged_array->size = malloc(sizeof(size_t));
    *jagged_array->size = *size;

    jagged_array->jarray = malloc(*size * sizeof(struct array*));
    for (size_t i = 0; i < *size; i++) {
      jagged_array->jarray[i] = NULL;
    }

    return jagged_array;
}

/**
 * free Jaggedarray
 * RETURNS:
 *  - None
 * INPUTE:
 *  - Jaggedarray Pointer
*/
void free_ja(struct Jaggedarray* jagged_array) {
    if (jagged_array == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    if (jagged_array->jarray != NULL) {
      for (size_t i = 0; i < *jagged_array->size; i++) {
        if (jagged_array->jarray[i] != NULL) {
          free_a(jagged_array->jarray[i]);
        }
      }

      free(jagged_array->jarray);
    }

    if (jagged_array->size != NULL) {
      free(jagged_array->size);
    }

    free(jagged_array);
}

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
void set_ja(struct Jaggedarray* jagged_array, struct array* array, size_t* index) {
    if (jagged_array == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    struct array* copy_array = zeros_a(array->num_dimensions, array->dimensions, array->type);
    copy_over_a(copy_array, array, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});

    if (jagged_array->jarray[*index] != NULL) {
      //need to free the array in this position
      free_a(jagged_array->jarray[*index]);
      jagged_array->jarray[*index] = copy_array;
    } else {
      //position does not contain an array
      jagged_array->jarray[*index] = copy_array;
    }
}

/**
 * get array in Jaggedarray
 * RETURNS:
 *  - struct array pointer
 * INPUTE:
 *  - struct Jaggedarray pointer
 *  - index
*/
struct array* get_ja(struct Jaggedarray* jagged_array, size_t* index) {
    if (jagged_array == NULL) {
      log_null_value(__FILE__, __LINE__);
    }
    return jagged_array->jarray[*index];
}

/**
 * save Jaggedarray
 * RETURNS:
 *  - None
 * INPUTE:
 *  - Jaggedarray
 *  - directory
 *  - filename
*/
void save_to_file_ja(struct Jaggedarray* jagged_array, char* directory, char* filename) {
    if (jagged_array == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    char full_path[256];

    if (strcmp(directory, "") != 0) {
        snprintf(full_path, sizeof(full_path), "%s/%s.jarray", directory, filename);
        create_directory(directory);
    } else {
        snprintf(full_path, sizeof(full_path), "%s.jarray",filename);
    }

    FILE *combinedDataFile = fopen(full_path, "w");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return;
    }

    fprintf(combinedDataFile, "%zu\n", *jagged_array->size);
    for (size_t i = 0; i < *jagged_array->size; i++) {
      
      fprintf(combinedDataFile, "%d", *jagged_array->jarray[i]->type);
      fprintf(combinedDataFile, " %zu", *jagged_array->jarray[i]->num_dimensions);
      for (size_t j = 0; j < *jagged_array->jarray[i]->num_dimensions; j++) {
        fprintf(combinedDataFile, " %zu", jagged_array->jarray[i]->dimensions[j]);
      }

      switch (*jagged_array->jarray[i]->type) {
          case DOUBLE: {
              double* arr = jagged_array->jarray[i]->array;
              for (int h = 0; h < get_size_a(jagged_array->jarray[i]); h++) {
                  fprintf(combinedDataFile, " %f", arr[h]);
              }
              break;
          }
          case INT: {
              int* arr = jagged_array->jarray[i]->array;
              for (int h = 0; h < get_size_a(jagged_array->jarray[i]); h++) {
                  fprintf(combinedDataFile, " %d", arr[h]);
              }
              break;
          }
          case FLOAT: {
              float* arr = jagged_array->jarray[i]->array;
              for (int h = 0; h < get_size_a(jagged_array->jarray[i]); h++) {
                  fprintf(combinedDataFile, " %f", arr[h]);
              }
              break;
          }
          case COMPLEX: {
              double complex* arr = jagged_array->jarray[i]->array;
              for (int h = 0; h < get_size_a(jagged_array->jarray[i]); h++) {
                  fprintf(combinedDataFile, " %f,%f", creal(arr[h]), cimag(arr[h]));
              }
              break;
          }
          default: {
              log_invalid_type(__FILE__, __LINE__);
          }
      }

      fprintf(combinedDataFile, "\n");
    }

    fclose(combinedDataFile);
}

/**
 * Read Jaggedarray
 * RETURNS:
 *  - Jaggedarray struct pointer
 * INPUTE:
 *  - filepath
*/
struct Jaggedarray* read_from_file_ja(char* filepath) {
    if(!ends_with_string(filepath, ".jarray")) {
        log_invalid_file_extension(__FILE__,__LINE__);
    }

    FILE *combinedDataFile = fopen(filepath, "r");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return NULL;
    }

    size_t size = 0;
    if (fscanf(combinedDataFile, "%zu", &size) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }

    struct Jaggedarray* read_jagged_array = new_ja(&size);

    for (size_t i = 0; i < size; i++) {
      enum DataType type = 0;
      int type_int = 0;
      size_t num_dimensions = 0;
      size_t dimensions [3] = {0,0,0};

      if (fscanf(combinedDataFile, "%d", &type_int) != 1) {
        log_error_fscan(__FILE__,__LINE__);
      }
      type = type_int;
      if (fscanf(combinedDataFile, "%zu", &num_dimensions) != 1) {
        log_error_fscan(__FILE__,__LINE__);
      }
      for (size_t j = 0; j < num_dimensions; j++) {
        if (fscanf(combinedDataFile, "%zu", &dimensions[j]) != 1) {
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
      set_ja(read_jagged_array, result, &(size_t){i});
      free_a(result);
    }

    fclose(combinedDataFile);
    return read_jagged_array;
}

/**
 * print Jaggedarray
 * RETURNS:
 *  - None
 * INPUTE:
 *  - Jaggedarray
*/
void print_ja(struct Jaggedarray* jagged_array) {
    if (jagged_array == NULL) {
      log_null_value(__FILE__, __LINE__);
    }
    printf("[\n");
    for (size_t i = 0; i < *jagged_array->size; i++) {
        if (jagged_array->jarray[i] == NULL) {
          printf("  [NULL]\n");
        } else {
          printf("  ");
          print_a(jagged_array->jarray[i]);
        }
    }
    printf("]\n");
}

/**
 * Average of Jaggedarray
 * RETURNS:
 *  - average
 * INPUTE:
 *  - Jaggedarray
*/
void* average_ja(struct Jaggedarray* jagged_array) {
    if (jagged_array == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    if (jagged_array->jarray[0] == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    switch (*jagged_array->jarray[0]->type) {
        case DOUBLE: {
            double* sum = malloc(sizeof(double));
            *sum = 0;
            size_t count = 0;
            for (size_t i = 0; i < *jagged_array->size; i++) {
                if (jagged_array->jarray[i] != NULL) {
                    double* arr = get_a(jagged_array->jarray[i]);
                    for (size_t j = 0; j < get_size_a(jagged_array->jarray[i]); j++) {
                      *sum += arr[j];
                      count += 1;
                    }
                }
            }
            *sum = *sum / count;
            return sum;
            break;
        }
        case INT: {
            double* sum = malloc(sizeof(double));
            *sum = 0;
            size_t count = 0;
            for (size_t i = 0; i < *jagged_array->size; i++) {
                if (jagged_array->jarray[i] != NULL) {
                    int* arr = get_a(jagged_array->jarray[i]);
                    for (size_t j = 0; j < get_size_a(jagged_array->jarray[i]); j++) {
                      *sum += arr[j];
                      count += 1;
                    }
                }
            }
            *sum = *sum / count;
            return sum;
            break;
        }
        case FLOAT: {
            float* sum = malloc(sizeof(double));
            *sum = 0;
            size_t count = 0;
            for (size_t i = 0; i < *jagged_array->size; i++) {
                if (jagged_array->jarray[i] != NULL) {
                    float* arr = get_a(jagged_array->jarray[i]);
                    for (size_t j = 0; j < get_size_a(jagged_array->jarray[i]); j++) {
                      *sum += arr[j];
                      count += 1;
                    }
                }
            }
            *sum = *sum / count;
            return sum;
            break;
        }
        case COMPLEX: {
            double complex* sum = malloc(sizeof(double complex));
            *sum = 0 + 0*I;
            size_t count = 0;
            for (size_t i = 0; i < *jagged_array->size; i++) {
                if (jagged_array->jarray[i] != NULL) {
                    double complex* arr = get_a(jagged_array->jarray[i]);
                    for (size_t j = 0; j < get_size_a(jagged_array->jarray[i]); j++) {
                      *sum += arr[j];
                      count += 1;
                    }
                }
            }
            *sum = *sum / count;
            return sum;
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }

    log_invalid_type(__FILE__, __LINE__);

    return NULL;
}

/**
 * Norm of Jaggedarray
 * RETURNS:
 *  - norm
 * INPUTE:
 *  - Jaggedarray
*/
void* norm_ja(struct Jaggedarray* jagged_array) {
    if (jagged_array == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    if (jagged_array->jarray[0] == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    switch (*jagged_array->jarray[0]->type) {
        case DOUBLE: {
            double* norm = malloc(sizeof(double));
            *norm = 0;
            for (size_t i = 0; i < *jagged_array->size; i++) {
                if (jagged_array->jarray[i] != NULL) {
                    double* arr = get_a(jagged_array->jarray[i]);
                    for (size_t j = 0; j < get_size_a(jagged_array->jarray[i]); j++) {
                      *norm += pow(arr[j],2);
                    }
                }
            }
            *norm = sqrt(*norm);
            return norm;
            break;
        }
        case INT: {
            double* norm = malloc(sizeof(double));
            *norm = 0;
            for (size_t i = 0; i < *jagged_array->size; i++) {
                if (jagged_array->jarray[i] != NULL) {
                    int* arr = get_a(jagged_array->jarray[i]);
                    for (size_t j = 0; j < get_size_a(jagged_array->jarray[i]); j++) {
                      *norm += pow(arr[j],2);
                    }
                }
            }
            *norm = sqrt(*norm);
            return norm;
            break;
        }
        case FLOAT: {
            float* norm = malloc(sizeof(float));
            *norm = 0;
            for (size_t i = 0; i < *jagged_array->size; i++) {
                if (jagged_array->jarray[i] != NULL) {
                    float* arr = get_a(jagged_array->jarray[i]);
                    for (size_t j = 0; j < get_size_a(jagged_array->jarray[i]); j++) {
                      *norm += pow(arr[j],2);
                    }
                }
            }
            *norm = sqrt(*norm);
            return norm;
            break;
        }
        case COMPLEX: {
            double* norm = malloc(sizeof(double));
            *norm = 0; 
            for (size_t i = 0; i < *jagged_array->size; i++) {
                if (jagged_array->jarray[i] != NULL) {
                    double complex* arr = get_a(jagged_array->jarray[i]);
                    for (size_t j = 0; j < get_size_a(jagged_array->jarray[i]); j++) {
                        *norm += pow(cabs(arr[j]),2);
                    }
                }
            }
            *norm = sqrt(*norm);
            return norm;
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }

    log_invalid_type(__FILE__, __LINE__);

    return NULL;
}