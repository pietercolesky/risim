#include "unity.h"
#include "stopwatch.h"

// void setUp(void) {}
// void tearDown(void) {}

void test_new_sw() {
    struct Stopwatch* stopwatch = new_sw();

    TEST_ASSERT_NOT_NULL(stopwatch);

    free_sw(stopwatch);
}

void test_elapsed_sw() {
    struct Stopwatch* stopwatch = new_sw();
    elapsed_sw(stopwatch);

    TEST_ASSERT_NOT_NULL(stopwatch);

    free_sw(stopwatch);
} 

// Main function to run the tests
// int main(void) {
//     UNITY_BEGIN();
//     RUN_TEST(test_new_sw);
//     RUN_TEST(test_elapsed_sw);
//     return UNITY_END();
// }   