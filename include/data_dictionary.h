/**
  * @file data_dictionary.h
  * @description A definition of functions offered by the data_dictionary
  */

#include "array.h"
#include "utils.h"
#include <pthread.h>
#include <stdbool.h>
#include "data_store.h"

#ifndef _DATA_DICTIONARY_H
#define _DATA_DICTIONARY_H

/**
 * DataWarehouse struct
*/
struct DataDictionary {
    char* identity;
    char* directory;
    size_t* amount_items;
    size_t* total_size;
    char** items;
    pthread_mutex_t* mutex;
};

/**
 * Create a new DataDictionary
*/
struct DataDictionary* new_dd(char* identity, char* directory);

/**
 * Create a new DataDictionary
*/
struct DataDictionary* create_dd(char* identity, char* directory, size_t* total_size);

/**
 * Free DataDictionary
*/
void free_dd(struct DataDictionary* data_dictionary);

/**
 * This will print the data dictionary
*/
void print_dd(struct DataDictionary* data_dictionary);
/**
 * This fuction will set the value of a specific index
*/
void set_item_dd(struct DataDictionary* data_dictionary, char* item, size_t* index);

/**
 * This function will add an item to the end and increase the arra if needed.
*/
void add_item_dd(struct DataDictionary* data_dictionary, char* item);

/**
 * This function will increase the item and path array when it is full
*/
void increase_items_dd(struct DataDictionary* data_dictionary);

/**
 * This function checks if an item is in the datadictionary
*/
bool contains_item_dd(struct DataDictionary* data_dictionary, char* item);

/**
 * save datadictionary to file
*/
void save_to_file_dd(struct DataDictionary* data_dictionary);

/**
 * Read .array file
*/
struct DataDictionary* read_from_file_dd(char* directory, char* filename);

/**
 * Update DataDictionary file
*/
struct DataDictionary* update_file_dd(struct DataDictionary* data_dictionary);

/**
 * This function will lock the datadictionary when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct DataDictionary pointer
*/
void lock_dd(struct DataDictionary* data_dictionary);

/**
 * This function will unlock the datadictionary when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct DataDictionary pointer
*/
void unlock_dd(struct DataDictionary* data_dictionary);

/**
 * This function will create a new data store with your parameters and return the struct pointer
*/
struct DataStore* new_data_store_dd(struct DataDictionary* data_dictionary, double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed, int* num_workers, size_t* grid_length, enum AlgorithmType* algorithm_type, size_t* amount_hyperparameters, struct array* hyperparameters_bounds);

/**
 * This function will create a new data store with your parameters and return the struct pointer
*/
struct DataStore* new_data_store_update_dd(struct DataDictionary** data_dictionary, double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed, int* num_workers, size_t* grid_length, enum AlgorithmType* algorithm_type, size_t* amount_hyperparameters, struct array* hyperparameters_bounds);

/**
 * This function will create a new data container that contains arrays and return its pointer. Makes sure that the container has a unique name
 */
struct DataContainer* new_data_container_type_1_dd(struct DataDictionary* data_dictionary, char* identity, size_t* size);

/**
 * This function will create a new data container that contains arrays and stores and return its pointer.
 */
struct DataContainer* new_data_container_type_1_update_dd(struct DataDictionary** data_dictionary, char* identity, size_t* size);

/**
 * This function will create a new data container that contains jarrays and return its pointer. Makes sure that the container has a unique name
 */
struct DataContainer* new_data_container_type_2_dd(struct DataDictionary* data_dictionary, char* identity, size_t* size);

/**
 * This function will create a new data container that contains jarrays and stores and return its pointer.
 */
struct DataContainer* new_data_container_type_2_update_dd(struct DataDictionary** data_dictionary, char* identity, size_t* size);

/**
 * This function will get the data store already created and return it.
*/
struct DataStore* read_data_store_dd(char* directory, char* identity);

/**
 * Create a new DataDictionary and saves the datadictionary
*/
struct DataDictionary* new_save_dd(char* identity, char* directory);

/**
 * This function adds and updates a datadictionary
*/
void add_item_update_dd(struct DataDictionary** data_dictionary, char* item);

/**
 * This function will start running the configurations in the data_store
*/
void start_data_store_dd(struct DataStore* data_store); 

/**
 * Plot average iterations
*/
void map_average_iterations_std_dd(struct DataStore* data_store);

#endif