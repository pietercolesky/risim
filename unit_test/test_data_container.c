#include "unity.h"
#include "data_container.h"

// void setUp(void) {}
// void tearDown(void) {}

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif
#define SPEED_OF_LIGHT 299792458.0

void test_new_dcon() {
    char* identity = "data_container";
    char* directory = "./test";
    size_t type = 0;
    size_t size = 5;

    struct DataContainer* data_container  = new_dcon(identity, directory, &type, &size);
    // print_dcon(data_container);

    free_dcon(data_container);
}

void test_print_dcon() {
    char* identity = "data_container";
    char* directory = "./test";
    size_t type = 0;
    size_t size = 5;

    struct DataContainer* data_container  = new_dcon(identity, directory, &type, &size);
    print_dcon(data_container);

    free_dcon(data_container);
}

void test_set_item_dcon() {
    char* identity = "data_container";
    char* directory = "./test";
    size_t type = 0;
    size_t size = 5;

    struct DataContainer* data_container  = new_dcon(identity, directory, &type, &size);
    set_item_dcon(data_container, "hello", &(size_t){0});

    free_dcon(data_container);
}

void test_save_to_file_dcon() {
    char* identity = "data_container";
    char* directory = "./test";
    size_t type = 0;
    size_t size = 5;

    struct DataContainer* data_container  = new_dcon(identity, directory, &type, &size);
    set_item_dcon(data_container, "hello0", &(size_t){0});
    set_item_dcon(data_container, "hello1", &(size_t){1});
    set_item_dcon(data_container, "hello2", &(size_t){2});
    set_item_dcon(data_container, "hello3", &(size_t){3});

    save_to_file_dcon(data_container);

    free_dcon(data_container);
}

void test_read_from_file_dcon() {
    struct DataContainer* data_container = read_from_file_dcon("./test","data_container");
    TEST_ASSERT_NOT_NULL(data_container);
    free_dcon(data_container);
}

void test_contains_item_dcon() {
    struct DataContainer* data_container = read_from_file_dcon("./test","data_container");
    TEST_ASSERT_NOT_NULL(data_container);
    bool check = contains_item_dcon(data_container, "hello0");
    TEST_ASSERT_EQUAL(true, check);
    free_dcon(data_container);
}

void test_update_file_dcon() {
    struct DataContainer* data_container = read_from_file_dcon("./test","data_container");
    TEST_ASSERT_NOT_NULL(data_container);

    set_item_dcon(data_container, "change0", &(size_t){0});
    struct DataContainer* updated_data_container = update_file_dcon(data_container);

    free_dcon(data_container);
    free_dcon(updated_data_container);
}

void test_set_item_update_dcon() {
    struct DataContainer* data_container = read_from_file_dcon("./test","data_container");
    TEST_ASSERT_NOT_NULL(data_container);
    set_item_update_dcon(&data_container, "hereIam0", &(size_t){0});
    free_dcon(data_container);
}

void test_get_progress_dcon() {
    struct DataContainer* data_container = read_from_file_dcon("./test","data_container");
    TEST_ASSERT_NOT_NULL(data_container);
    int progress = get_progress_dcon(data_container);
    TEST_ASSERT_EQUAL(4, progress);
    free_dcon(data_container);
}

//Main function to run the tests
// int main(void) {
//     UNITY_BEGIN();
//     RUN_TEST(test_new_dcon);
//     RUN_TEST(test_print_dcon);
//     RUN_TEST(test_set_item_dcon);
//     RUN_TEST(test_save_to_file_dcon);
//     RUN_TEST(test_read_from_file_dcon);
//     RUN_TEST(test_contains_item_dcon);
//     RUN_TEST(test_update_file_dcon);
//     RUN_TEST(test_set_item_update_dcon);
//     RUN_TEST(test_get_progress_dcon);
//     return UNITY_END();
// }   