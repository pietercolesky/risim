#include "unity.h"
#include "stopwatch.h"
#include "array.h"
#include "utils.h"
#include "pso.h"

// void setUp(void) {}
// void tearDown(void) {}

void test_new_pso() {
    enum AlgorithmType option = STEFCAL_LAMBDA;
    size_t num_dimensions = 2;
    struct array* position_bounds  = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -0.4, 3,-0.4,3
                                    });
    size_t iterations_max = 10;
    size_t num_particles = 1;
    int seed = 1234;
    char* simulators_directory = "./data/simulators";
    int order = 2;
    size_t k = 5;
    char* dd_name = "name";
    char* dd_directory = "./directory";
    int num_workers = 4;

    struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, & num_particles, &seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers);

    free_pso(pso);
    free_a(position_bounds);
}

void test_generate_random_number_pso() {
    enum AlgorithmType option = STEFCAL_LAMBDA;
    size_t num_dimensions = 2;
    struct array* position_bounds  = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -0.4, 3,-0.4,3
                                    });
    size_t iterations_max = 10;
    size_t num_particles = 1;
    int seed = 1234;
    char* simulators_directory = "./data/simulators";
    int order = 2;
    size_t k = 5;
    char* dd_name = "name";
    char* dd_directory = "./directory";
    int num_workers = 4;

    struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, & num_particles, &seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers);

    double random = generate_random_number_pso(pso, &(size_t){0});

    TEST_ASSERT_EQUAL(0.251166, random);

    free_pso(pso);
    free_a(position_bounds);
}

void test_random_position_pso() {
    enum AlgorithmType option = STEFCAL_LAMBDA;
    size_t num_dimensions = 2;
    struct array* position_bounds  = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -0.5, 3,-0.5,3
                                    });
    size_t iterations_max = 10;
    size_t num_particles = 1;
    int seed = 1234;
    char* simulators_directory = "./data/simulators";
    int order = 2;
    size_t k = 5;
    char* dd_name = "name";
    char* dd_directory = "./directory";
    int num_workers = 4;

    struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, & num_particles, &seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers); 

    for (size_t i = 0; i < 1; i++) {
        struct array* random_position = random_position_pso(pso);
        print_a(random_position);
        free_a(random_position);
    }

    free_pso(pso);
    free_a(position_bounds);
}

void test_random_position_bound1_pso() {
    enum AlgorithmType option = STEFCAL_LAMBDA;
    size_t num_dimensions = 2;
    struct array* position_bounds  = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -0.4, 3,-0.4,3
                                    });
    size_t iterations_max = 10;
    size_t num_particles = 1;
    int seed = 1234;
    char* simulators_directory = "./data/simulators";
    int order = 2;
    size_t k = 5;
    char* dd_name = "name";
    char* dd_directory = "./directory";
    int num_workers = 4;

    struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, & num_particles, &seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers); 

    for (size_t i = 0; i < 1; i++) {
        struct array* random_position = random_position_bound1_pso(pso);
        print_a(random_position);
        free_a(random_position);
    }

    free_pso(pso);
    free_a(position_bounds);
}

void test_random_position_bound2_pso() {
    enum AlgorithmType option = STEFCAL_LAMBDA;
    size_t num_dimensions = 2;
    struct array* position_bounds  = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -0.5, 3,-0.5,3
                                    });
    size_t iterations_max = 10;
    size_t num_particles = 1;
    int seed = 1234;
    char* simulators_directory = "./data/simulators";
    int order = 2;
    size_t k = 5;
    char* dd_name = "name";
    char* dd_directory = "./directory";
    int num_workers = 4;

    struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, & num_particles, &seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers); 

    for (size_t i = 0; i < 100000; i++) {
        struct array* random_position = random_position_bound2_pso(pso);

        double x = *(double*)get_element_a(random_position, (size_t[]){0});
        double y = *(double*)get_element_a(random_position, (size_t[]){1});

        double x_bound = - y + 0.5;
        double y_bound = - x + 0.5;

        if (x < -0.5 || x > 3 || y < -0.5 || y > 3 || (x < x_bound && y < y_bound) || (x > -0.05 && x < 0.05) || (y > -0.05 && y < 0.05)) {
            printf("GOTCHA!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            print_a(random_position);
        }
        free_a(random_position);
    }

    free_pso(pso);
    free_a(position_bounds);
}


void test_start_pso() {
    enum AlgorithmType option = LEVENBERG_MARQUARDT;
    size_t num_dimensions = 2;
    struct array* position_bounds  = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        1, 20,1.001,20
                                    });
    size_t iterations_max = 50;
    size_t num_particles = 25;
    int seed = 49052764;
    char* simulators_directory = "./data/simulators";
    int order = 2;
    size_t k = 5;
    char* dd_name = "pso_data_lm_adaptive_order_2";
    char* dd_directory = "./bin/test";
    int num_workers = 4;

    struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, & num_particles, &seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers); 

    start_pso(pso);

    free_pso(pso);
    free_a(position_bounds);
}

void test_start_single_pso() {
    enum AlgorithmType option = STEFCAL_LAMBDA;
    size_t num_dimensions = 2;
    struct array* position_bounds  = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -0.4, 3,-0.4,3
                                    });
    size_t iterations_max = 2;
    size_t num_particles = 2;
    int seed = 49052764;
    char* simulators_directory = "./data/simulators";
    int order = 2;
    size_t k = 5;
    char* dd_name = "pso_data_stefcal_order_2";
    char* dd_directory = "./bin/test";
    int num_workers = 4;

    struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, & num_particles, &seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers); 

    int result = start_single_pso(pso);

    printf("RESULT = %d\n", result);

    free_pso(pso);
    free_a(position_bounds);
}

void test_start_pso_stefcal_lambda() {
    enum AlgorithmType option = STEFCAL_LAMBDA;
    size_t num_dimensions = 2;
    struct array* position_bounds  = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -0.5, 3,-0.5,3
                                    });
    size_t iterations_max = 50;
    size_t num_particles = 25;
    int seed = 49052764;
    char* simulators_directory = "./data/simulators";
    int order = 2;
    size_t k = 5;
    char* dd_name = "pso_data_stefcal_lambda_order_2";
    char* dd_directory = "./bin/test";
    int num_workers = 4;

    struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, & num_particles, &seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers); 

    start_pso(pso);

    free_pso(pso);
    free_a(position_bounds);
}

void test_start_pso_stefcal_improved() {
    enum AlgorithmType option = STEFCAL_IMPROVED;
    size_t num_dimensions = 2;
    struct array* position_bounds  = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -2.5, 2.5,-2.5,2.5
                                    });
    size_t iterations_max = 50;
    size_t num_particles = 25;
    int seed = 49052764;
    char* simulators_directory = "./data/simulators";
    int order = 2;
    size_t k = 5;
    char* dd_name = "pso_data_stefcal_improved_order_2";
    char* dd_directory = "./bin/test";
    int num_workers = 4;

    struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, & num_particles, &seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers); 

    start_pso(pso);

    free_pso(pso);
    free_a(position_bounds);
}


// Main function to run the tests
// int main(void) {
//     UNITY_BEGIN();
//     RUN_TEST(test_new_pso);
//     RUN_TEST(test_generate_random_number_pso);
//     RUN_TEST(test_random_position_pso);
//     RUN_TEST(test_random_position_bound1_pso);
//     RUN_TEST(test_random_position_bound2_pso);
//     RUN_TEST(test_start_pso_stefcal_lambda);
//     RUN_TEST(test_start_pso_stefcal_improved);
//     RUN_TEST(test_start_single_pso);
//     return UNITY_END();
// }   