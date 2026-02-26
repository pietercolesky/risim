/**
  * @file logger.c
  * This is the implementation of the functions of logger.h
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define LOG_ERROR(file, line, message) fprintf(stderr, "Error in %s at line %d: %s\n", file, line, message)

/**
 * This logs a specified error message.
*/
void log_error(char *error_message, const char* file, int line) {
    LOG_ERROR(file, line, error_message);
    exit(0);
}

/**
 * This is an error for when the simulator is null.
*/
void log_sim_null(const char* file, int line){
    LOG_ERROR(file, line, "Simulator is NULL");
    exit(0);
}

/**
 *  This is an error for when there is an memory allocation fail.
*/
void log_mem_aloc_fail(const char* file, int line) {
    LOG_ERROR(file, line, "Memory allocation failed");
    exit(0);
}

/**
 * When a Datatype is entered that is not supported.
*/
void log_unsupported_type(const char* file, int line) {
    LOG_ERROR(file, line, "Unsupported Type");
    exit(0);
}

/**
 * When the types of two arrays are not the same.
*/
void log_incompatible_types(const char* file, int line){
    LOG_ERROR(file, line, "Incompatible Types");
    exit(0);
}

/**
 * The size of two arrays are not the same.
*/
void log_incompatible_size(const char* file, int line){
    LOG_ERROR(file, line, "Incompatible Size");
    exit(0);
}

/**
 * The dimensions of two arrays are not the same size
*/
void log_incompatible_dimensions(const char* file, int line){
    LOG_ERROR(file, line, "Incompatible Dimensions");
    exit(0);
}

/**
 * This is an out of bound error
*/
void log_index_out_of_bounds(const char* file, int line){
    LOG_ERROR(file, line, "Index Out Of Bound");
    exit(0);
}

/**
 * array is better for as Matrix with one dimention.
*/
void log_use_array(const char* file, int line){
    LOG_ERROR(file, line, "Use array Struct Instead");
    exit(0);
}

/**
 * Dimentions are invalid.
*/
void log_invalid_dimensions(const char* file, int line){
    LOG_ERROR(file, line, "Invalid Dimensions");
    exit(0);
}

/**
 * Type are invalid.
*/
void log_invalid_type(const char* file, int line){
    LOG_ERROR(file, line, "Invalid Type");
    exit(0);
}

/**
 * Matrix construction fail.
*/
void log_matrix_construction_fail(const char* file, int line){
    LOG_ERROR(file, line, "Matrix Construction Fail.");
    exit(0);
}

/**
 * When a portion of code is not yet implemented.
*/
void log_not_implemented(const char* file, int line) {
    LOG_ERROR(file, line, "Not Yet Implemented.");
    exit(0);
}

/**
 * When slice is incompatible.
*/
void log_incompatible_slice(const char* file, int line) {
    LOG_ERROR(file, line, "Incompatible Slice.");
    exit(0);
}

/**
 * Invalid Order.
*/
void log_invalid_order(const char* file, int line) {
    LOG_ERROR(file, line, "Invalid Order. Order Needs To Be More Than One.");
    exit(0);
}

/**
 * Invalid Layout
*/
void log_invalid_layout(const char* file, int line) {
    LOG_ERROR(file, line, "Invalid Layout.");
    exit(0);
}

/**
 * NULL array
*/
void log_null_array(const char* file, int line) {
    LOG_ERROR(file, line, "NULL array.");
    exit(0);
}

/**
 * Error opening file
*/
void log_error_opening_file(const char* file, int line) {
    LOG_ERROR(file, line, "Error opening file.");
    exit(0);
}

/**
 * fscan error
*/
void log_error_fscan(const char* file, int line) {
    LOG_ERROR(file, line, "Error with fscan.");
    exit(0);
}

/**
 * Invalid file extension
*/
void log_invalid_file_extension(const char* file, int line) {
    LOG_ERROR(file, line, "Invalid file extension.");
    exit(0);
}

/**
 * Error opening file to lock
*/
void log_error_opening_file_to_lock(const char* file, int line) {
    LOG_ERROR(file, line, "Error opening file to lock.");
    exit(0);
}

/**
 * Error locking file
*/
void log_error_locking_file(const char* file, int line) {
    LOG_ERROR(file, line, "Error locking file.");
    exit(0);
}

/**
 * Error unlocking file
*/
void log_error_unlocking_file(const char* file, int line) {
    LOG_ERROR(file, line, "Error unlocking file.");
    exit(0);
}

/**
 * Null value
*/
void log_null_value(const char* file, int line) {
    LOG_ERROR(file, line, "NULL value.");
    exit(0);
}

/**
 * Error truncating file
*/
void log_error_truncating_file(const char* file, int line) {
    LOG_ERROR(file, line, "Error truncating file.");
    exit(0);
}

/**
 * String too long
*/
void log_string_too_long(const char* file, int line) {
    LOG_ERROR(file, line, "String too long.");
    exit(0);
}

/**
 * Could not open directory
*/
void log_could_not_open_directory(const char* file, int line) {
    LOG_ERROR(file, line, "Could not open directory.");
    exit(0);
}

/**
 * Data Dictionary already exists
*/
void log_data_dictionary_already_exists(const char* file, int line) {
    LOG_ERROR(file, line, "Data Dictionary already exists.");
    exit(0);
}

/**
 * Data Store already exists
*/
void log_data_store_already_exists(const char* file, int line) {
    LOG_ERROR(file, line, "Data Store already exists.");
    exit(0);
}

/**
 * Invalid algorithm type
*/
void log_invalid_algorithm_type(const char* file, int line) {
    LOG_ERROR(file, line, "Invalid algorithm type.");
    exit(0);
}

/**
 * Invalid directory
*/
void log_invalid_directory(const char* file, int line) {
    LOG_ERROR(file, line, "Invalid directory.");
    exit(0);
}

/**
 * File already exists
*/
void log_file_already_exists(const char* file, int line) {
    LOG_ERROR(file, line, "File already exists.");
    exit(0);
}

/**
 * Invalid choice
 */
void log_invalid_choice(const char* file, int line) {
    LOG_ERROR(file, line, "Invalid choice.");
    exit(0);
}

/**
 * SVD did not converge
 */
void log_svd_not_converge(const char* file, int line) {
    LOG_ERROR(file, line, "SVD did not converge.");
    exit(0);
}