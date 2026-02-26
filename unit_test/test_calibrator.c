#include "unity.h"
#include "calibrator.h"
#include "simulator.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif
#define SPEED_OF_LIGHT 299792458.0

void setUp(void) {}
void tearDown(void) {}

void test_new_c() {
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
    create_vis_mat(simulator, NULL, true);


    struct Calibrator* calibrator = new_c(simulator);

    TEST_ASSERT_NOT_NULL(calibrator);

    free_s(simulator);
    free_c(calibrator);
}

void test_solve_G_LM_Diag_Hessian_Lambda() {
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
    create_vis_mat(simulator, NULL, true);


    struct Calibrator* calibrator = new_c(simulator);

    solve_G_LM_Diag_Hessian_Lambda(calibrator, &(size_t){1}, &(double){0.00001}, &(double){0}, &(double){1});

    free_s(simulator);
    free_c(calibrator);
}

void test_solve_G_StEFCal() {
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
    create_vis_mat(simulator, NULL, true);


    struct Calibrator* calibrator = new_c(simulator);

    solve_G_StEFCal(calibrator, &(size_t){1000}, &(double){0.00001});

    free_s(simulator);
    free_c(calibrator);
}

void test_correct_visibilities() {
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
    create_vis_mat(simulator, NULL, true);


    struct Calibrator* calibrator = new_c(simulator);

    solve_G_StEFCal(calibrator, &(size_t){1}, &(double){0.00001});
    correct_visibilities(calibrator);

    free_s(simulator);
    free_c(calibrator);
}

void test_solve_G_LM_Diag_Hessian_Lambda_parallel() {
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
    int num_workers = 3;
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    uv_tracks(simulator);
    create_point_sources(simulator, &(size_t){100},&(double){3.0}, &(double){2.0});
    create_antenna_gains(simulator,&(double){5.0});
    create_vis_mat(simulator, NULL, false);
    create_vis_mat(simulator, NULL, true);


    struct Calibrator* calibrator = new_c(simulator);
    set_parallel_c(calibrator, &(int){num_workers});

    solve_G_LM_Diag_Hessian_Lambda(calibrator, &(size_t){1}, &(double){0.00001}, &(double){0}, &(double){1});

    free_s(simulator);
    free_c(calibrator);
}

void test_solve_G_StEFCal_parallel() {
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
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    uv_tracks(simulator);
    create_point_sources(simulator, &(size_t){100},&(double){3.0}, &(double){2.0});
    create_antenna_gains(simulator,&(double){5.0});
    create_vis_mat(simulator, NULL, false);
    create_vis_mat(simulator, NULL, true);


    struct Calibrator* calibrator = new_c(simulator);
    set_parallel_c(calibrator, &(int){num_workers});

    solve_G_StEFCal(calibrator, &(size_t){1000}, &(double){0.00001});

    free_s(simulator);
    free_c(calibrator);
}

void test_solve_G_StEFCal_Lambda() {
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
    create_vis_mat(simulator, NULL, true);


    struct Calibrator* calibrator = new_c(simulator);

    solve_G_StEFCal_Lambda(calibrator, &(size_t){10000}, &(double){0.00001}, &(double){0}, &(double){1});

    // print_a(calibrator->datacollector->damping_g[0]);

    free_s(simulator);
    free_c(calibrator);
}

void test_solve_G_StEFCal_Lambda_parallel() {
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
    double snr = 1000;
    double max_amp = 2;

    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);
    generate_antenna_layout(simulator);
    uv_tracks(simulator);
    create_point_sources(simulator, &(size_t){100},&(double){3.0}, &(double){2.0});
    create_antenna_gains(simulator,&(double){5.0});
    create_vis_mat(simulator, NULL, false);
    create_vis_mat(simulator, NULL, true);


    struct Calibrator* calibrator = new_c(simulator);
    set_parallel_c(calibrator, &(int){num_workers});

    solve_G_StEFCal_Lambda(calibrator, &(size_t){1000}, &(double){0.00001}, &(double){0}, &(double){1});

    // print_a(calibrator->datacollector->damping_g[0]);

    free_s(simulator);
    free_c(calibrator);
}

void test_solve_G_LM_REDUCED_TIMESTEPS() {
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
    create_vis_mat(simulator, NULL, true);


    struct Calibrator* calibrator = new_c(simulator);

    solve_G_LM_REDUCED_TIMESTEPS(calibrator, &(size_t){1}, &(double){0.00001}, &(double){0}, &(double){1});

    free_s(simulator);
    free_c(calibrator);
}

//Main function to run the tests
int main(void) {
    UNITY_BEGIN();
    // RUN_TEST(test_new_c);
    // RUN_TEST(test_solve_G_LM_Diag_Hessian_Lambda);
    // RUN_TEST(test_solve_G_StEFCal);
    // RUN_TEST(test_correct_visibilities);
    // RUN_TEST(test_solve_G_LM_Diag_Hessian_Lambda_parallel);
    // RUN_TEST(test_solve_G_StEFCal_parallel);
    // RUN_TEST(test_solve_G_StEFCal_Lambda);
    // RUN_TEST(test_solve_G_StEFCal_Lambda_parallel);
    RUN_TEST(test_solve_G_LM_REDUCED_TIMESTEPS);
    return UNITY_END();
}   

