#include "unity.h"
#include "progress_bar.h"

// void setUp(void) {}
// void tearDown(void) {}

void test_new_pb() {

    struct ProgressBar* progress_bar = new_pb(&(size_t){50}, &(size_t){100});
    update_pb(progress_bar,&(size_t){100});
    TEST_ASSERT_NOT_NULL(progress_bar);
    stop_free_pb(progress_bar, "");

}

// Main function to run the tests
// int main(void) {
//     UNITY_BEGIN();
//     RUN_TEST(test_new_pb);
//     return UNITY_END();
// }   