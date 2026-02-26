/**
  * @file terminal.h
  * @description A definition of functions offered by the terminal
  */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#ifndef _TERMINAL_H
#define _TERMINAL_H

/**
 * This function will start the terminal in order to interact with the code.
*/
void start_terminal_t();

/**
 * This extracts the tokens
*/
char** tokenize_input_t(char *input, int *token_count);

/**
 * this function frees the tokens being used created
*/
void free_tokens_t(char **tokens, int token_count);

/**
 * This function will display the logo
*/
void logo_t();

/**
 * Function for help in terminal
*/
void handle_help_t();

/**
 * This function will list all the datadictionaries in the current directory
*/
void list_files_t(char* path, char* suffix);

/**
 * This function will print the invalid token sequence
*/
void print_invalid_tokens_t(char** tokens, int* token_count);

#endif