/**
  * @file data_container.h
  * @description A definition of functions offered by the data_container
  */

#include "array.h"
#include "utils.h"
#include <stdbool.h>
#include "jagged_array.h"

#ifndef _DATA_CONTAINER_H
#define _DATA_CONTAINER_H

/**
 * DataContainer struct
*/
struct DataContainer {
    char* identity;
    char* directory;
    size_t* type;
    size_t* size;
    char** items;
};

/**
 * Create a new DataContainer struct
*/
struct DataContainer* new_dcon(char* identity, char* directory, size_t* type, size_t* size);

/**
 * Free DataContainer struct
*/
void free_dcon(struct DataContainer* data_container);

/**
 * This will print the DataContainer
*/
void print_dcon(struct DataContainer* data_container);

/**
 * This fuction will set the value of a specific index
*/
void set_item_dcon(struct DataContainer* data_container, char* item, size_t* index);

/**
 * This function checks if an item is in the DataContainer
*/
bool contains_item_dcon(struct DataContainer* data_container, char* item);

/**
 * Check if a data store is finished.
*/
bool check_done_dcon(struct DataContainer* data_container);

/**
 * Get the persentage done of a data store
*/
double get_persentage_done_dcon(struct DataContainer* data_container);

/**
 * save DataStore to file
*/
void save_to_file_dcon(struct DataContainer* data_container);

/**
 * Read .array file
*/
struct DataContainer* read_from_file_dcon(char* directory, char* identity);

/**
 * Update DataContainer file
*/
struct DataContainer* update_file_dcon(struct DataContainer* data_container);

/**
 * Create a DataStore struct
*/
struct DataContainer* new_save_dcon(char* identity, char* directory, size_t* type, size_t* size);

/**
 * This function sets and updates a DataContainer
*/
void set_item_update_dcon(struct DataContainer** data_container, char* item, size_t* index);

/**
 * This function returns the number of experiments already have been run
*/
int get_progress_dcon(struct DataContainer* data_container);

/**
 * Set a new datacontainer holding data containers to this datacontainer at a specific position
*/
struct DataContainer* set_new_datacontainer_0_dcon(struct DataContainer** data_container, size_t* index, char* identity, size_t* size);

/**
 * Set a new datacontainer holding arrays to this datacontainer at a specific position
*/
struct DataContainer* set_new_datacontainer_1_dcon(struct DataContainer** data_container, size_t* index, char* identity, size_t* size);

/**
 * Set a new datacontainer holding jarrays to this datacontainer at a specific position
*/
struct DataContainer* set_new_datacontainer_2_dcon(struct DataContainer** data_container, size_t* index, char* identity, size_t* size);

/**
 * Add an array to this datacontainer at a sepecific posision
*/
void set_new_array_dcon(struct DataContainer** data_container, size_t* index, char* name, struct array* array);

/**
 * Add a jarray to this datacontainer at a sepecific posision
*/
void set_new_jarray_dcon(struct DataContainer** data_container, size_t* index, char* name, struct Jaggedarray* jagged_array);

#endif