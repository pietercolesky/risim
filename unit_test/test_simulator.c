#include "unity.h"
#include "simulator.h"
#include "calibrator.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif
#define SPEED_OF_LIGHT 299792458.0

// void setUp(void) {}
// void tearDown(void) {}

void test_simulator_initialization_structure() {
    struct Simulator* simulator = newSimulatorStructure();

    TEST_ASSERT_NOT_NULL(simulator);
    free_s(simulator);
}

void test_simulator_initialize() {
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = HEX;
    size_t n_steps = 600;
    double bas_len = 50;
    int order = 1;
    int seed = 12345678;
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);

    TEST_ASSERT_NOT_NULL(simulator);
    TEST_ASSERT_EQUAL(*simulator->h_min, h_min);
    TEST_ASSERT_EQUAL(*simulator->h_max, h_max);
    TEST_ASSERT_EQUAL(*simulator->dec, dec);
    TEST_ASSERT_EQUAL(*simulator->lat, lat);
    TEST_ASSERT_EQUAL(*simulator->freq, freq);
    TEST_ASSERT_EQUAL(*simulator->layout, layout);
    TEST_ASSERT_EQUAL(*simulator->n_steps, n_steps);
    TEST_ASSERT_EQUAL(*simulator->bas_len, bas_len);
    TEST_ASSERT_EQUAL(*simulator->order, order);
    TEST_ASSERT_EQUAL(*simulator->seed, seed);
    TEST_ASSERT_EQUAL(*simulator->wave , SPEED_OF_LIGHT / (*simulator->freq));
    TEST_ASSERT_NOT_NULL(simulator->rng);

    struct array* range = linspace_a(&(double){h_min}, &(double){h_max} , &(size_t){n_steps} , &(enum DataType){DOUBLE});
    struct array* h_range = multiply_constant_a(range, &(double){M_PI/12}, &(enum DataType){DOUBLE});
    double* arr_h_range = get_a(h_range);
    double* arr_h_range_sim = get_a(simulator->h_range);
    size_t size = get_size_a(h_range);

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_h_range[i], arr_h_range_sim[i]);
    }

    free_s(simulator);
    free_a(range);
    free_a(h_range);
}

void test_initialize_simulator_default() {
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = HEX;
    size_t n_steps = 600;
    double bas_len = 50;
    int order = 1;
    int seed = 11;
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator_default();

    TEST_ASSERT_NOT_NULL(simulator);
    TEST_ASSERT_EQUAL(*simulator->h_min, h_min);
    TEST_ASSERT_EQUAL(*simulator->h_max, h_max);
    TEST_ASSERT_EQUAL(*simulator->dec, dec);
    TEST_ASSERT_EQUAL(*simulator->lat, lat);
    TEST_ASSERT_EQUAL(*simulator->freq, freq);
    TEST_ASSERT_EQUAL(*simulator->layout, layout);
    TEST_ASSERT_EQUAL(*simulator->n_steps, n_steps);
    TEST_ASSERT_EQUAL(*simulator->bas_len, bas_len);
    TEST_ASSERT_EQUAL(*simulator->order, order);
    TEST_ASSERT_EQUAL(*simulator->snr, snr);
    TEST_ASSERT_EQUAL(*simulator->max_amp, max_amp);
    TEST_ASSERT_EQUAL(*simulator->seed, seed);
    TEST_ASSERT_EQUAL(*simulator->wave , SPEED_OF_LIGHT / (*simulator->freq));
    TEST_ASSERT_NOT_NULL(simulator->rng);

    struct array* range = linspace_a(&(double){h_min}, &(double){h_max} , &(size_t){n_steps} , &(enum DataType){DOUBLE});
    struct array* h_range = multiply_constant_a(range, &(double){M_PI/12}, &(enum DataType){DOUBLE});
    double* arr_h_range = get_a(h_range);
    double* arr_h_range_sim = get_a(simulator->h_range);
    size_t size = get_size_a(h_range);

    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL(arr_h_range[i], arr_h_range_sim[i]);
    }

    free_s(simulator);
    free_a(range);
    free_a(h_range);
}

void test_generate_antenna_layout_hex() {
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = HEX;
    size_t n_steps = 600;
    double bas_len = 50;
    int order = 1;
    int seed = 12345678;
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    
    TEST_ASSERT_NOT_NULL(simulator->ant);

    double* arr = get_a(simulator->ant);
    size_t size = get_size_a(simulator->ant);
    double arr_test[] = {-25,-43.30127019,0,
                         25,-43.30127019,0,
                         -50,0,0,
                         0,0,0,
                         50,0,0,
                         -25,43.30127019,0,
                         25,43.30127019,0,};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL( arr_test[i],arr[i]);
    } 

    free_s(simulator);
}

void test_generate_antenna_layout_sqr() {
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
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);

    TEST_ASSERT_NOT_NULL(simulator->ant);

    double* arr = get_a(simulator->ant);
    size_t size = get_size_a(simulator->ant);
    double arr_test[] = {-25,-25,0,
                         25,-25,0,
                         -25,25,0,
                         25,25,0,};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL( arr_test[i],arr[i]);
    } 

    free_s(simulator);
}

void test_generate_antenna_layout_reg() {
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = REG;
    size_t n_steps = 600;
    double bas_len = 50;
    int order = 4;
    int seed = 12345678;
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);

    TEST_ASSERT_NOT_NULL(simulator->ant);

    double* arr = get_a(simulator->ant);
    size_t size = get_size_a(simulator->ant);
    double arr_test[] = {0,0,0,
                         50,0,0,
                         100,0,0,
                         150,0,0,};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL( arr_test[i],arr[i]);
    } 

    free_s(simulator);
}

void test_plot_ant() {
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = REG;
    size_t n_steps = 600;
    double bas_len = 50;
    int order = 4;
    int seed = 12345678;
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    plot_ant(simulator);

    TEST_ASSERT_NOT_NULL(simulator->ant);

    free_s(simulator);
}

void test_uv_tracks() {
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
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    uv_tracks(simulator);
    
    double* arr = get_a(simulator->u_m);
    size_t size = get_size_a(simulator->u_m);
    double arr_test[] = {0,0,
                         24.14814566,24.14814566,
                         -3.30554165,3.30554165,
                         20.84260401,27.45368731,
                         -24.14814566,-24.14814566,
                         0,0,
                         -27.45368731,-20.84260401,
                         -3.30554165,3.30554165,
                         3.30554165,-3.30554165,
                         27.45368731,20.84260401,
                         0,0,
                         24.14814566,24.14814566,
                         -20.84260401,-27.45368731,
                         3.30554165,-3.30554165,
                         -24.14814566,-24.14814566,
                         0,0,};
    for (size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL( arr_test[i],arr[i]);
    } 

    double* arr1 = get_a(simulator->v_m);
    size_t size1 = get_size_a(simulator->v_m);
    double arr_test1[] = {
                        0, 0,
                        6.23996454, -6.23996454,
                        17.58231644, 17.58231644,
                        23.82228098, 11.3423519,
                        -6.23996454, 6.23996454,
                        0, 0,
                        11.3423519, 23.82228098,
                        17.58231644, 17.58231644,
                        -17.58231644, -17.58231644,
                        -11.3423519, -23.82228098,
                        0, 0,
                        6.23996454, -6.23996454,
                        -23.82228098, -11.3423519,
                        -17.58231644, -17.58231644,
                        -6.23996454, 6.23996454,
                        0, 0
                    };
    for (size_t i = 0; i < size1; i++) {
        TEST_ASSERT_EQUAL( arr_test1[i],arr1[i]);
    } 

    double* arr2 = get_a(simulator->w_m);
    size_t size2 = get_size_a(simulator->w_m);
    double arr_test2[] = {
    0, 0,
    1.71169619, -1.71169619,
    -17.4624037, -17.4624037,
    -15.7507075, -19.17409989,
    -1.71169619, 1.71169619,
    0, 0,
    -19.17409989, -15.7507075,
    -17.4624037, -17.4624037,
    17.4624037, 17.4624037,
    19.17409989, 15.7507075,
    0, 0,
    1.71169619, -1.71169619,
    15.7507075, 19.17409989,
    17.4624037, 17.4624037,
    -1.71169619, 1.71169619,
    0, 0
};
    for (size_t i = 0; i < size2; i++) {
        TEST_ASSERT_EQUAL( arr_test2[i],arr2[i]);
    } 

    free_s(simulator);
}

void test_plot_uv_coverage() {
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
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    plot_ant(simulator);
    uv_tracks(simulator);
    plot_uv_coverage(simulator);

    TEST_ASSERT_NOT_NULL(simulator->u_m);
    TEST_ASSERT_NOT_NULL(simulator->v_m);
    TEST_ASSERT_NOT_NULL(simulator->w_m);

    free_s(simulator);
}

void test_create_point_sources() {
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
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    uv_tracks(simulator);
    create_point_sources(simulator, &(size_t){100},&(double){3.0}, &(double){2.0});

    TEST_ASSERT_EQUAL(100*3, get_size_a(simulator->point_sources));

    free_s(simulator);
}

void test_create_antenna_gains() {
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
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    uv_tracks(simulator);
    create_point_sources(simulator, &(size_t){100},&(double){3.0}, &(double){2.0});
    create_antenna_gains(simulator,&(double){5.0});

    TEST_ASSERT_NOT_NULL(simulator->antenna_gains);

    free_s(simulator);
}

void test_create_vis_mat_no_error() {
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
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    uv_tracks(simulator);
    create_point_sources(simulator, &(size_t){100},&(double){3.0}, &(double){2.0});
    create_antenna_gains(simulator,&(double){5.0});
    create_vis_mat(simulator, NULL, NULL);

    TEST_ASSERT_NOT_NULL(simulator->M);

    free_s(simulator);
}

void test_det_power_of_signal() {
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
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    uv_tracks(simulator);
    create_point_sources(simulator, &(size_t){100},&(double){3.0}, &(double){2.0});
    create_antenna_gains(simulator,&(double){5.0});
    create_vis_mat(simulator, NULL, false);

    double* P_signal = det_power_of_signal(simulator->M);
    double check = *P_signal;

    TEST_ASSERT_EQUAL(752.876595, check);
    // TEST_ASSERT_NOT_NULL(simulator->M);

    free_s(simulator);
    free(P_signal);
}

void test_create_vis_mat_with_error() {
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
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    uv_tracks(simulator);
    create_point_sources(simulator, &(size_t){100},&(double){3.0}, &(double){2.0});
    create_antenna_gains(simulator, &(double){5.0});
    create_vis_mat(simulator, NULL, true);

    TEST_ASSERT_NOT_NULL(simulator->D);

    free_s(simulator);
}

void test_plot_visibilities() {
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
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    uv_tracks(simulator);
    create_point_sources(simulator, &(size_t){100},&(double){3.0}, &(double){2.0});
    create_antenna_gains(simulator, &(double){5.0});
    create_vis_mat(simulator, NULL, false);
    create_vis_mat(simulator, NULL, true);
    plot_visibilities(simulator,&(size_t){0},&(size_t){2});

    TEST_ASSERT_NOT_NULL(simulator->D);

    free_s(simulator);
}

void test_save_to_file_s(){
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
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    uv_tracks(simulator);
    create_point_sources(simulator, &(size_t){100},&(double){3.0}, &(double){2.0});
    create_antenna_gains(simulator, &(double){5.0});
    create_vis_mat(simulator, NULL, false);
    create_vis_mat(simulator, NULL, true);

    create_directory("./simulators");
    save_to_file_s(simulator, "./simulators");

    free_s(simulator);
}

void test_read_from_file_s() {
    struct Simulator* simulator = read_from_file_s("./simulators/sim_seed_12345678_order_2_snr_1000_maxamp_2.sconfig");
    free_s(simulator);
}

void test_read_and_run() {
    struct Simulator* simulator = read_from_file_s("./simulators/sim_seed_12345678_order_2_snr_1000_maxamp_2.sconfig");

    struct Calibrator* calibrator = new_c(simulator);
    double l1 = 0;
    double l2 = 1;

    solve_G_StEFCal_Lambda(calibrator, &(size_t){1000}, &(double){0.00001}, &(double){l1}, &(double){l2});
    

    double* average_iterations = get_average_iterations_dc(calibrator->datacollector);
    printf("Average Iterations = %f\n", *average_iterations);
    free(average_iterations);

    correct_visibilities(calibrator);
    plot_visibilities(simulator, &(size_t){0}, &(size_t){1});
    plot_corrected_visibilities_c(calibrator, &(size_t){0}, &(size_t){1});

    free_s(simulator);
    free_c(calibrator);
}

// Main function to run the tests
// int main(void) {
//     UNITY_BEGIN();
//     // RUN_TEST(test_simulator_initialization_structure);
//     // RUN_TEST(test_simulator_initialize);
//     // RUN_TEST(test_initialize_simulator_default);
//     // RUN_TEST(test_generate_antenna_layout_hex);
//     // RUN_TEST(test_generate_antenna_layout_sqr);
//     // RUN_TEST(test_generate_antenna_layout_reg);
//     // RUN_TEST(test_plot_ant);
//     // RUN_TEST(test_uv_tracks);
//     // RUN_TEST(test_plot_uv_coverage);
//     // RUN_TEST(test_create_point_sources);
//     // RUN_TEST(test_create_antenna_gains);
//     // RUN_TEST(test_create_vis_mat_no_error);
//     // RUN_TEST(test_det_power_of_signal);
//     // RUN_TEST(test_create_vis_mat_with_error);
//     // RUN_TEST(test_plot_visibilities);
//     // RUN_TEST(test_save_to_file_s);
//     // RUN_TEST(test_read_from_file_s);
//     RUN_TEST(test_read_and_run);
//     return UNITY_END();
// }   