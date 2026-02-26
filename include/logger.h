/**
  * @file logger.h
  * @description A definition of functions offered by the logger
  */

#ifndef _LOGGER_H
#define _LOGGER_H

/* Functions */
void log_error(char *error_message, const char* file, int line);
void log_sim_null(const char* file, int line);
void log_mem_aloc_fail(const char* file, int line);
void log_unsupported_type(const char* file, int line);
void log_incompatible_types(const char* file, int line);
void log_incompatible_size(const char* file, int line);
void log_incompatible_dimensions(const char* file, int line);
void log_index_out_of_bounds(const char* file, int line);
void log_use_array(const char* file, int line);
void log_invalid_dimensions(const char* file, int line);
void log_invalid_type(const char* file, int line);
void log_matrix_construction_fail(const char* file, int line);
void log_not_implemented(const char* file, int line);
void log_incompatible_slice(const char* file, int line);
void log_invalid_order(const char* file, int line);
void log_invalid_layout(const char* file, int line);
void log_null_array(const char* file, int line);
void log_error_opening_file(const char* file, int line);
void log_error_fscan(const char* file, int line);
void log_invalid_file_extension(const char* file, int line);
void log_error_opening_file_to_lock(const char* file, int line);
void log_error_locking_file(const char* file, int line);
void log_error_unlocking_file(const char* file, int line);
void log_null_value(const char* file, int line);
void log_error_truncating_file(const char* file, int line);
void log_string_too_long(const char* file, int line);
void log_could_not_open_directory(const char* file, int line);
void log_data_dictionary_already_exists(const char* file, int line);
void log_data_store_already_exists(const char* file, int line);
void log_invalid_algorithm_type(const char* file, int line);
void log_invalid_directory(const char* file, int line);
void log_file_already_exists(const char* file, int line);
void log_invalid_choice(const char* file, int line);
void log_svd_not_converge(const char* file, int line);

#endif