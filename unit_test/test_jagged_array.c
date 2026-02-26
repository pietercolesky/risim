#include "unity.h"
#include "jagged_array.h"

// void setUp(void) {}
// void tearDown(void) {}

void test_new_js() {
    size_t size = 3;
    struct Jaggedarray* jagged_array = new_ja(&size);
    TEST_ASSERT_NOT_NULL(jagged_array);
    free_ja(jagged_array);
}

void test_set_js() {
    size_t size = 3;
    struct Jaggedarray* jagged_array = new_ja(&size);
    TEST_ASSERT_NOT_NULL(jagged_array);

    struct array* array = zeros_a(&(size_t){1}, (size_t[]){4}, &(enum DataType){DOUBLE});

    set_ja(jagged_array, array, &(size_t){0});

    free_a(array);
    free_ja(jagged_array);
}

void test_get_js() {
    size_t size = 3;
    struct Jaggedarray* jagged_array = new_ja(&size);
    TEST_ASSERT_NOT_NULL(jagged_array);

    struct array* array = zeros_a(&(size_t){1}, (size_t[]){4}, &(enum DataType){DOUBLE});

    set_ja(jagged_array, array, &(size_t){0});

    struct array* array_get = get_ja(jagged_array, &(size_t){0});

    double* arr_1 = get_a(array);
    double* arr_2 = get_a(array_get);


    for (size_t i = 0; i < get_size_a(array); i++) {
        TEST_ASSERT_EQUAL(arr_1[i], arr_2[i]);
    }

    free_a(array);
    free_ja(jagged_array);
}

void test_save_to_file_js() {
    size_t size = 3;
    struct Jaggedarray* jagged_array = new_ja(&size);
    TEST_ASSERT_NOT_NULL(jagged_array);

    struct array* array = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3]){
                                    1 + 0*I,2 + 0*I,3 + 0*I
                                });
    for (size_t i = 0; i < size; i++) {
        set_ja(jagged_array, array, &(size_t){i});
    }

    save_to_file_ja(jagged_array, "./unit_test/bin", "test");
    free_a(array);
    free_ja(jagged_array);
}

void test_read_from_file_ja() {
    struct Jaggedarray* jagged_array = read_from_file_ja("./unit_test/bin/test.jarray");
    TEST_ASSERT_NOT_NULL(jagged_array);
    print_ja(jagged_array);
    free_ja(jagged_array);
}

void test_average_ja_COMPLEX() {
    size_t size = 3;
    struct Jaggedarray* jagged_array = new_ja(&size);
    TEST_ASSERT_NOT_NULL(jagged_array);

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
                                    1 + 5*I,2 + 3*I,3 + 2*I
                                });

    struct array* array3 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3]){
                                    8 + 9*I,2 + 4*I,2 + 9*I
                                });

    set_ja(jagged_array, array1, &(size_t){0});
    set_ja(jagged_array, array2, &(size_t){1});
    set_ja(jagged_array, array3, &(size_t){2});

    double complex* average = average_ja(jagged_array);

    TEST_ASSERT_EQUAL(2.666667 + 3.555556*I, *average);

    free(average);
    free_a(array1);
    free_a(array2);
    free_a(array3);
    free_ja(jagged_array);
}

void test_average_ja_DOUBLE() {
    size_t size = 3;
    struct Jaggedarray* jagged_array = new_ja(&size);
    TEST_ASSERT_NOT_NULL(jagged_array);

    struct array* array1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){DOUBLE}, 
                                (double [3]){
                                    1,2,3
                                });

    struct array* array2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3]){
                                    1,2,3
                                });

    struct array* array3 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3]){
                                    8,2,2
                                });

    set_ja(jagged_array, array1, &(size_t){0});
    set_ja(jagged_array, array2, &(size_t){1});
    set_ja(jagged_array, array3, &(size_t){2});

    double* average = average_ja(jagged_array);

    TEST_ASSERT_EQUAL(2.666667, *average);

    free(average);
    free_a(array1);
    free_a(array2);
    free_a(array3);
    free_ja(jagged_array);
}

void test_average_ja_INT() {
    size_t size = 3;
    struct Jaggedarray* jagged_array = new_ja(&size);
    TEST_ASSERT_NOT_NULL(jagged_array);

    struct array* array1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){INT}, 
                                (int [3]){
                                    1,2,3
                                });

    struct array* array2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){INT}, 
                                (int[3]){
                                    1,2,3
                                });

    struct array* array3 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){INT}, 
                                (int[3]){
                                    8,2,2
                                });

    set_ja(jagged_array, array1, &(size_t){0});
    set_ja(jagged_array, array2, &(size_t){1});
    set_ja(jagged_array, array3, &(size_t){2});

    double* average = average_ja(jagged_array);

    TEST_ASSERT_EQUAL(2.666667, *average);

    free(average);
    free_a(array1);
    free_a(array2);
    free_a(array3);
    free_ja(jagged_array);
}

void test_average_ja_FLOAT() {
    size_t size = 3;
    struct Jaggedarray* jagged_array = new_ja(&size);
    TEST_ASSERT_NOT_NULL(jagged_array);

    struct array* array1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){FLOAT}, 
                                (float [3]){
                                    1,2,3
                                });

    struct array* array2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3]){
                                    1,2,3
                                });

    struct array* array3 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3]){
                                    8,2,2
                                });

    set_ja(jagged_array, array1, &(size_t){0});
    set_ja(jagged_array, array2, &(size_t){1});
    set_ja(jagged_array, array3, &(size_t){2});

    float* average = average_ja(jagged_array);

    TEST_ASSERT_EQUAL(2.666667, *average);

    free(average);
    free_a(array1);
    free_a(array2);
    free_a(array3);
    free_ja(jagged_array);
}

void test_norm_ja_COMPLEX() {
    size_t size = 4;
    struct Jaggedarray* jagged_array = new_ja(&size);
    TEST_ASSERT_NOT_NULL(jagged_array);

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
                                    1 + 5*I,2 + 3*I,3 + 2*I
                                });

    struct array* array3 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3]){
                                    8 + 9*I,2 + 4*I,2 + 9*I
                                });

    struct array* array4 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){COMPLEX}, 
                                (double complex[3]){
                                    8 + 9*I,2 + 4*I,2 + 9*I
                                });

    set_ja(jagged_array, array1, &(size_t){0});
    set_ja(jagged_array, array2, &(size_t){1});
    set_ja(jagged_array, array3, &(size_t){2});
    set_ja(jagged_array, array4, &(size_t){3});

    double* norm = norm_ja(jagged_array);

    TEST_ASSERT_EQUAL(23.79, *norm);

    free(norm);
    free_a(array1);
    free_a(array2);
    free_a(array3);
    free_a(array4);
    free_ja(jagged_array);
}

void test_norm_ja_DOUBLE() {
    size_t size = 4;
    struct Jaggedarray* jagged_array = new_ja(&size);
    TEST_ASSERT_NOT_NULL(jagged_array);

    struct array* array1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){DOUBLE}, 
                                (double [3]){
                                    1,2,3
                                });

    struct array* array2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3]){
                                    1,2,3
                                });

    struct array* array3 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3]){
                                    8,2,2
                                });

    struct array* array4 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){DOUBLE}, 
                                (double[3]){
                                    8,2,2
                                });

    set_ja(jagged_array, array1, &(size_t){0});
    set_ja(jagged_array, array2, &(size_t){1});
    set_ja(jagged_array, array3, &(size_t){2});
    set_ja(jagged_array, array4, &(size_t){3});

    double* norm = norm_ja(jagged_array);

    TEST_ASSERT_EQUAL(13.11, *norm);

    free(norm);
    free_a(array1);
    free_a(array2);
    free_a(array3);
    free_a(array4);
    free_ja(jagged_array);
}

void test_norm_ja_INT() {
    size_t size = 4;
    struct Jaggedarray* jagged_array = new_ja(&size);
    TEST_ASSERT_NOT_NULL(jagged_array);

    struct array* array1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){INT}, 
                                (int [3]){
                                    1,2,3
                                });

    struct array* array2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){INT}, 
                                (int[3]){
                                    1,2,3
                                });

    struct array* array3 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){INT}, 
                                (int[3]){
                                    8,2,2
                                });
    
    struct array* array4 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){INT}, 
                                (int[3]){
                                    8,2,2
                                });

    set_ja(jagged_array, array1, &(size_t){0});
    set_ja(jagged_array, array2, &(size_t){1});
    set_ja(jagged_array, array3, &(size_t){2});
    set_ja(jagged_array, array4, &(size_t){3});

    double* norm = norm_ja(jagged_array);

    TEST_ASSERT_EQUAL(13.11, *norm);

    free(norm);
    free_a(array1);
    free_a(array2);
    free_a(array3);
    free_a(array4);
    free_ja(jagged_array);
}

void test_norm_ja_FLOAT() {
    size_t size = 4;
    struct Jaggedarray* jagged_array = new_ja(&size);
    TEST_ASSERT_NOT_NULL(jagged_array);

    struct array* array1 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){FLOAT}, 
                                (float [3]){
                                    1,2,3
                                });

    struct array* array2 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3]){
                                    1,2,3
                                });

    struct array* array3 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3]){
                                    8,2,2
                                });

    struct array* array4 = new_set_a(&(size_t){1},
                                (size_t[]){3}, 
                                &(enum DataType){FLOAT}, 
                                (float[3]){
                                    8,2,2
                                });

    set_ja(jagged_array, array1, &(size_t){0});
    set_ja(jagged_array, array2, &(size_t){1});
    set_ja(jagged_array, array3, &(size_t){2});
    set_ja(jagged_array, array4, &(size_t){3});

    float* norm = norm_ja(jagged_array);

    TEST_ASSERT_EQUAL(13.11, *norm);

    free(norm);
    free_a(array1);
    free_a(array2);
    free_a(array3);
    free_a(array4);
    free_ja(jagged_array);
}

// int main(void) {
//     UNITY_BEGIN();
//     // RUN_TEST(test_new_js);
//     // RUN_TEST(test_set_js);
//     // RUN_TEST(test_get_js);
//     // RUN_TEST(test_save_to_file_js);
//     // RUN_TEST(test_read_from_file_ja);
//     // RUN_TEST(test_average_ja_COMPLEX);
//     // RUN_TEST(test_average_ja_DOUBLE);
//     // RUN_TEST(test_average_ja_INT);
//     // RUN_TEST(test_average_ja_FLOAT);
//     // RUN_TEST(test_norm_ja_COMPLEX);
//     // RUN_TEST(test_norm_ja_DOUBLE);
//     // RUN_TEST(test_norm_ja_INT);
//     // RUN_TEST(test_norm_ja_FLOAT);
//     return UNITY_END();
// } 