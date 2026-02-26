/**
  * @file data_store.h
  * @description A definition of functions offered by the data_store
  */

#include "array.h"
#include "utils.h"
#include <stdbool.h>
#include "data_container.h"
#include "calibrator.h"

#ifndef _DATA_STORE_H
#define _DATA_STORE_H

/**
 * DataStore struct
*/
struct DataStore {
    char* identity;
    char* directory;
    double* h_min;
    double* h_max;
    double* dec;
    double* lat;
    double* freq;
    enum LayoutType* layout;
    size_t* n_steps;
    double* bas_len;
    int* order;
    double* snr;
    double* max_amp;
    int* seed;
    int* num_workers;
    size_t* grid_length;
    enum AlgorithmType* algorithm_type;
    size_t* amount_hyperparameters;
    struct array* hyperparameters_bounds;
    struct DataContainer* iterations_dcon;
    struct DataContainer* gains_dcon;
    struct DataContainer* all_gains_dcon;
    struct DataContainer* damping_g_dcon;
    struct DataContainer* damping_g_old_dcon;
};

/**
 * Create a DataStore struct
*/
struct DataStore* new_ds(char* identity, char* directory, double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed, int* num_workers, size_t* grid_length, enum AlgorithmType* algorithm_type, size_t* amount_hyperparameters, struct array* hyperparameters_bounds);

/**
 * Free DataStore struct
*/
void free_ds(struct DataStore* data_store);

/**
 * This will print the data dictionary
*/
void print_ds(struct DataStore* data_store);


/**
 * This function will start a storing data with specific starting parameters
*/
size_t start_ds(struct DataStore* data_store);

/**
 * save DataStore to file
*/
void save_to_file_ds(struct DataStore* data_store);

/**
 * Read .array file
*/
struct DataStore* read_from_file_ds(char* directory, char* identity);

/**
 * Create a DataStore struct
*/
struct DataStore* new_save_ds(char* identity, char* directory, double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed, int* num_workers, size_t* grid_length, enum AlgorithmType* algorithm_type, size_t* amount_hyperparameters, struct array* hyperparameters_bounds);

/**
 * This fuction will set up the directory structure for a data store
*/
struct DataStore* new_structure_ds(char* identity, char* directory, double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed, int* num_workers, size_t* grid_length, enum AlgorithmType* algorithm_type, size_t* amount_hyperparameters, struct array* hyperparameters_bounds);

/**
 * Read structure 
*/
struct DataStore* read_structure_ds(char* directory, char* identity);

/**
 * Saving the data to structure
*/
void save_data_to_structure_ds(struct DataStore* data_store, struct Calibrator* calibrator, size_t* index, char* filename);

/**
 * This function will extract the information from a datastore
 */
struct Jaggedarray* extract_info_ds(struct DataStore* data_store, char* directory);

/**
 * Extract Heatmap data from data store
*/
void extract_heatmap_data_ds(struct DataStore* data_store, char* save_directory, char* directory);

/**
 * Extract Heatmap data from data store
*/
void extract_heatmap_data_pso_ds(struct DataStore* data_store, char* save_directory);

/**
 * Plot average iterations
*/
void map_average_iterations_std_ds(struct DataStore* data_store);

/**
 * check if the data_store is done running
*/
bool check_done_ds(struct DataStore* data_store);

/**
 * Get the amount done in the data store
*/
int get_progress_ds(struct DataStore* data_store);

/**
 * Get persesntage done
*/
double get_persentage_done_ds(struct DataStore* data_store);

/**
 * Compare parameters
*/
bool compare_parameters_ds(struct DataStore* data_store, double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed, size_t* grid_length, enum AlgorithmType* algorithm_type, size_t* amount_hyperparameters, struct array* hyperparameters_bounds);

#endif