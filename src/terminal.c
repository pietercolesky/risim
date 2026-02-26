/**
  * @file terminal.c
  * This is the implementation of the functions of terminal.h
  */

#include <stdio.h>
#include <stdlib.h>
#include "terminal.h"
#include <sys/time.h>
#include <string.h>
#include "logger.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include "utils.h"
#include <math.h>

#include "data_dictionary.h"
#include "data_store.h"
#include "constants.h"



/**
 * This function will start the terminal in order to interact with the code.
*/
void start_terminal_t() {
    //****************************************************************
    // Preset Variables (Can be changed by user)
    //****************************************************************
    char dd_directory[256] = "../../../../media/pieter/wd/data";
    double h_min = -1;
    double h_max = 1;
    double dec = (-74 - 39.0 / 60 - 37.481 / 3600) * (M_PI / 180);
    double lat = (-30 - 43.0 / 60 - 17.34 / 3600) * (M_PI / 180);
    double freq = 150 * pow(10,6);
    enum LayoutType layout = SQR;
    size_t n_steps = 600;
    double bas_len = 50;
    int order = 2;
    int seed = 12345678;
    int num_workers = 4;
    size_t grid_length = 50;
    double snr = 1000;
    double max_amp = 1.2;
    enum AlgorithmType algorithm_type = STEFCAL_LM_LAMBDA;
    size_t amount_hyperparameters = 2;
    struct array* hyperparameters_bounds = new_set_a(&(size_t){1},
                                    (size_t[]){4}, 
                                    &(enum DataType){DOUBLE}, 
                                    (double[4]){
                                        -0.5,3.0,-0.5,3.0
                                    });
    //****************************************************************

    char input[256];
    int token_count;
    char **tokens;

    struct DataDictionary* data_dictionary = NULL;
    struct DataStore* data_store = NULL;

    logo_t();
    printf("Type 'help' for a list of commands.\n");

    while (1) {
        // Prompt for input
        printf("\033[38;2;20;200;255m");
        printf("> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            log_error_fscan(__FILE__, __LINE__);
        }
        printf("\033[0m"); 

        input[strcspn(input, "\n")] = 0;

        tokens = tokenize_input_t(input, &token_count);

        if (token_count > 0) {
            if (strcmp(tokens[0], "exit") == 0) {
                if (data_dictionary != NULL) {
                    free_dd(data_dictionary);
                }
                if (data_store != NULL) {
                    free_ds(data_store);
                }
                free_a(hyperparameters_bounds);
                free_tokens_t(tokens, token_count);
                break;
            } else if (strcmp(tokens[0], "help") == 0) {
                handle_help_t();
            } else if (strcmp(tokens[0], "list") == 0) {
                if (token_count > 1) {
                    if (strcmp(tokens[1], "dd") == 0) {
                        list_files_t(dd_directory, ".ddconfig");
                    } else if (strcmp(tokens[1], "ds") == 0) {
                        if (data_dictionary != NULL) {
                            char filepath[512];
                            sprintf(filepath, "%s/%s", data_dictionary->directory, data_dictionary->identity);
                            int check = check_file_path(filepath);
                            if (check) {
                                list_files_t(filepath, ".dsconfig");
                            } else {
                                printf("No Data Stores found.\n");
                            }
                        } else {
                            printf("Data Dictionary first need to be created or loaded.\n");
                        }
                    } else if (strcmp(tokens[1], "var") == 0) {
                        if (token_count == 2) {
                            printf("Variables:\n");
                            printf("    dd_directory = %s\n", dd_directory);
                            printf("    h_min = %lf\n", h_min);
                            printf("    h_max = %lf\n", h_max);
                            printf("    dec = %lf\n", dec);
                            printf("    lat = %lf\n", lat);
                            printf("    freq = %lf\n", freq);
                            int layout_int = layout;
                            printf("    layout = %d\n", layout_int);
                            printf("    n_steps = %zu\n", n_steps);
                            printf("    bas_len = %lf\n", bas_len);
                            printf("    order = %d\n", order);
                            printf("    seed = %d\n", seed);
                            printf("    num_workers = %d\n", num_workers);
                            printf("    grid_length = %zu\n", grid_length);
                            printf("    snr = %lf\n", snr);
                            printf("    max_amp = %lf\n", max_amp);
                            int algorithm_int = algorithm_type;
                            printf("    algorithm_type = %d\n", algorithm_int);
                            printf("    amount_hyperparameters = %zu\n", amount_hyperparameters);
                            double* arr = get_a(hyperparameters_bounds);
                            printf("    hyperparameters bounds: [");
                            for (int i = 0; i < get_size_a(hyperparameters_bounds); i++) {
                                if (i == 0) {
                                    printf("%lf",arr[i]);
                                } else {
                                    printf(", %lf",arr[i]);
                                }
                            }
                            printf("]\n");
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else {
                        print_invalid_tokens_t(tokens, &token_count);
                    }
                } else {
                    print_invalid_tokens_t(tokens, &token_count);
                }
            } else if (strcmp(tokens[0], "new") == 0) {
                if (token_count > 1) {
                    if (strcmp(tokens[1], "dd") == 0) {
                        if (token_count == 3) {
                            // create_directory(dd_directory);
                            bool check_dir = check_file_path(dd_directory);
                            if (check_dir) {
                                char filepath[512];
                                sprintf(filepath, "%s/%s.ddconfig", dd_directory, tokens[2]);
                                int check = check_file_path(filepath);
                                if (check) {
                                    printf("Data Dictionary %s already exists.\n", tokens[2]);
                                } else {
                                    data_dictionary = new_save_dd(tokens[2], dd_directory);
                                    printf("Data Dictionary %s created successfully\n", tokens[2]);
                                }
                            } else {
                                printf("Directory %s does not exist.\n", dd_directory);
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "ds") == 0) {
                        if (token_count == 2) {
                            if (data_dictionary != NULL) {
                                //create a new data source
                                struct array* duplicate = zeros_a(hyperparameters_bounds->num_dimensions, hyperparameters_bounds->dimensions, hyperparameters_bounds->type);
                                copy_over_a(duplicate, hyperparameters_bounds, (int[]){-1}, (int[]){-1});
                                data_store = new_data_store_update_dd(&data_dictionary, &h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed, &num_workers, &grid_length, &algorithm_type, &amount_hyperparameters, duplicate);
                                if (data_store == NULL) {
                                    printf("Error creating data store.\n");
                                } else {
                                    printf("Data Store %s created successfully\n", data_store->identity);
                                }
                            } else {
                                printf("Data Dictionary first need to be created or loaded.\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else {
                        print_invalid_tokens_t(tokens, &token_count);
                    }
                } else {
                    print_invalid_tokens_t(tokens, &token_count);
                }
            } else if (strcmp(tokens[0], "load") == 0) {
                if (token_count > 1) {
                    if (strcmp(tokens[1], "dd") == 0) {
                        if (token_count > 2) {
                            //check if directory exists
                            char filepath[512];
                            sprintf(filepath, "%s/%s.ddconfig", dd_directory, tokens[2]);
                            int check = check_file_path(filepath);
                            if (check) {
                                if (data_dictionary != NULL) {
                                    free_dd(data_dictionary);
                                }
                                data_dictionary = read_from_file_dd(dd_directory, tokens[2]);
                                printf("Data Dictionary %s loaded successfully.\n", tokens[2]);
                            } else {
                                printf("Data Dictionary %s does not exist.\n", tokens[2]);
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "ds") == 0) {
                        if (data_dictionary != NULL) {
                            if (token_count > 2) {
                                char filepath[512];
                                sprintf(filepath, "%s/%s", data_dictionary->directory, data_dictionary->identity);
                                int check = check_file_path(filepath);
                                if (check) {
                                    if (data_store != NULL) {
                                        free_ds(data_store);
                                    }
                                    data_store = read_from_file_ds(filepath, tokens[2]);
                                    if (data_store == NULL) {
                                        printf("Could not load Data Store. Try again.");
                                    } else {
                                        printf("Data Store %s loaded successfully.\n", tokens[2]);
                                    }
                                } else {
                                    printf("Data Store %s does not exist.\n", tokens[2]);
                                }
                            } else {
                                print_invalid_tokens_t(tokens, &token_count);
                            }
                        } else {
                            printf("Data Dictionary first need to be created or loaded.\n");
                        }
                    } else {
                        print_invalid_tokens_t(tokens, &token_count);
                    }
                } else {
                    print_invalid_tokens_t(tokens, &token_count);
                }
            } else if (strcmp(tokens[0], "set") == 0) {
                if (token_count > 1) {
                    if (strcmp(tokens[1], "dd_directory") == 0) {
                        if (token_count == 3 ) {
                            char value[256];
                            if (sscanf(tokens[2], "%255s", value) != 1) {
                                printf("Your value is not of type string.\n");
                            } else {
                                strcpy(dd_directory, value);
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "h_min") == 0) {
                        if (token_count == 3 ) {
                            double value = 0;
                            if (sscanf(tokens[2], "%lf", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                h_min = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "h_max") == 0) {
                        if (token_count == 3 ) {
                            double value = 0;
                            if (sscanf(tokens[2], "%lf", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                h_max = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "dec") == 0) {
                        if (token_count == 3 ) {
                            double value = 0;
                            if (sscanf(tokens[2], "%lf", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                dec = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "lat") == 0) {
                        if (token_count == 3 ) {
                            double value = 0;
                            if (sscanf(tokens[2], "%lf", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                lat = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "freq") == 0) {
                        if (token_count == 3 ) {
                            double value = 0;
                            if (sscanf(tokens[2], "%lf", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                freq = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "layout") == 0) {
                        if (token_count == 3 ) {
                            int value = 0;
                            if (sscanf(tokens[2], "%d", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                layout = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "n_steps") == 0) {
                        if (token_count == 3 ) {
                            size_t value = 0;
                            if (sscanf(tokens[2], "%zu", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                n_steps = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "bas_len") == 0) {
                        if (token_count == 3 ) {
                            double value = 0;
                            if (sscanf(tokens[2], "%lf", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                bas_len = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "order") == 0) {
                        if (token_count == 3 ) {
                            int value = 0;
                            if (sscanf(tokens[2], "%d", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                order = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "seed") == 0) {
                        if (token_count == 3 ) {
                            int value = 0;
                            if (sscanf(tokens[2], "%d", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                seed = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "num_workers") == 0) {
                        if (token_count == 3 ) {
                            int value = 0;
                            if (sscanf(tokens[2], "%d", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                num_workers = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "grid_length") == 0) {
                        if (token_count == 3 ) {
                            size_t value = 0;
                            if (sscanf(tokens[2], "%zu", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                grid_length = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "snr") == 0) {
                        if (token_count == 3 ) {
                            double value = 0;
                            if (sscanf(tokens[2], "%lf", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                snr = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "max_amp") == 0) {
                        if (token_count == 3 ) {
                            double value = 0;
                            if (sscanf(tokens[2], "%lf", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                max_amp = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "algorithm_type") == 0) {
                        if (token_count == 3 ) {
                            int value = 0;
                            if (sscanf(tokens[2], "%d", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                algorithm_type = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "amount_hyperparameters") == 0) {
                        if (token_count == 3 ) {
                            size_t value = 0;
                            if (sscanf(tokens[2], "%zu", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else {
                                amount_hyperparameters = value;
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else if (strcmp(tokens[1], "hyperparameters_bounds") == 0) {
                        if (token_count == 4 ) {
                            double value = 0;
                            double index = 0;
                            if (sscanf(tokens[2], "%lf", &index) == 0) {
                                printf("Your value is not of type double.\n");
                            } else
                            if (sscanf(tokens[3], "%lf", &value) == 0) {
                                printf("Your value is not of type double.\n");
                            } else
                            if (index >= get_size_a(hyperparameters_bounds)) {
                                printf("Index out of bound.\n");
                            } else {
                                set_element_a(hyperparameters_bounds,(size_t[]){index}, &(double){value});
                                printf("Variable set successfully\n");
                            }
                        } else {
                            print_invalid_tokens_t(tokens, &token_count);
                        }
                    } else {
                        print_invalid_tokens_t(tokens, &token_count);
                    }
                } else {
                    print_invalid_tokens_t(tokens, &token_count);
                }
            } else if (strcmp(tokens[0], "start") == 0) {
                if (token_count == 2) {
                    if (strcmp(tokens[1], "ds") == 0) {
                        if (data_store == NULL) {
                            printf("Please load of create a Data Source\n");
                        } else {
                            start_data_store_dd(data_store);
                        }
                    } else {
                        print_invalid_tokens_t(tokens, &token_count);
                    }
                } else {
                    print_invalid_tokens_t(tokens, &token_count);
                }
            } else if (strcmp(tokens[0], "map") == 0) {
                if (token_count == 2) {
                    if (strcmp(tokens[1], "ds") == 0) {
                        if (data_store == NULL) {
                            printf("Please load of create a Data Source\n");
                        } else {
                            map_average_iterations_std_dd(data_store);
                            printf("Mapping successfull.\n");
                        }
                    } else {
                        print_invalid_tokens_t(tokens, &token_count);
                    }
                } else {
                    print_invalid_tokens_t(tokens, &token_count);
                }
            } else {
                print_invalid_tokens_t(tokens, &token_count);
            }
        }
        // Free the tokens array after processing the command
        free_tokens_t(tokens, token_count);
    }
}
/**
 * This extracts the tokens
*/
char** tokenize_input_t(char *input, int *token_count) {
    int capacity = 10; // Initial capacity for tokens array
    char **tokens = malloc(capacity * sizeof(char*));
    if (tokens == NULL) {
        perror("Unable to allocate memory for tokens");
        exit(EXIT_FAILURE);
    }

    char *token;
    int count = 0;

    // Use strtok to split input by spaces
    token = strtok(input, " ");
    while (token != NULL) {
        // Reallocate if capacity is exceeded
        if (count >= capacity) {
            capacity *= 2;
            tokens = realloc(tokens, capacity * sizeof(char*));
            if (tokens == NULL) {
                perror("Unable to reallocate memory for tokens");
                exit(EXIT_FAILURE);
            }
        }
        tokens[count] = malloc(strlen(token) + 1);
        if (tokens[count] == NULL) {
            perror("Unable to allocate memory for token");
            exit(EXIT_FAILURE);
        }
        strcpy(tokens[count], token);
        count++;
        token = strtok(NULL, " ");
    }

    *token_count = count;
    return tokens;
}

/**
 * this function frees the tokens being used created
*/
void free_tokens_t(char **tokens, int token_count) {
    for (int i = 0; i < token_count; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

/**
 * This function will display the logo
*/
void logo_t() {
    printf("\033[38;2;20;200;255m");
    printf("             *                         ,-.                                        \n");
    printf(" .        .       .            .      / \\  `.  __..-,O      .             .     \n");
    printf("    *                   *            :   \\ --''_..-'.'          *    *         \n");
    printf("               *         *           |    . .-' `. '.                             \n");
    printf("      ,-.                 *          :     .     .`.'          *                   \n");
    printf("     / \\  `.  __..-,O          .      \\     `.  /  ..                           \n");
    printf("    :   \\ --''_..-'.'        .         \\      `.   ' .     .         *           \n");
    printf("    |    . .-' `. '.                    `,       `.   \\            .             \n");
    printf("    :     .     .`.'                    ,|,`.        `-.\\                        \n");
    printf("     \\     `.  /  ..                   '.||  ``-...__..-`                        \n");
    printf("      \\      `.   ' .                   |  |                                     \n");
    printf("       `,       `.   \\                  |__|                                     \n");
    printf("      ,|,`.        `-.\\                 /||\\                                    \n");
    printf("     '.||  ``-...__..-`                //||\\\\                                   \n");
    printf("      |  |                            // || \\\\                                  \n");
    printf("      |__|                         __//__||__\\\\__                               \n");
    printf("      /||\\                        '--------------'                               \n");
    printf("     //||\\\\        _____       _             __                _____      _     \n");
    printf("    // || \\\\      |_   _|     | |           / _|              / ____|    | |    \n");
    printf(" __//__||__\\\\__     | |  _ __ | |_ ___ _ __| |_ ___ _ __ ___ | |     __ _| |    \n");
    printf("'--------------'    | | | '_ \\| __/ _ \\ '__|  _/ _ \\ '__/ _ \\| |    / _` | |  \n");
    printf("                   _| |_| | | | ||  __/ |  | ||  __/ | | (_) | |___| (_| | |      \n");
    printf("                  |_____|_| |_|\\__\\___|_|  |_| \\___|_|  \\___/ \\_____\\__,_|_|\n");
    printf("\n");
    printf("\033[0m"); 
}

/**
 * Function for help in terminal
*/
void handle_help_t() {
    printf("Available commands:\n");
    printf("  help              - Show this help message.\n");
    printf("  exit              - Exit the terminal.\n");
    printf("  list dd           - Lists Data Dictionaries in your directory.\n");
    printf("  list ds           - Lists Data Stores availible in your Data Dictionary.\n");
    printf("  new dd <name>     - Create a new Data Dictionary.\n");
    printf("  new ds            - Create a new Data Store.\n");
    printf("  load dd <name>    - Loads specified Data Dictionary.\n");
    printf("  load ds <name>    - Loads specified Data Store.\n");
    printf("\n");
    printf("Available algorithms:\n");
    printf("  [0]    GRADIENT_DESENT\n");
    printf("  [1]    GRADIENT_DESENT_MOMENTUM\n");
    printf("  [2]    NESTROVS_ACCELERATED_GRADIENT\n");
    printf("  [3]    ADAGRAD\n");
    printf("  [4]    RMSPROP\n");
    printf("  [5]    ADAM\n");
    printf("  [6]    GAUSS_NEWTON\n");
    printf("  [7]    LEVENBERG_MARQUARDT\n");
    printf("  [8]    LEVENBERG_MARQUARDT_DIAGONAL_HESSIAN_LAMBDA\n");
    printf("  [9]    STEFCAL\n");
    printf("  [10]   STEFCAL_LAMBDA\n");
    printf("  [11]   STEFCAL_LM_LAMBDA\n");
    printf("\n");
    printf("Available antenna layouts:\n");
    printf("  [0]    HEX\n");
    printf("  [1]    SQR\n");
    printf("  [2]    REG\n");

}

/**
 * This function will list all the datadictionaries in the current directory
*/
void list_files_t(char* path, char* suffix) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    bool check = false;

    bool check_dir = check_file_path(path);
    if (check_dir) {

        dir = opendir(path);
        if (dir == NULL) {
            log_could_not_open_directory(__FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }

        while ((entry = readdir(dir)) != NULL) {
            char full_path[512];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

            if (stat(full_path, &file_stat) == -1) {
                continue;
            }

            if (S_ISREG(file_stat.st_mode)) {
                if (ends_with_string(entry->d_name, suffix)) {
                    check = true;
                    char* filename = extract_filename_from_filepath(entry->d_name);
                    printf("%s\n", filename);
                    free(filename);
                }
            }
        }

        
        if (strcmp(suffix, ".ddconfig") == 1) {
            if (!check) {
                printf("No data dictionary found.\n");
            }
        } else if (strcmp(suffix, ".ddconfig") == 1) {
            if (!check) {
                printf("No data store found.\n");
            }
        } else {
            if (!check) {
                printf("No data found.\n");
            }
        }

        closedir(dir);
    } else {
        printf("No data dictionary found.\n");
    }
}

/**
 * This function will print the invalid token sequence
*/
void print_invalid_tokens_t(char** tokens, int* token_count) {
    switch (*token_count) {
        case 0: {
            printf("Please enter a command.\n");
            break;
        }
        case 1: {
            printf("Unknown command: %s\n", tokens[0]);
            break;
        }
        case 2: {
            printf("Unknown command: %s %s\n", tokens[0], tokens[1]);
            break;
        }
        case 3: {
            printf("Unknown command: %s %s %s\n", tokens[0], tokens[1], tokens[2]);
            break;
        }
        case 4: {
            printf("Unknown command: %s %s %s %s\n", tokens[0], tokens[1], tokens[2], tokens[3]);
            break;
        }
        case 5: {
            printf("Unknown command: %s %s %s %s %s\n", tokens[0], tokens[1], tokens[2] , tokens[3], tokens[4]);
            break;
        }
        case 6: {
            printf("Unknown command: %s %s %s %s %s %s\n", tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5]);
            break;
        }
        case 7: {
            printf("Unknown command: %s %s %s %s %s %s %s\n", tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[6]);
            break;
        }
        default: {
            printf("Command too long.\n");
            break;
        }
    }
}