#include "unity.h"
#include "data_store.h"
#include "jagged_array.h"

// void setUp(void) {}
// void tearDown(void) {}

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif
#define SPEED_OF_LIGHT 299792458.0

void test_create_ds() {
    char directory[] = "./test";
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;
    int order = 2;
    int seed = 12345678;
    int num_workers = 4;
    size_t grid_length = 4;
    double snr = 1000;
    double max_amp = 2;
    enum AlgorithmType algorithm_type = STEFCAL;
    size_t amount_hyperparameters = 2;
    struct array* hyperparameters_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -2.0,2.0,-2.0,2.0
                                    });

    struct DataStore* data_store = new_ds("data_store", directory, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds);
    TEST_ASSERT_NOT_NULL(data_store);
    free_ds(data_store);
    free_a(hyperparameters_bounds);
}

void test_print_ds() {
    char directory[] = "./test";
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;
    int order = 2;
    int seed = 12345678;
    int num_workers = 4;
    size_t grid_length = 4;
    double snr = 1000;
    double max_amp = 2;
    enum AlgorithmType algorithm_type = STEFCAL;
    size_t amount_hyperparameters = 2;
    struct array* hyperparameters_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -2.0,2.0,-2.0,2.0
                                    });

    struct DataStore* data_store = new_ds("data_store", directory, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds);
    TEST_ASSERT_NOT_NULL(data_store);
    print_ds(data_store);
    free_ds(data_store);
    free_a(hyperparameters_bounds);
}

void test_save_to_file_ds() {
    char directory[] = "./test";
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;
    int order = 2;
    int seed = 12345678;
    int num_workers = 4;
    size_t grid_length = 4;
    double snr = 1000;
    double max_amp = 2;
    enum AlgorithmType algorithm_type = STEFCAL;
    size_t amount_hyperparameters = 2;
    struct array* hyperparameters_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -2.0,2.0,-2.0,2.0
                                    });

    struct DataStore* data_store = new_ds("data_store", directory, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds);
    TEST_ASSERT_NOT_NULL(data_store);
    save_to_file_ds(data_store);
    free_ds(data_store);
    free_a(hyperparameters_bounds);
}

void test_read_from_file_ds() {
    struct DataStore* data_store = read_from_file_ds("./test","data_store");
    TEST_ASSERT_NOT_NULL(data_store);
    print_ds(data_store);
    free_ds(data_store);
}

void test_start_ds() {
    char directory[] = "../../../../media/pieter/wd/test";
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;
    int order = 2;
    int seed = 12345678;
    int num_workers = 4;
    size_t grid_length = 2;
    double snr = 1000;
    double max_amp = 2;
    enum AlgorithmType algorithm_type = STEFCAL_LAMBDA;
    size_t amount_hyperparameters = 2;
    struct array* hyperparameters_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        0.0,5.0,0.0,5.0
                                    });

    struct DataStore* data_store = new_structure_ds("data_store", directory, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds);
    TEST_ASSERT_NOT_NULL(data_store);
    start_ds(data_store);
    free_ds(data_store);
    free_a(hyperparameters_bounds);
}

void test_new_structure_ds() {
    char directory[] = "./test";
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;
    int order = 2;
    int seed = 12345678;
    int num_workers = 4;
    size_t grid_length = 4;
    double snr = 1000;
    double max_amp = 2;
    enum AlgorithmType algorithm_type = STEFCAL;
    size_t amount_hyperparameters = 2;
    struct array* hyperparameters_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -2.0,2.0,-2.0,2.0
                                    });

    struct DataStore* data_store = new_structure_ds("data_store", directory, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds);
    TEST_ASSERT_NOT_NULL(data_store);
    free_ds(data_store);
    free_a(hyperparameters_bounds);
}

void test_read_structure_ds() {
    struct DataStore* data_store = read_structure_ds("./test","data_store");
    TEST_ASSERT_NOT_NULL(data_store);
    free_ds(data_store);
}

void test_stefcal_lm_lambda() {
    char directory[] = "./test";
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 2;
    double bas_len = 50;
    int order = 2;
    int seed = 12345678;
    int num_workers = 4;
    size_t grid_length = 2;
    double snr = 1000;
    double max_amp = 2;
    enum AlgorithmType algorithm_type = STEFCAL_LM_LAMBDA;
    size_t amount_hyperparameters = 2;
    struct array* hyperparameters_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -0.5,1.0,-0.5,1.0
                                    });

    struct DataStore* data_store = new_structure_ds("data_store", directory, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds);
    TEST_ASSERT_NOT_NULL(data_store);
    start_ds(data_store);
    free_ds(data_store);
    free_a(hyperparameters_bounds);
}

void test_map_average_iterations_ds() {
    struct DataStore* datastore = read_structure_ds("../../../../media/pieter/wd/data/data_stefcal","de619e14-9d41-4cda-ba30-2a70fe9358f5");
    map_average_iterations_std_ds(datastore);
    free_ds(datastore);
}

void test_compare_parameters_ds_false() {
    char directory[] = "./test";
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;
    int order = 2;
    int seed = 12345678;
    int num_workers = 4;
    size_t grid_length = 2;
    double snr = 1000;
    double max_amp = 2;
    enum AlgorithmType algorithm_type = STEFCAL;
    size_t amount_hyperparameters = 2;
    struct array* hyperparameters_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -2.0,2.0,-2.0,2.0
                                    });

    size_t grid_length_2 = 7;

    struct DataStore* data_store = new_ds("data_store", directory, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds);
    
    
    bool check = compare_parameters_ds(data_store, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &grid_length_2, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds);

    TEST_ASSERT_EQUAL(false, check);

    free_a(hyperparameters_bounds);
    free_ds(data_store);
}

void test_compare_parameters_ds_true() {
    char directory[] = "./test";
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;
    int order = 2;
    int seed = 12345678;
    int num_workers = 4;
    size_t grid_length = 4;
    double snr = 1000;
    double max_amp = 2;
    enum AlgorithmType algorithm_type = STEFCAL;
    size_t amount_hyperparameters = 2;
    struct array* hyperparameters_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -2.0,2.0,-2.0,2.0
                                    });

    struct DataStore* data_store = new_ds("data_store", directory, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds);
    bool check = compare_parameters_ds(data_store, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds);

    TEST_ASSERT_EQUAL(true, check);

    free_a(hyperparameters_bounds);
    free_ds(data_store);
}

//Main function to run the tests
// int main(void) {
//     UNITY_BEGIN();
//     // RUN_TEST(test_create_ds);
//     // RUN_TEST(test_print_ds);
//     // RUN_TEST(test_save_to_file_ds);
//     // RUN_TEST(test_read_from_file_ds);
//     RUN_TEST(test_start_ds);
//     // RUN_TEST(test_new_structure_ds);
//     // RUN_TEST(test_read_structure_ds);
//     // RUN_TEST(test_stefcal_lm_lambda);
//     // RUN_TEST(test_map_average_iterations_ds);
//     // RUN_TEST(test_compare_parameters_ds_false);
//     // RUN_TEST(test_compare_parameters_ds_true);
//     return UNITY_END();
// }   
