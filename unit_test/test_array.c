#include "unity.h"
#include "array.h"
#include "utils.h"


// Define empty setUp and tearDown functions
// void setUp(void) {}
// void tearDown(void) {}

//Testing the creation of an array
void test_new_a_1d_DOUBLE(void) {
    struct array* array = new_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){DOUBLE});
    TEST_ASSERT_NOT_NULL(array);
    TEST_ASSERT_EQUAL_UINT32(1, *array->num_dimensions);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[0]);
    TEST_ASSERT_EQUAL_INT(DOUBLE, *array->type);
    free_a(array);
}

//Testing the creation of an array
void test_new_a_1d_INT(void) {
    struct array* array = new_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){INT});
    TEST_ASSERT_NOT_NULL(array);
    TEST_ASSERT_EQUAL_UINT32(1, *array->num_dimensions);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[0]);
    TEST_ASSERT_EQUAL_INT(INT, *array->type);
    free_a(array);
}

//Testing the creation of an array
void test_new_a_1d_FLOAT(void) {
    struct array* array = new_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){FLOAT});
    TEST_ASSERT_NOT_NULL(array);
    TEST_ASSERT_EQUAL_UINT32(1, *array->num_dimensions);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[0]);
    TEST_ASSERT_EQUAL_INT(FLOAT, *array->type);
    free_a(array);
}

//Testing the creation of an array
void test_new_a_1d_COMPLEX(void) {
    struct array* array = new_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){COMPLEX});
    TEST_ASSERT_NOT_NULL(array);
    TEST_ASSERT_EQUAL_UINT32(1, *array->num_dimensions);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[0]);
    TEST_ASSERT_EQUAL_INT(COMPLEX, *array->type);
    free_a(array);
}

//Testing the creation of an array
void test_new_a_2d_DOUBLE(void) {
    struct array* array = new_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){DOUBLE});
    TEST_ASSERT_NOT_NULL(array);
    TEST_ASSERT_EQUAL_UINT32(2, *array->num_dimensions);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[0]);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[1]);
    TEST_ASSERT_EQUAL_INT(DOUBLE, *array->type);
    free_a(array);
}

//Testing the creation of an array
void test_new_a_2d_INT(void) {
    struct array* array = new_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){INT});
    TEST_ASSERT_NOT_NULL(array);
    TEST_ASSERT_EQUAL_UINT32(2, *array->num_dimensions);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[0]);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[1]);
    TEST_ASSERT_EQUAL_INT(INT, *array->type);
    free_a(array);
}

//Testing the creation of an array
void test_new_a_2d_FLOAT(void) {
    struct array* array = new_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){FLOAT});
    TEST_ASSERT_NOT_NULL(array);
    TEST_ASSERT_EQUAL_UINT32(2, *array->num_dimensions);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[0]);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[1]);
    TEST_ASSERT_EQUAL_INT(FLOAT, *array->type);
    free_a(array);
}

//Testing the creation of an array
void test_new_a_2d_COMPLEX(void) {
    struct array* array = new_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){COMPLEX});
    TEST_ASSERT_NOT_NULL(array);
    TEST_ASSERT_EQUAL_UINT32(2, *array->num_dimensions);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[0]);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[1]);
    TEST_ASSERT_EQUAL_INT(COMPLEX, *array->type);
    free_a(array);
}

//Testing the creation of an array
void test_new_a_3d_DOUBLE(void) {
    struct array* array = new_a(&(size_t){3}, (size_t[]){3,3,3}, &(enum DataType){DOUBLE});
    TEST_ASSERT_NOT_NULL(array);
    TEST_ASSERT_EQUAL_UINT32(3, *array->num_dimensions);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[0]);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[1]);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[2]);
    TEST_ASSERT_EQUAL_INT(DOUBLE, *array->type);
    free_a(array);
}

//Testing the creation of an array
void test_new_a_3d_INT(void) {
    struct array* array = new_a(&(size_t){3}, (size_t[]){3,3,3}, &(enum DataType){INT});
    TEST_ASSERT_NOT_NULL(array);
    TEST_ASSERT_EQUAL_UINT32(3, *array->num_dimensions);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[0]);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[1]);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[2]);
    TEST_ASSERT_EQUAL_INT(INT, *array->type);
    free_a(array);
}

//Testing the creation of an array
void test_new_a_3d_FLOAT(void) {
    struct array* array = new_a(&(size_t){3}, (size_t[]){3,3,3}, &(enum DataType){FLOAT});
    TEST_ASSERT_NOT_NULL(array);
    TEST_ASSERT_EQUAL_UINT32(3, *array->num_dimensions);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[0]);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[1]);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[2]);
    TEST_ASSERT_EQUAL_INT(FLOAT, *array->type);
    free_a(array);
}

//Testing the creation of an array
void test_new_a_3d_COMPLEX(void) {
    struct array* array = new_a(&(size_t){3}, (size_t[]){3,3,3}, &(enum DataType){COMPLEX});
    TEST_ASSERT_NOT_NULL(array);
    TEST_ASSERT_EQUAL_UINT32(3, *array->num_dimensions);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[0]);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[1]);
    TEST_ASSERT_EQUAL_UINT32(3, array->dimensions[2]);
    TEST_ASSERT_EQUAL_INT(COMPLEX, *array->type);
    free_a(array);
}


void test_get_size_1d(void) {
    struct array* array = zeros_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){DOUBLE});
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(3,size);
    free_a(array);
}

void test_get_size_2d(void) {
    struct array* array = zeros_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){DOUBLE});
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(9,size);
    free_a(array);
}

void test_get_size_3d(void) {
    struct array* array = zeros_a(&(size_t){3}, (size_t[]){3,3,3}, &(enum DataType){DOUBLE});
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(27,size);
    free_a(array);
}

void test_get_a(void) {
    struct array* array = zeros_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){DOUBLE});
    double* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    free_a(array);
}

void test_get_element_a(void) {
    struct array* array = zeros_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){DOUBLE});
    double* value = get_element_a(array, (size_t[]){0});
    TEST_ASSERT_EQUAL(0.0, *value);
    free_a(array);
}


// Testing zero_a
void test_zeros_a_1d_DOUBLE(void) {
    struct array* array = zeros_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){DOUBLE});
    double* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(3,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(0.0,arr[i]);
    }
    free_a(array);
}

void test_zeros_a_2d_DOUBLE(void) {
    struct array* array = zeros_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){DOUBLE});
    double* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(9,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(0.0,arr[i]);
    }
    free_a(array);
}

void test_zeros_a_3d_DOUBLE(void) {
    struct array* array = zeros_a(&(size_t){3}, (size_t[]){3,3,3}, &(enum DataType){DOUBLE});
    double* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(27,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(0.0,arr[i]);
    }
    free_a(array);
}

// Testing zero_a
void test_zeros_a_1d_INT(void) {
    struct array* array = zeros_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){INT});
    int* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(3,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(0,arr[i]);
    }
    free_a(array);
}

void test_zeros_a_2d_INT(void) {
    struct array* array = zeros_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){INT});
    int* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(9,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(0,arr[i]);
    }
    free_a(array);
}

void test_zeros_a_3d_INT(void) {
    struct array* array = zeros_a(&(size_t){3}, (size_t[]){3,3,3}, &(enum DataType){INT});
    int* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(27,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(0,arr[i]);
    }
    free_a(array);
}

// Testing zero_a
void test_zeros_a_1d_FLOAT(void) {
    struct array* array = zeros_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){FLOAT});
    float* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(3,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(0,arr[i]);
    }
    free_a(array);
}

void test_zeros_a_2d_FLOAT(void) {
    struct array* array = zeros_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){FLOAT});
    float* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(9,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(0,arr[i]);
    }
    free_a(array);
}

void test_zeros_a_3d_FLOAT(void) {
    struct array* array = zeros_a(&(size_t){3}, (size_t[]){3,3,3}, &(enum DataType){FLOAT});
    float* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(27,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(0.0,arr[i]);
    }
    free_a(array);
}

// Testing zero_a
void test_zeros_a_1d_COMPLEX(void) {
    struct array* array = zeros_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){COMPLEX});
    double complex* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(3,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(0 + 0*I,arr[i]);
    }
    free_a(array);
}

void test_zeros_a_2d_COMPLEX(void) {
    struct array* array = zeros_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){COMPLEX});
    double complex* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(9,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(0 + 0*I,arr[i]);
    }
    free_a(array);
}

void test_zeros_a_3d_COMPLEX(void) {
    struct array* array = zeros_a(&(size_t){3}, (size_t[]){3,3,3}, &(enum DataType){COMPLEX});
    double complex* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(27,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(0 + 0*I,arr[i]);
    }
    free_a(array);
}

// Testing ones_a
void test_ones_a_1d_DOUBLE(void) {
    struct array* array = ones_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){DOUBLE});
    double* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(3,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(1.0,arr[i]);
    }
    free_a(array);
}

void test_ones_a_2d_DOUBLE(void) {
    struct array* array = ones_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){DOUBLE});
    double* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(9,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(1.0,arr[i]);
    }
    free_a(array);
}

void test_ones_a_3d_DOUBLE(void) {
    struct array* array = ones_a(&(size_t){3}, (size_t[]){3,3,3}, &(enum DataType){DOUBLE});
    double* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(27,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(1.0,arr[i]);
    }
    free_a(array);
}

// Testing zero_a
void test_ones_a_1d_INT(void) {
    struct array* array = ones_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){INT});
    int* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(3,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(1,arr[i]);
    }
    free_a(array);
}

void test_ones_a_2d_INT(void) {
    struct array* array = ones_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){INT});
    int* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(9,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(1,arr[i]);
    }
    free_a(array);
}

void test_ones_a_3d_INT(void) {
    struct array* array = ones_a(&(size_t){3}, (size_t[]){3,3,3}, &(enum DataType){INT});
    int* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(27,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(1,arr[i]);
    }
    free_a(array);
}

// Testing zero_a
void test_ones_a_1d_FLOAT(void) {
    struct array* array = ones_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){FLOAT});
    float* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(3,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(1.0,arr[i]);
    }
    free_a(array);
}

void test_ones_a_2d_FLOAT(void) {
    struct array* array = ones_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){FLOAT});
    float* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(9,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(1.0,arr[i]);
    }
    free_a(array);
}

void test_ones_a_3d_FLOAT(void) {
    struct array* array = ones_a(&(size_t){3}, (size_t[]){3,3,3}, &(enum DataType){FLOAT});
    float* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(27,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(1.0,arr[i]);
    }
    free_a(array);
}

// Testing zero_a
void test_ones_a_1d_COMPLEX(void) {
    struct array* array = ones_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){COMPLEX});
    double complex* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(3,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(1 + 0*I,arr[i]);
    }
    free_a(array);
}

void test_ones_a_2d_COMPLEX(void) {
    struct array* array = ones_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){COMPLEX});
    double complex* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(9,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(1 + 0*I,arr[i]);
    }
    free_a(array);
}

void test_ones_a_3d_COMPLEX(void) {
    struct array* array = ones_a(&(size_t){3}, (size_t[]){3,3,3}, &(enum DataType){COMPLEX});
    double complex* arr = get_a(array);
    TEST_ASSERT_EQUAL(arr,array->array);
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(27,size);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(1 + 0*I,arr[i]);
    }
    free_a(array);
}

void test_set_element_a_DOUBLE(void) {
    struct array* array = ones_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){DOUBLE});
    set_element_a(array, (size_t[]){1}, &(double){3});

    double* arr = get_a(array);
    size_t size = get_size_a(array);
    double arr_test[] = {1.0,3.0,1.0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }
    free_a(array);
}

void test_set_element_a_INT(void) {
    struct array* array = ones_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){INT});
    set_element_a(array, (size_t[]){1}, &(int){3});

    int* arr = get_a(array);
    size_t size = get_size_a(array);
    int arr_test[] = {1,3,1};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }
    free_a(array);
}

void test_set_element_a_FLOAT(void) {
    struct array* array = ones_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){FLOAT});
    set_element_a(array, (size_t[]){1}, &(float){3});

    float* arr = get_a(array);
    size_t size = get_size_a(array);
    float arr_test[] = {1.0,3.0,1.0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }
    free_a(array);
}

void test_set_element_a_COMPLEX(void) {
    struct array* array = ones_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){COMPLEX});
    set_element_a(array, (size_t[]){1}, &(double complex){3 + 0*I});

    double complex* arr = get_a(array);
    size_t size = get_size_a(array);
    double complex arr_test[] = {1 + 0*I,3 + 0*I,1 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }
    free_a(array);
}

void test_set_a_1d_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });

    double* arr = get_a(array);
    size_t size = get_size_a(array);
    double arr_test[] = {1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
}

void test_set_a_1d_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });

    int* arr = get_a(array);
    size_t size = get_size_a(array);
    int arr_test[] = {1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
}

void test_set_a_1d_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });

    float* arr = get_a(array);
    size_t size = get_size_a(array);
    float arr_test[] = {1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
}

void test_set_a_1d_COMPLEX(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    double complex* arr = get_a(array);
    size_t size = get_size_a(array);
    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
}

void test_set_a_2d_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    double* arr = get_a(array);
    size_t size = get_size_a(array);
    double arr_test[] = {1,2,3,1,2,3,1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
}

void test_set_a_2d_INT(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    int* arr = get_a(array);
    size_t size = get_size_a(array);
    int arr_test[] = {1,2,3,1,2,3,1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
}

void test_set_a_2d_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    float* arr = get_a(array);
    size_t size = get_size_a(array);
    float arr_test[] = {1,2,3,1,2,3,1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
}

void test_set_a_2d_COMPLEX(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    double complex* arr = get_a(array);
    size_t size = get_size_a(array);
    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I,1 + 0*I,2 + 0*I,3 + 0*I,1 + 0*I,2 + 0*I,3 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
}

void test_set_a_3d_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                    });

    double* arr = get_a(array);
    size_t size = get_size_a(array);
    double arr_test[] = {1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
}

void test_set_a_3d_INT(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3][3]){
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                    });

    int* arr = get_a(array);
    size_t size = get_size_a(array);
    int arr_test[] = {1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
}

void test_set_a_3d_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3][3]){
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                    });

    float* arr = get_a(array);
    size_t size = get_size_a(array);
    float arr_test[] = {1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
}

void test_set_a_3d_COMPLEX(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3][3]){
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                         {1 + 0*I,2 + 0*I,3 + 0*I},
                                         {1 + 0*I,2 + 0*I,3 + 0*I}},
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                         {1 + 0*I,2 + 0*I,3 + 0*I},
                                         {1 + 0*I,2 + 0*I,3 + 0*I}},
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                         {1 + 0*I,2 + 0*I,3 + 0*I},
                                         {1 + 0*I,2 + 0*I,3 + 0*I}},
                                    });

    double complex* arr = get_a(array);
    size_t size = get_size_a(array);
    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I, 1 + 0*I,2 + 0*I,3 + 0*I, 1 + 0*I,2 + 0*I,3 + 0*I, 1 + 0*I,2 + 0*I,3 + 0*I, 1 + 0*I,2 + 0*I,3 + 0*I, 1 + 0*I,2 + 0*I,3 + 0*I, 1 + 0*I,2 + 0*I,3 + 0*I, 1 + 0*I,2 + 0*I,3 + 0*I, 1 + 0*I,2 + 0*I,3 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
}

//Multiply
void test_multiply_a_DOUBLE_DOUBLE(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });

    struct array* result = multiply_a(array1, array2);
    double* arr = get_a(result);
    size_t size = get_size_a(result);
    double arr_test[] = {1,4,9};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_multiply_a_INT_INT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });

    struct array* result = multiply_a(array1, array2);
    int* arr = get_a(result);
    size_t size = get_size_a(result);
    int arr_test[] = {1,4,9};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_multiply_a_FLOAT_FLOAT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });

    struct array* result = multiply_a(array1, array2);
    float* arr = get_a(result);
    size_t size = get_size_a(result);
    float arr_test[] = {1,4,9};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_multiply_a_COMPLEX_COMPLEX(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* result = multiply_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {1 + 0*I,4 + 0*I,9 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_multiply_a_COMPLEX_DOUBLE(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });

    struct array* result = multiply_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {1 + 0*I,4 + 0*I,9 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_multiply_a_COMPLEX_INT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });

    struct array* result = multiply_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {1 + 0*I,4 + 0*I,9 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_multiply_a_COMPLEX_FLOAT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });

    struct array* result = multiply_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {1 + 0*I,4 + 0*I,9 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

//Divide
void test_divide_a_DOUBLE_DOUBLE(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });

    struct array* result = divide_a(array1, array2);
    double* arr = get_a(result);
    size_t size = get_size_a(result);
    double arr_test[] = {1,1,1};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_divide_a_INT_INT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });

    struct array* result = divide_a(array1, array2);
    int* arr = get_a(result);
    size_t size = get_size_a(result);
    int arr_test[] = {1,1,1};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_divide_a_FLOAT_FLOAT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });

    struct array* result = divide_a(array1, array2);
    float* arr = get_a(result);
    size_t size = get_size_a(result);
    float arr_test[] = {1,1,1};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_divide_a_COMPLEX_COMPLEX(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* result = divide_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {1 + 0*I,1 + 0*I,1 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_divide_a_COMPLEX_DOUBLE(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });

    struct array* result = divide_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {1 + 0*I,1 + 0*I,1 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_divide_a_COMPLEX_INT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });

    struct array* result = divide_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {1 + 0*I,1 + 0*I,1 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_divide_a_COMPLEX_FLOAT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });

    struct array* result = divide_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {1 + 0*I,1 + 0*I,1 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

//Addition
void test_addition_a_DOUBLE_DOUBLE(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });

    struct array* result = addition_a(array1, array2);
    double* arr = get_a(result);
    size_t size = get_size_a(result);
    double arr_test[] = {2,4,6};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_addition_a_INT_INT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });

    struct array* result = addition_a(array1, array2);
    int* arr = get_a(result);
    size_t size = get_size_a(result);
    int arr_test[] = {2,4,6};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_addition_a_FLOAT_FLOAT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });

    struct array* result = addition_a(array1, array2);
    float* arr = get_a(result);
    size_t size = get_size_a(result);
    float arr_test[] = {2,4,6};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_addition_a_COMPLEX_COMPLEX(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* result = addition_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {2 + 0*I,4 + 0*I,6 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_addition_a_COMPLEX_DOUBLE(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });

    struct array* result = addition_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {2 + 0*I,4 + 0*I,6 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_addition_a_COMPLEX_INT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });

    struct array* result = addition_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {2 + 0*I,4 + 0*I,6 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_addition_a_COMPLEX_FLOAT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });

    struct array* result = addition_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {2 + 0*I,4 + 0*I,6 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

//Minus
void test_minus_a_DOUBLE_DOUBLE(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });

    struct array* result = minus_a(array1, array2);
    double* arr = get_a(result);
    size_t size = get_size_a(result);
    double arr_test[] = {0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_minus_a_INT_INT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });

    struct array* result = minus_a(array1, array2);
    int* arr = get_a(result);
    size_t size = get_size_a(result);
    int arr_test[] = {0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_minus_a_FLOAT_FLOAT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });

    struct array* result = minus_a(array1, array2);
    float* arr = get_a(result);
    size_t size = get_size_a(result);
    float arr_test[] = {0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_minus_a_COMPLEX_COMPLEX(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* result = minus_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {0 + 0*I,0 + 0*I,0 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_minus_a_COMPLEX_DOUBLE(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });

    struct array* result = minus_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {0 + 0*I,0 + 0*I,0 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_minus_a_COMPLEX_INT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });

    struct array* result = minus_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {0 + 0*I,0 + 0*I,0 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_minus_a_COMPLEX_FLOAT(void) {
    struct array* array1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });
    struct array* array2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });

    struct array* result = minus_a(array1, array2);
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {0 + 0*I,0 + 0*I,0 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array1);
    free_a(array2);
    free_a(result);
}

void test_get_2d_index_a(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    size_t index = get_2d_index_a(array, (size_t[]){1,1});
    TEST_ASSERT_EQUAL(4,index);
    free_a(array);
}

void test_get_3d_index_a(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3][3]){
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                    });
    size_t index = get_3d_index_a(array, (size_t[]){1,1,1});
    TEST_ASSERT_EQUAL(13,index);

    free_a(array);
}

void test_get_size_a_1d(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(3,size);
    free_a(array);
}

void test_get_size_a_2d(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(9,size);
    free_a(array);
}

void test_get_size_a_3d(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3][3]){
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                        {{1,2,3},
                                         {1,2,3},
                                         {1,2,3}},
                                    });
    size_t size = get_size_a(array);
    TEST_ASSERT_EQUAL(27,size);
    free_a(array);
}

void test_linspace_a_DOUBLE(void) {
    struct array* linspace = linspace_a(&(double){0}, &(double){10}, &(size_t){5}, &(enum DataType){DOUBLE});
    double* arr = get_a(linspace);
    size_t size = get_size_a(linspace);
    double arr_test[] = {0,2.5,5,7.5,10};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }
    free_a(linspace);
}

void test_linspace_a_FLOAT(void) {
    struct array* linspace = linspace_a(&(float){0}, &(float){10}, &(size_t){5}, &(enum DataType){FLOAT});
    // print_a(linspace);
    float* arr = get_a(linspace);
    size_t size = get_size_a(linspace);
    float arr_test[] = {0,2.5,5,7.5,10};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }
    free_a(linspace);
}

void test_multiply_constant_a_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,3
                                    });

    struct array* result = multiply_constant_a(array, &(double){2}, &(enum DataType){DOUBLE});
    double* arr = get_a(result);
    size_t size = get_size_a(result);
    double arr_test[] = {2,4,6};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_multiply_constant_a_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,3
                                    });

    struct array* result = multiply_constant_a(array, &(int){2}, &(enum DataType){INT});
    int* arr = get_a(result);
    size_t size = get_size_a(result);
    int arr_test[] = {2,4,6};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_multiply_constant_a_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,3
                                    });

    struct array* result = multiply_constant_a(array, &(float){2}, &(enum DataType){FLOAT});
    float* arr = get_a(result);
    size_t size = get_size_a(result);
    float arr_test[] = {2,4,6};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_multiply_constant_a_COMPLEX_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 3 + 0*I
                                    });

    struct array* result = multiply_constant_a(array, &(double){2}, &(enum DataType){DOUBLE});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {2 + 0*I,4 + 0*I,6 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_multiply_constant_a_COMPLEX_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 3 + 0*I
                                    });

    struct array* result = multiply_constant_a(array, &(int){2}, &(enum DataType){INT});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {2 + 0*I,4 + 0*I,6 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_multiply_constant_a_COMPLEX_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 3 + 0*I
                                    });

    struct array* result = multiply_constant_a(array, &(float){2}, &(enum DataType){FLOAT});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {2 + 0*I,4 + 0*I,6 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_multiply_constant_a_COMPLEX_COMPLEX(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 3 + 0*I
                                    });

    struct array* result = multiply_constant_a(array, &(double complex){2 + 0*I}, &(enum DataType){COMPLEX});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {2 + 0*I,4 + 0*I,6 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_division_constant_a_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,4
                                    });

    struct array* result = division_constant_a(array, &(double){2}, &(enum DataType){DOUBLE});
    double* arr = get_a(result);
    size_t size = get_size_a(result);
    double arr_test[] = {0.5,1,2};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_division_constant_a_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,4
                                    });

    struct array* result = division_constant_a(array, &(int){2}, &(enum DataType){INT});
    int* arr = get_a(result);
    size_t size = get_size_a(result);
    int arr_test[] = {0,1,2};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_division_constant_a_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,4
                                    });

    struct array* result = division_constant_a(array, &(float){2}, &(enum DataType){FLOAT});
    float* arr = get_a(result);
    size_t size = get_size_a(result);
    float arr_test[] = {0.5,1,2};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_division_constant_a_COMPLEX_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 4 + 0*I
                                    });

    struct array* result = division_constant_a(array, &(double){2}, &(enum DataType){DOUBLE});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {0.5 + 0*I,1 + 0*I,2 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_division_constant_a_COMPLEX_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 4 + 0*I
                                    });

    struct array* result = division_constant_a(array, &(int){2}, &(enum DataType){INT});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {0.5 + 0*I,1 + 0*I,2 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_division_constant_a_COMPLEX_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 4 + 0*I
                                    });

    struct array* result = division_constant_a(array, &(float){2}, &(enum DataType){FLOAT});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {0.5 + 0*I,1 + 0*I,2 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_division_constant_a_COMPLEX_COMPLEX(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 4 + 0*I
                                    });

    struct array* result = division_constant_a(array, &(double complex){2 + 0*I}, &(enum DataType){COMPLEX});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {0.5 + 0*I,1 + 0*I,2 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_minus_constant_a_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,4
                                    });

    struct array* result = minus_constant_a(array, &(double){2}, &(enum DataType){DOUBLE});
    double* arr = get_a(result);
    size_t size = get_size_a(result);
    double arr_test[] = {-1,0,2};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_minus_constant_a_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,4
                                    });

    struct array* result = minus_constant_a(array, &(int){2}, &(enum DataType){INT});
    int* arr = get_a(result);
    size_t size = get_size_a(result);
    int arr_test[] = {-1,0,2};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_minus_constant_a_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,4
                                    });

    struct array* result = minus_constant_a(array, &(float){2}, &(enum DataType){FLOAT});
    float* arr = get_a(result);
    size_t size = get_size_a(result);
    float arr_test[] = {-1,0,2};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_minus_constant_a_COMPLEX_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 4 + 0*I
                                    });

    struct array* result = minus_constant_a(array, &(double){2}, &(enum DataType){DOUBLE});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {-1 + 0*I,0 + 0*I,2 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_minus_constant_a_COMPLEX_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 4 + 0*I
                                    });

    struct array* result = minus_constant_a(array, &(int){2}, &(enum DataType){INT});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {-1 + 0*I,0 + 0*I,2 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_minus_constant_a_COMPLEX_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 4 + 0*I
                                    });

    struct array* result = minus_constant_a(array, &(float){2}, &(enum DataType){FLOAT});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {-1 + 0*I,0 + 0*I,2 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_minus_constant_a_COMPLEX_COMPLEX(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 4 + 0*I
                                    });

    struct array* result = minus_constant_a(array, &(double complex){2 + 0*I}, &(enum DataType){COMPLEX});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {-1 + 0*I,0 + 0*I,2 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_addition_constant_a_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1, 2,4
                                    });

    struct array* result = addition_constant_a(array, &(double){2}, &(enum DataType){DOUBLE});
    double* arr = get_a(result);
    size_t size = get_size_a(result);
    double arr_test[] = {3,4,6};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_addition_constant_a_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1, 2,4
                                    });

    struct array* result = addition_constant_a(array, &(int){2}, &(enum DataType){INT});
    int* arr = get_a(result);
    size_t size = get_size_a(result);
    int arr_test[] = {3,4,6};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_addition_constant_a_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1, 2,4
                                    });

    struct array* result = addition_constant_a(array, &(float){2}, &(enum DataType){FLOAT});
    float* arr = get_a(result);
    size_t size = get_size_a(result);
    float arr_test[] = {3,4,6};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_addition_constant_a_COMPLEX_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 4 + 0*I
                                    });

    struct array* result = addition_constant_a(array, &(double){2}, &(enum DataType){DOUBLE});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {3 + 0*I,4 + 0*I,6 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_addition_constant_a_COMPLEX_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 4 + 0*I
                                    });

    struct array* result = addition_constant_a(array, &(int){2}, &(enum DataType){INT});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {3 + 0*I,4 + 0*I,6 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_addition_constant_a_COMPLEX_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 4 + 0*I
                                    });

    struct array* result = addition_constant_a(array, &(float){2}, &(enum DataType){FLOAT});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {3 + 0*I,4 + 0*I,6 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_addition_constant_a_COMPLEX_COMPLEX(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I, 2 + 0*I, 4 + 0*I
                                    });

    struct array* result = addition_constant_a(array, &(double complex){2 + 0*I}, &(enum DataType){COMPLEX});
    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {3 + 0*I,4 + 0*I,6 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    }                                
    free_a(array);
    free_a(result);
}

void test_max_a_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        -15, 2,4
                                    });
    double* result = max_a(array);
    TEST_ASSERT_EQUAL(4,*result);
    free(result);
    free_a(array);
}

void test_max_a_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        -15, 2,4
                                    });
    int* result = max_a(array);
    TEST_ASSERT_EQUAL(4,*result);
    free(result);
    free_a(array);
}

void test_max_a_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        -15, 2,4
                                    });
    float* result = max_a(array);
    TEST_ASSERT_EQUAL(4,*result);
    free(result);
    free_a(array);
}

void test_min_a_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        -15, 2,4
                                    });
    double* result = min_a(array);
    TEST_ASSERT_EQUAL(-15,*result);
    free(result);
    free_a(array);
}

void test_min_a_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        -15, 2,4
                                    });
    int* result = min_a(array);
    TEST_ASSERT_EQUAL(-15,*result);
    free(result);
    free_a(array);
}

void test_min_a_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        -15, 2,4
                                    });
    float* result = min_a(array);
    TEST_ASSERT_EQUAL(-15,*result);
    free(result);
    free_a(array);
}

void test_amax_a_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        -15, 2,4
                                    });
    double* result = amax_a(array);
    TEST_ASSERT_EQUAL(15,*result);
    free(result);
    free_a(array);
}

void test_amax_a_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        -15, 2,4
                                    });
    int* result = amax_a(array);
    TEST_ASSERT_EQUAL(15,*result);
    free(result);
    free_a(array);
}

void test_amax_a_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        -15, 2,4
                                    });
    float* result = amax_a(array);
    TEST_ASSERT_EQUAL(15,*result);
    free(result);
    free_a(array);
}

void test_copy_over_1d_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* zeros = zeros_a(&(size_t){1},(size_t[]){3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1}, (int[]){-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_1d_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1,2,3
                                    });

    struct array* zeros = zeros_a(&(size_t){1},(size_t[]){3},&(enum DataType){INT});

    copy_over_a(zeros, array, (int[]){-1}, (int[]){-1});

    int* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    int arr_test[] = {1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_1d_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1,2,3
                                    });

    struct array* zeros = zeros_a(&(size_t){1},(size_t[]){3},&(enum DataType){FLOAT});

    copy_over_a(zeros, array, (int[]){-1}, (int[]){-1});

    float* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    float arr_test[] = {1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_1d_COMPLEX(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* zeros = zeros_a(&(size_t){1},(size_t[]){3},&(enum DataType){COMPLEX});

    copy_over_a(zeros, array, (int[]){-1}, (int[]){-1});

    double complex* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_1d_1_0_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,0}, (int[]){-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,2,0,0,3,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_1d_1_0_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1,2,3
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){INT});

    copy_over_a(zeros, array, (int[]){-1,0}, (int[]){-1});

    int* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    int arr_test[] = {1,0,0,2,0,0,3,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_1d_1_0_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1,2,3
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){FLOAT});

    copy_over_a(zeros, array, (int[]){-1,0}, (int[]){-1});

    float* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    float arr_test[] = {1,0,0,2,0,0,3,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_1d_1_0_COMPLEX(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){COMPLEX});

    copy_over_a(zeros, array, (int[]){-1,0}, (int[]){-1});

    double complex* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double complex arr_test[] = {1 + 0*I,0 + 0*I,0 + 0*I,2 + 0*I,0 + 0*I,0 + 0*I,3 + 0*I,0 + 0*I,0 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_1d_0_1_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,-1}, (int[]){-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3,0,0,0,0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_1d_0_1_INT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1,2,3
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){INT});

    copy_over_a(zeros, array, (int[]){0,-1}, (int[]){-1});

    int* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    int arr_test[] = {1,2,3,0,0,0,0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_1d_0_1_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1,2,3
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){FLOAT});

    copy_over_a(zeros, array, (int[]){0,-1}, (int[]){-1});

    float* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    float arr_test[] = {1,2,3,0,0,0,0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_1d_0_1_COMPLEX(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){COMPLEX});

    copy_over_a(zeros, array, (int[]){0,-1}, (int[]){-1});

    double complex* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I,0 + 0*I,0 + 0*I,0 + 0*I,0 + 0*I,0 + 0*I,0 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_2d_1_1_to_1_1_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,-1}, (int[]){-1,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3,1,2,3,1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_2d_1_1_to_1_1_INT(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){INT});

    copy_over_a(zeros, array, (int[]){-1,-1}, (int[]){-1,-1});

    int* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    int arr_test[] = {1,2,3,1,2,3,1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_2d_1_1_to_1_1_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){FLOAT});

    copy_over_a(zeros, array, (int[]){-1,-1}, (int[]){-1,-1});

    float* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    float arr_test[] = {1,2,3,1,2,3,1,2,3};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_2d_1_1_to_1_1_COMPLEX(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){COMPLEX});

    copy_over_a(zeros, array, (int[]){-1,-1}, (int[]){-1,-1});

    double complex* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I,1 + 0*I,2 + 0*I,3 + 0*I,1 + 0*I,2 + 0*I,3 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_2d_1_0_to_1_0(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,0}, (int[]){-1,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,1,0,0,1,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_2d_0_1_to_0_1(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,-1}, (int[]){0,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3,0,0,0,0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_2d_1_0_to_0_1(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,0}, (int[]){0,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,2,0,0,3,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_2d_to_2d_0_1_to_1_0(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){2},(size_t[]){3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,-1}, (int[]){-1,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,1,1,0,0,0,0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_1d_1_0_0_to_1(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,0,0}, (int[]){-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         0,0,0,
                         0,0,0,

                         2,0,0,
                         0,0,0,
                         0,0,0,

                         3,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_1d_0_1_0_to_1(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,-1,0}, (int[]){-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         2,0,0,
                         3,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_1d_0_0_1_to_1(void) {
    struct array* array = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,0,-1}, (int[]){-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_2d_1_1_0_to_1_1(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,-1,0}, (int[]){-1,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         2,0,0,
                         3,0,0,

                         1,0,0,
                         2,0,0,
                         3,0,0,

                         1,0,0,
                         2,0,0,
                         3,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_2d_1_0_1_to_1_1(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,0,-1}, (int[]){-1,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3,
                         0,0,0,
                         0,0,0,

                         1,2,3,
                         0,0,0,
                         0,0,0,

                         1,2,3,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_2d_0_1_1_to_1_1(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,-1,-1}, (int[]){-1,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_2d_0_0_1_to_0_1(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,0,-1}, (int[]){0,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_2d_0_0_1_to_1_0(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,0,-1}, (int[]){-1,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,1,1,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_2d_0_1_0_to_0_1(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,-1,0}, (int[]){0,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         2,0,0,
                         3,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_2d_0_1_0_to_1_0(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,-1,0}, (int[]){-1,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         1,0,0,
                         1,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_2d_1_0_0_to_0_1(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,0,0}, (int[]){0,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         0,0,0,
                         0,0,0,

                         2,0,0,
                         0,0,0,
                         0,0,0,

                         3,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_2d_1_0_0_to_1_0(void) {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,0,0}, (int[]){-1,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         0,0,0,
                         0,0,0,

                         1,0,0,
                         0,0,0,
                         0,0,0,

                         1,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_1_1_1_to_1_1_1_DOUBLE(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3,
                         4,5,6,
                         7,8,9,

                         1,2,3,
                         4,5,6,
                         7,8,9,

                         1,2,3,
                         4,5,6,
                         7,8,9};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_1_1_1_to_1_1_1_INT(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){INT});

    copy_over_a(zeros, array, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});

    int* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    int arr_test[] = {1,2,3,
                         4,5,6,
                         7,8,9,

                         1,2,3,
                         4,5,6,
                         7,8,9,

                         1,2,3,
                         4,5,6,
                         7,8,9};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_1_1_1_to_1_1_1_FLOAT(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){FLOAT});

    copy_over_a(zeros, array, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});

    float* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    float arr_test[] = {1,2,3,
                         4,5,6,
                         7,8,9,

                         1,2,3,
                         4,5,6,
                         7,8,9,

                         1,2,3,
                         4,5,6,
                         7,8,9};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_1_1_1_to_1_1_1_COMPLEX(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3][3]){
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                        {4 + 0*I,5 + 0*I,6 + 0*I},
                                        {7 + 0*I,8 + 0*I,9 + 0*I},},
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                        {4 + 0*I,5 + 0*I,6 + 0*I},
                                        {7 + 0*I,8 + 0*I,9 + 0*I},},
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                        {4 + 0*I,5 + 0*I,6 + 0*I},
                                        {7 + 0*I,8 + 0*I,9 + 0*I},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){COMPLEX});

    copy_over_a(zeros, array, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});

    double complex* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I,
                         4 + 0*I,5 + 0*I,6 + 0*I,
                         7 + 0*I,8 + 0*I,9 + 0*I,

                         1 + 0*I,2 + 0*I,3 + 0*I,
                         4 + 0*I,5 + 0*I,6 + 0*I,
                         7 + 0*I,8 + 0*I,9 + 0*I,

                         1 + 0*I,2 + 0*I,3 + 0*I,
                         4 + 0*I,5 + 0*I,6 + 0*I,
                         7 + 0*I,8 + 0*I,9 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_1_1_0_to_1_1_0(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,-1,0}, (int[]){-1,-1,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         4,0,0,
                         7,0,0,

                         1,0,0,
                         4,0,0,
                         7,0,0,

                         1,0,0,
                         4,0,0,
                         7,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_1_1_0_to_1_0_1(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,-1,0}, (int[]){-1,-0,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         2,0,0,
                         3,0,0,

                         1,0,0,
                         2,0,0,
                         3,0,0,

                         1,0,0,
                         2,0,0,
                         3,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_1_1_0_to_0_1_1(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,-1,0}, (int[]){0,-1,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         2,0,0,
                         3,0,0,

                         4,0,0,
                         5,0,0,
                         6,0,0,

                         7,0,0,
                         8,0,0,
                         9,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_1_0_1_to_1_1_0(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,0,-1}, (int[]){-1,-1,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,4,7,
                         0,0,0,
                         0,0,0,

                         1,4,7,
                         0,0,0,
                         0,0,0,

                         1,4,7,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_1_0_1_to_1_0_1(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,0,-1}, (int[]){-1,-0,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3,
                         0,0,0,
                         0,0,0,

                         1,2,3,
                         0,0,0,
                         0,0,0,

                         1,2,3,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_1_0_1_to_0_1_1(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,0,-1}, (int[]){0,-1,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3,
                         0,0,0,
                         0,0,0,

                         4,5,6,
                         0,0,0,
                         0,0,0,

                         7,8,9,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_0_1_1_to_1_1_0(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,-1,-1}, (int[]){-1,-1,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,4,7,
                         1,4,7,
                         1,4,7,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_0_1_1_to_1_0_1(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,-1,-1}, (int[]){-1,-0,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_0_1_1_to_0_1_1(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,-1,-1}, (int[]){0,-1,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3,
                         4,5,6,
                         7,8,9,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_0_0_1_to_0_1_0(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,0,-1}, (int[]){0,-1,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,4,7,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_0_0_1_to_1_0_0(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,0,-1}, (int[]){-1,-0,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,1,1,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_0_0_1_to_0_0_1(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,0,-1}, (int[]){0,0,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,2,3,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_0_1_0_to_0_1_0(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,-1,0}, (int[]){0,-1,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         4,0,0,
                         7,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_0_1_0_to_1_0_0(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,-1,0}, (int[]){-1,-0,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         1,0,0,
                         1,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_0_1_0_to_0_0_1(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){0,-1,0}, (int[]){0,0,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         2,0,0,
                         3,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0,

                         0,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_1_0_0_to_0_1_0(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,0,0}, (int[]){0,-1,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         0,0,0,
                         0,0,0,

                         4,0,0,
                         0,0,0,
                         0,0,0,

                         7,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_1_0_0_to_1_0_0(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,0,0}, (int[]){-1,-0,0});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         0,0,0,
                         0,0,0,

                         1,0,0,
                         0,0,0,
                         0,0,0,

                         1,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}

void test_copy_over_3d_to_3d_1_0_0_to_0_0_1(void) {
    struct array* array = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                        {{1,2,3},
                                        {4,5,6},
                                        {7,8,9},},
                                    });

    struct array* zeros = zeros_a(&(size_t){3},(size_t[]){3,3,3},&(enum DataType){DOUBLE});

    copy_over_a(zeros, array, (int[]){-1,0,0}, (int[]){0,0,-1});

    double* arr = get_a(zeros);
    size_t size = get_size_a(zeros);
    double arr_test[] = {1,0,0,
                         0,0,0,
                         0,0,0,

                         2,0,0,
                         0,0,0,
                         0,0,0,

                         3,0,0,
                         0,0,0,
                         0,0,0};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(zeros);
    free_a(array);
}



void test_dot_a_1d_double_1d_double() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double arr_test[] = {14};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_double_1d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                        (size_t[]){3}, 
                                        &(enum DataType){COMPLEX}, 
                                        (double complex[3]){
                                            1 + 0*I,2 + 0*I,3 + 0*I
                                        });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_int_1d_int() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1,2,3
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1,2,3
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    int* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    int arr_test[] = {14};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_int_1d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1,2,3
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                        (size_t[]){3}, 
                                        &(enum DataType){COMPLEX}, 
                                        (double complex[3]){
                                            1 + 0*I,2 + 0*I,3 + 0*I
                                        });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_float_1d_float() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1,2,3
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1,2,3
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    float* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    float arr_test[] = {14};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_float_1d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1,2,3
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                        (size_t[]){3}, 
                                        &(enum DataType){COMPLEX}, 
                                        (double complex[3]){
                                            1 + 0*I,2 + 0*I,3 + 0*I
                                        });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_complex_1d_double() {

    struct array* arr_1 = new_set_a(&(size_t){1},
                                        (size_t[]){3}, 
                                        &(enum DataType){COMPLEX}, 
                                        (double complex[3]){
                                            1 + 0*I,2 + 0*I,3 + 0*I
                                        });
    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_complex_1d_int() {

    struct array* arr_1 = new_set_a(&(size_t){1},
                                        (size_t[]){3}, 
                                        &(enum DataType){COMPLEX}, 
                                        (double complex[3]){
                                            1 + 0*I,2 + 0*I,3 + 0*I
                                        });
    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1,2,3
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_complex_1d_float() {

    struct array* arr_1 = new_set_a(&(size_t){1},
                                        (size_t[]){3}, 
                                        &(enum DataType){COMPLEX}, 
                                        (double complex[3]){
                                            1 + 0*I,2 + 0*I,3 + 0*I
                                        });
    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1,2,3
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_complex_1d_complex() {

    struct array* arr_1 = new_set_a(&(size_t){1},
                                        (size_t[]){3}, 
                                        &(enum DataType){COMPLEX}, 
                                        (double complex[3]){
                                            1 + 0*I,2 + 0*I,3 + 0*I
                                        });
    struct array* arr_2 = new_set_a(&(size_t){1},
                                        (size_t[]){3}, 
                                        &(enum DataType){COMPLEX}, 
                                        (double complex[3]){
                                            1 + 0*I,2 + 0*I,3 + 0*I
                                        });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_double_2d_double() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double arr_test[] = {6,12,18};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_double_2d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_int_2d_int() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1,2,3
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    int* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    int arr_test[] = {6,12,18};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_int_2d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1,2,3
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_float_2d_float() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1,2,3
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    float* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    float arr_test[] = {6,12,18};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_float_2d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1,2,3
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_complex_2d_double() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_complex_2d_int() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_complex_2d_float() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_1d_complex_2d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}


void test_dot_a_2d_double_1d_double() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double arr_test[] = {14,14,14};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_double_1d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I,14 + 0*I,14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_int_1d_int() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1,2,3
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    int* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    int arr_test[] = {14,14,14};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_int_1d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I,14 + 0*I,14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}


void test_dot_a_2d_float_1d_float() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1,2,3
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    float* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    float arr_test[] = {14,14,14};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_float_1d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I,14 + 0*I,14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_complex_1d_double() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I,14 + 0*I,14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_complex_1d_int() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1,2,3
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I,14 + 0*I,14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_complex_1d_float() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1,2,3
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I,14 + 0*I,14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_complex_1d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {14 + 0*I,14 + 0*I,14 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_double_2d_double() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double arr_test[] = {6,12,18,
                         6,12,18,
                         6,12,18,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_double_2d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_int_2d_int() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    int* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    int arr_test[] = {6,12,18,
                         6,12,18,
                         6,12,18,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_int_2d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_float_2d_float() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    float* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    float arr_test[] = {6,12,18,
                         6,12,18,
                         6,12,18,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_float_2d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_complex_2d_double() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });
    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });



    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void test_dot_a_2d_complex_2d_int() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });
    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });



    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}


void test_dot_a_2d_complex_2d_float() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });
    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });



    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}


void test_dot_a_2d_complex_2d_complex() {
    struct array* arr_1 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* arr_2 = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* dot_product = dot_a(arr_1, arr_2);

    double complex* arr = get_a(dot_product);
    size_t size = get_size_a(dot_product);
    double complex arr_test[] = {6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,
                         6 + 0*I,12 + 0*I,18 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(arr_1);
    free_a(arr_2);
    free_a(dot_product);
}

void diag_a_1d_double() {
    struct array* arr = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3]){
                                        1,2,3
                                    });

    struct array* diagonal = diag_a(arr, (int[]){-1});

    double* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    double arr_test[] = {1,0,0,
                         0,2,0,
                         0,0,3,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_1d_int() {
    struct array* arr = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){INT}, 
                                    (int[3]){
                                        1,2,3
                                    });

    struct array* diagonal = diag_a(arr, (int[]){-1});

    int* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    int arr_test[] = {1,0,0,
                         0,2,0,
                         0,0,3,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_1d_float() {
    struct array* arr = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3]){
                                        1,2,3
                                    });

    struct array* diagonal = diag_a(arr, (int[]){-1});

    float* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    float arr_test[] = {1,0,0,
                         0,2,0,
                         0,0,3,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_1d_complex() {
    struct array* arr = new_set_a(&(size_t){1},
                                    (size_t[]){3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3]){
                                        1 + 0*I,2 + 0*I,3 + 0*I
                                    });

    struct array* diagonal = diag_a(arr, (int[]){-1});

    double complex* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    double complex arr_test[] = {1 + 0*I,0 + 0*I,0 + 0*I,
                         0 + 0*I,2 + 0*I,0 + 0*I,
                         0 + 0*I,0 + 0*I,3 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_2d_double_1_0() {
    struct array* arr = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){-1,0});

    double* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    double arr_test[] = {1,0,0,
                         0,1,0,
                         0,0,1,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_2d_int_1_0() {
    struct array* arr = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){-1,0});

    int* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    int arr_test[] = {1,0,0,
                         0,1,0,
                         0,0,1,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_2d_float_1_0() {
    struct array* arr = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){-1,0});

    float* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    float arr_test[] = {1,0,0,
                         0,1,0,
                         0,0,1,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_2d_complex_1_0() {
    struct array* arr = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){-1,0});

    double complex* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    double complex arr_test[] = {1 + 0*I,0 + 0*I,0 + 0*I,
                         0 + 0*I,1 + 0*I,0 + 0*I,
                         0 + 0*I,0 + 0*I,1 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_2d_double_0_1() {
    struct array* arr = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){0,-1});

    double* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    double arr_test[] = {1,0,0,
                         0,2,0,
                         0,0,3,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_2d_int_0_1() {
    struct array* arr = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){0,-1});

    int* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    int arr_test[] = {1,0,0,
                         0,2,0,
                         0,0,3,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_2d_float_0_1() {
    struct array* arr = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){0,-1});

    float* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    float arr_test[] = {1,0,0,
                         0,2,0,
                         0,0,3,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_2d_complex_0_1() {
    struct array* arr = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){0,-1});

    double complex* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    double complex arr_test[] = {1 + 0*I,0 + 0*I,0 + 0*I,
                         0 + 0*I,2 + 0*I,0 + 0*I,
                         0 + 0*I,0 + 0*I,3 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_3d_double_1_0_0() {
    struct array* arr = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){-1,0,0});

    double* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    double arr_test[] = {1,0,0,
                         0,1,0,
                         0,0,1,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_3d_int_1_0_0() {
    struct array* arr = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3][3]){
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){-1,0,0});

    int* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    int arr_test[] = {1,0,0,
                         0,1,0,
                         0,0,1,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_3d_float_1_0_0() {
    struct array* arr = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3][3]){
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){-1,0,0});

    float* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    float arr_test[] = {1,0,0,
                         0,1,0,
                         0,0,1,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_3d_complex_1_0_0() {
    struct array* arr = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3][3]){
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},},
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},},
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){-1,0,0});

    double complex* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    double complex arr_test[] = {1 + 0*I,0 + 0*I,0 + 0*I,
                         0 + 0*I,1 + 0*I,0 + 0*I,
                         0 + 0*I,0 + 0*I,1 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_3d_double_0_1_0() {
    struct array* arr = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){0,-1,0});

    double* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    double arr_test[] = {1,0,0,
                         0,1,0,
                         0,0,1,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_3d_int_0_1_0() {
    struct array* arr = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3][3]){
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){0,-1,0});

    int* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    int arr_test[] = {1,0,0,
                         0,1,0,
                         0,0,1,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_3d_float_0_1_0() {
    struct array* arr = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3][3]){
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){0,-1,0});

    float* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    float arr_test[] = {1,0,0,
                         0,1,0,
                         0,0,1,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_3d_complex_0_1_0() {
    struct array* arr = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3][3]){
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},},
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},},
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){0,-1,0});

    double complex* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    double complex arr_test[] = {1 + 0*I,0 + 0*I,0 + 0*I,
                         0 + 0*I,1 + 0*I,0 + 0*I,
                         0 + 0*I,0 + 0*I,1 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_3d_double_0_0_1() {
    struct array* arr = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3][3]){
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){0,0,-1});

    double* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    double arr_test[] = {1,0,0,
                         0,2,0,
                         0,0,3,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_3d_int_0_0_1() {
    struct array* arr = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3][3]){
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){0,0,-1});

    int* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    int arr_test[] = {1,0,0,
                         0,2,0,
                         0,0,3,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_3d_float_0_0_1() {
    struct array* arr = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3][3]){
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                        {{1,2,3},
                                        {1,2,3},
                                        {1,2,3},},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){0,0,-1});

    float* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    float arr_test[] = {1,0,0,
                         0,2,0,
                         0,0,3,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void diag_a_3d_complex_0_0_1() {
    struct array* arr = new_set_a(&(size_t){3},
                                    (size_t[]){3,3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3][3]){
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},},
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},},
                                        {{1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    });

    struct array* diagonal = diag_a(arr, (int[]){0,0,-1});

    double complex* arr_diag = get_a(diagonal);
    size_t size = get_size_a(diagonal);
    double complex arr_test[] = {1 + 0*I,0 + 0*I,0 + 0*I,
                         0 + 0*I,2 + 0*I,0 + 0*I,
                         0 + 0*I,0 + 0*I,3 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 
    
    free_a(arr);
    free_a(diagonal);
}

void slice_a_2d_1_0_DOUBLE() {
    struct array* arr = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,0});

    double* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double arr_test[] = {1,1,1};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_2d_1_0_INT() {
    struct array* arr = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,0});

    int* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    int arr_test[] = {1,1,1};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_2d_1_0_FLOAT() {
    struct array* arr = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,0});

    float* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    float arr_test[] = {1,1,1};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_2d_1_0_COMPLEX() {
    struct array* arr = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,0});

    double complex* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double complex arr_test[] = {1 + 0*I,1 + 0*I,1 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_2d_0_1_DOUBLE() {
    struct array* arr = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,-1});

    double* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double arr_test[] = {1,2,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_2d_0_1_INT() {
    struct array* arr = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,-1});

    int* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    int arr_test[] = {1,2,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_2d_0_1_FLOAT() {
    struct array* arr = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,-1});

    float* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    float arr_test[] = {1,2,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_2d_0_1_COMPLEX() {
    struct array* arr = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,-1});

    double complex* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_1_0_0_DOUBLE() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,0,0});

    double* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double arr_test[] = {1,1,1};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_1_0_0_INT() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,0,0});

    int* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    int arr_test[] = {1,1,1};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}


void slice_a_3d_1_0_0_FLOAT() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,0,0});

    float* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    float arr_test[] = {1,1,1};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}


void slice_a_3d_1_0_0_COMPLEX() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3][3]){
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,0,0});

    double complex* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double complex arr_test[] = {1 + 0*I,1 + 0*I,1 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_0_1_0_DOUBLE() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,-1,0});

    double* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double arr_test[] = {1,1,1};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_0_1_0_INT() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,-1,0});

    int* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    int arr_test[] = {1,1,1};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}


void slice_a_3d_0_1_0_FLOAT() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,-1,0});

    float* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    float arr_test[] = {1,1,1};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}


void slice_a_3d_0_1_0_COMPLEX() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3][3]){
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,-1,0});

    double complex* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double complex arr_test[] = {1 + 0*I,1 + 0*I,1 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_0_0_1_DOUBLE() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,0,-1});

    double* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double arr_test[] = {1,2,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_0_0_1_INT() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,0,-1});

    int* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    int arr_test[] = {1,2,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}


void slice_a_3d_0_0_1_FLOAT() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,0,-1});

    float* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    float arr_test[] = {1,2,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}


void slice_a_3d_0_0_1_COMPLEX() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3][3]){
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,0,-1});

    double complex* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_1_1_0_DOUBLE() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,-1,0});

    double* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double arr_test[] = {1,1,1,
                         1,1,1,
                         1,1,1};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_1_1_0_INT() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,-1,0});

    int* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    int arr_test[] = {1,1,1,
                         1,1,1,
                         1,1,1};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_1_1_0_FLOAT() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,-1,0});

    float* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    float arr_test[] = {1,1,1,
                         1,1,1,
                         1,1,1};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_1_1_0_COMPLEX() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3][3]){
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,-1,0});

    double complex* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double complex arr_test[] = {1 + 0*I,1 + 0*I,1 + 0*I,
                         1 + 0*I,1 + 0*I,1 + 0*I,
                         1 + 0*I,1 + 0*I,1 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_1_0_1_DOUBLE() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,0,-1});

    double* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_1_0_1_INT() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,0,-1});

    int* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    int arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_1_0_1_FLOAT() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,0,-1});

    float* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    float arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_1_0_1_COMPLEX() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3][3]){
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){-1,0,-1});

    double complex* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I,
                         1 + 0*I,2 + 0*I,3 + 0*I,
                         1 + 0*I,2 + 0*I,3 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_0_1_1_DOUBLE() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,-1,-1});

    double* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_0_1_1_INT() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,-1,-1});

    int* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    int arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_0_1_1_FLOAT() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3][3]){
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                    {{1,2,3},
                                     {1,2,3},
                                     {1,2,3},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,-1,-1});

    float* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    float arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void slice_a_3d_0_1_1_COMPLEX() {
    struct array* arr = new_set_a(&(size_t){3},
                                (size_t[]){3,3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3][3]){
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                    {{1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},
                                     {1 + 0*I,2 + 0*I,3 + 0*I},},
                                });
    
    struct array* slice = slice_a(arr, (int[]){0,-1,-1});

    double complex* arr_diag = get_a(slice);
    size_t size = get_size_a(slice);
    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I,
                         1 + 0*I,2 + 0*I,3 + 0*I,
                         1 + 0*I,2 + 0*I,3 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_diag[i]);
    } 

    free_a(arr);
    free_a(slice);
}

void test_conjugate_a() {
    struct array* arr = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 1*I,2 - 0*I,3 - 2*I},
                                     {1 + 1*I,2 + 0*I,3 + 2*I},
                                     {1 + 1*I,2 + 0*I,3 - 2*I},
                                });

    struct array* conjugate = conjugate_a(arr);

    double complex* arr_conj = get_a(conjugate);
    size_t size = get_size_a(conjugate);
    double complex arr_test[] = {1 - 1*I,2 + 0*I,3 + 2*I,
                                 1 - 1*I,2 - 0*I,3 - 2*I,
                                 1 - 1*I,2 - 0*I,3 + 2*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr_conj[i]);
    } 

    free_a(arr);
    free_a(conjugate);
}

void test_fill_diagonal_a_2d_double() {
    struct array* array = zeros_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){DOUBLE});
    fill_diagonal_a(array, &(double){2});

    double* arr = get_a(array);
    double arr_test[] = {2,0,0,
                        0,2,0,
                        0,0,2};
    size_t size = get_size_a(array);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
}

void test_fill_diagonal_a_2d_int() {
    struct array* array = zeros_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){INT});
    fill_diagonal_a(array, &(int){2});

    int* arr = get_a(array);
    int arr_test[] = {2,0,0,
                        0,2,0,
                        0,0,2};
    size_t size = get_size_a(array);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
}

void test_fill_diagonal_a_2d_float() {
    struct array* array = zeros_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){FLOAT});
    fill_diagonal_a(array, &(float){2});

    float* arr = get_a(array);
    float arr_test[] = {2,0,0,
                        0,2,0,
                        0,0,2};
    size_t size = get_size_a(array);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
}

void test_fill_diagonal_a_2d_complex() {
    struct array* array = zeros_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){COMPLEX});
    fill_diagonal_a(array, &(double complex){2 + 0*I});

    double complex* arr = get_a(array);
    double complex arr_test[] = {2 + 0*I,0 + 0*I,0 + 0*I,
                        0 + 0*I,2 + 0*I,0 + 0*I,
                        0 + 0*I,0 + 0*I,2 + 0*I};
    size_t size = get_size_a(array);
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
}

void test_mean_a_double() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    double* mean = (double*)mean_a(array);

    TEST_ASSERT_EQUAL(2,*mean);
    free_a(array);
    free(mean);
}

void test_mean_a_int() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    double* mean = (double*)mean_a(array);

    TEST_ASSERT_EQUAL(2,*mean);
    free_a(array);
    free(mean);
}

void test_mean_a_float() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    float* mean = (float*)mean_a(array);

    TEST_ASSERT_EQUAL(2,*mean);
    free_a(array);
    free(mean);
}

void test_mean_a_complex() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });

    double* mean = (double*)mean_a(array);

    TEST_ASSERT_EQUAL(2,*mean);
    free_a(array);
    free(mean);
}

void test_outer_a_double() {
    struct array* array_1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3]){
                                    1,2,3
                                });

    struct array* array_2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3]){
                                    1,2,3
                                });

    struct array* array_outer = outer_a(array_1, array_2);

    double* arr = get_a(array_outer);
    size_t size = get_size_a(array_outer);
    double arr_test[] = {1,2,3,
                        2,4,6,
                        3,6,9};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array_1);
    free_a(array_2);
    free_a(array_outer);

}

void test_outer_a_int() {
    struct array* array_1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){INT}, 
                                (int[3]){
                                    1,2,3
                                });

    struct array* array_2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){INT}, 
                                (int[3]){
                                    1,2,3
                                });

    struct array* array_outer = outer_a(array_1, array_2);

    int* arr = get_a(array_outer);
    size_t size = get_size_a(array_outer);
    int arr_test[] = {1,2,3,
                        2,4,6,
                        3,6,9};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array_1);
    free_a(array_2);
    free_a(array_outer);

}

void test_outer_a_float() {
    struct array* array_1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3]){
                                    1,2,3
                                });

    struct array* array_2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3]){
                                    1,2,3
                                });

    struct array* array_outer = outer_a(array_1, array_2);

    float* arr = get_a(array_outer);
    size_t size = get_size_a(array_outer);
    float arr_test[] = {1,2,3,
                        2,4,6,
                        3,6,9};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array_1);
    free_a(array_2);
    free_a(array_outer);

}

void test_outer_a_complex() {
    struct array* array_1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3]){
                                    1 + 0*I,2 + 0*I,3 + 0*I
                                });

    struct array* array_2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3]){
                                    1 + 0*I,2 + 0*I,3 + 0*I
                                });

    struct array* array_outer = outer_a(array_1, array_2);

    double complex* arr = get_a(array_outer);
    size_t size = get_size_a(array_outer);
    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I,
                        2 + 0*I,4 + 0*I,6 + 0*I,
                        3 + 0*I,6 + 0*I,9 + 0*I};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array_1);
    free_a(array_2);
    free_a(array_outer);

}

void test_harmard_element_wise_inverse_a() {
    struct array* array = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3]){
                                    1 + 0*I,2 + 0*I,3 + 0*I
                                });

    struct array* h_inverse = hadamard_inverse_a(array);

    double complex* arr = get_a(h_inverse);
    size_t size = get_size_a(h_inverse);
    double complex arr_test[] = {1.0/(1 + 0*I),1.0/(2 + 0*I),1.0/(3 + 0*I),};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(h_inverse);
}

void test_diag_harmard_element_wise_inverse_a() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });
    
    struct array* diag = diag_hadamard_inverse_a(array);

    double complex* arr = get_a(diag);
    size_t size = get_size_a(diag);
    double complex arr_test[] = {1.0/(1 + 0*I),0 + 0*I,0 + 0*I,
                        0 + 0*I,1.0/(2 + 0*I),0 + 0*I,
                        0 + 0*I,0 + 0*I,1.0/(9 + 0*I)};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(diag);
}

void test_transpose_a_double_1() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });
    
    struct array* transpose = transpose_a(array);

    double* arr = get_a(transpose);
    size_t size = get_size_a(transpose);
    double arr_test[] = {1,1,1,
                         2,2,2,
                         3,3,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(transpose);
}

void test_transpose_a_double_2() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,4}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][4]){
                                    {1,2,3,4},
                                    {1,2,3,4},
                                    {1,2,3,4},
                                });
    
    struct array* transpose = transpose_a(array);

    double* arr = get_a(transpose);
    size_t size = get_size_a(transpose);
    double arr_test[] = {1,1,1,
                         2,2,2,
                         3,3,3,
                         4,4,4};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(transpose);
}

void test_transpose_a_double_3() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){4,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[4][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });
    
    struct array* transpose = transpose_a(array);

    double* arr = get_a(transpose);
    size_t size = get_size_a(transpose);
    double arr_test[] = {1,1,1,1,
                         2,2,2,2,
                         3,3,3,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(transpose);
}

void test_transpose_a_int_1() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });
    
    struct array* transpose = transpose_a(array);

    int* arr = get_a(transpose);
    size_t size = get_size_a(transpose);
    int arr_test[] = {1,1,1,
                         2,2,2,
                         3,3,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(transpose);
}

void test_transpose_a_int_2() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,4}, 
                                &(enum DataType){INT}, 
                                (int[3][4]){
                                    {1,2,3,4},
                                    {1,2,3,4},
                                    {1,2,3,4},
                                });
    
    struct array* transpose = transpose_a(array);

    int* arr = get_a(transpose);
    size_t size = get_size_a(transpose);
    int arr_test[] = {1,1,1,
                         2,2,2,
                         3,3,3,
                         4,4,4};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(transpose);
}

void test_transpose_a_int_3() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){4,3}, 
                                &(enum DataType){INT}, 
                                (int[4][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });
    
    struct array* transpose = transpose_a(array);

    int* arr = get_a(transpose);
    size_t size = get_size_a(transpose);
    int arr_test[] = {1,1,1,1,
                         2,2,2,2,
                         3,3,3,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(transpose);
}

void test_transpose_a_float_1() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });
    
    struct array* transpose = transpose_a(array);

    float* arr = get_a(transpose);
    size_t size = get_size_a(transpose);
    float arr_test[] = {1,1,1,
                         2,2,2,
                         3,3,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(transpose);
}

void test_transpose_a_float_2() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,4}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][4]){
                                    {1,2,3,4},
                                    {1,2,3,4},
                                    {1,2,3,4},
                                });
    
    struct array* transpose = transpose_a(array);

    float* arr = get_a(transpose);
    size_t size = get_size_a(transpose);
    float arr_test[] = {1,1,1,
                         2,2,2,
                         3,3,3,
                         4,4,4};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(transpose);
}

void test_transpose_a_float_3() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){4,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[4][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });
    
    struct array* transpose = transpose_a(array);

    float* arr = get_a(transpose);
    size_t size = get_size_a(transpose);
    float arr_test[] = {1,1,1,1,
                         2,2,2,2,
                         3,3,3,3};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(transpose);
}

void test_transpose_a_complex_1() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });
    
    struct array* transpose = transpose_a(array);

    double complex* arr = get_a(transpose);
    size_t size = get_size_a(transpose);
    double complex arr_test[] = {1 + 0*I,1 + 0*I,1 + 0*I,
                         2 + 0*I,2 + 0*I,2 + 0*I,
                         3 + 0*I,3 + 0*I,3 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(transpose);
}

void test_transpose_a_complex_2() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,4}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][4]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I,4 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I,4 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I,4 + 0*I},
                                });
    
    struct array* transpose = transpose_a(array);

    double complex* arr = get_a(transpose);
    size_t size = get_size_a(transpose);
    double complex arr_test[] = {1 + 0*I,1 + 0*I,1 + 0*I,
                         2 + 0*I,2 + 0*I,2 + 0*I,
                         3 + 0*I,3 + 0*I,3 + 0*I,
                         4 + 0*I,4 + 0*I,4 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(transpose);
}

void test_transpose_a_complex_3() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){4,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[4][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });
    
    struct array* transpose = transpose_a(array);

    double complex* arr = get_a(transpose);
    size_t size = get_size_a(transpose);
    double complex arr_test[] = {1 + 0*I,1 + 0*I,1 + 0*I,1 + 0*I,
                         2 + 0*I,2 + 0*I,2 + 0*I,2 + 0*I,
                         3 + 0*I,3 + 0*I,3 + 0*I,3 + 0*I};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(array);
    free_a(transpose);
}

void test_save_to_file_a_double() {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });
    save_to_file_a(array,"./unit_test/bin", "test_double");
    free_a(array);
}

void test_save_to_file_a_int() {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){INT}, 
                                    (int[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });
    save_to_file_a(array,"./unit_test/bin", "test_int");
    free_a(array);
}

void test_save_to_file_a_float() {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){FLOAT}, 
                                    (float[3][3]){
                                        {1,2,3},
                                        {1,2,3},
                                        {1,2,3},
                                    });
    save_to_file_a(array,"./unit_test/bin", "test_float");
    free_a(array);
}

void test_save_to_file_a_complex() {
    struct array* array = new_set_a(&(size_t){2},
                                    (size_t[]){3,3}, 
                                    &(enum DataType){COMPLEX}, 
                                    (double complex[3][3]){
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                        {1 + 0*I,2 + 0*I,3 + 0*I},
                                    });
    save_to_file_a(array,"./unit_test/bin", "test_complex");
    free_a(array);
}

void test_read_from_file_a_double() {
    struct array* result = read_from_file_a("./unit_test/bin/test_double.array");

    double* arr = get_a(result);
    size_t size = get_size_a(result);
    double arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(result);
}

void test_read_from_file_a_int() {
    struct array* result = read_from_file_a("./unit_test/bin/test_int.array");

    int* arr = get_a(result);
    size_t size = get_size_a(result);
    int arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(result);
}

void test_read_from_file_a_float() {
    struct array* result = read_from_file_a("./unit_test/bin/test_float.array");

    float* arr = get_a(result);
    size_t size = get_size_a(result);
    float arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(result);
}

void test_read_from_file_a_complex() {
    struct array* result = read_from_file_a("./unit_test/bin/test_complex.array");

    double complex* arr = get_a(result);
    size_t size = get_size_a(result);
    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I,
                         1 + 0*I,2 + 0*I,3 + 0*I,
                         1 + 0*I,2 + 0*I,3 + 0*I,};

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(result);
}

void test_std_a_double() {
    struct array* array = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3]){
                                    1,2,3
                                });
    
    double* std = std_a(array);

    TEST_ASSERT_EQUAL(*std, 0.816497);

    free(std);
    free_a(array);
}

void test_std_a_int() {
    struct array* array = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){INT}, 
                                (int[3]){
                                    1,2,3
                                });
    
    double* std = std_a(array);

    TEST_ASSERT_EQUAL(*std, 0.816497);

    free(std);
    free_a(array);
}

void test_std_a_float() {
    struct array* array = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3]){
                                    1,2,3
                                });
    
    float* std = std_a(array);

    TEST_ASSERT_EQUAL(*std, 0.816497);

    free(std);
    free_a(array);
}

void test_std_a_complex() {
    struct array* array = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3]){
                                    1 + 0*I,2 + 0*I,3 + 0*I
                                });
    
    double* std = std_a(array);

    TEST_ASSERT_EQUAL(*std, 0.816497);

    free(std);
    free_a(array);
}

void test_equal_a_double() {
    struct array* array1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3]){
                                    1,2,3
                                });

    struct array* array2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3]){
                                    1,2,3
                                });

    bool check = equal_a(array1, array2);

    TEST_ASSERT_EQUAL(true, check);

    free_a(array1);
    free_a(array2);
}

void test_equal_a_int() {
    struct array* array1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){INT}, 
                                (int[3]){
                                    1,2,3
                                });

    struct array* array2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){INT}, 
                                (int[3]){
                                    1,2,3
                                });

    bool check = equal_a(array1, array2);

    TEST_ASSERT_EQUAL(true, check);

    free_a(array1);
    free_a(array2);
}

void test_equal_a_float() {
    struct array* array1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3]){
                                    1,2,3
                                });

    struct array* array2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3]){
                                    1,2,3
                                });

    bool check = equal_a(array1, array2);

    TEST_ASSERT_EQUAL(true, check);

    free_a(array1);
    free_a(array2);
}

void test_equal_a_complex() {
    struct array* array1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3]){
                                    1 + 0*I,2 + 0*I,3 + 0*I
                                });

    struct array* array2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3]){
                                    1 + 0*I,2 + 0*I,3 + 0*I
                                });

    bool check = equal_a(array1, array2);

    TEST_ASSERT_EQUAL(true, check);

    free_a(array1);
    free_a(array2);
}

void test_reciprocal_a_double() {
    struct array* array = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3]){
                                    1,2,3
                                });

    struct array* recip = reciprocal_a(array);
    double* arr = recip->array;

    double arr_test[] = {1/1,1/2,1/3};

    for (size_t i = 0; i < get_size_a(array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(recip);
    free_a(array);
}

void test_reciprocal_a_int() {
    struct array* array = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){INT}, 
                                (int[3]){
                                    1,2,3
                                });

    struct array* recip = reciprocal_a(array);
    int* arr = recip->array;

    int arr_test[] = {1/1,1/2,1/3};

    for (size_t i = 0; i < get_size_a(array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(recip);
    free_a(array);
}

void test_reciprocal_a_float() {
    struct array* array = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3]){
                                    1,2,3
                                });

    struct array* recip = reciprocal_a(array);
    float* arr = recip->array;

    float arr_test[] = {1/1,1/2,1/3};

    for (size_t i = 0; i < get_size_a(array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(recip);
    free_a(array);
}

void test_diag_reciprocal_a_double() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    struct array* recip = diag_reciprocal_a(array);
    double* arr = recip->array;

    double arr_test[] = {1/1,0,0,
                         0,1/2,0,
                         0,0,1/3};

    for (size_t i = 0; i < get_size_a(array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(recip);
    free_a(array);
}

void test_diag_reciprocal_a_int() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    struct array* recip = diag_reciprocal_a(array);
    int* arr = recip->array;

    int arr_test[] = {1/1,0,0,
                         0,1/2,0,
                         0,0,1/3};

    for (size_t i = 0; i < get_size_a(array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(recip);
    free_a(array);
}


void test_diag_reciprocal_a_float() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    struct array* recip = diag_reciprocal_a(array);
    float* arr = recip->array;

    float arr_test[] = {1/1,0,0,
                         0,1/2,0,
                         0,0,1/3};

    for (size_t i = 0; i < get_size_a(array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(recip);
    free_a(array);
}

void test_real_a() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 4*I,2 + 4*I,3 + 4*I},
                                    {1 + 4*I,2 + 4*I,3 + 4*I},
                                    {1 + 4*I,2 + 4*I,3 + 4*I},
                                });

    struct array* real = real_a(array);
    double* arr = real->array;

    double arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3};

    for (size_t i = 0; i < get_size_a(array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(real);
    free_a(array);
}

void test_imag_a() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 4*I,2 + 4*I,3 + 4*I},
                                    {1 + 4*I,2 + 4*I,3 + 4*I},
                                    {1 + 4*I,2 + 4*I,3 + 4*I},
                                });

    struct array* imag = imag_a(array);
    double* arr = imag->array;

    double arr_test[] = {4,4,4,
                         4,4,4,
                         4,4,4};

    for (size_t i = 0; i < get_size_a(array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(imag);
    free_a(array);
}

void test_contains_double_true() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    TEST_ASSERT_EQUAL(true, contains_element_a(array, &(double){1}));

    free_a(array);
}

void test_contains_double_false() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    TEST_ASSERT_EQUAL(false, contains_element_a(array, &(double){4}));

    free_a(array);
}

void test_contains_int_true() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    TEST_ASSERT_EQUAL(true, contains_element_a(array, &(int){1}));

    free_a(array);
}

void test_contains_int_false() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    TEST_ASSERT_EQUAL(false, contains_element_a(array, &(int){4}));

    free_a(array);
}

void test_contains_float_true() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    TEST_ASSERT_EQUAL(true, contains_element_a(array, &(float){1}));

    free_a(array);
}

void test_contains_float_false() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    TEST_ASSERT_EQUAL(false, contains_element_a(array, &(float){4}));

    free_a(array);
}

void test_contains_complex_true() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });

    TEST_ASSERT_EQUAL(true, contains_element_a(array, &(double complex){1 + 0*I}));

    free_a(array);
}

void test_contains_complex_false() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });

    TEST_ASSERT_EQUAL(false, contains_element_a(array, &(double complex){4 + 0*I}));

    free_a(array);
}

void test_index_of_element_double_true() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    TEST_ASSERT_EQUAL(0, index_of_element_a(array, &(double){1}));

    free_a(array);
}

void test_index_of_element_double_false() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    TEST_ASSERT_EQUAL(-1, index_of_element_a(array, &(double){4}));

    free_a(array);
}

void test_index_of_element_int_true() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    TEST_ASSERT_EQUAL(0, index_of_element_a(array, &(int){1}));

    free_a(array);
}

void test_index_of_element_int_false() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    TEST_ASSERT_EQUAL(-1, index_of_element_a(array, &(int){4}));

    free_a(array);
}

void test_index_of_element_float_true() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    TEST_ASSERT_EQUAL(0, index_of_element_a(array, &(float){1}));

    free_a(array);
}

void test_index_of_element_float_false() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    TEST_ASSERT_EQUAL(-1, index_of_element_a(array, &(float){4}));

    free_a(array);
}

void test_index_of_element_complex_true() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });

    TEST_ASSERT_EQUAL(0, index_of_element_a(array, &(double complex){1 + 0*I}));

    free_a(array);
}

void test_index_of_element_complex_false() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });

    TEST_ASSERT_EQUAL(-1, index_of_element_a(array, &(double complex){4 + 0*I}));

    free_a(array);
}

void test_norm_a_double() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    double* norm = (double*)norm_a(array);

    TEST_ASSERT_EQUAL(6.48,*norm);
    free_a(array);
    free(norm);
}

void test_norm_a_int() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    double* norm = (double*)norm_a(array);

    TEST_ASSERT_EQUAL(6.48,*norm);
    free_a(array);
    free(norm);
}

void test_norm_a_float() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    float* norm = (float*)norm_a(array);

    TEST_ASSERT_EQUAL(6.48,*norm);
    free_a(array);
    free(norm);
}

void test_norm_a_complex() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });

    double* norm = (double*)norm_a(array);

    TEST_ASSERT_EQUAL(6.48,*norm);
    free_a(array);
    free(norm);
}

void test_svd_a_complex() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });

    struct array** svd_values = svd_a(array);

    printf("U\n");
    print_a(svd_values[0]);
    printf("\n");

    printf("S\n");
    print_a(svd_values[1]);
    printf("\n");

    printf("VT\n");
    print_a(svd_values[2]);
    printf("\n");

    for (size_t i = 0; i < 3; i++) {
        free_a(svd_values[i]);
    }
    free(svd_values);
    free_a(array);
}

void test_pinv_a_complex() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });

    struct array* pinv = p_inv_a(array);

    print_a(pinv);

    free_a(pinv);
    free_a(array);
}

void test_abs_a_double() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3]){
                                    {-1,2,3},
                                    {1,-2,3},
                                    {1,2,-3},
                                });

    struct array* abs_array = abs_a(array);
    double* arr = abs_array->array;

    double arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3};

    for (size_t i = 0; i < get_size_a(abs_array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(abs_array);
    free_a(array);
}

void test_abs_a_int() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3]){
                                    {-1,2,3},
                                    {1,-2,3},
                                    {1,2,-3},
                                });

    struct array* abs_array = abs_a(array);
    int* arr = abs_array->array;

    int arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3};

    for (size_t i = 0; i < get_size_a(abs_array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(abs_array);
    free_a(array);
}

void test_abs_a_float() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3]){
                                    {-1,2,3},
                                    {1,-2,3},
                                    {1,2,-3},
                                });

    struct array* abs_array = abs_a(array);
    float* arr = abs_array->array;

    float arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3};

    for (size_t i = 0; i < get_size_a(abs_array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(abs_array);
    free_a(array);
}

void test_abs_a_complex() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 4*I,2 + 4*I,3 + 4*I},
                                    {1 + 4*I,2 + 4*I,3 + 4*I},
                                    {1 + 4*I,2 + 4*I,3 + 4*I},
                                });

    struct array* abs_array = abs_a(array);
    double* arr = abs_array->array;

    double arr_test[] = {sqrt(17),sqrt(20),sqrt(25),
                         sqrt(17),sqrt(20),sqrt(25),
                         sqrt(17),sqrt(20),sqrt(25)};

    for (size_t i = 0; i < get_size_a(abs_array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(abs_array);
    free_a(array);
}

void test_power_a_double() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    struct array* power_array = power_a(array, 2);
    double* arr = power_array->array;

    double arr_test[] = {1,4,9,
                         1,4,9,
                         1,4,9};

    for (size_t i = 0; i < get_size_a(power_array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(power_array);
    free_a(array);
}

void test_power_a_int() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    struct array* power_array = power_a(array, 2);
    int* arr = power_array->array;

    int arr_test[] = {1,4,9,
                         1,4,9,
                         1,4,9};

    for (size_t i = 0; i < get_size_a(power_array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(power_array);
    free_a(array);
}

void test_power_a_float() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3]){
                                    {1,2,3},
                                    {1,2,3},
                                    {1,2,3},
                                });

    struct array* power_array = power_a(array, 2);
    float* arr = power_array->array;

    float arr_test[] = {1,4,9,
                         1,4,9,
                         1,4,9};

    for (size_t i = 0; i < get_size_a(power_array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(power_array);
    free_a(array);
}

void test_power_a_complex() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                    {1 + 0*I,2 + 0*I,3 + 0*I},
                                });

    struct array* power_array = power_a(array, 2);
    double complex* arr = power_array->array;

    double complex arr_test[] = {1 + 0*I,4 + 0*I,9 + 0*I,
                                  1 + 0*I,4 + 0*I,9 + 0*I,
                                  1 + 0*I,4 + 0*I,9 + 0*I};

    for (size_t i = 0; i < get_size_a(power_array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(power_array);
    free_a(array);
}

void test_sqrt_a_double() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3][3]){
                                    {1,4,9},
                                    {1,4,9},
                                    {1,4,9},
                                });

    struct array* sqrt_array = sqrt_a(array);
    double* arr = sqrt_array->array;

    double arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3};

    for (size_t i = 0; i < get_size_a(sqrt_array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(sqrt_array);
    free_a(array);
}

void test_sqrt_a_int() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){INT}, 
                                (int[3][3]){
                                    {1,4,9},
                                    {1,4,9},
                                    {1,4,9},
                                });

    struct array* sqrt_array = sqrt_a(array);
    int* arr = sqrt_array->array;

    int arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3};

    for (size_t i = 0; i < get_size_a(sqrt_array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(sqrt_array);
    free_a(array);
}

void test_sqrt_a_float() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3][3]){
                                    {1,4,9},
                                    {1,4,9},
                                    {1,4,9},
                                });

    struct array* sqrt_array = sqrt_a(array);
    float* arr = sqrt_array->array;

    float arr_test[] = {1,2,3,
                         1,2,3,
                         1,2,3};

    for (size_t i = 0; i < get_size_a(sqrt_array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(sqrt_array);
    free_a(array);
}

void test_sqrt_a_complex() {
    struct array* array = new_set_a(&(size_t){2},
                                (size_t[]){3,3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3][3]){
                                    {1 + 0*I,4 + 0*I,9 + 0*I},
                                    {1 + 0*I,4 + 0*I,9 + 0*I},
                                    {1 + 0*I,4 + 0*I,9 + 0*I},
                                });

    struct array* sqrt_array = sqrt_a(array);
    double complex* arr = sqrt_array->array;

    double complex arr_test[] = {1 + 0*I,2 + 0*I,3 + 0*I,
                                  1 + 0*I,2 + 0*I,3 + 0*I,
                                  1 + 0*I,2 + 0*I,3 + 0*I};

    for (size_t i = 0; i < get_size_a(sqrt_array); i++) {
        TEST_ASSERT_EQUAL(arr_test[i],arr[i]);
    } 

    free_a(sqrt_array);
    free_a(array);
}


//Main function to run the tests
// int main(void) {
//     UNITY_BEGIN();
    // RUN_TEST(test_new_a_1d_DOUBLE);
    // RUN_TEST(test_new_a_1d_INT);
    // RUN_TEST(test_new_a_1d_FLOAT);
    // RUN_TEST(test_new_a_1d_COMPLEX);
    // RUN_TEST(test_new_a_2d_DOUBLE);
    // RUN_TEST(test_new_a_2d_INT);
    // RUN_TEST(test_new_a_2d_FLOAT);
    // RUN_TEST(test_new_a_2d_COMPLEX);
    // RUN_TEST(test_new_a_3d_DOUBLE);
    // RUN_TEST(test_new_a_3d_INT);
    // RUN_TEST(test_new_a_3d_FLOAT);
    // RUN_TEST(test_new_a_3d_COMPLEX);
    // RUN_TEST(test_get_size_1d);
    // RUN_TEST(test_get_size_2d);
    // RUN_TEST(test_get_size_3d);
    // RUN_TEST(test_get_a);
    // RUN_TEST(test_get_element_a);
    // RUN_TEST(test_zeros_a_1d_DOUBLE);
    // RUN_TEST(test_zeros_a_2d_DOUBLE);
    // RUN_TEST(test_zeros_a_3d_DOUBLE);
    // RUN_TEST(test_zeros_a_1d_INT);
    // RUN_TEST(test_zeros_a_2d_INT);
    // RUN_TEST(test_zeros_a_3d_INT);
    // RUN_TEST(test_zeros_a_1d_FLOAT);
    // RUN_TEST(test_zeros_a_2d_FLOAT);
    // RUN_TEST(test_zeros_a_3d_FLOAT);
    // RUN_TEST(test_zeros_a_1d_COMPLEX);
    // RUN_TEST(test_zeros_a_2d_COMPLEX);
    // RUN_TEST(test_zeros_a_3d_COMPLEX);
    // RUN_TEST(test_ones_a_1d_DOUBLE);
    // RUN_TEST(test_ones_a_2d_DOUBLE);
    // RUN_TEST(test_ones_a_3d_DOUBLE);
    // RUN_TEST(test_ones_a_1d_INT);
    // RUN_TEST(test_ones_a_2d_INT);
    // RUN_TEST(test_ones_a_3d_INT);
    // RUN_TEST(test_ones_a_1d_FLOAT);
    // RUN_TEST(test_ones_a_2d_FLOAT);
    // RUN_TEST(test_ones_a_3d_FLOAT);
    // RUN_TEST(test_ones_a_1d_COMPLEX);
    // RUN_TEST(test_ones_a_2d_COMPLEX);
    // RUN_TEST(test_ones_a_3d_COMPLEX);
    // RUN_TEST(test_set_element_a_DOUBLE);
    // RUN_TEST(test_set_element_a_INT);
    // RUN_TEST(test_set_element_a_FLOAT);
    // RUN_TEST(test_set_element_a_COMPLEX);
    // RUN_TEST(test_set_a_1d_DOUBLE);
    // RUN_TEST(test_set_a_1d_INT);
    // RUN_TEST(test_set_a_1d_FLOAT);
    // RUN_TEST(test_set_a_1d_COMPLEX);
    // RUN_TEST(test_set_a_2d_DOUBLE);
    // RUN_TEST(test_set_a_2d_INT);
    // RUN_TEST(test_set_a_2d_FLOAT);
    // RUN_TEST(test_set_a_2d_COMPLEX);
    // RUN_TEST(test_set_a_3d_DOUBLE);
    // RUN_TEST(test_set_a_3d_INT);
    // RUN_TEST(test_set_a_3d_FLOAT);
    // RUN_TEST(test_set_a_3d_COMPLEX);
    // RUN_TEST(test_multiply_a_DOUBLE_DOUBLE);
    // RUN_TEST(test_multiply_a_INT_INT);
    // RUN_TEST(test_multiply_a_FLOAT_FLOAT);
    // RUN_TEST(test_multiply_a_COMPLEX_COMPLEX);
    // RUN_TEST(test_multiply_a_COMPLEX_DOUBLE);
    // RUN_TEST(test_multiply_a_COMPLEX_INT);
    // RUN_TEST(test_multiply_a_COMPLEX_FLOAT);
    // RUN_TEST(test_divide_a_DOUBLE_DOUBLE);
    // RUN_TEST(test_divide_a_INT_INT);
    // RUN_TEST(test_divide_a_FLOAT_FLOAT);
    // RUN_TEST(test_divide_a_COMPLEX_COMPLEX);
    // RUN_TEST(test_divide_a_COMPLEX_DOUBLE);
    // RUN_TEST(test_divide_a_COMPLEX_INT);
    // RUN_TEST(test_divide_a_COMPLEX_FLOAT);
    // RUN_TEST(test_addition_a_DOUBLE_DOUBLE);
    // RUN_TEST(test_addition_a_INT_INT);
    // RUN_TEST(test_addition_a_FLOAT_FLOAT);
    // RUN_TEST(test_addition_a_COMPLEX_COMPLEX);
    // RUN_TEST(test_addition_a_COMPLEX_DOUBLE);
    // RUN_TEST(test_addition_a_COMPLEX_INT);
    // RUN_TEST(test_addition_a_COMPLEX_FLOAT);
    // RUN_TEST(test_minus_a_DOUBLE_DOUBLE);
    // RUN_TEST(test_minus_a_INT_INT);
    // RUN_TEST(test_minus_a_FLOAT_FLOAT);
    // RUN_TEST(test_minus_a_COMPLEX_COMPLEX);
    // RUN_TEST(test_minus_a_COMPLEX_DOUBLE);
    // RUN_TEST(test_minus_a_COMPLEX_INT);
    // RUN_TEST(test_minus_a_COMPLEX_FLOAT);
    // RUN_TEST(test_get_2d_index_a);
    // RUN_TEST(test_get_3d_index_a);
    // RUN_TEST(test_get_size_a_1d);
    // RUN_TEST(test_get_size_a_2d);
    // RUN_TEST(test_get_size_a_3d);
    // RUN_TEST(test_linspace_a_DOUBLE);
    // RUN_TEST(test_linspace_a_FLOAT);
    // RUN_TEST(test_multiply_constant_a_DOUBLE);
    // RUN_TEST(test_multiply_constant_a_INT);
    // RUN_TEST(test_multiply_constant_a_FLOAT);
    // RUN_TEST(test_multiply_constant_a_COMPLEX_DOUBLE);
    // RUN_TEST(test_multiply_constant_a_COMPLEX_INT);
    // RUN_TEST(test_multiply_constant_a_COMPLEX_FLOAT);
    // RUN_TEST(test_multiply_constant_a_COMPLEX_COMPLEX);
    // RUN_TEST(test_division_constant_a_DOUBLE);
    // RUN_TEST(test_division_constant_a_INT);
    // RUN_TEST(test_division_constant_a_FLOAT);
    // RUN_TEST(test_division_constant_a_COMPLEX_DOUBLE);
    // RUN_TEST(test_division_constant_a_COMPLEX_INT);
    // RUN_TEST(test_division_constant_a_COMPLEX_FLOAT);
    // RUN_TEST(test_division_constant_a_COMPLEX_COMPLEX);
    // RUN_TEST(test_minus_constant_a_DOUBLE);
    // RUN_TEST(test_minus_constant_a_INT);
    // RUN_TEST(test_minus_constant_a_FLOAT);
    // RUN_TEST(test_minus_constant_a_COMPLEX_DOUBLE);
    // RUN_TEST(test_minus_constant_a_COMPLEX_INT);
    // RUN_TEST(test_minus_constant_a_COMPLEX_FLOAT);
    // RUN_TEST(test_minus_constant_a_COMPLEX_COMPLEX);
    // RUN_TEST(test_addition_constant_a_DOUBLE);
    // RUN_TEST(test_addition_constant_a_INT);
    // RUN_TEST(test_addition_constant_a_FLOAT);
    // RUN_TEST(test_addition_constant_a_COMPLEX_DOUBLE);
    // RUN_TEST(test_addition_constant_a_COMPLEX_INT);
    // RUN_TEST(test_addition_constant_a_COMPLEX_FLOAT);
    // RUN_TEST(test_addition_constant_a_COMPLEX_COMPLEX);
    // RUN_TEST(test_max_a_DOUBLE);
    // RUN_TEST(test_max_a_INT);
    // RUN_TEST(test_max_a_FLOAT);
    // RUN_TEST(test_min_a_DOUBLE);
    // RUN_TEST(test_min_a_INT);
    // RUN_TEST(test_min_a_FLOAT);
    // RUN_TEST(test_amax_a_DOUBLE);
    // RUN_TEST(test_amax_a_INT);
    // RUN_TEST(test_amax_a_FLOAT);
    // RUN_TEST(test_copy_over_1d_DOUBLE);
    // RUN_TEST(test_copy_over_1d_INT);
    // RUN_TEST(test_copy_over_1d_FLOAT);
    // RUN_TEST(test_copy_over_1d_COMPLEX);
    // RUN_TEST(test_copy_over_2d_to_1d_1_0_DOUBLE);
    // RUN_TEST(test_copy_over_2d_to_1d_1_0_INT);
    // RUN_TEST(test_copy_over_2d_to_1d_1_0_FLOAT);
    // RUN_TEST(test_copy_over_2d_to_1d_1_0_COMPLEX);
    // RUN_TEST(test_copy_over_2d_to_1d_0_1_DOUBLE);
    // RUN_TEST(test_copy_over_2d_to_1d_0_1_INT);
    // RUN_TEST(test_copy_over_2d_to_1d_0_1_FLOAT);
    // RUN_TEST(test_copy_over_2d_to_1d_0_1_COMPLEX);
    // RUN_TEST(test_copy_over_2d_to_2d_1_1_to_1_1_DOUBLE);
    // RUN_TEST(test_copy_over_2d_to_2d_1_1_to_1_1_INT);
    // RUN_TEST(test_copy_over_2d_to_2d_1_1_to_1_1_FLOAT);
    // RUN_TEST(test_copy_over_2d_to_2d_1_1_to_1_1_COMPLEX);
    // RUN_TEST(test_copy_over_2d_to_2d_1_0_to_1_0);
    // RUN_TEST(test_copy_over_2d_to_2d_0_1_to_0_1);
    // RUN_TEST(test_copy_over_2d_to_2d_0_1_to_1_0);
    // RUN_TEST(test_copy_over_2d_to_2d_1_0_to_0_1);
    // RUN_TEST(test_copy_over_3d_to_1d_1_0_0_to_1);
    // RUN_TEST(test_copy_over_3d_to_1d_0_1_0_to_1);
    // RUN_TEST(test_copy_over_3d_to_1d_0_0_1_to_1);
    // RUN_TEST(test_copy_over_3d_to_2d_1_1_0_to_1_1);
    // RUN_TEST(test_copy_over_3d_to_2d_1_0_1_to_1_1);
    // RUN_TEST(test_copy_over_3d_to_2d_0_1_1_to_1_1);
    // RUN_TEST(test_copy_over_3d_to_2d_0_0_1_to_0_1);
    // RUN_TEST(test_copy_over_3d_to_2d_0_0_1_to_1_0);
    // RUN_TEST(test_copy_over_3d_to_2d_0_1_0_to_0_1);
    // RUN_TEST(test_copy_over_3d_to_2d_0_1_0_to_1_0);
    // RUN_TEST(test_copy_over_3d_to_2d_1_0_0_to_0_1);
    // RUN_TEST(test_copy_over_3d_to_2d_1_0_0_to_1_0);
    // RUN_TEST(test_copy_over_3d_to_3d_1_1_1_to_1_1_1_DOUBLE);
    // RUN_TEST(test_copy_over_3d_to_3d_1_1_1_to_1_1_1_INT);
    // RUN_TEST(test_copy_over_3d_to_3d_1_1_1_to_1_1_1_FLOAT);
    // RUN_TEST(test_copy_over_3d_to_3d_1_1_1_to_1_1_1_COMPLEX);
    // RUN_TEST(test_copy_over_3d_to_3d_1_1_0_to_1_1_0);
    // RUN_TEST(test_copy_over_3d_to_3d_1_1_0_to_1_0_1);
    // RUN_TEST(test_copy_over_3d_to_3d_1_1_0_to_0_1_1);
    // RUN_TEST(test_copy_over_3d_to_3d_1_0_1_to_1_1_0);
    // RUN_TEST(test_copy_over_3d_to_3d_1_0_1_to_1_0_1);
    // RUN_TEST(test_copy_over_3d_to_3d_1_0_1_to_0_1_1);
    // RUN_TEST(test_copy_over_3d_to_3d_0_1_1_to_1_1_0);
    // RUN_TEST(test_copy_over_3d_to_3d_0_1_1_to_1_0_1);
    // RUN_TEST(test_copy_over_3d_to_3d_0_1_1_to_0_1_1);
    // RUN_TEST(test_copy_over_3d_to_3d_0_0_1_to_0_1_0);
    // RUN_TEST(test_copy_over_3d_to_3d_0_0_1_to_1_0_0);
    // RUN_TEST(test_copy_over_3d_to_3d_0_0_1_to_0_0_1);
    // RUN_TEST(test_copy_over_3d_to_3d_0_1_0_to_0_1_0);
    // RUN_TEST(test_copy_over_3d_to_3d_0_1_0_to_1_0_0);
    // RUN_TEST(test_copy_over_3d_to_3d_0_1_0_to_0_0_1);
    // RUN_TEST(test_copy_over_3d_to_3d_1_0_0_to_0_1_0);
    // RUN_TEST(test_copy_over_3d_to_3d_1_0_0_to_1_0_0);
    // RUN_TEST(test_copy_over_3d_to_3d_1_0_0_to_0_0_1);
    // RUN_TEST(test_dot_a_1d_double_1d_double);
    // RUN_TEST(test_dot_a_1d_double_1d_complex);
    // RUN_TEST(test_dot_a_1d_int_1d_int);
    // RUN_TEST(test_dot_a_1d_int_1d_complex);
    // RUN_TEST(test_dot_a_1d_float_1d_float);
    // RUN_TEST(test_dot_a_1d_float_1d_complex);
    // RUN_TEST(test_dot_a_1d_complex_1d_double);
    // RUN_TEST(test_dot_a_1d_complex_1d_int);
    // RUN_TEST(test_dot_a_1d_complex_1d_float);
    // RUN_TEST(test_dot_a_1d_complex_1d_complex);
    // RUN_TEST(test_dot_a_1d_double_2d_double);
    // RUN_TEST(test_dot_a_1d_double_2d_complex);
    // RUN_TEST(test_dot_a_1d_int_2d_int);
    // RUN_TEST(test_dot_a_1d_int_2d_complex);
    // RUN_TEST(test_dot_a_1d_float_2d_float);
    // RUN_TEST(test_dot_a_1d_float_2d_complex);
    // RUN_TEST(test_dot_a_1d_complex_2d_double);
    // RUN_TEST(test_dot_a_1d_complex_2d_int);
    // RUN_TEST(test_dot_a_1d_complex_2d_float);
    // RUN_TEST(test_dot_a_1d_complex_2d_complex);
    // RUN_TEST(test_dot_a_2d_double_1d_double);
    // RUN_TEST(test_dot_a_2d_double_1d_complex);
    // RUN_TEST(test_dot_a_2d_int_1d_int);
    // RUN_TEST(test_dot_a_2d_int_1d_complex);
    // RUN_TEST(test_dot_a_2d_float_1d_float);
    // RUN_TEST(test_dot_a_2d_float_1d_complex);
    // RUN_TEST(test_dot_a_2d_complex_1d_double);
    // RUN_TEST(test_dot_a_2d_complex_1d_int);
    // RUN_TEST(test_dot_a_2d_complex_1d_float);
    // RUN_TEST(test_dot_a_2d_complex_1d_complex);
    // RUN_TEST(test_dot_a_2d_double_2d_double);
    // RUN_TEST(test_dot_a_2d_double_2d_complex);
    // RUN_TEST(test_dot_a_2d_int_2d_int);
    // RUN_TEST(test_dot_a_2d_int_2d_complex);
    // RUN_TEST(test_dot_a_2d_float_2d_float);
    // RUN_TEST(test_dot_a_2d_float_2d_complex);
    // RUN_TEST(test_dot_a_2d_complex_2d_double);
    // RUN_TEST(test_dot_a_2d_complex_2d_int);
    // RUN_TEST(test_dot_a_2d_complex_2d_float);
    // RUN_TEST(test_dot_a_2d_complex_2d_complex);
    // RUN_TEST(diag_a_1d_double);
    // RUN_TEST(diag_a_1d_int);
    // RUN_TEST(diag_a_1d_float);
    // RUN_TEST(diag_a_1d_complex);
    // RUN_TEST(diag_a_2d_double_1_0);
    // RUN_TEST(diag_a_2d_int_1_0);
    // RUN_TEST(diag_a_2d_float_1_0);
    // RUN_TEST(diag_a_2d_complex_1_0);
    // RUN_TEST(diag_a_2d_double_0_1);
    // RUN_TEST(diag_a_2d_int_0_1);
    // RUN_TEST(diag_a_2d_float_0_1);
    // RUN_TEST(diag_a_2d_complex_0_1);
    // RUN_TEST(diag_a_3d_double_1_0_0);
    // RUN_TEST(diag_a_3d_int_1_0_0);
    // RUN_TEST(diag_a_3d_float_1_0_0);
    // RUN_TEST(diag_a_3d_complex_1_0_0);
    // RUN_TEST(diag_a_3d_double_0_1_0);
    // RUN_TEST(diag_a_3d_int_0_1_0);
    // RUN_TEST(diag_a_3d_float_0_1_0);
    // RUN_TEST(diag_a_3d_complex_0_1_0);
    // RUN_TEST(slice_a_2d_1_0_DOUBLE);
    // RUN_TEST(slice_a_2d_1_0_INT);
    // RUN_TEST(slice_a_2d_1_0_FLOAT);
    // RUN_TEST(slice_a_2d_1_0_COMPLEX);
    // RUN_TEST(slice_a_2d_0_1_DOUBLE);
    // RUN_TEST(slice_a_2d_0_1_INT);
    // RUN_TEST(slice_a_2d_0_1_FLOAT);
    // RUN_TEST(slice_a_2d_0_1_COMPLEX);
    // RUN_TEST(slice_a_3d_1_0_0_DOUBLE);
    // RUN_TEST(slice_a_3d_1_0_0_INT);
    // RUN_TEST(slice_a_3d_1_0_0_FLOAT);
    // RUN_TEST(slice_a_3d_1_0_0_COMPLEX);
    // RUN_TEST(slice_a_3d_0_1_0_DOUBLE);
    // RUN_TEST(slice_a_3d_0_1_0_INT);
    // RUN_TEST(slice_a_3d_0_1_0_FLOAT);
    // RUN_TEST(slice_a_3d_0_0_1_COMPLEX);
    // RUN_TEST(slice_a_3d_0_0_1_DOUBLE);
    // RUN_TEST(slice_a_3d_0_0_1_INT);
    // RUN_TEST(slice_a_3d_0_0_1_FLOAT);
    // RUN_TEST(slice_a_3d_0_0_1_COMPLEX);
    // RUN_TEST(slice_a_3d_1_1_0_DOUBLE);
    // RUN_TEST(slice_a_3d_1_1_0_INT);
    // RUN_TEST(slice_a_3d_1_1_0_FLOAT);
    // RUN_TEST(slice_a_3d_1_1_0_COMPLEX);
    // RUN_TEST(slice_a_3d_1_0_1_DOUBLE);
    // RUN_TEST(slice_a_3d_1_0_1_INT);
    // RUN_TEST(slice_a_3d_1_0_1_FLOAT);
    // RUN_TEST(slice_a_3d_1_0_1_COMPLEX);
    // RUN_TEST(slice_a_3d_0_1_1_DOUBLE);
    // RUN_TEST(slice_a_3d_0_1_1_INT);
    // RUN_TEST(slice_a_3d_0_1_1_FLOAT);
    // RUN_TEST(slice_a_3d_0_1_1_COMPLEX);
    // RUN_TEST(test_conjugate_a);
    // RUN_TEST(test_fill_diagonal_a_2d_double);
    // RUN_TEST(test_fill_diagonal_a_2d_int);
    // RUN_TEST(test_fill_diagonal_a_2d_float);
    // RUN_TEST(test_fill_diagonal_a_2d_complex);
    // RUN_TEST(test_mean_a_double);
    // RUN_TEST(test_mean_a_int);
    // RUN_TEST(test_mean_a_float);
    // RUN_TEST(test_mean_a_complex);
    // RUN_TEST(test_outer_a_double);
    // RUN_TEST(test_outer_a_int);
    // RUN_TEST(test_outer_a_float);
    // RUN_TEST(test_outer_a_complex);
    // RUN_TEST(test_harmard_element_wise_inverse_a);
    // RUN_TEST(test_diag_harmard_element_wise_inverse_a);
    // RUN_TEST(test_transpose_a_double_1);
    // RUN_TEST(test_transpose_a_double_2);
    // RUN_TEST(test_transpose_a_double_3);
    // RUN_TEST(test_transpose_a_int_1);
    // RUN_TEST(test_transpose_a_int_2);
    // RUN_TEST(test_transpose_a_int_3);
    // RUN_TEST(test_transpose_a_float_1);
    // RUN_TEST(test_transpose_a_float_2);
    // RUN_TEST(test_transpose_a_float_3);
    // RUN_TEST(test_transpose_a_complex_1);
    // RUN_TEST(test_transpose_a_complex_2);
    // RUN_TEST(test_transpose_a_complex_3);
    // RUN_TEST(test_save_to_file_a_double);
    // RUN_TEST(test_save_to_file_a_int);
    // RUN_TEST(test_save_to_file_a_float);
    // RUN_TEST(test_save_to_file_a_complex);
    // RUN_TEST(test_read_from_file_a_double);
    // RUN_TEST(test_read_from_file_a_int);
    // RUN_TEST(test_read_from_file_a_float);
    // RUN_TEST(test_read_from_file_a_complex);
    // RUN_TEST(test_std_a_double);
    // RUN_TEST(test_std_a_int);
    // RUN_TEST(test_std_a_float);
    // RUN_TEST(test_std_a_complex);
    // RUN_TEST(test_equal_a_double);
    // RUN_TEST(test_equal_a_int);
    // RUN_TEST(test_equal_a_float);
    // RUN_TEST(test_equal_a_complex);
    // RUN_TEST(test_reciprocal_a_double);
    // RUN_TEST(test_reciprocal_a_float);
    // RUN_TEST(test_reciprocal_a_int);
    // RUN_TEST(test_diag_reciprocal_a_double);
    // RUN_TEST(test_diag_reciprocal_a_float);
    // RUN_TEST(test_diag_reciprocal_a_int);
    // RUN_TEST(test_real_a);
    // RUN_TEST(test_imag_a);
    // RUN_TEST(test_contains_double_true);
    // RUN_TEST(test_contains_double_false);
    // RUN_TEST(test_contains_int_true);
    // RUN_TEST(test_contains_int_false);
    // RUN_TEST(test_contains_float_true);
    // RUN_TEST(test_contains_float_false);
    // RUN_TEST(test_contains_complex_true);
    // RUN_TEST(test_contains_complex_false);
    // RUN_TEST(test_index_of_element_double_true);
    // RUN_TEST(test_index_of_element_double_false);
    // RUN_TEST(test_index_of_element_int_true);
    // RUN_TEST(test_index_of_element_int_false);
    // RUN_TEST(test_index_of_element_float_true);
    // RUN_TEST(test_index_of_element_float_false);
    // RUN_TEST(test_index_of_element_complex_true);
    // RUN_TEST(test_index_of_element_complex_false);
    // RUN_TEST(test_norm_a_double);
    // RUN_TEST(test_norm_a_int);
    // RUN_TEST(test_norm_a_float);
    // RUN_TEST(test_norm_a_complex);
    // RUN_TEST(test_svd_a_complex);
    // RUN_TEST(test_pinv_a_complex);
    // RUN_TEST(test_abs_a_double);
    // RUN_TEST(test_abs_a_int);
    // RUN_TEST(test_abs_a_float);
    // RUN_TEST(test_abs_a_complex);
    // RUN_TEST(test_power_a_double);
    // RUN_TEST(test_power_a_int);
    // RUN_TEST(test_power_a_float);
    // RUN_TEST(test_power_a_complex);
    // RUN_TEST(test_sqrt_a_double);
    // RUN_TEST(test_sqrt_a_int);
    // RUN_TEST(test_sqrt_a_float);
    // RUN_TEST(test_sqrt_a_complex);
//     return UNITY_END();
// }
