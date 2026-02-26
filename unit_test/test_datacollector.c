#include "unity.h"
#include "datacollector.h"
#include "calibrator.h"
#include "simulator.h"
#include "jagged_array.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif
#define SPEED_OF_LIGHT 299792458.0

// void setUp(void) {}
// void tearDown(void) {}

void test_new_dc() {
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


    struct Calibrator* calibrator = new_c(simulator);

    struct DataCollector* datacollector = new_dc(calibrator);

    free_dc(datacollector);
    free_s(simulator);
    free_c(calibrator);
    
}

void test_add_iteration_dc() {
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


    struct Calibrator* calibrator = new_c(simulator);

    struct DataCollector* datacollector = new_dc(calibrator);

    add_iteration_dc(datacollector, &(size_t){0}, &(int){0.0 + 1});

    free_dc(datacollector);
    free_s(simulator);
    free_c(calibrator);
}

void test_add_gain_dc() {
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


    struct Calibrator* calibrator = new_c(simulator);

    struct DataCollector* datacollector = new_dc(calibrator);

    struct array* array = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){DOUBLE});
    add_gain_dc(datacollector, &(size_t){0}, array);
    free_a(array);

    free_dc(datacollector);
    free_s(simulator);
    free_c(calibrator);
}

void test_add_all_gain_dc() {
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


    struct Calibrator* calibrator = new_c(simulator);

    struct DataCollector* datacollector = new_dc(calibrator);

    struct array* array = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){DOUBLE});
    add_all_gain_dc(datacollector, &(size_t){0}, &(size_t){0}, array);
    free_a(array);

    free_dc(datacollector);
    free_s(simulator);
    free_c(calibrator);
}

void test_add_damping_g_dc() {
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


    struct Calibrator* calibrator = new_c(simulator);

    struct DataCollector* datacollector = new_dc(calibrator);

    struct array* array = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){DOUBLE});
    add_damping_g_dc(datacollector, &(size_t){0}, array);
    free_a(array);

    free_dc(datacollector);
    free_s(simulator);
    free_c(calibrator);
}

void test_add_damping_g_old_dc() {
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


    struct Calibrator* calibrator = new_c(simulator);

    struct DataCollector* datacollector = new_dc(calibrator);

    struct array* array = zeros_a(&(size_t){1}, (size_t[]){1}, &(enum DataType){DOUBLE});
    add_damping_g_old_dc(datacollector, &(size_t){0}, array);
    free_a(array);

    free_dc(datacollector);
    free_s(simulator);
    free_c(calibrator);
}

void test_get_all_gain_dc() {
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

    struct Calibrator* calibrator = new_c(simulator);

    solve_G_StEFCal_Lambda(calibrator, &(size_t){1000}, &(double){0.00001}, &(double){0}, &(double){1});

    struct Jaggedarray* resized_all_gains = get_all_gain_dc(calibrator->datacollector);

    print_ja(resized_all_gains);

    free_ja(resized_all_gains);

    free_s(simulator);
    free_c(calibrator);
}

//Main function to run the tests
// int main(void) {
//     UNITY_BEGIN();
//     // RUN_TEST(test_new_dc);
//     // RUN_TEST(test_add_iteration_dc);
//     // RUN_TEST(test_add_gain_dc);
//     // RUN_TEST(test_add_all_gain_dc);
//     // RUN_TEST(test_add_damping_g_dc);
//     // RUN_TEST(test_add_damping_g_old_dc);
//     RUN_TEST(test_get_all_gain_dc);
//     return UNITY_END();
// }   