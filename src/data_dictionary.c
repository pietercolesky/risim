/**
  * @file data_dictionary.c
  * This is the implementation of the functions of data_dictionary.h
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <uuid/uuid.h>
#include "data_dictionary.h"
#include "logger.h"
#include <string.h>
#include "data_store.h"
#include "constants.h"



/**
 * Create a new DataWarehouse
*/
struct DataDictionary* new_dd(char* identity, char* directory) {
    //check if the directory exists
    bool check_directory = check_file_path(directory);
    if (!check_directory) {
        log_invalid_directory(__FILE__, __LINE__);
    }

    char filepath[512];
    sprintf(filepath, "%s/%s.ddconfig", directory, identity);
    int check = check_file_path(filepath);
    if (check) {
        log_data_dictionary_already_exists(__FILE__, __LINE__);
    }

    struct DataDictionary* data_dictionary = malloc(sizeof(struct DataDictionary));
    if (!data_dictionary) {
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }

    data_dictionary->identity = malloc(MAX_STRING_LENGTH* sizeof(char));
    if (!data_dictionary->identity) {
        free_dd(data_dictionary);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    strcpy(data_dictionary->identity, identity);

    data_dictionary->directory = malloc(MAX_STRING_LENGTH* sizeof(char));
    if (!data_dictionary->directory) {
        free_dd(data_dictionary);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    strcpy(data_dictionary->directory, directory);

    data_dictionary->amount_items = malloc(sizeof(size_t));
    if (!data_dictionary->amount_items) {
        free_dd(data_dictionary);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_dictionary->amount_items = 0;

    data_dictionary->total_size = malloc(sizeof(size_t));
    if (!data_dictionary->total_size) {
        free(data_dictionary->amount_items);
        free(data_dictionary);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_dictionary->total_size = INIT_SIZE;

    data_dictionary->items = malloc(INIT_SIZE * sizeof(char*));
    if (!data_dictionary->items) {
        free(data_dictionary->amount_items);
        free(data_dictionary->total_size);
        free(data_dictionary);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }

    for (size_t i = 0; i < *data_dictionary->total_size; ++i) {
        data_dictionary->items[i] = strdup("");
        if (!data_dictionary->items[i]) {
            log_mem_aloc_fail(__FILE__, __LINE__);
            for (size_t j = 0; j < i; ++j) {
                free(data_dictionary->items[j]);
            }
            free(data_dictionary->items);
            free(data_dictionary->amount_items);
            free(data_dictionary->total_size);
            free(data_dictionary);
            return NULL;
        }
    }

    data_dictionary->mutex = malloc(sizeof(pthread_mutex_t));
    if (!data_dictionary->mutex) {
        // Free allocated memory and return NULL
        for (size_t i = 0; i < INIT_SIZE; ++i) {
            free(data_dictionary->items[i]);
        }
        free(data_dictionary->items);
        free(data_dictionary->amount_items);
        free(data_dictionary->total_size);
        free(data_dictionary);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    pthread_mutex_init(data_dictionary->mutex, NULL);

    return data_dictionary;
}

/**
 * Create a new DataDictionary
*/
struct DataDictionary* create_dd(char* identity, char* directory, size_t* total_size) {
    struct DataDictionary* data_dictionary = malloc(sizeof(struct DataDictionary));
    if (!data_dictionary) {
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }

    data_dictionary->identity = malloc(MAX_STRING_LENGTH* sizeof(char));
    if (!data_dictionary->identity) {
        free_dd(data_dictionary);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    strcpy(data_dictionary->identity, identity);

    data_dictionary->directory = malloc(MAX_STRING_LENGTH* sizeof(char));
    if (!data_dictionary->directory) {
        free_dd(data_dictionary);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    strcpy(data_dictionary->directory, directory);

    data_dictionary->amount_items = malloc(sizeof(size_t));
    if (!data_dictionary->amount_items) {
        free(data_dictionary);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_dictionary->amount_items = 0;

    data_dictionary->total_size = malloc(sizeof(size_t));
    if (!data_dictionary->total_size) {
        free(data_dictionary->amount_items);
        free(data_dictionary);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    *data_dictionary->total_size = *total_size;

    data_dictionary->items = malloc(*total_size * sizeof(char*));
    if (!data_dictionary->items) {
        free(data_dictionary->amount_items);
        free(data_dictionary->total_size);
        free(data_dictionary);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }


    for (size_t i = 0; i < *data_dictionary->total_size; ++i) {
        data_dictionary->items[i] = strdup("");
        if (!data_dictionary->items[i]) {
            log_mem_aloc_fail(__FILE__, __LINE__);
            for (size_t j = 0; j < i; ++j) {
                free(data_dictionary->items[j]);
            }
            free(data_dictionary->items);
            free(data_dictionary->amount_items);
            free(data_dictionary->total_size);
            free(data_dictionary);
            return NULL;
        }
    }

    data_dictionary->mutex = malloc(sizeof(pthread_mutex_t));
    if (!data_dictionary->mutex) {
        // Free allocated memory and return NULL
        for (size_t i = 0; i < INIT_SIZE; ++i) {
            free(data_dictionary->items[i]);
        }
        free(data_dictionary->items);
        free(data_dictionary->amount_items);
        free(data_dictionary->total_size);
        free(data_dictionary);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }
    pthread_mutex_init(data_dictionary->mutex, NULL);


    return data_dictionary;
}

/**
 * Free DataDictionary
 */
void free_dd(struct DataDictionary* data_dictionary) {
    if (data_dictionary == NULL) {
        return;
    }

    if (data_dictionary->identity != NULL) {
        free(data_dictionary->identity);
    }

    if (data_dictionary->directory != NULL) {
        free(data_dictionary->directory);
    }

    if (data_dictionary->items != NULL) {
        for (size_t i = 0; i < *data_dictionary->total_size; ++i) {
            if (data_dictionary->items[i] != NULL) {
              free(data_dictionary->items[i]);
            }
        }
        free(data_dictionary->items);
    }

    if (data_dictionary->mutex != NULL) {
        pthread_mutex_destroy(data_dictionary->mutex);
        free(data_dictionary->mutex);
    }

    if (data_dictionary->amount_items != NULL) {
      free(data_dictionary->amount_items);
    }

    if (data_dictionary->total_size != NULL) {
      free(data_dictionary->total_size);
    }

    free(data_dictionary);
}

/**
 * This will print the data dictionary
*/
void print_dd(struct DataDictionary* data_dictionary) {
    if (data_dictionary == NULL) {
      return;
    }
    printf("{\n");
    printf("  Identity: %s\n",data_dictionary->identity);
    printf("  Directory: %s\n",data_dictionary->directory);
    printf("  Amount of Items: %zu\n",*data_dictionary->amount_items);
    printf("  Total Space: %zu\n",*data_dictionary->total_size);

    if (*data_dictionary->amount_items == 0) {
      printf("\n");
      printf("  Currently no items in Data Dictionary.\n");
      printf("}\n");
    } else {
      printf("\n");
      for (size_t i = 0; i < *data_dictionary->amount_items; i++) {
        printf("  Item: %s\n", data_dictionary->items[i]);
      }
      printf("}\n");
    }
}

/**
 * This fuction will set the value of a specific index
*/
void set_item_dd(struct DataDictionary* data_dictionary, char* item, size_t* index) {
    if (data_dictionary == NULL || item == NULL || index == NULL) {
        log_null_value(__FILE__, __LINE__);
    }

    if (*index >= *data_dictionary->total_size) {
        log_index_out_of_bounds(__FILE__, __LINE__);
    }

    size_t item_len = strlen(item);

    if (item_len >= MAX_STRING_LENGTH) {
        log_string_too_long(__FILE__,__LINE__);
    }

    if (data_dictionary->items[*index] != NULL) {
        free(data_dictionary->items[*index]);
    }

    data_dictionary->items[*index] = malloc((item_len + 1) * sizeof(char));
    if (data_dictionary->items[*index] == NULL) {
        log_mem_aloc_fail(__FILE__,__LINE__);
        return;
    }

    strcpy(data_dictionary->items[*index], item);
}

/**
 * This function will add an item to the end and increase the arra if needed.
*/
void add_item_dd(struct DataDictionary* data_dictionary, char* item) {
    if (*data_dictionary->amount_items >= *data_dictionary->total_size) {
        increase_items_dd(data_dictionary);
    }
    set_item_dd(data_dictionary, item, data_dictionary->amount_items);

    (*data_dictionary->amount_items)++;
}

/**
 * This function will increase the item and path array when it is full
*/
void increase_items_dd(struct DataDictionary* data_dictionary) {
    size_t new_size = (size_t)(*data_dictionary->total_size * 1.5);
    char** new_items = realloc(data_dictionary->items, new_size * sizeof(char*));

    if (new_items == NULL) {
        log_mem_aloc_fail(__FILE__, __LINE__);
        return;
    }

    data_dictionary->items = new_items;

    for (size_t i = *data_dictionary->total_size; i < new_size; ++i) {
        new_items[i] = strdup("");
        if (new_items[i] == NULL) {
            for (size_t j = *data_dictionary->total_size; j < i; ++j) {
                free(new_items[j]);
            }
            free(new_items);
            log_mem_aloc_fail(__FILE__, __LINE__);
            return;
        }
    }

    data_dictionary->items = new_items;
    *data_dictionary->total_size = new_size;
}

/**
 * This function checks if an item is in the datadictionary
*/
bool contains_item_dd(struct DataDictionary* data_dictionary, char* item) {
    if (!data_dictionary || !item) {
        log_null_value(__FILE__, __LINE__);
    }

    for (size_t i = 0; i < *data_dictionary->amount_items; i++) {
        if (strcmp(item, data_dictionary->items[i]) == 0) {
            return true;
        }
    }

    return false;
}

/**
 * save datadictionary to file
*/
void save_to_file_dd(struct DataDictionary* data_dictionary) {
    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s/%s.ddconfig",data_dictionary->directory,data_dictionary->identity);

    FILE *combinedDataFile = fopen(full_path, "w");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return;
    }

    fprintf(combinedDataFile, "%s\n", data_dictionary->identity);
    fprintf(combinedDataFile, "%zu\n", *data_dictionary->amount_items);
    fprintf(combinedDataFile, "%zu\n", *data_dictionary->total_size);

    for (size_t i = 0; i < *data_dictionary->amount_items; i++) {
      fprintf(combinedDataFile, "%s\n", data_dictionary->items[i]);
    }

    fclose(combinedDataFile);
}

/**
 * Read .array file
*/
struct DataDictionary* read_from_file_dd(char* directory, char* filename) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s.ddconfig", directory, filename);

    if(!ends_with_string(filepath, ".ddconfig")) {
        log_invalid_file_extension(__FILE__,__LINE__);
    }

    FILE *combinedDataFile = fopen(filepath, "r");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return NULL;
    }

    char identity[256];
    size_t amount_items = 0;
    size_t total_size = 0;

    if (fscanf(combinedDataFile, "%255s\n", identity) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }

    if (fscanf(combinedDataFile, "%zu", &amount_items) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }

    if (fscanf(combinedDataFile, "%zu", &total_size) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }

    // Consume the newline character
    int c;
    while ((c = fgetc(combinedDataFile)) != EOF && c != '\n');

    struct DataDictionary* read_data_dictionary = create_dd(identity, directory,&(size_t){total_size});

    for (size_t i = 0; i < amount_items; i++) {
        //add items to the list
        char item[256];

        if (fscanf(combinedDataFile, "%255s\n", item) != 1) {
            log_error_fscan(__FILE__, __LINE__);
            free_dd(read_data_dictionary);
            fclose(combinedDataFile);
            return NULL;
        }

        add_item_dd(read_data_dictionary, item);

        if (read_data_dictionary->items[i] == NULL) {
            log_mem_aloc_fail(__FILE__, __LINE__);
            free_dd(read_data_dictionary); // Free allocated memory
            fclose(combinedDataFile);
            return NULL;
        }
    }

    fclose(combinedDataFile);

    return read_data_dictionary;
}

/**
 * Update DataDictionary file
*/
struct DataDictionary* update_file_dd(struct DataDictionary* data_dictionary) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/%s.ddconfig",data_dictionary->directory,data_dictionary->identity);

    if(!ends_with_string(filepath, ".ddconfig")) {
        log_invalid_file_extension(__FILE__,__LINE__);
    }

    FILE *combinedDataFile = fopen(filepath, "r+");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return NULL;
    }

    char identity[256];
    size_t amount_items = 0;
    size_t total_size = 0;

    if (fscanf(combinedDataFile, "%255s\n", identity) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }

    if (fscanf(combinedDataFile, "%zu", &amount_items) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }

    if (fscanf(combinedDataFile, "%zu", &total_size) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }

    // Consume the newline character
    int c;
    while ((c = fgetc(combinedDataFile)) != EOF && c != '\n');

    struct DataDictionary* read_data_dictionary = create_dd(identity, data_dictionary->directory,&(size_t){total_size});

    for (size_t i = 0; i < amount_items; i++) {
        //add items to the list
        char item[256];

        if (fscanf(combinedDataFile, "%255s\n", item) != 1) {
            log_error_fscan(__FILE__, __LINE__);
            free_dd(read_data_dictionary);
            fclose(combinedDataFile);
            return NULL;
        }

        add_item_dd(read_data_dictionary, item);

        if (read_data_dictionary->items[i] == NULL) {
            log_mem_aloc_fail(__FILE__, __LINE__);
            free_dd(read_data_dictionary); // Free allocated memory
            fclose(combinedDataFile);
            return NULL;
        }
    }
    //check what is not in read_data_dictionary but is in data_dictionary and add it to read_data_dictionary

    for (size_t i = 0; i < *data_dictionary->amount_items; i++) {
        bool check = contains_item_dd(read_data_dictionary, data_dictionary->items[i]);
        if (!check) {
            //add item to read_data_dictionary
            add_item_dd(read_data_dictionary, data_dictionary->items[i]);
        }
    }

    // //overwrite the saved file with read_datadictionary.
    if (ftruncate(fileno(combinedDataFile), 0) == -1) {
        fclose(combinedDataFile);
        free_dd(read_data_dictionary);
        log_error_truncating_file(__FILE__, __LINE__);
        return NULL;
    }

    rewind(combinedDataFile);

    fprintf(combinedDataFile, "%s\n", read_data_dictionary->identity);
    fprintf(combinedDataFile, "%zu\n", *read_data_dictionary->amount_items);
    fprintf(combinedDataFile, "%zu\n", *read_data_dictionary->total_size);

    for (size_t i = 0; i < *read_data_dictionary->amount_items; i++) {
      fprintf(combinedDataFile, "%s\n", read_data_dictionary->items[i]);
    }

    fclose(combinedDataFile);

    return read_data_dictionary;
}


/**
 * This function will lock the datadictionary when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct DataDictionary pointer
*/
void lock_dd(struct DataDictionary* data_dictionary) {
    pthread_mutex_lock(data_dictionary->mutex);
}

/**
 * This function will unlock the datadictionary when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct DataDictionary pointer
*/
void unlock_dd(struct DataDictionary* data_dictionary) {
    pthread_mutex_unlock(data_dictionary->mutex);
}

/**
 * This function will create a new data store with your parameters and return the struct pointer
*/
struct DataStore* new_data_store_dd(struct DataDictionary* data_dictionary, double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed, int* num_workers, size_t* grid_length, enum AlgorithmType* algorithm_type, size_t* amount_hyperparameters, struct array* hyperparameters_bounds) {
    //Create a new identity for the data store that is not in the datadictionary
    uuid_t new_uuid;
    char uuid_str[37];
    while (1) {
        uuid_generate(new_uuid);
        uuid_unparse(new_uuid, uuid_str);

        if (!contains_item_dd(data_dictionary, uuid_str)) {
            break;
        } 
    } 

    char directory[512];
    sprintf(directory, "%s/%s",data_dictionary->directory, data_dictionary->identity);

    struct DataStore* data_store = new_structure_ds(uuid_str, directory, h_min, h_max, dec, lat, freq, layout, n_steps, bas_len, order, snr, max_amp, seed, num_workers, grid_length, algorithm_type, amount_hyperparameters, hyperparameters_bounds);
    return data_store;
}

/**
 * This function will create a new data store with your parameters and return the struct pointer
*/
struct DataStore* new_data_store_update_dd(struct DataDictionary** data_dictionary, double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed, int* num_workers, size_t* grid_length, enum AlgorithmType* algorithm_type, size_t* amount_hyperparameters, struct array* hyperparameters_bounds) {
    struct DataStore* data_store = new_data_store_dd(*data_dictionary, h_min, h_max, dec, lat, freq, layout, n_steps, bas_len, order, snr, max_amp, seed, num_workers, grid_length, algorithm_type, amount_hyperparameters, hyperparameters_bounds);
    add_item_update_dd(data_dictionary, data_store->identity);
    return data_store;
}

/**
 * This function will create a new data container that contains arrays and return its pointer. Makes sure that the container has a unique name
 */
struct DataContainer* new_data_container_type_1_dd(struct DataDictionary* data_dictionary, char* identity, size_t* size) {

    if (contains_item_dd(data_dictionary, identity)) {
        log_file_already_exists(__FILE__, __LINE__);
    } 

    char directory[512];
    sprintf(directory, "%s/%s",data_dictionary->directory, data_dictionary->identity);

    struct DataContainer* data_container = new_save_dcon(identity, directory, &(size_t){1}, &(size_t){*size});

    return data_container;
}

/**
 * This function will create a new data container that contains arrays and stores and return its pointer.
 */
struct DataContainer* new_data_container_type_1_update_dd(struct DataDictionary** data_dictionary, char* identity, size_t* size) {

    struct DataContainer* data_container = new_data_container_type_1_dd(*data_dictionary, identity, size);
    add_item_update_dd(data_dictionary, data_container->identity);
    return data_container;
}

/**
 * This function will create a new data container that contains jarrays and return its pointer. Makes sure that the container has a unique name
 */
struct DataContainer* new_data_container_type_2_dd(struct DataDictionary* data_dictionary, char* identity, size_t* size) {

    if (contains_item_dd(data_dictionary, identity)) {
        log_file_already_exists(__FILE__, __LINE__);
    } 

    char directory[512];
    sprintf(directory, "%s/%s",data_dictionary->directory, data_dictionary->identity);

    struct DataContainer* data_container = new_save_dcon(identity, directory, &(size_t){2}, &(size_t){*size});

    return data_container;
}

/**
 * This function will create a new data container that contains jarrays and stores and return its pointer.
 */
struct DataContainer* new_data_container_type_2_update_dd(struct DataDictionary** data_dictionary, char* identity, size_t* size) {
    struct DataContainer* data_container = new_data_container_type_2_dd(*data_dictionary, identity, size);
    add_item_update_dd(data_dictionary, data_container->identity);
    return data_container;
}



/**
 * This function will get the data store already created and return it.
*/
struct DataStore* read_data_store_dd(char* directory, char* identity) {
    struct DataStore* data_store = read_from_file_ds(directory, identity);
    return data_store;
}

/**
 * Create a new DataDictionary and saves the datadictionary
*/
struct DataDictionary* new_save_dd(char* identity, char* directory) {
    struct DataDictionary* data_dictionary = new_dd(identity, directory);
    save_to_file_dd(data_dictionary);
    return data_dictionary;
}

/**
 * This function adds and updates a datadictionary
*/
void add_item_update_dd(struct DataDictionary** data_dictionary, char* item) {
    add_item_dd(*data_dictionary, item);
    struct DataDictionary* updated_data_dictionary = update_file_dd(*data_dictionary);
    free_dd(*data_dictionary);
    *data_dictionary = updated_data_dictionary;
}

/**
 * This function will start running the configurations in the data_store
*/
void start_data_store_dd(struct DataStore* data_store) {
    start_ds(data_store);
}

/**
 * Plot average iterations
*/
void map_average_iterations_std_dd(struct DataStore* data_store) {
    map_average_iterations_std_ds(data_store);
}