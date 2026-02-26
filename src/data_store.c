/**
  * @file data_store.c
  * This is the implementation of the functions of data_store.h
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <uuid/uuid.h>
#include "data_store.h"
#include "logger.h"
#include <string.h>
#include "simulator.h"
#include "calibrator.h"
#include "data_container.h"
#include <math.h>
#include "stopwatch.h"
#include "constants.h"
#include "jagged_array.h"
#include <sys/wait.h>

/**
 * Create a DataStore struct
*/
struct DataStore* new_ds(char* identity, char* directory, double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed, int* num_workers, size_t* grid_length, enum AlgorithmType* algorithm_type, size_t* amount_hyperparameters, struct array* hyperparameters_bounds) {
    struct DataStore* data_store = malloc(sizeof(struct DataStore));

    if (data_store == NULL) {
        return NULL;
    }

    data_store->identity = malloc(MAX_STRING_LENGTH* sizeof(char));
    if (!data_store->identity) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    strcpy(data_store->identity, identity);

    data_store->directory = malloc(MAX_STRING_LENGTH* sizeof(char));
    if (!data_store->directory) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    strcpy(data_store->directory, directory);

    data_store->h_min = malloc(sizeof(double));
    if (!data_store->h_min) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->h_min = *h_min;
    data_store->h_max = malloc(sizeof(double));
    if (!data_store->h_max) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->h_max = *h_max;
    data_store->dec = malloc(sizeof(double));
    if (!data_store->dec) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->dec = *dec;
    data_store->lat = malloc(sizeof(double));
    if (!data_store->lat) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->lat = *lat;
    data_store->freq = malloc(sizeof(double));
    if (!data_store->freq) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->freq = *freq;
    data_store->layout = malloc(sizeof(enum LayoutType));
    if (!data_store->layout) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->layout = *layout;
    data_store->n_steps = malloc(sizeof(size_t));
    if (!data_store->n_steps) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->n_steps = *n_steps;
    data_store->bas_len = malloc(sizeof(size_t));
    if (!data_store->bas_len) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->bas_len = *bas_len;
    data_store->order = malloc(sizeof(int));
    if (!data_store->order) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->order = *order;
    data_store->snr = malloc(sizeof(double));
    if (!data_store->snr) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->snr = *snr;
    data_store->max_amp = malloc(sizeof(double));
    if (!data_store->max_amp) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->max_amp = *max_amp;
    data_store->seed = malloc(sizeof(int));
    if (!data_store->seed) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->seed = *seed;
    data_store->num_workers = malloc(sizeof(int));
    if (!data_store->num_workers) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->num_workers = *num_workers;
    data_store->grid_length = malloc(sizeof(size_t));
    if (!data_store->grid_length) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->grid_length = *grid_length;
    data_store->algorithm_type = malloc(sizeof(enum AlgorithmType));
    if (!data_store->algorithm_type) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->algorithm_type = *algorithm_type;
    data_store->amount_hyperparameters = malloc(sizeof(size_t));
    if (!data_store->amount_hyperparameters) {
        free_ds(data_store);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_store->amount_hyperparameters = *amount_hyperparameters;

    data_store->hyperparameters_bounds = zeros_a(hyperparameters_bounds->num_dimensions, hyperparameters_bounds->dimensions, hyperparameters_bounds->type);
    copy_over_a(data_store->hyperparameters_bounds, hyperparameters_bounds, (int[]){-1}, (int[]){-1});

    data_store->iterations_dcon = NULL;
    data_store->gains_dcon = NULL;
    data_store->all_gains_dcon = NULL;
    data_store->damping_g_dcon = NULL;
    data_store->damping_g_old_dcon = NULL;

    return data_store;
}

/**
 * Free a s DataStore struct
*/
void free_ds(struct DataStore* data_store) {
    if (data_store != NULL) {
        if (data_store->identity != NULL) {
            free(data_store->identity);
        }
        if (data_store->directory != NULL) {
            free(data_store->directory);
        }
        if (data_store->h_min != NULL) {
            free(data_store->h_min);
        }
        if (data_store->h_max != NULL) {
            free(data_store->h_max);
        }
        if (data_store->dec != NULL) {
            free(data_store->dec);
        }
        if (data_store->lat != NULL) {
            free(data_store->lat);
        }
        if (data_store->freq != NULL) {
            free(data_store->freq);
        }
        if (data_store->layout != NULL) {
            free(data_store->layout);
        }
        if (data_store->n_steps != NULL) {
            free(data_store->n_steps);
        }
        if (data_store->bas_len != NULL) {
            free(data_store->bas_len);
        }
        if (data_store->order != NULL) {
            free(data_store->order);
        }
        if (data_store->snr != NULL) {
            free(data_store->snr);
        }
        if (data_store->max_amp != NULL) {
            free(data_store->max_amp);
        }
        if (data_store->seed != NULL) {
            free(data_store->seed);
        }
        if (data_store->num_workers != NULL) {
            free(data_store->num_workers);
        }
        if (data_store->grid_length != NULL) {
            free(data_store->grid_length);
        }
        if (data_store->algorithm_type != NULL) {
            free(data_store->algorithm_type);
        }
        if (data_store->amount_hyperparameters != NULL) {
            free(data_store->amount_hyperparameters);
        }
        if (data_store->hyperparameters_bounds != NULL) {
            free_a(data_store->hyperparameters_bounds);
        }
        if (data_store->iterations_dcon != NULL) {
            free_dcon(data_store->iterations_dcon); 
        }
        if (data_store->gains_dcon != NULL) {
            free_dcon(data_store->gains_dcon); 
        }
        if (data_store->all_gains_dcon != NULL) {
            free_dcon(data_store->all_gains_dcon); 
        }
        if (data_store->damping_g_dcon != NULL) {
            free_dcon(data_store->damping_g_dcon); 
        }
        if (data_store->damping_g_old_dcon != NULL) {
            free_dcon(data_store->damping_g_old_dcon); 
        }

        free(data_store);
    }
}

/**
 * This will print the data dictionary
*/
void print_ds(struct DataStore* data_store) {
    if (data_store == NULL) {
      return;
    }
    printf("{\n");
    printf("  identity: %s\n",data_store->identity);
    printf("  directory: %s\n",data_store->directory);
    printf("  h_min: %lf\n",*data_store->h_min);
    printf("  h_max: %lf\n",*data_store->h_max);
    printf("  dec: %lf\n",*data_store->dec);
    printf("  lat: %lf\n",*data_store->lat);
    printf("  freq: %lf\n",*data_store->freq);
    int layout_int = *data_store->layout;
    printf("  layout: %d\n",layout_int);
    printf("  n_steps: %zu\n",*data_store->n_steps);
    printf("  bas_len: %lf\n",*data_store->bas_len);
    printf("  order: %d\n",*data_store->order);
    printf("  snr: %lf\n",*data_store->snr);
    printf("  max_amp: %lf\n",*data_store->max_amp);
    printf("  seed: %d\n",*data_store->seed);
    printf("  num_workers: %d\n",*data_store->num_workers);
    printf("  grid_length: %zu\n",*data_store->grid_length);
    int algorithm_type_int = *data_store->algorithm_type;
    printf("  algorithm_type: %d\n",algorithm_type_int);
    printf("  amount_hyperparameters: %zu\n",*data_store->amount_hyperparameters);
    double* arr = get_a(data_store->hyperparameters_bounds);
    printf("  hyperparameter bounds: [");
    for (int i = 0; i < get_size_a(data_store->hyperparameters_bounds); i++) {
        if (i == 0) {
            printf("%lf",arr[i]);
        } else {
            printf(", %lf",arr[i]);
        }
    }
    printf("]\n");
    printf("}\n");
}


/**
 * This function will start a storing data with specific starting parameters
*/
size_t start_ds(struct DataStore* data_store) {

    //simulating the data needed for calibration.
    // struct Simulator* simulator = initialize_simulator(data_store->h_min, data_store->h_max, data_store->dec, data_store->lat, data_store->freq, data_store->layout, data_store->n_steps, data_store->bas_len, data_store->order, data_store->snr, data_store->max_amp, data_store->seed);
    // generate_antenna_layout(simulator);
    // uv_tracks(simulator);
    // create_point_sources(simulator, &(size_t){100},&(double){3.0}, &(double){2.0});
    // create_antenna_gains(simulator,&(double){5.0});
    // create_vis_mat(simulator, NULL, true);
    // create_vis_mat(simulator, NULL, false);
    char filename [256]; 
    //snprintf(filename, sizeof(filename), "./data/simulators/sim_seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf.sconfig", *data_store->seed, *data_store->order, *data_store->snr, *data_store->max_amp);
    //snprintf(filename, sizeof(filename), "./simulators/sim_seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf.sconfig", *data_store->seed, *data_store->order, *data_store->snr, *data_store->max_amp);
    snprintf(filename, sizeof(filename), "/media/pieter-colesky/Expansion/simulators/sim_seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf.sconfig", *data_store->seed, *data_store->order, *data_store->snr, *data_store->max_amp);
    //snprintf(filename, sizeof(filename), "/media/pieter/MSC_BACKUP/simulators/sim_seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf.sconfig", *data_store->seed, *data_store->order, *data_store->snr, *data_store->max_amp);
    struct Simulator* simulator = read_from_file_s(filename);
    struct Calibrator* calibrator = new_c(simulator);
    set_parallel_c(calibrator, data_store->num_workers);

    struct array* linspace_h1 = NULL;
    struct array* linspace_h2 = NULL;
    struct array* linspace_h3 = NULL;

    struct array* results_averages = NULL;

    struct array* hyperparameter_bounds = data_store->hyperparameters_bounds;

    //Creating the hyperparameter bounds linspaces
    switch (*data_store->amount_hyperparameters) {
        case 1: {
            linspace_h1 = linspace_a(get_element_a(hyperparameter_bounds, (size_t[]){0}), get_element_a(hyperparameter_bounds, (size_t[]){1}), data_store->grid_length, &(enum DataType){DOUBLE});
            results_averages = zeros_a(&(size_t){1}, (size_t[]){*data_store->grid_length}, &(enum DataType){DOUBLE});

            int progress = get_progress_dcon(data_store->gains_dcon);
            size_t index = 0;

            for (size_t i = 0; i < *data_store->grid_length; i++) {
                if (index >= progress && index < *data_store->gains_dcon->size) {
                    switch (*data_store->algorithm_type) {
                        case GRADIENT_DESENT: {
                            log_not_implemented(__FILE__, __LINE__);
                            break;
                        }
                        case ADAGRAD: {
                            log_not_implemented(__FILE__, __LINE__);
                            break;
                        }
                        default: {
                            log_invalid_algorithm_type(__FILE__, __LINE__);
                            break;
                        }
                    }
                }
                index++;
            } 
            break;
        }
        case 2: {
            linspace_h1 = linspace_a(get_element_a(hyperparameter_bounds, (size_t[]){0}), get_element_a(hyperparameter_bounds, (size_t[]){1}), data_store->grid_length, &(enum DataType){DOUBLE});
            linspace_h2 = linspace_a(get_element_a(hyperparameter_bounds, (size_t[]){2}), get_element_a(hyperparameter_bounds, (size_t[]){3}), data_store->grid_length, &(enum DataType){DOUBLE});
            results_averages = zeros_a(&(size_t){2}, (size_t[]){*data_store->grid_length, *data_store->grid_length}, &(enum DataType){DOUBLE});

            int progress = get_progress_dcon(data_store->gains_dcon);
            size_t index = 0;

            for (size_t i = 0; i < *data_store->grid_length; i++) {
                for (size_t j = 0; j < *data_store->grid_length; j++) {
                    if (index >= progress && index < *data_store->gains_dcon->size) {
                        switch (*data_store->algorithm_type) {
                            case LEVENBERG_MARQUARDT: {
                                double l1 = *(double*){get_element_a(linspace_h1, (size_t[]){i})};
                                double l2 = *(double*){get_element_a(linspace_h2, (size_t[]){j})};

                                if (PRINT_DATA) {
                                    printf("Levenberg Marquardt: h1 = %lf, h2 = %lf\n", l1, l2);
                                }
                                //solve
                                solve_G_LM(calibrator, &(size_t){IMAX}, &(double){TAU}, &l1, &l2);
                                //extract data
                                double* average_iterations = get_average_iterations_dc(calibrator->datacollector);
                                set_element_a(results_averages, (size_t[]){i,j}, average_iterations);
                                if (PRINT_DATA) {
                                    printf("Average Iterations = %f\n", *average_iterations);
                                }
                                free(average_iterations);

                                //filename
                                char filename[256];
                                sprintf(filename, "levenberg_marquardt_%ld_%ld",i,j);
                                save_data_to_structure_ds(data_store, calibrator, &index, filename);
                                break;
                            }
                            default: {
                                log_invalid_algorithm_type(__FILE__, __LINE__);
                            }
                        }
                    }
                    index++;
                } 
            }
            break;
        }
        case 3: {
            linspace_h1 = linspace_a(get_element_a(hyperparameter_bounds, (size_t[]){0}), get_element_a(hyperparameter_bounds, (size_t[]){1}), data_store->grid_length, &(enum DataType){DOUBLE});
            linspace_h2 = linspace_a(get_element_a(hyperparameter_bounds, (size_t[]){2}), get_element_a(hyperparameter_bounds, (size_t[]){3}), data_store->grid_length, &(enum DataType){DOUBLE});
            linspace_h3 = linspace_a(get_element_a(hyperparameter_bounds, (size_t[]){4}), get_element_a(hyperparameter_bounds, (size_t[]){5}), data_store->grid_length, &(enum DataType){DOUBLE});
            results_averages = zeros_a(&(size_t){3}, (size_t[]){*data_store->grid_length, *data_store->grid_length, *data_store->grid_length}, &(enum DataType){DOUBLE});

            int progress = get_progress_dcon(data_store->gains_dcon);
            size_t index = 0;

            for (size_t i = 0; i < *data_store->grid_length; i++) {
                for (size_t j = 0; j < *data_store->grid_length; j++) {
                    for (size_t h = 0; h < *data_store->grid_length; h++) {
                        if (index >= progress && index < *data_store->gains_dcon->size) {
                            switch (*data_store->algorithm_type) {
                                case ADAM: {
                                    log_not_implemented(__FILE__, __LINE__);
                                    break;
                                }
                                default: {
                                    log_invalid_algorithm_type(__FILE__, __LINE__);
                                    break;
                                }
                            }
                        }
                        index++;
                    }
                } 
            }
            break;
        }
        default: {
            break;
        }
    }

    switch (*data_store->amount_hyperparameters) {
        case 1: {
            if (linspace_h1 != NULL) {
                free_a(linspace_h1);
            }
            if (results_averages != NULL) {
                free_a(results_averages);
            }
            break;
        }
        case 2: {
            if (linspace_h1 != NULL) {
                free_a(linspace_h1);
            }
            if (linspace_h2 != NULL) {
                free_a(linspace_h2);
            }
            if (results_averages != NULL) {
                free_a(results_averages);
            }
            break;
        }
        case 3: {
            if (linspace_h1 != NULL) {
                free_a(linspace_h1);
            }
            if (linspace_h2 != NULL) {
                free_a(linspace_h2);
            }
            if (linspace_h3 != NULL) {
                free_a(linspace_h3);
            }
            if (results_averages != NULL) {
                free_a(results_averages);
            }
            break;
        }
        default: {
            break;
        }
    }

    free_c(calibrator);
    free_s(simulator);

    return 0;
}

/**
 * save DataStore to file
*/
void save_to_file_ds(struct DataStore* data_store) {
    char filepath[512];
    sprintf(filepath, "%s/%s.dsconfig", data_store->directory, data_store->identity);

    create_directory(data_store->directory);

    FILE *combinedDataFile = fopen(filepath, "w");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return;
    }
    fprintf(combinedDataFile, "%lf\n", *data_store->h_min);
    fprintf(combinedDataFile, "%lf\n", *data_store->h_max);
    fprintf(combinedDataFile, "%lf\n", *data_store->dec);
    fprintf(combinedDataFile, "%lf\n", *data_store->lat);
    fprintf(combinedDataFile, "%lf\n", *data_store->freq);
    int layout_type = *data_store->layout;
    fprintf(combinedDataFile, "%d\n", layout_type);
    fprintf(combinedDataFile, "%zu\n", *data_store->n_steps);
    fprintf(combinedDataFile, "%lf\n", *data_store->bas_len);
    fprintf(combinedDataFile, "%d\n", *data_store->order);
    fprintf(combinedDataFile, "%lf\n", *data_store->snr);
    fprintf(combinedDataFile, "%lf\n", *data_store->max_amp);
    fprintf(combinedDataFile, "%d\n", *data_store->seed);
    fprintf(combinedDataFile, "%d\n", *data_store->num_workers);
    fprintf(combinedDataFile, "%zu\n", *data_store->grid_length);
    int algorithm_type = *data_store->algorithm_type;
    fprintf(combinedDataFile, "%d\n", algorithm_type);
    fprintf(combinedDataFile, "%zu\n", *data_store->amount_hyperparameters);
    double* arr = get_a(data_store->hyperparameters_bounds);
    for (size_t i = 0; i < get_size_a(data_store->hyperparameters_bounds); i ++) {
        fprintf(combinedDataFile, "%lf\n", arr[i]);
    }
    fclose(combinedDataFile);
}

/**
 * Read .array file
*/
struct DataStore* read_from_file_ds(char* directory, char* identity) {
    char filepath[512];
    sprintf(filepath, "%s/%s.dsconfig", directory, identity);

    if(!ends_with_string(filepath, ".dsconfig")) {
        log_invalid_file_extension(__FILE__,__LINE__);
    }

    FILE *combinedDataFile = fopen(filepath, "r");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return NULL;
    }

    double h_min = 0;
    double h_max = 0;
    double dec = 0;
    double lat = 0;
    double freq = 0;
    int layout_type_int = 0;
    size_t n_steps = 0;
    double bas_len = 0;
    int order = 0;
    double snr = 0;
    double max_amp = 0;
    int seed = 0;
    int num_workers = 0;
    size_t grid_length = 0;
    int algorithm_type_int = 0;
    size_t amount_hyperparameters = 0;

    if (fscanf(combinedDataFile, "%lf", &h_min) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &h_max) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &dec) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &lat) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &freq) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%d", &layout_type_int) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%zu", &n_steps) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &bas_len) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%d", &order) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &snr) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &max_amp) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%d", &seed) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%d", &num_workers) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%zu", &grid_length) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%d", &algorithm_type_int) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%zu", &amount_hyperparameters) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }

    struct array* hyperparameter_bounds = zeros_a(&(size_t){1}, (size_t[]){amount_hyperparameters*2}, &(enum DataType){DOUBLE});

    double bound = 0;
    for (size_t i = 0; i < amount_hyperparameters*2; i++) {
        if (fscanf(combinedDataFile, "%lf", &bound) != 1) {
            log_error_fscan(__FILE__,__LINE__);
        }
        set_element_a(hyperparameter_bounds,(size_t[]){i}, &bound);
    }

    char* filename = extract_filename_from_filepath(filepath);

    enum LayoutType layout = layout_type_int;
    enum AlgorithmType algorithm = algorithm_type_int;

    struct DataStore* read_data_store = new_ds(filename, directory, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &num_workers, &grid_length, &algorithm, &amount_hyperparameters, hyperparameter_bounds);
    
    free_a(hyperparameter_bounds);
    free(filename);

    fclose(combinedDataFile);

    return read_data_store;
}

/**
 * Create a DataStore struct
*/
struct DataStore* new_save_ds(char* identity, char* directory, double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed, int* num_workers, size_t* grid_length, enum AlgorithmType* algorithm_type, size_t* amount_hyperparameters, struct array* hyperparameters_bounds) {
    struct DataStore* data_store = new_ds(identity, directory, h_min, h_max, dec, lat, freq, layout, n_steps, bas_len, order, snr, max_amp, seed, num_workers, grid_length, algorithm_type, amount_hyperparameters, hyperparameters_bounds);
    save_to_file_ds(data_store);
    return data_store;
}

/**
 * This fuction will set up the directory structure for a data store
*/
struct DataStore* new_structure_ds(char* identity, char* directory, double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed, int* num_workers, size_t* grid_length, enum AlgorithmType* algorithm_type, size_t* amount_hyperparameters, struct array* hyperparameters_bounds) {
    struct DataStore* data_store = new_ds(identity, directory, h_min, h_max, dec, lat, freq, layout, n_steps, bas_len, order, snr, max_amp, seed, num_workers, grid_length, algorithm_type, amount_hyperparameters, hyperparameters_bounds);
    save_to_file_ds(data_store);

    char file_path[256];
    sprintf(file_path, "%s/%s", data_store->directory, data_store->identity);

    size_t length = 0;

    switch (*amount_hyperparameters) {
        case 1: {
            length = *grid_length;
            break;
        }
        case 2: {
            length = *grid_length * *grid_length;
            break;
        }
        case 3: {
            length = *grid_length * *grid_length * *grid_length;
            break;
        }
        default: {
            length = 0;
        }
    }


    data_store->iterations_dcon = new_save_dcon("iterations", file_path, &(size_t){1}, &length);
    
    data_store->gains_dcon = new_save_dcon("gains", file_path, &(size_t){2}, &length);
    if (ALL_DATA) {
        data_store->all_gains_dcon = new_save_dcon("all_gains", file_path, &(size_t){2}, &length);
        data_store->damping_g_dcon = new_save_dcon("damping_g", file_path, &(size_t){2}, &length);
        data_store->damping_g_old_dcon = new_save_dcon("damping_g_old", file_path, &(size_t){2}, &length);
    }

    return data_store;
}

/**
 * Read structure 
*/
struct DataStore* read_structure_ds(char* directory, char* identity) {
    char filepath[512];
    sprintf(filepath, "%s/%s.dsconfig", directory, identity);

    if(!ends_with_string(filepath, ".dsconfig")) {
        log_invalid_file_extension(__FILE__,__LINE__);
    }

    FILE *combinedDataFile = fopen(filepath, "r");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return NULL;
    }

    double h_min = 0;
    double h_max = 0;
    double dec = 0;
    double lat = 0;
    double freq = 0;
    int layout_type_int = 0;
    size_t n_steps = 0;
    double bas_len = 0;
    int order = 0;
    double snr = 0;
    double max_amp = 0;
    int seed = 0;
    int num_workers = 0;
    size_t grid_length = 0;
    int algorithm_type_int = 0;
    size_t amount_hyperparameters = 0;

    if (fscanf(combinedDataFile, "%lf", &h_min) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &h_max) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &dec) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &lat) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &freq) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%d", &layout_type_int) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%zu", &n_steps) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &bas_len) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%d", &order) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &snr) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &max_amp) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%d", &seed) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%d", &num_workers) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%zu", &grid_length) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%d", &algorithm_type_int) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%zu", &amount_hyperparameters) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }

    struct array* hyperparameter_bounds = zeros_a(&(size_t){1}, (size_t[]){amount_hyperparameters*2}, &(enum DataType){DOUBLE});

    double bound = 0;
    for (size_t i = 0; i < amount_hyperparameters*2; i++) {
        if (fscanf(combinedDataFile, "%lf", &bound) != 1) {
            log_error_fscan(__FILE__,__LINE__);
        }
        set_element_a(hyperparameter_bounds,(size_t[]){i}, &bound);
    }

    char* filename = extract_filename_from_filepath(filepath);

    enum LayoutType layout = layout_type_int;
    enum AlgorithmType algorithm = algorithm_type_int;


    struct DataStore* read_data_store = new_ds(filename, directory, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &num_workers, &grid_length, &algorithm, &amount_hyperparameters, hyperparameter_bounds);
    free_a(hyperparameter_bounds);

    char file_path_data_containers_iterations[256];
    sprintf(file_path_data_containers_iterations, "%s/%s", read_data_store->directory, read_data_store->identity);

    read_data_store->iterations_dcon = read_from_file_dcon(file_path_data_containers_iterations, "iterations");

    char file_path_data_containers_gains[256];
    sprintf(file_path_data_containers_gains, "%s/%s", read_data_store->directory, read_data_store->identity);

    read_data_store->gains_dcon = read_from_file_dcon(file_path_data_containers_gains, "gains");

    if (ALL_DATA) {
        char file_path_data_containers_all_gains[256];
        sprintf(file_path_data_containers_all_gains, "%s/%s", read_data_store->directory, read_data_store->identity);

        read_data_store->all_gains_dcon = read_from_file_dcon(file_path_data_containers_gains, "all_gains");

        char file_path_data_containers_damping_g[256];
        sprintf(file_path_data_containers_damping_g, "%s/%s", read_data_store->directory, read_data_store->identity);

        read_data_store->damping_g_dcon = read_from_file_dcon(file_path_data_containers_damping_g, "damping_g");

        char file_path_data_containers_damping_g_old[256];
        sprintf(file_path_data_containers_damping_g_old, "%s/%s", read_data_store->directory, read_data_store->identity);

        read_data_store->damping_g_old_dcon = read_from_file_dcon(file_path_data_containers_damping_g_old, "damping_g_old");
    }
    
    free(filename);

    fclose(combinedDataFile);

    return read_data_store;
}

// /**
//  * Saving the data to structure
// */
// void save_data_to_structure_ds(struct DataStore* data_store, struct Calibrator* calibrator, size_t* index, char* filename) {
//     printf("Saving Data\n");
//     //save iterations
//     //iterations_array does not have to be freed
//     struct array* iterations_array = get_iterations_dc(calibrator->datacollector);
//     set_new_array_dcon(&data_store->iterations_dcon, index, filename, iterations_array);
//     // printf("Iterations Done\n");

//     // print_a(calibrator->datacollector->gains[0]);
//     struct Stopwatch* stopwatch = new_sw();
//     //save gains
//     struct DataContainer* data_container_gains_ij = set_new_datacontainer_1_dcon(&data_store->gains_dcon, index, filename, calibrator->n_steps);
//     for (size_t ts = 0; ts < *calibrator->n_steps; ts++) {
//         char filename_2[256];
//         sprintf(filename_2, "t_%ld",ts);
//         set_new_array_dcon(&data_container_gains_ij, &ts,filename_2, calibrator->datacollector->gains[ts]);
//     }
//     free_dcon(data_container_gains_ij);
//     printf("gains done\n");
//     print_sw(stopwatch);
//     free_sw(stopwatch);

//     size_t data_container_all_gains_ij_size = RANDOM_TIMESTEP_AMOUNT;
//     struct DataContainer** data_containers_array = malloc(data_container_all_gains_ij_size * sizeof(struct DataContainer*));
//     struct DataContainer* data_container_all_gains_ij = set_new_datacontainer_0_dcon(&data_store->all_gains_dcon, index, filename, &data_container_all_gains_ij_size);
//     for (size_t ts = 0; ts < RANDOM_TIMESTEP_AMOUNT; ts++) {
//         printf("%zu\n", ts);
//         int iterations_ts = *(int*)get_element_a(iterations_array, (size_t[]){ts});
//         //creating a filename for the timestep datacontainer holding arrays
//         char filename_2[256];
//         sprintf(filename_2, "t_%ld",ts);
//         size_t iteration_ts_zu = iterations_ts;
//         struct DataContainer* data_container_all_gains_t = set_new_datacontainer_1_dcon(&data_container_all_gains_ij, &ts, filename_2, &iteration_ts_zu);
//         data_containers_array[ts] = data_container_all_gains_t;
//     }
//     free_dcon(data_container_all_gains_ij);

//     for (size_t ts = 0; ts < RANDOM_TIMESTEP_AMOUNT; ts++) {
//         int iterations_ts = *(int*)get_element_a(iterations_array, (size_t[]){ts});
//         if (iterations_ts >= MAX_ITERATIONS_CAPTURE) {
//             // print_a(calibrator->datacollector->all_gains[0]);
//             struct Stopwatch* stopwatch = new_sw();
//             for (size_t iter = 0; iter < MAX_ITERATIONS_CAPTURE; iter++) {
//                 //now you can save all the arrays for a specific timestep
//                 char filename_3[256];
//                 sprintf(filename_3, "i_%ld",iter);
//                 int pos = (ts * MAX_ITERATIONS_CAPTURE) + iter;
//                 set_new_array_dcon(&data_containers_array[ts], &iter, filename_3, calibrator->datacollector->all_gains[pos]);
//             }
//             print_sw(stopwatch);
//             free_sw(stopwatch);
//         } else {
//             struct Stopwatch* stopwatch = new_sw();
//             for (size_t iter = 0; iter < iterations_ts; iter++) {
//                 //now you can save all the arrays for a specific timestep
//                 char filename_3[256];
//                 sprintf(filename_3, "i_%ld",iter);
//                 int pos = (ts * MAX_ITERATIONS_CAPTURE) + iter;
//                 set_new_array_dcon(&data_containers_array[ts], &iter, filename_3, calibrator->datacollector->all_gains[pos]);
//             }
//             print_sw(stopwatch);
//             free_sw(stopwatch);
//         }
//     }

//     for (size_t ts = 0; ts < RANDOM_TIMESTEP_AMOUNT; ts++) {
//         free_dcon(data_containers_array[ts]);
//     }
//     free(data_containers_array);

//     printf("All Gains Done\n");


//     //save damping_g
//     struct DataContainer* data_container_damping_g_ij = set_new_datacontainer_1_dcon(&data_store->damping_g_dcon, index, filename, calibrator->n_steps);
//     for (size_t ts = 0; ts < RANDOM_TIMESTEP_AMOUNT; ts++) {
//         // printf("%zu\n", ts);
//         char filename_2[256];
//         sprintf(filename_2, "t_%ld",ts);
//         set_new_array_dcon(&data_container_damping_g_ij, &ts, filename_2, calibrator->datacollector->damping_g[ts]);
//     }
//     free_dcon(data_container_damping_g_ij);

//     // printf("Damping_g Done\n");

//     //save damping_g_old
//     struct DataContainer* data_container_damping_g_old_ij = set_new_datacontainer_1_dcon(&data_store->damping_g_old_dcon, index, filename, calibrator->n_steps);
//     for (size_t ts = 0; ts < RANDOM_TIMESTEP_AMOUNT; ts++) {
//         // printf("%zu\n", ts);
//         char filename_2[256];
//         sprintf(filename_2, "t_%ld",ts);
//         set_new_array_dcon(&data_container_damping_g_old_ij, &ts, filename_2, calibrator->datacollector->damping_g_old[ts]);
//     }
//     free_dcon(data_container_damping_g_old_ij);
//     // printf("Damping_g_old Done\n");
// }

/**
 * Saving the data to structure
*/
void save_data_to_structure_ds(struct DataStore* data_store, struct Calibrator* calibrator, size_t* index, char* filename) {
    printf("Saving Data\n");
    //save iterations
    //iterations_array does not have to be freed
    struct array* iterations_array = get_iterations_dc(calibrator->datacollector);
    set_new_array_dcon(&data_store->iterations_dcon, index, filename, iterations_array);

    //save gains
    set_new_jarray_dcon(&data_store->gains_dcon, index, filename, calibrator->datacollector->gains);

    if (ALL_DATA) {
        //save all gains
        //calculating the sum of iterations_array
        int* arr_int = get_a(iterations_array);
        size_t sum = 0;
        for (size_t s = 0; s < get_size_a(iterations_array); s++) {
            int s_int = s;
            if (contains_element_a(calibrator->random_timesteps, &(int){s_int})) {
                if (arr_int[s] >= MAX_ITERATIONS_CAPTURE) {
                    sum += MAX_ITERATIONS_CAPTURE;
                } else {
                    sum += arr_int[s];
                }
            }
        }
        //Creating a jarray with size sum
        struct Jaggedarray* new_all_gains_jagged_array = new_ja(&sum);
        //go through the iterations there is a 1000 for each
        //Creating a datacontainer holding datacontainers
        printf("Random Timesteps: ");
        fflush(stdout);
        size_t count = 0;
        for (size_t ts = 0; ts < RANDOM_TIMESTEP_AMOUNT; ts++) {
            printf("%zu ",ts);
            fflush(stdout);
            //Need to get the sum of the iterations array.
            //Need to get the index.
            int* index_into_iterations_array = get_element_a(calibrator->random_timesteps, (size_t[]){ts});
            int iterations_ts = *(int*)get_element_a(iterations_array, (size_t[]){*index_into_iterations_array});
            if (iterations_ts >= MAX_ITERATIONS_CAPTURE) {
                for (size_t iter = 0; iter < MAX_ITERATIONS_CAPTURE; iter++) {
                    size_t pos = (ts * MAX_ITERATIONS_CAPTURE) + iter;
                    set_ja(new_all_gains_jagged_array, get_ja(calibrator->datacollector->all_gains, &pos), &count);
                    count += 1;
                }
            } else {
                for (size_t iter = 0; iter < iterations_ts; iter++) {
                    size_t pos = (ts * MAX_ITERATIONS_CAPTURE) + iter;
                    set_ja(new_all_gains_jagged_array, get_ja(calibrator->datacollector->all_gains, &pos), &count);
                    count += 1;
                }
            }
        }
        printf("\n");
        set_new_jarray_dcon(&data_store->all_gains_dcon, index, filename, new_all_gains_jagged_array);
        free_ja(new_all_gains_jagged_array);

        //save damping_g
        set_new_jarray_dcon(&data_store->damping_g_dcon, index, filename, calibrator->datacollector->damping_g);
        //save damping_g_old
        set_new_jarray_dcon(&data_store->damping_g_old_dcon, index, filename, calibrator->datacollector->damping_g_old);
    }
    
    
}

/**
 * This function will extract the information from a datastore
 */
struct Jaggedarray* extract_info_ds(struct DataStore* data_store, char* directory) {
    if (data_store == NULL) {
        log_null_value(__FILE__, __LINE__);
    }

    bool done = check_done_dcon(data_store->iterations_dcon);
    if (done) {
        struct array* means = zeros_a(&(size_t){1}, (size_t[]){*data_store->iterations_dcon->size}, &(enum DataType){DOUBLE});
        struct array* stds = zeros_a(&(size_t){1}, (size_t[]){*data_store->iterations_dcon->size}, &(enum DataType){DOUBLE});
        struct array* residual_norms = zeros_a(&(size_t){1}, (size_t[]){*data_store->gains_dcon->size}, &(enum DataType){DOUBLE});

        char filename_simulator [256];
        snprintf(filename_simulator, sizeof(filename_simulator), "%s/simulators/sim_seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf.sconfig",directory, *data_store->seed, *data_store->order, *data_store->snr, *data_store->max_amp);
        struct Simulator* simulator = read_from_file_s(filename_simulator);

        for (size_t i = 0; i < *data_store->iterations_dcon->size; i++) {
            char* filename = data_store->iterations_dcon->items[i];
            char filepath[512];
            sprintf(filepath, "%s/%s/%s.array", data_store->iterations_dcon->directory, data_store->iterations_dcon->identity, filename);
            struct array* array = read_from_file_a(filepath);
            double* mean = (double*)mean_a(array);
            double* std = (double*)std_a(array);

            set_element_a(means, &(size_t){i}, mean);
            set_element_a(stds, &(size_t){i}, std);

            free(std);
            free(mean);
            free_a(array);
        }

        struct array** M_t_s = malloc(*simulator->n_steps * sizeof(struct array*));
        struct array** D_t_s = malloc(*simulator->n_steps * sizeof(struct array*));
        for (size_t step = 0; step < *simulator->n_steps; step++) {
            struct array* M_t = slice_a(simulator->M, (int[]){-1,-1,step});
            struct array* D_t = slice_a(simulator->D, (int[]){-1,-1,step});
            M_t_s[step] = M_t;
            D_t_s[step] = D_t;
        } 

        for (size_t i = 0; i < *data_store->gains_dcon->size; i++) {
            //read the jarray for the pixel
            char* filename = data_store->gains_dcon->items[i];
            char filepath[512];
            sprintf(filepath, "%s/%s/%s.jarray", data_store->gains_dcon->directory, data_store->gains_dcon->identity, filename);
            struct Jaggedarray* jarray = read_from_file_ja(filepath);
            struct Jaggedarray* jarray_residuals = new_ja(jarray->size);

            for (size_t t = 0; t < *jarray->size; t++) {
                struct array* r = compute_r_N(M_t_s[t], D_t_s[t], jarray->jarray[t]);
                set_ja(jarray_residuals, r, &t);
                free_a(r);
            }

            double* residual_norm = (double*)norm_ja(jarray_residuals);
            set_element_a(residual_norms, &i, &(double){*residual_norm});
            
            free(residual_norm);
            free_ja(jarray_residuals);
            free_ja(jarray);            
        }

        for (size_t step = 0; step < *simulator->n_steps; step++) {
            free_a(M_t_s[step]);
            free_a(D_t_s[step]);
        } 
        free(M_t_s);
        free(D_t_s);

        struct array* linspace_h1 = linspace_a(get_element_a(data_store->hyperparameters_bounds, (size_t[]){0}), get_element_a(data_store->hyperparameters_bounds, (size_t[]){1}), data_store->grid_length, &(enum DataType){DOUBLE});
        struct array* linspace_h2 = linspace_a(get_element_a(data_store->hyperparameters_bounds, (size_t[]){2}), get_element_a(data_store->hyperparameters_bounds, (size_t[]){3}), data_store->grid_length, &(enum DataType){DOUBLE});

        double min_l1 = 0;
        double min_l2 = 0;
        double min_mean = INFINITY;
        double min_std = INFINITY;
        double min_residual_norm = INFINITY;

        double min_l1_0_1 = 0;
        double min_l2_0_1 = 0;
        double min_mean_0_1 = 0;
        double min_std_0_1 = 0;

        double min_l1_1_0 = 0;
        double min_l2_1_0 = 0;
        double min_mean_1_0 = 0;
        double min_std_1_0 = 0;

        //Getting the minimum point coordinates
        size_t stefcal_improved = 0;
        size_t count = 0;
        for (size_t i = 0; i < *data_store->grid_length; i++) {
            for (size_t j = 0; j < *data_store->grid_length; j++) {
                if (stefcal_improved) {
                    double position_l1 = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                    double position_l2 = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                    if ((position_l1 <= -0.05 || position_l1 >= 0.05) &&
                        (position_l2 <= -0.05 || position_l2 >= 0.05)) 
                    {
                        if (*(double*)get_element_a(means, (size_t[]){count}) < min_mean) {
                            min_l1 = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                            min_l2 = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                            min_mean = *(double*)get_element_a(means, (size_t[]){count});
                            min_std = *(double*)get_element_a(stds, (size_t[]){count});
                            min_residual_norm = *(double*)get_element_a(residual_norms, (size_t[]){count});
                        } else if (*(double*)get_element_a(means, (size_t[]){count}) == min_mean) {
                            if (fabs(*(double*)get_element_a(stds, (size_t[]){count}) - min_std) > EPSILON_DATA_STORE &&
                                    *(double*)get_element_a(stds, (size_t[]){count}) < min_std) {
                                min_l1 = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                                min_l2 = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                                min_mean = *(double*)get_element_a(means, (size_t[]){count});
                                min_std = *(double*)get_element_a(stds, (size_t[]){count});
                                min_residual_norm = *(double*)get_element_a(residual_norms, (size_t[]){count});
                            } else if (fabs(*(double*)get_element_a(stds, (size_t[]){count}) - min_std) <= EPSILON_DATA_STORE) {
                                if (fabs(*(double*)get_element_a(residual_norms, (size_t[]){count}) - min_residual_norm) > EPSILON_DATA_STORE &&
                                        *(double*)get_element_a(residual_norms, (size_t[]){count}) < min_residual_norm) {
                                    min_l1 = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                                    min_l2 = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                                    min_mean = *(double*)get_element_a(means, (size_t[]){count});
                                    min_std = *(double*)get_element_a(stds, (size_t[]){count});
                                    min_residual_norm = *(double*)get_element_a(residual_norms, (size_t[]){count});
                                }
                            }
                        }
                    }
                } else {
                    if (*(double*)get_element_a(means, (size_t[]){count}) < min_mean) {
                        min_l1 = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                        min_l2 = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                        min_mean = *(double*)get_element_a(means, (size_t[]){count});
                        min_std = *(double*)get_element_a(stds, (size_t[]){count});
                        min_residual_norm = *(double*)get_element_a(residual_norms, (size_t[]){count});
                    } else if (*(double*)get_element_a(means, (size_t[]){count}) == min_mean) {
                        if (fabs(*(double*)get_element_a(stds, (size_t[]){count}) - min_std) > EPSILON_DATA_STORE &&
                                *(double*)get_element_a(stds, (size_t[]){count}) < min_std) {
                            min_l1 = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                            min_l2 = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                            min_mean = *(double*)get_element_a(means, (size_t[]){count});
                            min_std = *(double*)get_element_a(stds, (size_t[]){count});
                            min_residual_norm = *(double*)get_element_a(residual_norms, (size_t[]){count});
                        } else if (fabs(*(double*)get_element_a(stds, (size_t[]){count}) - min_std) <= EPSILON_DATA_STORE) {
                            if (fabs(*(double*)get_element_a(residual_norms, (size_t[]){count}) - min_residual_norm) > EPSILON_DATA_STORE &&
                                    *(double*)get_element_a(residual_norms, (size_t[]){count}) < min_residual_norm) {
                                min_l1 = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                                min_l2 = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                                min_mean = *(double*)get_element_a(means, (size_t[]){count});
                                min_std = *(double*)get_element_a(stds, (size_t[]){count});
                                min_residual_norm = *(double*)get_element_a(residual_norms, (size_t[]){count});
                            }
                        }
                    }
                }

                if (*(double*)get_element_a(linspace_h1, (size_t[]){i}) == 0 && *(double*)get_element_a(linspace_h2, (size_t[]){j}) == 1) {
                    min_l1_0_1 = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                    min_l2_0_1 = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                    min_mean_0_1 = *(double*)get_element_a(means, (size_t[]){count});
                    min_std_0_1 = *(double*)get_element_a(stds, (size_t[]){count});
                }
                if (*(double*)get_element_a(linspace_h1, (size_t[]){i}) == 1 && *(double*)get_element_a(linspace_h2, (size_t[]){j}) == 0) {
                    min_l1_1_0 = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                    min_l2_1_0 = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                    min_mean_1_0 = *(double*)get_element_a(means, (size_t[]){count});
                    min_std_1_0 = *(double*)get_element_a(stds, (size_t[]){count});
                }
                count += 1;
            }
        }

        //The data can now be added to a jarray and saved
        //jarray
        //   - hyperparameters
        //   - means
        //   - stds
        //   - min point
        //   - point 0 1
        //   - point 1 0

        double seed_double = (double)*(int*)data_store->seed;
        double order_double = (double)*(int*)data_store->order;

        //seed , order, snr, max_amp
        struct array* hyperparameters_array = new_set_a(
                                &(size_t){1},
                                (size_t[]){4}, 
                                &(enum DataType){DOUBLE}, 
                                (double[4]){
                                    seed_double, order_double, *data_store->snr, *data_store->max_amp
                                }
                              );
        struct array* min_point_array = new_set_a(
                                &(size_t){1},
                                (size_t[]){4}, 
                                &(enum DataType){DOUBLE}, 
                                (double[4]){
                                    min_l1, min_l2, min_mean, min_std
                                }
                              );
        
        struct array* point_0_1_array = new_set_a(
                                &(size_t){1},
                                (size_t[]){4}, 
                                &(enum DataType){DOUBLE}, 
                                (double[4]){
                                    min_l1_0_1, min_l2_0_1, min_mean_0_1, min_std_0_1
                                }
                              );

        struct array* point_1_0_array = new_set_a(
                                &(size_t){1},
                                (size_t[]){4}, 
                                &(enum DataType){DOUBLE}, 
                                (double[4]){
                                    min_l1_1_0, min_l2_1_0, min_mean_1_0, min_std_1_0
                                }
                              );

        struct Jaggedarray* jarray = new_ja(&(size_t){6});
        set_ja(jarray, hyperparameters_array, &(size_t){0});
        set_ja(jarray, means, &(size_t){1});
        set_ja(jarray, stds, &(size_t){2});
        set_ja(jarray, min_point_array, &(size_t){3});
        set_ja(jarray, point_0_1_array, &(size_t){4});
        set_ja(jarray, point_1_0_array, &(size_t){5});

        free_a(hyperparameters_array);
        free_a(point_1_0_array);
        free_a(point_0_1_array);
        free_a(min_point_array);
        free_a(linspace_h1);
        free_a(linspace_h2);
        free_a(means);
        free_a(stds);
        free_a(residual_norms);
        free_s(simulator);

        return jarray;
    }

    printf("DATA STORE NOT DONE!!!!!!!\n");

    return NULL;

}

/**
 * Extract Heatmap data from data store
*/
void extract_heatmap_data_ds(struct DataStore* data_store, char* save_directory, char* directory) {
    if (data_store == NULL) {
        log_null_value(__FILE__, __LINE__);
    }

    bool done = check_done_dcon(data_store->iterations_dcon);
    if (done) {
        struct array* means = zeros_a(&(size_t){1}, (size_t[]){*data_store->iterations_dcon->size}, &(enum DataType){DOUBLE});
        struct array* stds = zeros_a(&(size_t){1}, (size_t[]){*data_store->iterations_dcon->size}, &(enum DataType){DOUBLE});
        struct array* residual_norms = zeros_a(&(size_t){1}, (size_t[]){*data_store->gains_dcon->size}, &(enum DataType){DOUBLE});

        char filename_simulator [256];
        snprintf(filename_simulator, sizeof(filename_simulator), "%s/simulators/sim_seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf.sconfig",directory, *data_store->seed, *data_store->order, *data_store->snr, *data_store->max_amp);
        struct Simulator* simulator = read_from_file_s(filename_simulator);

        for (size_t i = 0; i < *data_store->iterations_dcon->size; i++) {
            char* filename = data_store->iterations_dcon->items[i];
            char filepath[512];
            sprintf(filepath, "%s/%s/%s.array", data_store->iterations_dcon->directory, data_store->iterations_dcon->identity, filename);
            struct array* array = read_from_file_a(filepath);
            double* mean = (double*)mean_a(array);
            double* std = (double*)std_a(array);

            set_element_a(means, &(size_t){i}, mean);
            set_element_a(stds, &(size_t){i}, std);

            free(std);
            free(mean);
            free_a(array);
        }

        struct array** M_t_s = malloc(*simulator->n_steps * sizeof(struct array*));
        struct array** D_t_s = malloc(*simulator->n_steps * sizeof(struct array*));
        for (size_t step = 0; step < *simulator->n_steps; step++) {
            struct array* M_t = slice_a(simulator->M, (int[]){-1,-1,step});
            struct array* D_t = slice_a(simulator->D, (int[]){-1,-1,step});
            M_t_s[step] = M_t;
            D_t_s[step] = D_t;
        } 

        for (size_t i = 0; i < *data_store->gains_dcon->size; i++) {
            //read the jarray for the pixel
            char* filename = data_store->gains_dcon->items[i];
            char filepath[512];
            sprintf(filepath, "%s/%s/%s.jarray", data_store->gains_dcon->directory, data_store->gains_dcon->identity, filename);
            struct Jaggedarray* jarray = read_from_file_ja(filepath);
            struct Jaggedarray* jarray_residuals = new_ja(jarray->size);

            for (size_t t = 0; t < *jarray->size; t++) {
                struct array* r = compute_r_N(M_t_s[t], D_t_s[t], jarray->jarray[t]);
                set_ja(jarray_residuals, r, &t);
                free_a(r);
            }

            double* residual_norm = (double*)norm_ja(jarray_residuals);
            set_element_a(residual_norms, &i, &(double){*residual_norm});
            
            free(residual_norm);
            free_ja(jarray_residuals);
            free_ja(jarray);            
        }

        for (size_t step = 0; step < *simulator->n_steps; step++) {
            free_a(M_t_s[step]);
            free_a(D_t_s[step]);
        } 
        free(M_t_s);
        free(D_t_s);

        save_to_file_a(means, save_directory, "iteration_means");
        save_to_file_a(stds, save_directory, "iteration_stds");
        save_to_file_a(residual_norms, save_directory, "residual_norms");

        free_a(means);
        free_a(stds);
        free_a(residual_norms);
        free_s(simulator);

    }
}

/**
 * Extract Heatmap data from data store
*/
void extract_heatmap_data_pso_ds(struct DataStore* data_store, char* save_directory) {
    if (data_store == NULL) {
        log_null_value(__FILE__, __LINE__);
    }

    bool done = check_done_dcon(data_store->iterations_dcon);
    if (done) {
        struct array* means = zeros_a(&(size_t){1}, (size_t[]){*data_store->iterations_dcon->size}, &(enum DataType){DOUBLE});
        struct array* stds = zeros_a(&(size_t){1}, (size_t[]){*data_store->iterations_dcon->size}, &(enum DataType){DOUBLE});
        struct array* residual_norms = zeros_a(&(size_t){1}, (size_t[]){*data_store->gains_dcon->size}, &(enum DataType){DOUBLE});

        char filename_simulator [256];
        snprintf(filename_simulator, sizeof(filename_simulator), "../data_order_%d/simulators/sim_seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf.sconfig",*data_store->order, *data_store->seed, *data_store->order, *data_store->snr, *data_store->max_amp);
        struct Simulator* simulator = read_from_file_s(filename_simulator);

        for (size_t i = 0; i < *data_store->iterations_dcon->size; i++) {
            char* filename = data_store->iterations_dcon->items[i];
            char filepath[512];
            sprintf(filepath, "%s/%s/%s.array", data_store->iterations_dcon->directory, data_store->iterations_dcon->identity, filename);
            struct array* array = read_from_file_a(filepath);
            double* mean = (double*)mean_a(array);
            double* std = (double*)std_a(array);

            set_element_a(means, &(size_t){i}, mean);
            set_element_a(stds, &(size_t){i}, std);

            free(std);
            free(mean);
            free_a(array);
        }

        struct array** M_t_s = malloc(*simulator->n_steps * sizeof(struct array*));
        struct array** D_t_s = malloc(*simulator->n_steps * sizeof(struct array*));
        for (size_t step = 0; step < *simulator->n_steps; step++) {
            struct array* M_t = slice_a(simulator->M, (int[]){-1,-1,step});
            struct array* D_t = slice_a(simulator->D, (int[]){-1,-1,step});
            M_t_s[step] = M_t;
            D_t_s[step] = D_t;
        } 

        for (size_t i = 0; i < *data_store->gains_dcon->size; i++) {
            //read the jarray for the pixel
            char* filename = data_store->gains_dcon->items[i];
            char filepath[512];
            sprintf(filepath, "%s/%s/%s.jarray", data_store->gains_dcon->directory, data_store->gains_dcon->identity, filename);
            struct Jaggedarray* jarray = read_from_file_ja(filepath);
            struct Jaggedarray* jarray_residuals = new_ja(jarray->size);

            for (size_t t = 0; t < *jarray->size; t++) {
                struct array* r = compute_r_N(M_t_s[t], D_t_s[t], jarray->jarray[t]);
                set_ja(jarray_residuals, r, &t);
                free_a(r);
            }

            double* residual_norm = (double*)norm_ja(jarray_residuals);
            set_element_a(residual_norms, &i, &(double){*residual_norm});
            
            free(residual_norm);
            free_ja(jarray_residuals);
            free_ja(jarray);            
        }

        for (size_t step = 0; step < *simulator->n_steps; step++) {
            free_a(M_t_s[step]);
            free_a(D_t_s[step]);
        } 
        free(M_t_s);
        free(D_t_s);

        save_to_file_a(means, save_directory, "iteration_means");
        save_to_file_a(stds, save_directory, "iteration_stds");
        save_to_file_a(residual_norms, save_directory, "residual_norms");

        free_a(means);
        free_a(stds);
        free_a(residual_norms);
        free_s(simulator);

    }
}

/**
 * Plot average iterations
*/
void map_average_iterations_std_ds(struct DataStore* data_store) {
    if (data_store == NULL) {
        log_null_value(__FILE__, __LINE__);
    }

    bool done = check_done_dcon(data_store->iterations_dcon);
    if (done) {
        struct array* means = zeros_a(&(size_t){1}, (size_t[]){*data_store->iterations_dcon->size}, &(enum DataType){DOUBLE});
        struct array* stds = zeros_a(&(size_t){1}, (size_t[]){*data_store->iterations_dcon->size}, &(enum DataType){DOUBLE});
        struct array* residual_norms = zeros_a(&(size_t){1}, (size_t[]){*data_store->gains_dcon->size}, &(enum DataType){DOUBLE});

        char filename_simulator [256];
        snprintf(filename_simulator, sizeof(filename_simulator), "./data/simulators/sim_seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf.sconfig", *data_store->seed, *data_store->order, *data_store->snr, *data_store->max_amp);
        struct Simulator* simulator = read_from_file_s(filename_simulator);

        for (size_t i = 0; i < *data_store->iterations_dcon->size; i++) {
            char* filename = data_store->iterations_dcon->items[i];
            char filepath[512];
            sprintf(filepath, "%s/%s/%s.array", data_store->iterations_dcon->directory, data_store->iterations_dcon->identity, filename);
            struct array* array = read_from_file_a(filepath);
            double* mean = (double*)mean_a(array);
            double* std = (double*)std_a(array);

            set_element_a(means, &(size_t){i}, mean);
            set_element_a(stds, &(size_t){i}, std);

            free(std);
            free(mean);
            free_a(array);
        }

        struct array** M_t_s = malloc(*simulator->n_steps * sizeof(struct array*));
        struct array** D_t_s = malloc(*simulator->n_steps * sizeof(struct array*));
        for (size_t step = 0; step < *simulator->n_steps; step++) {
            struct array* M_t = slice_a(simulator->M, (int[]){-1,-1,step});
            struct array* D_t = slice_a(simulator->D, (int[]){-1,-1,step});
            M_t_s[step] = M_t;
            D_t_s[step] = D_t;
        } 

        for (size_t i = 0; i < *data_store->gains_dcon->size; i++) {
            printf("t = %zu\n",i);
            //read the jarray for the pixel
            char* filename = data_store->gains_dcon->items[i];
            char filepath[512];
            sprintf(filepath, "%s/%s/%s.jarray", data_store->gains_dcon->directory, data_store->gains_dcon->identity, filename);
            struct Jaggedarray* jarray = read_from_file_ja(filepath);
            struct Jaggedarray* jarray_residuals = new_ja(jarray->size);

            for (size_t t = 0; t < *jarray->size; t++) {
                struct array* r = compute_r_N(M_t_s[t], D_t_s[t], jarray->jarray[t]);
                set_ja(jarray_residuals, r, &t);
                free_a(r);
            }

            double* residual_norm = (double*)norm_ja(jarray_residuals);
            set_element_a(residual_norms, &i, &(double){*residual_norm});
            
            free(residual_norm);
            free_ja(jarray_residuals);
            free_ja(jarray);            
        }

        for (size_t step = 0; step < *simulator->n_steps; step++) {
            free_a(M_t_s[step]);
            free_a(D_t_s[step]);
        } 
        free(M_t_s);
        free(D_t_s);

        char layout[100];
        switch (*data_store->layout) {
            case 0: {
                sprintf(layout, "HEX");
                break;
            }
            case 1: {
                sprintf(layout, "SQR");
                break;
            }
            case 2: {
                sprintf(layout, "REG");
                break;
            }
            default: {
                sprintf(layout, "UNKNOWN");
                break;
            }
        }

        char algorithm[100];
        char algorithm_title[100];
        switch (*data_store->algorithm_type) {
            case 0: {
                sprintf(algorithm, "GRADIENT_DESENT");
                sprintf(algorithm_title, "GRADIENT DESENT");
                break;
            }
            case 1: {
                sprintf(algorithm, "GRADIENT_DESENT_MOMENTUM");
                sprintf(algorithm_title, "GRADIENT DESENT MOMENTUM");
                break;
            }
            case 2: {
                sprintf(algorithm, "NESTROVS_ACCELERATED_GRADIENT");
                sprintf(algorithm_title, "NESTROVS ACCELERATED GRADIENT");
                break;
            }
            case 3: {
                sprintf(algorithm, "ADAGRAD");
                sprintf(algorithm_title, "ADAGRAD");
                break;
            }
            case 4: {
                sprintf(algorithm, "RMSPROP");
                sprintf(algorithm_title, "RMSPROP");
                break;
            }
            case 5: {
                sprintf(algorithm, "ADAM");
                sprintf(algorithm_title, "ADAM");
                break;
            }
            case 6: {
                sprintf(algorithm, "GAUSS_NEWTON");
                sprintf(algorithm_title, "GAUSS NEWTON");
                break;
            }
            case 7: {
                sprintf(algorithm, "LEVENBERG_MARQUARDT");
                sprintf(algorithm_title, "LEVENBERG MARQUARDT");
                break;
            }
            case 8: {
                sprintf(algorithm, "LEVENBERG_MARQUARDT_DIAGONAL_HESSIAN_LAMBDA");
                sprintf(algorithm_title, "LEVENBERG MARQUARDT DIAGONAL HESSIAN LAMBDA");
                break;
            }
            case 9: {
                sprintf(algorithm, "STEFCAL");
                sprintf(algorithm_title, "STEFCAL");
                break;
            }
            case 10: {
                sprintf(algorithm, "STEFCAL_LAMBDA");
                sprintf(algorithm_title, "STEFCAL LAMBDA");
                break;
            }
            case 11: {
                sprintf(algorithm, "STEFCAL_LM_LAMBDA");
                sprintf(algorithm_title, "STEFCAL LM LAMBDA");
                break;
            }
            default: {
                sprintf(algorithm, "UNKNOWN");
                sprintf(algorithm_title, "UNKNOWN");
                break;
            }
        }


        //writing the data to a txt file
        FILE *dataFile = fopen("heatmap_average_iterations_data.txt", "w");
        if (dataFile == NULL) {
            perror("Error opening data file");
            exit(EXIT_FAILURE);
        }
        FILE *dataFileResidualNorm = fopen("heatmap_residual_norm_data.txt", "w");
        if (dataFileResidualNorm == NULL) {
            perror("Error opening data file");
            exit(EXIT_FAILURE);
        }
        FILE *dataFileStd = fopen("heatmap_std_data.txt", "w");
        if (dataFileStd == NULL) {
            perror("Error opening data file");
            exit(EXIT_FAILURE);
        }

        char filenameAverageIterationsSave[512];
        sprintf(filenameAverageIterationsSave, "./data/heatmaps/average_iterations/%s_%s_order_%d_snr_%lf_maxamp_%lf_seed_%d_gridlength_%zu.txt", algorithm, layout, *data_store->order, *data_store->snr, *data_store->max_amp, *data_store->seed, *data_store->grid_length);
        char filenameStandardDevitionSave[512];
        sprintf(filenameStandardDevitionSave, "./data/heatmaps/standard_deviation/%s_%s_order_%d_snr_%lf_maxamp_%lf_seed_%d_gridlength_%zu.txt", algorithm, layout, *data_store->order, *data_store->snr, *data_store->max_amp, *data_store->seed, *data_store->grid_length);
        char filenameResidualNormSave[512];
        sprintf(filenameResidualNormSave, "./data/heatmaps/residual_norm/%s_%s_order_%d_snr_%lf_maxamp_%lf_seed_%d_gridlength_%zu.txt", algorithm, layout, *data_store->order, *data_store->snr, *data_store->max_amp, *data_store->seed, *data_store->grid_length);


        FILE *dataFileSave = fopen(filenameAverageIterationsSave, "w");
        if (dataFileSave == NULL) {
            perror("Error opening data file");
            exit(EXIT_FAILURE);
        }
        FILE *dataFileResidualNormSave = fopen(filenameResidualNormSave, "w");
        if (dataFileResidualNormSave == NULL) {
            perror("Error opening data file");
            exit(EXIT_FAILURE);
        }
        FILE *dataFileStdSave = fopen(filenameStandardDevitionSave, "w");
        if (dataFileStdSave == NULL) {
            perror("Error opening data file");
            exit(EXIT_FAILURE);
        }

        //writing the data to a txt file
        FILE *pointsFile = fopen("points_data.txt", "w");
        if (pointsFile == NULL) {
            perror("Error opening data file");
            exit(EXIT_FAILURE);
        }
        
        fprintf(pointsFile, "0 1\n");
        fprintf(pointsFile, "1 0\n");

        fclose(pointsFile);

        //writing the data to a txt file
        FILE *minpointsFile = fopen("min_points_data.txt", "w");
        if (minpointsFile == NULL) {
            perror("Error opening data file");
            exit(EXIT_FAILURE);
        }

        FILE *minpointsResidualNormFile = fopen("min_points_data_residual_norm.txt", "w");
        if (minpointsResidualNormFile == NULL) {
            perror("Error opening data file");
            exit(EXIT_FAILURE);
        }

        FILE *minpointStdFile = fopen("min_points_std_data.txt", "w");
        if (minpointStdFile == NULL) {
            perror("Error opening data file");
            exit(EXIT_FAILURE);
        }

        //changing the values to log scale for average iterations
        struct array* means_log10 = zeros_a(&(size_t){1}, (size_t[]){*data_store->iterations_dcon->size}, &(enum DataType){DOUBLE});
        double* arr_means_log10 = (double*)get_a(means_log10);
        double* arr_means = (double*)get_a(means);
        for (size_t h = 0; h < get_size_a(means_log10); h++) {
            arr_means_log10[h] = log10(arr_means[h]);
        }

        struct array* linspace_h1 = linspace_a(get_element_a(data_store->hyperparameters_bounds, (size_t[]){0}), get_element_a(data_store->hyperparameters_bounds, (size_t[]){1}), data_store->grid_length, &(enum DataType){DOUBLE});
        struct array* linspace_h2 = linspace_a(get_element_a(data_store->hyperparameters_bounds, (size_t[]){2}), get_element_a(data_store->hyperparameters_bounds, (size_t[]){3}), data_store->grid_length, &(enum DataType){DOUBLE});

        double* min_means_log10 = min_a(means_log10);

        double min_l1 = 0;
        double min_l2 = 0;
        double min_mean = 0;
        double min_std = 0;

        double min_l1_0_1 = 0;
        double min_l2_0_1 = 0;
        double min_mean_0_1 = 0;
        double min_std_0_1 = 0;

        double min_l1_1_0 = 0;
        double min_l2_1_0 = 0;
        double min_mean_1_0 = 0;
        double min_std_1_0 = 0;

        //This for loops adds the values that will make the heatmap
        size_t count = 0;
        for (size_t i = 0; i < *data_store->grid_length; i++) {
            for (size_t j = 0; j < *data_store->grid_length; j++) {
                if (*(double*)get_element_a(means_log10, (size_t[]){count}) == *min_means_log10) {
                    min_l1 = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                    min_l2 = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                    min_mean = *(double*)get_element_a(means, (size_t[]){count});
                    min_std = *(double*)get_element_a(stds, (size_t[]){count});

                    fprintf(minpointsFile, "%lf %lf %lf\n", *(double*)get_element_a(linspace_h1, (size_t[]){i}), *(double*)get_element_a(linspace_h2, (size_t[]){j}), *(double*)get_element_a(means_log10, (size_t[]){count}));
                }
                if (*(double*)get_element_a(linspace_h1, (size_t[]){i}) == 0 && *(double*)get_element_a(linspace_h2, (size_t[]){j}) == 1) {
                    min_l1_0_1 = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                    min_l2_0_1 = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                    min_mean_0_1 = *(double*)get_element_a(means, (size_t[]){count});
                    min_std_0_1 = *(double*)get_element_a(stds, (size_t[]){count});
                }
                if (*(double*)get_element_a(linspace_h1, (size_t[]){i}) == 1 && *(double*)get_element_a(linspace_h2, (size_t[]){j}) == 0) {
                    min_l1_1_0 = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                    min_l2_1_0 = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                    min_mean_1_0 = *(double*)get_element_a(means, (size_t[]){count});
                    min_std_1_0 = *(double*)get_element_a(stds, (size_t[]){count});
                }
                fprintf(dataFile, "%lf %lf %lf\n", *(double*)get_element_a(linspace_h1, (size_t[]){i}), *(double*)get_element_a(linspace_h2, (size_t[]){j}), *(double*)get_element_a(means_log10, (size_t[]){count}));
                fprintf(dataFileSave, "%lf %lf %lf\n", *(double*)get_element_a(linspace_h1, (size_t[]){i}), *(double*)get_element_a(linspace_h2, (size_t[]){j}), *(double*)get_element_a(means_log10, (size_t[]){count}));
                fprintf(dataFileStd, "%lf %lf %lf\n", *(double*)get_element_a(linspace_h1, (size_t[]){i}), *(double*)get_element_a(linspace_h2, (size_t[]){j}), *(double*)get_element_a(stds, (size_t[]){count}));
                fprintf(dataFileStdSave, "%lf %lf %lf\n", *(double*)get_element_a(linspace_h1, (size_t[]){i}), *(double*)get_element_a(linspace_h2, (size_t[]){j}), *(double*)get_element_a(stds, (size_t[]){count}));
                count += 1;
            }
            fprintf(dataFile, "\n");
            fprintf(dataFileStd, "\n");
        }

        struct array* residual_norm_log10 = zeros_a(&(size_t){1}, (size_t[]){*data_store->gains_dcon->size}, &(enum DataType){DOUBLE});
        double* arr_residual_norm_log10 = (double*)get_a(residual_norm_log10);
        double* arr_residual_norm = (double*)get_a(residual_norms);
        for (size_t h = 0; h < get_size_a(residual_norm_log10); h++) {
            arr_residual_norm_log10[h] = log10(arr_residual_norm[h]);
        }

        double* min_residual_norm_log10 = min_a(residual_norm_log10);


        double min_l1_res = 0;
        double min_l2_res = 0;
        double min_residual_norm = 0;

        double min_l1_0_1_res = 0;
        double min_l2_0_1_res = 0;
        double min_0_1_residual_norm = 0;

        double min_l1_1_0_res = 0;
        double min_l2_1_0_res = 0;
        double min_1_0_residual_norm = 0;

        size_t count_res = 0;
        for (size_t i = 0; i < *data_store->grid_length; i++) {
            for (size_t j = 0; j < *data_store->grid_length; j++) {
                if (*(double*)get_element_a(residual_norm_log10, (size_t[]){count_res}) == *min_residual_norm_log10) {
                    min_l1_res = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                    min_l2_res = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                    min_residual_norm = *(double*)get_element_a(residual_norms, (size_t[]){count_res});
                    fprintf(minpointsResidualNormFile, "%lf %lf %lf\n", *(double*)get_element_a(linspace_h1, (size_t[]){i}), *(double*)get_element_a(linspace_h2, (size_t[]){j}), *(double*)get_element_a(residual_norm_log10, (size_t[]){count_res}));
                }
                if (*(double*)get_element_a(linspace_h1, (size_t[]){i}) == 0 && *(double*)get_element_a(linspace_h2, (size_t[]){j}) == 1) {
                    min_l1_0_1_res = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                    min_l2_0_1_res = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                    min_0_1_residual_norm = *(double*)get_element_a(residual_norms, (size_t[]){count_res});
                }
                if (*(double*)get_element_a(linspace_h1, (size_t[]){i}) == 1 && *(double*)get_element_a(linspace_h2, (size_t[]){j}) == 0) {
                    min_l1_1_0_res = *(double*)get_element_a(linspace_h1, (size_t[]){i});
                    min_l2_1_0_res = *(double*)get_element_a(linspace_h2, (size_t[]){j});
                    min_1_0_residual_norm = *(double*)get_element_a(residual_norms, (size_t[]){count_res});
                }
                fprintf(dataFileResidualNorm, "%lf %lf %lf\n", *(double*)get_element_a(linspace_h1, (size_t[]){i}), *(double*)get_element_a(linspace_h2, (size_t[]){j}), *(double*)get_element_a(residual_norm_log10, (size_t[]){count_res}));
                fprintf(dataFileResidualNormSave, "%lf %lf %lf\n", *(double*)get_element_a(linspace_h1, (size_t[]){i}), *(double*)get_element_a(linspace_h2, (size_t[]){j}), *(double*)get_element_a(residual_norm_log10, (size_t[]){count_res}));
                count_res += 1;
            }
            fprintf(dataFileResidualNorm, "\n");
        }

        fclose(dataFile);
        fclose(dataFileResidualNorm);
        fclose(dataFileStd);
        fclose(minpointsFile);
        fclose(minpointStdFile);
        fclose(minpointsResidualNormFile);
        fclose(dataFileSave);
        fclose(dataFileResidualNormSave);
        fclose(dataFileStdSave);

        free_a(linspace_h1);
        free_a(linspace_h2);


        
        double* max_mean_log10 = max_a(means_log10);
        char filename[512];
        sprintf(filename, "./figures/average_iterations_heatmaps/%s_%s_order_%d_snr_%lf_maxamp_%lf_seed_%d_gridlength_%zu", algorithm, layout, *data_store->order, *data_store->snr, *data_store->max_amp, *data_store->seed, *data_store->grid_length);

        /*******************************************************************************************************************
         * Plotting average iterations heatmap
         * ****************************************************************************************************************/

        FILE *commandsFile = fopen("heatmap_average_iterations_commands.gplot", "w");
        if (commandsFile == NULL) {
            perror("Error opening commands file");
            exit(EXIT_FAILURE);
        }
        fprintf(commandsFile, "set terminal png\n");
        fprintf(commandsFile, "set output '%s.png'\n", filename);
        fprintf(commandsFile, "set title '%s with %s layout'\n", algorithm_title, layout);
        fprintf(commandsFile, "set xrange[%lf:%lf]\n", *(double*)get_element_a(data_store->hyperparameters_bounds, (size_t[]){0}), *(double*)get_element_a(data_store->hyperparameters_bounds, (size_t[]){1}));
        fprintf(commandsFile, "set yrange[%lf:%lf]\n", *(double*)get_element_a(data_store->hyperparameters_bounds, (size_t[]){3}), *(double*)get_element_a(data_store->hyperparameters_bounds, (size_t[]){2}));
        fprintf(commandsFile, "set xlabel 'λ1'\n");
        fprintf(commandsFile, "set ylabel 'λ2'\n");
        fprintf(commandsFile, "set view map\n");
        fprintf(commandsFile, "set palette defined (0 '#0000FF', 0.1 '#00BFFF', 0.2 '#00FFFF', 0.3 '#00FF00', 0.4 '#7FFF00', 0.5 '#FFFF00', 0.6 '#FFD700', 0.7 '#FF8C00', 0.8 '#FF4500', 0.9 '#FF0000', 1 '#8B0000')\n");
        fprintf(commandsFile, "set pm3d interpolate 0,0\n");
        fprintf(commandsFile, "set multiplot layout 2,1 columns\n");
        fprintf(commandsFile, "set size 1,0.9\n");
        fprintf(commandsFile, "set origin 0.0,0.1\n");
        fprintf(commandsFile, "splot 'heatmap_average_iterations_data.txt' with pm3d, 'points_data.txt' using 1:2:(%lf) with points linecolor rgb 'blue' pointtype 1 title 'Points', 'min_points_data.txt' using 1:2:3 with points linecolor rgb 'blue' pointtype 13 title 'Min Mean (%.2lf,%.2lf)'\n", *max_mean_log10, min_l1, min_l2);
        fprintf(commandsFile, "unset grid\n");
        fprintf(commandsFile, "unset border\n");
        fprintf(commandsFile, "unset xtics\n");
        fprintf(commandsFile, "unset ytics\n");
        fprintf(commandsFile, "unset xlabel\n");
        fprintf(commandsFile, "unset ylabel\n");
        fprintf(commandsFile, "unset title\n"); 
        fprintf(commandsFile, "set size 1,0.1\n");
        fprintf(commandsFile, "set label 20 at graph 0.02, graph 2.5 'snr = %.2lf    max amp = %.2lf    grid length = %zu    layout = %s'\n", *data_store->snr, *data_store->max_amp, *data_store->grid_length, layout);
        fprintf(commandsFile, "set label 21 at graph 0.02, graph 1 'order = %d    seed = %d    algorithm = %s'\n", *data_store->order, *data_store->seed, algorithm_title );
        fprintf(commandsFile, "set label 22 at graph 0.02, graph -0.5 '(%.2lf,%.2lf) = %.2lf    (%.2lf,%.2lf) = %.2lf    (%.2lf,%.2lf) = %.2lf'\n", min_l1, min_l2, min_mean, min_l1_0_1, min_l2_0_1, min_mean_0_1, min_l1_1_0, min_l2_1_0, min_mean_1_0 );
        fprintf(commandsFile, "plot NaN notitle\n");
        fprintf(commandsFile, "unset multiplot\n");
        
        fclose(commandsFile);

        int systemResult = system("gnuplot heatmap_average_iterations_commands.gplot");

        if (systemResult == -1) {
            perror("Error running Gnuplot");
            exit(EXIT_FAILURE);
        } else if (WIFEXITED(systemResult) && WEXITSTATUS(systemResult) != 0) {
            fprintf(stderr, "Gnuplot command failed with exit status %d\n", WEXITSTATUS(systemResult));
            exit(EXIT_FAILURE);
        }
        

        /*******************************************************************************************************************
         * Plotting std heatmap
         * ****************************************************************************************************************/
        char filenameStd[512];
        sprintf(filenameStd, "./figures/std_heatmaps/%s_%s_order_%d_snr_%lf_maxamp_%lf_seed_%d_gridlength_%zu", algorithm, layout, *data_store->order, *data_store->snr, *data_store->max_amp, *data_store->seed, *data_store->grid_length);
        double* max_std = max_a(stds);

        FILE *commandsFileStd = fopen("heatmap_std_commands.gplot", "w");
        if (commandsFileStd == NULL) {
            perror("Error opening commands file");
            exit(EXIT_FAILURE);
        }
        fprintf(commandsFileStd, "set terminal png\n");
        fprintf(commandsFileStd, "set output '%s.png'\n", filenameStd);
        fprintf(commandsFileStd, "set title '%s with %s layout'\n", algorithm_title, layout);
        fprintf(commandsFileStd, "set xrange[%lf:%lf]\n", *(double*)get_element_a(data_store->hyperparameters_bounds, (size_t[]){0}), *(double*)get_element_a(data_store->hyperparameters_bounds, (size_t[]){1}));
        fprintf(commandsFileStd, "set yrange[%lf:%lf]\n", *(double*)get_element_a(data_store->hyperparameters_bounds, (size_t[]){3}), *(double*)get_element_a(data_store->hyperparameters_bounds, (size_t[]){2}));
        fprintf(commandsFileStd, "set xlabel 'λ1'\n");
        fprintf(commandsFileStd, "set ylabel 'λ2'\n");
        fprintf(commandsFileStd, "set view map\n");
        fprintf(commandsFileStd, "set palette defined (0 '#0000FF', 0.1 '#00BFFF', 0.2 '#00FFFF', 0.3 '#00FF00', 0.4 '#7FFF00', 0.5 '#FFFF00', 0.6 '#FFD700', 0.7 '#FF8C00', 0.8 '#FF4500', 0.9 '#FF0000', 1 '#8B0000')\n"); 
        fprintf(commandsFileStd, "set pm3d interpolate 0,0\n");
        fprintf(commandsFileStd, "set multiplot layout 2,1 columns\n");
        fprintf(commandsFileStd, "set size 1,0.9\n");
        fprintf(commandsFileStd, "set origin 0.0,0.1\n");
        fprintf(commandsFileStd, "splot 'heatmap_std_data.txt' with pm3d, 'points_data.txt' using 1:2:(%lf) with points linecolor rgb 'blue' pointtype 1 title 'Points', 'min_points_data.txt' using 1:2:3 with points linecolor rgb 'blue' pointtype 13 title 'Min (%.2lf,%.2lf)'\n", *max_std, min_l1, min_l2);
        fprintf(commandsFileStd, "unset grid\n");
        fprintf(commandsFileStd, "unset border\n");
        fprintf(commandsFileStd, "unset xtics\n");
        fprintf(commandsFileStd, "unset ytics\n");
        fprintf(commandsFileStd, "unset xlabel\n");
        fprintf(commandsFileStd, "unset ylabel\n");
        fprintf(commandsFileStd, "unset title\n"); 
        fprintf(commandsFileStd, "set size 1,0.1\n");
        fprintf(commandsFileStd, "set label 20 at graph 0.02, graph 2.5 'snr = %.2lf    max amp = %.2lf    grid length = %zu    layout = %s'\n", *data_store->snr, *data_store->max_amp, *data_store->grid_length, layout);
        fprintf(commandsFileStd, "set label 21 at graph 0.02, graph 1 'order = %d    seed = %d    algorithm = %s'\n", *data_store->order, *data_store->seed, algorithm_title );
        fprintf(commandsFileStd, "set label 22 at graph 0.02, graph -0.5 '(%.2lf,%.2lf) = %.2lf    (%.2lf,%.2lf) = %.2lf    (%.2lf,%.2lf) = %.2lf'\n", min_l1, min_l2, min_std, min_l1_0_1, min_l2_0_1, min_std_0_1, min_l1_1_0, min_l2_1_0, min_std_1_0 );
        fprintf(commandsFileStd, "plot NaN notitle\n");
        fprintf(commandsFileStd, "unset multiplot\n");
        
        fclose(commandsFileStd);

        int systemResultStd = system("gnuplot heatmap_std_commands.gplot");

        if (systemResultStd == -1) {
            perror("Error running Gnuplot");
            exit(EXIT_FAILURE);
        } else if (WIFEXITED(systemResultStd) && WEXITSTATUS(systemResultStd) != 0) {
            fprintf(stderr, "Gnuplot command failed with exit status %d\n", WEXITSTATUS(systemResultStd));
            exit(EXIT_FAILURE);
        }

        /*******************************************************************************************************************
         * Plotting residual norm heatmap
         * ****************************************************************************************************************/
        char filenameResidualNorm[512];
        sprintf(filenameResidualNorm, "./figures/residual_norm_heatmaps/%s_%s_order_%d_snr_%lf_maxamp_%lf_seed_%d_gridlength_%zu", algorithm, layout, *data_store->order, *data_store->snr, *data_store->max_amp, *data_store->seed, *data_store->grid_length);
        double* max_residual_norm = max_a(residual_norm_log10);

        FILE *commandsFileResidualNorm = fopen("heatmap_residual_norm_commands.gplot", "w");
        if (commandsFileResidualNorm == NULL) {
            perror("Error opening commands file");
            exit(EXIT_FAILURE);
        }
        fprintf(commandsFileResidualNorm, "set terminal png\n");
        fprintf(commandsFileResidualNorm, "set output '%s.png'\n", filenameResidualNorm);
        fprintf(commandsFileResidualNorm, "set title '%s with %s layout'\n", algorithm_title, layout);
        fprintf(commandsFileResidualNorm, "set xrange[%lf:%lf]\n", *(double*)get_element_a(data_store->hyperparameters_bounds, (size_t[]){0}), *(double*)get_element_a(data_store->hyperparameters_bounds, (size_t[]){1}));
        fprintf(commandsFileResidualNorm, "set yrange[%lf:%lf]\n", *(double*)get_element_a(data_store->hyperparameters_bounds, (size_t[]){3}), *(double*)get_element_a(data_store->hyperparameters_bounds, (size_t[]){2}));
        fprintf(commandsFileResidualNorm, "set xlabel 'λ1'\n");
        fprintf(commandsFileResidualNorm, "set ylabel 'λ2'\n");
        fprintf(commandsFileResidualNorm, "set view map\n");
        fprintf(commandsFileResidualNorm, "set palette defined (0 '#0000FF', 0.1 '#00BFFF', 0.2 '#00FFFF', 0.3 '#00FF00', 0.4 '#7FFF00', 0.5 '#FFFF00', 0.6 '#FFD700', 0.7 '#FF8C00', 0.8 '#FF4500', 0.9 '#FF0000', 1 '#8B0000')\n"); 
        fprintf(commandsFileResidualNorm, "set pm3d interpolate 0,0\n");
        fprintf(commandsFileResidualNorm, "set multiplot layout 2,1 columns\n");
        fprintf(commandsFileResidualNorm, "set size 1,0.9\n");
        fprintf(commandsFileResidualNorm, "set origin 0.0,0.1\n");
        fprintf(commandsFileResidualNorm, "splot 'heatmap_residual_norm_data.txt' with pm3d, 'points_data.txt' using 1:2:(%lf) with points linecolor rgb 'blue' pointtype 1 title 'Points', 'min_points_data_residual_norm.txt' using 1:2:3 with points linecolor rgb 'blue' pointtype 13 title 'Min (%.2lf,%.2lf)'\n", *max_residual_norm, min_l1_res, min_l2_res);
        fprintf(commandsFileResidualNorm, "unset grid\n");
        fprintf(commandsFileResidualNorm, "unset border\n");
        fprintf(commandsFileResidualNorm, "unset xtics\n");
        fprintf(commandsFileResidualNorm, "unset ytics\n");
        fprintf(commandsFileResidualNorm, "unset xlabel\n");
        fprintf(commandsFileResidualNorm, "unset ylabel\n");
        fprintf(commandsFileResidualNorm, "unset title\n"); 
        fprintf(commandsFileResidualNorm, "set size 1,0.1\n");
        fprintf(commandsFileResidualNorm, "set label 20 at graph 0.02, graph 2.5 'snr = %.2lf    max amp = %.2lf    grid length = %zu    layout = %s'\n", *data_store->snr, *data_store->max_amp, *data_store->grid_length, layout);
        fprintf(commandsFileResidualNorm, "set label 21 at graph 0.02, graph 1 'order = %d    seed = %d    algorithm = %s'\n", *data_store->order, *data_store->seed, algorithm_title );
        fprintf(commandsFileResidualNorm, "set label 22 at graph 0.02, graph -0.5 '(%.2lf,%.2lf) = %.2lf    (%.2lf,%.2lf) = %.2lf    (%.2lf,%.2lf) = %.2lf'\n", min_l1_res, min_l2_res, min_residual_norm, min_l1_0_1_res, min_l2_0_1_res, min_0_1_residual_norm, min_l1_1_0_res, min_l2_1_0_res, min_1_0_residual_norm );
        fprintf(commandsFileResidualNorm, "plot NaN notitle\n");
        fprintf(commandsFileResidualNorm, "unset multiplot\n");

        fclose(commandsFileResidualNorm);

        int systemResultResidualNorm = system("gnuplot heatmap_residual_norm_commands.gplot");

        if (systemResultResidualNorm == -1) {
            perror("Error running Gnuplot");
            exit(EXIT_FAILURE);
        } else if (WIFEXITED(systemResultResidualNorm) && WEXITSTATUS(systemResultResidualNorm) != 0) {
            fprintf(stderr, "Gnuplot command failed with exit status %d\n", WEXITSTATUS(systemResultResidualNorm));
            exit(EXIT_FAILURE);
        }


        remove("points_data.txt");
        remove("min_points_data.txt");
        remove("min_points_data_residual_norm.txt");
        remove("min_points_std_data.txt");
        remove("heatmap_average_iterations_data.txt");
        remove("heatmap_std_data.txt");
        remove("heatmap_average_iterations_commands.gplot");
        remove("heatmap_residual_norm_data.txt");
        remove("heatmap_std_commands.gplot");

        free(max_std);
        free(max_mean_log10);
        free_a(means_log10);
        free_a(means);
        free_a(stds);
        free_a(residual_norms);
        free(min_means_log10);
        free(min_residual_norm_log10);
        free_a(residual_norm_log10);
        free_s(simulator);
        free(max_residual_norm);
    }
}

/**
 * check if the data_store is done running
*/
bool check_done_ds(struct DataStore* data_store) {
    return check_done_dcon(data_store->iterations_dcon);
}

/**
 * Get the amount done in the data store
*/
int get_progress_ds(struct DataStore* data_store) {
    return get_progress_dcon(data_store->iterations_dcon);
}

/**
 * Get persesntage done
*/
double get_persentage_done_ds(struct DataStore* data_store) {
    return get_persentage_done_dcon(data_store->iterations_dcon);
}

/**
 * Compare parameters
 * returns true if everything is the same.
*/
bool compare_parameters_ds(struct DataStore* data_store, double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed, size_t* grid_length, enum AlgorithmType* algorithm_type, size_t* amount_hyperparameters, struct array* hyperparameters_bounds) {
    if (data_store == NULL) {
        return false;
    }
    
    if (!compare_doubles(data_store->h_min, h_min)) {
        return false;
    }
    if (!compare_doubles(data_store->h_max, h_max)) {
        return false;
    }
    if (!compare_doubles(data_store->dec, dec)) {
        return false;
    }
    if (!compare_doubles(data_store->lat, lat)) {
        return false;
    }
    if (!compare_doubles(data_store->freq, freq)) {
        return false;
    }
    if (*data_store->layout != *layout) {
        return false;
    }
    if (*data_store->n_steps != *n_steps) {
        return false;
    }
    if (!compare_doubles(data_store->bas_len, bas_len)) {
        return false;
    }
    if (*data_store->order != *order) {
        return false;
    }
    if (!compare_doubles(data_store->snr, snr)) {
        return false;
    }
    if (!compare_doubles(data_store->max_amp, max_amp)) {
        return false;
    }
    if (*data_store->seed != *seed) {
        return false;
    }
    if (*data_store->grid_length != *grid_length) {
        return false;
    }
    if (*data_store->algorithm_type != *algorithm_type) {
        return false;
    }
    if (*data_store->amount_hyperparameters != *amount_hyperparameters) {
        return false;
    }
    if (*data_store->h_max != *h_max) {
        return false;
    }
    //check if the hyperparameter bounds are the same
    if (!equal_a(data_store->hyperparameters_bounds, hyperparameters_bounds)) {
        return false;
    }
    
    return true;
}
