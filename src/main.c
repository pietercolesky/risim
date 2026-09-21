/**
 * @file main.c
 * @description Command-line front end for RISim.
 *
 * The commands below are the pipeline used to produce the results in
 * "Hyperparameter Optimization of Calibration Solvers" (Colesky, Grobler &
 * Kleynhans). They are meant to be run in stages, and each stage reads what the
 * previous one wrote underneath the data directory:
 *
 *   1. simulate         -- generate the visibility datasets (.sconfig)
 *   2. dict-grid/-pso   -- create the data-dictionary configs for a solver
 *   3. grid-*           -- grid search over the hyperparameter plane (heatmaps)
 *   4. pso-*            -- particle-swarm search for the optimal hyperparameters
 *   5. best-*           -- fix one hyperparameter before the second search round
 *   6. heatmap-*,
 *      pso-*, perf-*,
 *      points-*, pbest-* -- write the .txt/.csv the plotting scripts read
 *
 * Run ./risim with no arguments for the full command list.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <complex.h>
#include <uuid/uuid.h>
#include <lapacke.h>

#include "logger.h"
#include "simulator.h"
#include "utils.h"
#include "array.h"
#include "stopwatch.h"
#include "calibrator.h"
#include "progress_bar.h"
#include "datacollector.h"
#include "data_dictionary.h"
#include "terminal.h"
#include "data_store.h"
#include "pso.h"
#include "particle.h"
#include "constants.h"

/** Data directory used when -d is not given. */
#define DEFAULT_DIRECTORY "./simulation_environment"

/**
 * Scale overrides, set by -w, -g, -i and -p.
 *
 * Zero means "use the value this experiment was published with", so leaving
 * them alone reproduces the paper exactly. They exist so that the pipeline can
 * also be run at a size that finishes on a workstation: the published grid is
 * 500x500 points over 64 datasets per antenna order, and the published worker
 * counts assume an HPC node.
 */
static int override_workers = 0;
static size_t override_grid_length = 0;
static size_t override_pso_iterations = 0;
static size_t override_pso_particles = 0;

static int workers(int published) {
    return override_workers > 0 ? override_workers : published;
}

static size_t grid_points(size_t published) {
    return override_grid_length > 0 ? override_grid_length : published;
}

static size_t pso_iterations(size_t published) {
    return override_pso_iterations > 0 ? override_pso_iterations : published;
}

static size_t pso_particles(size_t published) {
    return override_pso_particles > 0 ? override_pso_particles : published;
}


/**
 * Open a file for writing, creating the directory it lives in first.
 *
 * The extraction stage writes into directories that only come into existence
 * once an earlier stage has run, so each output file makes sure of its own
 * parent rather than relying on the run order.
 */
static FILE* open_output(const char* path, const char* mode) {
    const char* slash = strrchr(path, '/');
    if (slash != NULL) {
        size_t len = (size_t)(slash - path);
        char parent[1024];
        if (len > 0 && len < sizeof(parent)) {
            memcpy(parent, path, len);
            parent[len] = '\0';
            create_directories(parent);
        }
    }
    return fopen(path, mode);
}


/**
 * Directory the per-particle best arrays are written into.
 *
 * These used to be saved to the working directory, so where the output landed
 * depended on where risim happened to be started from. They now sit under the
 * data directory with everything else.
 */
static char* p_best_dir(char* directory) {
    static char path[512];
    snprintf(path, sizeof(path), "%s/extracted_p_best", directory);
    create_directories(path);
    return path;
}

int main(int argc, char* argv[]);

void mapping_local(int* order, char* directory, char* filename, enum AlgorithmType* algorithm);
void create_simulators(int* order, char* directory);
void create_data_dictionary_extracted_data(char* directory, char* filename);
void extract_heatmap_data(char* directory, char* filename, int* order);
void extract_heatmap_data_orders(char* directory, char* filename, int* order_min, int* order_max);
void pso_extract_trace_specific_all(char* directory, char* filename, int* order_from, int* order_to);
void pso_extract_all_best_hyperparameters(char* directory, char* filename, int* order_from, int* order_to);
void mapping_stochastic_adaptive_local(char* directory, int* order_min, int* order_max);
void mapping_pso_stochastic_adaptive_local(char* directory, int* order_min, int* order_max);
void mapping_pso_stochastic_l1l2_local(char* directory, int* order_min, int* order_max);
void mapping_pso_stochastic_fixed_b_local(char* directory, int* order_min, int* order_max);
void extraction_optimal_stefcal(char* directory, size_t* order_min, size_t* order_max);
void extraction_0_1_stefcal(char* directory, size_t* order_min, size_t* order_max);
void extraction_optimal_stefcal_improved(char* directory, size_t* order_min, size_t* order_max);
void extraction_original_stefcal_improved(char* directory, size_t* order_min, size_t* order_max);
void extraction_optimal_lm_non_adaptive_l1_l2(char* directory, size_t* order_min, size_t* order_max);
void extraction_optimal_lm_non_adaptive_l1_l2_REDUCED_TIMESTEPS(char* directory, size_t* order_min, size_t* order_max);
void extraction_original_lm_non_adaptive_l1_l2(char* directory, size_t* order_min, size_t* order_max);
void extraction_optimal_lm_adaptive(size_t* order_min, size_t* order_max, char* directory);
void extraction_optimal_lm_adaptive_REDUCED_TIMESTEPS(size_t* order_min, size_t* order_max, char* directory);
void extraction_original_lm_adaptive(char* directory, size_t* order_min, size_t* order_max);
void create_x_y_points_from_optimal_stefcal(char* directory, size_t* order_min, size_t* order_max);
void create_x_y_points_from_original_stefcal(char* directory, size_t* order_min, size_t* order_max);
void create_x_y_points_from_optimal_stefcal_improved(char* directory, size_t* order_min, size_t* order_max);
void create_x_y_points_from_original_stefcal_improved(char* directory, size_t* order_min, size_t* order_max);
void create_x_y_points_from_optimal_lm_non_adaptive_l1_l2(char* directory, size_t* order_min, size_t* order_max);
void create_x_y_points_from_original_lm_non_adaptive_l1_l2(char* directory, size_t* order_min, size_t* order_max);
void create_x_y_points_from_optimal_lm_adaptive(char* directory, size_t* order_min, size_t* order_max);
void create_x_y_points_from_original_lm_adaptive(char* directory, size_t* order_min, size_t* order_max);
void mapping_pso_lm_adaptive_local(char* directory, int* order_min, int* order_max);
void create_data_dictionary_pso_orders(char* directory, char* filename, int* order_min, int* order_max);
void create_data_dictionary_orders(char* directory, char* filename, int* order_min, int* order_max);
void mapping_pso_local(char* directory, char* filename, enum AlgorithmType* algorithm, int* iteration, int* order_min, int* order_max);
void pso_extract_all_best_hyperparameters_save_to_jarray(char* directory, char* filename, int* order_from, int* order_to);
void extract_all_iterations_original_from_data_optimal_solutions(char* directory, char* filename, int* order_min, int* order_max);
void extract_all_iterations_optimal_from_data_optimal_solutions(char* directory, char* filename, int* order_min, int* order_max);
void mapping_adam_fixed_alpha_local(char* directory, int* order_min, int* order_max);
void mapping_stochastic_fixed_b_local(char* directory, int* order_min, int* order_max);
void mapping_pso_adam_fixed_alpha_local(char* directory, int* order_min, int* order_max);
void extract_best_alpha_adam(char* directory, size_t* order_min, size_t* order_max);
void extract_all_best_b1_b2_alpha_adam_3d(char* directory, size_t* order_min, size_t* order_max);
void extract_all_best_b1_b2_alpha_adam(char* directory, size_t* order_min, size_t* order_max);
void extract_best_b_stochastic(char* directory, size_t* order_min, size_t* order_max);
void extract_all_best_l1_l2_b_stochastic(char* directory, size_t* order_min, size_t* order_max);
void extract_normal_p_best_adam_fixed_alpha(char* directory, int* order_min, int* order_max);
void extract_normal_p_best_stefcal(char* directory, int* order_min, int* order_max);
void extract_normal_p_best_lm_non_adaptive_l1_l2(char* directory, int* order_min, int* order_max);
void extract_normal_p_best_stefcal_improved(char* directory, int* order_min, int* order_max);
void extract_pso_p_best_adam_3d_b1_b2(char* directory, int* order_min, int* order_max);
void extract_pso_p_best_adam_fixed_alpha(char* directory, int* order_min, int* order_max);
void extract_normal_p_best_stochastic_fixed_b(char* directory, int* order_min, int* order_max);
void extract_pso_p_best_stochastic_fixed_b(char* directory, int* order_min, int* order_max);
void extraction_optimal_adam_3d(char* directory, size_t* order_min, size_t* order_max);
void extraction_optimal_adam_fixed_alpha(char* directory, size_t* order_min, size_t* order_max);
void extraction_original_adam_fixed_alpha(char* directory, size_t* order_min, size_t* order_max);
void extraction_optimal_stochastic(char* directory, size_t* order_min, size_t* order_max);
void extraction_original_stochastic(char* directory, size_t* order_min, size_t* order_max);
static void create_x_y_points_adam_fixed_alpha_variant(char* directory, size_t* order_min, size_t* order_max, const char* variant);
void create_x_y_points_from_optimal_adam_fixed_alpha(char* directory, size_t* order_min, size_t* order_max);
void create_x_y_points_from_original_adam_fixed_alpha(char* directory, size_t* order_min, size_t* order_max);
static void create_x_y_points_stochastic_variant(char* directory, size_t* order_min, size_t* order_max, const char* variant);
void create_x_y_points_from_optimal_stochastic(char* directory, size_t* order_min, size_t* order_max);
void create_x_y_points_from_original_stochastic(char* directory, size_t* order_min, size_t* order_max);
void extraction_optimal_stochastic_adaptive(char* directory, size_t* order_min, size_t* order_max);
void extraction_original_stochastic_adaptive(char* directory, size_t* order_min, size_t* order_max);
static int file_exists_er(const char* path);
static int load_best_lm_adaptive(char* directory, size_t order, int data_seed, double snr, double max_amp, double* out_l_init, double* out_v);
static int load_best_stefcal(char* directory, size_t order, int data_seed, double snr, double max_amp, double* out_l1, double* out_l2);
void compare_stefcal_v2(char* directory);
void extended_research_sas_dostefcal_v2(char* directory, size_t* order_min, size_t* order_max);
void extended_research_sas_stefcal(char* directory, size_t* order_min, size_t* order_max);
static int load_best_l1_l2_pso_family(char* directory, const char* family, size_t order, int data_seed, double snr, double max_amp, double* out_l1, double* out_l2);
static int load_best_l1_l2_stochastic(char* directory, size_t order, int data_seed, double snr, double max_amp, double* out_l1, double* out_l2, const char** out_family);
static int load_best_b0_stochastic(char* directory, size_t order, int data_seed, double snr, double max_amp, size_t* out_b0);
static void raw_sas_solve_and_save(char* directory, const char* variant, const char* pso_family, size_t order, int data_seed, double snr, double max_amp, double l1, double l2, size_t b0, FILE* csv_summary, FILE* csv_trace);
void extraction_raw_iterations_sas(char* directory, size_t* order_min, size_t* order_max);
static void create_x_y_points_stochastic_adaptive_variant(char* directory, size_t* order_min, size_t* order_max, const char* variant);
void create_x_y_points_from_optimal_stochastic_adaptive(char* directory, size_t* order_min, size_t* order_max);
void create_x_y_points_from_original_stochastic_adaptive(char* directory, size_t* order_min, size_t* order_max);


void mapping_local(int* order, char* directory, char* filename, enum AlgorithmType* algorithm) {
    char dd_directory[256];
    sprintf(dd_directory,"%s/data/data_order_%d",directory, *order);
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;
    int num_workers = workers(2);
    size_t grid_length = grid_points(500);
    enum AlgorithmType algorithm_type = *algorithm;
    size_t amount_hyperparameters = 2;
    struct array* hyperparameters_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -10,10,-10,10
                                    });

    char dd_filename[256];
    sprintf(dd_filename,"data_%s_order_%d",filename, *order);

    struct DataDictionary* data_dictionary = read_from_file_dd(dd_directory, dd_filename);

    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    size_t counter = 0;

    for (size_t h = 0; h < 4; h++) {
        for (size_t i = 0; i < 4; i++) {
            for (size_t j = 0; j < 4; j++) {
                if (counter == *data_dictionary->amount_items) {
                    struct DataStore* data_store = new_data_store_update_dd(&data_dictionary, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, order, &(double){snrs[i]}, &(double){max_amps[j]}, &(int){seeds[h]}, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds);
                    start_data_store_dd(data_store);
                    free_ds(data_store);
                } else {
                    counter += 1;
                }
            }
        }
    }

    free_dd(data_dictionary);
    free_a(hyperparameters_bounds);
    return;
}

void create_simulators(int* order, char* directory) {
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

    char dd_filename[512];
    sprintf(dd_filename,"%s/simulators", directory);
    
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

                    create_directories(dd_filename);
                    save_to_file_s(simulator, dd_filename);

                    free_s(simulator);
                }
            }
        }
    }
}

/**
 * Local New
 */
void create_data_dictionary_extracted_data(char* directory, char* filename) {
    char dd_directory[256];
    sprintf(dd_directory,"%s/extracted_data",directory);
    char dd_filename[256];
    sprintf(dd_filename,"data_extract_%s",filename);
    struct DataDictionary* data_dictionary = new_save_dd(dd_filename, dd_directory);

    free_dd(data_dictionary);
}

void extract_heatmap_data(char* directory, char* filename, int* order) {
    char dd_name[256];
    sprintf(dd_name, "data_%s_order_%d",filename, *order);
    char dd_directory[256];
    sprintf(dd_directory, "%s/data/data_order_%d",directory, *order);

    struct DataDictionary* data_dictionary = read_from_file_dd(dd_directory, dd_name);

    struct DataStore** data_stores = malloc(*data_dictionary->amount_items * sizeof(struct DataStore*));
    for (size_t i = 0; i < *data_dictionary->amount_items; i++) {
        char filepath_ds[512];
        sprintf(filepath_ds, "%s/%s", data_dictionary->directory, data_dictionary->identity);
        data_stores[i] = read_structure_ds(filepath_ds, data_dictionary->items[i]);
    }
    printf("Done Reading\n");

    for (size_t i = 0; i < *data_dictionary->amount_items; i++) {
        printf("%s\n", data_stores[i]->identity);

        char save_directory[256];
        sprintf(save_directory, "%s/extracted_heatmap_data/data_order_%d/data_%s_order_%d/seed_%d_order_%d_snr_%lf_max_amp_%lf",directory, *order, filename, *order, *data_stores[i]->seed, *data_stores[i]->order, *data_stores[i]->snr, *data_stores[i]->max_amp);
        create_directories(save_directory);
        extract_heatmap_data_ds(data_stores[i], save_directory, directory);
    }


    for (size_t i = 0; i < *data_dictionary->amount_items; i++) {
        free_ds(data_stores[i]);
    }
    free(data_stores);
    free_dd(data_dictionary);
}

void extract_heatmap_data_orders(char* directory, char* filename, int* order_min, int* order_max) {

    for (int order = *order_min; order <= *order_max; order++) {
        char dd_name[256];
        sprintf(dd_name, "data_%s_order_%d",filename, order);
        char dd_directory[256];
        sprintf(dd_directory, "%s/data/data_order_%d",directory, order);

        struct DataDictionary* data_dictionary = read_from_file_dd(dd_directory, dd_name);

        struct DataStore** data_stores = malloc(*data_dictionary->amount_items * sizeof(struct DataStore*));
        for (size_t i = 0; i < *data_dictionary->amount_items; i++) {
            char filepath_ds[512];
            sprintf(filepath_ds, "%s/%s", data_dictionary->directory, data_dictionary->identity);

            // Skip datastores still being written (mapping not finished): a complete
            // datastore has a non-empty iterations.dcconfig.
            char iter_check[640];
            sprintf(iter_check, "%s/%s/iterations.dcconfig", filepath_ds, data_dictionary->items[i]);
            FILE *icheck = fopen(iter_check, "r");
            long isize = -1;
            if (icheck) { fseek(icheck, 0, SEEK_END); isize = ftell(icheck); fclose(icheck); }
            if (isize <= 0) {
                printf("Skipping incomplete datastore: %s\n", data_dictionary->items[i]);
                data_stores[i] = NULL;
                continue;
            }

            data_stores[i] = read_structure_ds(filepath_ds, data_dictionary->items[i]);
        }

        for (size_t i = 0; i < *data_dictionary->amount_items; i++) {
            if (data_stores[i] == NULL) continue;
            printf("%s\n", data_stores[i]->identity);

            char save_directory[256];
            sprintf(save_directory, "%s/extracted_heatmap_data/data_order_%d/data_%s_order_%d/seed_%d_order_%d_snr_%lf_max_amp_%lf",directory, order, filename, order, *data_stores[i]->seed, *data_stores[i]->order, *data_stores[i]->snr, *data_stores[i]->max_amp);
            create_directories(save_directory);
            extract_heatmap_data_ds(data_stores[i], save_directory, directory);
        }


        for (size_t i = 0; i < *data_dictionary->amount_items; i++) {
            if (data_stores[i] != NULL) free_ds(data_stores[i]);
        }
        free(data_stores);
        free_dd(data_dictionary);
    }
}

void pso_extract_trace_specific_all(char* directory, char* filename, int* order_from, int* order_to) {

    char ellipses_points_filename[256];
    sprintf(ellipses_points_filename, "%s/extracted_trace_data/extracted_trace_pso_%s/ellipses_points.txt", directory, filename);
    FILE *file_ellipses_points = open_output(ellipses_points_filename, "w");
    if (file_ellipses_points == NULL) {
        perror("Error opening commands file");
        exit(EXIT_FAILURE);
    }

    char violin_p_best_std_filename[256];
    sprintf(violin_p_best_std_filename, "%s/extracted_trace_data/extracted_trace_pso_%s/violin_p_best_std.txt", directory, filename);
    FILE *file_txt_violin_p_best_std = open_output(violin_p_best_std_filename, "w");
    if (file_txt_violin_p_best_std == NULL) {
        perror("Error opening commands file");
        exit(EXIT_FAILURE);
    }

    char violin_p_best_mean_filename[256];
    sprintf(violin_p_best_mean_filename, "%s/extracted_trace_data/extracted_trace_pso_%s/violin_p_best_mean.txt", directory, filename);
    FILE *file_txt_violin_p_best_mean = open_output(violin_p_best_mean_filename, "w");
    if (file_txt_violin_p_best_mean == NULL) {
        perror("Error opening commands file");
        exit(EXIT_FAILURE);
    }

    char violin_p_best_residual_norm_filename[256];
    sprintf(violin_p_best_residual_norm_filename, "%s/extracted_trace_data/extracted_trace_pso_%s/violin_p_best_residual_norm.txt", directory, filename);
    FILE *file_txt_violin_p_best_residual_norm = open_output(violin_p_best_residual_norm_filename, "w");
    if (file_txt_violin_p_best_residual_norm == NULL) {
        perror("Error opening commands file");
        exit(EXIT_FAILURE);
    }

    
    for (int order = *order_from; order <= *order_to; order++) {
        size_t array_size = 0;
        int check_array_size_set = 0;
        size_t index_all = 0;

        printf("ORDER %d\n !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", order);

        struct array** all_p_best_l_1 = malloc(4 * sizeof(struct array*));
        struct array** all_p_best_l_2 = malloc(4 * sizeof(struct array*));
        struct array** all_p_best_mean = malloc(4 * sizeof(struct array*));
        struct array** all_p_best_std = malloc(4 * sizeof(struct array*));
        struct array** all_p_best_residual_norms = malloc(4 * sizeof(struct array*));

        //initializing
        for (size_t a = 0; a < 4; a++) {
            all_p_best_l_1[a] = NULL;
            all_p_best_l_2[a] = NULL;
            all_p_best_mean[a] = NULL;
            all_p_best_std[a] = NULL;
            all_p_best_residual_norms[a] = NULL;
        }

        int seeds[] = {49052764, 12039846 ,32609861, 18796345};
        int seed_checks[] = {0, 0, 0, 0};

        for (int seed = 0; seed < 4; seed++) {
            char dd_pso_name[256];
            sprintf(dd_pso_name, "data_pso_%s_order_%d_seed_%d", filename, order, seeds[seed]);

            char dd_pso_directory[256];
            sprintf(dd_pso_directory, "%s/data/data_order_%d", directory, order);

            //check if the data dictionary exist for this seed
            char full_path_name[600];
            sprintf(full_path_name, "%s/%s.ddconfig", dd_pso_directory, dd_pso_name);

            int full_path_name_exists = check_file_path(full_path_name);

            if (full_path_name_exists) {
                printf("File Path Exists: %s\n", full_path_name);
            } else {
                printf("File Path Does Not Exists: %s\n", full_path_name);
            }

            if (full_path_name_exists) {
                //File path exists, can continue to extract the data
                seed_checks[seed] = full_path_name_exists;

                struct DataDictionary* data_dictionary_pso = read_from_file_dd(dd_pso_directory, dd_pso_name);

                size_t p_best_count = 0;
                struct array* p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});
                struct array* p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});
                struct array* p_best_mean = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});
                struct array* p_best_std = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});
                struct array* p_best_residual_norms = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});

                if (!check_array_size_set) {
                    array_size = (size_t){*data_dictionary_pso->amount_items};
                    check_array_size_set = 1;
                }

                //Iterate through all the datacontainers that holds the best values for a specific mapping
                for (size_t i = 0; i < *data_dictionary_pso->amount_items; i++) {
                
                    char filepath_dcon[256];
                    sprintf(filepath_dcon, "%s/%s", data_dictionary_pso->directory, data_dictionary_pso->identity);
                    struct DataContainer* data_container = read_from_file_dcon(filepath_dcon, data_dictionary_pso->items[i]);

                    //This datacontainer holds two arrays, double_data.array and positions.array
                    //positions.array
                    //  -   l1
                    //  -   l2
                    //
                    //double_data.array
                    //  -   global_best_iteration_mean
                    //  -   global_best_iteration_std
                    //  -   global_best_residual_norm
                    //  -   global_best_w
                    //  -   global_best_c1
                    //  -   global_best_c2

                    //open both arrays and save
                    //  -   p_best_l_1[i] = l1
                    //  -   p_best_l_2[i] = l2
                    //  -   p_best_mean[i] = global_best_iteration_mean
                    //  -   p_best_std[i] = global_best_iteration_std
                    //  -   p_best_residual_norms[i] = global_best_iteration_std

                    //##########################################################
                    char filepath_positions[512];
                    sprintf(filepath_positions, "%s/%s/%s.array", filepath_dcon, data_container->identity, "positions");
                    char filepath_double_data[512];
                    sprintf(filepath_double_data, "%s/%s/%s.array", filepath_dcon, data_container->identity, "double_data");

                    struct array* positions = read_from_file_a(filepath_positions);
                    struct array* double_data = read_from_file_a(filepath_double_data);


                    double l1_check = *(double*)get_element_a(positions, (size_t[]){0});
                    double l2_check = *(double*)get_element_a(positions, (size_t[]){1});
                    double mean_check = *(double*)get_element_a(double_data, (size_t[]){0});
                    double std_check = *(double*)get_element_a(double_data, (size_t[]){1});
                    double residual_norm_check = *(double*)get_element_a(double_data, (size_t[]){2});

                    double d_min_to_0_1 = sqrt(pow((0 - l1_check),2) + pow((1 - l2_check),2));
                    double d_min_to_1_0 = sqrt(pow((1 - l1_check),2) + pow((0 - l2_check),2));
                
                    if (d_min_to_0_1 <= d_min_to_1_0) {
                        //Add to p_left
                        set_element_a(p_best_l_1, (size_t[]){p_best_count}, &(double){l1_check});
                        set_element_a(p_best_l_2, (size_t[]){p_best_count}, &(double){l2_check});
                        set_element_a(p_best_mean, (size_t[]){p_best_count}, &(double){mean_check});
                        set_element_a(p_best_std, (size_t[]){p_best_count}, &(double){std_check});
                        set_element_a(p_best_residual_norms, (size_t[]){p_best_count}, &(double){residual_norm_check});
                        //increment p_left_count
                        p_best_count += 1;

                    } else {
                        //Switching l1 and l2 because of the symmetry
                        //Add to p_right
                        set_element_a(p_best_l_1, (size_t[]){p_best_count}, &(double){l2_check});
                        set_element_a(p_best_l_2, (size_t[]){p_best_count}, &(double){l1_check});
                        set_element_a(p_best_mean, (size_t[]){p_best_count}, &(double){mean_check});
                        set_element_a(p_best_std, (size_t[]){p_best_count}, &(double){std_check});
                        set_element_a(p_best_residual_norms, (size_t[]){p_best_count}, &(double){residual_norm_check});
                        //increment p_right_count
                        p_best_count += 1;
                    }
                    
                    free_a(positions);
                    free_a(double_data);

                    free_dcon(data_container);
                }


                all_p_best_l_1[index_all] = p_best_l_1;
                all_p_best_l_2[index_all] = p_best_l_2;
                all_p_best_mean[index_all] = p_best_mean;
                all_p_best_std[index_all] = p_best_std;
                all_p_best_residual_norms[index_all] = p_best_residual_norms;

                index_all += 1;

                free_dd(data_dictionary_pso);

            } else {
                //File path does not exist, continue to the next one
                seed_checks[seed] = full_path_name_exists;
            }
        }

        //First check if any of the seeds checked out
        int check_extract = 0;
        for (size_t o = 0; o < 4; o++) {
            if (seed_checks[o]) {
                check_extract = 1;
                break;
            }
        }

        if (check_extract) {

            //Creating data structures that will hold the final best data points
            // size_t final_p_best_count = 0;
            struct array* final_p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});
            struct array* final_p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});
            struct array* final_p_best_mean = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});
            struct array* final_p_best_std = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});
            struct array* final_p_best_residual_norms = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});

            //Get the best value between the different sources that exists
            for (size_t r = 0; r < array_size; r++) {
                int is_set = 0;
                double check_p_best_l_1 = 0;
                double check_p_best_l_2 = 0;
                double check_p_best_mean = 0;
                double check_p_best_std = 0;
                double check_p_best_residual_norms = 0;
                for (size_t g = 0; g < 4; g++) {
                    if (seed_checks[g]) {
                        if (!is_set) {
                            //This is when the variables above is not set yet
                            check_p_best_l_1 = *(double*)get_element_a(all_p_best_l_1[g], (size_t[]){r});
                            check_p_best_l_2 = *(double*)get_element_a(all_p_best_l_2[g], (size_t[]){r});
                            check_p_best_mean = *(double*)get_element_a(all_p_best_mean[g], (size_t[]){r});
                            check_p_best_std = *(double*)get_element_a(all_p_best_std[g], (size_t[]){r});
                            check_p_best_residual_norms = *(double*)get_element_a(all_p_best_residual_norms[g], (size_t[]){r});
                            is_set = 1;
                        } else {
                            //Variables are set and need to be checked if it should be update
                            double temp_check_p_best_l_1 = *(double*)get_element_a(all_p_best_l_1[g], (size_t[]){r});
                            double temp_check_p_best_l_2 = *(double*)get_element_a(all_p_best_l_2[g], (size_t[]){r});
                            double temp_check_p_best_mean = *(double*)get_element_a(all_p_best_mean[g], (size_t[]){r});
                            double temp_check_p_best_std = *(double*)get_element_a(all_p_best_std[g], (size_t[]){r});
                            double temp_check_p_best_residual_norms = *(double*)get_element_a(all_p_best_residual_norms[g], (size_t[]){r});

                            if (temp_check_p_best_mean < check_p_best_mean) {
                                //update
                                check_p_best_l_1 = temp_check_p_best_l_1;
                                check_p_best_l_2 = temp_check_p_best_l_2;
                                check_p_best_mean = temp_check_p_best_mean;
                                check_p_best_std = temp_check_p_best_std;
                                check_p_best_residual_norms = temp_check_p_best_residual_norms;
                            } else if (temp_check_p_best_mean == check_p_best_mean) {
                                if (temp_check_p_best_std < check_p_best_std) {
                                    //update
                                    check_p_best_l_1 = temp_check_p_best_l_1;
                                    check_p_best_l_2 = temp_check_p_best_l_2;
                                    check_p_best_mean = temp_check_p_best_mean;
                                    check_p_best_std = temp_check_p_best_std;
                                    check_p_best_residual_norms = temp_check_p_best_residual_norms;
                                } else if (temp_check_p_best_std == check_p_best_std) {
                                    if (temp_check_p_best_residual_norms < check_p_best_residual_norms) {
                                        //update
                                        check_p_best_l_1 = temp_check_p_best_l_1;
                                        check_p_best_l_2 = temp_check_p_best_l_2;
                                        check_p_best_mean = temp_check_p_best_mean;
                                        check_p_best_std = temp_check_p_best_std;
                                        check_p_best_residual_norms = temp_check_p_best_residual_norms;
                                    }
                                }
                            }

                        }
                    }
                }

                //Set the values in the final arrays
                set_element_a(final_p_best_l_1, (size_t[]){r}, &check_p_best_l_1);
                set_element_a(final_p_best_l_2, (size_t[]){r}, &check_p_best_l_2);
                set_element_a(final_p_best_mean, (size_t[]){r}, &check_p_best_mean);
                set_element_a(final_p_best_std, (size_t[]){r}, &check_p_best_std);
                set_element_a(final_p_best_residual_norms, (size_t[]){r}, &check_p_best_residual_norms);
            }

            // Save the data to the file
            for (size_t u = 0; u < get_size_a(final_p_best_mean); u++) {
                double* violin_arr = get_a(final_p_best_mean);
                if (u == 0) {
                    fprintf(file_txt_violin_p_best_mean, "%lf", violin_arr[u]);
                } else {
                    fprintf(file_txt_violin_p_best_mean, " %lf", violin_arr[u]);
                }
            }
            fprintf(file_txt_violin_p_best_mean, "\n");

            for (size_t u = 0; u < get_size_a(final_p_best_std); u++) {
                double* violin_arr = get_a(final_p_best_std);
                if (u == 0) {
                    fprintf(file_txt_violin_p_best_std, "%lf", violin_arr[u]);
                } else {
                    fprintf(file_txt_violin_p_best_std, " %lf", violin_arr[u]);
                }
            }
            fprintf(file_txt_violin_p_best_std, "\n");

            for (size_t u = 0; u < get_size_a(final_p_best_residual_norms); u++) {
                double* violin_arr = get_a(final_p_best_residual_norms);
                if (u == 0) {
                    fprintf(file_txt_violin_p_best_residual_norm, "%lf", violin_arr[u]);
                } else {
                    fprintf(file_txt_violin_p_best_residual_norm, " %lf", violin_arr[u]);
                }
            }
            fprintf(file_txt_violin_p_best_residual_norm, "\n");

            double* p_best_l_1_mean = mean_a(final_p_best_l_1);
            double* p_best_l_2_mean = mean_a(final_p_best_l_2);
            double* p_best_l_1_std = std_a(final_p_best_l_1);
            double* p_best_l_2_std = std_a(final_p_best_l_2);

            // Ellipses data block
            fprintf(file_ellipses_points, "%lf %lf %lf %lf\n", *p_best_l_1_mean, *p_best_l_2_mean, *p_best_l_1_std, *p_best_l_2_std);

            free(p_best_l_1_mean);
            free(p_best_l_2_mean);
            free(p_best_l_1_std);
            free(p_best_l_2_std);

            save_to_file_a(final_p_best_l_2, p_best_dir(directory), "pso_p_best_l_2");
            save_to_file_a(final_p_best_l_1, p_best_dir(directory), "pso_p_best_l_1");
            save_to_file_a(final_p_best_mean, p_best_dir(directory), "pso_p_best_mean");
            save_to_file_a(final_p_best_std, p_best_dir(directory), "pso_p_best_std");

            free_a(final_p_best_l_1);
            free_a(final_p_best_l_2);
            free_a(final_p_best_mean);
            free_a(final_p_best_std);
            free_a(final_p_best_residual_norms);
        }
        

        //Freeing the data structures that holds the data
        for (size_t y = 0; y < 4; y++) {
            if (all_p_best_l_1[y] != NULL) {
                free_a(all_p_best_l_1[y]);
            }
            if (all_p_best_l_2[y] != NULL) {
                free_a(all_p_best_l_2[y]);
            }
            if (all_p_best_mean[y] != NULL) {
                free_a(all_p_best_mean[y]);
            }
            if (all_p_best_std[y] != NULL) {
                free_a(all_p_best_std[y]);
            }
            if (all_p_best_residual_norms[y] != NULL) {
                free_a(all_p_best_residual_norms[y]);
            }
        }

        free(all_p_best_l_1);
        free(all_p_best_l_2);
        free(all_p_best_mean);
        free(all_p_best_std);
        free(all_p_best_residual_norms);

        //pbest

    }

    fclose(file_txt_violin_p_best_residual_norm);
    fclose(file_txt_violin_p_best_mean);
    fclose(file_txt_violin_p_best_std);
    fclose(file_ellipses_points);
}

void pso_extract_all_best_hyperparameters(char* directory, char* filename, int* order_from, int* order_to) {

    char all_points_filename[256];
    sprintf(all_points_filename, "%s/extracted_trace_data/extracted_trace_pso_%s/all_points.txt", directory, filename);
    FILE *file_all_points = open_output(all_points_filename, "w");
    if (file_all_points == NULL) {
        perror("Error opening commands file");
        exit(EXIT_FAILURE);
    }
    
    for (int order = *order_from; order <= *order_to; order++) {
        size_t array_size = 0;
        int check_array_size_set = 0;
        size_t index_all = 0;

        printf("ORDER %d\n !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", order);

        struct array** all_p_best_l_1 = malloc(4 * sizeof(struct array*));
        struct array** all_p_best_l_2 = malloc(4 * sizeof(struct array*));
        struct array** all_p_best_mean = malloc(4 * sizeof(struct array*));
        struct array** all_p_best_std = malloc(4 * sizeof(struct array*));
        struct array** all_p_best_residual_norms = malloc(4 * sizeof(struct array*));

        //initializing
        for (size_t a = 0; a < 4; a++) {
            all_p_best_l_1[a] = NULL;
            all_p_best_l_2[a] = NULL;
            all_p_best_mean[a] = NULL;
            all_p_best_std[a] = NULL;
            all_p_best_residual_norms[a] = NULL;
        }

        int seeds[] = {49052764, 12039846 ,32609861, 18796345};
        int seed_checks[] = {0, 0, 0, 0};

        for (int seed = 0; seed < 4; seed++) {
            char dd_pso_name[256];
            sprintf(dd_pso_name, "data_pso_%s_order_%d_seed_%d", filename, order, seeds[seed]);

            char dd_pso_directory[256];
            sprintf(dd_pso_directory, "%s/data/data_order_%d", directory, order);

            //check if the data dictionary exist for this seed
            char full_path_name[600];
            sprintf(full_path_name, "%s/%s.ddconfig", dd_pso_directory, dd_pso_name);

            int full_path_name_exists = check_file_path(full_path_name);

            if (full_path_name_exists) {
                printf("File Path Exists: %s\n", full_path_name);
            } else {
                printf("File Path Does Not Exists: %s\n", full_path_name);
            }

            if (full_path_name_exists) {
                //File path exists, can continue to extract the data
                seed_checks[seed] = full_path_name_exists;

                struct DataDictionary* data_dictionary_pso = read_from_file_dd(dd_pso_directory, dd_pso_name);

                size_t p_best_count = 0;
                struct array* p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});
                struct array* p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});
                struct array* p_best_mean = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});
                struct array* p_best_std = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});
                struct array* p_best_residual_norms = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});

                if (!check_array_size_set) {
                    array_size = (size_t){*data_dictionary_pso->amount_items};
                    check_array_size_set = 1;
                }

                //Iterate through all the datacontainers that holds the best values for a specific mapping
                for (size_t i = 0; i < *data_dictionary_pso->amount_items; i++) {
                
                    char filepath_dcon[256];
                    sprintf(filepath_dcon, "%s/%s", data_dictionary_pso->directory, data_dictionary_pso->identity);
                    struct DataContainer* data_container = read_from_file_dcon(filepath_dcon, data_dictionary_pso->items[i]);

                    char filepath_positions[512];
                    sprintf(filepath_positions, "%s/%s/%s.array", filepath_dcon, data_container->identity, "positions");
                    char filepath_double_data[512];
                    sprintf(filepath_double_data, "%s/%s/%s.array", filepath_dcon, data_container->identity, "double_data");

                    struct array* positions = read_from_file_a(filepath_positions);
                    struct array* double_data = read_from_file_a(filepath_double_data);


                    double l1_check = *(double*)get_element_a(positions, (size_t[]){0});
                    double l2_check = *(double*)get_element_a(positions, (size_t[]){1});
                    double mean_check = *(double*)get_element_a(double_data, (size_t[]){0});
                    double std_check = *(double*)get_element_a(double_data, (size_t[]){1});
                    double residual_norm_check = *(double*)get_element_a(double_data, (size_t[]){2});

                    double d_min_to_0_1 = sqrt(pow((0 - l1_check),2) + pow((1 - l2_check),2));
                    double d_min_to_1_0 = sqrt(pow((1 - l1_check),2) + pow((0 - l2_check),2));
                
                    /* The (0,1)/(1,0) fold exploits the symmetry of the even/odd
                     * damping pair. rho and nu are not interchangeable, so skip it. */
                    if (strncmp(filename, "lm_adaptive", 11) == 0 || d_min_to_0_1 <= d_min_to_1_0) {
                        //Add to p_left
                        set_element_a(p_best_l_1, (size_t[]){p_best_count}, &(double){l1_check});
                        set_element_a(p_best_l_2, (size_t[]){p_best_count}, &(double){l2_check});
                        set_element_a(p_best_mean, (size_t[]){p_best_count}, &(double){mean_check});
                        set_element_a(p_best_std, (size_t[]){p_best_count}, &(double){std_check});
                        set_element_a(p_best_residual_norms, (size_t[]){p_best_count}, &(double){residual_norm_check});
                        //increment p_left_count
                        p_best_count += 1;

                    } else {
                        //Switching l1 and l2 because of the symmetry
                        //Add to p_right
                        set_element_a(p_best_l_1, (size_t[]){p_best_count}, &(double){l2_check});
                        set_element_a(p_best_l_2, (size_t[]){p_best_count}, &(double){l1_check});
                        set_element_a(p_best_mean, (size_t[]){p_best_count}, &(double){mean_check});
                        set_element_a(p_best_std, (size_t[]){p_best_count}, &(double){std_check});
                        set_element_a(p_best_residual_norms, (size_t[]){p_best_count}, &(double){residual_norm_check});
                        //increment p_right_count
                        p_best_count += 1;
                    }
                    
                    free_a(positions);
                    free_a(double_data);

                    free_dcon(data_container);
                }


                all_p_best_l_1[index_all] = p_best_l_1;
                all_p_best_l_2[index_all] = p_best_l_2;
                all_p_best_mean[index_all] = p_best_mean;
                all_p_best_std[index_all] = p_best_std;
                all_p_best_residual_norms[index_all] = p_best_residual_norms;

                index_all += 1;

                free_dd(data_dictionary_pso);

            } else {
                //File path does not exist, continue to the next one
                seed_checks[seed] = full_path_name_exists;
            }
        }

        //First check if any of the seeds checked out
        int check_extract = 0;
        for (size_t o = 0; o < 4; o++) {
            if (seed_checks[o]) {
                check_extract = 1;
                break;
            }
        }

        if (check_extract) {

            //Creating data structures that will hold the final best data points
            // size_t final_p_best_count = 0;
            struct array* final_p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});
            struct array* final_p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});
            struct array* final_p_best_mean = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});
            struct array* final_p_best_std = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});
            struct array* final_p_best_residual_norms = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});

            //Get the best value between the different sources that exists
            for (size_t r = 0; r < array_size; r++) {
                int is_set = 0;
                double check_p_best_l_1 = 0;
                double check_p_best_l_2 = 0;
                double check_p_best_mean = 0;
                double check_p_best_std = 0;
                double check_p_best_residual_norms = 0;
                for (size_t g = 0; g < 4; g++) {
                    if (seed_checks[g]) {
                        if (!is_set) {
                            //This is when the variables above is not set yet
                            check_p_best_l_1 = *(double*)get_element_a(all_p_best_l_1[g], (size_t[]){r});
                            check_p_best_l_2 = *(double*)get_element_a(all_p_best_l_2[g], (size_t[]){r});
                            check_p_best_mean = *(double*)get_element_a(all_p_best_mean[g], (size_t[]){r});
                            check_p_best_std = *(double*)get_element_a(all_p_best_std[g], (size_t[]){r});
                            check_p_best_residual_norms = *(double*)get_element_a(all_p_best_residual_norms[g], (size_t[]){r});
                            is_set = 1;
                        } else {
                            //Variables are set and need to be checked if it should be update
                            double temp_check_p_best_l_1 = *(double*)get_element_a(all_p_best_l_1[g], (size_t[]){r});
                            double temp_check_p_best_l_2 = *(double*)get_element_a(all_p_best_l_2[g], (size_t[]){r});
                            double temp_check_p_best_mean = *(double*)get_element_a(all_p_best_mean[g], (size_t[]){r});
                            double temp_check_p_best_std = *(double*)get_element_a(all_p_best_std[g], (size_t[]){r});
                            double temp_check_p_best_residual_norms = *(double*)get_element_a(all_p_best_residual_norms[g], (size_t[]){r});

                            if (temp_check_p_best_mean < check_p_best_mean) {
                                //update
                                check_p_best_l_1 = temp_check_p_best_l_1;
                                check_p_best_l_2 = temp_check_p_best_l_2;
                                check_p_best_mean = temp_check_p_best_mean;
                                check_p_best_std = temp_check_p_best_std;
                                check_p_best_residual_norms = temp_check_p_best_residual_norms;
                            } else if (temp_check_p_best_mean == check_p_best_mean) {
                                if (temp_check_p_best_std < check_p_best_std) {
                                    //update
                                    check_p_best_l_1 = temp_check_p_best_l_1;
                                    check_p_best_l_2 = temp_check_p_best_l_2;
                                    check_p_best_mean = temp_check_p_best_mean;
                                    check_p_best_std = temp_check_p_best_std;
                                    check_p_best_residual_norms = temp_check_p_best_residual_norms;
                                } else if (temp_check_p_best_std == check_p_best_std) {
                                    if (temp_check_p_best_residual_norms < check_p_best_residual_norms) {
                                        //update
                                        check_p_best_l_1 = temp_check_p_best_l_1;
                                        check_p_best_l_2 = temp_check_p_best_l_2;
                                        check_p_best_mean = temp_check_p_best_mean;
                                        check_p_best_std = temp_check_p_best_std;
                                        check_p_best_residual_norms = temp_check_p_best_residual_norms;
                                    }
                                }
                            }

                        }
                    }
                }

                //Set the values in the final arrays
                set_element_a(final_p_best_l_1, (size_t[]){r}, &check_p_best_l_1);
                set_element_a(final_p_best_l_2, (size_t[]){r}, &check_p_best_l_2);
                set_element_a(final_p_best_mean, (size_t[]){r}, &check_p_best_mean);
                set_element_a(final_p_best_std, (size_t[]){r}, &check_p_best_std);
                set_element_a(final_p_best_residual_norms, (size_t[]){r}, &check_p_best_residual_norms);
            }

            for (size_t r = 0; r < array_size; r++) {
                if (r == array_size - 1) {
                    fprintf(file_all_points, "%lf,%lf;\n", *(double*)get_element_a(final_p_best_l_1, (size_t[]){r}) , *(double*)get_element_a(final_p_best_l_2, (size_t[]){r}));
                } else {
                    fprintf(file_all_points, "%lf,%lf;", *(double*)get_element_a(final_p_best_l_1, (size_t[]){r}) , *(double*)get_element_a(final_p_best_l_2, (size_t[]){r}));
                }
            }

            // Ellipses data block
            free_a(final_p_best_l_1);
            free_a(final_p_best_l_2);
            free_a(final_p_best_mean);
            free_a(final_p_best_std);
            free_a(final_p_best_residual_norms);
        }
        

        //Freeing the data structures that holds the data
        for (size_t y = 0; y < 4; y++) {
            if (all_p_best_l_1[y] != NULL) {
                free_a(all_p_best_l_1[y]);
            }
            if (all_p_best_l_2[y] != NULL) {
                free_a(all_p_best_l_2[y]);
            }
            if (all_p_best_mean[y] != NULL) {
                free_a(all_p_best_mean[y]);
            }
            if (all_p_best_std[y] != NULL) {
                free_a(all_p_best_std[y]);
            }
            if (all_p_best_residual_norms[y] != NULL) {
                free_a(all_p_best_residual_norms[y]);
            }
        }

        free(all_p_best_l_1);
        free(all_p_best_l_2);
        free(all_p_best_mean);
        free(all_p_best_std);
        free(all_p_best_residual_norms);

        //pbest

    }

    fclose(file_all_points);
}

/**
 * Local (non-HPC) driver for the stochastic-adaptive StEFCal gridding sweep over (lambda, b0).
 * Loops all (seed, snr, max_amp) experiments for each order in [order_min, order_max].
 */
void mapping_stochastic_adaptive_local(char* directory, int* order_min, int* order_max) {
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;
    int num_workers = workers(30);
    size_t grid_length = grid_points(50);
    enum AlgorithmType algorithm_type = STEFCAL_STOCHASTIC_ADAPTIVE;
    size_t amount_hyperparameters = 2;
    struct array* hyperparameters_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4},
                                    &(enum DataType){DOUBLE},
                                    (double[4]){
                                        0, 3, 2, 16
                                    });

    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    for (int order = *order_min; order <= *order_max; order++) {
        char dd_directory[256];
        sprintf(dd_directory, "%s/data/data_order_%d", directory, order);
        char dd_name[256];
        sprintf(dd_name, "data_stochastic_adaptive_order_%d", order);

        struct DataDictionary* data_dictionary = read_from_file_dd(dd_directory, dd_name);

        size_t counter = 0;
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    if (counter >= *data_dictionary->amount_items) {
                        struct DataStore* data_store = new_data_store_update_dd(&data_dictionary, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &(double){snrs[i]}, &(double){max_amps[j]}, &(int){seeds[h]}, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds);
                        start_data_store_dd(data_store);
                        free_ds(data_store);
                    }
                    counter += 1;
                }
            }
        }

        free_dd(data_dictionary);
    }

    free_a(hyperparameters_bounds);
}

/**
 * Local (non-HPC) driver for the stochastic-adaptive StEFCal PSO tuner. Sweeps the PSO seeds
 * for each order in [order_min, order_max], tuning (lambda, b0).
 */
void mapping_pso_stochastic_adaptive_local(char* directory, int* order_min, int* order_max) {

    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};

    enum AlgorithmType option = STEFCAL_STOCHASTIC_ADAPTIVE;
    size_t num_dimensions = 2;
    struct array* position_bounds  = new_set_a(&(size_t){1},
                                    (size_t[]){4},
                                    &(enum DataType){DOUBLE},
                                    (double[4]){
                                        0, 3, 2, 16
                                    });

    size_t iterations_max = pso_iterations(50);
    size_t num_particles = pso_particles(25);
    size_t k = 9;
    int num_workers = workers(30);

    char simulators_directory[256];
    sprintf(simulators_directory, "%s/simulators", directory);

    for (int order = *order_min; order <= *order_max; order++) {
        for (size_t i = 0; i < 4; i++) {
            int pso_seed = pso_seeds[i];

            char dd_directory[256];
            sprintf(dd_directory, "%s/data/data_order_%d", directory, order);
            char dd_name[256];
            sprintf(dd_name, "data_pso_stochastic_adaptive_order_%d_seed_%d", order, pso_seed);

            struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, &num_particles, &pso_seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers);
            start_single_pso_local(pso);
            free_pso(pso);
        }
    }

    free_a(position_bounds);
}

void mapping_pso_stochastic_l1l2_local(char* directory, int* order_min, int* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    enum AlgorithmType option = STEFCAL_STOCHASTIC_L1L2;
    size_t num_dimensions = 3;
    struct array* position_bounds  = new_set_a(&(size_t){1},
                                    (size_t[]){6},
                                    &(enum DataType){DOUBLE},
                                    (double[6]){
                                        0.001, 3, 0.001, 3, 2, 16
                                    });
    size_t iterations_max = pso_iterations(50);
    size_t num_particles = pso_particles(25);
    size_t k = 9;
    int num_workers = workers(30);
    char simulators_directory[256];
    sprintf(simulators_directory, "%s/simulators", directory);
    for (int order = *order_min; order <= *order_max; order++) {
        for (size_t i = 0; i < 4; i++) {
            int pso_seed = pso_seeds[i];
            char dd_directory[256];
            sprintf(dd_directory, "%s/data/data_order_%d", directory, order);
            char dd_name[256];
            sprintf(dd_name, "data_pso_stochastic_l1l2_order_%d_seed_%d", order, pso_seed);
            struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, &num_particles, &pso_seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers);
            start_single_pso_local(pso);
            free_pso(pso);
        }
    }
    free_a(position_bounds);
}

/* Round 2a: 2-param PSO over (l1, l2) with b0 fixed to the round-1 best (loaded per experiment from
 * data_optimal_solutions/stochastic_best_b/...). Redundant-by-design = the consistency check. */

void mapping_pso_stochastic_fixed_b_local(char* directory, int* order_min, int* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    enum AlgorithmType option = STEFCAL_STOCHASTIC_FIXED_B;
    size_t num_dimensions = 2;
    struct array* position_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4},
                                    &(enum DataType){DOUBLE},
                                    (double[4]){
                                        0.001, 3, 0.001, 3
                                    });
    size_t iterations_max = pso_iterations(50);
    size_t num_particles = pso_particles(25);
    size_t k = 9;
    int num_workers = workers(30);
    char simulators_directory[256];
    sprintf(simulators_directory, "%s/simulators", directory);
    for (int order = *order_min; order <= *order_max; order++) {
        for (size_t i = 0; i < 4; i++) {
            int pso_seed = pso_seeds[i];
            char dd_directory[256];
            sprintf(dd_directory, "%s/data/data_order_%d", directory, order);
            char dd_name[256];
            sprintf(dd_name, "data_pso_stochastic_fixed_b_order_%d_seed_%d", order, pso_seed);
            struct PSO* pso = new_pso_fixed_b(&option, &num_dimensions, position_bounds, &iterations_max, &num_particles, &pso_seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers, NULL);
            pso->b_directory = malloc(strlen(directory) + 1);
            strcpy(pso->b_directory, directory);
            start_single_pso_local(pso);
            free_pso(pso);
        }
    }
    free_a(position_bounds);
}

void extraction_optimal_stefcal(char* directory, size_t* order_min, size_t* order_max) {
    int pso_seeds[] = {49052764, 12039846 ,32609861, 18796345};
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};
    int num_workers = workers(28);

    //order
    for (size_t order = *order_min; order <= *order_max; order++) {
        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    //This is for the pso_seeds
                    //Compare performance for each pso_seed number and take the best performing.
                    //Run and save: 
                    //  -iterations for all time steps
                    //  -last residual norm for all time steps
                    //  -last gain for each time step
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_l1 = INFINITY;
                    double best_l2 = INFINITY;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[256];
                        sprintf(path_doubles,"%s/data/data_order_%ld/data_pso_stefcal_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array", directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[256];
                        sprintf(path_positions,"%s/data/data_order_%ld/data_pso_stefcal_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array", directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_l1 = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double position_l2 = *(double*)get_element_a(positions_array, (size_t[]){1});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_l1 = position_l1;
                            best_l2 = position_l2;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_l1 = position_l1;
                                best_l2 = position_l2;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_l1 = position_l1;
                                    best_l2 = position_l2;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    //Need to run the specific hyperparameters
                    char simulator_path [256];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig", directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    solve_G_StEFCal_Lambda(calibrator, &(size_t){IMAX}, &(double){TAU}, &(double){best_l1}, &(double){best_l2});

                    //get the data needed for graphing
                    //all information extracted will be freed with the calibrator
                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector); 
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);         
                    
                    char directory_save[256];
                    sprintf(directory_save,"%s/data_optimal_solutions/stefcal_optimal/data_order_%ld/data_stefcal_lambda_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    double* mean_test = mean_a(iterations);
                    printf("l1 = %f -  l2 = %f\n", best_l1, best_l2);
                    printf("MEANS TEST: %f - %f\n", *mean_test, best_iteration_mean);
                    free(mean_test);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }

}

void extraction_0_1_stefcal(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};
    int num_workers = workers(20);

    //order
    for (size_t order = *order_min; order <= *order_max; order++) {
        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char simulator_path [256];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig", directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator_0_1 = new_c(simulator);
                    set_parallel_c(calibrator_0_1, &num_workers);
                    solve_G_StEFCal_Lambda(calibrator_0_1, &(size_t){IMAX}, &(double){TAU}, &(double){0}, &(double){1});
                    double* iteration_mean_0_1 = (double*)get_average_iterations_dc(calibrator_0_1->datacollector);
                    double* iteration_std_0_1 = (double*)get_standard_deviation_iterations_dc(calibrator_0_1->datacollector);
                    double* residual_norm_0_1 = (double*)get_last_residuals_norm_dc(calibrator_0_1->datacollector);

                    struct Calibrator* calibrator_1_0 = new_c(simulator);
                    set_parallel_c(calibrator_1_0, &num_workers);
                    solve_G_StEFCal_Lambda(calibrator_1_0, &(size_t){IMAX}, &(double){TAU}, &(double){1}, &(double){0});
                    double* iteration_mean_1_0 = (double*)get_average_iterations_dc(calibrator_1_0->datacollector);
                    double* iteration_std_1_0 = (double*)get_standard_deviation_iterations_dc(calibrator_1_0->datacollector);
                    double* residual_norm_1_0 = (double*)get_last_residuals_norm_dc(calibrator_1_0->datacollector);

                    int check = 1;
                    if (*iteration_mean_0_1 < *iteration_mean_1_0) {
                        check = 0;
                    } else if ((*iteration_mean_0_1 - *iteration_mean_1_0) < EPSILON) {
                        if (*iteration_std_0_1 < *iteration_std_1_0) {
                            check = 0;
                        } else if ((*iteration_std_0_1 - *iteration_std_1_0) < EPSILON) {
                            if (*residual_norm_0_1 < *residual_norm_1_0) {
                                check = 0;
                            }
                        }
                    }
                    
                    //all information extracted will be freed with the calibrator
                    struct array* iterations = NULL;
                    struct JaggedArray* all_gains = NULL;
                    struct JaggedArray* damping_g = NULL;
                    struct JaggedArray* damping_g_old = NULL; 
                    struct JaggedArray* last_gains = NULL;
                    struct JaggedArray* last_residuals = NULL;     

                    if (check) {
                        iterations = get_iterations_dc(calibrator_1_0->datacollector);
                        all_gains = get_all_gain_dc(calibrator_1_0->datacollector);
                        damping_g = get_damping_g_dc(calibrator_1_0->datacollector);
                        damping_g_old = get_damping_g_old_dc(calibrator_1_0->datacollector); 
                        last_gains = get_last_gains_dc(calibrator_1_0->datacollector);
                        last_residuals = get_last_residual_dc(calibrator_1_0->datacollector); 
                    } else {
                        iterations = get_iterations_dc(calibrator_0_1->datacollector);
                        all_gains = get_all_gain_dc(calibrator_0_1->datacollector);
                        damping_g = get_damping_g_dc(calibrator_0_1->datacollector);
                        damping_g_old = get_damping_g_old_dc(calibrator_0_1->datacollector); 
                        last_gains = get_last_gains_dc(calibrator_0_1->datacollector);
                        last_residuals = get_last_residual_dc(calibrator_0_1->datacollector); 
                    }

                    char directory_save[256];
                    //printf("Saving to Directory\n");
                    sprintf(directory_save,"%s/data_optimal_solutions/stefcal_original/data_order_%ld/data_stefcal_lambda_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");

                    free(residual_norm_1_0);
                    free(iteration_std_1_0);
                    free(iteration_mean_1_0);

                    free(residual_norm_0_1);
                    free(iteration_std_0_1);
                    free(iteration_mean_0_1);

                    free_ja(all_gains);

                    free_c(calibrator_0_1);
                    free_c(calibrator_1_0);
                    free_s(simulator);
                }
            }
        }
    }
}

void extraction_optimal_stefcal_improved(char* directory, size_t* order_min, size_t* order_max) {
    int pso_seeds[] = {49052764, 12039846 ,32609861, 18796345};
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};
    int num_workers = workers(28);

    //order
    for (size_t order = *order_min; order <= *order_max; order++) {
        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    //This is for the pso_seeds
                    //Compare performance for each pso_seed number and take the best performing.
                    //Run and save: 
                    //  -iterations for all time steps
                    //  -last residual norm for all time steps
                    //  -last gain for each time step
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_l1 = INFINITY;
                    double best_l2 = INFINITY;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[256];
                        sprintf(path_doubles,"%s/data/data_order_%ld/data_pso_stefcal_improved_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array", directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[256];
                        sprintf(path_positions,"%s/data/data_order_%ld/data_pso_stefcal_improved_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array", directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_l1 = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double position_l2 = *(double*)get_element_a(positions_array, (size_t[]){1});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_l1 = position_l1;
                            best_l2 = position_l2;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_l1 = position_l1;
                                best_l2 = position_l2;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_l1 = position_l1;
                                    best_l2 = position_l2;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    //Need to run the specific hyperparameters
                    char simulator_path [256];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig", directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    solve_G_StEFCal_improved(calibrator, &(size_t){IMAX}, &(double){TAU}, &(double){best_l1}, &(double){best_l2});

                    //get the data needed for graphing
                    //all information extracted will be freed with the calibrator
                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector); 
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);         
                    
                    char directory_save[256];
                    sprintf(directory_save,"%s/data_optimal_solutions/stefcal_improved_optimal/data_order_%ld/data_stefcal_improved_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    double* mean_test = mean_a(iterations);
                    printf("l1 = %f -  l2 = %f\n", best_l1, best_l2);
                    printf("MEANS TEST: %f - %f\n", *mean_test, best_iteration_mean);
                    free(mean_test);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }

}

void extraction_original_stefcal_improved(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};
    int num_workers = workers(25);

    //order
    for (size_t order = *order_min; order <= *order_max; order++) {
        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char simulator_path [256];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig", directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);

                    struct Calibrator* calibrator_0 = new_c(simulator);
                    set_parallel_c(calibrator_0, &num_workers);
                    solve_G_StEFCal_improved(calibrator_0, &(size_t){IMAX}, &(double){TAU}, &(double){0.05}, &(double){1});
                    double* iteration_mean_0 = (double*)get_average_iterations_dc(calibrator_0->datacollector);
                    double* iteration_std_0 = (double*)get_standard_deviation_iterations_dc(calibrator_0->datacollector);
                    double* residual_norm_0 = (double*)get_last_residuals_norm_dc(calibrator_0->datacollector);

                    struct Calibrator* calibrator_1 = new_c(simulator);
                    set_parallel_c(calibrator_1, &num_workers);
                    solve_G_StEFCal_improved(calibrator_1, &(size_t){IMAX}, &(double){TAU}, &(double){-0.05}, &(double){1});
                    double* iteration_mean_1 = (double*)get_average_iterations_dc(calibrator_1->datacollector);
                    double* iteration_std_1 = (double*)get_standard_deviation_iterations_dc(calibrator_1->datacollector);
                    double* residual_norm_1 = (double*)get_last_residuals_norm_dc(calibrator_1->datacollector);

                    struct Calibrator* calibrator_2 = new_c(simulator);
                    set_parallel_c(calibrator_2, &num_workers);
                    solve_G_StEFCal_improved(calibrator_2, &(size_t){IMAX}, &(double){TAU}, &(double){1}, &(double){0.05});
                    double* iteration_mean_2 = (double*)get_average_iterations_dc(calibrator_2->datacollector);
                    double* iteration_std_2 = (double*)get_standard_deviation_iterations_dc(calibrator_2->datacollector);
                    double* residual_norm_2 = (double*)get_last_residuals_norm_dc(calibrator_2->datacollector);

                    struct Calibrator* calibrator_3 = new_c(simulator);
                    set_parallel_c(calibrator_3, &num_workers);
                    solve_G_StEFCal_improved(calibrator_3, &(size_t){IMAX}, &(double){TAU}, &(double){1}, &(double){-0.05});
                    double* iteration_mean_3 = (double*)get_average_iterations_dc(calibrator_3->datacollector);
                    double* iteration_std_3 = (double*)get_standard_deviation_iterations_dc(calibrator_3->datacollector);
                    double* residual_norm_3 = (double*)get_last_residuals_norm_dc(calibrator_3->datacollector);

                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_mean_residual_norm = INFINITY;

                    int check = -1;

                    if (*iteration_mean_0 < best_iteration_mean) {
                        check = 0;
                        best_iteration_mean = *iteration_mean_0;
                        best_iteration_std = *iteration_std_0;
                        best_mean_residual_norm = *residual_norm_0;
                    } else if ((*iteration_mean_0 - best_iteration_mean) < EPSILON) {
                        if (*iteration_std_0 < best_iteration_std) {
                            check = 0;
                            best_iteration_mean = *iteration_mean_0;
                            best_iteration_std = *iteration_std_0;
                            best_mean_residual_norm = *residual_norm_0;
                        } else if ((*iteration_std_0 - best_iteration_std) < EPSILON) {
                            if (*residual_norm_0 < best_mean_residual_norm) {
                                check = 0;
                                best_iteration_mean = *iteration_mean_0;
                                best_iteration_std = *iteration_std_0;
                                best_mean_residual_norm = *residual_norm_0;
                            }
                        }
                    }

                    if (*iteration_mean_1 < best_iteration_mean) {
                        check = 1;
                        best_iteration_mean = *iteration_mean_1;
                        best_iteration_std = *iteration_std_1;
                        best_mean_residual_norm = *residual_norm_1;
                    } else if ((*iteration_mean_1 - best_iteration_mean) < EPSILON) {
                        if (*iteration_std_1 < best_iteration_std) {
                            check = 1;
                            best_iteration_mean = *iteration_mean_1;
                            best_iteration_std = *iteration_std_1;
                            best_mean_residual_norm = *residual_norm_1;
                        } else if ((*iteration_std_1 - best_iteration_std) < EPSILON) {
                            if (*residual_norm_1 < best_mean_residual_norm) {
                                check = 1;
                                best_iteration_mean = *iteration_mean_1;
                                best_iteration_std = *iteration_std_1;
                                best_mean_residual_norm = *residual_norm_1;
                            }
                        }
                    }

                    if (*iteration_mean_2 < best_iteration_mean) {
                        check = 2;
                        best_iteration_mean = *iteration_mean_2;
                        best_iteration_std = *iteration_std_2;
                        best_mean_residual_norm = *residual_norm_2;
                    } else if ((*iteration_mean_2 - best_iteration_mean) < EPSILON) {
                        if (*iteration_std_2 < best_iteration_std) {
                            check = 2;
                            best_iteration_mean = *iteration_mean_2;
                            best_iteration_std = *iteration_std_2;
                            best_mean_residual_norm = *residual_norm_2;
                        } else if ((*iteration_std_2 - best_iteration_std) < EPSILON) {
                            if (*residual_norm_2 < best_mean_residual_norm) {
                                check = 2;
                                best_iteration_mean = *iteration_mean_2;
                                best_iteration_std = *iteration_std_2;
                                best_mean_residual_norm = *residual_norm_2;
                            }
                        }
                    }

                    if (*iteration_mean_3 < best_iteration_mean) {
                        check = 3;
                        best_iteration_mean = *iteration_mean_3;
                        best_iteration_std = *iteration_std_3;
                        best_mean_residual_norm = *residual_norm_3;
                    } else if ((*iteration_mean_3 - best_iteration_mean) < EPSILON) {
                        if (*iteration_std_3 < best_iteration_std) {
                            check = 3;
                            best_iteration_mean = *iteration_mean_3;
                            best_iteration_std = *iteration_std_3;
                            best_mean_residual_norm = *residual_norm_3;
                        } else if ((*iteration_std_3 - best_iteration_std) < EPSILON) {
                            if (*residual_norm_3 < best_mean_residual_norm) {
                                check = 3;
                                best_iteration_mean = *iteration_mean_3;
                                best_iteration_std = *iteration_std_3;
                                best_mean_residual_norm = *residual_norm_3;
                            }
                        }
                    }
                    
                    //all information extracted will be freed with the calibrator
                    struct array* iterations = NULL;
                    struct JaggedArray* all_gains = NULL;
                    struct JaggedArray* damping_g = NULL;
                    struct JaggedArray* damping_g_old = NULL; 
                    struct JaggedArray* last_gains = NULL;
                    struct JaggedArray* last_residuals = NULL;     

                    if (check == 0) {
                        iterations = get_iterations_dc(calibrator_0->datacollector);
                        all_gains = get_all_gain_dc(calibrator_0->datacollector);
                        damping_g = get_damping_g_dc(calibrator_0->datacollector);
                        damping_g_old = get_damping_g_old_dc(calibrator_0->datacollector); 
                        last_gains = get_last_gains_dc(calibrator_0->datacollector);
                        last_residuals = get_last_residual_dc(calibrator_0->datacollector); 
                    } else if (check == 1) {
                        iterations = get_iterations_dc(calibrator_1->datacollector);
                        all_gains = get_all_gain_dc(calibrator_1->datacollector);
                        damping_g = get_damping_g_dc(calibrator_1->datacollector);
                        damping_g_old = get_damping_g_old_dc(calibrator_1->datacollector); 
                        last_gains = get_last_gains_dc(calibrator_1->datacollector);
                        last_residuals = get_last_residual_dc(calibrator_1->datacollector); 
                    } else if (check == 2) {
                        iterations = get_iterations_dc(calibrator_2->datacollector);
                        all_gains = get_all_gain_dc(calibrator_2->datacollector);
                        damping_g = get_damping_g_dc(calibrator_2->datacollector);
                        damping_g_old = get_damping_g_old_dc(calibrator_2->datacollector); 
                        last_gains = get_last_gains_dc(calibrator_2->datacollector);
                        last_residuals = get_last_residual_dc(calibrator_2->datacollector); 
                    } else if (check == 3) {
                        iterations = get_iterations_dc(calibrator_3->datacollector);
                        all_gains = get_all_gain_dc(calibrator_3->datacollector);
                        damping_g = get_damping_g_dc(calibrator_3->datacollector);
                        damping_g_old = get_damping_g_old_dc(calibrator_3->datacollector); 
                        last_gains = get_last_gains_dc(calibrator_3->datacollector);
                        last_residuals = get_last_residual_dc(calibrator_3->datacollector); 
                    }

                    char directory_save[256];
                    //printf("Saving to Directory\n");
                    sprintf(directory_save,"%s/data_optimal_solutions/stefcal_improved_original/data_order_%ld/data_stefcal_improved_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");

                    free(residual_norm_0);
                    free(iteration_std_0);
                    free(iteration_mean_0);

                    free(residual_norm_1);
                    free(iteration_std_1);
                    free(iteration_mean_1);
                    
                    free(residual_norm_2);
                    free(iteration_std_2);
                    free(iteration_mean_2);

                    free(residual_norm_3);
                    free(iteration_std_3);
                    free(iteration_mean_3);

                    free_ja(all_gains);

                    free_c(calibrator_0);
                    free_c(calibrator_1);
                    free_c(calibrator_2);
                    free_c(calibrator_3);
                    
                    free_s(simulator);
                }
            }
        }
    }
}

void extraction_optimal_lm_non_adaptive_l1_l2(char* directory, size_t* order_min, size_t* order_max) {
    int pso_seeds[] = {49052764, 12039846 ,32609861, 18796345};
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};
    int num_workers = workers(28);

    //order
    for (size_t order = *order_min; order <= *order_max; order++) {
        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    //This is for the pso_seeds
                    //Compare performance for each pso_seed number and take the best performing.
                    //Run and save: 
                    //  -iterations for all time steps
                    //  -last residual norm for all time steps
                    //  -last gain for each time step
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_l1 = INFINITY;
                    double best_l2 = INFINITY;

                    for (size_t k = 0; k < 4; k++) {  
                        char path_doubles[256];
                        sprintf(path_doubles,"%s/data/data_order_%ld/data_pso_lm_non_adaptive_l1_l2_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array", directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[256];
                        sprintf(path_positions,"%s/data/data_order_%ld/data_pso_lm_non_adaptive_l1_l2_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array", directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_l1 = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double position_l2 = *(double*)get_element_a(positions_array, (size_t[]){1});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_l1 = position_l1;
                            best_l2 = position_l2;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_l1 = position_l1;
                                best_l2 = position_l2;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_l1 = position_l1;
                                    best_l2 = position_l2;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    //Need to run the specific hyperparameters
                    char simulator_path [256];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig", directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    solve_G_LM_Non_Adaptive_l1_l2(calibrator, &(size_t){IMAX}, &(double){TAU}, &(double){best_l1}, &(double){best_l2});

                    //get the data needed for graphing
                    //all information extracted will be freed with the calibrator
                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector); 
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);         
                    
                    char directory_save[256];
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_non_adaptive_l1_l2_optimal/data_order_%ld/data_lm_non_adaptive_l1_l2_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }
}

void extraction_optimal_lm_non_adaptive_l1_l2_REDUCED_TIMESTEPS(char* directory, size_t* order_min, size_t* order_max) {
    int pso_seeds[] = {49052764, 12039846 ,32609861, 18796345};
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};
    int num_workers = workers(28);

    //order
    for (size_t order = *order_min; order <= *order_max; order++) {
        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    //This is for the pso_seeds
                    //Compare performance for each pso_seed number and take the best performing.
                    //Run and save: 
                    //  -iterations for all time steps
                    //  -last residual norm for all time steps
                    //  -last gain for each time step
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_l1 = INFINITY;
                    double best_l2 = INFINITY;

                    for (size_t k = 0; k < 4; k++) {  
                        char path_doubles[256];
                        sprintf(path_doubles,"%s/data/data_order_%ld/data_pso_lm_non_adaptive_l1_l2_REDUCED_TIMESTEPS_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array", directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[256];
                        sprintf(path_positions,"%s/data/data_order_%ld/data_pso_lm_non_adaptive_l1_l2_REDUCED_TIMESTEPS_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array", directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_l1 = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double position_l2 = *(double*)get_element_a(positions_array, (size_t[]){1});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_l1 = position_l1;
                            best_l2 = position_l2;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_l1 = position_l1;
                                best_l2 = position_l2;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_l1 = position_l1;
                                    best_l2 = position_l2;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    //Need to run the specific hyperparameters
                    char simulator_path [256];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig", directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    solve_G_LM_Non_Adaptive_l1_l2(calibrator, &(size_t){IMAX}, &(double){TAU}, &(double){best_l1}, &(double){best_l2});

                    //get the data needed for graphing
                    //all information extracted will be freed with the calibrator
                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector); 
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);         
                    
                    char directory_save[256];
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_non_adaptive_l1_l2_optimal/data_order_%ld/data_lm_non_adaptive_l1_l2_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }

}

void extraction_original_lm_non_adaptive_l1_l2(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};
    int num_workers = workers(28);

    //order
    for (size_t order = *order_min; order <= *order_max; order++) {
        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char simulator_path [256];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig", directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator_0_1 = new_c(simulator);
                    set_parallel_c(calibrator_0_1, &num_workers);
                    solve_G_LM_Non_Adaptive_l1_l2(calibrator_0_1, &(size_t){IMAX}, &(double){TAU}, &(double){0}, &(double){1});
                    double* iteration_mean_0_1 = (double*)get_average_iterations_dc(calibrator_0_1->datacollector);
                    double* iteration_std_0_1 = (double*)get_standard_deviation_iterations_dc(calibrator_0_1->datacollector);
                    double* residual_norm_0_1 = (double*)get_last_residuals_norm_dc(calibrator_0_1->datacollector);

                    struct Calibrator* calibrator_1_0 = new_c(simulator);
                    set_parallel_c(calibrator_1_0, &num_workers);
                    solve_G_LM_Non_Adaptive_l1_l2(calibrator_1_0, &(size_t){IMAX}, &(double){TAU}, &(double){1}, &(double){0});
                    double* iteration_mean_1_0 = (double*)get_average_iterations_dc(calibrator_1_0->datacollector);
                    double* iteration_std_1_0 = (double*)get_standard_deviation_iterations_dc(calibrator_1_0->datacollector);
                    double* residual_norm_1_0 = (double*)get_last_residuals_norm_dc(calibrator_1_0->datacollector);

                    int check = 1;
                    if (*iteration_mean_0_1 < *iteration_mean_1_0) {
                        check = 0;
                    } else if ((*iteration_mean_0_1 - *iteration_mean_1_0) < EPSILON) {
                        if (*iteration_std_0_1 < *iteration_std_1_0) {
                            check = 0;
                        } else if ((*iteration_std_0_1 - *iteration_std_1_0) < EPSILON) {
                            if (*residual_norm_0_1 < *residual_norm_1_0) {
                                check = 0;
                            }
                        }
                    }
                    
                    //all information extracted will be freed with the calibrator
                    struct array* iterations = NULL;
                    struct JaggedArray* all_gains = NULL;
                    struct JaggedArray* damping_g = NULL;
                    struct JaggedArray* damping_g_old = NULL; 
                    struct JaggedArray* last_gains = NULL;
                    struct JaggedArray* last_residuals = NULL;     

                    if (check) {
                        iterations = get_iterations_dc(calibrator_1_0->datacollector);
                        all_gains = get_all_gain_dc(calibrator_1_0->datacollector);
                        damping_g = get_damping_g_dc(calibrator_1_0->datacollector);
                        damping_g_old = get_damping_g_old_dc(calibrator_1_0->datacollector); 
                        last_gains = get_last_gains_dc(calibrator_1_0->datacollector);
                        last_residuals = get_last_residual_dc(calibrator_1_0->datacollector); 
                    } else {
                        iterations = get_iterations_dc(calibrator_0_1->datacollector);
                        all_gains = get_all_gain_dc(calibrator_0_1->datacollector);
                        damping_g = get_damping_g_dc(calibrator_0_1->datacollector);
                        damping_g_old = get_damping_g_old_dc(calibrator_0_1->datacollector); 
                        last_gains = get_last_gains_dc(calibrator_0_1->datacollector);
                        last_residuals = get_last_residual_dc(calibrator_0_1->datacollector); 
                    }

                    char directory_save[256];
                    //printf("Saving to Directory\n");
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_non_adaptive_l1_l2_original/data_order_%ld/data_lm_non_adaptive_l1_l2_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");

                    free(residual_norm_1_0);
                    free(iteration_std_1_0);
                    free(iteration_mean_1_0);

                    free(residual_norm_0_1);
                    free(iteration_std_0_1);
                    free(iteration_mean_0_1);

                    free_ja(all_gains);

                    free_c(calibrator_0_1);
                    free_c(calibrator_1_0);
                    free_s(simulator);
                }
            }
        }
    }
}

void extraction_optimal_lm_adaptive(size_t* order_min, size_t* order_max, char* directory) {
    int pso_seeds[] = {49052764, 12039846 ,32609861, 18796345};
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};
    int num_workers = workers(28);

    //order
    for (size_t order = *order_min; order <= *order_max; order++) {
        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    //This is for the pso_seeds
                    //Compare performance for each pso_seed number and take the best performing.
                    //Run and save: 
                    //  -iterations for all time steps
                    //  -last residual norm for all time steps
                    //  -last gain for each time step
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_l1 = INFINITY;
                    double best_l2 = INFINITY;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[256];
                        sprintf(path_doubles,"%s/data/data_order_%ld/data_pso_lm_adaptive_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array", directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);

                        char path_positions[256];
                        sprintf(path_positions,"%s/data/data_order_%ld/data_pso_lm_adaptive_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array", directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_l1 = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double position_l2 = *(double*)get_element_a(positions_array, (size_t[]){1});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_l1 = position_l1;
                            best_l2 = position_l2;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_l1 = position_l1;
                                best_l2 = position_l2;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_l1 = position_l1;
                                    best_l2 = position_l2;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    //Need to run the specific hyperparameters
                    char simulator_path [256];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig", directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    solve_G_LM(calibrator, &(size_t){IMAX}, &(double){TAU}, &(double){best_l1}, &(double){best_l2});

                    //get the data needed for graphing
                    //all information extracted will be freed with the calibrator
                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector); 
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);     
                    
                    char directory_save[256];
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_adaptive_optimal/data_order_%ld/data_lm_adaptive_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }

}

void extraction_optimal_lm_adaptive_REDUCED_TIMESTEPS(size_t* order_min, size_t* order_max, char* directory) {
    int pso_seeds[] = {49052764, 12039846 ,32609861, 18796345};
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};
    int num_workers = workers(28);

    //order
    for (size_t order = *order_min; order <= *order_max; order++) {
        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    //This is for the pso_seeds
                    //Compare performance for each pso_seed number and take the best performing.
                    //Run and save: 
                    //  -iterations for all time steps
                    //  -last residual norm for all time steps
                    //  -last gain for each time step
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_l1 = INFINITY;
                    double best_l2 = INFINITY;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[256];
                        sprintf(path_doubles,"%s/data/data_order_%ld/data_pso_lm_adaptive_REDUCED_TIMESTEPS_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array", directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);

                        char path_positions[256];
                        sprintf(path_positions,"%s/data/data_order_%ld/data_pso_lm_adaptive_REDUCED_TIMESTEPS_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array", directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_l1 = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double position_l2 = *(double*)get_element_a(positions_array, (size_t[]){1});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_l1 = position_l1;
                            best_l2 = position_l2;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_l1 = position_l1;
                                best_l2 = position_l2;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_l1 = position_l1;
                                    best_l2 = position_l2;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    //Need to run the specific hyperparameters
                    char simulator_path [256];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig", directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    solve_G_LM(calibrator, &(size_t){IMAX}, &(double){TAU}, &(double){best_l1}, &(double){best_l2});

                    //get the data needed for graphing
                    //all information extracted will be freed with the calibrator
                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector); 
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);     
                    
                    char directory_save[256];
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_adaptive_optimal/data_order_%ld/data_lm_adaptive_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }

}

void extraction_original_lm_adaptive(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};
    int num_workers = workers(28);

    //order
    for (size_t order = *order_min; order <= *order_max; order++) {
        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char simulator_path [256];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig", directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    solve_G_LM(calibrator, &(size_t){IMAX}, &(double){TAU}, &(double){1}, &(double){1.1});
                    double* iteration_mean = (double*)get_average_iterations_dc(calibrator->datacollector);
                    double* iteration_std = (double*)get_standard_deviation_iterations_dc(calibrator->datacollector);
                    double* residual_norm = (double*)get_last_residuals_norm_dc(calibrator->datacollector);
                    
                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector); 
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);

                    char directory_save[256];
                    //printf("Saving to Directory\n");
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_adaptive_original/data_order_%ld/data_lm_adaptive_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");

                    free(residual_norm);
                    free(iteration_std);
                    free(iteration_mean);

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }
}

void create_x_y_points_from_optimal_stefcal(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    for (size_t order = *order_min; order <= *order_max; order++) {

        double sum_iterations = 0;
        double sum_residual_norm = 0;
        size_t count_iterations = 0;
        size_t count_residual_norm = 0;

        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/stefcal_optimal/data_order_%ld/data_stefcal_lambda_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);
                    char directory_all_gains_jarray[500];
                    sprintf(directory_all_gains_jarray, "%s/all_gains.jarray",directory_save);
                    char directory_damping_g_jarray[500];
                    sprintf(directory_damping_g_jarray, "%s/damping_g.jarray",directory_save);
                    char directory_damping_g_old_jarray[500];
                    sprintf(directory_damping_g_old_jarray, "%s/damping_g_old.jarray",directory_save);
                    char directory_last_gains_jarray[500];
                    sprintf(directory_last_gains_jarray, "%s/last_gains.jarray",directory_save);
                    char directory_last_residuals_jarray[500];
                    sprintf(directory_last_residuals_jarray, "%s/last_residuals.jarray",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    struct JaggedArray* last_residuals_jarray = read_from_file_ja(directory_last_residuals_jarray);

                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_iterations += *element;

                        count_iterations += 1;
                    }

                    for (size_t y = 0; y < 600; y++) {
                        struct array* residual_array = get_ja(last_residuals_jarray, (size_t[]){y});
                        double* residual_norm = norm_a(residual_array);
                        sum_residual_norm += *residual_norm;

                        free(residual_norm);
                        count_residual_norm += 1;
                    }

                    free_a(iterations_array);
                    free_ja(last_residuals_jarray);
                }
            }
        }

        //calculate the mean residual norm and mean iterations
        double mean_iterations = sum_iterations / count_iterations;
        double mean_last_residual_norm = sum_residual_norm / count_residual_norm;

        double sum_of_squared_deviation = 0;
        double sum_of_absolute_deviation = 0;

        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/stefcal_optimal/data_order_%ld/data_stefcal_lambda_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_of_squared_deviation += pow((*element - mean_iterations), 2.0);
                        sum_of_absolute_deviation += fabs(*element - mean_iterations);
                    }

                    free_a(iterations_array);
                }
            }
        }

        double std_iterations = sqrt(sum_of_squared_deviation/count_iterations);
        double mad_iterations = sum_of_absolute_deviation/count_iterations;

        printf("Order = %ld\n", order);
        printf("Mean = %f\n", mean_iterations);
        printf("Std = %f\n", std_iterations);
        printf("MAD = %f\n", mad_iterations);
        // printf("Residual Norm = %f\n", *residual_norm);
        printf("Mean Last Residual Norm = %f\n", mean_last_residual_norm);
        printf("\n");

        char directory_ursi[200];
        sprintf(directory_ursi,"%s/data_optimal_solutions_extracted_data_points/stefcal_optimal/data_order_%ld", directory, order);

        create_directories(directory_ursi);

        char directory_ursi_average_iterations[300];
        sprintf(directory_ursi_average_iterations,"%s/iterations_mean.txt", directory_ursi);
        char directory_ursi_iteration_std[300];
        sprintf(directory_ursi_iteration_std,"%s/iterations_std.txt", directory_ursi);
        char directory_ursi_iteration_mad[300];
        sprintf(directory_ursi_iteration_mad,"%s/iterations_mad.txt", directory_ursi);
        char directory_ursi_mean_last_residual_norm[300];
        sprintf(directory_ursi_mean_last_residual_norm,"%s/mean_last_residual_norm.txt", directory_ursi);

        FILE *fp1 = open_output(directory_ursi_average_iterations, "w");
        if (fp1 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_average_iterations);
            return;
        }
        fprintf(fp1, "%f\n", mean_iterations);
        fclose(fp1);

        FILE *fp2 = open_output(directory_ursi_iteration_std, "w");
        if (fp2 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_std);
            return;
        }
        fprintf(fp2, "%f\n", std_iterations);
        fclose(fp2);

        FILE *fp3 = open_output(directory_ursi_iteration_mad, "w");
        if (fp3 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_mad);
            return;
        }
        fprintf(fp3, "%f\n", mad_iterations);
        fclose(fp3);

        FILE *fp4 = open_output(directory_ursi_mean_last_residual_norm, "w");
        if (fp4 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_mean_last_residual_norm);
            return;
        }
        fprintf(fp4, "%f\n", mean_last_residual_norm);
        fclose(fp4);
    }
}

void create_x_y_points_from_original_stefcal(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    for (size_t order = *order_min; order <= *order_max; order++) {

        double sum_iterations = 0;
        double sum_residual_norm = 0;
        size_t count_iterations = 0;
        size_t count_residual_norm = 0;

        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/stefcal_original/data_order_%ld/data_stefcal_lambda_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);
                    char directory_all_gains_jarray[500];
                    sprintf(directory_all_gains_jarray, "%s/all_gains.jarray",directory_save);
                    char directory_damping_g_jarray[500];
                    sprintf(directory_damping_g_jarray, "%s/damping_g.jarray",directory_save);
                    char directory_damping_g_old_jarray[500];
                    sprintf(directory_damping_g_old_jarray, "%s/damping_g_old.jarray",directory_save);
                    char directory_last_gains_jarray[500];
                    sprintf(directory_last_gains_jarray, "%s/last_gains.jarray",directory_save);
                    char directory_last_residuals_jarray[500];
                    sprintf(directory_last_residuals_jarray, "%s/last_residuals.jarray",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    struct JaggedArray* last_residuals_jarray = read_from_file_ja(directory_last_residuals_jarray);

                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_iterations += *element;

                        count_iterations += 1;
                    }

                    for (size_t y = 0; y < 600; y++) {
                        struct array* residual_array = get_ja(last_residuals_jarray, (size_t[]){y});
                        double* residual_norm = norm_a(residual_array);
                        sum_residual_norm += *residual_norm;

                        free(residual_norm);
                        count_residual_norm += 1;
                    }

                    free_a(iterations_array);
                    free_ja(last_residuals_jarray);
                }
            }
        }

        //calculate the mean residual norm and mean iterations
        double mean_iterations = sum_iterations / count_iterations;
        double mean_last_residual_norm = sum_residual_norm / count_residual_norm;

        double sum_of_squared_deviation = 0;
        double sum_of_absolute_deviation = 0;

        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/stefcal_original/data_order_%ld/data_stefcal_lambda_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_of_squared_deviation += pow((*element - mean_iterations), 2.0);
                        sum_of_absolute_deviation += fabs(*element - mean_iterations);
                    }

                    free_a(iterations_array);
                }
            }
        }

        double std_iterations = sqrt(sum_of_squared_deviation/count_iterations);
        double mad_iterations = sum_of_absolute_deviation/count_iterations;

        printf("Order = %ld\n", order);
        printf("Mean = %f\n", mean_iterations);
        printf("Std = %f\n", std_iterations);
        printf("MAD = %f\n", mad_iterations);
        // printf("Residual Norm = %f\n", *residual_norm);
        printf("Mean Last Residual Norm = %f\n", mean_last_residual_norm);
        printf("\n");

        char directory_ursi[200];
        sprintf(directory_ursi,"%s/data_optimal_solutions_extracted_data_points/stefcal_original/data_order_%ld", directory, order);

        create_directories(directory_ursi);

        char directory_ursi_average_iterations[300];
        sprintf(directory_ursi_average_iterations,"%s/iterations_mean.txt", directory_ursi);
        char directory_ursi_iteration_std[300];
        sprintf(directory_ursi_iteration_std,"%s/iterations_std.txt", directory_ursi);
        char directory_ursi_iteration_mad[300];
        sprintf(directory_ursi_iteration_mad,"%s/iterations_mad.txt", directory_ursi);
        char directory_ursi_mean_last_residual_norm[300];
        sprintf(directory_ursi_mean_last_residual_norm,"%s/mean_last_residual_norm.txt", directory_ursi);

        FILE *fp1 = open_output(directory_ursi_average_iterations, "w");
        if (fp1 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_average_iterations);
            return;
        }
        fprintf(fp1, "%f\n", mean_iterations);
        fclose(fp1);

        FILE *fp2 = open_output(directory_ursi_iteration_std, "w");
        if (fp2 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_std);
            return;
        }
        fprintf(fp2, "%f\n", std_iterations);
        fclose(fp2);

        FILE *fp3 = open_output(directory_ursi_iteration_mad, "w");
        if (fp3 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_mad);
            return;
        }
        fprintf(fp3, "%f\n", mad_iterations);
        fclose(fp3);

        FILE *fp4 = open_output(directory_ursi_mean_last_residual_norm, "w");
        if (fp4 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_mean_last_residual_norm);
            return;
        }
        fprintf(fp4, "%f\n", mean_last_residual_norm);
        fclose(fp4);
    }
}

void create_x_y_points_from_optimal_stefcal_improved(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    for (size_t order = *order_min; order <= *order_max; order++) {

        double sum_iterations = 0;
        double sum_residual_norm = 0;
        size_t count_iterations = 0;
        size_t count_residual_norm = 0;

        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/stefcal_improved_optimal/data_order_%ld/data_stefcal_improved_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);
                    char directory_all_gains_jarray[500];
                    sprintf(directory_all_gains_jarray, "%s/all_gains.jarray",directory_save);
                    char directory_damping_g_jarray[500];
                    sprintf(directory_damping_g_jarray, "%s/damping_g.jarray",directory_save);
                    char directory_damping_g_old_jarray[500];
                    sprintf(directory_damping_g_old_jarray, "%s/damping_g_old.jarray",directory_save);
                    char directory_last_gains_jarray[500];
                    sprintf(directory_last_gains_jarray, "%s/last_gains.jarray",directory_save);
                    char directory_last_residuals_jarray[500];
                    sprintf(directory_last_residuals_jarray, "%s/last_residuals.jarray",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    struct JaggedArray* last_residuals_jarray = read_from_file_ja(directory_last_residuals_jarray);

                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_iterations += *element;

                        count_iterations += 1;
                    }

                    for (size_t y = 0; y < 600; y++) {
                        struct array* residual_array = get_ja(last_residuals_jarray, (size_t[]){y});
                        double* residual_norm = norm_a(residual_array);
                        sum_residual_norm += *residual_norm;

                        free(residual_norm);
                        count_residual_norm += 1;
                    }

                    free_a(iterations_array);
                    free_ja(last_residuals_jarray);
                }
            }
        }

        //calculate the mean residual norm and mean iterations
        double mean_iterations = sum_iterations / count_iterations;
        double mean_last_residual_norm = sum_residual_norm / count_residual_norm;

        double sum_of_squared_deviation = 0;
        double sum_of_absolute_deviation = 0;

        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/stefcal_improved_optimal/data_order_%ld/data_stefcal_improved_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_of_squared_deviation += pow((*element - mean_iterations), 2.0);
                        sum_of_absolute_deviation += fabs(*element - mean_iterations);
                    }

                    free_a(iterations_array);
                }
            }
        }

        double std_iterations = sqrt(sum_of_squared_deviation/count_iterations);
        double mad_iterations = sum_of_absolute_deviation/count_iterations;

        printf("Order = %ld\n", order);
        printf("Mean = %f\n", mean_iterations);
        printf("Std = %f\n", std_iterations);
        printf("MAD = %f\n", mad_iterations);
        // printf("Residual Norm = %f\n", *residual_norm);
        printf("Mean Last Residual Norm = %f\n", mean_last_residual_norm);
        printf("\n");

        char directory_ursi[200];
        sprintf(directory_ursi,"%s/data_optimal_solutions_extracted_data_points/stefcal_improved_optimal/data_order_%ld", directory, order);

        create_directories(directory_ursi);

        char directory_ursi_average_iterations[300];
        sprintf(directory_ursi_average_iterations,"%s/iterations_mean.txt", directory_ursi);
        char directory_ursi_iteration_std[300];
        sprintf(directory_ursi_iteration_std,"%s/iterations_std.txt", directory_ursi);
        char directory_ursi_iteration_mad[300];
        sprintf(directory_ursi_iteration_mad,"%s/iterations_mad.txt", directory_ursi);
        char directory_ursi_mean_last_residual_norm[300];
        sprintf(directory_ursi_mean_last_residual_norm,"%s/mean_last_residual_norm.txt", directory_ursi);

        FILE *fp1 = open_output(directory_ursi_average_iterations, "w");
        if (fp1 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_average_iterations);
            return;
        }
        fprintf(fp1, "%f\n", mean_iterations);
        fclose(fp1);

        FILE *fp2 = open_output(directory_ursi_iteration_std, "w");
        if (fp2 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_std);
            return;
        }
        fprintf(fp2, "%f\n", std_iterations);
        fclose(fp2);

        FILE *fp3 = open_output(directory_ursi_iteration_mad, "w");
        if (fp3 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_mad);
            return;
        }
        fprintf(fp3, "%f\n", mad_iterations);
        fclose(fp3);

        FILE *fp4 = open_output(directory_ursi_mean_last_residual_norm, "w");
        if (fp4 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_mean_last_residual_norm);
            return;
        }
        fprintf(fp4, "%f\n", mean_last_residual_norm);
        fclose(fp4);
    }
}

void create_x_y_points_from_original_stefcal_improved(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    for (size_t order = *order_min; order <= *order_max; order++) {

        double sum_iterations = 0;
        double sum_residual_norm = 0;
        size_t count_iterations = 0;
        size_t count_residual_norm = 0;

        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/stefcal_improved_original/data_order_%ld/data_stefcal_improved_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);
                    char directory_all_gains_jarray[500];
                    sprintf(directory_all_gains_jarray, "%s/all_gains.jarray",directory_save);
                    char directory_damping_g_jarray[500];
                    sprintf(directory_damping_g_jarray, "%s/damping_g.jarray",directory_save);
                    char directory_damping_g_old_jarray[500];
                    sprintf(directory_damping_g_old_jarray, "%s/damping_g_old.jarray",directory_save);
                    char directory_last_gains_jarray[500];
                    sprintf(directory_last_gains_jarray, "%s/last_gains.jarray",directory_save);
                    char directory_last_residuals_jarray[500];
                    sprintf(directory_last_residuals_jarray, "%s/last_residuals.jarray",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    struct JaggedArray* last_residuals_jarray = read_from_file_ja(directory_last_residuals_jarray);

                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_iterations += *element;

                        count_iterations += 1;
                    }

                    for (size_t y = 0; y < 600; y++) {
                        struct array* residual_array = get_ja(last_residuals_jarray, (size_t[]){y});
                        double* residual_norm = norm_a(residual_array);
                        sum_residual_norm += *residual_norm;

                        free(residual_norm);
                        count_residual_norm += 1;
                    }

                    free_a(iterations_array);
                    free_ja(last_residuals_jarray);
                }
            }
        }

        //calculate the mean residual norm and mean iterations
        double mean_iterations = sum_iterations / count_iterations;
        double mean_last_residual_norm = sum_residual_norm / count_residual_norm;

        double sum_of_squared_deviation = 0;
        double sum_of_absolute_deviation = 0;

        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/stefcal_improved_original/data_order_%ld/data_stefcal_improved_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_of_squared_deviation += pow((*element - mean_iterations), 2.0);
                        sum_of_absolute_deviation += fabs(*element - mean_iterations);
                    }

                    free_a(iterations_array);
                }
            }
        }

        double std_iterations = sqrt(sum_of_squared_deviation/count_iterations);
        double mad_iterations = sum_of_absolute_deviation/count_iterations;

        printf("Order = %ld\n", order);
        printf("Mean = %f\n", mean_iterations);
        printf("Std = %f\n", std_iterations);
        printf("MAD = %f\n", mad_iterations);
        // printf("Residual Norm = %f\n", *residual_norm);
        printf("Mean Last Residual Norm = %f\n", mean_last_residual_norm);
        printf("\n");

        char directory_ursi[200];
        sprintf(directory_ursi,"%s/data_optimal_solutions_extracted_data_points/stefcal_improved_original/data_order_%ld", directory, order);

        create_directories(directory_ursi);

        char directory_ursi_average_iterations[300];
        sprintf(directory_ursi_average_iterations,"%s/iterations_mean.txt", directory_ursi);
        char directory_ursi_iteration_std[300];
        sprintf(directory_ursi_iteration_std,"%s/iterations_std.txt", directory_ursi);
        char directory_ursi_iteration_mad[300];
        sprintf(directory_ursi_iteration_mad,"%s/iterations_mad.txt", directory_ursi);
        char directory_ursi_mean_last_residual_norm[300];
        sprintf(directory_ursi_mean_last_residual_norm,"%s/mean_last_residual_norm.txt", directory_ursi);

        FILE *fp1 = open_output(directory_ursi_average_iterations, "w");
        if (fp1 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_average_iterations);
            return;
        }
        fprintf(fp1, "%f\n", mean_iterations);
        fclose(fp1);

        FILE *fp2 = open_output(directory_ursi_iteration_std, "w");
        if (fp2 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_std);
            return;
        }
        fprintf(fp2, "%f\n", std_iterations);
        fclose(fp2);

        FILE *fp3 = open_output(directory_ursi_iteration_mad, "w");
        if (fp3 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_mad);
            return;
        }
        fprintf(fp3, "%f\n", mad_iterations);
        fclose(fp3);

        FILE *fp4 = open_output(directory_ursi_mean_last_residual_norm, "w");
        if (fp4 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_mean_last_residual_norm);
            return;
        }
        fprintf(fp4, "%f\n", mean_last_residual_norm);
        fclose(fp4);
    }
}

void create_x_y_points_from_optimal_lm_non_adaptive_l1_l2(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    for (size_t order = *order_min; order <= *order_max; order++) {

        double sum_iterations = 0;
        double sum_residual_norm = 0;
        size_t count_iterations = 0;
        size_t count_residual_norm = 0;

        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_non_adaptive_l1_l2_optimal/data_order_%ld/data_lm_non_adaptive_l1_l2_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);
                    char directory_all_gains_jarray[500];
                    sprintf(directory_all_gains_jarray, "%s/all_gains.jarray",directory_save);
                    char directory_damping_g_jarray[500];
                    sprintf(directory_damping_g_jarray, "%s/damping_g.jarray",directory_save);
                    char directory_damping_g_old_jarray[500];
                    sprintf(directory_damping_g_old_jarray, "%s/damping_g_old.jarray",directory_save);
                    char directory_last_gains_jarray[500];
                    sprintf(directory_last_gains_jarray, "%s/last_gains.jarray",directory_save);
                    char directory_last_residuals_jarray[500];
                    sprintf(directory_last_residuals_jarray, "%s/last_residuals.jarray",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    struct JaggedArray* last_residuals_jarray = read_from_file_ja(directory_last_residuals_jarray);

                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_iterations += *element;

                        count_iterations += 1;
                    }

                    for (size_t y = 0; y < 600; y++) {
                        struct array* residual_array = get_ja(last_residuals_jarray, (size_t[]){y});
                        double* residual_norm = norm_a(residual_array);
                        sum_residual_norm += *residual_norm;

                        free(residual_norm);
                        count_residual_norm += 1;
                    }

                    free_a(iterations_array);
                    free_ja(last_residuals_jarray);
                }
            }
        }

        //calculate the mean residual norm and mean iterations
        double mean_iterations = sum_iterations / count_iterations;
        double mean_last_residual_norm = sum_residual_norm / count_residual_norm;

        double sum_of_squared_deviation = 0;
        double sum_of_absolute_deviation = 0;

        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_non_adaptive_l1_l2_optimal/data_order_%ld/data_lm_non_adaptive_l1_l2_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_of_squared_deviation += pow((*element - mean_iterations), 2.0);
                        sum_of_absolute_deviation += fabs(*element - mean_iterations);
                    }

                    free_a(iterations_array);
                }
            }
        }

        double std_iterations = sqrt(sum_of_squared_deviation/count_iterations);
        double mad_iterations = sum_of_absolute_deviation/count_iterations;

        printf("Order = %ld\n", order);
        printf("Mean = %f\n", mean_iterations);
        printf("Std = %f\n", std_iterations);
        printf("MAD = %f\n", mad_iterations);
        // printf("Residual Norm = %f\n", *residual_norm);
        printf("Mean Last Residual Norm = %f\n", mean_last_residual_norm);
        printf("\n");

        char directory_ursi[200];
        sprintf(directory_ursi,"%s/data_optimal_solutions_extracted_data_points/lm_non_adaptive_l1_l2_optimal/data_order_%ld", directory, order);

        create_directories(directory_ursi);

        char directory_ursi_average_iterations[300];
        sprintf(directory_ursi_average_iterations,"%s/iterations_mean.txt", directory_ursi);
        char directory_ursi_iteration_std[300];
        sprintf(directory_ursi_iteration_std,"%s/iterations_std.txt", directory_ursi);
        char directory_ursi_iteration_mad[300];
        sprintf(directory_ursi_iteration_mad,"%s/iterations_mad.txt", directory_ursi);
        char directory_ursi_mean_last_residual_norm[300];
        sprintf(directory_ursi_mean_last_residual_norm,"%s/mean_last_residual_norm_check.txt", directory_ursi);

        FILE *fp1 = open_output(directory_ursi_average_iterations, "w");
        if (fp1 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_average_iterations);
            return;
        }
        fprintf(fp1, "%f\n", mean_iterations);
        fclose(fp1);

        FILE *fp2 = open_output(directory_ursi_iteration_std, "w");
        if (fp2 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_std);
            return;
        }
        fprintf(fp2, "%f\n", std_iterations);
        fclose(fp2);

        FILE *fp3 = open_output(directory_ursi_iteration_mad, "w");
        if (fp3 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_mad);
            return;
        }
        fprintf(fp3, "%f\n", mad_iterations);
        fclose(fp3);

        FILE *fp4 = open_output(directory_ursi_mean_last_residual_norm, "w");
        if (fp4 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_mean_last_residual_norm);
            return;
        }
        fprintf(fp4, "%f\n", mean_last_residual_norm);
        fclose(fp4);
    }
}

void create_x_y_points_from_original_lm_non_adaptive_l1_l2(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    for (size_t order = *order_min; order <= *order_max; order++) {

        double sum_iterations = 0;
        double sum_residual_norm = 0;
        size_t count_iterations = 0;
        size_t count_residual_norm = 0;

        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_non_adaptive_l1_l2_original/data_order_%ld/data_lm_non_adaptive_l1_l2_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);
                    char directory_all_gains_jarray[500];
                    sprintf(directory_all_gains_jarray, "%s/all_gains.jarray",directory_save);
                    char directory_damping_g_jarray[500];
                    sprintf(directory_damping_g_jarray, "%s/damping_g.jarray",directory_save);
                    char directory_damping_g_old_jarray[500];
                    sprintf(directory_damping_g_old_jarray, "%s/damping_g_old.jarray",directory_save);
                    char directory_last_gains_jarray[500];
                    sprintf(directory_last_gains_jarray, "%s/last_gains.jarray",directory_save);
                    char directory_last_residuals_jarray[500];
                    sprintf(directory_last_residuals_jarray, "%s/last_residuals.jarray",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    struct JaggedArray* last_residuals_jarray = read_from_file_ja(directory_last_residuals_jarray);

                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_iterations += *element;

                        count_iterations += 1;
                    }

                    for (size_t y = 0; y < 600; y++) {
                        struct array* residual_array = get_ja(last_residuals_jarray, (size_t[]){y});
                        double* residual_norm = norm_a(residual_array);
                        sum_residual_norm += *residual_norm;

                        free(residual_norm);
                        count_residual_norm += 1;
                    }

                    free_a(iterations_array);
                    free_ja(last_residuals_jarray);
                }
            }
        }

        //calculate the mean residual norm and mean iterations
        double mean_iterations = sum_iterations / count_iterations;
        double mean_last_residual_norm = sum_residual_norm / count_residual_norm;

        double sum_of_squared_deviation = 0;
        double sum_of_absolute_deviation = 0;

        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_non_adaptive_l1_l2_original/data_order_%ld/data_lm_non_adaptive_l1_l2_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_of_squared_deviation += pow((*element - mean_iterations), 2.0);
                        sum_of_absolute_deviation += fabs(*element - mean_iterations);
                    }

                    free_a(iterations_array);
                }
            }
        }

        double std_iterations = sqrt(sum_of_squared_deviation/count_iterations);
        double mad_iterations = sum_of_absolute_deviation/count_iterations;

        printf("Order = %ld\n", order);
        printf("Mean = %f\n", mean_iterations);
        printf("Std = %f\n", std_iterations);
        printf("MAD = %f\n", mad_iterations);
        // printf("Residual Norm = %f\n", *residual_norm);
        printf("Mean Last Residual Norm = %f\n", mean_last_residual_norm);
        printf("\n");

        char directory_ursi[200];
        sprintf(directory_ursi,"%s/data_optimal_solutions_extracted_data_points/lm_non_adaptive_l1_l2_original/data_order_%ld", directory, order);

        create_directories(directory_ursi);

        char directory_ursi_average_iterations[300];
        sprintf(directory_ursi_average_iterations,"%s/iterations_mean.txt", directory_ursi);
        char directory_ursi_iteration_std[300];
        sprintf(directory_ursi_iteration_std,"%s/iterations_std.txt", directory_ursi);
        char directory_ursi_iteration_mad[300];
        sprintf(directory_ursi_iteration_mad,"%s/iterations_mad.txt", directory_ursi);
        char directory_ursi_mean_last_residual_norm[300];
        sprintf(directory_ursi_mean_last_residual_norm,"%s/mean_last_residual_norm_check.txt", directory_ursi);

        FILE *fp1 = open_output(directory_ursi_average_iterations, "w");
        if (fp1 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_average_iterations);
            return;
        }
        fprintf(fp1, "%f\n", mean_iterations);
        fclose(fp1);

        FILE *fp2 = open_output(directory_ursi_iteration_std, "w");
        if (fp2 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_std);
            return;
        }
        fprintf(fp2, "%f\n", std_iterations);
        fclose(fp2);

        FILE *fp3 = open_output(directory_ursi_iteration_mad, "w");
        if (fp3 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_mad);
            return;
        }
        fprintf(fp3, "%f\n", mad_iterations);
        fclose(fp3);

        FILE *fp4 = open_output(directory_ursi_mean_last_residual_norm, "w");
        if (fp4 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_mean_last_residual_norm);
            return;
        }
        fprintf(fp4, "%f\n", mean_last_residual_norm);
        fclose(fp4);
    }
}

void create_x_y_points_from_optimal_lm_adaptive(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    for (size_t order = *order_min; order <= *order_max; order++) {

        double sum_iterations = 0;
        double sum_residual_norm = 0;
        size_t count_iterations = 0;
        size_t count_residual_norm = 0;

        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_adaptive_optimal/data_order_%ld/data_lm_adaptive_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);
                    char directory_all_gains_jarray[500];
                    sprintf(directory_all_gains_jarray, "%s/all_gains.jarray",directory_save);
                    char directory_damping_g_jarray[500];
                    sprintf(directory_damping_g_jarray, "%s/damping_g.jarray",directory_save);
                    char directory_damping_g_old_jarray[500];
                    sprintf(directory_damping_g_old_jarray, "%s/damping_g_old.jarray",directory_save);
                    char directory_last_gains_jarray[500];
                    sprintf(directory_last_gains_jarray, "%s/last_gains.jarray",directory_save);
                    char directory_last_residuals_jarray[500];
                    sprintf(directory_last_residuals_jarray, "%s/last_residuals.jarray",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    struct JaggedArray* last_residuals_jarray = read_from_file_ja(directory_last_residuals_jarray);

                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_iterations += *element;

                        count_iterations += 1;
                    }

                    for (size_t y = 0; y < 600; y++) {
                        struct array* residual_array = get_ja(last_residuals_jarray, (size_t[]){y});
                        double* residual_norm = norm_a(residual_array);
                        sum_residual_norm += *residual_norm;

                        free(residual_norm);
                        count_residual_norm += 1;
                    }

                    free_a(iterations_array);
                    free_ja(last_residuals_jarray);
                }
            }
        }

        //calculate the mean residual norm and mean iterations
        double mean_iterations = sum_iterations / count_iterations;
        double mean_last_residual_norm = sum_residual_norm / count_residual_norm;

        double sum_of_squared_deviation = 0;
        double sum_of_absolute_deviation = 0;

        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_adaptive_optimal/data_order_%ld/data_lm_adaptive_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_of_squared_deviation += pow((*element - mean_iterations), 2.0);
                        sum_of_absolute_deviation += fabs(*element - mean_iterations);
                    }

                    free_a(iterations_array);
                }
            }
        }

        double std_iterations = sqrt(sum_of_squared_deviation/count_iterations);
        double mad_iterations = sum_of_absolute_deviation/count_iterations;

        printf("Order = %ld\n", order);
        printf("Mean = %f\n", mean_iterations);
        printf("Std = %f\n", std_iterations);
        printf("MAD = %f\n", mad_iterations);
        // printf("Residual Norm = %f\n", *residual_norm);
        printf("Mean Last Residual Norm = %f\n", mean_last_residual_norm);
        printf("\n");

        char directory_ursi[200];
        sprintf(directory_ursi,"%s/data_optimal_solutions_extracted_data_points/lm_adaptive_optimal/data_order_%ld", directory, order);

        create_directories(directory_ursi);

        char directory_ursi_average_iterations[300];
        sprintf(directory_ursi_average_iterations,"%s/iterations_mean.txt", directory_ursi);
        char directory_ursi_iteration_std[300];
        sprintf(directory_ursi_iteration_std,"%s/iterations_std.txt", directory_ursi);
        char directory_ursi_iteration_mad[300];
        sprintf(directory_ursi_iteration_mad,"%s/iterations_mad.txt", directory_ursi);
        char directory_ursi_mean_last_residual_norm[300];
        sprintf(directory_ursi_mean_last_residual_norm,"%s/mean_last_residual_norm.txt", directory_ursi);

        FILE *fp1 = open_output(directory_ursi_average_iterations, "w");
        if (fp1 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_average_iterations);
            return;
        }
        fprintf(fp1, "%f\n", mean_iterations);
        fclose(fp1);

        FILE *fp2 = open_output(directory_ursi_iteration_std, "w");
        if (fp2 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_std);
            return;
        }
        fprintf(fp2, "%f\n", std_iterations);
        fclose(fp2);

        FILE *fp3 = open_output(directory_ursi_iteration_mad, "w");
        if (fp3 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_mad);
            return;
        }
        fprintf(fp3, "%f\n", mad_iterations);
        fclose(fp3);

        FILE *fp4 = open_output(directory_ursi_mean_last_residual_norm, "w");
        if (fp4 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_mean_last_residual_norm);
            return;
        }
        fprintf(fp4, "%f\n", mean_last_residual_norm);
        fclose(fp4);
    }
}

void create_x_y_points_from_original_lm_adaptive(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    for (size_t order = *order_min; order <= *order_max; order++) {

        double sum_iterations = 0;
        double sum_residual_norm = 0;
        size_t count_iterations = 0;
        size_t count_residual_norm = 0;

        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_adaptive_original/data_order_%ld/data_lm_adaptive_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);
                    char directory_all_gains_jarray[500];
                    sprintf(directory_all_gains_jarray, "%s/all_gains.jarray",directory_save);
                    char directory_damping_g_jarray[500];
                    sprintf(directory_damping_g_jarray, "%s/damping_g.jarray",directory_save);
                    char directory_damping_g_old_jarray[500];
                    sprintf(directory_damping_g_old_jarray, "%s/damping_g_old.jarray",directory_save);
                    char directory_last_gains_jarray[500];
                    sprintf(directory_last_gains_jarray, "%s/last_gains.jarray",directory_save);
                    char directory_last_residuals_jarray[500];
                    sprintf(directory_last_residuals_jarray, "%s/last_residuals.jarray",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    struct JaggedArray* last_residuals_jarray = read_from_file_ja(directory_last_residuals_jarray);

                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_iterations += *element;

                        count_iterations += 1;
                    }

                    for (size_t y = 0; y < 600; y++) {
                        struct array* residual_array = get_ja(last_residuals_jarray, (size_t[]){y});
                        double* residual_norm = norm_a(residual_array);
                        sum_residual_norm += *residual_norm;

                        free(residual_norm);
                        count_residual_norm += 1;
                    }

                    free_a(iterations_array);
                    free_ja(last_residuals_jarray);
                }
            }
        }

        //calculate the mean residual norm and mean iterations
        double mean_iterations = sum_iterations / count_iterations;
        double mean_last_residual_norm = sum_residual_norm / count_residual_norm;

        double sum_of_squared_deviation = 0;
        double sum_of_absolute_deviation = 0;

        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[400];
                    sprintf(directory_save,"%s/data_optimal_solutions/lm_adaptive_original/data_order_%ld/data_lm_adaptive_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f", directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    
                    char directory_iterations_array[500];
                    sprintf(directory_iterations_array, "%s/iterations.array",directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_of_squared_deviation += pow((*element - mean_iterations), 2.0);
                        sum_of_absolute_deviation += fabs(*element - mean_iterations);
                    }

                    free_a(iterations_array);
                }
            }
        }

        double std_iterations = sqrt(sum_of_squared_deviation/count_iterations);
        double mad_iterations = sum_of_absolute_deviation/count_iterations;

        printf("Order = %ld\n", order);
        printf("Mean = %f\n", mean_iterations);
        printf("Std = %f\n", std_iterations);
        printf("MAD = %f\n", mad_iterations);
        // printf("Residual Norm = %f\n", *residual_norm);
        printf("Mean Last Residual Norm = %f\n", mean_last_residual_norm);
        printf("\n");

        char directory_ursi[200];
        sprintf(directory_ursi,"%s/data_optimal_solutions_extracted_data_points/lm_adaptive_original/data_order_%ld", directory, order);

        create_directories(directory_ursi);

        char directory_ursi_average_iterations[300];
        sprintf(directory_ursi_average_iterations,"%s/iterations_mean.txt", directory_ursi);
        char directory_ursi_iteration_std[300];
        sprintf(directory_ursi_iteration_std,"%s/iterations_std.txt", directory_ursi);
        char directory_ursi_iteration_mad[300];
        sprintf(directory_ursi_iteration_mad,"%s/iterations_mad.txt", directory_ursi);
        char directory_ursi_mean_last_residual_norm[300];
        sprintf(directory_ursi_mean_last_residual_norm,"%s/mean_last_residual_norm.txt", directory_ursi);

        FILE *fp1 = open_output(directory_ursi_average_iterations, "w");
        if (fp1 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_average_iterations);
            return;
        }
        fprintf(fp1, "%f\n", mean_iterations);
        fclose(fp1);

        FILE *fp2 = open_output(directory_ursi_iteration_std, "w");
        if (fp2 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_std);
            return;
        }
        fprintf(fp2, "%f\n", std_iterations);
        fclose(fp2);

        FILE *fp3 = open_output(directory_ursi_iteration_mad, "w");
        if (fp3 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_iteration_mad);
            return;
        }
        fprintf(fp3, "%f\n", mad_iterations);
        fclose(fp3);

        FILE *fp4 = open_output(directory_ursi_mean_last_residual_norm, "w");
        if (fp4 == NULL)
        {
            printf("Error opening the file %s", directory_ursi_mean_last_residual_norm);
            return;
        }
        fprintf(fp4, "%f\n", mean_last_residual_norm);
        fclose(fp4);
    }
}

void mapping_pso_lm_adaptive_local(char* directory, int* order_min, int* order_max) {

    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    
    enum AlgorithmType option = LEVENBERG_MARQUARDT;
    size_t num_dimensions = 2;
    struct array* position_bounds  = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                      0.001, 20,1.001,20
                                    });

    size_t iterations_max = pso_iterations(50);
    size_t num_particles = pso_particles(25);
    
    char simulators_directory[512];
    sprintf(simulators_directory, "%s/simulators", directory);
    size_t k = 5;
    int num_workers = workers(30);

    for (int order = *order_min; order <= *order_max; order++) {
        for (size_t i = 0; i < 4; i++) {
            char dd_directory[256];
            sprintf(dd_directory,"%s/data/data_order_%d",directory, order);
            printf("%s\n",dd_directory);

            int pso_seed = pso_seeds[i];

            char dd_name[256];
            sprintf(dd_name,"data_pso_lm_adaptive_order_%d_seed_%d",order, pso_seed);

            printf("%s\n",dd_name);

            struct PSO* pso = new_pso(&option, &num_dimensions, position_bounds, &iterations_max, & num_particles, &pso_seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers); 
            start_single_pso_local(pso);
            free_a(position_bounds);
            free_pso(pso);
        }
    }
}

void create_data_dictionary_pso_orders(char* directory, char* filename, int* order_min, int* order_max) {
    int pso_seeds[] = {49052764, 12039846 ,32609861, 18796345};
    for (int order = *order_min; order <= *order_max; order++) {
        printf("Order = %d\n", order);
        for (size_t i = 0; i < 4; i++) {
            int pso_seed = pso_seeds[i];
            char dd_directory[256];
            sprintf(dd_directory,"%s/data/data_order_%d",directory, order);
            create_directories(dd_directory);
            char dd_filename[256];
            sprintf(dd_filename,"data_pso_%s_order_%d_seed_%d",filename, order, pso_seed);
            struct DataDictionary* data_dictionary = new_save_dd(dd_filename, dd_directory);
            free_dd(data_dictionary);
        }
    }
}

void create_data_dictionary_orders(char* directory, char* filename, int* order_min, int* order_max) {
    for (int order = *order_min; order <= *order_max; order++) {
        char dd_directory[256];
        sprintf(dd_directory,"%s/data/data_order_%d",directory, order);
        create_directories(dd_directory);
        char dd_filename[256];
        sprintf(dd_filename,"data_%s_order_%d",filename, order);
        struct DataDictionary* data_dictionary = new_save_dd(dd_filename, dd_directory);
        free_dd(data_dictionary);
    }
}

void mapping_pso_local(char* directory, char* filename, enum AlgorithmType* algorithm, int* iteration, int* order_min, int* order_max) {

    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    
    // size_t num_dimensions = 3;
    // struct array* position_bounds  = new_set_a(&(size_t){1},
    //                                 (size_t[]){6}, 
    //                                 &(enum DataType){DOUBLE}, 
    //                                 (double[6]){
    //                                     0.00001,2,0.001,0.9999,0.001,0.9999
    //                                 });
    // size_t iterations_max = 50;
    // size_t num_particles = 100;

    // size_t num_dimensions = 2;
    // struct array* position_bounds  = new_set_a(&(size_t){1},
    //                                 (size_t[]){4}, 
    //                                 &(enum DataType){DOUBLE}, 
    //                                 (double[4]){
    //                                     0.0001, 20,1.0001,20
    //                                 });
    // size_t iterations_max = 50;
    // size_t num_particles = 25;

    size_t num_dimensions = 2;
    struct array* position_bounds  = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -0.4, 3,-0.4,3
                                    });
    size_t iterations_max = pso_iterations(50);
    size_t num_particles = pso_particles(25);

    char simulators_directory[256];
    sprintf(simulators_directory,"%s/simulators",directory);
    size_t k = 9;
    int num_workers = workers(7);

    for (int order = *order_min; order <= *order_max; order++) {
        char dd_directory[256];
        sprintf(dd_directory,"%s/data/data_order_%d",directory, order);
        printf("%s\n",dd_directory);

        int pso_seed = pso_seeds[*iteration];

        char dd_name[256];
        sprintf(dd_name,"data_pso_%s_order_%d_seed_%d",filename ,order, pso_seed);

        printf("%s\n",dd_name);

        struct PSO* pso = new_pso(algorithm, &num_dimensions, position_bounds, &iterations_max, & num_particles, &pso_seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers); 
        start_single_pso_local(pso);

        printf("DONE\n");
        free_pso(pso);
    }
    free_a(position_bounds);
}

void pso_extract_all_best_hyperparameters_save_to_jarray(char* directory, char* filename, int* order_from, int* order_to) {

    size_t number_orders = *order_to - *order_from + 1;
    struct JaggedArray* hyperparameters = new_ja(&number_orders);
    size_t hyperparameter_count = 0;
    
    for (int order = *order_from; order <= *order_to; order++) {
        size_t array_size = 0;
        int check_array_size_set = 0;
        size_t index_all = 0;

        printf("ORDER %d\n !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", order);

        struct array** all_p_best_l_1 = malloc(4 * sizeof(struct array*));
        struct array** all_p_best_l_2 = malloc(4 * sizeof(struct array*));
        struct array** all_p_best_mean = malloc(4 * sizeof(struct array*));
        struct array** all_p_best_std = malloc(4 * sizeof(struct array*));
        struct array** all_p_best_residual_norms = malloc(4 * sizeof(struct array*));

        //initializing
        for (size_t a = 0; a < 4; a++) {
            all_p_best_l_1[a] = NULL;
            all_p_best_l_2[a] = NULL;
            all_p_best_mean[a] = NULL;
            all_p_best_std[a] = NULL;
            all_p_best_residual_norms[a] = NULL;
        }

        int seeds[] = {49052764, 12039846 ,32609861, 18796345};
        int seed_checks[] = {0, 0, 0, 0};

        for (int seed = 0; seed < 4; seed++) {
            char dd_pso_name[256];
            sprintf(dd_pso_name, "data_pso_%s_order_%d_seed_%d", filename, order, seeds[seed]);

            char dd_pso_directory[256];
            sprintf(dd_pso_directory, "%s/data/data_order_%d", directory, order);

            //check if the data dictionary exist for this seed
            char full_path_name[600];
            sprintf(full_path_name, "%s/%s.ddconfig", dd_pso_directory, dd_pso_name);

            int full_path_name_exists = check_file_path(full_path_name);

            if (full_path_name_exists) {
                printf("File Path Exists: %s\n", full_path_name);
            } else {
                printf("File Path Does Not Exists: %s\n", full_path_name);
            }

            if (full_path_name_exists) {
                //File path exists, can continue to extract the data
                seed_checks[seed] = full_path_name_exists;

                struct DataDictionary* data_dictionary_pso = read_from_file_dd(dd_pso_directory, dd_pso_name);

                size_t p_best_count = 0;
                struct array* p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});
                struct array* p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});
                struct array* p_best_mean = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});
                struct array* p_best_std = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});
                struct array* p_best_residual_norms = zeros_a(&(size_t){1}, (size_t[]){*data_dictionary_pso->amount_items}, &(enum DataType){DOUBLE});

                if (!check_array_size_set) {
                    array_size = (size_t){*data_dictionary_pso->amount_items};
                    check_array_size_set = 1;
                }

                //Iterate through all the datacontainers that holds the best values for a specific mapping
                for (size_t i = 0; i < *data_dictionary_pso->amount_items; i++) {
                
                    char filepath_dcon[256];
                    sprintf(filepath_dcon, "%s/%s", data_dictionary_pso->directory, data_dictionary_pso->identity);
                    struct DataContainer* data_container = read_from_file_dcon(filepath_dcon, data_dictionary_pso->items[i]);

                    char filepath_positions[512];
                    sprintf(filepath_positions, "%s/%s/%s.array", filepath_dcon, data_container->identity, "positions");
                    char filepath_double_data[512];
                    sprintf(filepath_double_data, "%s/%s/%s.array", filepath_dcon, data_container->identity, "double_data");

                    struct array* positions = read_from_file_a(filepath_positions);
                    struct array* double_data = read_from_file_a(filepath_double_data);


                    double l1_check = *(double*)get_element_a(positions, (size_t[]){0});
                    double l2_check = *(double*)get_element_a(positions, (size_t[]){1});
                    double mean_check = *(double*)get_element_a(double_data, (size_t[]){0});
                    double std_check = *(double*)get_element_a(double_data, (size_t[]){1});
                    double residual_norm_check = *(double*)get_element_a(double_data, (size_t[]){2});

                    double d_min_to_0_1 = sqrt(pow((0 - l1_check),2) + pow((1 - l2_check),2));
                    double d_min_to_1_0 = sqrt(pow((1 - l1_check),2) + pow((0 - l2_check),2));
                
                    if (d_min_to_0_1 <= d_min_to_1_0) {
                        //Add to p_left
                        set_element_a(p_best_l_1, (size_t[]){p_best_count}, &(double){l1_check});
                        set_element_a(p_best_l_2, (size_t[]){p_best_count}, &(double){l2_check});
                        set_element_a(p_best_mean, (size_t[]){p_best_count}, &(double){mean_check});
                        set_element_a(p_best_std, (size_t[]){p_best_count}, &(double){std_check});
                        set_element_a(p_best_residual_norms, (size_t[]){p_best_count}, &(double){residual_norm_check});
                        //increment p_left_count
                        p_best_count += 1;

                    } else {
                        //Switching l1 and l2 because of the symmetry
                        //Add to p_right
                        set_element_a(p_best_l_1, (size_t[]){p_best_count}, &(double){l1_check});
                        set_element_a(p_best_l_2, (size_t[]){p_best_count}, &(double){l2_check});
                        set_element_a(p_best_mean, (size_t[]){p_best_count}, &(double){mean_check});
                        set_element_a(p_best_std, (size_t[]){p_best_count}, &(double){std_check});
                        set_element_a(p_best_residual_norms, (size_t[]){p_best_count}, &(double){residual_norm_check});
                        //increment p_right_count
                        p_best_count += 1;
                    }
                    
                    free_a(positions);
                    free_a(double_data);

                    free_dcon(data_container);
                }


                all_p_best_l_1[index_all] = p_best_l_1;
                all_p_best_l_2[index_all] = p_best_l_2;
                all_p_best_mean[index_all] = p_best_mean;
                all_p_best_std[index_all] = p_best_std;
                all_p_best_residual_norms[index_all] = p_best_residual_norms;

                index_all += 1;

                free_dd(data_dictionary_pso);

            } else {
                //File path does not exist, continue to the next one
                seed_checks[seed] = full_path_name_exists;
            }
        }

        //First check if any of the seeds checked out
        int check_extract = 0;
        for (size_t o = 0; o < 4; o++) {
            if (seed_checks[o]) {
                check_extract = 1;
                break;
            }
        }

        if (check_extract) {

            //Creating data structures that will hold the final best data points
            // size_t final_p_best_count = 0;
            struct array* final_p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});
            struct array* final_p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});
            struct array* final_p_best_mean = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});
            struct array* final_p_best_std = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});
            struct array* final_p_best_residual_norms = zeros_a(&(size_t){1}, (size_t[]){array_size}, &(enum DataType){DOUBLE});

            //Get the best value between the different sources that exists
            for (size_t r = 0; r < array_size; r++) {
                int is_set = 0;
                double check_p_best_l_1 = 0;
                double check_p_best_l_2 = 0;
                double check_p_best_mean = 0;
                double check_p_best_std = 0;
                double check_p_best_residual_norms = 0;
                for (size_t g = 0; g < 4; g++) {
                    if (seed_checks[g]) {
                        if (!is_set) {
                            //This is when the variables above is not set yet
                            check_p_best_l_1 = *(double*)get_element_a(all_p_best_l_1[g], (size_t[]){r});
                            check_p_best_l_2 = *(double*)get_element_a(all_p_best_l_2[g], (size_t[]){r});
                            check_p_best_mean = *(double*)get_element_a(all_p_best_mean[g], (size_t[]){r});
                            check_p_best_std = *(double*)get_element_a(all_p_best_std[g], (size_t[]){r});
                            check_p_best_residual_norms = *(double*)get_element_a(all_p_best_residual_norms[g], (size_t[]){r});
                            is_set = 1;
                        } else {
                            //Variables are set and need to be checked if it should be update
                            double temp_check_p_best_l_1 = *(double*)get_element_a(all_p_best_l_1[g], (size_t[]){r});
                            double temp_check_p_best_l_2 = *(double*)get_element_a(all_p_best_l_2[g], (size_t[]){r});
                            double temp_check_p_best_mean = *(double*)get_element_a(all_p_best_mean[g], (size_t[]){r});
                            double temp_check_p_best_std = *(double*)get_element_a(all_p_best_std[g], (size_t[]){r});
                            double temp_check_p_best_residual_norms = *(double*)get_element_a(all_p_best_residual_norms[g], (size_t[]){r});

                            if (temp_check_p_best_mean < check_p_best_mean) {
                                //update
                                check_p_best_l_1 = temp_check_p_best_l_1;
                                check_p_best_l_2 = temp_check_p_best_l_2;
                                check_p_best_mean = temp_check_p_best_mean;
                                check_p_best_std = temp_check_p_best_std;
                                check_p_best_residual_norms = temp_check_p_best_residual_norms;
                            } else if (temp_check_p_best_mean == check_p_best_mean) {
                                if (temp_check_p_best_std < check_p_best_std) {
                                    //update
                                    check_p_best_l_1 = temp_check_p_best_l_1;
                                    check_p_best_l_2 = temp_check_p_best_l_2;
                                    check_p_best_mean = temp_check_p_best_mean;
                                    check_p_best_std = temp_check_p_best_std;
                                    check_p_best_residual_norms = temp_check_p_best_residual_norms;
                                } else if (temp_check_p_best_std == check_p_best_std) {
                                    if (temp_check_p_best_residual_norms < check_p_best_residual_norms) {
                                        //update
                                        check_p_best_l_1 = temp_check_p_best_l_1;
                                        check_p_best_l_2 = temp_check_p_best_l_2;
                                        check_p_best_mean = temp_check_p_best_mean;
                                        check_p_best_std = temp_check_p_best_std;
                                        check_p_best_residual_norms = temp_check_p_best_residual_norms;
                                    }
                                }
                            }

                        }
                    }
                }

                //Set the values in the final arrays
                set_element_a(final_p_best_l_1, (size_t[]){r}, &check_p_best_l_1);
                set_element_a(final_p_best_l_2, (size_t[]){r}, &check_p_best_l_2);
                set_element_a(final_p_best_mean, (size_t[]){r}, &check_p_best_mean);
                set_element_a(final_p_best_std, (size_t[]){r}, &check_p_best_std);
                set_element_a(final_p_best_residual_norms, (size_t[]){r}, &check_p_best_residual_norms);
            }

            struct array* hyperparameters_order = new_a(&(size_t){1}, (size_t[]){array_size * 2}, &(enum DataType){DOUBLE});

            for (size_t r = 0; r < array_size; r++) {
                size_t index_1 = r * 2;
                size_t index_2 = (r * 2) + 1;

                set_element_a(hyperparameters_order, (size_t[]){index_1}, (double*)get_element_a(final_p_best_l_1, (size_t[]){r}));
                set_element_a(hyperparameters_order, (size_t[]){index_2}, (double*)get_element_a(final_p_best_l_2, (size_t[]){r}));

            }

            set_ja(hyperparameters, hyperparameters_order, &hyperparameter_count);

            // Ellipses data block
            free_a(final_p_best_l_1);
            free_a(final_p_best_l_2);
            free_a(final_p_best_mean);
            free_a(final_p_best_std);
            free_a(final_p_best_residual_norms);
            free_a(hyperparameters_order);
            hyperparameter_count += 1;
        }

        //Freeing the data structures that holds the data
        for (size_t y = 0; y < 4; y++) {
            if (all_p_best_l_1[y] != NULL) {
                free_a(all_p_best_l_1[y]);
            }
            if (all_p_best_l_2[y] != NULL) {
                free_a(all_p_best_l_2[y]);
            }
            if (all_p_best_mean[y] != NULL) {
                free_a(all_p_best_mean[y]);
            }
            if (all_p_best_std[y] != NULL) {
                free_a(all_p_best_std[y]);
            }
            if (all_p_best_residual_norms[y] != NULL) {
                free_a(all_p_best_residual_norms[y]);
            }
        }

        free(all_p_best_l_1);
        free(all_p_best_l_2);
        free(all_p_best_mean);
        free(all_p_best_std);
        free(all_p_best_residual_norms);

        //pbest
    }

    save_to_file_ja(hyperparameters, ".", "best_hyperparameters_stefcal_optimal");

    free_ja(hyperparameters);


}

void extract_all_iterations_original_from_data_optimal_solutions(char* directory, char* filename, int* order_min, int* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    //order
    for (int order = *order_min; order <= *order_max; order++) {
        struct array* mean_iterations = zeros_a(&(size_t){1}, (size_t[]){64}, &(enum DataType){DOUBLE});
        size_t count = 0;
        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char file_path_iterations[512];
                    sprintf(file_path_iterations,"%s/data_optimal_solutions/%s_original/data_order_%d/data_%s_order_%d_seed_%d_snr_%.0f_maxamp_%.0f/iterations.array",directory, filename, order, filename, order, seeds[h], snrs[i], max_amps[j]);
                    struct array* iterations = read_from_file_a(file_path_iterations);

                    double* mean = mean_a(iterations);
                    set_element_a(mean_iterations, (size_t[]){count}, mean);
                    count += 1;

                    free(mean);
                    free_a(iterations);
                }
            }
        }
        // Save the array as .array and as .txt
        char file_path_iterations[256];
        sprintf(file_path_iterations,"%s/data_optimal_solutions/%s_original/data_order_%d",directory, filename, order);
        char file_name_all_iterations[256];
        sprintf(file_name_all_iterations,"%s_original_order_%d_all_iterations",filename, order);

        save_to_file_a(mean_iterations, file_path_iterations, file_name_all_iterations);
        free_a(mean_iterations);
    }
}

void extract_all_iterations_optimal_from_data_optimal_solutions(char* directory, char* filename, int* order_min, int* order_max) {
    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    //order
    for (int order = *order_min; order <= *order_max; order++) {
        struct array* mean_iterations = zeros_a(&(size_t){1}, (size_t[]){64}, &(enum DataType){DOUBLE});
        size_t count = 0;
        //seeds
        for (size_t h = 0; h < 4; h++) {
            //snrs
            for (size_t i = 0; i < 4; i++) {
                //max_amps
                for (size_t j = 0; j < 4; j++) {
                    char file_path_iterations[512];
                    sprintf(file_path_iterations,"%s/data_optimal_solutions/%s_optimal/data_order_%d/data_%s_order_%d_seed_%d_snr_%.0f_maxamp_%.0f/iterations.array",directory, filename, order, filename, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", file_path_iterations);
                    struct array* iterations = read_from_file_a(file_path_iterations);

                    double* mean = mean_a(iterations);
                    set_element_a(mean_iterations, (size_t[]){count}, mean);
                    count += 1;

                    free(mean);
                    free_a(iterations);
                }
            }
        }
        // Save the array as .array and as .txt
        char file_path_iterations[256];
        sprintf(file_path_iterations,"%s/data_optimal_solutions/%s_optimal/data_order_%d",directory, filename, order);
        char file_name_all_iterations[256];
        sprintf(file_name_all_iterations,"%s_optimal_order_%d_all_iterations",filename, order);

        save_to_file_a(mean_iterations, file_path_iterations, file_name_all_iterations);
        free_a(mean_iterations);
    }
}

/**
 * Local gridding: 2D sweep over (B1, B2) for ADAM with per-experiment fixed
 * alpha. Runs every remaining experiment for each order in one invocation
 * (resume-friendly: skips DataStores already in the dictionary).
 * Pre-requisite: alpha files exist at
 *   {directory}/data_optimal_solutions/adam_best_alpha/data_order_<O>/
 *     seed_<S>_snr_<SNR>_maxamp_<AMP>.array
 * and the DataDictionary {directory}/data/data_order_<O>/data_adam_fixed_alpha_order_<O>.ddconfig
 * has been created.
 */
void mapping_adam_fixed_alpha_local(char* directory, int* order_min, int* order_max) {
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;
    int num_workers = workers(30);
    size_t grid_length = grid_points(200);
    enum AlgorithmType algorithm_type = ADAM_FIXED_ALPHA;
    size_t amount_hyperparameters = 2;
    struct array* hyperparameters_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4},
                                    &(enum DataType){DOUBLE},
                                    (double[4]){
                                        0.7, 0.9999, 0.7, 0.9999
                                    });

    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    for (int order = *order_min; order <= *order_max; order++) {
        char dd_directory[256];
        sprintf(dd_directory, "%s/data/data_order_%d", directory, order);
        char dd_name[256];
        sprintf(dd_name, "data_adam_fixed_alpha_order_%d", order);

        struct DataDictionary* data_dictionary = read_from_file_dd(dd_directory, dd_name);

        size_t counter = 0;
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    if (counter >= *data_dictionary->amount_items) {
                        char alpha_path[512];
                        sprintf(alpha_path, "%s/data_optimal_solutions/adam_best_alpha/data_order_%d/seed_%d_snr_%.0f_maxamp_%.0f.array",
                                directory, order, seeds[h], snrs[i], max_amps[j]);
                        struct array* alpha_array = read_from_file_a(alpha_path);
                        double fixed_alpha = *(double*)get_element_a(alpha_array, (size_t[]){0});
                        free_a(alpha_array);

                        struct DataStore* data_store = new_data_store_update_dd_fixed_alpha(&data_dictionary, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &(double){snrs[i]}, &(double){max_amps[j]}, &(int){seeds[h]}, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds, &fixed_alpha);
                        start_data_store_dd(data_store);
                        free_ds(data_store);
                    }
                    counter += 1;
                }
            }
        }

        free_dd(data_dictionary);
    }

    free_a(hyperparameters_bounds);
}

/* Round 2b (deliverable): 2D (l1,l2) gridding heatmap of the stochastic adaptive solver with b0
 * fixed per experiment to the round-1 best (read from data_optimal_solutions/stochastic_best_b/...).
 * On the work-equivalent metric the diagonal (l1=l2) is cheap and the off-diagonal expensive.
 * HPC variant: one (seed, snr, max_amp) experiment per invocation (mirrors mapping_adam_fixed_alpha_hpc). */

void mapping_stochastic_fixed_b_local(char* directory, int* order_min, int* order_max) {
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;
    int num_workers = workers(30);
    size_t grid_length = grid_points(200);
    enum AlgorithmType algorithm_type = STEFCAL_STOCHASTIC_FIXED_B;
    size_t amount_hyperparameters = 2;
    struct array* hyperparameters_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4},
                                    &(enum DataType){DOUBLE},
                                    (double[4]){
                                        -0.5, 3, -0.5, 3
                                    });

    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    for (int order = *order_min; order <= *order_max; order++) {
        char dd_directory[256];
        sprintf(dd_directory, "%s/data/data_order_%d", directory, order);
        char dd_name[256];
        sprintf(dd_name, "data_stochastic_fixed_b_order_%d", order);

        struct DataDictionary* data_dictionary = read_from_file_dd(dd_directory, dd_name);

        size_t counter = 0;
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    if (counter >= *data_dictionary->amount_items) {
                        char b_path[512];
                        sprintf(b_path, "%s/data_optimal_solutions/stochastic_best_b/data_order_%d/seed_%d_snr_%.0f_maxamp_%.0f.array",
                                directory, order, seeds[h], snrs[i], max_amps[j]);
                        struct array* b_array = read_from_file_a(b_path);
                        double fixed_b = *(double*)get_element_a(b_array, (size_t[]){0});
                        free_a(b_array);

                        struct DataStore* data_store = new_data_store_update_dd_fixed_b(&data_dictionary, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &(double){snrs[i]}, &(double){max_amps[j]}, &(int){seeds[h]}, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, hyperparameters_bounds, &fixed_b);
                        start_data_store_dd(data_store);
                        free_ds(data_store);
                    }
                    counter += 1;
                }
            }
        }

        free_dd(data_dictionary);
    }

    free_a(hyperparameters_bounds);
}

/**
 * Resume any incomplete ADAM_FIXED_ALPHA gridding DataStores.
 * mapping_adam_fixed_alpha_local only CREATES new DataStores when counter
 * passes amount_items — it never re-runs existing ones. If a previous run
 * was interrupted partway through a DataStore's grid sweep, that DataStore
 * sits in the DD with progress < grid_length² and is skipped on rerun.
 *
 * This function walks every DataStore already in the DD, looks up its
 * per-experiment fixed_alpha, and calls start_data_store_dd on it.
 * start_ds is already resume-friendly via its `progress` watermark — fully
 * completed DataStores are a quick no-op; partial ones get finished.
 */

/**
 * Local PSO: 2D PSO sweep over (B1, B2) for ADAM with per-experiment fixed
 * alpha. Loops over orders × pso_seeds and runs all 64 (seed, snr, max_amp)
 * experiments per (order, pso_seed) in one invocation (start_single_pso_local
 * handles the inner loop; alpha is looked up per experiment from
 *   {directory}/data_optimal_solutions/adam_best_alpha/data_order_<O>/...).
 * Pre-requisite: per-pso-seed data dictionaries already created at
 *   {directory}/data/data_order_<O>/data_pso_adam_fixed_alpha_order_<O>_seed_<S>.ddconfig.
 */
void mapping_pso_adam_fixed_alpha_local(char* directory, int* order_min, int* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};

    enum AlgorithmType option = ADAM_FIXED_ALPHA;
    size_t num_dimensions = 2;
    struct array* position_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4},
                                    &(enum DataType){DOUBLE},
                                    (double[4]){
                                        0.7, 0.9999, 0.7, 0.9999
                                    });

    size_t iterations_max = pso_iterations(50);
    size_t num_particles = pso_particles(25);
    size_t k = 9;
    int num_workers = workers(8);

    char simulators_directory[256];
    sprintf(simulators_directory, "%s/simulators", directory);

    for (int order = *order_min; order <= *order_max; order++) {
        for (size_t i = 0; i < 4; i++) {
            int pso_seed = pso_seeds[i];

            char dd_directory[256];
            sprintf(dd_directory, "%s/data/data_order_%d", directory, order);
            char dd_name[256];
            sprintf(dd_name, "data_pso_adam_fixed_alpha_order_%d_seed_%d", order, pso_seed);

            struct PSO* pso = new_pso_fixed_alpha(&option, &num_dimensions, position_bounds, &iterations_max, &num_particles, &pso_seed, simulators_directory, &order, &k, dd_name, dd_directory, &num_workers, NULL);
            pso->alpha_directory = malloc(strlen(directory) + 1);
            strcpy(pso->alpha_directory, directory);

            start_single_pso_local(pso);
            free_pso(pso);
        }
    }

    free_a(position_bounds);
}

/**
 * For each (seed, snr, max_amp) over [order_min, order_max], pick the best
 * alpha across the 4 PSO seeds of the 3-hyperparameter ADAM PSO run.
 * Selection order: min iter_mean -> tie-break iter_std -> tie-break residual_norm.
 * Writes a 1-element double .array per experiment under:
 *   {directory}/data_optimal_solutions/adam_best_alpha/data_order_<order>/
 *     seed_<seed>_snr_<snr>_maxamp_<max_amp>.array
 */

/**
 * For each (seed, snr, max_amp) over [order_min, order_max], pick the best
 * alpha across the 4 PSO seeds of the 3-hyperparameter ADAM PSO run.
 * Selection order: min iter_mean -> tie-break iter_std -> tie-break residual_norm.
 * Writes a 1-element double .array per experiment under:
 *   {directory}/data_optimal_solutions/adam_best_alpha/data_order_<order>/
 *     seed_<seed>_snr_<snr>_maxamp_<max_amp>.array
 */
void extract_best_alpha_adam(char* directory, size_t* order_min, size_t* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_alpha = NAN;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%ld/data_pso_adam_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%ld/data_pso_adam_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_alpha = *(double*)get_element_a(positions_array, (size_t[]){0});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_alpha = position_alpha;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_alpha = position_alpha;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_alpha = position_alpha;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/adam_best_alpha/data_order_%ld", directory, order);
                    create_directories(directory_save);

                    char filename_save[256];
                    sprintf(filename_save, "seed_%d_snr_%.0f_maxamp_%.0f", seeds[h], snrs[i], max_amps[j]);

                    struct array* alpha_array = new_set_a(&(size_t){1},
                                                          (size_t[]){1},
                                                          &(enum DataType){DOUBLE},
                                                          (double[1]){best_alpha});
                    save_to_file_a(alpha_array, directory_save, filename_save);
                    free_a(alpha_array);
                }
            }
        }
    }
}

/**
 * Hyperparameter scatter for ADAM taken ENTIRELY from the 3-parameter PSO.
 *
 * extract_all_best_b1_b2_alpha_adam pairs alpha from the 3-parameter run with (B1, B2)
 * from the fixed-alpha run. That pairing is unusable: ADAM_FIXED_ALPHA was missing from
 * the switch in update_position_bounds_particle, so the fixed-alpha swarm never moved
 * and its "optimum" is just the best of its random initial sample.
 *
 * Here alpha, B1 and B2 all come from the same winning 3-parameter run
 * (data_pso_adam_*, positions[0] = alpha, [1] = B1, [2] = B2), selected across the 4 PSO
 * seeds by iteration_mean -> iteration_std -> residual_norm. That is the same rule
 * extract_best_alpha_adam uses, so the alpha here matches the one the grid sweep fixed,
 * and the (B1, B2) therefore lie on the plotted alpha-slice.
 *
 * No symmetry swap: B1 and B2 are not interchangeable.
 *
 * Writes one line per order (the 64 experiments as ';'-separated "B1,B2,alpha" triples) to
 *   <directory>/extracted_trace_data/extracted_trace_pso_adam_b1_b2_alpha_3d/all_points.txt
 */

/**
 * Hyperparameter scatter for ADAM taken ENTIRELY from the 3-parameter PSO.
 *
 * extract_all_best_b1_b2_alpha_adam pairs alpha from the 3-parameter run with (B1, B2)
 * from the fixed-alpha run. That pairing is unusable: ADAM_FIXED_ALPHA was missing from
 * the switch in update_position_bounds_particle, so the fixed-alpha swarm never moved
 * and its "optimum" is just the best of its random initial sample.
 *
 * Here alpha, B1 and B2 all come from the same winning 3-parameter run
 * (data_pso_adam_*, positions[0] = alpha, [1] = B1, [2] = B2), selected across the 4 PSO
 * seeds by iteration_mean -> iteration_std -> residual_norm. That is the same rule
 * extract_best_alpha_adam uses, so the alpha here matches the one the grid sweep fixed,
 * and the (B1, B2) therefore lie on the plotted alpha-slice.
 *
 * No symmetry swap: B1 and B2 are not interchangeable.
 *
 * Writes one line per order (the 64 experiments as ';'-separated "B1,B2,alpha" triples) to
 *   <directory>/extracted_trace_data/extracted_trace_pso_adam_b1_b2_alpha_3d/all_points.txt
 */
void extract_all_best_b1_b2_alpha_adam_3d(char* directory, size_t* order_min, size_t* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    char out_directory[512];
    sprintf(out_directory, "%s/extracted_trace_data/extracted_trace_pso_adam_b1_b2_alpha_3d", directory);
    create_directories(out_directory);

    char all_points_filename[600];
    sprintf(all_points_filename, "%s/all_points.txt", out_directory);
    FILE *file_all_points = open_output(all_points_filename, "w");
    if (file_all_points == NULL) {
        perror("Error opening all_points file");
        exit(EXIT_FAILURE);
    }

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    double best_mean = INFINITY;
                    double best_std = INFINITY;
                    double best_residual = INFINITY;
                    double best_alpha = NAN;
                    double best_B1 = NAN;
                    double best_B2 = NAN;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%ld/data_pso_adam_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%ld/data_pso_adam_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);

                        if (!check_file_path(path_doubles) || !check_file_path(path_positions)) {
                            continue;
                        }

                        struct array* doubles_array = read_from_file_a(path_doubles);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_alpha = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double position_B1 = *(double*)get_element_a(positions_array, (size_t[]){1});
                        double position_B2 = *(double*)get_element_a(positions_array, (size_t[]){2});

                        int better = 0;
                        if (iteration_mean < best_mean) {
                            better = 1;
                        } else if (iteration_mean == best_mean) {
                            if (iteration_std < best_std) {
                                better = 1;
                            } else if (iteration_std == best_std && residual_norm < best_residual) {
                                better = 1;
                            }
                        }
                        if (better) {
                            best_mean = iteration_mean;
                            best_std = iteration_std;
                            best_residual = residual_norm;
                            best_alpha = position_alpha;
                            best_B1 = position_B1;
                            best_B2 = position_B2;
                        }

                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    int is_last = (h == 3 && i == 3 && j == 3);
                    if (is_last) {
                        fprintf(file_all_points, "%lf,%lf,%lf;\n", best_B1, best_B2, best_alpha);
                    } else {
                        fprintf(file_all_points, "%lf,%lf,%lf;", best_B1, best_B2, best_alpha);
                    }
                }
            }
        }
    }

    fclose(file_all_points);
    printf("wrote %s\n", all_points_filename);
}

/**
 * Hyperparameter scatter for the ADAM study, emitting (B1, B2, alpha) per experiment so the
 * learning rate alpha can be displayed alongside B1/B2. Mirrors extract_all_best_l1_l2_b_stochastic.
 *
 * Per the same pairing convention: B1/B2 are taken from the fixed-alpha PSO (data_pso_adam_fixed_alpha_*,
 * positions[0],[1]) and alpha is taken from the 3-param PSO (data_pso_adam_*, positions[0]) where alpha
 * was a free parameter. Both are selected as the best across the 4 PSO seeds by
 * (iteration_mean -> iteration_std -> residual_norm).
 *
 * Unlike the stochastic (l1,l2) case there is no symmetry swap (B1 and B2 are distinct roles, not
 * interchangeable) and alpha is left continuous (not rounded). Writes one line per order (the 64
 * experiments as ';'-separated "B1,B2,alpha" triples) to
 *   <directory>/extracted_trace_data/extracted_trace_pso_adam_b1_b2_alpha/all_points.txt
 */

/**
 * Hyperparameter scatter for the ADAM study, emitting (B1, B2, alpha) per experiment so the
 * learning rate alpha can be displayed alongside B1/B2. Mirrors extract_all_best_l1_l2_b_stochastic.
 *
 * Per the same pairing convention: B1/B2 are taken from the fixed-alpha PSO (data_pso_adam_fixed_alpha_*,
 * positions[0],[1]) and alpha is taken from the 3-param PSO (data_pso_adam_*, positions[0]) where alpha
 * was a free parameter. Both are selected as the best across the 4 PSO seeds by
 * (iteration_mean -> iteration_std -> residual_norm).
 *
 * Unlike the stochastic (l1,l2) case there is no symmetry swap (B1 and B2 are distinct roles, not
 * interchangeable) and alpha is left continuous (not rounded). Writes one line per order (the 64
 * experiments as ';'-separated "B1,B2,alpha" triples) to
 *   <directory>/extracted_trace_data/extracted_trace_pso_adam_b1_b2_alpha/all_points.txt
 */
void extract_all_best_b1_b2_alpha_adam(char* directory, size_t* order_min, size_t* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    char out_directory[512];
    sprintf(out_directory, "%s/extracted_trace_data/extracted_trace_pso_adam_b1_b2_alpha", directory);
    create_directories(out_directory);

    char all_points_filename[600];
    sprintf(all_points_filename, "%s/all_points.txt", out_directory);
    FILE *file_all_points = open_output(all_points_filename, "w");
    if (file_all_points == NULL) {
        perror("Error opening all_points file");
        exit(EXIT_FAILURE);
    }

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    // --- B1/B2 from the fixed-alpha PSO: best across the 4 PSO seeds ---
                    double best_fa_mean = INFINITY;
                    double best_fa_std = INFINITY;
                    double best_fa_residual = INFINITY;
                    double best_B1 = NAN;
                    double best_B2 = NAN;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%ld/data_pso_adam_fixed_alpha_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%ld/data_pso_adam_fixed_alpha_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_B1 = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double position_B2 = *(double*)get_element_a(positions_array, (size_t[]){1});

                        if (iteration_mean < best_fa_mean) {
                            best_fa_mean = iteration_mean;
                            best_fa_std = iteration_std;
                            best_fa_residual = residual_norm;
                            best_B1 = position_B1;
                            best_B2 = position_B2;
                        } else if (iteration_mean == best_fa_mean) {
                            if (iteration_std < best_fa_std) {
                                best_fa_mean = iteration_mean;
                                best_fa_std = iteration_std;
                                best_fa_residual = residual_norm;
                                best_B1 = position_B1;
                                best_B2 = position_B2;
                            } else if (iteration_std == best_fa_std) {
                                if (residual_norm < best_fa_residual) {
                                    best_fa_mean = iteration_mean;
                                    best_fa_std = iteration_std;
                                    best_fa_residual = residual_norm;
                                    best_B1 = position_B1;
                                    best_B2 = position_B2;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    // --- alpha from the 3-param PSO: best across the 4 PSO seeds (same as extract_best_alpha_adam) ---
                    double best_3p_mean = INFINITY;
                    double best_3p_std = INFINITY;
                    double best_3p_residual = INFINITY;
                    double best_alpha = NAN;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%ld/data_pso_adam_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%ld/data_pso_adam_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_alpha = *(double*)get_element_a(positions_array, (size_t[]){0});

                        if (iteration_mean < best_3p_mean) {
                            best_3p_mean = iteration_mean;
                            best_3p_std = iteration_std;
                            best_3p_residual = residual_norm;
                            best_alpha = position_alpha;
                        } else if (iteration_mean == best_3p_mean) {
                            if (iteration_std < best_3p_std) {
                                best_3p_mean = iteration_mean;
                                best_3p_std = iteration_std;
                                best_3p_residual = residual_norm;
                                best_alpha = position_alpha;
                            } else if (iteration_std == best_3p_std) {
                                if (residual_norm < best_3p_residual) {
                                    best_3p_mean = iteration_mean;
                                    best_3p_std = iteration_std;
                                    best_3p_residual = residual_norm;
                                    best_alpha = position_alpha;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    int is_last = (h == 3 && i == 3 && j == 3);
                    if (is_last) {
                        fprintf(file_all_points, "%lf,%lf,%lf;\n", best_B1, best_B2, best_alpha);
                    } else {
                        fprintf(file_all_points, "%lf,%lf,%lf;", best_B1, best_B2, best_alpha);
                    }
                }
            }
        }
    }

    fclose(file_all_points);
}

/* Mirror of extract_best_alpha_adam for the stochastic (l1,l2,b) study: across the 4 PSO seeds of
 * the round-1 3-param PSO, pick the best by work-mean -> std -> residual, extract its b0
 * (positions index 2), and write it (rounded) per experiment to
 *   <directory>/data_optimal_solutions/stochastic_best_b/data_order_<O>/seed_<>_snr_<>_maxamp_<>.array
 * which the fixed-b PSO + grid then read. */

/* Mirror of extract_best_alpha_adam for the stochastic (l1,l2,b) study: across the 4 PSO seeds of
 * the round-1 3-param PSO, pick the best by work-mean -> std -> residual, extract its b0
 * (positions index 2), and write it (rounded) per experiment to
 *   <directory>/data_optimal_solutions/stochastic_best_b/data_order_<O>/seed_<>_snr_<>_maxamp_<>.array
 * which the fixed-b PSO + grid then read. */
void extract_best_b_stochastic(char* directory, size_t* order_min, size_t* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_b = NAN;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%ld/data_pso_stochastic_l1l2_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%ld/data_pso_stochastic_l1l2_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_b = *(double*)get_element_a(positions_array, (size_t[]){2});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_b = position_b;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_b = position_b;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_b = position_b;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/stochastic_best_b/data_order_%ld", directory, order);
                    create_directories(directory_save);

                    char filename_save[256];
                    sprintf(filename_save, "seed_%d_snr_%.0f_maxamp_%.0f", seeds[h], snrs[i], max_amps[j]);

                    struct array* b_array = new_set_a(&(size_t){1},
                                                      (size_t[]){1},
                                                      &(enum DataType){DOUBLE},
                                                      (double[1]){round(best_b)});
                    save_to_file_a(b_array, directory_save, filename_save);
                    free_a(b_array);
                }
            }
        }
    }
}

/**
 * Hyperparameter scatter for the stochastic-adaptive StEFCal study, emitting (l1, l2, b) per
 * experiment so the b0 column can be displayed alongside l1/l2.
 *
 * Per the paper's pairing: l1/l2 are taken from the round-2a fixed-b PSO (data_pso_stochastic_fixed_b_*,
 * positions[0],[1]) — the deliverable values — while b is taken from the round-1 3-param PSO
 * (data_pso_stochastic_l1l2_*, positions[2]) where b0 was a free parameter. Both are selected as the
 * best across the 4 PSO seeds by (iteration_mean -> iteration_std -> residual_norm), and b is rounded
 * to match extract_best_b_stochastic (the b that was actually fixed for the fixed-b runs).
 *
 * l1/l2 are canonicalised with the same symmetry swap as pso_extract_all_best_hyperparameters so the
 * stats line up with the existing scatter/ellipse plots. Writes one line per order (the 64 experiments
 * as ';'-separated "l1,l2,b" triples) to
 *   <directory>/extracted_trace_data/extracted_trace_pso_stochastic_l1l2_b/all_points.txt
 */

/**
 * Hyperparameter scatter for the stochastic-adaptive StEFCal study, emitting (l1, l2, b) per
 * experiment so the b0 column can be displayed alongside l1/l2.
 *
 * Per the paper's pairing: l1/l2 are taken from the round-2a fixed-b PSO (data_pso_stochastic_fixed_b_*,
 * positions[0],[1]) — the deliverable values — while b is taken from the round-1 3-param PSO
 * (data_pso_stochastic_l1l2_*, positions[2]) where b0 was a free parameter. Both are selected as the
 * best across the 4 PSO seeds by (iteration_mean -> iteration_std -> residual_norm), and b is rounded
 * to match extract_best_b_stochastic (the b that was actually fixed for the fixed-b runs).
 *
 * l1/l2 are canonicalised with the same symmetry swap as pso_extract_all_best_hyperparameters so the
 * stats line up with the existing scatter/ellipse plots. Writes one line per order (the 64 experiments
 * as ';'-separated "l1,l2,b" triples) to
 *   <directory>/extracted_trace_data/extracted_trace_pso_stochastic_l1l2_b/all_points.txt
 */
void extract_all_best_l1_l2_b_stochastic(char* directory, size_t* order_min, size_t* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    char out_directory[512];
    sprintf(out_directory, "%s/extracted_trace_data/extracted_trace_pso_stochastic_l1l2_b", directory);
    create_directories(out_directory);

    char all_points_filename[600];
    sprintf(all_points_filename, "%s/all_points.txt", out_directory);
    FILE *file_all_points = open_output(all_points_filename, "w");
    if (file_all_points == NULL) {
        perror("Error opening all_points file");
        exit(EXIT_FAILURE);
    }

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    // --- l1/l2 from the fixed-b PSO: best across the 4 PSO seeds ---
                    double best_fb_mean = INFINITY;
                    double best_fb_std = INFINITY;
                    double best_fb_residual = INFINITY;
                    double best_l1 = NAN;
                    double best_l2 = NAN;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%ld/data_pso_stochastic_fixed_b_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%ld/data_pso_stochastic_fixed_b_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_l1 = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double position_l2 = *(double*)get_element_a(positions_array, (size_t[]){1});

                        if (iteration_mean < best_fb_mean) {
                            best_fb_mean = iteration_mean;
                            best_fb_std = iteration_std;
                            best_fb_residual = residual_norm;
                            best_l1 = position_l1;
                            best_l2 = position_l2;
                        } else if (iteration_mean == best_fb_mean) {
                            if (iteration_std < best_fb_std) {
                                best_fb_mean = iteration_mean;
                                best_fb_std = iteration_std;
                                best_fb_residual = residual_norm;
                                best_l1 = position_l1;
                                best_l2 = position_l2;
                            } else if (iteration_std == best_fb_std) {
                                if (residual_norm < best_fb_residual) {
                                    best_fb_mean = iteration_mean;
                                    best_fb_std = iteration_std;
                                    best_fb_residual = residual_norm;
                                    best_l1 = position_l1;
                                    best_l2 = position_l2;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    // Canonicalise l1/l2 by the (0,1)/(1,0) symmetry, matching pso_extract_all_best_hyperparameters.
                    double d_min_to_0_1 = sqrt(pow((0 - best_l1), 2) + pow((1 - best_l2), 2));
                    double d_min_to_1_0 = sqrt(pow((1 - best_l1), 2) + pow((0 - best_l2), 2));
                    double out_l1 = best_l1;
                    double out_l2 = best_l2;
                    if (d_min_to_0_1 > d_min_to_1_0) {
                        out_l1 = best_l2;
                        out_l2 = best_l1;
                    }

                    // --- b from the 3-param PSO: best across the 4 PSO seeds (same as extract_best_b_stochastic) ---
                    double best_l1l2_mean = INFINITY;
                    double best_l1l2_std = INFINITY;
                    double best_l1l2_residual = INFINITY;
                    double best_b = NAN;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%ld/data_pso_stochastic_l1l2_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%ld/data_pso_stochastic_l1l2_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_b = *(double*)get_element_a(positions_array, (size_t[]){2});

                        if (iteration_mean < best_l1l2_mean) {
                            best_l1l2_mean = iteration_mean;
                            best_l1l2_std = iteration_std;
                            best_l1l2_residual = residual_norm;
                            best_b = position_b;
                        } else if (iteration_mean == best_l1l2_mean) {
                            if (iteration_std < best_l1l2_std) {
                                best_l1l2_mean = iteration_mean;
                                best_l1l2_std = iteration_std;
                                best_l1l2_residual = residual_norm;
                                best_b = position_b;
                            } else if (iteration_std == best_l1l2_std) {
                                if (residual_norm < best_l1l2_residual) {
                                    best_l1l2_mean = iteration_mean;
                                    best_l1l2_std = iteration_std;
                                    best_l1l2_residual = residual_norm;
                                    best_b = position_b;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    double out_b = round(best_b);

                    int is_last = (h == 3 && i == 3 && j == 3);
                    if (is_last) {
                        fprintf(file_all_points, "%lf,%lf,%lf;\n", out_l1, out_l2, out_b);
                    } else {
                        fprintf(file_all_points, "%lf,%lf,%lf;", out_l1, out_l2, out_b);
                    }
                }
            }
        }
    }

    fclose(file_all_points);
}

/**
 * HPC variant of extract_best_b_stochastic. Runs from ${PBS_O_WORKDIR}/msc/, using the same
 * ../<sibling> path convention as extract_best_alpha_adam_hpc. Reads the round-1 3-param PSO
 * results and writes the best b0 (rounded) per experiment to
 *   ../data_optimal_solutions/stochastic_best_b/data_order_<O>/seed_<>_snr_<>_maxamp_<>.array
 */

/**
 * Per-experiment optimal gridding point for ADAM_FIXED_ALPHA.
 * Reads iteration_means/stds from the heatmap output produced by
 * extract_heatmap_data, finds the argmin grid cell, recovers (B1, B2)
 * from a linspace over [0.7, 0.9999], and writes four flat arrays to cwd:
 *   normal_p_best_l_1.array, normal_p_best_l_2.array,
 *   normal_p_best_mean.array, normal_p_best_std.array
 *
 * The output filenames intentionally match what extract_trace_specific
 * produces so the manual copy-to-python/data step is unchanged.
 */
void extract_normal_p_best_adam_fixed_alpha(char* directory, int* order_min, int* order_max) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    /* grid_length is derived per experiment from the stored array below: these
     * heatmaps are 200 x 200, and hardcoding 50 both truncated the argmin scan to
     * the first 2500 cells and converted the winning index through the wrong
     * lattice, so the recovered (B1, B2) did not match the reported value. */
    double bound_min = 0.7;
    double bound_max = 0.9999;

    size_t total = (*order_max - *order_min + 1) * 4 * 4 * 4;
    struct array* p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_mean = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_std = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});

    size_t count = 0;
    for (int order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    char path_means[512];
                    sprintf(path_means, "%s/extracted_heatmap_data/data_order_%d/data_adam_fixed_alpha_order_%d/seed_%d_order_%d_snr_%lf_max_amp_%lf/iteration_means.array",
                            directory, order, order, seeds[h], order, snrs[i], max_amps[j]);

                    char path_stds[512];
                    sprintf(path_stds, "%s/extracted_heatmap_data/data_order_%d/data_adam_fixed_alpha_order_%d/seed_%d_order_%d_snr_%lf_max_amp_%lf/iteration_stds.array",
                            directory, order, order, seeds[h], order, snrs[i], max_amps[j]);

                    // read_from_file_a() exit(0)s on a missing file, so probe first. Missing/empty
                    // experiments get NaN placeholders to keep the seed->snr->max_amp index
                    // alignment the Python heatmap plotters rely on.
                    FILE* probe_means = fopen(path_means, "r");
                    FILE* probe_stds = fopen(path_stds, "r");
                    if (probe_means) fclose(probe_means);
                    if (probe_stds) fclose(probe_stds);
                    if (!probe_means || !probe_stds) {
                        printf("SKIP (no data): %s\n", path_means);
                        set_element_a(p_best_l_1, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_l_2, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_mean, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_std, (size_t[]){count}, &(double){NAN});
                        count += 1;
                        continue;
                    }

                    printf("%s\n", path_means);
                    struct array* means = read_from_file_a(path_means);
                    struct array* stds = read_from_file_a(path_stds);

                    size_t grid_length = (size_t)llround(sqrt((double)get_size_a(means)));
                    if (grid_length * grid_length != get_size_a(means)) {
                        printf("SKIP (not a square grid: %zu values): %s\n", get_size_a(means), path_means);
                        set_element_a(p_best_l_1, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_l_2, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_mean, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_std, (size_t[]){count}, &(double){NAN});
                        count += 1;
                        free_a(means);
                        free_a(stds);
                        continue;
                    }

                    double best_mean = INFINITY;
                    size_t best_idx = 0;
                    for (size_t m = 0; m < grid_length * grid_length; m++) {
                        double v = *(double*)get_element_a(means, (size_t[]){m});
                        if (v < best_mean) {
                            best_mean = v;
                            best_idx = m;
                        }
                    }

                    size_t bi = best_idx / grid_length;   // B1 index
                    size_t bj = best_idx % grid_length;   // B2 index
                    double step = (bound_max - bound_min) / (double)(grid_length - 1);
                    double B1 = bound_min + step * (double)bi;
                    double B2 = bound_min + step * (double)bj;
                    double best_std = *(double*)get_element_a(stds, (size_t[]){best_idx});

                    set_element_a(p_best_l_1, (size_t[]){count}, &B1);
                    set_element_a(p_best_l_2, (size_t[]){count}, &B2);
                    set_element_a(p_best_mean, (size_t[]){count}, &best_mean);
                    set_element_a(p_best_std, (size_t[]){count}, &best_std);
                    count += 1;

                    free_a(means);
                    free_a(stds);
                }
            }
        }
    }

    save_to_file_a(p_best_l_1, p_best_dir(directory), "normal_p_best_l_1");
    save_to_file_a(p_best_l_2, p_best_dir(directory), "normal_p_best_l_2");
    save_to_file_a(p_best_mean, p_best_dir(directory), "normal_p_best_mean");
    save_to_file_a(p_best_std, p_best_dir(directory), "normal_p_best_std");

    free_a(p_best_l_1);
    free_a(p_best_l_2);
    free_a(p_best_mean);
    free_a(p_best_std);
}

/**
 * Per-experiment grid best (lambda_1, lambda_2) for StEFCal — same structure as
 * extract_normal_p_best_adam_fixed_alpha, but for the StEFCal heatmaps.
 *
 * Reads iteration_means/iteration_stds from
 *   extracted_heatmap_data/data_order_<O>/data_stefcal_order_<O>/seed_..._snr_..._max_amp_...
 * finds the argmin grid cell, recovers (lambda_1, lambda_2) over the [-0.5, 3.0] bounds, and
 * writes stefcal_normal_p_best_{l_1,l_2,mean,std}.array.
 *
 * Tolerant of partial data: read_from_file_a() exit(0)s on a missing file, so each experiment
 * is probed first and missing/empty ones get NaN placeholders, preserving the linear
 * seed->snr->max_amp index alignment the Python plotters rely on. The grid length is derived
 * from the array size (StEFCal is 200x200, StEFCal-improved is 50x50), not hardcoded.
 */

/**
 * Per-experiment grid best (lambda_1, lambda_2) for StEFCal — same structure as
 * extract_normal_p_best_adam_fixed_alpha, but for the StEFCal heatmaps.
 *
 * Reads iteration_means/iteration_stds from
 *   extracted_heatmap_data/data_order_<O>/data_stefcal_order_<O>/seed_..._snr_..._max_amp_...
 * finds the argmin grid cell, recovers (lambda_1, lambda_2) over the [-0.5, 3.0] bounds, and
 * writes stefcal_normal_p_best_{l_1,l_2,mean,std}.array.
 *
 * Tolerant of partial data: read_from_file_a() exit(0)s on a missing file, so each experiment
 * is probed first and missing/empty ones get NaN placeholders, preserving the linear
 * seed->snr->max_amp index alignment the Python plotters rely on. The grid length is derived
 * from the array size (StEFCal is 200x200, StEFCal-improved is 50x50), not hardcoded.
 */
void extract_normal_p_best_stefcal(char* directory, int* order_min, int* order_max) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    double bound_min = -0.5;
    double bound_max = 3.0;

    size_t total = (*order_max - *order_min + 1) * 4 * 4 * 4;
    struct array* p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_mean = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_std = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});

    size_t count = 0;
    for (int order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    char path_means[512];
                    sprintf(path_means, "%s/extracted_heatmap_data/data_order_%d/data_stefcal_order_%d/seed_%d_order_%d_snr_%lf_max_amp_%lf/iteration_means.array",
                            directory, order, order, seeds[h], order, snrs[i], max_amps[j]);

                    char path_stds[512];
                    sprintf(path_stds, "%s/extracted_heatmap_data/data_order_%d/data_stefcal_order_%d/seed_%d_order_%d_snr_%lf_max_amp_%lf/iteration_stds.array",
                            directory, order, order, seeds[h], order, snrs[i], max_amps[j]);

                    FILE* probe_means = fopen(path_means, "r");
                    FILE* probe_stds = fopen(path_stds, "r");
                    if (probe_means) fclose(probe_means);
                    if (probe_stds) fclose(probe_stds);
                    if (!probe_means || !probe_stds) {
                        printf("SKIP (no data): %s\n", path_means);
                        set_element_a(p_best_l_1, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_l_2, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_mean, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_std, (size_t[]){count}, &(double){NAN});
                        count += 1;
                        continue;
                    }

                    printf("%s\n", path_means);
                    struct array* means = read_from_file_a(path_means);
                    struct array* stds = read_from_file_a(path_stds);

                    size_t grid_length = (size_t)llround(sqrt((double)get_size_a(means)));

                    double step = (bound_max - bound_min) / (double)(grid_length - 1);
                    double best_mean = INFINITY;
                    size_t best_idx = 0;
                    int found = 0;
                    for (size_t m = 0; m < grid_length * grid_length; m++) {
                        size_t ci = m / grid_length;   // lambda_1 index
                        size_t cj = m % grid_length;   // lambda_2 index
                        double cL1 = bound_min + step * (double)ci;
                        double cL2 = bound_min + step * (double)cj;
                        /* Restrict the grid argmin to the region the PSO is allowed to occupy
                         * (random_position_bound1_pso / update_position_bounds_particle case
                         * STEFCAL_LAMBDA): l_1 + l_2 >= 0.5. */
                        if (cL1 + cL2 < 0.5) continue;
                        double v = *(double*)get_element_a(means, (size_t[]){m});
                        if (v < best_mean) {
                            best_mean = v;
                            best_idx = m;
                            found = 1;
                        }
                    }

                    if (!found) {
                        printf("SKIP (no feasible point with l_1 + l_2 >= 0.5): %s\n", path_means);
                        set_element_a(p_best_l_1, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_l_2, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_mean, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_std, (size_t[]){count}, &(double){NAN});
                        count += 1;
                        free_a(means);
                        free_a(stds);
                        continue;
                    }

                    size_t bi = best_idx / grid_length;   // lambda_1 index
                    size_t bj = best_idx % grid_length;   // lambda_2 index
                    double L1 = bound_min + step * (double)bi;
                    double L2 = bound_min + step * (double)bj;
                    double best_std = *(double*)get_element_a(stds, (size_t[]){best_idx});

                    set_element_a(p_best_l_1, (size_t[]){count}, &L1);
                    set_element_a(p_best_l_2, (size_t[]){count}, &L2);
                    set_element_a(p_best_mean, (size_t[]){count}, &best_mean);
                    set_element_a(p_best_std, (size_t[]){count}, &best_std);
                    count += 1;

                    free_a(means);
                    free_a(stds);
                }
            }
        }
    }

    save_to_file_a(p_best_l_1, p_best_dir(directory), "stefcal_normal_p_best_l_1");
    save_to_file_a(p_best_l_2, p_best_dir(directory), "stefcal_normal_p_best_l_2");
    save_to_file_a(p_best_mean, p_best_dir(directory), "stefcal_normal_p_best_mean");
    save_to_file_a(p_best_std, p_best_dir(directory), "stefcal_normal_p_best_std");

    free_a(p_best_l_1);
    free_a(p_best_l_2);
    free_a(p_best_mean);
    free_a(p_best_std);
}

/**
 * Per-experiment grid best (lambda_1, lambda_2) for LM_(lambda_even, lambda_odd).
 * Identical to extract_normal_p_best_stefcal but reads the
 * data_lm_non_adaptive_l1_l2_order_<O> heatmaps and writes
 * lm_non_adaptive_l1_l2_normal_p_best_{l_1,l_2,mean,std}.array.
 *
 * Replaces the old extract_trace_specific route, which read a pre-stored minpoint per
 * data container: that route applied no feasibility filter and was indexed by data
 * dictionary position rather than by (seed, snr, max_amp), so its output could not be
 * indexed alongside the 64-entry PSO arrays.
 */

/**
 * Per-experiment grid best (lambda_1, lambda_2) for LM_(lambda_even, lambda_odd).
 * Identical to extract_normal_p_best_stefcal but reads the
 * data_lm_non_adaptive_l1_l2_order_<O> heatmaps and writes
 * lm_non_adaptive_l1_l2_normal_p_best_{l_1,l_2,mean,std}.array.
 *
 * Replaces the old extract_trace_specific route, which read a pre-stored minpoint per
 * data container: that route applied no feasibility filter and was indexed by data
 * dictionary position rather than by (seed, snr, max_amp), so its output could not be
 * indexed alongside the 64-entry PSO arrays.
 */
void extract_normal_p_best_lm_non_adaptive_l1_l2(char* directory, int* order_min, int* order_max) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    double bound_min = -0.5;
    double bound_max = 3.0;

    size_t total = (*order_max - *order_min + 1) * 4 * 4 * 4;
    struct array* p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_mean = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_std = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});

    size_t count = 0;
    for (int order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    char path_means[512];
                    sprintf(path_means, "%s/extracted_heatmap_data/data_order_%d/data_lm_non_adaptive_l1_l2_order_%d/seed_%d_order_%d_snr_%lf_max_amp_%lf/iteration_means.array",
                            directory, order, order, seeds[h], order, snrs[i], max_amps[j]);

                    char path_stds[512];
                    sprintf(path_stds, "%s/extracted_heatmap_data/data_order_%d/data_lm_non_adaptive_l1_l2_order_%d/seed_%d_order_%d_snr_%lf_max_amp_%lf/iteration_stds.array",
                            directory, order, order, seeds[h], order, snrs[i], max_amps[j]);

                    FILE* probe_means = fopen(path_means, "r");
                    FILE* probe_stds = fopen(path_stds, "r");
                    if (probe_means) fclose(probe_means);
                    if (probe_stds) fclose(probe_stds);
                    if (!probe_means || !probe_stds) {
                        printf("SKIP (no data): %s\n", path_means);
                        set_element_a(p_best_l_1, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_l_2, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_mean, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_std, (size_t[]){count}, &(double){NAN});
                        count += 1;
                        continue;
                    }

                    printf("%s\n", path_means);
                    struct array* means = read_from_file_a(path_means);
                    struct array* stds = read_from_file_a(path_stds);

                    size_t grid_length = (size_t)llround(sqrt((double)get_size_a(means)));

                    double step = (bound_max - bound_min) / (double)(grid_length - 1);
                    double best_mean = INFINITY;
                    size_t best_idx = 0;
                    int found = 0;
                    for (size_t m = 0; m < grid_length * grid_length; m++) {
                        size_t ci = m / grid_length;   // lambda_1 index
                        size_t cj = m % grid_length;   // lambda_2 index
                        double cL1 = bound_min + step * (double)ci;
                        double cL2 = bound_min + step * (double)cj;
                        /* Restrict the grid argmin to the region the PSO is allowed to occupy
                         * (random_position_bound1_pso / update_position_bounds_particle case
                         * LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2): l_1 + l_2 >= 0.5. */
                        if (cL1 + cL2 < 0.5) continue;
                        double v = *(double*)get_element_a(means, (size_t[]){m});
                        if (v < best_mean) {
                            best_mean = v;
                            best_idx = m;
                            found = 1;
                        }
                    }

                    if (!found) {
                        printf("SKIP (no feasible point with l_1 + l_2 >= 0.5): %s\n", path_means);
                        set_element_a(p_best_l_1, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_l_2, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_mean, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_std, (size_t[]){count}, &(double){NAN});
                        count += 1;
                        free_a(means);
                        free_a(stds);
                        continue;
                    }

                    size_t bi = best_idx / grid_length;   // lambda_1 index
                    size_t bj = best_idx % grid_length;   // lambda_2 index
                    double L1 = bound_min + step * (double)bi;
                    double L2 = bound_min + step * (double)bj;
                    double best_std = *(double*)get_element_a(stds, (size_t[]){best_idx});

                    set_element_a(p_best_l_1, (size_t[]){count}, &L1);
                    set_element_a(p_best_l_2, (size_t[]){count}, &L2);
                    set_element_a(p_best_mean, (size_t[]){count}, &best_mean);
                    set_element_a(p_best_std, (size_t[]){count}, &best_std);
                    count += 1;

                    free_a(means);
                    free_a(stds);
                }
            }
        }
    }

    save_to_file_a(p_best_l_1, p_best_dir(directory), "lm_non_adaptive_l1_l2_normal_p_best_l_1");
    save_to_file_a(p_best_l_2, p_best_dir(directory), "lm_non_adaptive_l1_l2_normal_p_best_l_2");
    save_to_file_a(p_best_mean, p_best_dir(directory), "lm_non_adaptive_l1_l2_normal_p_best_mean");
    save_to_file_a(p_best_std, p_best_dir(directory), "lm_non_adaptive_l1_l2_normal_p_best_std");

    free_a(p_best_l_1);
    free_a(p_best_l_2);
    free_a(p_best_mean);
    free_a(p_best_std);
}

/**
 * Per-experiment grid best (lambda_1, lambda_2) for StEFCal improved. Identical to
 * extract_normal_p_best_stefcal but reads the data_stefcal_improved_order_<O> heatmaps and
 * writes stefcal_improved_normal_p_best_{l_1,l_2,mean,std}.array.
 */

/**
 * Per-experiment grid best (lambda_1, lambda_2) for StEFCal improved. Identical to
 * extract_normal_p_best_stefcal but reads the data_stefcal_improved_order_<O> heatmaps and
 * writes stefcal_improved_normal_p_best_{l_1,l_2,mean,std}.array.
 */
void extract_normal_p_best_stefcal_improved(char* directory, int* order_min, int* order_max) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    double bound_min = -0.5;
    double bound_max = 3.0;

    size_t total = (*order_max - *order_min + 1) * 4 * 4 * 4;
    struct array* p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_mean = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_std = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});

    size_t count = 0;
    for (int order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    char path_means[512];
                    sprintf(path_means, "%s/extracted_heatmap_data/data_order_%d/data_stefcal_improved_order_%d/seed_%d_order_%d_snr_%lf_max_amp_%lf/iteration_means.array",
                            directory, order, order, seeds[h], order, snrs[i], max_amps[j]);

                    char path_stds[512];
                    sprintf(path_stds, "%s/extracted_heatmap_data/data_order_%d/data_stefcal_improved_order_%d/seed_%d_order_%d_snr_%lf_max_amp_%lf/iteration_stds.array",
                            directory, order, order, seeds[h], order, snrs[i], max_amps[j]);

                    FILE* probe_means = fopen(path_means, "r");
                    FILE* probe_stds = fopen(path_stds, "r");
                    if (probe_means) fclose(probe_means);
                    if (probe_stds) fclose(probe_stds);
                    if (!probe_means || !probe_stds) {
                        printf("SKIP (no data): %s\n", path_means);
                        set_element_a(p_best_l_1, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_l_2, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_mean, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_std, (size_t[]){count}, &(double){NAN});
                        count += 1;
                        continue;
                    }

                    printf("%s\n", path_means);
                    struct array* means = read_from_file_a(path_means);
                    struct array* stds = read_from_file_a(path_stds);

                    size_t grid_length = (size_t)llround(sqrt((double)get_size_a(means)));

                    double step = (bound_max - bound_min) / (double)(grid_length - 1);
                    double best_mean = INFINITY;
                    size_t best_idx = 0;
                    int found = 0;
                    for (size_t m = 0; m < grid_length * grid_length; m++) {
                        size_t ci = m / grid_length;   // lambda_1 index
                        size_t cj = m % grid_length;   // lambda_2 index
                        double cL1 = bound_min + step * (double)ci;
                        double cL2 = bound_min + step * (double)cj;
                        /* Restrict the grid argmin to the region the PSO is allowed to occupy
                         * (random_position_bound2_pso / update_position_bounds_particle case
                         * STEFCAL_IMPROVED): l_1 + l_2 >= 0.5 and |l_1|, |l_2| >= 0.05. */
                        if (cL1 + cL2 < 0.5) continue;
                        if (fabs(cL1) < 0.05 || fabs(cL2) < 0.05) continue;
                        double v = *(double*)get_element_a(means, (size_t[]){m});
                        if (v < best_mean) {
                            best_mean = v;
                            best_idx = m;
                            found = 1;
                        }
                    }

                    if (!found) {
                        printf("SKIP (no feasible point with l_1 + l_2 >= 0.5 and |l_1|, |l_2| >= 0.05): %s\n", path_means);
                        set_element_a(p_best_l_1, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_l_2, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_mean, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_std, (size_t[]){count}, &(double){NAN});
                        count += 1;
                        free_a(means);
                        free_a(stds);
                        continue;
                    }

                    size_t bi = best_idx / grid_length;   // lambda_1 index
                    size_t bj = best_idx % grid_length;   // lambda_2 index
                    double L1 = bound_min + step * (double)bi;
                    double L2 = bound_min + step * (double)bj;
                    double best_std = *(double*)get_element_a(stds, (size_t[]){best_idx});

                    set_element_a(p_best_l_1, (size_t[]){count}, &L1);
                    set_element_a(p_best_l_2, (size_t[]){count}, &L2);
                    set_element_a(p_best_mean, (size_t[]){count}, &best_mean);
                    set_element_a(p_best_std, (size_t[]){count}, &best_std);
                    count += 1;

                    free_a(means);
                    free_a(stds);
                }
            }
        }
    }

    save_to_file_a(p_best_l_1, p_best_dir(directory), "stefcal_improved_normal_p_best_l_1");
    save_to_file_a(p_best_l_2, p_best_dir(directory), "stefcal_improved_normal_p_best_l_2");
    save_to_file_a(p_best_mean, p_best_dir(directory), "stefcal_improved_normal_p_best_mean");
    save_to_file_a(p_best_std, p_best_dir(directory), "stefcal_improved_normal_p_best_std");

    free_a(p_best_l_1);
    free_a(p_best_l_2);
    free_a(p_best_mean);
    free_a(p_best_std);
}

/**
 * Per-experiment PSO best (B1, B2) — placeholder using the 3-D ADAM PSO results.
 * Reads positions[1] = B1, positions[2] = B2 from data_pso_adam_order_<O>_seed_<S>
 * and selects across the 4 PSO seeds with the standard tiebreak
 * (iter_mean -> iter_std -> residual_norm). No symmetry swap.
 *
 * Output filenames match extract_trace_specific:
 *   pso_p_best_l_1.array, pso_p_best_l_2.array,
 *   pso_p_best_mean.array, pso_p_best_std.array
 */

/**
 * Per-experiment PSO best (B1, B2) — placeholder using the 3-D ADAM PSO results.
 * Reads positions[1] = B1, positions[2] = B2 from data_pso_adam_order_<O>_seed_<S>
 * and selects across the 4 PSO seeds with the standard tiebreak
 * (iter_mean -> iter_std -> residual_norm). No symmetry swap.
 *
 * Output filenames match extract_trace_specific:
 *   pso_p_best_l_1.array, pso_p_best_l_2.array,
 *   pso_p_best_mean.array, pso_p_best_std.array
 */
void extract_pso_p_best_adam_3d_b1_b2(char* directory, int* order_min, int* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    size_t total = (*order_max - *order_min + 1) * 4 * 4 * 4;
    struct array* p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_mean = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_std = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});

    size_t count = 0;
    for (int order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_B1 = NAN;
                    double best_B2 = NAN;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%d/data_pso_adam_order_%d_seed_%d/seed_%d_order_%d_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%d/data_pso_adam_order_%d_seed_%d/seed_%d_order_%d_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double B1 = *(double*)get_element_a(positions_array, (size_t[]){1});  // skip alpha at index 0
                        double B2 = *(double*)get_element_a(positions_array, (size_t[]){2});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_B1 = B1;
                            best_B2 = B2;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_B1 = B1;
                                best_B2 = B2;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_B1 = B1;
                                    best_B2 = B2;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    set_element_a(p_best_l_1, (size_t[]){count}, &best_B1);
                    set_element_a(p_best_l_2, (size_t[]){count}, &best_B2);
                    set_element_a(p_best_mean, (size_t[]){count}, &best_iteration_mean);
                    set_element_a(p_best_std, (size_t[]){count}, &best_iteration_std);
                    count += 1;
                }
            }
        }
    }

    save_to_file_a(p_best_l_1, p_best_dir(directory), "pso_p_best_l_1");
    save_to_file_a(p_best_l_2, p_best_dir(directory), "pso_p_best_l_2");
    save_to_file_a(p_best_mean, p_best_dir(directory), "pso_p_best_mean");
    save_to_file_a(p_best_std, p_best_dir(directory), "pso_p_best_std");

    free_a(p_best_l_1);
    free_a(p_best_l_2);
    free_a(p_best_mean);
    free_a(p_best_std);
}

/**
 * Per-experiment PSO best (B1, B2) — final version using the 2-D
 * adam_fixed_alpha PSO results. Identical structure to
 * extract_pso_p_best_adam_3d_b1_b2 but reads positions[0] = B1,
 * positions[1] = B2 from data_pso_adam_fixed_alpha_order_<O>_seed_<S>.
 * Use this once the 2-D PSO has completed.
 */

/**
 * Per-experiment PSO best (B1, B2) — final version using the 2-D
 * adam_fixed_alpha PSO results. Identical structure to
 * extract_pso_p_best_adam_3d_b1_b2 but reads positions[0] = B1,
 * positions[1] = B2 from data_pso_adam_fixed_alpha_order_<O>_seed_<S>.
 * Use this once the 2-D PSO has completed.
 */
void extract_pso_p_best_adam_fixed_alpha(char* directory, int* order_min, int* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    size_t total = (*order_max - *order_min + 1) * 4 * 4 * 4;
    struct array* p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_mean = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_std = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});

    size_t count = 0;
    for (int order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_B1 = NAN;
                    double best_B2 = NAN;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%d/data_pso_adam_fixed_alpha_order_%d_seed_%d/seed_%d_order_%d_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%d/data_pso_adam_fixed_alpha_order_%d_seed_%d/seed_%d_order_%d_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double B1 = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double B2 = *(double*)get_element_a(positions_array, (size_t[]){1});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_B1 = B1;
                            best_B2 = B2;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_B1 = B1;
                                best_B2 = B2;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_B1 = B1;
                                    best_B2 = B2;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    set_element_a(p_best_l_1, (size_t[]){count}, &best_B1);
                    set_element_a(p_best_l_2, (size_t[]){count}, &best_B2);
                    set_element_a(p_best_mean, (size_t[]){count}, &best_iteration_mean);
                    set_element_a(p_best_std, (size_t[]){count}, &best_iteration_std);
                    count += 1;
                }
            }
        }
    }

    save_to_file_a(p_best_l_1, p_best_dir(directory), "pso_p_best_l_1");
    save_to_file_a(p_best_l_2, p_best_dir(directory), "pso_p_best_l_2");
    save_to_file_a(p_best_mean, p_best_dir(directory), "pso_p_best_mean");
    save_to_file_a(p_best_std, p_best_dir(directory), "pso_p_best_std");

    free_a(p_best_l_1);
    free_a(p_best_l_2);
    free_a(p_best_mean);
    free_a(p_best_std);
}

/**
 * Per-experiment grid best (l1, l2) for the stochastic-adaptive fixed-b StEFCal.
 * Mirror of extract_normal_p_best_stefcal: reads the data_stochastic_fixed_b_order_<O>
 * heatmaps, picks the grid cell with the lowest mean iterations, maps the index
 * back to (l1, l2) over the [0.001, 3] grid bounds used by
 * mapping_stochastic_fixed_b_local, and writes
 * stochastic_fixed_b_normal_p_best_{l_1,l_2,mean,std}.array. Missing experiments
 * are recorded as NaN and skipped.
 */

/**
 * Per-experiment grid best (l1, l2) for the stochastic-adaptive fixed-b StEFCal.
 * Mirror of extract_normal_p_best_stefcal: reads the data_stochastic_fixed_b_order_<O>
 * heatmaps, picks the grid cell with the lowest mean iterations, maps the index
 * back to (l1, l2) over the [0.001, 3] grid bounds used by
 * mapping_stochastic_fixed_b_local, and writes
 * stochastic_fixed_b_normal_p_best_{l_1,l_2,mean,std}.array. Missing experiments
 * are recorded as NaN and skipped.
 */
void extract_normal_p_best_stochastic_fixed_b(char* directory, int* order_min, int* order_max) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    /* Sweep extent of the stored heatmaps, used to turn a grid index back into a lambda.
     * This is the gridding domain, NOT the feasibility floor: mapping_stochastic_fixed_b_local
     * sweeps [-0.5, 3] at 200 nodes (the HPC variant's [0.001, 3] is 50 nodes, and these
     * arrays are 200x200), and the 2-D figure is drawn with extent [-0.5, 3] to match.
     * It was 0.001, which mislabelled every recovered coordinate and, worse, made the
     * feasibility test below select cells from the excluded lambda ~ 0 region while
     * believing they were legal. The > 0.001 test stays as it is; it only does any work
     * once the coordinates are right. */
    double bound_min = -0.5;
    double bound_max = 3.0;

    size_t total = (*order_max - *order_min + 1) * 4 * 4 * 4;
    struct array* p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_mean = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_std = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});

    size_t count = 0;
    for (int order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    char path_means[512];
                    sprintf(path_means, "%s/extracted_heatmap_data/data_order_%d/data_stochastic_fixed_b_order_%d/seed_%d_order_%d_snr_%lf_max_amp_%lf/iteration_means.array",
                            directory, order, order, seeds[h], order, snrs[i], max_amps[j]);

                    char path_stds[512];
                    sprintf(path_stds, "%s/extracted_heatmap_data/data_order_%d/data_stochastic_fixed_b_order_%d/seed_%d_order_%d_snr_%lf_max_amp_%lf/iteration_stds.array",
                            directory, order, order, seeds[h], order, snrs[i], max_amps[j]);

                    FILE* probe_means = fopen(path_means, "r");
                    FILE* probe_stds = fopen(path_stds, "r");
                    if (probe_means) fclose(probe_means);
                    if (probe_stds) fclose(probe_stds);
                    if (!probe_means || !probe_stds) {
                        printf("SKIP (no data): %s\n", path_means);
                        set_element_a(p_best_l_1, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_l_2, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_mean, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_std, (size_t[]){count}, &(double){NAN});
                        count += 1;
                        continue;
                    }

                    printf("%s\n", path_means);
                    struct array* means = read_from_file_a(path_means);
                    struct array* stds = read_from_file_a(path_stds);

                    size_t grid_length = (size_t)llround(sqrt((double)get_size_a(means)));

                    double step = (bound_max - bound_min) / (double)(grid_length - 1);
                    double best_mean = INFINITY;
                    size_t best_idx = 0;
                    int found = 0;
                    for (size_t m = 0; m < grid_length * grid_length; m++) {
                        size_t ci = m / grid_length;   // l1 index
                        size_t cj = m % grid_length;   // l2 index
                        double cL1 = bound_min + step * (double)ci;
                        double cL2 = bound_min + step * (double)cj;
                        if (cL1 + cL2 < 0.5) continue;   // keep only points above y = -x + 0.5 (i.e. l_1 + l_2 >= 0.5)
                        if (cL1 <= 0.001 || cL2 <= 0.001) continue;   // must be strictly above x = 0.001 and y = 0.001
                        double v = *(double*)get_element_a(means, (size_t[]){m});
                        if (v < best_mean) {
                            best_mean = v;
                            best_idx = m;
                            found = 1;
                        }
                    }

                    if (!found) {
                        printf("SKIP (no feasible point with l_2 >= l_1 + 0.05): %s\n", path_means);
                        set_element_a(p_best_l_1, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_l_2, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_mean, (size_t[]){count}, &(double){NAN});
                        set_element_a(p_best_std, (size_t[]){count}, &(double){NAN});
                        count += 1;
                        free_a(means);
                        free_a(stds);
                        continue;
                    }

                    size_t bi = best_idx / grid_length;   // l1 index
                    size_t bj = best_idx % grid_length;   // l2 index
                    double L1 = bound_min + step * (double)bi;
                    double L2 = bound_min + step * (double)bj;
                    double best_std = *(double*)get_element_a(stds, (size_t[]){best_idx});

                    set_element_a(p_best_l_1, (size_t[]){count}, &L1);
                    set_element_a(p_best_l_2, (size_t[]){count}, &L2);
                    set_element_a(p_best_mean, (size_t[]){count}, &best_mean);
                    set_element_a(p_best_std, (size_t[]){count}, &best_std);
                    count += 1;

                    free_a(means);
                    free_a(stds);
                }
            }
        }
    }

    save_to_file_a(p_best_l_1, p_best_dir(directory), "stochastic_fixed_b_normal_p_best_l_1");
    save_to_file_a(p_best_l_2, p_best_dir(directory), "stochastic_fixed_b_normal_p_best_l_2");
    save_to_file_a(p_best_mean, p_best_dir(directory), "stochastic_fixed_b_normal_p_best_mean");
    save_to_file_a(p_best_std, p_best_dir(directory), "stochastic_fixed_b_normal_p_best_std");

    free_a(p_best_l_1);
    free_a(p_best_l_2);
    free_a(p_best_mean);
    free_a(p_best_std);
}

/**
 * Per-experiment PSO best (l1, l2) for the stochastic-adaptive fixed-b StEFCal.
 * Mirror of extract_pso_p_best_adam_fixed_alpha: across the 4 PSO seeds reads
 * positions[0]=l1, positions[1]=l2 and double_data {iter_mean, iter_std,
 * residual_norm}, selects the best by (iter_mean -> iter_std -> residual_norm),
 * and writes stochastic_fixed_b_pso_p_best_{l_1,l_2,mean,std}.array. Missing
 * experiments are recorded as NaN and skipped.
 */

/**
 * Per-experiment PSO best (l1, l2) for the stochastic-adaptive fixed-b StEFCal.
 * Mirror of extract_pso_p_best_adam_fixed_alpha: across the 4 PSO seeds reads
 * positions[0]=l1, positions[1]=l2 and double_data {iter_mean, iter_std,
 * residual_norm}, selects the best by (iter_mean -> iter_std -> residual_norm),
 * and writes stochastic_fixed_b_pso_p_best_{l_1,l_2,mean,std}.array. Missing
 * experiments are recorded as NaN and skipped.
 */
void extract_pso_p_best_stochastic_fixed_b(char* directory, int* order_min, int* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    size_t total = (*order_max - *order_min + 1) * 4 * 4 * 4;
    struct array* p_best_l_1 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_l_2 = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_mean = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});
    struct array* p_best_std = zeros_a(&(size_t){1}, (size_t[]){total}, &(enum DataType){DOUBLE});

    size_t count = 0;
    for (int order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_l1 = NAN;
                    double best_l2 = NAN;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%d/data_pso_stochastic_fixed_b_order_%d_seed_%d/seed_%d_order_%d_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%d/data_pso_stochastic_fixed_b_order_%d_seed_%d/seed_%d_order_%d_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);

                        FILE* probe_d = fopen(path_doubles, "r");
                        FILE* probe_p = fopen(path_positions, "r");
                        if (probe_d) fclose(probe_d);
                        if (probe_p) fclose(probe_p);
                        if (!probe_d || !probe_p) {
                            printf("SKIP (no data): %s\n", path_doubles);
                            continue;
                        }

                        struct array* doubles_array = read_from_file_a(path_doubles);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double l1 = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double l2 = *(double*)get_element_a(positions_array, (size_t[]){1});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_l1 = l1;
                            best_l2 = l2;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_l1 = l1;
                                best_l2 = l2;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_l1 = l1;
                                    best_l2 = l2;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    if (isinf(best_iteration_mean)) best_iteration_mean = NAN;
                    if (isinf(best_iteration_std)) best_iteration_std = NAN;

                    set_element_a(p_best_l_1, (size_t[]){count}, &best_l1);
                    set_element_a(p_best_l_2, (size_t[]){count}, &best_l2);
                    set_element_a(p_best_mean, (size_t[]){count}, &best_iteration_mean);
                    set_element_a(p_best_std, (size_t[]){count}, &best_iteration_std);
                    count += 1;
                }
            }
        }
    }

    save_to_file_a(p_best_l_1, p_best_dir(directory), "stochastic_fixed_b_pso_p_best_l_1");
    save_to_file_a(p_best_l_2, p_best_dir(directory), "stochastic_fixed_b_pso_p_best_l_2");
    save_to_file_a(p_best_mean, p_best_dir(directory), "stochastic_fixed_b_pso_p_best_mean");
    save_to_file_a(p_best_std, p_best_dir(directory), "stochastic_fixed_b_pso_p_best_std");

    free_a(p_best_l_1);
    free_a(p_best_l_2);
    free_a(p_best_mean);
    free_a(p_best_std);
}

/**
 * For each (seed, snr, max_amp, order):
 *   1. Reads adam_fixed_alpha PSO results across 4 pso_seeds, picks the best
 *      (B1, B2) by (iter_mean -> iter_std -> residual_norm).
 *   2. Reads the per-experiment fixed_alpha from
 *      {directory}/data_optimal_solutions/adam_best_alpha/data_order_<O>/seed_<>_snr_<>_maxamp_<>.array
 *   3. Reads the simulator and runs solve_G_ADAM_without_r(cal, IMAX, TAU,
 *      &fixed_alpha, &best_B1, &best_B2).
 *   4. Saves iterations.array, all_gains.jarray, damping_g.jarray,
 *      damping_g_old.jarray, last_gains.jarray, last_residuals.jarray to
 *      {directory}/data_optimal_solutions/adam_fixed_alpha_optimal/data_order_<O>/
 *        data_adam_fixed_alpha_order_<O>_seed_<>_snr_<>_maxamp_<>/
 *
 * Mirror of extraction_optimal_lm_non_adaptive_l1_l2 (src/main.c:3341) with
 * the deltas listed above. No symmetry swap (B1/B2 are not symmetric).
 */
/**
 * Re-measure optimal ADAM at the hyperparameters found by the 3-PARAMETER PSO.
 *
 * extraction_optimal_adam_fixed_alpha takes (B1, B2) from data_pso_adam_fixed_alpha_*.
 * That family is unusable: ADAM_FIXED_ALPHA was missing from the switch in
 * update_position_bounds_particle, so its swarm never moved and its "optimum" is the best
 * of a random initial sample. This variant reads data_pso_adam_* instead, taking
 * alpha = positions[0], B1 = positions[1] and B2 = positions[2] from the run that wins on
 * iteration_mean -> iteration_std -> residual_norm across the 4 PSO seeds.
 *
 * Everything else -- the solve, the collected arrays and the output path -- is unchanged,
 * so the downstream steps (create_x_y_points_from_optimal_adam_fixed_alpha,
 * extract_all_iterations_optimal_from_data_optimal_solutions) need no modification.
 *
 * NOTE: it writes to the SAME data_optimal_solutions/adam_fixed_alpha_optimal tree and
 * therefore OVERWRITES the previous measurements. Back that tree up first if you want to
 * keep the old numbers for comparison.
 */

/**
 * Re-measure optimal ADAM at the hyperparameters found by the 3-PARAMETER PSO.
 *
 * extraction_optimal_adam_fixed_alpha takes (B1, B2) from data_pso_adam_fixed_alpha_*.
 * That family is unusable: ADAM_FIXED_ALPHA was missing from the switch in
 * update_position_bounds_particle, so its swarm never moved and its "optimum" is the best
 * of a random initial sample. This variant reads data_pso_adam_* instead, taking
 * alpha = positions[0], B1 = positions[1] and B2 = positions[2] from the run that wins on
 * iteration_mean -> iteration_std -> residual_norm across the 4 PSO seeds.
 *
 * Everything else -- the solve, the collected arrays and the output path -- is unchanged,
 * so the downstream steps (create_x_y_points_from_optimal_adam_fixed_alpha,
 * extract_all_iterations_optimal_from_data_optimal_solutions) need no modification.
 *
 * NOTE: it writes to the SAME data_optimal_solutions/adam_fixed_alpha_optimal tree and
 * therefore OVERWRITES the previous measurements. Back that tree up first if you want to
 * keep the old numbers for comparison.
 */
void extraction_optimal_adam_3d(char* directory, size_t* order_min, size_t* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    int num_workers = workers(28);

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_B1 = INFINITY;
                    double best_B2 = INFINITY;
                    double best_alpha = INFINITY;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%ld/data_pso_adam_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%ld/data_pso_adam_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_alpha = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double position_B1 = *(double*)get_element_a(positions_array, (size_t[]){1});
                        double position_B2 = *(double*)get_element_a(positions_array, (size_t[]){2});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_B1 = position_B1;
                            best_B2 = position_B2;
                            best_alpha = position_alpha;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_B1 = position_B1;
                                best_B2 = position_B2;
                                best_alpha = position_alpha;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_B1 = position_B1;
                                    best_B2 = position_B2;
                                    best_alpha = position_alpha;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    /* alpha comes from the same winning run as (B1, B2), rather than from
                     * data_optimal_solutions/adam_best_alpha. The two agree, because
                     * extract_best_alpha_adam selects with the identical rule, but taking all
                     * three from one run makes the triple self-consistent by construction. */
                    double fixed_alpha = best_alpha;

                    char simulator_path[512];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig",
                             directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    solve_G_ADAM_without_r(calibrator, &(size_t){IMAX}, &(double){TAU}, &fixed_alpha, &best_B1, &best_B2);

                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector);
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);

                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/adam_fixed_alpha_optimal/data_order_%ld/data_adam_fixed_alpha_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f",
                            directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }
}

void extraction_optimal_adam_fixed_alpha(char* directory, size_t* order_min, size_t* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    int num_workers = workers(28);

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_B1 = INFINITY;
                    double best_B2 = INFINITY;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%ld/data_pso_adam_fixed_alpha_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%ld/data_pso_adam_fixed_alpha_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_B1 = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double position_B2 = *(double*)get_element_a(positions_array, (size_t[]){1});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_B1 = position_B1;
                            best_B2 = position_B2;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_B1 = position_B1;
                                best_B2 = position_B2;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_B1 = position_B1;
                                    best_B2 = position_B2;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    char alpha_path[512];
                    sprintf(alpha_path, "%s/data_optimal_solutions/adam_best_alpha/data_order_%ld/seed_%d_snr_%.0f_maxamp_%.0f.array",
                            directory, order, seeds[h], snrs[i], max_amps[j]);
                    struct array* alpha_array = read_from_file_a(alpha_path);
                    double fixed_alpha = *(double*)get_element_a(alpha_array, (size_t[]){0});
                    free_a(alpha_array);

                    char simulator_path[512];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig",
                             directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    solve_G_ADAM_without_r(calibrator, &(size_t){IMAX}, &(double){TAU}, &fixed_alpha, &best_B1, &best_B2);

                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector);
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);

                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/adam_fixed_alpha_optimal/data_order_%ld/data_adam_fixed_alpha_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f",
                            directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }
}

/**
 * Baseline pipeline for adam_fixed_alpha: runs ADAM at the default Kingma & Ba
 * (2014) values B1 = 0.9, B2 = 0.999, with the same per-experiment fixed alpha
 * the optimal pipeline reads. Saves the same 5 calibrator output files but
 * under {directory}/data_optimal_solutions/adam_fixed_alpha_original/...
 */

/**
 * Baseline pipeline for adam_fixed_alpha: runs ADAM at the default Kingma & Ba
 * (2014) values B1 = 0.9, B2 = 0.999, with the same per-experiment fixed alpha
 * the optimal pipeline reads. Saves the same 5 calibrator output files but
 * under {directory}/data_optimal_solutions/adam_fixed_alpha_original/...
 */
void extraction_original_adam_fixed_alpha(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    int num_workers = workers(28);

    double default_B1 = 0.9;
    double default_B2 = 0.999;

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    char alpha_path[512];
                    sprintf(alpha_path, "%s/data_optimal_solutions/adam_best_alpha/data_order_%ld/seed_%d_snr_%.0f_maxamp_%.0f.array",
                            directory, order, seeds[h], snrs[i], max_amps[j]);
                    struct array* alpha_array = read_from_file_a(alpha_path);
                    double fixed_alpha = *(double*)get_element_a(alpha_array, (size_t[]){0});
                    free_a(alpha_array);

                    char simulator_path[512];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig",
                             directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    solve_G_ADAM_without_r(calibrator, &(size_t){IMAX}, &(double){TAU}, &fixed_alpha, &default_B1, &default_B2);

                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector);
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);

                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/adam_fixed_alpha_original/data_order_%ld/data_adam_fixed_alpha_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f",
                            directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }
}

/**
 * Optimal pipeline for the stochastic-adaptive StEFCal (l1,l2,b) study. For each of the 64
 * experiments per order it takes the best (l1,l2) from the round-2a fixed-b PSO
 * (data_pso_stochastic_fixed_b_*, positions[0],[1], best across the 4 PSO seeds by
 * iteration_mean -> std -> residual_norm) and the fixed b0 from stochastic_best_b, runs
 * solve_G_StEFCal_stochastic_adaptive_l1l2, and saves the calibrator outputs under
 *   {directory}/data_optimal_solutions/stochastic_fixed_b_optimal/...
 * (naming kept exactly aligned with extract_all_iterations_*_from_data_optimal_solutions using
 *  filename "stochastic_fixed_b").
 */

/**
 * Optimal pipeline for the stochastic-adaptive StEFCal (l1,l2,b) study. For each of the 64
 * experiments per order it takes the best (l1,l2) from the round-2a fixed-b PSO
 * (data_pso_stochastic_fixed_b_*, positions[0],[1], best across the 4 PSO seeds by
 * iteration_mean -> std -> residual_norm) and the fixed b0 from stochastic_best_b, runs
 * solve_G_StEFCal_stochastic_adaptive_l1l2, and saves the calibrator outputs under
 *   {directory}/data_optimal_solutions/stochastic_fixed_b_optimal/...
 * (naming kept exactly aligned with extract_all_iterations_*_from_data_optimal_solutions using
 *  filename "stochastic_fixed_b").
 */
void extraction_optimal_stochastic(char* directory, size_t* order_min, size_t* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    int num_workers = workers(28);

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_l1 = INFINITY;
                    double best_l2 = INFINITY;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%ld/data_pso_stochastic_fixed_b_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%ld/data_pso_stochastic_fixed_b_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_l1 = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double position_l2 = *(double*)get_element_a(positions_array, (size_t[]){1});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_l1 = position_l1;
                            best_l2 = position_l2;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_l1 = position_l1;
                                best_l2 = position_l2;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_l1 = position_l1;
                                    best_l2 = position_l2;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    char b_path[512];
                    sprintf(b_path, "%s/data_optimal_solutions/stochastic_best_b/data_order_%ld/seed_%d_snr_%.0f_maxamp_%.0f.array",
                            directory, order, seeds[h], snrs[i], max_amps[j]);
                    struct array* b_array = read_from_file_a(b_path);
                    size_t b0 = (size_t)round(*(double*)get_element_a(b_array, (size_t[]){0}));
                    free_a(b_array);

                    char simulator_path[512];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig",
                             directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    solve_G_StEFCal_stochastic_adaptive_l1l2(calibrator, &(size_t){IMAX}, &(double){TAU}, &best_l1, &best_l2, &b0, &(int){0});

                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector);
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);
                    struct JaggedArray* b_values = get_b_values_dc(calibrator->datacollector);

                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/stochastic_fixed_b_optimal/data_order_%ld/data_stochastic_fixed_b_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f",
                            directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");
                    save_to_file_ja(b_values, directory_save, "b_values");

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }
}

/**
 * Baseline pipeline for the stochastic-adaptive StEFCal study: runs the untuned defaults
 * l1 = l2 = 0.5 and b0 = 2 (no PSO selection). Saves the same calibrator outputs under
 *   {directory}/data_optimal_solutions/stochastic_fixed_b_original/...
 */

/**
 * Baseline pipeline for the stochastic-adaptive StEFCal study: runs the untuned defaults
 * l1 = l2 = 0.5 and b0 = 2 (no PSO selection). Saves the same calibrator outputs under
 *   {directory}/data_optimal_solutions/stochastic_fixed_b_original/...
 */
void extraction_original_stochastic(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    int num_workers = workers(28);

    double default_l1 = 0.5;
    double default_l2 = 0.5;

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    size_t b0 = 2;

                    char simulator_path[512];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig",
                             directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    solve_G_StEFCal_stochastic_adaptive_l1l2(calibrator, &(size_t){IMAX}, &(double){TAU}, &default_l1, &default_l2, &b0, &(int){0});

                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector);
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);
                    struct JaggedArray* b_values = get_b_values_dc(calibrator->datacollector);

                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/stochastic_fixed_b_original/data_order_%ld/data_stochastic_fixed_b_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f",
                            directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");
                    save_to_file_ja(b_values, directory_save, "b_values");

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }
}

/**
 * Aggregator: walks the 64 experiments × 600 timesteps written by the
 * extraction step (either _optimal or _original — pulled out into a helper
 * to share between the two final aggregators) and writes the 4 stat .txt
 * files the python display script reads.
 *
 * `variant` should be either "optimal" or "original" — controls both the
 * input tree and the output tree.
 */

/**
 * Aggregator: walks the 64 experiments × 600 timesteps written by the
 * extraction step (either _optimal or _original — pulled out into a helper
 * to share between the two final aggregators) and writes the 4 stat .txt
 * files the python display script reads.
 *
 * `variant` should be either "optimal" or "original" — controls both the
 * input tree and the output tree.
 */
static void create_x_y_points_adam_fixed_alpha_variant(char* directory, size_t* order_min, size_t* order_max, const char* variant) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    for (size_t order = *order_min; order <= *order_max; order++) {

        double sum_iterations = 0;
        double sum_residual_norm = 0;
        size_t count_iterations = 0;
        size_t count_residual_norm = 0;

        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/adam_fixed_alpha_%s/data_order_%ld/data_adam_fixed_alpha_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f",
                            directory, variant, order, order, seeds[h], snrs[i], max_amps[j]);

                    char directory_iterations_array[600];
                    sprintf(directory_iterations_array, "%s/iterations.array", directory_save);
                    char directory_last_residuals_jarray[600];
                    sprintf(directory_last_residuals_jarray, "%s/last_residuals.jarray", directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    struct JaggedArray* last_residuals_jarray = read_from_file_ja(directory_last_residuals_jarray);

                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_iterations += *element;
                        count_iterations += 1;
                    }

                    for (size_t y = 0; y < 600; y++) {
                        struct array* residual_array = get_ja(last_residuals_jarray, (size_t[]){y});
                        double* residual_norm = norm_a(residual_array);
                        sum_residual_norm += *residual_norm;
                        free(residual_norm);
                        count_residual_norm += 1;
                    }

                    free_a(iterations_array);
                    free_ja(last_residuals_jarray);
                }
            }
        }

        double mean_iterations = sum_iterations / count_iterations;
        double mean_last_residual_norm = sum_residual_norm / count_residual_norm;

        double sum_of_squared_deviation = 0;
        double sum_of_absolute_deviation = 0;

        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/adam_fixed_alpha_%s/data_order_%ld/data_adam_fixed_alpha_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f",
                            directory, variant, order, order, seeds[h], snrs[i], max_amps[j]);

                    char directory_iterations_array[600];
                    sprintf(directory_iterations_array, "%s/iterations.array", directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_of_squared_deviation += pow((*element - mean_iterations), 2.0);
                        sum_of_absolute_deviation += fabs(*element - mean_iterations);
                    }

                    free_a(iterations_array);
                }
            }
        }

        double std_iterations = sqrt(sum_of_squared_deviation / count_iterations);
        double mad_iterations = sum_of_absolute_deviation / count_iterations;

        printf("ADAM Fixed Alpha [%s] Order = %ld\n", variant, order);
        printf("  Mean = %f\n", mean_iterations);
        printf("  Std  = %f\n", std_iterations);
        printf("  MAD  = %f\n", mad_iterations);
        printf("  Mean Last Residual Norm = %f\n", mean_last_residual_norm);
        printf("\n");

        char directory_out[400];
        sprintf(directory_out, "%s/data_optimal_solutions_extracted_data_points/adam_fixed_alpha_%s/data_order_%ld", directory, variant, order);
        create_directories(directory_out);

        char path_mean[600];
        sprintf(path_mean, "%s/iterations_mean.txt", directory_out);
        char path_std[600];
        sprintf(path_std, "%s/iterations_std.txt", directory_out);
        char path_mad[600];
        sprintf(path_mad, "%s/iterations_mad.txt", directory_out);
        char path_res[600];
        sprintf(path_res, "%s/mean_last_residual_norm.txt", directory_out);

        FILE *fp1 = open_output(path_mean, "w");
        if (fp1 == NULL) { printf("Error opening %s\n", path_mean); return; }
        fprintf(fp1, "%f\n", mean_iterations);
        fclose(fp1);

        FILE *fp2 = open_output(path_std, "w");
        if (fp2 == NULL) { printf("Error opening %s\n", path_std); return; }
        fprintf(fp2, "%f\n", std_iterations);
        fclose(fp2);

        FILE *fp3 = open_output(path_mad, "w");
        if (fp3 == NULL) { printf("Error opening %s\n", path_mad); return; }
        fprintf(fp3, "%f\n", mad_iterations);
        fclose(fp3);

        FILE *fp4 = open_output(path_res, "w");
        if (fp4 == NULL) { printf("Error opening %s\n", path_res); return; }
        fprintf(fp4, "%f\n", mean_last_residual_norm);
        fclose(fp4);
    }
}

void create_x_y_points_from_optimal_adam_fixed_alpha(char* directory, size_t* order_min, size_t* order_max) {
    create_x_y_points_adam_fixed_alpha_variant(directory, order_min, order_max, "optimal");
}

void create_x_y_points_from_original_adam_fixed_alpha(char* directory, size_t* order_min, size_t* order_max) {
    create_x_y_points_adam_fixed_alpha_variant(directory, order_min, order_max, "original");
}

/* Mirror of create_x_y_points_adam_fixed_alpha_variant for the stochastic-adaptive StEFCal study:
 * aggregates the per-experiment iterations.array + last_residuals.jarray under
 * data_optimal_solutions/stochastic_fixed_b_<variant>/ into the four per-order stat .txt files the
 * table script reads. Computation is identical to the adam/stefcal variants (only paths differ). */

/* Mirror of create_x_y_points_adam_fixed_alpha_variant for the stochastic-adaptive StEFCal study:
 * aggregates the per-experiment iterations.array + last_residuals.jarray under
 * data_optimal_solutions/stochastic_fixed_b_<variant>/ into the four per-order stat .txt files the
 * table script reads. Computation is identical to the adam/stefcal variants (only paths differ). */
static void create_x_y_points_stochastic_variant(char* directory, size_t* order_min, size_t* order_max, const char* variant) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    for (size_t order = *order_min; order <= *order_max; order++) {

        double sum_iterations = 0;
        double sum_residual_norm = 0;
        size_t count_iterations = 0;
        size_t count_residual_norm = 0;

        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/stochastic_fixed_b_%s/data_order_%ld/data_stochastic_fixed_b_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f",
                            directory, variant, order, order, seeds[h], snrs[i], max_amps[j]);

                    char directory_iterations_array[600];
                    sprintf(directory_iterations_array, "%s/iterations.array", directory_save);
                    char directory_last_residuals_jarray[600];
                    sprintf(directory_last_residuals_jarray, "%s/last_residuals.jarray", directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    struct JaggedArray* last_residuals_jarray = read_from_file_ja(directory_last_residuals_jarray);

                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_iterations += *element;
                        count_iterations += 1;
                    }

                    for (size_t y = 0; y < 600; y++) {
                        struct array* residual_array = get_ja(last_residuals_jarray, (size_t[]){y});
                        double* residual_norm = norm_a(residual_array);
                        sum_residual_norm += *residual_norm;
                        free(residual_norm);
                        count_residual_norm += 1;
                    }

                    free_a(iterations_array);
                    free_ja(last_residuals_jarray);
                }
            }
        }

        double mean_iterations = sum_iterations / count_iterations;
        double mean_last_residual_norm = sum_residual_norm / count_residual_norm;

        double sum_of_squared_deviation = 0;
        double sum_of_absolute_deviation = 0;

        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/stochastic_fixed_b_%s/data_order_%ld/data_stochastic_fixed_b_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f",
                            directory, variant, order, order, seeds[h], snrs[i], max_amps[j]);

                    char directory_iterations_array[600];
                    sprintf(directory_iterations_array, "%s/iterations.array", directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_of_squared_deviation += pow((*element - mean_iterations), 2.0);
                        sum_of_absolute_deviation += fabs(*element - mean_iterations);
                    }

                    free_a(iterations_array);
                }
            }
        }

        double std_iterations = sqrt(sum_of_squared_deviation / count_iterations);
        double mad_iterations = sum_of_absolute_deviation / count_iterations;

        printf("Stochastic Adaptive StEFCal [%s] Order = %ld\n", variant, order);
        printf("  Mean = %f\n", mean_iterations);
        printf("  Std  = %f\n", std_iterations);
        printf("  MAD  = %f\n", mad_iterations);
        printf("  Mean Last Residual Norm = %f\n", mean_last_residual_norm);
        printf("\n");

        char directory_out[400];
        sprintf(directory_out, "%s/data_optimal_solutions_extracted_data_points/stochastic_fixed_b_%s/data_order_%ld", directory, variant, order);
        create_directories(directory_out);

        char path_mean[600];
        sprintf(path_mean, "%s/iterations_mean.txt", directory_out);
        char path_std[600];
        sprintf(path_std, "%s/iterations_std.txt", directory_out);
        char path_mad[600];
        sprintf(path_mad, "%s/iterations_mad.txt", directory_out);
        char path_res[600];
        sprintf(path_res, "%s/mean_last_residual_norm.txt", directory_out);

        FILE *fp1 = open_output(path_mean, "w");
        if (fp1 == NULL) { printf("Error opening %s\n", path_mean); return; }
        fprintf(fp1, "%f\n", mean_iterations);
        fclose(fp1);

        FILE *fp2 = open_output(path_std, "w");
        if (fp2 == NULL) { printf("Error opening %s\n", path_std); return; }
        fprintf(fp2, "%f\n", std_iterations);
        fclose(fp2);

        FILE *fp3 = open_output(path_mad, "w");
        if (fp3 == NULL) { printf("Error opening %s\n", path_mad); return; }
        fprintf(fp3, "%f\n", mad_iterations);
        fclose(fp3);

        FILE *fp4 = open_output(path_res, "w");
        if (fp4 == NULL) { printf("Error opening %s\n", path_res); return; }
        fprintf(fp4, "%f\n", mean_last_residual_norm);
        fclose(fp4);
    }
}

void create_x_y_points_from_optimal_stochastic(char* directory, size_t* order_min, size_t* order_max) {
    create_x_y_points_stochastic_variant(directory, order_min, order_max, "optimal");
}

void create_x_y_points_from_original_stochastic(char* directory, size_t* order_min, size_t* order_max) {
    create_x_y_points_stochastic_variant(directory, order_min, order_max, "original");
}

/**
 * Optimal pipeline for the diagonal (l = l1 = l2) stochastic-adaptive StEFCal study.
 * For each experiment picks the best (lambda, b0) across the four PSO seeds by
 * mean -> std -> residual. The PSO searched STEFCAL_STOCHASTIC_ADAPTIVE, so
 * positions[0] = lambda and positions[1] = b0. Re-solves with the single-lambda solver
 * solve_G_StEFCal_stochastic_adaptive (l1 == l2 by construction, so the (0,1) off-diagonal
 * corner is unreachable) and saves the calibrator outputs under
 *   {directory}/data_optimal_solutions/stochastic_adaptive_optimal/...
 */

/**
 * Optimal pipeline for the diagonal (l = l1 = l2) stochastic-adaptive StEFCal study.
 * For each experiment picks the best (lambda, b0) across the four PSO seeds by
 * mean -> std -> residual. The PSO searched STEFCAL_STOCHASTIC_ADAPTIVE, so
 * positions[0] = lambda and positions[1] = b0. Re-solves with the single-lambda solver
 * solve_G_StEFCal_stochastic_adaptive (l1 == l2 by construction, so the (0,1) off-diagonal
 * corner is unreachable) and saves the calibrator outputs under
 *   {directory}/data_optimal_solutions/stochastic_adaptive_optimal/...
 */
void extraction_optimal_stochastic_adaptive(char* directory, size_t* order_min, size_t* order_max) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    int num_workers = workers(28);

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    double best_iteration_mean = INFINITY;
                    double best_iteration_std = INFINITY;
                    double best_residual_norm = INFINITY;
                    double best_lambda = INFINITY;
                    double best_b = INFINITY;

                    for (size_t k = 0; k < 4; k++) {
                        char path_doubles[512];
                        sprintf(path_doubles, "%s/data/data_order_%ld/data_pso_stochastic_adaptive_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/double_data.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* doubles_array = read_from_file_a(path_doubles);
                        char path_positions[512];
                        sprintf(path_positions, "%s/data/data_order_%ld/data_pso_stochastic_adaptive_order_%ld_seed_%d/seed_%d_order_%ld_snr_%.0f_maxamp_%.0f/positions.array",
                                directory, order, order, pso_seeds[k], seeds[h], order, snrs[i], max_amps[j]);
                        struct array* positions_array = read_from_file_a(path_positions);

                        double iteration_mean = *(double*)get_element_a(doubles_array, (size_t[]){0});
                        double iteration_std = *(double*)get_element_a(doubles_array, (size_t[]){1});
                        double residual_norm = *(double*)get_element_a(doubles_array, (size_t[]){2});
                        double position_lambda = *(double*)get_element_a(positions_array, (size_t[]){0});
                        double position_b = *(double*)get_element_a(positions_array, (size_t[]){1});

                        if (iteration_mean < best_iteration_mean) {
                            best_iteration_mean = iteration_mean;
                            best_iteration_std = iteration_std;
                            best_residual_norm = residual_norm;
                            best_lambda = position_lambda;
                            best_b = position_b;
                        } else if (iteration_mean == best_iteration_mean) {
                            if (iteration_std < best_iteration_std) {
                                best_iteration_mean = iteration_mean;
                                best_iteration_std = iteration_std;
                                best_residual_norm = residual_norm;
                                best_lambda = position_lambda;
                                best_b = position_b;
                            } else if (iteration_std == best_iteration_std) {
                                if (residual_norm < best_residual_norm) {
                                    best_iteration_mean = iteration_mean;
                                    best_iteration_std = iteration_std;
                                    best_residual_norm = residual_norm;
                                    best_lambda = position_lambda;
                                    best_b = position_b;
                                }
                            }
                        }
                        free_a(doubles_array);
                        free_a(positions_array);
                    }

                    size_t b0 = (size_t)round(best_b);

                    char simulator_path[512];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig",
                             directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    // Diagonal by construction: pass the single best lambda as both l1 and l2. The
                    // _l1l2 solver is identical to the single-lambda solver when l1 == l2, but it also
                    // captures the per-iteration damping_g/all_gains that the single-lambda solver omits,
                    // keeping the saved output structurally identical to the fixed-b extraction.
                    solve_G_StEFCal_stochastic_adaptive_l1l2(calibrator, &(size_t){IMAX}, &(double){TAU}, &best_lambda, &best_lambda, &b0, &(int){0});

                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector);
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);
                    struct JaggedArray* b_values = get_b_values_dc(calibrator->datacollector);

                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/stochastic_adaptive_optimal/data_order_%ld/data_stochastic_adaptive_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f",
                            directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");
                    save_to_file_ja(b_values, directory_save, "b_values");

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }
}

/**
 * Baseline pipeline for the diagonal stochastic-adaptive StEFCal study: runs the untuned
 * default lambda = 0.5 (l1 = l2 = 0.5) and b0 = 2 (no PSO selection). Saves the same calibrator
 * outputs under {directory}/data_optimal_solutions/stochastic_adaptive_original/...
 */

/**
 * Baseline pipeline for the diagonal stochastic-adaptive StEFCal study: runs the untuned
 * default lambda = 0.5 (l1 = l2 = 0.5) and b0 = 2 (no PSO selection). Saves the same calibrator
 * outputs under {directory}/data_optimal_solutions/stochastic_adaptive_original/...
 */
void extraction_original_stochastic_adaptive(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    int num_workers = workers(28);

    double default_lambda = 0.5;

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    size_t b0 = 2;

                    char simulator_path[512];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig",
                             directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, &num_workers);
                    // Diagonal by construction (l1 == l2 == default_lambda); _l1l2 solver used for the
                    // per-iteration capture, identical math to the single-lambda solver at l1 == l2.
                    solve_G_StEFCal_stochastic_adaptive_l1l2(calibrator, &(size_t){IMAX}, &(double){TAU}, &default_lambda, &default_lambda, &b0, &(int){0});

                    struct array* iterations = get_iterations_dc(calibrator->datacollector);
                    struct JaggedArray* all_gains = get_all_gain_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g = get_damping_g_dc(calibrator->datacollector);
                    struct JaggedArray* damping_g_old = get_damping_g_old_dc(calibrator->datacollector);
                    struct JaggedArray* last_gains = get_last_gains_dc(calibrator->datacollector);
                    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);
                    struct JaggedArray* b_values = get_b_values_dc(calibrator->datacollector);

                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/stochastic_adaptive_original/data_order_%ld/data_stochastic_adaptive_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f",
                            directory, order, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", directory_save);
                    create_directories(directory_save);

                    save_to_file_a(iterations, directory_save, "iterations");
                    save_to_file_ja(all_gains, directory_save, "all_gains");
                    save_to_file_ja(damping_g, directory_save, "damping_g");
                    save_to_file_ja(damping_g_old, directory_save, "damping_g_old");
                    save_to_file_ja(last_gains, directory_save, "last_gains");
                    save_to_file_ja(last_residuals, directory_save, "last_residuals");
                    save_to_file_ja(b_values, directory_save, "b_values");

                    free_ja(all_gains);
                    free_c(calibrator);
                    free_s(simulator);
                }
            }
        }
    }
}

/* Returns 1 if the file at `path` exists and is readable, else 0. */

/* Returns 1 if the file at `path` exists and is readable, else 0. */
static int file_exists_er(const char* path) {
    FILE* f = fopen(path, "r");
    if (f) { fclose(f); return 1; }
    return 0;
}

/* Load the best PSO-tuned adaptive-LM hyperparameters (l_init, v) for one error configuration,
 * selected as best-of-4-PSO-seeds (min iter_mean -> iter_std -> residual), mirroring
 * extraction_optimal_lm_adaptive. positions.array[0]=l_init, [1]=v; double_data.array[0..2] =
 * iter_mean, iter_std, residual. The PSO output directory family switches from data_pso_lm_adaptive_*
 * (small orders) to data_pso_lm_adaptive_REDUCED_TIMESTEPS_* (large orders); we probe per pso_seed and
 * use whichever exists. Returns 1 if any value was found (out params updated), else 0 (out params
 * left at the caller's defaults). */

/* Load the best PSO-tuned adaptive-LM hyperparameters (l_init, v) for one error configuration,
 * selected as best-of-4-PSO-seeds (min iter_mean -> iter_std -> residual), mirroring
 * extraction_optimal_lm_adaptive. positions.array[0]=l_init, [1]=v; double_data.array[0..2] =
 * iter_mean, iter_std, residual. The PSO output directory family switches from data_pso_lm_adaptive_*
 * (small orders) to data_pso_lm_adaptive_REDUCED_TIMESTEPS_* (large orders); we probe per pso_seed and
 * use whichever exists. Returns 1 if any value was found (out params updated), else 0 (out params
 * left at the caller's defaults). */
static int load_best_lm_adaptive(char* directory, size_t order, int data_seed, double snr, double max_amp,
                                 double* out_l_init, double* out_v) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    double best_mean = INFINITY, best_std = INFINITY, best_res = INFINITY;
    int found = 0;
    for (size_t k = 0; k < 4; k++) {
        const char* token = "data_pso_lm_adaptive";
        char probe[700];
        snprintf(probe, sizeof(probe),
                 "%s/data/data_order_%zu/data_pso_lm_adaptive_order_%zu_seed_%d/seed_%d_order_%zu_snr_%.0f_maxamp_%.0f/positions.array",
                 directory, order, order, pso_seeds[k], data_seed, order, snr, max_amp);
        if (!file_exists_er(probe)) token = "data_pso_lm_adaptive_REDUCED_TIMESTEPS";

        char path_pos[700], path_dbl[700];
        snprintf(path_pos, sizeof(path_pos),
                 "%s/data/data_order_%zu/%s_order_%zu_seed_%d/seed_%d_order_%zu_snr_%.0f_maxamp_%.0f/positions.array",
                 directory, order, token, order, pso_seeds[k], data_seed, order, snr, max_amp);
        snprintf(path_dbl, sizeof(path_dbl),
                 "%s/data/data_order_%zu/%s_order_%zu_seed_%d/seed_%d_order_%zu_snr_%.0f_maxamp_%.0f/double_data.array",
                 directory, order, token, order, pso_seeds[k], data_seed, order, snr, max_amp);
        if (!file_exists_er(path_pos) || !file_exists_er(path_dbl)) continue;

        struct array* pos = read_from_file_a(path_pos);
        struct array* dbl = read_from_file_a(path_dbl);
        double m = *(double*)get_element_a(dbl, (size_t[]){0});
        double s = *(double*)get_element_a(dbl, (size_t[]){1});
        double r = *(double*)get_element_a(dbl, (size_t[]){2});
        double p_l_init = *(double*)get_element_a(pos, (size_t[]){0});
        double p_v = *(double*)get_element_a(pos, (size_t[]){1});

        if (m < best_mean ||
            (m == best_mean && (s < best_std ||
            (s == best_std && r < best_res)))) {
            best_mean = m; best_std = s; best_res = r;
            *out_l_init = p_l_init;
            *out_v = p_v;
            found = 1;
        }
        free_a(pos);
        free_a(dbl);
    }
    return found;
}

/**
 * Extended-research benchmark (supervisor request): a head-to-head of four FIXED-condition schemes on
 * every error configuration, with no PSO in the loop and no work-equivalent scaling. For each (order,
 * seed, snr, max_amp) it runs, up to 10 times in an interleaved loop (cycling the algos each run so
 * there is no ordering bias), the following and records the wall-clock solve time every run:
 *   0) SAS  l1 = l2 = 0.5, b0 = 2   (diagonal / constant-lambda)   -- timed 10x
 *   1) SAS  l1 = 0, l2 = 1, b0 = 2  (classic-StEFCal damping schedule) -- timed 10x
 *   2) plain StEFCal (no hyperparameters)                          -- timed 10x
 *   3) LM adaptive (solve_G_LM) at the PSO-tuned (l_init, v) for this exact configuration -- timed 3x
 *      (LM is deterministic and expensive; see algo_runs. Its t4..t10 columns are left blank.)
 * The SAS solves pass use_work_equiv = 0, so iteration counts are RAW (no scaling) and the
 * per-iteration b values are captured. The quality metrics (mean/std iterations over timesteps,
 * mean residual norm, mean batch size b) are deterministic for a fixed configuration -- the l1l2
 * solver seeds the kernel per timestep (seed = t + 1) and LM/StEFCal have no RNG -- so they are
 * identical across the 10 runs and are recorded only on run 0; only the timings vary run to run.
 *
 * Writes one CSV per configuration to {directory}/sas_stef_extended_research/ with the full
 * configuration in the filename AND in every row, so the later notebook can pd.concat a glob with
 * no filename parsing. 4 rows (one per algo) x 19 columns fits comfortably in one file
 * (mean_b is blank for stefcal and lm_adaptive).
 */
/**
 * Best PSO-tuned (lambda_even, lambda_odd) for StEFCal on one error configuration.
 *
 * Mirrors the selection in extraction_optimal_stefcal (src/main.c:3144): across the four PSO
 * seeds, take the run with the lowest mean iterations, breaking ties on the standard deviation
 * and then on the residual norm. Returns 0 and leaves the outputs untouched if no PSO data is
 * present for this configuration.
 */

/**
 * Best PSO-tuned (lambda_even, lambda_odd) for StEFCal on one error configuration.
 *
 * Mirrors the selection in extraction_optimal_stefcal (src/main.c:3144): across the four PSO
 * seeds, take the run with the lowest mean iterations, breaking ties on the standard deviation
 * and then on the residual norm. Returns 0 and leaves the outputs untouched if no PSO data is
 * present for this configuration.
 */
static int load_best_stefcal(char* directory, size_t order, int data_seed, double snr, double max_amp,
                             double* out_l1, double* out_l2) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    double best_mean = INFINITY, best_std = INFINITY, best_res = INFINITY;
    int found = 0;
    for (size_t k = 0; k < 4; k++) {
        char path_pos[700], path_dbl[700];
        snprintf(path_pos, sizeof(path_pos),
                 "%s/data/data_order_%zu/data_pso_stefcal_order_%zu_seed_%d/seed_%d_order_%zu_snr_%.0f_maxamp_%.0f/positions.array",
                 directory, order, order, pso_seeds[k], data_seed, order, snr, max_amp);
        snprintf(path_dbl, sizeof(path_dbl),
                 "%s/data/data_order_%zu/data_pso_stefcal_order_%zu_seed_%d/seed_%d_order_%zu_snr_%.0f_maxamp_%.0f/double_data.array",
                 directory, order, order, pso_seeds[k], data_seed, order, snr, max_amp);
        if (!file_exists_er(path_pos) || !file_exists_er(path_dbl)) continue;

        struct array* pos = read_from_file_a(path_pos);
        struct array* dbl = read_from_file_a(path_dbl);
        double m = *(double*)get_element_a(dbl, (size_t[]){0});
        double s = *(double*)get_element_a(dbl, (size_t[]){1});
        double r = *(double*)get_element_a(dbl, (size_t[]){2});
        double p_l1 = *(double*)get_element_a(pos, (size_t[]){0});
        double p_l2 = *(double*)get_element_a(pos, (size_t[]){1});

        if (m < best_mean ||
            (m == best_mean && (s < best_std ||
            (s == best_std && r < best_res)))) {
            best_mean = m; best_std = s; best_res = r;
            *out_l1 = p_l1; *out_l2 = p_l2;
            found = 1;
        }
        free_a(pos);
        free_a(dbl);
    }
    return found;
}

/**
 * Wall-clock benchmark of SAS (0.5,0.5) against DOStEFCal only.
 *
 * The four-algorithm study (extended_research_sas_stefcal, src/main.c:10493) times SAS against
 * *plain* StEFCal, so it cannot answer whether the subsampling advantage survives against the
 * PSO-tuned solver. This runs the same protocol over the same grid for the two solvers that
 * comparison needs, and unlike the four-algorithm study it is cheap enough to carry to order 10.
 *
 * DOStEFCal's (lambda_even, lambda_odd) are the per-configuration PSO optima, selected exactly as
 * extraction_optimal_stefcal selects them.
 *
 * Writes <directory>/sas_dostefcal_walltime/order_<OO>_seed_<S>_snr_<R>_maxamp_<A>.csv, one row
 * per solver, with the same columns as the four-algorithm study so the notebooks can read both.
 */
/**
 * Verify solve_G_StEFCal_Lambda_v2 reproduces solve_G_StEFCal_Lambda exactly.
 *
 * v2 is only useful for timing if it is numerically identical to the original -- otherwise a
 * wall-clock comparison against SAS is comparing two different solvers. Runs both over a spread of
 * array sizes and SNRs and reports the largest disagreement in mean iterations, iteration standard
 * deviation and mean residual norm. Any non-zero difference is a bug, not a tolerance.
 */

/**
 * Verify solve_G_StEFCal_Lambda_v2 reproduces solve_G_StEFCal_Lambda exactly.
 *
 * v2 is only useful for timing if it is numerically identical to the original -- otherwise a
 * wall-clock comparison against SAS is comparing two different solvers. Runs both over a spread of
 * array sizes and SNRs and reports the largest disagreement in mean iterations, iteration standard
 * deviation and mean residual norm. Any non-zero difference is a bug, not a tolerance.
 */
void compare_stefcal_v2(char* directory) {
    int seeds[] = {12345678, 23476934};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 10};
    size_t orders[] = {2, 5, 8, 10};
    int num_workers = workers(28);

    double worst_it = 0.0, worst_sd = 0.0, worst_rn = 0.0;
    size_t compared = 0, mismatched = 0;

    printf("%5s %8s %6s %7s | %14s %14s | %12s %12s | %s\n",
           "order", "seed", "snr", "maxamp", "iters v1", "iters v2", "resid v1", "resid v2", "match");
    for (size_t oi = 0; oi < 4; oi++) {
        size_t order = orders[oi];
        for (size_t h = 0; h < 2; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 2; j++) {
                    char sim_path[512];
                    snprintf(sim_path, sizeof(sim_path),
                             "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig",
                             directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(sim_path);
                    if (simulator == NULL) { fprintf(stderr, "missing %s\n", sim_path); continue; }

                    double l1 = 1.0, l2 = 0.0;
                    load_best_stefcal(directory, order, seeds[h], snrs[i], max_amps[j], &l1, &l2);

                    double it[2], sd[2], rn[2];
                    for (int v = 0; v < 2; v++) {
                        struct Calibrator* calibrator = new_c(simulator);
                        set_parallel_c(calibrator, &num_workers);
                        if (v == 0) solve_G_StEFCal_Lambda(calibrator, &(size_t){IMAX}, &(double){TAU}, &l1, &l2);
                        else        solve_G_StEFCal_Lambda_v2(calibrator, &(size_t){IMAX}, &(double){TAU}, &l1, &l2);
                        double* a = get_average_iterations_dc(calibrator->datacollector);
                        double* b = get_standard_deviation_iterations_dc(calibrator->datacollector);
                        double* c = (double*)get_last_residuals_norm_dc(calibrator->datacollector);
                        it[v] = *a; sd[v] = *b; rn[v] = *c;
                        free(a); free(b); free(c);
                        free_c(calibrator);
                    }
                    free_s(simulator);

                    double d_it = fabs(it[0]-it[1]), d_sd = fabs(sd[0]-sd[1]), d_rn = fabs(rn[0]-rn[1]);
                    int ok = (d_it == 0.0 && d_sd == 0.0 && d_rn == 0.0);
                    if (d_it > worst_it) worst_it = d_it;
                    if (d_sd > worst_sd) worst_sd = d_sd;
                    if (d_rn > worst_rn) worst_rn = d_rn;
                    compared++;
                    if (!ok) mismatched++;

                    printf("%5ld %8d %6.0f %7.0f | %14.8f %14.8f | %12.4g %12.4g | %s\n",
                           order, seeds[h], snrs[i], max_amps[j], it[0], it[1], rn[0], rn[1],
                           ok ? "yes" : "NO");
                    fflush(stdout);
                }
            }
        }
    }
    printf("\ncompared %zu configurations, %zu mismatched\n", compared, mismatched);
    printf("worst |d mean_iterations| = %.6g\n", worst_it);
    printf("worst |d std_iterations|  = %.6g\n", worst_sd);
    printf("worst |d residual_norm|   = %.6g\n", worst_rn);
}

/**
 * As extended_research_sas_dostefcal, but timing THREE solvers instead of two:
 *
 *   sas_0.5_0.5   -- untuned SAS at the equal-damping default
 *   dostefcal     -- solve_G_StEFCal_Lambda      (struct-array inner loop)
 *   dostefcal_v2  -- solve_G_StEFCal_Lambda_v2   (flat-buffer inner loop, same numbers)
 *   stefcal       -- plain StEFCal at the (0,1) default, also on the flat-buffer implementation
 *
 * The two DOStEFCal rows are numerically identical (compare_stefcal_v2 verifies this), so the
 * difference between them is purely the cost of the array abstraction, and the difference between
 * SAS and dostefcal_v2 is the algorithmic O(Nb) saving with that overhead removed. Timing all three
 * in one pass keeps them on identical hardware conditions.
 *
 * Writes <directory>/sas_dostefcal_walltime_v2/ -- the two-solver tree is left untouched.
 */

/**
 * As extended_research_sas_dostefcal, but timing THREE solvers instead of two:
 *
 *   sas_0.5_0.5   -- untuned SAS at the equal-damping default
 *   dostefcal     -- solve_G_StEFCal_Lambda      (struct-array inner loop)
 *   dostefcal_v2  -- solve_G_StEFCal_Lambda_v2   (flat-buffer inner loop, same numbers)
 *   stefcal       -- plain StEFCal at the (0,1) default, also on the flat-buffer implementation
 *
 * The two DOStEFCal rows are numerically identical (compare_stefcal_v2 verifies this), so the
 * difference between them is purely the cost of the array abstraction, and the difference between
 * SAS and dostefcal_v2 is the algorithmic O(Nb) saving with that overhead removed. Timing all three
 * in one pass keeps them on identical hardware conditions.
 *
 * Writes <directory>/sas_dostefcal_walltime_v2/ -- the two-solver tree is left untouched.
 */
void extended_research_sas_dostefcal_v2(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    int num_workers = workers(28);
    const size_t N_RUNS = 10;
    const size_t N_ALGOS = 4;
    const char* algo_labels[4] = {"sas_0.5_0.5", "dostefcal", "dostefcal_v2", "stefcal"};
    int has_b[4] = {1, 0, 0, 0};   /* SAS captures b; the full-batch solvers do not */

    char out_dir[512];
    snprintf(out_dir, sizeof(out_dir), "%s/sas_dostefcal_walltime_v2", directory);
    create_directories(out_dir);

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    char simulator_path[512];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig",
                             directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);

                    /* DOStEFCal's tuned damping pair for this configuration. Fall back to the
                     * StEFCal default (1,0) and warn, rather than silently timing a different solver. */
                    double l1 = 1.0, l2 = 0.0;
                    if (!load_best_stefcal(directory, order, seeds[h], snrs[i], max_amps[j], &l1, &l2)) {
                        fprintf(stderr, "WARNING: no StEFCal PSO hyperparameters for order %zu seed %d snr %.0f maxamp %.0f -- using defaults (%.4f, %.4f)\n",
                                order, seeds[h], snrs[i], max_amps[j], l1, l2);
                    }

                    double times[4][10];
                    double mean_it[4] = {0}, std_it[4] = {0}, mean_res[4] = {0}, mean_b[4] = {0};

                    for (size_t run = 0; run < N_RUNS; run++) {
                        for (size_t a = 0; a < N_ALGOS; a++) {
                            struct Calibrator* calibrator = new_c(simulator);
                            set_parallel_c(calibrator, &num_workers);

                            struct Stopwatch* stopwatch = new_sw();
                            if (a == 0) {
                                solve_G_StEFCal_stochastic_adaptive_l1l2(calibrator, &(size_t){IMAX}, &(double){TAU}, &(double){0.5}, &(double){0.5}, &(size_t){2}, &(int){0});
                            } else if (a == 1) {
                                solve_G_StEFCal_Lambda(calibrator, &(size_t){IMAX}, &(double){TAU}, &l1, &l2);
                            } else if (a == 2) {
                                solve_G_StEFCal_Lambda_v2(calibrator, &(size_t){IMAX}, &(double){TAU}, &l1, &l2);
                            } else {
                                /* Plain StEFCal: the default schedule is a full step on the odd
                                 * iterations and an average on the even ones, i.e. (l1, l2) = (0, 1)
                                 * in this kernel's convention. Run on the v2 implementation so it is
                                 * timed on the same footing as everything else. */
                                solve_G_StEFCal_Lambda_v2(calibrator, &(size_t){IMAX}, &(double){TAU},
                                                          &(double){0.0}, &(double){1.0});
                            }
                            times[a][run] = elapsed_sw(stopwatch);
                            free_sw(stopwatch);

                            if (run == 0) {
                                double* mi = get_average_iterations_dc(calibrator->datacollector);
                                double* si = get_standard_deviation_iterations_dc(calibrator->datacollector);
                                double* rn = (double*)get_last_residuals_norm_dc(calibrator->datacollector);
                                mean_it[a] = *mi;
                                std_it[a] = *si;
                                mean_res[a] = *rn;
                                free(mi);
                                free(si);
                                free(rn);

                                if (has_b[a]) {
                                    struct JaggedArray* bv = get_b_values_dc(calibrator->datacollector);
                                    double sum = 0.0;
                                    size_t cnt = 0;
                                    if (bv != NULL && bv->size != NULL) {
                                        for (size_t r = 0; r < *bv->size; r++) {
                                            if (bv->jarray[r] != NULL) {
                                                int* arr = get_a(bv->jarray[r]);
                                                size_t len = get_size_a(bv->jarray[r]);
                                                for (size_t k = 0; k < len; k++) {
                                                    sum += arr[k];
                                                    cnt++;
                                                }
                                            }
                                        }
                                    }
                                    mean_b[a] = (cnt > 0) ? sum / (double)cnt : 0.0;
                                }
                            }
                            free_c(calibrator);
                        }
                    }

                    free_s(simulator);

                    char csv_path[1024];
                    snprintf(csv_path, sizeof(csv_path), "%s/order_%02ld_seed_%d_snr_%.0f_maxamp_%.0f.csv",
                             out_dir, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", csv_path);
                    FILE* csv_file = open_output(csv_path, "w");
                    if (csv_file == NULL) {
                        perror("Error opening CSV file");
                        exit(EXIT_FAILURE);
                    }
                    fprintf(csv_file, "order,seed,snr,maxamp,algorithm,l1,l2,mean_iterations,std_iterations,mean_residual_norm,mean_b,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10\n");
                    for (size_t a = 0; a < N_ALGOS; a++) {
                        fprintf(csv_file, "%ld,%d,%.0f,%.0f,%s,", order, seeds[h], snrs[i], max_amps[j], algo_labels[a]);
                        if (a == 1 || a == 2) {
                            fprintf(csv_file, "%.10f,%.10f,", l1, l2);      /* the tuned pair actually used */
                        } else if (a == 3) {
                            fprintf(csv_file, "0.0000000000,1.0000000000,");  /* plain StEFCal default */
                        } else {
                            fprintf(csv_file, "0.5000000000,0.5000000000,");
                        }
                        fprintf(csv_file, "%.10f,%.10f,%.10f,", mean_it[a], std_it[a], mean_res[a]);
                        if (has_b[a]) {
                            fprintf(csv_file, "%.10f", mean_b[a]);
                        }
                        for (size_t run = 0; run < N_RUNS; run++) {
                            fprintf(csv_file, ",%.10f", times[a][run]);
                        }
                        fprintf(csv_file, "\n");
                    }
                    fclose(csv_file);
                }
            }
        }
    }
}

void extended_research_sas_stefcal(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    int num_workers = workers(28);
    const size_t N_RUNS = 10;
    const size_t N_ALGOS = 4;
    const char* algo_labels[4] = {"sas_0.5_0.5", "sas_0_1", "stefcal", "lm_adaptive"};
    int has_b[4] = {1, 1, 0, 0};   /* the two SAS variants capture b; StEFCal and LM do not */
    /* Timing repeats per algo (<= N_RUNS). LM is deterministic and very slow (~2-11 min/solve at large
     * N), so it is timed fewer times than the cheap StEFCal/SAS; its quality metrics (iterations,
     * residual) are recorded once on run 0 regardless, so 3 timing samples lose nothing. */
    size_t algo_runs[4] = {10, 10, 10, 3};

    char out_dir[512];
    snprintf(out_dir, sizeof(out_dir), "%s/sas_stef_extended_research", directory);
    create_directories(out_dir);

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    /* Load the simulator ONCE per configuration; new_c below gives each solve its own
                     * fresh calibrator + datacollector, and the solvers never mutate the simulator. */
                    char simulator_path[512];
                    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%ld_snr_%.0lf_maxamp_%.0lf.sconfig",
                             directory, seeds[h], order, snrs[i], max_amps[j]);
                    struct Simulator* simulator = read_from_file_s(simulator_path);

                    /* Load the PSO-tuned adaptive-LM hyperparameters for this configuration once. */
                    double lm_l_init = 0.5, lm_v = 2.0;   /* fallback defaults if PSO data is missing */
                    if (!load_best_lm_adaptive(directory, order, seeds[h], snrs[i], max_amps[j], &lm_l_init, &lm_v)) {
                        fprintf(stderr, "WARNING: no LM PSO hyperparameters for order %zu seed %d snr %.0f maxamp %.0f -- using defaults (%.4f, %.4f)\n",
                                order, seeds[h], snrs[i], max_amps[j], lm_l_init, lm_v);
                    }

                    double times[4][10];
                    double mean_it[4] = {0}, std_it[4] = {0}, mean_res[4] = {0}, mean_b[4] = {0};

                    for (size_t run = 0; run < N_RUNS; run++) {
                        for (size_t a = 0; a < N_ALGOS; a++) {
                            if (run >= algo_runs[a]) continue;   /* fewer timing repeats for the slow LM */
                            struct Calibrator* calibrator = new_c(simulator);
                            set_parallel_c(calibrator, &num_workers);

                            struct Stopwatch* stopwatch = new_sw();
                            if (a == 0) {
                                solve_G_StEFCal_stochastic_adaptive_l1l2(calibrator, &(size_t){IMAX}, &(double){TAU}, &(double){0.5}, &(double){0.5}, &(size_t){2}, &(int){0});
                            } else if (a == 1) {
                                solve_G_StEFCal_stochastic_adaptive_l1l2(calibrator, &(size_t){IMAX}, &(double){TAU}, &(double){0.0}, &(double){1.0}, &(size_t){2}, &(int){0});
                            } else if (a == 2) {
                                solve_G_StEFCal(calibrator, &(size_t){IMAX}, &(double){TAU});
                            } else {
                                solve_G_LM(calibrator, &(size_t){IMAX}, &(double){TAU}, &lm_l_init, &lm_v);
                            }
                            times[a][run] = elapsed_sw(stopwatch);
                            free_sw(stopwatch);

                            if (run == 0) {
                                double* mi = get_average_iterations_dc(calibrator->datacollector);
                                double* si = get_standard_deviation_iterations_dc(calibrator->datacollector);
                                double* rn = (double*)get_last_residuals_norm_dc(calibrator->datacollector);
                                mean_it[a] = *mi;
                                std_it[a] = *si;
                                mean_res[a] = *rn;
                                free(mi);
                                free(si);
                                free(rn);

                                if (has_b[a]) {
                                    struct JaggedArray* bv = get_b_values_dc(calibrator->datacollector);
                                    double sum = 0.0;
                                    size_t cnt = 0;
                                    if (bv != NULL && bv->size != NULL) {
                                        for (size_t r = 0; r < *bv->size; r++) {
                                            if (bv->jarray[r] != NULL) {
                                                int* arr = get_a(bv->jarray[r]);
                                                size_t len = get_size_a(bv->jarray[r]);
                                                for (size_t k = 0; k < len; k++) {
                                                    sum += arr[k];
                                                    cnt++;
                                                }
                                            }
                                        }
                                    }
                                    mean_b[a] = (cnt > 0) ? sum / (double)cnt : 0.0;
                                }
                            }
                            free_c(calibrator);
                        }
                    }

                    free_s(simulator);

                    /* One CSV per error configuration; config encoded in the name and repeated in-row. */
                    char csv_path[1024];
                    snprintf(csv_path, sizeof(csv_path), "%s/order_%02ld_seed_%d_snr_%.0f_maxamp_%.0f.csv",
                             out_dir, order, seeds[h], snrs[i], max_amps[j]);
                    printf("%s\n", csv_path);
                    FILE* csv_file = open_output(csv_path, "w");
                    if (csv_file == NULL) {
                        perror("Error opening CSV file");
                        exit(EXIT_FAILURE);
                    }
                    fprintf(csv_file, "order,seed,snr,maxamp,algorithm,mean_iterations,std_iterations,mean_residual_norm,mean_b,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10\n");
                    for (size_t a = 0; a < N_ALGOS; a++) {
                        fprintf(csv_file, "%ld,%d,%.0f,%.0f,%s,%.10f,%.10f,%.10f,",
                                order, seeds[h], snrs[i], max_amps[j], algo_labels[a], mean_it[a], std_it[a], mean_res[a]);
                        if (has_b[a]) {
                            fprintf(csv_file, "%.10f", mean_b[a]);   /* blank field for plain StEFCal */
                        }
                        for (size_t run = 0; run < N_RUNS; run++) {
                            if (run < algo_runs[a]) {
                                fprintf(csv_file, ",%.10f", times[a][run]);
                            } else {
                                fprintf(csv_file, ",");   /* blank: this algo was timed fewer times */
                            }
                        }
                        fprintf(csv_file, "\n");
                    }
                    fclose(csv_file);
                }
            }
        }
    }
}

/* ----------------------------------------------------------------------------------------------
 * RAW (unscaled) SAS re-measurement.
 *
 * The SAS PSO searched with use_work_equiv = 1, so every iteration count it recorded -- and every
 * count in data_optimal_solutions/stochastic_fixed_b_{optimal,original}/ -- is the work-equivalent
 * value sum(b)/(N-1), not a sweep count. That scaling was the right search objective (it stops the
 * swarm buying fewer sweeps with a bigger b), but it is not a number the paper can report as
 * "iterations". The functions below re-run the solver at exactly the hyperparameters the PSO chose
 * with use_work_equiv = 0, which (a) reports the raw sweep count and (b) makes the solver capture
 * the full per-iteration b sequence into datacollector->b_values.
 *
 * The toggle changes only what jacobi_stoch_adaptive_timestep writes to *iters_out -- the gain
 * trajectory, convergence and residuals are identical to the scaled run. This is a re-measurement
 * of runs already validated, not a new experiment.
 *
 * Everything is written under a brand-new {directory}/sas_raw_iterations/ tree so no existing
 * data_optimal_solutions* output can be overwritten.
 * ---------------------------------------------------------------------------------------------- */

/* Best-of-4-PSO-seeds (l1, l2) for one error configuration within a single PSO family, by
 * iteration_mean -> iteration_std -> residual_norm. Same selection and tie-breaking as the inline
 * loop in extraction_optimal_stochastic, so the raw re-run uses exactly the hyperparameters that
 * extraction picked. positions.array[0]=l1, [1]=l2 (the l1l2 family also has [2]=b0, unused here --
 * b0 always comes from stochastic_best_b so its sourcing is uniform across orders);
 * double_data.array[0..2] = iter_mean, iter_std, residual. Returns 1 if any record was found. */

/* Best-of-4-PSO-seeds (l1, l2) for one error configuration within a single PSO family, by
 * iteration_mean -> iteration_std -> residual_norm. Same selection and tie-breaking as the inline
 * loop in extraction_optimal_stochastic, so the raw re-run uses exactly the hyperparameters that
 * extraction picked. positions.array[0]=l1, [1]=l2 (the l1l2 family also has [2]=b0, unused here --
 * b0 always comes from stochastic_best_b so its sourcing is uniform across orders);
 * double_data.array[0..2] = iter_mean, iter_std, residual. Returns 1 if any record was found. */
static int load_best_l1_l2_pso_family(char* directory, const char* family, size_t order,
                                      int data_seed, double snr, double max_amp,
                                      double* out_l1, double* out_l2) {
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};
    double best_mean = INFINITY, best_std = INFINITY, best_res = INFINITY;
    int found = 0;
    for (size_t k = 0; k < 4; k++) {
        char path_pos[700], path_dbl[700];
        snprintf(path_pos, sizeof(path_pos),
                 "%s/data/data_order_%zu/%s_order_%zu_seed_%d/seed_%d_order_%zu_snr_%.0f_maxamp_%.0f/positions.array",
                 directory, order, family, order, pso_seeds[k], data_seed, order, snr, max_amp);
        snprintf(path_dbl, sizeof(path_dbl),
                 "%s/data/data_order_%zu/%s_order_%zu_seed_%d/seed_%d_order_%zu_snr_%.0f_maxamp_%.0f/double_data.array",
                 directory, order, family, order, pso_seeds[k], data_seed, order, snr, max_amp);
        if (!file_exists_er(path_pos) || !file_exists_er(path_dbl)) continue;

        struct array* pos = read_from_file_a(path_pos);
        struct array* dbl = read_from_file_a(path_dbl);
        double m = *(double*)get_element_a(dbl, (size_t[]){0});
        double s = *(double*)get_element_a(dbl, (size_t[]){1});
        double r = *(double*)get_element_a(dbl, (size_t[]){2});
        double p_l1 = *(double*)get_element_a(pos, (size_t[]){0});
        double p_l2 = *(double*)get_element_a(pos, (size_t[]){1});

        if (m < best_mean ||
            (m == best_mean && (s < best_std ||
            (s == best_std && r < best_res)))) {
            best_mean = m; best_std = s; best_res = r;
            *out_l1 = p_l1;
            *out_l2 = p_l2;
            found = 1;
        }
        free_a(pos);
        free_a(dbl);
    }
    return found;
}

/* Best PSO-tuned (l1, l2), preferring the round-2a fixed-b PSO because that is the family
 * extraction_optimal_stochastic used -- so the raw numbers stay directly comparable to the scaled
 * ones already on disk. Order 2 has no data_pso_stochastic_fixed_b_* records (only the 3-parameter
 * data_pso_stochastic_l1l2_*), so fall back to that family rather than dropping the smallest antenna
 * size out of the FLOPs figure entirely. The whole best-of-4 selection is redone inside one family;
 * the two are never mixed, since their objective values come from different search spaces. Writes
 * the family actually used to *out_family for the CSV. Returns 1 if any record was found. */

/* Best PSO-tuned (l1, l2), preferring the round-2a fixed-b PSO because that is the family
 * extraction_optimal_stochastic used -- so the raw numbers stay directly comparable to the scaled
 * ones already on disk. Order 2 has no data_pso_stochastic_fixed_b_* records (only the 3-parameter
 * data_pso_stochastic_l1l2_*), so fall back to that family rather than dropping the smallest antenna
 * size out of the FLOPs figure entirely. The whole best-of-4 selection is redone inside one family;
 * the two are never mixed, since their objective values come from different search spaces. Writes
 * the family actually used to *out_family for the CSV. Returns 1 if any record was found. */
static int load_best_l1_l2_stochastic(char* directory, size_t order, int data_seed,
                                      double snr, double max_amp,
                                      double* out_l1, double* out_l2, const char** out_family) {
    if (load_best_l1_l2_pso_family(directory, "data_pso_stochastic_fixed_b", order, data_seed, snr, max_amp, out_l1, out_l2)) {
        *out_family = "stochastic_fixed_b";
        return 1;
    }
    if (load_best_l1_l2_pso_family(directory, "data_pso_stochastic_l1l2", order, data_seed, snr, max_amp, out_l1, out_l2)) {
        *out_family = "stochastic_l1l2";
        return 1;
    }
    return 0;
}

/* PSO-selected INITIAL sub-sample size b0 for one error configuration, as written by
 * extract_best_b_stochastic. Note this is b0 only -- b doubles on stall inside the solve, so the
 * effective batch size is measured from the b trace, never from this file. Returns 1 on success,
 * 0 if the record is missing (out param untouched). */

/* PSO-selected INITIAL sub-sample size b0 for one error configuration, as written by
 * extract_best_b_stochastic. Note this is b0 only -- b doubles on stall inside the solve, so the
 * effective batch size is measured from the b trace, never from this file. Returns 1 on success,
 * 0 if the record is missing (out param untouched). */
static int load_best_b0_stochastic(char* directory, size_t order, int data_seed,
                                   double snr, double max_amp, size_t* out_b0) {
    char b_path[700];
    snprintf(b_path, sizeof(b_path),
             "%s/data_optimal_solutions/stochastic_best_b/data_order_%zu/seed_%d_snr_%.0f_maxamp_%.0f.array",
             directory, order, data_seed, snr, max_amp);
    if (!file_exists_er(b_path)) return 0;
    struct array* b_array = read_from_file_a(b_path);
    *out_b0 = (size_t)round(*(double*)get_element_a(b_array, (size_t[]){0}));
    free_a(b_array);
    return 1;
}

/* Solve one error configuration with raw-iteration reporting and dump everything.
 *
 * Saves iterations.array (raw sweeps), b_values.jarray (per-iteration b, one row per timestep) and
 * last_residuals.jarray per experiment, appends one aggregate row to csv_summary and one row per
 * timestep to csv_trace. all_gains/damping_g/damping_g_old are deliberately NOT saved -- they are
 * irrelevant to the iteration/b/FLOPS question and are the bulk of the disk footprint.
 *
 * The FLOPS-critical quantity is sum_b per solve, not a mean b: b doubles mid-solve, so the exact
 * cost is 18*N*sum(b_it) + (17N+1)*iters_raw. */

/* Solve one error configuration with raw-iteration reporting and dump everything.
 *
 * Saves iterations.array (raw sweeps), b_values.jarray (per-iteration b, one row per timestep) and
 * last_residuals.jarray per experiment, appends one aggregate row to csv_summary and one row per
 * timestep to csv_trace. all_gains/damping_g/damping_g_old are deliberately NOT saved -- they are
 * irrelevant to the iteration/b/FLOPS question and are the bulk of the disk footprint.
 *
 * The FLOPS-critical quantity is sum_b per solve, not a mean b: b doubles mid-solve, so the exact
 * cost is 18*N*sum(b_it) + (17N+1)*iters_raw. */
static void raw_sas_solve_and_save(char* directory, const char* variant, const char* pso_family,
                                   size_t order, int data_seed, double snr, double max_amp,
                                   double l1, double l2, size_t b0,
                                   FILE* csv_summary, FILE* csv_trace) {
    int num_workers = workers(28);
    size_t N = order * order;

    char simulator_path[700];
    snprintf(simulator_path, sizeof(simulator_path), "%s/simulators/sim_seed_%u_order_%zu_snr_%.0lf_maxamp_%.0lf.sconfig",
             directory, (unsigned)data_seed, order, snr, max_amp);
    struct Simulator* simulator = read_from_file_s(simulator_path);
    struct Calibrator* calibrator = new_c(simulator);
    set_parallel_c(calibrator, &num_workers);

    /* use_work_equiv = 0 is the whole point: raw sweep count, plus cap_b allocation so the
     * per-iteration b sequence lands in datacollector->b_values. */
    solve_G_StEFCal_stochastic_adaptive_l1l2(calibrator, &(size_t){IMAX}, &(double){TAU},
                                             &l1, &l2, &b0, &(int){0});

    struct array* iterations = get_iterations_dc(calibrator->datacollector);
    struct JaggedArray* b_values = get_b_values_dc(calibrator->datacollector);
    struct JaggedArray* last_residuals = get_last_residual_dc(calibrator->datacollector);

    char directory_save[900];
    snprintf(directory_save, sizeof(directory_save),
             "%s/sas_raw_iterations/stochastic_fixed_b_%s/data_order_%zu/data_stochastic_fixed_b_order_%zu_seed_%d_snr_%.0f_maxamp_%.0f",
             directory, variant, order, order, data_seed, snr, max_amp);
    printf("%s\n", directory_save);
    create_directories(directory_save);

    save_to_file_a(iterations, directory_save, "iterations");
    save_to_file_ja(b_values, directory_save, "b_values");
    save_to_file_ja(last_residuals, directory_save, "last_residuals");

    size_t n_steps = get_size_a(iterations);
    double* sum_b_per_step = malloc(n_steps * sizeof(double));
    double sum_res = 0;
    size_t total_b = 0, total_iters = 0;

    for (size_t t = 0; t < n_steps; t++) {
        int iters_t = *(int*)get_element_a(iterations, (size_t[]){t});
        size_t sum_b_t = 0;
        size_t len = 0;
        int* b_row = NULL;
        if (b_values != NULL && b_values->jarray[t] != NULL) {
            b_row = (int*)get_a(b_values->jarray[t]);
            len = get_size_a(b_values->jarray[t]);
            for (size_t q = 0; q < len; q++) sum_b_t += (size_t)b_row[q];
        }
        /* Invariant: one captured b per raw sweep. A mismatch means the trace is incomplete and
         * every FLOP number derived from it would be wrong, so shout rather than average silently. */
        if ((size_t)iters_t != len) {
            fprintf(stderr, "WARNING: b trace length %zu != raw iterations %d (%s order %zu seed %d snr %.0f maxamp %.0f timestep %zu)\n",
                    len, iters_t, variant, order, data_seed, snr, max_amp, t);
        }

        sum_b_per_step[t] = (double)sum_b_t;
        total_b += sum_b_t;
        total_iters += (size_t)iters_t;

        double* residual_norm = norm_a(last_residuals->jarray[t]);
        sum_res += *residual_norm;
        free(residual_norm);

        fprintf(csv_trace, "%zu,%d,%.0f,%.0f,%zu,%d,%zu,", order, data_seed, snr, max_amp, t, iters_t, sum_b_t);
        for (size_t q = 0; q < len; q++) {
            if (q) fputc(';', csv_trace);
            fprintf(csv_trace, "%d", b_row[q]);
        }
        fprintf(csv_trace, "\n");
    }

    /* Iteration stats via the same helpers every other pipeline uses, so they are directly
     * comparable to the existing (scaled) iterations_mean.txt numbers. */
    double* mean_it = get_average_iterations_dc(calibrator->datacollector);
    double* std_it = get_standard_deviation_iterations_dc(calibrator->datacollector);

    double mean_sb = 0;
    for (size_t t = 0; t < n_steps; t++) mean_sb += sum_b_per_step[t];
    mean_sb /= (double)n_steps;
    double var_sb = 0;
    for (size_t t = 0; t < n_steps; t++) var_sb += (sum_b_per_step[t] - mean_sb) * (sum_b_per_step[t] - mean_sb);
    double std_sb = sqrt(var_sb / (double)n_steps);   /* population std, matching std_a */

    double mean_b_per_iteration = (total_iters > 0) ? (double)total_b / (double)total_iters : 0.0;
    double mean_res = sum_res / (double)n_steps;

    fprintf(csv_summary, "%zu,%zu,%d,%.0f,%.0f,%s,%s,%.10f,%.10f,%zu,%zu,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f\n",
            order, N, data_seed, snr, max_amp, variant, pso_family, l1, l2, b0, n_steps,
            *mean_it, *std_it, mean_sb, std_sb, mean_b_per_iteration, mean_res);
    fflush(csv_summary);
    fflush(csv_trace);

    free(mean_it);
    free(std_it);
    free(sum_b_per_step);
    free_c(calibrator);
    free_s(simulator);
}

/**
 * Raw-iteration SAS extraction: re-runs the stochastic-adaptive solver over the full 64-experiment
 * grid per order with use_work_equiv = 0, for both
 *   "optimal"  -- (l1, l2) from the fixed-b PSO, b0 from stochastic_best_b   (the TOSAS series), and
 *   "original" -- the untuned defaults l1 = l2 = 0.5, b0 = 2                 (the SAS series),
 * so the FLOPs figure compares raw against raw rather than mixing scales.
 *
 * Writes to {directory}/sas_raw_iterations/:
 *   stochastic_fixed_b_{optimal,original}/data_order_{O}/data_..._seed_..._snr_..._maxamp_.../
 *       iterations.array, b_values.jarray, last_residuals.jarray
 *   summary/{variant}_order_{OO}.csv    64 rows: hyperparameters + mean/std raw iterations,
 *                                       mean/std sum_b, mean b per iteration, mean residual norm
 *   b_traces/{variant}_order_{OO}.csv   one row per timestep with the full ';'-joined b sequence
 *
 * CSVs are per-order and opened "w", so re-running a single order rewrites only that order and
 * never appends duplicates.
 */

/**
 * Raw-iteration SAS extraction: re-runs the stochastic-adaptive solver over the full 64-experiment
 * grid per order with use_work_equiv = 0, for both
 *   "optimal"  -- (l1, l2) from the fixed-b PSO, b0 from stochastic_best_b   (the TOSAS series), and
 *   "original" -- the untuned defaults l1 = l2 = 0.5, b0 = 2                 (the SAS series),
 * so the FLOPs figure compares raw against raw rather than mixing scales.
 *
 * Writes to {directory}/sas_raw_iterations/:
 *   stochastic_fixed_b_{optimal,original}/data_order_{O}/data_..._seed_..._snr_..._maxamp_.../
 *       iterations.array, b_values.jarray, last_residuals.jarray
 *   summary/{variant}_order_{OO}.csv    64 rows: hyperparameters + mean/std raw iterations,
 *                                       mean/std sum_b, mean b per iteration, mean residual norm
 *   b_traces/{variant}_order_{OO}.csv   one row per timestep with the full ';'-joined b sequence
 *
 * CSVs are per-order and opened "w", so re-running a single order rewrites only that order and
 * never appends duplicates.
 */
void extraction_raw_iterations_sas(char* directory, size_t* order_min, size_t* order_max) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};
    const char* variants[2] = {"optimal", "original"};

    char summary_dir[700], trace_dir[700];
    snprintf(summary_dir, sizeof(summary_dir), "%s/sas_raw_iterations/summary", directory);
    snprintf(trace_dir, sizeof(trace_dir), "%s/sas_raw_iterations/b_traces", directory);
    create_directories(summary_dir);
    create_directories(trace_dir);

    for (size_t order = *order_min; order <= *order_max; order++) {
        for (size_t v = 0; v < 2; v++) {
            char summary_path[900], trace_path[900];
            snprintf(summary_path, sizeof(summary_path), "%s/%s_order_%02zu.csv", summary_dir, variants[v], order);
            snprintf(trace_path, sizeof(trace_path), "%s/%s_order_%02zu.csv", trace_dir, variants[v], order);

            FILE* csv_summary = open_output(summary_path, "w");
            if (csv_summary == NULL) { perror("Error opening summary CSV"); exit(EXIT_FAILURE); }
            FILE* csv_trace = open_output(trace_path, "w");
            if (csv_trace == NULL) { perror("Error opening b-trace CSV"); exit(EXIT_FAILURE); }

            fprintf(csv_summary, "order,N,seed,snr,maxamp,variant,pso_family,l1,l2,b0,n_timesteps,mean_iterations_raw,std_iterations_raw,mean_sum_b,std_sum_b,mean_b_per_iteration,mean_last_residual_norm\n");
            fprintf(csv_trace, "order,seed,snr,maxamp,timestep,iterations_raw,sum_b,b_trace\n");

            for (size_t h = 0; h < 4; h++) {
                for (size_t i = 0; i < 4; i++) {
                    for (size_t j = 0; j < 4; j++) {
                        double l1 = 0.5, l2 = 0.5;
                        size_t b0 = 2;
                        const char* pso_family = "none";   /* the untuned baseline uses no PSO record */

                        if (v == 0) {
                            if (!load_best_l1_l2_stochastic(directory, order, seeds[h], snrs[i], max_amps[j], &l1, &l2, &pso_family)) {
                                fprintf(stderr, "WARNING: no SAS PSO record in either family for order %zu seed %d snr %.0f maxamp %.0f -- skipping\n",
                                        order, seeds[h], snrs[i], max_amps[j]);
                                continue;
                            }
                            if (!load_best_b0_stochastic(directory, order, seeds[h], snrs[i], max_amps[j], &b0)) {
                                fprintf(stderr, "WARNING: no stochastic_best_b record for order %zu seed %d snr %.0f maxamp %.0f -- skipping\n",
                                        order, seeds[h], snrs[i], max_amps[j]);
                                continue;
                            }
                        }

                        raw_sas_solve_and_save(directory, variants[v], pso_family, order, seeds[h], snrs[i], max_amps[j],
                                               l1, l2, b0, csv_summary, csv_trace);
                    }
                }
            }

            fclose(csv_summary);
            fclose(csv_trace);
            printf("%s\n%s\n", summary_path, trace_path);
        }
    }
}

/* Mirror of create_x_y_points_stochastic_variant for the diagonal stochastic-adaptive study:
 * aggregates the per-experiment iterations.array + last_residuals.jarray under
 * data_optimal_solutions/stochastic_adaptive_<variant>/ into the four per-order stat .txt files the
 * table script reads. Computation is identical to the fixed-b variant (only paths differ). */

/* Mirror of create_x_y_points_stochastic_variant for the diagonal stochastic-adaptive study:
 * aggregates the per-experiment iterations.array + last_residuals.jarray under
 * data_optimal_solutions/stochastic_adaptive_<variant>/ into the four per-order stat .txt files the
 * table script reads. Computation is identical to the fixed-b variant (only paths differ). */
static void create_x_y_points_stochastic_adaptive_variant(char* directory, size_t* order_min, size_t* order_max, const char* variant) {
    int seeds[] = {12345678, 23476934, 98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2, 3, 5, 10};

    for (size_t order = *order_min; order <= *order_max; order++) {

        double sum_iterations = 0;
        double sum_residual_norm = 0;
        size_t count_iterations = 0;
        size_t count_residual_norm = 0;

        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/stochastic_adaptive_%s/data_order_%ld/data_stochastic_adaptive_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f",
                            directory, variant, order, order, seeds[h], snrs[i], max_amps[j]);

                    char directory_iterations_array[600];
                    sprintf(directory_iterations_array, "%s/iterations.array", directory_save);
                    char directory_last_residuals_jarray[600];
                    sprintf(directory_last_residuals_jarray, "%s/last_residuals.jarray", directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    struct JaggedArray* last_residuals_jarray = read_from_file_ja(directory_last_residuals_jarray);

                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_iterations += *element;
                        count_iterations += 1;
                    }

                    for (size_t y = 0; y < 600; y++) {
                        struct array* residual_array = get_ja(last_residuals_jarray, (size_t[]){y});
                        double* residual_norm = norm_a(residual_array);
                        sum_residual_norm += *residual_norm;
                        free(residual_norm);
                        count_residual_norm += 1;
                    }

                    free_a(iterations_array);
                    free_ja(last_residuals_jarray);
                }
            }
        }

        double mean_iterations = sum_iterations / count_iterations;
        double mean_last_residual_norm = sum_residual_norm / count_residual_norm;

        double sum_of_squared_deviation = 0;
        double sum_of_absolute_deviation = 0;

        for (size_t h = 0; h < 4; h++) {
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    char directory_save[512];
                    sprintf(directory_save, "%s/data_optimal_solutions/stochastic_adaptive_%s/data_order_%ld/data_stochastic_adaptive_order_%ld_seed_%d_snr_%.0f_maxamp_%.0f",
                            directory, variant, order, order, seeds[h], snrs[i], max_amps[j]);

                    char directory_iterations_array[600];
                    sprintf(directory_iterations_array, "%s/iterations.array", directory_save);

                    struct array* iterations_array = read_from_file_a(directory_iterations_array);
                    size_t size_iterations_array = get_size_a(iterations_array);

                    for (size_t q = 0; q < size_iterations_array; q++) {
                        int* element = get_element_a(iterations_array, (size_t[]){q});
                        sum_of_squared_deviation += pow((*element - mean_iterations), 2.0);
                        sum_of_absolute_deviation += fabs(*element - mean_iterations);
                    }

                    free_a(iterations_array);
                }
            }
        }

        double std_iterations = sqrt(sum_of_squared_deviation / count_iterations);
        double mad_iterations = sum_of_absolute_deviation / count_iterations;

        printf("Stochastic Adaptive StEFCal (diagonal) [%s] Order = %ld\n", variant, order);
        printf("  Mean = %f\n", mean_iterations);
        printf("  Std  = %f\n", std_iterations);
        printf("  MAD  = %f\n", mad_iterations);
        printf("  Mean Last Residual Norm = %f\n", mean_last_residual_norm);
        printf("\n");

        char directory_out[400];
        sprintf(directory_out, "%s/data_optimal_solutions_extracted_data_points/stochastic_adaptive_%s/data_order_%ld", directory, variant, order);
        create_directories(directory_out);

        char path_mean[600];
        sprintf(path_mean, "%s/iterations_mean.txt", directory_out);
        char path_std[600];
        sprintf(path_std, "%s/iterations_std.txt", directory_out);
        char path_mad[600];
        sprintf(path_mad, "%s/iterations_mad.txt", directory_out);
        char path_res[600];
        sprintf(path_res, "%s/mean_last_residual_norm.txt", directory_out);

        FILE *fp1 = open_output(path_mean, "w");
        if (fp1 == NULL) { printf("Error opening %s\n", path_mean); return; }
        fprintf(fp1, "%f\n", mean_iterations);
        fclose(fp1);

        FILE *fp2 = open_output(path_std, "w");
        if (fp2 == NULL) { printf("Error opening %s\n", path_std); return; }
        fprintf(fp2, "%f\n", std_iterations);
        fclose(fp2);

        FILE *fp3 = open_output(path_mad, "w");
        if (fp3 == NULL) { printf("Error opening %s\n", path_mad); return; }
        fprintf(fp3, "%f\n", mad_iterations);
        fclose(fp3);

        FILE *fp4 = open_output(path_res, "w");
        if (fp4 == NULL) { printf("Error opening %s\n", path_res); return; }
        fprintf(fp4, "%f\n", mean_last_residual_norm);
        fclose(fp4);
    }
}

void create_x_y_points_from_optimal_stochastic_adaptive(char* directory, size_t* order_min, size_t* order_max) {
    create_x_y_points_stochastic_adaptive_variant(directory, order_min, order_max, "optimal");
}

void create_x_y_points_from_original_stochastic_adaptive(char* directory, size_t* order_min, size_t* order_max) {
    create_x_y_points_stochastic_adaptive_variant(directory, order_min, order_max, "original");
}

/* ===== (l1,l2) relaxation-justification study — ADAM-style 3->fix b->2 pipeline ===== */

/* Round 1: 3-param PSO over (l1, l2, b0) of the adaptive l1/l2 stochastic solver. On the
 * work-equivalent metric this lands on l1≈l2 + small b0 → gives the best init b0 to fix. */


/**
 * COMMAND LINE
 */

/** Shape of the pipeline function a command dispatches to. */
enum ArgKind {
    K_SZ,        /* (directory, size_t* order_min, size_t* order_max)              */
    K_SZ_REV,    /* (size_t* order_min, size_t* order_max, directory)              */
    K_INT,       /* (directory, int* order_min, int* order_max)                    */
    K_NAMED,     /* (directory, filename, int* order_min, int* order_max)          */
    K_DIR,       /* (directory)                                                    */
    K_SIM,       /* (int* order, directory)                                        */
    K_GRID,      /* (int* order, directory, filename, enum AlgorithmType*)         */
    K_PSO,       /* (directory, filename, algorithm, iteration, order_min, order_max) */
    K_ORDER,     /* (directory, filename, int* order)                              */
    K_DICT       /* (directory, filename)                                          */
};

typedef void (*fnptr)(void);

struct Command {
    const char* name;
    const char* group;
    enum ArgKind kind;
    fnptr fn;
    const char* help;
};

/**
 * Maps the -a name onto the enum the solvers dispatch on and onto the file-name
 * stem that the data dictionaries, data stores and extracted data all use.
 */
struct Algorithm {
    const char* name;
    enum AlgorithmType type;
    const char* stem;
};

static const struct Algorithm ALGORITHMS[] = {
    {"stefcal",     STEFCAL_LAMBDA,                                      "stefcal"},
    {"nos",         STEFCAL_IMPROVED,                                    "stefcal_improved"},
    {"lm-rv",       LEVENBERG_MARQUARDT,                                 "lm_adaptive"},
    {"lm-eo",       LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2,              "lm_non_adaptive_l1_l2"},
    {"lm-rv-rt",    LEVENBERG_MARQUARDT_REDUCED_TIMESTEPS,               "lm_adaptive_REDUCED_TIMESTEPS"},
    {"lm-eo-rt",    LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2_REDUCED_TIMESTEPS,
                                                                         "lm_non_adaptive_l1_l2_REDUCED_TIMESTEPS"},
    {"adam",        ADAM,                                                "adam"},
    {"adam-fixed",  ADAM_FIXED_ALPHA,                                    "adam_fixed_alpha"},
    {"sas",         STEFCAL_STOCHASTIC_FIXED_B,                          "stochastic_fixed_b"},
    {"sas-l1l2",    STEFCAL_STOCHASTIC_L1L2,                             "stochastic_l1l2"},
    {"sas-adaptive", STEFCAL_STOCHASTIC_ADAPTIVE,                        "stochastic_adaptive"}
};

static const struct Command COMMANDS[] = {
    {"simulate",            "simulation", K_SIM,      (fnptr)create_simulators,
     "generate the visibility datasets for one antenna order"},

    {"dict-grid",           "dictionaries", K_NAMED,  (fnptr)create_data_dictionary_orders,
     "create the grid-search data dictionaries (-a)"},
    {"dict-pso",            "dictionaries", K_NAMED,  (fnptr)create_data_dictionary_pso_orders,
     "create the PSO data dictionaries (-a)"},
    {"dict-extracted",      "dictionaries", K_DICT,   (fnptr)create_data_dictionary_extracted_data,
     "create the dictionary that holds the extracted data (-a)"},

    {"grid",                "grid search", K_GRID,    (fnptr)mapping_local,
     "grid search over the hyperparameter plane (-a)"},
    {"grid-adam",           "grid search", K_INT,     (fnptr)mapping_adam_fixed_alpha_local,
     "ADAM grid search at the alpha fixed by best-alpha-adam"},
    {"grid-sas",            "grid search", K_INT,     (fnptr)mapping_stochastic_fixed_b_local,
     "SAS grid search at the b0 fixed by best-b-sas"},
    {"grid-sas-adaptive",   "grid search", K_INT,     (fnptr)mapping_stochastic_adaptive_local,
     "SAS grid search over the single-lambda plane"},

    {"pso",                 "pso", K_PSO,             (fnptr)mapping_pso_local,
     "particle-swarm search for the optimal hyperparameters (-a, -s)"},
    {"pso-lm-rv",           "pso", K_INT,             (fnptr)mapping_pso_lm_adaptive_local,
     "PSO over the LM (rho, nu) plane"},
    {"pso-adam",            "pso", K_INT,             (fnptr)mapping_pso_adam_fixed_alpha_local,
     "PSO over (b1, b2) at the alpha fixed by best-alpha-adam"},
    {"pso-sas",             "pso", K_INT,             (fnptr)mapping_pso_stochastic_fixed_b_local,
     "PSO over the SAS lambda plane at the b0 fixed by best-b-sas"},
    {"pso-sas-l1l2",        "pso", K_INT,             (fnptr)mapping_pso_stochastic_l1l2_local,
     "PSO over the full SAS (lambda_even, lambda_odd, b0) box"},
    {"pso-sas-adaptive",    "pso", K_INT,             (fnptr)mapping_pso_stochastic_adaptive_local,
     "PSO over the SAS single-lambda plane"},

    {"best-alpha-adam",     "round 2", K_SZ,          (fnptr)extract_best_alpha_adam,
     "pick the alpha ADAM's second round is run at"},
    {"best-alpha-adam-all", "round 2", K_SZ,          (fnptr)extract_all_best_b1_b2_alpha_adam,
     "write the best (b1, b2, alpha) triples ADAM reached"},
    {"best-alpha-adam-3d",  "round 2", K_SZ,          (fnptr)extract_all_best_b1_b2_alpha_adam_3d,
     "same, for the three-dimensional ADAM search"},
    {"best-b-sas",          "round 2", K_SZ,          (fnptr)extract_best_b_stochastic,
     "pick the b0 SAS's second round is run at"},
    {"best-l1l2-b-sas",     "round 2", K_SZ,          (fnptr)extract_all_best_l1_l2_b_stochastic,
     "write the best (lambda_even, lambda_odd, b0) triples SAS reached"},

    {"heatmap",             "heatmaps", K_ORDER,      (fnptr)extract_heatmap_data,
     "write one order's heatmap data (-a)"},
    {"heatmap-orders",      "heatmaps", K_NAMED,      (fnptr)extract_heatmap_data_orders,
     "write the heatmap data over a range of orders (-a)"},

    {"pso-traces",          "pso output", K_NAMED,    (fnptr)pso_extract_trace_specific_all,
     "write every particle trace the PSO produced (-a)"},
    {"pso-best",            "pso output", K_NAMED,    (fnptr)pso_extract_all_best_hyperparameters,
     "write the best hyperparameters the PSO found (-a)"},
    {"pso-best-jarray",     "pso output", K_NAMED,    (fnptr)pso_extract_all_best_hyperparameters_save_to_jarray,
     "same, saved as a jagged array (-a)"},

    {"perf-stefcal-opt",    "performance", K_SZ,      (fnptr)extraction_optimal_stefcal,
     "DOStEFCal iterations, standard deviation and residual norm"},
    {"perf-stefcal-def",    "performance", K_SZ,      (fnptr)extraction_0_1_stefcal,
     "StEFCal at its default (1,0) / (0,1) hyperparameters"},
    {"perf-nos-opt",        "performance", K_SZ,      (fnptr)extraction_optimal_stefcal_improved,
     "DONOS iterations, standard deviation and residual norm"},
    {"perf-nos-def",        "performance", K_SZ,      (fnptr)extraction_original_stefcal_improved,
     "NOS at its default hyperparameters"},
    {"perf-lm-eo-opt",      "performance", K_SZ,      (fnptr)extraction_optimal_lm_non_adaptive_l1_l2,
     "DOLM (lambda_even, lambda_odd)"},
    {"perf-lm-eo-def",      "performance", K_SZ,      (fnptr)extraction_original_lm_non_adaptive_l1_l2,
     "LM (lambda_even, lambda_odd) at its default hyperparameters"},
    {"perf-lm-eo-opt-rt",   "performance", K_SZ,      (fnptr)extraction_optimal_lm_non_adaptive_l1_l2_REDUCED_TIMESTEPS,
     "DOLM (lambda_even, lambda_odd), reduced time steps"},
    {"perf-lm-rv-opt",      "performance", K_SZ_REV,  (fnptr)extraction_optimal_lm_adaptive,
     "DOLM (rho, nu)"},
    {"perf-lm-rv-opt-rt",   "performance", K_SZ_REV,  (fnptr)extraction_optimal_lm_adaptive_REDUCED_TIMESTEPS,
     "DOLM (rho, nu), reduced time steps"},
    {"perf-lm-rv-def",      "performance", K_SZ,      (fnptr)extraction_original_lm_adaptive,
     "LM (rho, nu) at its default hyperparameters"},
    {"perf-adam-opt",       "performance", K_SZ,      (fnptr)extraction_optimal_adam_fixed_alpha,
     "TOADAM at the fixed alpha"},
    {"perf-adam-def",       "performance", K_SZ,      (fnptr)extraction_original_adam_fixed_alpha,
     "ADAM at its default hyperparameters"},
    {"perf-adam-3d",        "performance", K_SZ,      (fnptr)extraction_optimal_adam_3d,
     "TOADAM from the three-dimensional search"},
    {"perf-sas-opt",        "performance", K_SZ,      (fnptr)extraction_optimal_stochastic,
     "TOSAS at the fixed b0"},
    {"perf-sas-def",        "performance", K_SZ,      (fnptr)extraction_original_stochastic,
     "SAS at its default hyperparameters"},
    {"perf-sas-adaptive-opt", "performance", K_SZ,    (fnptr)extraction_optimal_stochastic_adaptive,
     "SAS single-lambda, optimized"},
    {"perf-sas-adaptive-def", "performance", K_SZ,    (fnptr)extraction_original_stochastic_adaptive,
     "SAS single-lambda, default hyperparameters"},

    {"points-stefcal-opt",  "points", K_SZ,           (fnptr)create_x_y_points_from_optimal_stefcal,
     "x/y points and ellipses for DOStEFCal"},
    {"points-stefcal-def",  "points", K_SZ,           (fnptr)create_x_y_points_from_original_stefcal,
     "x/y points and ellipses for StEFCal"},
    {"points-nos-opt",      "points", K_SZ,           (fnptr)create_x_y_points_from_optimal_stefcal_improved,
     "x/y points and ellipses for DONOS"},
    {"points-nos-def",      "points", K_SZ,           (fnptr)create_x_y_points_from_original_stefcal_improved,
     "x/y points and ellipses for NOS"},
    {"points-lm-eo-opt",    "points", K_SZ,           (fnptr)create_x_y_points_from_optimal_lm_non_adaptive_l1_l2,
     "x/y points and ellipses for DOLM (lambda_even, lambda_odd)"},
    {"points-lm-eo-def",    "points", K_SZ,           (fnptr)create_x_y_points_from_original_lm_non_adaptive_l1_l2,
     "x/y points and ellipses for LM (lambda_even, lambda_odd)"},
    {"points-lm-rv-opt",    "points", K_SZ,           (fnptr)create_x_y_points_from_optimal_lm_adaptive,
     "x/y points and ellipses for DOLM (rho, nu)"},
    {"points-lm-rv-def",    "points", K_SZ,           (fnptr)create_x_y_points_from_original_lm_adaptive,
     "x/y points and ellipses for LM (rho, nu)"},
    {"points-adam-opt",     "points", K_SZ,           (fnptr)create_x_y_points_from_optimal_adam_fixed_alpha,
     "x/y points and ellipses for TOADAM"},
    {"points-adam-def",     "points", K_SZ,           (fnptr)create_x_y_points_from_original_adam_fixed_alpha,
     "x/y points and ellipses for ADAM"},
    {"points-sas-opt",      "points", K_SZ,           (fnptr)create_x_y_points_from_optimal_stochastic,
     "x/y points and ellipses for TOSAS"},
    {"points-sas-def",      "points", K_SZ,           (fnptr)create_x_y_points_from_original_stochastic,
     "x/y points and ellipses for SAS"},
    {"points-sas-adaptive-opt", "points", K_SZ,       (fnptr)create_x_y_points_from_optimal_stochastic_adaptive,
     "x/y points and ellipses for SAS single-lambda, optimized"},
    {"points-sas-adaptive-def", "points", K_SZ,       (fnptr)create_x_y_points_from_original_stochastic_adaptive,
     "x/y points and ellipses for SAS single-lambda"},

    {"pbest-stefcal",       "particle best", K_INT,   (fnptr)extract_normal_p_best_stefcal,
     "per-particle best for StEFCal"},
    {"pbest-nos",           "particle best", K_INT,   (fnptr)extract_normal_p_best_stefcal_improved,
     "per-particle best for NOS"},
    {"pbest-lm-eo",         "particle best", K_INT,   (fnptr)extract_normal_p_best_lm_non_adaptive_l1_l2,
     "per-particle best for LM (lambda_even, lambda_odd)"},
    {"pbest-adam",          "particle best", K_INT,   (fnptr)extract_normal_p_best_adam_fixed_alpha,
     "per-particle best for ADAM"},
    {"pbest-sas",           "particle best", K_INT,   (fnptr)extract_normal_p_best_stochastic_fixed_b,
     "per-particle best for SAS"},
    {"pbest-pso-adam",      "particle best", K_INT,   (fnptr)extract_pso_p_best_adam_fixed_alpha,
     "per-particle PSO best for ADAM"},
    {"pbest-pso-adam-3d",   "particle best", K_INT,   (fnptr)extract_pso_p_best_adam_3d_b1_b2,
     "per-particle PSO best for the three-dimensional ADAM search"},
    {"pbest-pso-sas",       "particle best", K_INT,   (fnptr)extract_pso_p_best_stochastic_fixed_b,
     "per-particle PSO best for SAS"},

    {"iterations-opt",      "iterations", K_NAMED,    (fnptr)extract_all_iterations_optimal_from_data_optimal_solutions,
     "every iteration count recorded for the optimized solver (-a)"},
    {"iterations-def",      "iterations", K_NAMED,    (fnptr)extract_all_iterations_original_from_data_optimal_solutions,
     "every iteration count recorded for the default solver (-a)"},

    {"sas-vs-stefcal",      "sas study", K_SZ,        (fnptr)extended_research_sas_stefcal,
     "SAS against StEFCal: iterations, FLOPs and work-equivalent iterations"},
    {"sas-vs-dostefcal",    "sas study", K_SZ,        (fnptr)extended_research_sas_dostefcal_v2,
     "SAS against DOStEFCal"},
    {"sas-raw-iterations",  "sas study", K_SZ,        (fnptr)extraction_raw_iterations_sas,
     "raw per-run SAS iteration counts"},
    {"compare-stefcal-v2",  "sas study", K_DIR,       (fnptr)compare_stefcal_v2,
     "check the flat-buffer StEFCal against the original implementation"}
};

static const size_t NUM_COMMANDS = sizeof(COMMANDS) / sizeof(COMMANDS[0]);
static const size_t NUM_ALGORITHMS = sizeof(ALGORITHMS) / sizeof(ALGORITHMS[0]);

static void usage(void) {
    const char* group = NULL;
    size_t i;

    printf("Usage: risim <command> [options]\n\n");
    printf("Options:\n");
    printf("  -d <directory>   data directory (default: %s)\n", DEFAULT_DIRECTORY);
    printf("  -o <min> <max>   antenna order range, inclusive (default: 2 2)\n");
    printf("  -a <algorithm>   solver the command applies to (default: stefcal)\n");
    printf("  -s <index>       PSO seed index, 0-3 (default: 0)\n");
    printf("  -w <workers>     calibration threads (default: as published)\n");
    printf("  -g <points>      grid points per hyperparameter axis (default: as published)\n");
    printf("  -i <iterations>  PSO iterations (default: as published)\n");
    printf("  -p <particles>   PSO particles (default: as published)\n\n");
    printf("Algorithms:\n ");
    for (i = 0; i < NUM_ALGORITHMS; i++) {
        printf(" %s", ALGORITHMS[i].name);
    }
    printf("\n\nCommands:\n");
    for (i = 0; i < NUM_COMMANDS; i++) {
        if (group == NULL || strcmp(group, COMMANDS[i].group) != 0) {
            group = COMMANDS[i].group;
            printf("\n  %s\n", group);
        }
        printf("    %-24s %s\n", COMMANDS[i].name, COMMANDS[i].help);
    }
    printf("\nAn antenna order n describes an array of n*n antennas.\n");
}

int main(int argc, char* argv[]) {
    /* PSO seeds, indexed by -s. A search is repeated once per seed. */
    int pso_seeds[] = {49052764, 12039846, 32609861, 18796345};

    char* directory = DEFAULT_DIRECTORY;
    const struct Algorithm* algorithm = &ALGORITHMS[0];
    const struct Command* command = NULL;
    int order_min = 2;
    int order_max = 2;
    int iteration = 0;
    size_t i;

    if (argc < 2) {
        usage();
        return 0;
    }

    for (i = 0; i < NUM_COMMANDS; i++) {
        if (strcmp(argv[1], COMMANDS[i].name) == 0) {
            command = &COMMANDS[i];
            break;
        }
    }
    if (command == NULL) {
        printf("Unknown command: %s\n\n", argv[1]);
        usage();
        return 1;
    }

    for (i = 2; i < (size_t)argc; i++) {
        if (strcmp(argv[i], "-d") == 0 && i + 1 < (size_t)argc) {
            directory = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0 && i + 2 < (size_t)argc) {
            order_min = atoi(argv[++i]);
            order_max = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < (size_t)argc) {
            iteration = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-w") == 0 && i + 1 < (size_t)argc) {
            override_workers = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-g") == 0 && i + 1 < (size_t)argc) {
            override_grid_length = (size_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < (size_t)argc) {
            override_pso_iterations = (size_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < (size_t)argc) {
            override_pso_particles = (size_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "-a") == 0 && i + 1 < (size_t)argc) {
            const char* name = argv[++i];
            size_t j;
            algorithm = NULL;
            for (j = 0; j < NUM_ALGORITHMS; j++) {
                if (strcmp(name, ALGORITHMS[j].name) == 0) {
                    algorithm = &ALGORITHMS[j];
                    break;
                }
            }
            if (algorithm == NULL) {
                printf("Unknown algorithm: %s\n", name);
                return 1;
            }
        } else {
            printf("Unknown option: %s\n\n", argv[i]);
            usage();
            return 1;
        }
    }

    if (order_min > order_max) {
        printf("Order range is empty: %d > %d\n", order_min, order_max);
        return 1;
    }
    if (iteration < 0 || iteration >= (int)(sizeof(pso_seeds) / sizeof(pso_seeds[0]))) {
        printf("PSO seed index out of range: %d\n", iteration);
        return 1;
    }
    if (override_workers < 0) {
        printf("Worker count must be positive.\n");
        return 1;
    }

    {
        size_t sz_min = (size_t)order_min;
        size_t sz_max = (size_t)order_max;
        char* stem = (char*)algorithm->stem;
        enum AlgorithmType type = algorithm->type;
        int order;

        switch (command->kind) {
            case K_SZ:
                ((void (*)(char*, size_t*, size_t*))command->fn)(directory, &sz_min, &sz_max);
                break;
            case K_SZ_REV:
                ((void (*)(size_t*, size_t*, char*))command->fn)(&sz_min, &sz_max, directory);
                break;
            case K_INT:
                ((void (*)(char*, int*, int*))command->fn)(directory, &order_min, &order_max);
                break;
            case K_NAMED:
                ((void (*)(char*, char*, int*, int*))command->fn)(directory, stem, &order_min, &order_max);
                break;
            case K_DIR:
                ((void (*)(char*))command->fn)(directory);
                break;
            case K_DICT:
                ((void (*)(char*, char*))command->fn)(directory, stem);
                break;
            case K_SIM:
                /* create_simulators writes one order at a time. */
                for (order = order_min; order <= order_max; order++) {
                    ((void (*)(int*, char*))command->fn)(&order, directory);
                }
                break;
            case K_GRID:
                for (order = order_min; order <= order_max; order++) {
                    ((void (*)(int*, char*, char*, enum AlgorithmType*))command->fn)(
                        &order, directory, stem, &type);
                }
                break;
            case K_ORDER:
                for (order = order_min; order <= order_max; order++) {
                    ((void (*)(char*, char*, int*))command->fn)(directory, stem, &order);
                }
                break;
            case K_PSO:
                ((void (*)(char*, char*, enum AlgorithmType*, int*, int*, int*))command->fn)(
                    directory, stem, &type, &iteration, &order_min, &order_max);
                break;
        }
    }

    return 0;
}
