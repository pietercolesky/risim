#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "simulator.h"
#include "utils.h"
#include "array.h"
#include "complex.h"
#include <time.h>
#include "stopwatch.h"
#include "calibrator.h"
#include <unistd.h>
#include "progress_bar.h"
#include "datacollector.h"
#include <math.h>
#include "data_dictionary.h"
#include <uuid/uuid.h>
#include "terminal.h"
#include "data_store.h"
#include "pso.h"
#include "particle.h"
#include <lapacke.h>
#include "constants.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif
#define SPEED_OF_LIGHT 299792458.0

int main(int argc, char *argv[]);
void run_calibration_test(char* directory);

/**
 * LOCAL FUNCTIONS
 */

void run_calibration_test(char* directory) {

    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    int order = 2;
    int seed = seeds[0];
    double snr = snrs[0];
    double max_amp = max_amps[0];
    int num_workers = 4;


    char filename [256];
    snprintf(filename, sizeof(filename), "%s/simulators/sim_seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf.sconfig", directory, seed, order, snr, max_amp);

    printf("%s\n", filename);
    struct Simulator* simulator = read_from_file_s(filename);

    size_t option = 0;
    struct Calibrator* calibrator = new_c(simulator);
    set_parallel_c(calibrator, &num_workers);
    struct Stopwatch* stopwatch = new_sw();

    double l1 = 1;
    double l2 = 1.1;


    if (option == 0) {
        solve_G_StEFCal(calibrator, &(size_t){IMAX}, &(double){TAU});
    } else if (option == 1) {
        solve_G_LM(calibrator, &(size_t){IMAX}, &(double){TAU}, &(double){l1}, &(double){l2});
    } else if (option == 2) {
        solve_G_LM_REDUCED_TIMESTEPS(calibrator, &(size_t){IMAX}, &(double){TAU}, &(double){l1}, &(double){l2});
    }
    print_sw(stopwatch);
    free_sw(stopwatch);
    

    double* average_iterations = get_average_iterations_dc(calibrator->datacollector);
    double* std_iterations = get_standard_deviation_iterations_dc(calibrator->datacollector);
    double* last_residual_norm = get_last_residuals_norm_dc(calibrator->datacollector);

    printf("Iterations Mean= %f\n", *average_iterations);
    printf("Iteration std = %lf\n", *std_iterations);
    printf("Residual Norm = %lf\n", *last_residual_norm);
    free(last_residual_norm);
    free(std_iterations);
    free(average_iterations);


    // printf("Iterations\n");s
    // print_a(get_iterations_dc(calibrator->datacollector));

    correct_visibilities(calibrator);
    plot_visibilities(simulator, &(size_t){0}, &(size_t){2});
    plot_corrected_visibilities_c(calibrator, &(size_t){0}, &(size_t){2});

    free_s(simulator);
    free_c(calibrator);
}

void create_simulators(char* directory, int* order) {
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;

    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    char s_filename[512];
    sprintf(s_filename,"%s/simulators", directory);
    
    for (int i = *order; i < *order + 1; i++) {
        for (size_t j = 0; j < 4; j++) {
            for (size_t h = 0; h < 4; h++) {
                for (size_t l = 0; l < 4; l++) {
                    int seed = seeds[j];
                    double snr = snrs[h];
                    double max_amp = max_amps[l];
                    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, order, &snr, &max_amp, &seed);
                    generate_antenna_layout(simulator);
                    uv_tracks(simulator);
                    create_point_sources(simulator, &(size_t){100},&(double){3.0}, &(double){2.0});
                    create_antenna_gains(simulator, &(double){5.0});
                    create_vis_mat(simulator, NULL, false);
                    create_vis_mat(simulator, NULL, true);
                    create_directories(s_filename);
                    save_to_file_s(simulator, s_filename);
                    free_s(simulator);
                }
            }
        }
    }
}


/**
 * HPC FUNCTIONS
 */

 void create_simulators_hpc(char* directory, int* order) {
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;

    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    for (int i = *order; i < *order + 1; i++) {
        for (size_t j = 0; j < 4; j++) {
            for (size_t h = 0; h < 4; h++) {
                for (size_t l = 0; l < 4; l++) {
                    int seed = seeds[j];
                    double snr = snrs[h];
                    double max_amp = max_amps[l];
                    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, order, &snr, &max_amp, &seed);
                    generate_antenna_layout(simulator);
                    uv_tracks(simulator);
                    create_point_sources(simulator, &(size_t){100},&(double){3.0}, &(double){2.0});
                    create_antenna_gains(simulator, &(double){5.0});
                    create_vis_mat(simulator, NULL, false);
                    create_vis_mat(simulator, NULL, true);
                    char s_directory[256];
                    sprintf(s_directory,"%s/simulators", directory);
                    create_directories(s_directory);
                    save_to_file_s(simulator, s_directory);
                    free_s(simulator);
                }
            }
        }
    }
}


int main(int argc, char *argv[]) {

    char* directory = "./simulation_environment";
    int order = 2;

    /**
     * LOCAL CALL
     */

     run_calibration_test(directory);

    /**
     * Create Simulators
     */
    // create_simulators(directory, &order);
    // create_simulators_hpc(&order);

    return 0;
}
