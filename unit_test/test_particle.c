#include "unity.h"
#include "stopwatch.h"
#include "array.h"
#include "utils.h"
#include "pso.h"
#include "particle.h"

// void setUp(void) {}
// void tearDown(void) {}

void test_new_particle() {
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
    size_t k = 9;

    struct array* position  = new_set_a(&(size_t){1},
                                    (size_t[]){2}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[2]){
                                        1, 2
                                    });

    struct array* velocity  = new_set_a(&(size_t){1},
                                    (size_t[]){2}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[2]){
                                        0, 0
                                    });                  

    char* dd_name = "pso_data_stefcal_order_2";
    char* dd_directory = "./bin/test";
    int num_workers = 4;

    struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, & num_particles, &seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers); 

    struct Particle* particle = new_particle(pso, &num_dimensions, position, velocity);

    free_a(velocity);
    free_a(position);
    free_particle(particle);
    free_pso(pso);
    free_a(position_bounds);
}

void test_new_particle_w_c1_c2() {
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
    size_t k = 9;

    struct array* position  = new_set_a(&(size_t){1},
                                    (size_t[]){2}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[2]){
                                        1, 2
                                    });

    struct array* velocity  = new_set_a(&(size_t){1},
                                    (size_t[]){2}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[2]){
                                        0, 0
                                    });                  

    char* dd_name = "pso_data_stefcal_order_2";
    char* dd_directory = "./bin/test";
    int num_workers = 4;

    struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, & num_particles, &seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers); 

    struct Particle* particle = new_particle(pso, &num_dimensions, position, velocity);

    printf("w = %lf\n", *particle->w);
    printf("c1 = %lf\n", *particle->c1);
    printf("c2 = %lf\n", *particle->c2);

    free_a(velocity);
    free_a(position);
    free_particle(particle);
    free_pso(pso);
    free_a(position_bounds);
}

// Main function to run the tests
// int main(void) {
//     UNITY_BEGIN();
//     RUN_TEST(test_new_particle);
//     RUN_TEST(test_new_particle_w_c1_c2);
//     return UNITY_END();
// }   