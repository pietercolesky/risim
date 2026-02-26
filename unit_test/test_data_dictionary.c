#include "unity.h"
#include "data_dictionary.h"
#include <stdio.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif
#define SPEED_OF_LIGHT 299792458.0

// void setUp(void) {}
// void tearDown(void) {}

void test_new_dd() {
    create_directory("./test");
    remove("./test/data_dictionary.ddconfig");
    struct DataDictionary* data_dictionary = new_dd("data_dictionary", "./test");
    TEST_ASSERT_NOT_NULL(data_dictionary);
    remove("./test/data_dictionary.ddconfig");

    free_dd(data_dictionary);
}

void test_print_dd() {
    struct DataDictionary* data_dictionary = new_dd("data_dictionary", "./test");
    TEST_ASSERT_NOT_NULL(data_dictionary);
    print_dd(data_dictionary);
    remove("./test/data_dictionary.ddconfig");
    free_dd(data_dictionary);
}

void test_add_item_dd() {
    struct DataDictionary* data_dictionary = new_dd("data_dictionary", "./test");
    add_item_dd(data_dictionary, "item_1");
    TEST_ASSERT_NOT_NULL(data_dictionary);
    remove("./test/data_dictionary.ddconfig");
    free_dd(data_dictionary);
}

void test_increase_items() {
    struct DataDictionary* data_dictionary = new_dd("data_dictionary", "./test");
    add_item_dd(data_dictionary, "item_1");
    add_item_dd(data_dictionary, "item_2");
    add_item_dd(data_dictionary, "item_3");
    add_item_dd(data_dictionary, "item_4");
    add_item_dd(data_dictionary, "item_5");
    add_item_dd(data_dictionary, "item_6");
    add_item_dd(data_dictionary, "item_7");
    add_item_dd(data_dictionary, "item_8");
    add_item_dd(data_dictionary, "item_9");
    add_item_dd(data_dictionary, "item_10");
    add_item_dd(data_dictionary, "item_11");
    add_item_dd(data_dictionary, "item_12");
    add_item_dd(data_dictionary, "item_13");
    add_item_dd(data_dictionary, "item_14");
    add_item_dd(data_dictionary, "item_15");
    add_item_dd(data_dictionary, "item_16");
    add_item_dd(data_dictionary, "item_17");
    add_item_dd(data_dictionary, "item_18");
    add_item_dd(data_dictionary, "item_19");
    add_item_dd(data_dictionary, "item_20");
    add_item_dd(data_dictionary, "item_21");
    TEST_ASSERT_NOT_NULL(data_dictionary);
    remove("./test/data_dictionary.ddconfig");
    free_dd(data_dictionary);
}

void test_save_to_file_dd() {
    struct DataDictionary* data_dictionary = new_dd("data_dictionary", "./test");
    add_item_dd(data_dictionary, "item_1");
    add_item_dd(data_dictionary, "item_2");
    add_item_dd(data_dictionary, "item_3");
    add_item_dd(data_dictionary, "item_4");
    add_item_dd(data_dictionary, "item_5");
    add_item_dd(data_dictionary, "item_6");
    add_item_dd(data_dictionary, "item_7");
    add_item_dd(data_dictionary, "item_8");
    add_item_dd(data_dictionary, "item_9");
    add_item_dd(data_dictionary, "item_10");
    add_item_dd(data_dictionary, "item_11");
    add_item_dd(data_dictionary, "item_12");
    add_item_dd(data_dictionary, "item_13");
    add_item_dd(data_dictionary, "item_14");
    add_item_dd(data_dictionary, "item_15");
    add_item_dd(data_dictionary, "item_16");
    add_item_dd(data_dictionary, "item_17");
    add_item_dd(data_dictionary, "item_18");
    add_item_dd(data_dictionary, "item_19");
    add_item_dd(data_dictionary, "item_20");
    add_item_dd(data_dictionary, "item_21");
    TEST_ASSERT_NOT_NULL(data_dictionary);
    save_to_file_dd(data_dictionary);
    free_dd(data_dictionary);
}

void test_read_from_file_dd() {
    struct DataDictionary* data_dictionary = read_from_file_dd("./test", "data_dictionary");
    TEST_ASSERT_NOT_NULL(data_dictionary);
    remove("./test/data_dictionary.ddconfig");
    free_dd(data_dictionary);
}

void test_contains_item_dd() {
    struct DataDictionary* data_dictionary = new_dd("data_dictionary", "./test");
    add_item_dd(data_dictionary, "item_1");
    add_item_dd(data_dictionary, "item_2");
    add_item_dd(data_dictionary, "item_3");
    add_item_dd(data_dictionary, "item_4");
    add_item_dd(data_dictionary, "item_5");
    add_item_dd(data_dictionary, "item_6");
    add_item_dd(data_dictionary, "item_7");
    add_item_dd(data_dictionary, "item_8");
    add_item_dd(data_dictionary, "item_9");
    add_item_dd(data_dictionary, "item_10");
    add_item_dd(data_dictionary, "item_11");
    add_item_dd(data_dictionary, "item_12");
    add_item_dd(data_dictionary, "item_13");
    add_item_dd(data_dictionary, "item_14");
    add_item_dd(data_dictionary, "item_15");
    add_item_dd(data_dictionary, "item_16");
    add_item_dd(data_dictionary, "item_17");
    add_item_dd(data_dictionary, "item_18");
    add_item_dd(data_dictionary, "item_19");
    add_item_dd(data_dictionary, "item_20");
    add_item_dd(data_dictionary, "item_21");
    TEST_ASSERT_NOT_NULL(data_dictionary);
    bool check = contains_item_dd(data_dictionary, "item_21");
    TEST_ASSERT_EQUAL(true, check);
    remove("./test/data_dictionary.ddconfig");
    free_dd(data_dictionary);
}

void test_update_file_dd() {
    struct DataDictionary* data_dictionary = new_dd("data_dictionary", "./test");
    add_item_dd(data_dictionary, "item_1");
    add_item_dd(data_dictionary, "item_2");
    add_item_dd(data_dictionary, "item_3");
    add_item_dd(data_dictionary, "item_4");
    add_item_dd(data_dictionary, "item_5");
    add_item_dd(data_dictionary, "item_6");
    add_item_dd(data_dictionary, "item_7");
    add_item_dd(data_dictionary, "item_8");
    add_item_dd(data_dictionary, "item_9");
    add_item_dd(data_dictionary, "item_10");
    add_item_dd(data_dictionary, "item_11");
    add_item_dd(data_dictionary, "item_12");
    add_item_dd(data_dictionary, "item_13");
    add_item_dd(data_dictionary, "item_14");
    add_item_dd(data_dictionary, "item_15");
    add_item_dd(data_dictionary, "item_16");
    add_item_dd(data_dictionary, "item_17");
    add_item_dd(data_dictionary, "item_18");
    add_item_dd(data_dictionary, "item_19");
    add_item_dd(data_dictionary, "item_20");
    add_item_dd(data_dictionary, "item_21");
    save_to_file_dd(data_dictionary);
    
    add_item_dd(data_dictionary, "item_50");
    add_item_dd(data_dictionary, "item_51");
    add_item_dd(data_dictionary, "item_52");
    add_item_dd(data_dictionary, "item_53");
    struct DataDictionary* updated_data_dictionary = update_file_dd(data_dictionary);
    
    TEST_ASSERT_NOT_NULL(data_dictionary);

    free_dd(data_dictionary);
    free_dd(updated_data_dictionary);
}

void test_read_and_add_dd() {
    struct DataDictionary* data_dictionary = read_from_file_dd("./test", "data_dictionary");
    TEST_ASSERT_NOT_NULL(data_dictionary);
    add_item_dd(data_dictionary, "item_100");
    free_dd(data_dictionary);
    remove("./test/data_dictionary.ddconfig");
}

void test_setup_data_dictionary() {
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
    size_t grid_length = 50;
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

    struct DataDictionary* data_dictionary = new_save_dd("data_dictionary", "./test");
    TEST_ASSERT_NOT_NULL(data_dictionary);

    struct DataStore* data_store = new_data_store_update_dd(&data_dictionary, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds);

    free_ds(data_store);
    free_dd(data_dictionary);
    free_a(hyperparameters_bounds);
    remove("./test/data_dictionary.ddconfig");
}

void test_add_item_update() {
    struct DataDictionary* data_dictionary = new_dd("data_dictionary", "./test");
    add_item_dd(data_dictionary, "item_1");
    add_item_dd(data_dictionary, "item_2");
    add_item_dd(data_dictionary, "item_3");
    add_item_dd(data_dictionary, "item_4");
    add_item_dd(data_dictionary, "item_5");
    add_item_dd(data_dictionary, "item_6");
    add_item_dd(data_dictionary, "item_7");
    add_item_dd(data_dictionary, "item_8");
    add_item_dd(data_dictionary, "item_9");
    add_item_dd(data_dictionary, "item_10");
    add_item_dd(data_dictionary, "item_11");
    add_item_dd(data_dictionary, "item_12");
    add_item_dd(data_dictionary, "item_13");
    add_item_dd(data_dictionary, "item_14");
    add_item_dd(data_dictionary, "item_15");
    add_item_dd(data_dictionary, "item_16");
    add_item_dd(data_dictionary, "item_17");
    add_item_dd(data_dictionary, "item_18");
    add_item_dd(data_dictionary, "item_19");
    add_item_dd(data_dictionary, "item_20");
    add_item_dd(data_dictionary, "item_21");
    save_to_file_dd(data_dictionary);


    add_item_dd(data_dictionary, "item_15");
    add_item_dd(data_dictionary, "item_32");
    struct DataDictionary* updated_data_dictionary = update_file_dd(data_dictionary);
    TEST_ASSERT_NOT_NULL(data_dictionary);
    free_dd(data_dictionary);
    data_dictionary = updated_data_dictionary;
    free_dd(data_dictionary);
    remove("./test/data_dictionary.ddconfig");
}



//Main function to run the tests
// int main(void) {
//     UNITY_BEGIN();
//     RUN_TEST(test_new_dd);
//     RUN_TEST(test_print_dd);
//     RUN_TEST(test_add_item_dd);
//     RUN_TEST(test_increase_items);
//     RUN_TEST(test_save_to_file_dd);
//     RUN_TEST(test_read_from_file_dd);
//     RUN_TEST(test_contains_item_dd);
//     RUN_TEST(test_update_file_dd);
//     RUN_TEST(test_read_and_add_dd);
//     RUN_TEST(test_setup_data_dictionary);
//     RUN_TEST(test_add_item_update);
//     return UNITY_END();
// }   