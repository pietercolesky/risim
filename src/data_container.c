/**
  * @file data_container.c
  * This is the implementation of the functions of data_container.h
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <uuid/uuid.h>
#include "logger.h"
#include <string.h>
#include "simulator.h"
#include "calibrator.h"
#include "data_container.h"
#include "constants.h"
#include "jagged_array.h"

/**
 * Create a new DataContainer struct
*/
struct DataContainer* new_dcon(char* identity, char* directory, size_t* type, size_t* size) {
    struct DataContainer* data_container = malloc(sizeof(struct DataContainer));

    if (data_container == NULL) {
        return NULL;
    }

    data_container->identity = malloc(MAX_STRING_LENGTH* sizeof(char));
    if (!data_container->identity) {
        free_dcon(data_container);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    strcpy(data_container->identity, identity);

    data_container->directory = malloc(MAX_STRING_LENGTH* sizeof(char));
    if (!data_container->directory) {
        free_dcon(data_container);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    strcpy(data_container->directory, directory);

    data_container->type = malloc(sizeof(size_t));
    if (!data_container->type) {
        free_dcon(data_container);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    *data_container->type =  *type;

    data_container->size = malloc(sizeof(size_t));
    if (!data_container->size) {
        free_dcon(data_container);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    *data_container->size = *size;

    data_container->items = malloc(*data_container->size * sizeof(char*));
    if (!data_container->items) {
        free_dcon(data_container);
        log_mem_aloc_fail(__FILE__, __LINE__);
        return NULL;
    }

    for (size_t i = 0; i < *data_container->size; ++i) {
        data_container->items[i] = strdup("-");
        if (!data_container->items[i]) {
            log_mem_aloc_fail(__FILE__, __LINE__);
            for (size_t j = 0; j < i; ++j) {
                free(data_container->items[j]);
            }
            free(data_container->items);
            free_dcon(data_container);
            return NULL;
        }
    }

    return data_container;
}

/**
 * Free DataContainer struct
*/
void free_dcon(struct DataContainer* data_container) {
    if (data_container != NULL) {
        if (data_container->items != NULL) {
            for (size_t i = 0; i < *data_container->size; ++i) {
                if (data_container->items[i] != NULL) {
                free(data_container->items[i]);
                }
            }
            free(data_container->items);
        }
        if (data_container->identity != NULL) {
            free(data_container->identity);
        }
        if (data_container->directory != NULL) {
            free(data_container->directory);
        }
        if (data_container->type != NULL) {
            free(data_container->type);
        }
        if (data_container->size != NULL) {
            free(data_container->size);
        }
        free(data_container);
    }
}

/**
 * This will print the DataContainer
*/
void print_dcon(struct DataContainer* data_container) {
    if (data_container == NULL) {
        return;
    }

    printf("{\n");
    printf("  identity: %s\n",data_container->identity);
    printf("  directory: %s\n",data_container->directory);
    printf("  type: %zu\n",*data_container->type);
    printf("  size: %zu\n",*data_container->size);
    if (*data_container->size == 0) {
      printf("\n");
      printf("  Currently no items in Data Container.\n");
      printf("}\n");
    } else {
      printf("\n");
      for (size_t i = 0; i < *data_container->size; i++) {
        printf("  Item: %s\n", data_container->items[i]);
      }
      printf("}\n");
    }
}
/**
 * This fuction will set the value of a specific index
*/
void set_item_dcon(struct DataContainer* data_container, char* item, size_t* index) {
    if (data_container == NULL || item == NULL || index == NULL) {
        log_null_value(__FILE__, __LINE__);
    }

    if (*index >= *data_container->size) {
        log_index_out_of_bounds(__FILE__, __LINE__);
    }

    size_t item_len = strlen(item);

    if (item_len >= MAX_STRING_LENGTH) {
        log_string_too_long(__FILE__,__LINE__);
    }

    if (data_container->items[*index] != NULL) {
        free(data_container->items[*index]);
    }

    data_container->items[*index] = malloc((item_len + 1) * sizeof(char));
    if (data_container->items[*index] == NULL) {
        log_mem_aloc_fail(__FILE__,__LINE__);
        return;
    }

    strcpy(data_container->items[*index], item);
}

/**
 * This function checks if an item is in the DataContainer
*/
bool contains_item_dcon(struct DataContainer* data_container, char* item) {
    if (!data_container || !item) {
        log_null_value(__FILE__, __LINE__);
    }

    for (size_t i = 0; i < *data_container->size; i++) {
        if (strcmp(item, data_container->items[i]) == 0) {
            return true;
        }
    }

    return false;
}

/**
 * Check if a data store is finished.
*/
bool check_done_dcon(struct DataContainer* data_container) {
    int progress = get_progress_dcon(data_container);
    if (progress == *data_container->size) {
        return 1;
    }
    return 0;
}

/**
 * Get the persentage done of a data store
*/
double get_persentage_done_dcon(struct DataContainer* data_container) {
    return (double)get_progress_dcon(data_container) / *data_container->size * (double)100.00;
}

/**
 * save DataContainer to file
*/
void save_to_file_dcon(struct DataContainer* data_container) {
    char filepath[512];
    sprintf(filepath, "%s/%s.dcconfig", data_container->directory, data_container->identity);

    create_directory(data_container->directory);

    FILE *combinedDataFile = fopen(filepath, "w");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return;
    }
    fprintf(combinedDataFile, "%s\n", data_container->identity);
    fprintf(combinedDataFile, "%zu\n", *data_container->type);
    fprintf(combinedDataFile, "%zu\n", *data_container->size);

    for (size_t i = 0; i < *data_container->size; i++) {
      fprintf(combinedDataFile, "%s\n", data_container->items[i]);
    }

    fclose(combinedDataFile);
}

/**
 * Read .array file
*/
struct DataContainer* read_from_file_dcon(char* directory, char* identity) {
    char filepath[512];
    sprintf(filepath, "%s/%s.dcconfig", directory, identity);

    if(!ends_with_string(filepath, ".dcconfig")) {
        log_invalid_file_extension(__FILE__,__LINE__);
    }

    FILE *combinedDataFile = fopen(filepath, "r");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return NULL;
    }

    size_t type = 0;
    size_t size = 0;

    char identity_read[MAX_STRING_LENGTH];

    if (fscanf(combinedDataFile, "%s", identity_read) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%zu", &type) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%zu", &size) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }

    int c;
    while ((c = fgetc(combinedDataFile)) != EOF && c != '\n');
    char* filename = extract_filename_from_filepath(filepath);
    struct DataContainer* read_data_container = new_dcon(identity_read, directory, &type, &size);
    free(filename);

    for (size_t i = 0; i < size; i++) {
        //add items to the list
        char item[MAX_STRING_LENGTH];

        if (fscanf(combinedDataFile, "%255s\n", item) != 1) {
            log_error_fscan(__FILE__, __LINE__);
            free_dcon(read_data_container);
            fclose(combinedDataFile);
            return NULL;
        }

        // add_item_ds(read_data_store, item, path);
        set_item_dcon(read_data_container, item, &i );

        if (read_data_container->items[i] == NULL) {
            log_mem_aloc_fail(__FILE__, __LINE__);
            free_dcon(read_data_container); // Free allocated memory
            fclose(combinedDataFile);
            return NULL;
        }

    }

    fclose(combinedDataFile);
    return read_data_container;
}

/**
 * Update DataContainer file
*/
struct DataContainer* update_file_dcon(struct DataContainer* data_container) {
    char filepath[512];
    sprintf(filepath, "%s/%s.dcconfig", data_container->directory, data_container->identity);

    FILE *combinedDataFile = fopen(filepath, "r+");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return NULL;
    }

    char identity[MAX_STRING_LENGTH];
    size_t type = 0;
    size_t size = 0;

    if (fscanf(combinedDataFile, "%s", identity) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%zu", &type) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%zu", &size) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }

    int c;
    while ((c = fgetc(combinedDataFile)) != EOF && c != '\n');
    char* filename = extract_filename_from_filepath(filepath);
    struct DataContainer* read_data_container = new_dcon(identity, data_container->directory, &type, &size);
    free(filename); 

    for (size_t i = 0; i < size; i++) {
        //add items to the list
        char item[MAX_STRING_LENGTH];

        if (fscanf(combinedDataFile, "%255s\n", item) != 1) {
            log_error_fscan(__FILE__, __LINE__);
            free_dcon(read_data_container);
            fclose(combinedDataFile);
            return NULL;
        }

        // add_item_ds(read_data_store, item, path);
        set_item_dcon(read_data_container, item, &i );

        if (read_data_container->items[i] == NULL) {
            log_mem_aloc_fail(__FILE__, __LINE__);
            free_dcon(read_data_container); // Free allocated memory
            fclose(combinedDataFile);
            return NULL;
        }

    }

    //check what is not in read_data_dictionary but is in data_dictionary and add it to read_data_dictionary

    for (size_t i = 0; i < *data_container->size; i++) {
        if (!strcmp(read_data_container->items[i], data_container->items[i]) == 0) {
            set_item_dcon(read_data_container, data_container->items[i], &i);
        }
    }

    //overwrite the saved file with read_datadictionary.
    if (ftruncate(fileno(combinedDataFile), 0) == -1) {
        log_error_truncating_file(__FILE__, __LINE__);
        fclose(combinedDataFile);
        return NULL;
    }

    rewind(combinedDataFile);

    fprintf(combinedDataFile, "%s\n", data_container->identity);
    fprintf(combinedDataFile, "%zu\n", *data_container->type);
    fprintf(combinedDataFile, "%zu\n", *data_container->size);

    for (size_t i = 0; i < *data_container->size; i++) {
      fprintf(combinedDataFile, "%s\n", data_container->items[i]);
    }

    fclose(combinedDataFile);
    return read_data_container;
}

/**
 * Create a DataStore struct
*/
struct DataContainer* new_save_dcon(char* identity, char* directory, size_t* type, size_t* size) {
    struct DataContainer* data_container = new_dcon(identity, directory, type, size);
    save_to_file_dcon(data_container);
    return data_container;
}

/**
 * This function sets and updates a DataContainer
*/
void set_item_update_dcon(struct DataContainer** data_container, char* item, size_t* index) {
    set_item_dcon(*data_container, item, index);
    struct DataContainer* updated_data_container = update_file_dcon(*data_container);
    free_dcon(*data_container);
    *data_container = updated_data_container;
}

/**
 * This function returns the number of experiments already have been run
*/
int get_progress_dcon(struct DataContainer* data_container) {
    if (data_container == NULL) {
        log_null_value(__FILE__, __LINE__);
    }

    int counter = 0;

    for (size_t i = 0; i < *data_container->size; i++) {
        if (!strcmp(data_container->items[i], "-") == 0) {
            counter++;
        }
    }

    return counter;
}

/**
 * Add a datacontainer to this datacontainer at a specific position
*/
struct DataContainer* set_new_datacontainer_0_dcon(struct DataContainer** data_container, size_t* index, char* identity, size_t* size) {
    if (*(*data_container)->type != 0) {
        log_invalid_type(__FILE__,__LINE__);
    }
    
    char file_path[256];
    sprintf(file_path, "%s/%s", (*data_container)->directory, (*data_container)->identity);
    //create new datacontainer
    //save new datacontainer
    struct DataContainer* new_data_container = new_save_dcon(identity,file_path,&(size_t){0},size);
    //add datacontainer to datacontainer
    set_item_update_dcon(data_container,identity,index);
    //return new datacontainer
    return new_data_container;
}

/**
 * Set a new datacontainer holding arrays to this datacontainer at a specific position
*/
struct DataContainer* set_new_datacontainer_1_dcon(struct DataContainer** data_container, size_t* index, char* identity, size_t* size) {
    if (*(*data_container)->type != 0) {
        log_invalid_type(__FILE__,__LINE__);
    }
    
    char file_path[256];
    sprintf(file_path, "%s/%s", (*data_container)->directory, (*data_container)->identity);
    //create new datacontainer
    //save new datacontainer
    struct DataContainer* new_data_container = new_save_dcon(identity,file_path,&(size_t){1},size);
    //add datacontainer to datacontainer
    set_item_update_dcon(data_container,identity,index);
    //return new datacontainer
    return new_data_container;
}

/**
 * Add a datacontainer to this datacontainer at a specific position
*/
struct DataContainer* set_new_datacontainer_2_dcon(struct DataContainer** data_container, size_t* index, char* identity, size_t* size) {
    if (*(*data_container)->type != 0) {
        log_invalid_type(__FILE__,__LINE__);
    }
    
    char file_path[256];
    sprintf(file_path, "%s/%s", (*data_container)->directory, (*data_container)->identity);
    //create new datacontainer
    //save new datacontainer
    struct DataContainer* new_data_container = new_save_dcon(identity,file_path,&(size_t){2},size);
    //add datacontainer to datacontainer
    set_item_update_dcon(data_container,identity,index);
    //return new datacontainer
    return new_data_container;
}


/**
 * Add an array to this datacontainer at a sepecific posision
*/
void set_new_array_dcon(struct DataContainer** data_container, size_t* index, char* name, struct array* array) {
    if (*(*data_container)->type != 1) {
        log_invalid_type(__FILE__,__LINE__);
    }

    char file_path[256];
    sprintf(file_path, "%s/%s", (*data_container)->directory, (*data_container)->identity);
    //save array
    save_to_file_a(array, file_path, name);
    //add array to datacontainer
    set_item_update_dcon(data_container,name,index);
}

/**
 * Add a jarray to this datacontainer at a sepecific posision
*/
void set_new_jarray_dcon(struct DataContainer** data_container, size_t* index, char* name, struct Jaggedarray* jagged_array) {
    if (*(*data_container)->type != 2) {
        log_invalid_type(__FILE__,__LINE__);
    }

    char file_path[256];
    sprintf(file_path, "%s/%s", (*data_container)->directory, (*data_container)->identity);
    //save jagged array
    save_to_file_ja(jagged_array, file_path, name);
    //add array to datacontainer
    set_item_update_dcon(data_container,name,index);
}

