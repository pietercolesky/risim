#include "unity.h"
#include "thread_args.h"

// void setUp(void) {}
// void tearDown(void) {}

void test_init_ta() {

    struct ThreadArgs* thread_args = init_ta(&(size_t){0}, &(size_t){1});
    thread_args->args[0] = &(int){5};
    TEST_ASSERT_NOT_NULL(thread_args);
    free_ta(thread_args);

}

// Main function to run the tests
// int main(void) {
//     UNITY_BEGIN();
//     RUN_TEST(test_init_ta);
//     return UNITY_END();
// }   