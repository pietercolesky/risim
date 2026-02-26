/**
  * @file simulator.c
  * This is the implementation of the functions of simulator.h
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "logger.h"
#include "simulator.h"
#include "utils.h"
#include "constants.h"


/**
 * Create a simulator   +
 * RETURN: pointer to simulator struct that is empty
*/
struct Simulator* newSimulatorStructure(){
    struct Simulator* simulator = malloc(sizeof(struct Simulator));

    if (simulator == NULL) {
        return NULL;
    }

    simulator->h_min = malloc(sizeof(double));
    *simulator->h_min = 0;
    simulator->h_max = malloc(sizeof(double));
    *simulator->h_max = 0;
    simulator->n_steps = malloc(sizeof(size_t));
    *simulator->n_steps = 0;
    simulator->dec = malloc(sizeof(double));
    *simulator->dec = 0;
    simulator->lat = malloc(sizeof(double));
    *simulator->lat = 0;
    simulator->freq = malloc(sizeof(double));
    *simulator->freq = 0;
    simulator->wave = malloc(sizeof(double));
    *simulator->wave = 0;
    simulator->layout = malloc(sizeof(enum LayoutType));
    *simulator->layout = 0;
    simulator->bas_len = malloc(sizeof(double));
    *simulator->bas_len = 0;
    simulator->order = malloc(sizeof(int));
    *simulator->order = 0;
    simulator->N = malloc(sizeof(size_t));
    *simulator->N = 0;
    simulator->B = malloc(sizeof(size_t));
    *simulator->B = 0;
    simulator->L = malloc(sizeof(size_t));
    *simulator->L = 0;
    simulator->snr = malloc(sizeof(double));
    *simulator->snr = 0;
    simulator->max_amp = malloc(sizeof(double));
    *simulator->max_amp = 0;
    simulator->seed = malloc(sizeof(unsigned int));
    *simulator->seed = 0;
    
    simulator->h_range = NULL;
    simulator->ant = NULL;
    simulator->u_m = NULL;
    simulator->v_m = NULL;
    simulator->w_m = NULL;
    simulator->phi = NULL;
    simulator->zeta = NULL;
    simulator->point_sources = NULL;
    simulator->antenna_gains = NULL;
    simulator->M = NULL;
    simulator->D = NULL;
    simulator->rng = NULL;


    return simulator;
}

/**
 * Initializing similator with own parameters
 *  INPUTS: 
    - h_min - Minimum hour angle of the observation
    - h_max - Maximum hour angle of the observation
    - dec - Declination of observation
    - lat - Latitude of array
        #lat = (-30 - 43.0/60.0 - 17.34/3600)*(M_PI/180) #KAT7
        #lat = (52 + 55/60.0 + 0.09/3600)*(M_PI/180) #WSRT   
        #lat = (38. + 59./60. + 48./3600.)*(M_PI/180) #College Park
    - min_amp - Minimum gain amplitude
    - freq - Frequency of observation in Hz
            #freq = 1.4*10**9 1.4 GHz
    - layout - Specify the name of the antenna layout file 
            # layout = "HEX" - generate a HEX layout instead of reading from file using bas_len and order (number of rings)
            # layout = "REG" - generate a regular line instead of reading from file using bas_len and order (number of antennas)
            # layout = "SQR" - generate a square layout reading from file using bas_len and order (side length of square)   
    - nsteps - Number of timesteps
    - bas_len - Fundamental baseline length used to create HEX, SQR and REG layouts
    - order - Fundamental parameter which determines the layout size
    - seed - 
 *  RETURNS: Simulator struct pointer
*/
struct Simulator* initialize_simulator(double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed) {
    struct Simulator* simulator = newSimulatorStructure();
    //Initialization
    *simulator->h_min = *h_min;
    *simulator->h_max = *h_max;
    *simulator->n_steps = *n_steps;
    struct array* range = linspace_a(&(double){*simulator->h_min}, &(double){*simulator->h_max} , simulator->n_steps , &(enum DataType){DOUBLE});
    simulator->h_range = multiply_constant_a(range, &(double){M_PI/12}, &(enum DataType){DOUBLE});
    free_a(range);
    *simulator->dec = *dec;
    *simulator->lat = *lat;
    *simulator->freq = *freq;
    *simulator->wave = SPEED_OF_LIGHT / (*simulator->freq);
    *simulator->layout = *layout;
    *simulator->bas_len = *bas_len;
    *simulator->order = *order;
    *simulator->snr = *snr;
    *simulator->max_amp = *max_amp;
    *simulator->seed = *seed;
    simulator->rng = random_s(simulator);

    return simulator;
}

/**
 * This function initializes the simulator.
 * Set parameter to Null, to set default value.
 *  RETURNS: None
 *  INPUTS: None
*/
struct Simulator* initialize_simulator_default(){
    struct Simulator* simulator = newSimulatorStructure();

    if (simulator == NULL) {
        return NULL;
    }

    *simulator->h_min = DEFAULT_H_MIN;
    *simulator->h_max = DEFAULT_H_MAX;
    *simulator->n_steps = DEFAULT_N_STEPS;
    struct array* range = linspace_a(&(double){*simulator->h_min}, &(double){*simulator->h_max} , simulator->n_steps , &(enum DataType){DOUBLE});
    simulator->h_range = multiply_constant_a(range, &(double){M_PI/12}, &(enum DataType){DOUBLE});
    free_a(range);

    if (simulator->h_range == NULL) {
        free_s(simulator);
        return NULL;
    }

    *simulator->dec = DEFAULT_DEC;
    *simulator->lat = DEFAULT_LAT;
    *simulator->freq = DEFAULT_FREQ;
    *simulator->wave = SPEED_OF_LIGHT / (*simulator->freq);
    *simulator->layout = DEFAULT_LAYOUT;
    *simulator->bas_len = DEFAULT_BAS_LEN;
    *simulator->order = DEFAULT_ORDER;
    *simulator->snr = DEFAULT_SNR;
    *simulator->max_amp = DEFAULT_MAX_AMP;
    *simulator->seed = DEFAULT_SEED;
    simulator->rng = random_s(simulator);

    return simulator;
}

/**
 * This function frees the simulator.
 * This only frees the gsl vectors.
 *  RETURNS: None
 *  INPUTS: Simulator struct pointer
*/
void free_s(struct Simulator* simulator){
    if (simulator != NULL) {
        if (simulator->h_min != NULL) {
            free(simulator->h_min);
        }
        if (simulator->h_max != NULL) {
            free(simulator->h_max);
        }
        if (simulator->n_steps != NULL) {
            free(simulator->n_steps);
        }
        if (simulator->h_range != NULL) {
            free_a(simulator->h_range);
        }
        if (simulator->dec != NULL) {
            free(simulator->dec);
        }
        if (simulator->lat != NULL) {
            free(simulator->lat);
        }
        if (simulator->freq != NULL) {
            free(simulator->freq);
        }
        if (simulator->wave != NULL) {
            free(simulator->wave);
        }
        if (simulator->layout != NULL) {
            free(simulator->layout);
        }
        if (simulator->bas_len != NULL) {
            free(simulator->bas_len);
        }
        if (simulator->order != NULL) {
            free(simulator->order);
        }
        if (simulator->ant != NULL) {
            free_a(simulator->ant);
        }
        if (simulator->N != NULL) {
            free(simulator->N);
        }
        if (simulator->B != NULL) {
            free(simulator->B);
        }
        if (simulator->L != NULL) {
            free(simulator->L);
        }
        if (simulator->u_m != NULL) {
            free_a(simulator->u_m);
        }
        if (simulator->v_m != NULL) {
            free_a(simulator->v_m);
        }
        if (simulator->w_m != NULL) {
            free_a(simulator->w_m);
        }
        if (simulator->phi != NULL) {
            free_a(simulator->phi);
        }
        if (simulator->zeta != NULL) {
            free_a(simulator->zeta);
        }
        if (simulator->snr != NULL) {
            free(simulator->snr);
        }
        if (simulator->max_amp != NULL) {
            free(simulator->max_amp);
        }
        if (simulator->seed != NULL) {
            free(simulator->seed);
        }
        if (simulator->point_sources != NULL) {
            free_a(simulator->point_sources);
        }
        if (simulator->rng != NULL) {
            gsl_rng_free(simulator->rng);
        }
        if (simulator->antenna_gains != NULL) {
            free_a(simulator->antenna_gains);
        }
        if (simulator->M != NULL) {
            free_a(simulator->M);
        }
        if (simulator->D != NULL) {
            free_a(simulator->D);
        }
        free(simulator);
    }
}

/**
 *******************************************************************************************************************
 * ANTENNA LAYOUT AND UV-COVERAGE
 *******************************************************************************************************************
*/

/**
 * Generates an antenna layout either from a file or with a standard grid depending on the value of simulator->layout
 *  RETURNS: None
 *  INPUTS: None
*/
void generate_antenna_layout(struct Simulator* simulator) {
    if (*simulator->layout == HEX) {
        hex_grid(simulator);
    } else if (*simulator->layout == SQR) {
        square_grid(simulator);
    } else if (*simulator->layout == REG) {
        line_grid(simulator);
    } else {
        read_antenna_layout(simulator);
    }
}

/**
 * Generate an hexagonal layout
 * RETURN: None
 * INPUT: Simulator struct pointer
*/
void hex_grid(struct Simulator* simulator) {
    size_t hex_dim = *simulator->order;
    size_t side = hex_dim + 1;
    size_t ant_main_row = side + hex_dim;
    // size_t middel_row_length = side + hex_dim;

    size_t elements = 1;

    //Summing antennas in hexagonal rings
    for (size_t k = 0; k < hex_dim; k++) {
       elements = elements + (k+1)*6;
    }

    int middel = elements / 2;

    //Creating hexagonal layout starting from center
    struct array* ant_x = zeros_a(&(size_t){1}, (size_t[]){elements}, &(enum DataType){DOUBLE});
    struct array* ant_y = zeros_a(&(size_t){1}, (size_t[]){elements}, &(enum DataType){DOUBLE});

    double x = 0.0;
    double y = 0.0;
    size_t offset_1 = hex_dim;
    size_t offset_2 = hex_dim;

    for (size_t k = 0; k < side; k++) {
        double x_row = x;
        double y_row = y;

        if (k != 0) {
          offset_2 = offset_2 + ant_main_row;
        }
        
        
        for (size_t i = 0; i < ant_main_row; i++) {
            if (k == 0) {
                size_t index = middel - hex_dim + i;
                set_element_a(ant_x, (size_t[]){index}, &x_row);
                set_element_a(ant_y, (size_t[]){index}, &y_row);
                x_row = x_row + (*simulator->bas_len);
            } else {
                size_t index = middel + offset_1 + 1 + i;
                set_element_a(ant_x, (size_t[]){index}, &x_row);
                set_element_a(ant_y, (size_t[]){index}, &y_row);
                index = middel - offset_2 + i;
                set_element_a(ant_x, (size_t[]){index}, &x_row);
                double other_y = -1 * y_row;
                set_element_a(ant_y, (size_t[]){index}, &other_y);
                x_row = x_row + (*simulator->bas_len);
            }
        }

        if (k != 0) {
            offset_1 = offset_1 + ant_main_row;
        }

        x = x + (*simulator->bas_len) / 2.0;
        y = y + (sqrt(3)/2.0)* (*simulator->bas_len);
        ant_main_row = ant_main_row - 1;

    }

    double* max_x = (double*)max_a(ant_x);
    double constant = *max_x /2;
    struct array* origin_ant_x = minus_constant_a(ant_x, &constant, &(enum DataType){DOUBLE});

    struct array* temp_ant = zeros_a(&(size_t){2}, (size_t[]){origin_ant_x->dimensions[0], 3}, &(enum DataType){DOUBLE});

    copy_over_a(temp_ant, origin_ant_x, (int[]){-1,0}, (int[]){-1});
    copy_over_a(temp_ant, ant_y, (int[]){-1,1}, (int[]){-1});

    simulator->ant = temp_ant;

    free_a(ant_x);
    free_a(ant_y);
    free(max_x);
    free_a(origin_ant_x);
}

/**
 * Generate an square layout
 * RETURN: None
 * INPUT: Simulator struct pointer
*/
void square_grid(struct Simulator* simulator) {
    if (*simulator->order <= 1) {
        log_invalid_order(__FILE__, __LINE__);
    }

    size_t elements = (*simulator->order) * (*simulator->order);

    struct array* ant_x = zeros_a(&(size_t){1}, (size_t[]){elements}, &(enum DataType){DOUBLE});
    struct array* ant_y = zeros_a(&(size_t){1}, (size_t[]){elements}, &(enum DataType){DOUBLE});

    double x = 0.0;
    double y = 0.0;

    size_t counter = 0;

    for (size_t k = 0; k < *simulator->order; k++) {
        x = 0.0;
        for (size_t i = 0; i < *simulator->order; i++) {
            set_element_a(ant_x, (size_t[]){counter}, &x);
            set_element_a(ant_y, (size_t[]){counter}, &y);
            x = x + (*simulator->bas_len);
            counter = counter + 1;
        }
        y = y + (*simulator->bas_len);
    }

    //Shifting center of array to origin
    double* max_x = max_a(ant_x);
    double* max_y = max_a(ant_y);

    double constant_x = *max_x/2;
    double constant_y = *max_y/2;

    struct array* origin_ant_x = minus_constant_a(ant_x, &constant_x, &(enum DataType){DOUBLE});
    struct array* origin_ant_y = minus_constant_a(ant_y, &constant_y, &(enum DataType){DOUBLE});

    struct array* temp_ant = zeros_a(&(size_t){2}, (size_t[]){origin_ant_x->dimensions[0], 3}, &(enum DataType){DOUBLE});
    copy_over_a(temp_ant, origin_ant_x, (int[]){-1,0}, (int[]){-1});
    copy_over_a(temp_ant, origin_ant_y, (int[]){-1,1}, (int[]){-1});

    simulator->ant = temp_ant;

    free(max_x);
    free(max_y);
    free_a(ant_x);
    free_a(ant_y);
    free_a(origin_ant_x);
    free_a(origin_ant_y);
}

/**
 * Generate a regular EW layout
 * RETURN: None
 * INPUT: Simulator struct pointer
*/
void line_grid(struct Simulator* simulator) {
    if (*simulator->order <= 1) {
        log_invalid_order(__FILE__, __LINE__);
    }

    size_t elements = *simulator->order;

    struct array* ant_x = zeros_a(&(size_t){1}, (size_t[]){elements}, &(enum DataType){DOUBLE});
    struct array* ant_y = zeros_a(&(size_t){1}, (size_t[]){elements}, &(enum DataType){DOUBLE});

    double x = 0.0;

    for (size_t k = 0; k < elements; k++) {
        set_element_a(ant_x, (size_t[]){k}, &x);
        x = x + (*simulator->bas_len);
    }

    struct array* temp_ant = zeros_a(&(size_t){2}, (size_t[]){ant_x->dimensions[0], 3}, &(enum DataType){DOUBLE});
    copy_over_a(temp_ant, ant_x, (int[]){-1,0}, (int[]){-1});
    copy_over_a(temp_ant, ant_y, (int[]){-1,1}, (int[]){-1});

    simulator->ant = temp_ant;

    free_a(ant_x);
    free_a(ant_y);
}

/**
 * Reads the antenna layout from a text file
 * RETURN: None
 * INPUT: Simulator struct pointer
*/
void read_antenna_layout(struct Simulator* simulator) {
    log_not_implemented(__FILE__, __LINE__);
}


/**
 * Plot antenna layout
 * RETURN: PNG image
 * INPUT: Simulator struct pointer
*/
void plot_ant(struct Simulator* simulator) {
    // Write antenna coordinates to txt
    FILE *dataFile = fopen("data.txt", "w");
    if (*simulator->ant->num_dimensions == 2) {
        for (size_t i = 0; i < simulator->ant->dimensions[0]; i++) {
            fprintf(dataFile, "%lf %lf\n", *(double*)get_element_a(simulator->ant, (size_t[]){i,0}), *(double*)get_element_a(simulator->ant, (size_t[]){i,1}));
        }
    } else {
        log_incompatible_dimensions(__FILE__, __LINE__);
    }
    fclose(dataFile);

    char title[100];
    switch (*simulator->layout) {
        case HEX: {
            // snprintf(title, sizeof(title), "ENU coordinates of Hex antenna layout of size %ld", simulator->ant->dimensions[0]);
            snprintf(title, sizeof(title), "ENU coordinates of Hex antenna layout of size %ld. (KAT 7)", simulator->ant->dimensions[0]);
            break;
        }
        case SQR: {
            snprintf(title, sizeof(title), "ENU coordinates of Square antenna layout of size %ld", get_size_a(simulator->ant));
            break;
        }
        case REG: {
            snprintf(title, sizeof(title), "ENU coordinates of Regtangular antenna layout of size %ld", get_size_a(simulator->ant));
            break;
        }
        default: {
            log_invalid_layout(__FILE__, __LINE__);
        }
    }

    // Generate Gnuplot commands file
    FILE *commandsFile = fopen("commands.gplot", "w");
    fprintf(commandsFile, "set terminal png\n");
    fprintf(commandsFile, "set title '%s'\n", title);
    fprintf(commandsFile, "set output './figures/enu_coordinates_antenna.png'\n");
    fprintf(commandsFile, "set offsets graph 0.3, graph 0.3, graph 0.3, graph 0.3\n");
    fprintf(commandsFile, "set xlabel 'W-E [m]'\n");
    fprintf(commandsFile, "set ylabel 'S-N [m]'\n");
    fprintf(commandsFile, "set size square\n");
    fprintf(commandsFile, "plot 'data.txt' with points pt 7 ps 1.5 lc rgb 'blue' title 'Antenna Coordinates'\n");
    // fprintf(commandsFile, "pause -1 'Press Enter to exit'\n");
    fclose(commandsFile);

    // Run Gnuplot commands and check the return value
    int systemResult = system("gnuplot commands.gplot");

    // Cleanup
    remove("data.txt");
    remove("commands.gplot");

    // Check the return value
    if (systemResult == -1) {
        perror("Error running Gnuplot");
        exit(EXIT_FAILURE);
    } else if (WIFEXITED(systemResult) && WEXITSTATUS(systemResult) != 0) {
        fprintf(stderr, "Gnuplot command failed with exit status %d\n", WEXITSTATUS(systemResult));
        exit(EXIT_FAILURE);
    }
}

/**
 * Computes the uv-tracks of an array layout
 * RETURNS: 
 *  - None
 * INPUTS:
 *  - Simulator struct pointer
*/
void uv_tracks(struct Simulator* simulator) {
    *simulator->N = simulator->ant->dimensions[0];
    *simulator->B = (pow(*simulator->N, 2) - (*simulator->N))/2;
    

    struct array* DAE = zeros_a(&(size_t){2}, (size_t[]){*simulator->B, 3}, &(enum DataType){DOUBLE});
    struct array* p = zeros_a(&(size_t){1}, (size_t[]){*simulator->B}, &(enum DataType){INT});
    struct array* q = zeros_a(&(size_t){1}, (size_t[]){*simulator->B}, &(enum DataType){INT});
    
    size_t k = 0;
    for (size_t i = 0; i < *simulator->N; i++) {
        for (size_t j = i+1; j < *simulator->N; j++) {

            double distance = sqrt(pow((*(double*)get_element_a(simulator->ant, (size_t[]){i,0}) - *(double*)get_element_a(simulator->ant, (size_t[]){j,0})), 2) 
                                + pow((*(double*)get_element_a(simulator->ant, (size_t[]){i,1}) - *(double*)get_element_a(simulator->ant, (size_t[]){j,1})), 2) 
                                + pow((*(double*)get_element_a(simulator->ant, (size_t[]){i,2}) - *(double*)get_element_a(simulator->ant, (size_t[]){j,2})), 2));
            set_element_a(DAE, (size_t[]){k,0}, &distance);

            double azimith = atan2(*(double*)get_element_a(simulator->ant, (size_t[]){j,0}) - *(double*)get_element_a(simulator->ant, (size_t[]){i,0}), 
                                  *(double*)get_element_a(simulator->ant, (size_t[]){j,1}) - *(double*)get_element_a(simulator->ant, (size_t[]){i,1}));
            set_element_a(DAE, (size_t[]){k,1}, &azimith);

            double elevations = asin((*(double*)get_element_a(simulator->ant, (size_t[]){j,2}) - *(double*)get_element_a(simulator->ant, (size_t[]){i,2}))/ (*(double*)get_element_a(DAE, (size_t[]){k,0})));
            set_element_a(DAE, (size_t[]){k,2}, &elevations);

            set_element_a(p, (size_t[]){k}, &i);
            set_element_a(q, (size_t[]){k}, &j);

            k = k + 1;
        }
    }
    
    simulator->u_m = zeros_a(&(size_t){3}, (size_t[]){*simulator->N, *simulator->N, *simulator->n_steps}, &(enum DataType){DOUBLE});
    simulator->v_m = zeros_a(&(size_t){3}, (size_t[]){*simulator->N, *simulator->N, *simulator->n_steps}, &(enum DataType){DOUBLE});
    simulator->w_m = zeros_a(&(size_t){3}, (size_t[]){*simulator->N, *simulator->N, *simulator->n_steps}, &(enum DataType){DOUBLE});

    for (size_t i = 0; i < *simulator->B; i++) {
        struct array* uvw = zeros_a(&(size_t){2}, (size_t[]){get_size_a(simulator->h_range), 3}, &(enum DataType){DOUBLE});
        for (size_t j = 0; j < get_size_a(simulator->h_range); j++) {
            struct array* A = XYZ_to_uvw(get_element_a(simulator->h_range,&(size_t){j}), simulator->dec); //3x3
            struct array* xyz = DAE_to_XYZ(simulator, get_element_a(DAE, (size_t[]){i,0}), get_element_a(DAE, (size_t[]){i,1}), get_element_a(DAE, (size_t[]){i,2})); //3
            struct array* dot_product = dot_a(A, xyz);
            copy_over_a(uvw,dot_product, (int[]){j,-1}, (int[]){-1});
            free_a(A);
            free_a(xyz);
            free_a(dot_product);
        }
        struct array* minus_uvw = multiply_constant_a(uvw, &(double){-1.0}, &(enum DataType){DOUBLE});

        copy_over_a(simulator->u_m, uvw, (int[]){*(int*)get_element_a(p, (size_t[]){i}), *(int*)get_element_a(q, (size_t[]){i}), -1}, (int[]){-1, 0});
        copy_over_a(simulator->u_m, minus_uvw, (int[]){*(int*)get_element_a(q, (size_t[]){i}), *(int*)get_element_a(p, (size_t[]){i}), -1}, (int[]){-1, 0});
        copy_over_a(simulator->v_m, uvw, (int[]){*(int*)get_element_a(p, (size_t[]){i}), *(int*)get_element_a(q, (size_t[]){i}), -1}, (int[]){-1, 1});
        copy_over_a(simulator->v_m, minus_uvw, (int[]){*(int*)get_element_a(q, (size_t[]){i}), *(int*)get_element_a(p, (size_t[]){i}), -1}, (int[]){-1, 1});
        copy_over_a(simulator->w_m, uvw, (int[]){*(int*)get_element_a(p, (size_t[]){i}), *(int*)get_element_a(q, (size_t[]){i}), -1}, (int[]){-1, 2});
        copy_over_a(simulator->w_m, minus_uvw, (int[]){*(int*)get_element_a(q, (size_t[]){i}), *(int*)get_element_a(p, (size_t[]){i}), -1}, (int[]){-1, 2});

        free_a(uvw);
        free_a(minus_uvw);
    }

    free_a(DAE);
    free_a(p);
    free_a(q);
}

/**
 * Computes the rotation matrix A needed to convert XYZ into uvw at a specific hour angle
 * RETURNS:
 *  - A - rotation matrix capable of converting XYZ into uvw
 * INPUT:
 *  - h - a specific hour angel
 *  - delta - declinationof observation
*/
struct array* XYZ_to_uvw(double* h, double* delta) {
    struct array* A = zeros_a(&(size_t){2}, (size_t[]){3,3}, &(enum DataType){DOUBLE});

    set_element_a(A, (size_t[]){0,0}, &(double){sin(*h)});
    set_element_a(A, (size_t[]){0,1}, &(double){cos(*h)});
    set_element_a(A, (size_t[]){0,2}, &(double){0});
    set_element_a(A, (size_t[]){1,0}, &(double){-1 * sin(*delta) * cos(*h)});
    set_element_a(A, (size_t[]){1,1}, &(double){sin(*delta) * sin(*h)});
    set_element_a(A, (size_t[]){1,2}, &(double){cos(*delta)});
    set_element_a(A, (size_t[]){2,0}, &(double){cos(*delta) * cos(*h)});
    set_element_a(A, (size_t[]){2,1}, &(double){-1 * cos(*delta) * sin(*h)});
    set_element_a(A, (size_t[]){2,2}, &(double){sin(*delta)});

    return A;
}

/**
 * Converts baseline length, azimuth angle, elevation angle and latitude into XYZ coordinates
 * RETURNS:
 *  - XYZ - a vector of size 3 containing the XYZ coordinate of a specific baseline 
 *      #X - (0,0)
 *      #Y - (-6,0)
 *      #Z - NCP
 * INPUTS:
 *  - Simulator struct pointer
 *  - d - baseline lenght
 *  - az - azimuth angle of baseline
 *  - el - elevation angle of baseline
*/
struct array* DAE_to_XYZ(struct Simulator* simulator, double* d, double* az, double* el) {
    struct array* result = zeros_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){DOUBLE});
    set_element_a(result, (size_t[]){0}, &(double){*d * ((cos(*simulator->lat) * sin(*el)) - (sin(*simulator->lat) * cos(*el) * cos(*az))) / *simulator->wave});
    set_element_a(result, (size_t[]){1}, &(double){*d * (cos(*el) * sin(*az)) / *simulator->wave});
    set_element_a(result, (size_t[]){2}, &(double){*d * ((sin(*simulator->lat) * sin(*el)) + (cos(*simulator->lat) * cos(*el) * cos(*az))) / *simulator->wave});

    return result;
}

/**
 * Plots the uv-tracks of an array layout
 * RETURN: PNG image
 * INPUT: Simulator struct pointer
*/
void plot_uv_coverage(struct Simulator* simulator) {
    if (simulator->u_m == NULL || simulator->v_m == NULL || simulator->w_m == NULL) {
        log_null_array(__FILE__,__LINE__);
    }

    double* amax_x = (double*)amax_a(simulator->u_m);
    double* amax_y = (double*)amax_a(simulator->v_m);

    FILE *combinedDataFile = fopen("combined_data.txt", "w");

    for (size_t i = 0; i < *simulator->N; i++) {
        for (size_t j = i+1; j < *simulator->N; j++) {
            for (size_t h = 0; h < *simulator->n_steps; h++) {
                fprintf(combinedDataFile, "%lf %lf %d\n", *(double*)get_element_a(simulator->u_m, (size_t[]){i, j, h}), *(double*)get_element_a(simulator->v_m, (size_t[]){i, j, h}), 0);
                fprintf(combinedDataFile, "%lf %lf %d\n", *(double*)get_element_a(simulator->u_m, (size_t[]){j, i, h}), *(double*)get_element_a(simulator->v_m, (size_t[]){j, i, h}), 1);
            }
        }
    }
    fclose(combinedDataFile);

    char title[100];
    switch (*simulator->layout) {
        case HEX: {
            //snprintf(title, sizeof(title), "uv-tracks of Hex antenna layout of size %ld", simulator->ant->dimensions[0]);
            snprintf(title, sizeof(title), "uv-Coverage of KAT 7");
            break;
        }
        case SQR: {
            snprintf(title, sizeof(title), "uv-tracks of Square antenna layout of size %ld", get_size_a(simulator->ant));
            break;
        }
        case REG: {
            snprintf(title, sizeof(title), "uv-tracks of Regtangular antenna layout of size %ld", get_size_a(simulator->ant));
            break;
        }
        default: {
            log_invalid_layout(__FILE__, __LINE__);
        }
    }

    // Generate Gnuplot commands file
    FILE *commandsFile = fopen("commands.gplot", "w");
    fprintf(commandsFile, "set terminal png\n");
    fprintf(commandsFile, "set title '%s'\n", title);
    fprintf(commandsFile, "set output './figures/uv_tracks.png'\n");
    fprintf(commandsFile, "set offsets graph 0.3, graph 0.3, graph 0.3, graph 0.3\n");
    fprintf(commandsFile, "set yrange [%lf:%lf]\n", -1.1 * *amax_y, 1.1 * *amax_y);
    fprintf(commandsFile, "set xrange [%lf:%lf]\n", -1.1 * *amax_x, 1.1 * *amax_x);
    fprintf(commandsFile, "set xlabel 'u [ λ ]' font 'arial,10'\n");
    fprintf(commandsFile, "set ylabel 'v [ λ ]' font 'arial,10'\n");
    fprintf(commandsFile, "set size square\n");
    fprintf(commandsFile, "set palette defined (0 'blue', 1 'red')\n");
    fprintf(commandsFile, "unset colorbox\n");
    fprintf(commandsFile, "unset key\n");
    fprintf(commandsFile, "plot 'combined_data.txt' using 1:2:3 with points pt 7 ps 0.01 linecolor palette\n");
    fclose(commandsFile);

    // Run Gnuplot commands and check the return value
    int systemResult = system("gnuplot commands.gplot");

    free(amax_x);
    free(amax_y);

    // Cleanup
    remove("combined_data.txt");
    remove("commands.gplot");

    // Check the return value
    if (systemResult == -1) {
        perror("Error running Gnuplot");
        exit(EXIT_FAILURE);
    } else if (WIFEXITED(systemResult) && WEXITSTATUS(systemResult) != 0) {
        fprintf(stderr, "Gnuplot command failed with exit status %d\n", WEXITSTATUS(systemResult));
        exit(EXIT_FAILURE);
    }
}

/**
 * Creates a point sources array of dimension: num_sources x 3 
 * RETURNS:
 *  - None
 * INPUT:
 *  - Simulator struct simulator
 *  - num_sources - number of sources to create
 *  - a - Pareto parameter
 *  - fov = fov of sources
*/
void create_point_sources(struct Simulator* simulator, size_t* num_sources, double* fov, double* a) {
    //free the allready existing point sources to create new point sources.
    if (simulator->point_sources != NULL) {
        free_a(simulator->point_sources);
    }

    simulator->point_sources = zeros_a(&(size_t){2}, (size_t[]){*num_sources, 3}, &(enum DataType){DOUBLE});
    struct array* flux_values = generate_flux(simulator, a, num_sources);
    copy_over_a(simulator->point_sources, flux_values, (int[]){-1, 0}, (int[]){-1});
    struct array* pos_values_x = generate_pos(simulator, fov, num_sources);
    struct array* pos_values_y = generate_pos(simulator, fov, num_sources);
    copy_over_a(simulator->point_sources, pos_values_x, (int[]){-1, 1}, (int[]){-1});
    copy_over_a(simulator->point_sources, pos_values_y, (int[]){-1, 2}, (int[]){-1});

    free_a(flux_values);
    free_a(pos_values_x);
    free_a(pos_values_y);
}

/**
 * Generate flux values of sources (power law distribution)
 * RETURNS:
 *  - y - an array of pareto distributed samples of size num_sources
 * INPUT:
 *  - a - pareto distribution (power law) shape parameter.
 *  - num_sources - the amount of values to draw from the pareto distribution.
*/
struct array* generate_flux(struct Simulator* simulator, double* a, size_t* num_sources) {

    //Creating array
    struct array* flux_values = zeros_a(&(size_t){1}, (size_t[]){*num_sources}, &(enum DataType){DOUBLE});

    for (size_t i = 0; i < *num_sources; i++) {
        double flux = gsl_ran_pareto(simulator->rng, *a, 1.0);
        set_element_a(flux_values, (size_t[]){i}, &flux);
    }

    return flux_values;
}

/**
 * Generate position values of sources (uniform distribution)
 * RETURNS:
 *  - an array of uniformly distributed samples of size num_sources
 * INPUTS:
 *  - Simulator struct pointer
 *  - fov - the fov in which the sources reside (in degrees)
 *  - num_sources - the amount of values to draw from the uniform distribution.
*/
struct array* generate_pos(struct Simulator* simulator, double* fov, size_t* num_sources) {

    //Creating array
    struct array* pos_values = zeros_a(&(size_t){1}, (size_t[]){*num_sources}, &(enum DataType){DOUBLE});

    for (size_t i = 0; i < *num_sources; i++) {
        double pos = gsl_rng_uniform(simulator->rng) * (2 * fabs(*fov)) - fabs(*fov);
        pos = pos * M_PI/180;
        set_element_a(pos_values, (size_t[]){i}, &pos);
    }

    return pos_values;
}


/**
 * Creates a random number generator with a specific seed set in the simulator.
 * RETURNS:
 *  - gsl_rng pointer
 * INPUTS:
 *  - Simulator struct pointer
 * NOTE:
 *  - Remember to free random number generator with gsl_rng_free(rng)
*/
gsl_rng* random_s(struct Simulator* simulator) {

    const gsl_rng_type *rng_type;
    gsl_rng *rng;

    // Create a random number generator
    gsl_rng_env_setup();
    rng_type = gsl_rng_default;
    rng = gsl_rng_alloc(rng_type);

    // Set the seed for reproducibility (you may want to set a specific seed value)
    gsl_rng_set(rng, *simulator->seed);

    return rng;
}

/**
 * Setting the rng
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct Simulator pointer
*/
void set_rng_s(struct Simulator* simulator) {
    if (simulator->rng != NULL) {
        gsl_rng_free(simulator->rng);
    }

    simulator->rng = random_s(simulator);
}

/**
 * Generates the antenna gains via sinusoidal model
 * RETURNS:
 *  - None
 * INPUTS:
 *  - Simulator struct pointer
 *  - max - Maximum gain amplitude
 *  - freq_scale - How many complete periods in timesteps
*/
void create_antenna_gains(struct Simulator* simulator, double* freq_scale) {
    simulator->antenna_gains = zeros_a(&(size_t){2}, (size_t[]){*simulator->N, *simulator->n_steps}, &(enum DataType){COMPLEX});

    for (size_t i = 0; i < *simulator->n_steps; i++) {
        struct array* result = zeros_a(&(size_t){1}, (size_t[]){*simulator->N}, &(enum DataType){COMPLEX});
        for (size_t j = 0; j < *simulator->N; j++) {
            double amp_phase = gsl_ran_flat(simulator->rng, 1.0, *simulator->max_amp);
            double phase_phase = gsl_ran_flat(simulator->rng, 0, 2*M_PI);
            double complex result_complex = amp_phase * cexp(I * phase_phase);
            set_element_a(result, (size_t[]){j}, &result_complex);
        }
        copy_over_a(simulator->antenna_gains,result, (int[]){-1, i}, (int[]){-1});
        free_a(result);
    }
}

/**
 * Plotting the antenna gains amplitude
 * RETURNS:
 *  - None
 * INPUTS:
 *  - Simulator struct pointer
*/
void plot_antenna_gains_amplitude(struct Simulator* simulator) {
    FILE *combinedDataFile = fopen("combined_data.txt", "w");

    for (size_t i = 0; i < simulator->antenna_gains->dimensions[1]; i++) {
        for (size_t j = 0; j < simulator->antenna_gains->dimensions[0]; j++) {
            double complex complex_value = *(double complex*)get_element_a(simulator->antenna_gains, (size_t[]){j,i});
            double amplitude = cabs(complex_value);
            fprintf(combinedDataFile, "%ld %lf %ld\n", i, amplitude, j);
        }
    }
    fclose(combinedDataFile);

    char title[100];
    snprintf(title, sizeof(title), "Antenna Gains Amplitude");

        // Generate Gnuplot commands file
    FILE *commandsFile = fopen("commands.gplot", "w");
    fprintf(commandsFile, "set terminal png\n");
    fprintf(commandsFile, "set title '%s'\n", title);
    fprintf(commandsFile, "set output './figures/antenna_gains_amplitude.png'\n");
    fprintf(commandsFile, "set offsets graph 0.01, graph 0.01, graph 0.01, graph 0.01\n");
    fprintf(commandsFile, "set xlabel 'Time Slots' font 'arial,10'\n");
    fprintf(commandsFile, "set ylabel 'Amplitude |g|' font 'arial,10'\n");
    fprintf(commandsFile, "unset key\n");
    fprintf(commandsFile, "plot 'combined_data.txt' using 1:2:3 with points pt 7 ps 0.3 linecolor variable\n");
    fclose(commandsFile);

    // Run Gnuplot commands and check the return value
    int systemResult = system("gnuplot commands.gplot");

    // Cleanup
    remove("combined_data.txt");
    remove("commands.gplot");

    // Check the return value
    if (systemResult == -1) {
        perror("Error running Gnuplot");
        exit(EXIT_FAILURE);
    } else if (WIFEXITED(systemResult) && WEXITSTATUS(systemResult) != 0) {
        fprintf(stderr, "Gnuplot command failed with exit status %d\n", WEXITSTATUS(systemResult));
        exit(EXIT_FAILURE);
    }
}

/**
 * Plotting the antenna gains phase
 * RETURNS:
 *  - None
 * INPUTS:
 *  - Simulator struct pointer
*/
void plot_antenna_gains_phase(struct Simulator* simulator) {
    // printf("%f\n", 2*M_PI *(180/M_PI));
    FILE *combinedDataFile = fopen("combined_data.txt", "w");

    for (size_t i = 0; i < simulator->antenna_gains->dimensions[1]; i++) {
        for (size_t j = 0; j < simulator->antenna_gains->dimensions[0]; j++) {
            double complex complex_value = *(double complex*)get_element_a(simulator->antenna_gains, (size_t[]){j,i});
            double phase = carg(complex_value) * (180/M_PI);
            fprintf(combinedDataFile, "%ld %lf %ld\n", i, phase, j);
        }
    }
    fclose(combinedDataFile);

    char title[100];
    snprintf(title, sizeof(title), "Antenna Gains Phase");

        // Generate Gnuplot commands file
    FILE *commandsFile = fopen("commands.gplot", "w");
    fprintf(commandsFile, "set terminal png\n");
    fprintf(commandsFile, "set title '%s'\n", title);
    fprintf(commandsFile, "set output './figures/antenna_gains_phase.png'\n");
    fprintf(commandsFile, "set offsets graph 0.01, graph 0.01, graph 0.01, graph 0.01\n");
    fprintf(commandsFile, "set xlabel 'Time Slots' font 'arial,10'\n");
    fprintf(commandsFile, "set ylabel 'Phase (g)' font 'arial,10'\n");
    fprintf(commandsFile, "unset key\n");
    fprintf(commandsFile, "plot 'combined_data.txt' using 1:2:3 with points pt 7 ps 0.3 linecolor variable\n");
    fclose(commandsFile);

    // Run Gnuplot commands and check the return value
    int systemResult = system("gnuplot commands.gplot");

    // Cleanup
    remove("combined_data.txt");
    remove("commands.gplot");

    // Check the return value
    if (systemResult == -1) {
        perror("Error running Gnuplot");
        exit(EXIT_FAILURE);
    } else if (WIFEXITED(systemResult) && WEXITSTATUS(systemResult) != 0) {
        fprintf(stderr, "Gnuplot command failed with exit status %d\n", WEXITSTATUS(systemResult));
        exit(EXIT_FAILURE);
    }
}

/**
 *  Creates an observed visibility matrix
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - Simulator struct pointer
 *  - SNR - the signal to noise ratio in dB (gains are assumed to be signal)
 *  - w_m_include - boolean if it needs to be included for 3d simulation otherwise 2d simulation
*/
void create_vis_mat(struct Simulator* simulator, int* w_m_include, bool noise) {
    
    struct array* D = zeros_a(&(size_t){3}, (size_t[]){*simulator->N, *simulator->N, *simulator->n_steps}, &(enum DataType){COMPLEX});
    for (size_t k = 0; k < simulator->point_sources->dimensions[0]; k++) {
        double amp = *(double*)get_element_a(simulator->point_sources, (size_t[]){k,0});
        double l_0 = *(double*)get_element_a(simulator->point_sources, (size_t[]){k,1});
        double m_0 = *(double*)get_element_a(simulator->point_sources, (size_t[]){k,2});

        if (w_m_include == NULL) {
            double complex* d_arr = (double complex*)D->array;
            double* u_m_arr = (double*) simulator->u_m->array;
            double* v_m_arr = (double*) simulator->v_m->array;

            for (size_t i = 0; i < get_size_a(D); i++) {
                d_arr[i] = d_arr[i] + amp * cexp(-2 * M_PI * I * (u_m_arr[i] * l_0 + v_m_arr[i] * m_0));
            }
        } else {
            double complex* d_arr = (double complex*)D->array;
            double* u_m_arr = (double*) simulator->u_m->array;
            double* v_m_arr = (double*) simulator->v_m->array;
            double* w_m_arr = (double*) simulator->w_m->array;

            double n_0 = sqrt(1 - pow(l_0, 2) - pow(m_0, 2));

            for (size_t i = 0; i < get_size_a(D); i++) {
                d_arr[i] = d_arr[i] + amp * cexp(-2 * M_PI * I * (u_m_arr[i] * l_0 + v_m_arr[i] * m_0 + w_m_arr[i] * (n_0 - 1)));
            }
        }
    }

    // copy_over_a(D, simulator->M, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});
    

    if (noise) {
        
        // print_a(D);

        for (size_t t = 0; t < D->dimensions[2]; t++) {
            if (simulator->antenna_gains != NULL) {
                struct array* G = diag_a(simulator->antenna_gains, (int[]){-1, t});
                struct array* D_slice_1 = slice_a(D, (int[]){-1,-1,t});
                struct array* dot_product_1 = dot_a(G, D_slice_1);
                struct array* G_conj = conjugate_a(G);
                struct array* dot_product_2 = dot_a(dot_product_1, G_conj);
                copy_over_a(D, dot_product_2, (int[]){-1,-1,t}, (int[]){-1,-1});
                free_a(G);
                free_a(D_slice_1);
                free_a(dot_product_1);
                free_a(G_conj);
                free_a(dot_product_2);
            }
        }

        // printf("\n");
        // print_a(D);

        double* P_signal = det_power_of_signal(D);
        double* P_noise = power_needed_for_SNR(P_signal, simulator->snr);
        struct array* N = generate_noise(simulator, D, P_noise);
        struct array* D_noise = addition_a(D, N);
        //Freeing D in order to make it D_noise
        free_a(D);
        D = D_noise;
        free(P_signal);
        free(P_noise);
        free_a(N);

        if (simulator->D != NULL) {
            free_a(simulator->D);
        }

        simulator->D = D;
    } else {
        if (simulator->M != NULL) {
            free_a(simulator->M);
        }

        simulator->M = D;
    }

}

/**
 * Determines the power in signal
 * RETURNS:
 *  - array struct pointer = contains 
 *      {D_pow - The power in each baseline,
 *       d_pow1 - Average power over entire matrix,
 *       d_pow2 - Average of D_pow over baselines}
 * INPUTS:
 *  - D- Matrix to calculate the power
*/
double* det_power_of_signal(struct array* D) {
    if (*D->num_dimensions != 3) {
        log_invalid_dimensions(__FILE__, __LINE__);
    }

    // struct array* result = zeros_a(&(size_t){1}, (size_t[]){3}, &(enum DataType){DOUBLE});

    struct array* D_copy = zeros_a(D->num_dimensions, D->dimensions, D->type);
    copy_over_a(D_copy, D, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});

    
    struct array* ones_matrix = ones_a(&(size_t){2}, (size_t[]){D->dimensions[0], D->dimensions[1]}, &(enum DataType){DOUBLE});
    struct array* ones_arr = ones_a(&(size_t){1}, (size_t[]){D->dimensions[0]}, &(enum DataType){DOUBLE});
    struct array* ones_diag = diag_a(ones_arr, (int[]){-1});
    struct array* zero_diag = minus_a(ones_matrix, ones_diag);
    

    for (size_t t = 0; t < D_copy->dimensions[2]; t++) {
        struct array* D_slice = slice_a(D_copy, (int[]){-1,-1,t});
        struct array* D_slice_product = multiply_a(D_slice, zero_diag);
        copy_over_a(D_copy, D_slice_product, (int[]){-1,-1,t}, (int[]){-1,-1});
        free_a(D_slice);
        free_a(D_slice_product);
    }

    double bas = pow(D->dimensions[0], 2) - D->dimensions[0];

    //D_pow and d_pow1
    double* d_pow1 = malloc(sizeof(double));
    *d_pow1 = 0.0; 
    struct array* D_pow = zeros_a(&(size_t){2}, (size_t[]){D->dimensions[0], D->dimensions[1]}, &(enum DataType){DOUBLE});
    for (size_t i = 0; i < D->dimensions[0]; i ++) {
        for (size_t j = 0; j < D->dimensions[1]; j++) {
            double sum = 0.0;
            for (size_t h = 0; h < D->dimensions[2]; h++) {
                sum = sum + pow(cabs(*(double complex*)get_element_a(D_copy, (size_t[]){i,j,h})),2);

                *d_pow1 = *d_pow1 + pow(cabs(*(double complex*)get_element_a(D_copy, (size_t[]){i,j,h})),2);
            }
            double mean = sum / D->dimensions[2];
            set_element_a(D_pow, (size_t[]){i,j}, &(double){mean});
        }
    }

    //d_pow1
    *d_pow1 = *d_pow1 / (bas * D->dimensions[2]);
    
    //d_pow2
    // double d_pow2 = 0.0;
    // for (size_t i = 0; i < D->dimensions[0]; i ++) {
    //     for (size_t j = 0; j < D->dimensions[1]; j++) {
    //         d_pow2 = d_pow2 + *(double*)get_element_a(D_pow, (size_t[]){i,j});
    //     }
    // }
    // d_pow2 = d_pow2 / bas;

    free_a(ones_arr);
    free_a(ones_matrix);
    free_a(ones_diag);
    free_a(zero_diag);
    free_a(D_copy);
    free_a(D_pow);

    return d_pow1;
}


/**
 * How much power in the noise is needed to achieve SNR
 * RETURNS:
 *  - P_noise - Power in the noise
 * INPUTS:
 *  - P_signal - Power in the signal
 *  - SNR - SNR to achieve
*/
double* power_needed_for_SNR(double* P_signal, double* SNR) {
    double* P_noise = malloc(sizeof(double));
    *P_noise = 0;
    *P_noise = *P_signal * pow(10 , (-1 * (*SNR / 10.0)));
    return P_noise;
}

/**
 * Function that generates noise at a certain power level
 * http://dsp.stackexchange.com/questions/16216/adding-white-noise-to-complex-signal-complex-envelope
 * RETURNS:
 *  - noise mat
 * INPUT:
 *  - D - used to get dimensions
 *  - power - power of the noise to generate
*/
struct array* generate_noise(struct Simulator* simulator, struct array* D, double* power) {
    double sig = sqrt(*power / 2);
    struct array* mat = zeros_a(D->num_dimensions, D->dimensions, D->type);
    for (size_t i = 0; i < D->dimensions[0]; i++) {
        for (size_t j = i+1; j < D->dimensions[1]; j++) {
            for (size_t h = 0; h < D->dimensions[2]; h++) {
                double random1 = gsl_rng_uniform(simulator->rng);
                double random2 = gsl_rng_uniform(simulator->rng);

                double complex rand_complex = (sig * random1) + (sig * random2 * I);
                double complex rand_complex_conj = conj(rand_complex);

                set_element_a(mat, (size_t[]){i,j,h}, &rand_complex);
                set_element_a(mat, (size_t[]){j,i,h}, &rand_complex_conj);
            }
        }
    }

    return mat;
}


/**
 * Plot the real visibilities
 * RETURNS:
 *  - Makes .png image
 * INPUTS:
 *  - simulator - Simulator struct pointer
 *  - b_0 - first value of the baseline
 *  - b_1 - second value of the baseline
*/
void plot_visibilities(struct Simulator* simulator, size_t* b_0, size_t* b_1) {
    if (*simulator->M->type != *simulator->D->type) {
        log_incompatible_types(__FILE__, __LINE__);
    }
    if (*simulator->M->num_dimensions != *simulator->D->num_dimensions) {
        log_incompatible_size(__FILE__, __LINE__);
    }
    if (simulator->M->dimensions[0] != simulator->D->dimensions[0] || simulator->M->dimensions[1] != simulator->D->dimensions[1] || simulator->M->dimensions[2] != simulator->D->dimensions[2]) {
        log_incompatible_dimensions(__FILE__, __LINE__);
    }

    FILE *combinedDataFile = fopen("combined_data.txt", "w");

    for (int h = 0; h < simulator->M->dimensions[2]; h++) {
        double complex complex_value_M = *(double complex*)get_element_a(simulator->M, (size_t[]){*b_0,*b_1,h});
        double complex complex_value_D = *(double complex*)get_element_a(simulator->D, (size_t[]){*b_0,*b_1,h});
        fprintf(combinedDataFile, "%d %lf %d\n", h, creal(complex_value_M), 0);
        fprintf(combinedDataFile, "%d %lf %d\n", h, creal(complex_value_D), 1);
    }

    fclose(combinedDataFile);

    char title[100];
    snprintf(title, sizeof(title), "Visibilities for baseline %ld to %ld",*b_0, *b_1);

    // Generate Gnuplot commands file
    FILE *commandsFile = fopen("commands.gplot", "w");
    fprintf(commandsFile, "set terminal png\n");
    fprintf(commandsFile, "set title '%s'\n", title);
    fprintf(commandsFile, "set output './figures/visibilities_baseline.png'\n");
    fprintf(commandsFile, "set offsets graph 0.01, graph 0.01, graph 0.01, graph 0.01\n");
    fprintf(commandsFile, "set xlabel 'Time Slots' font 'arial,10'\n");
    fprintf(commandsFile, "set ylabel 'Jy' font 'arial,10'\n");
    fprintf(commandsFile, "set palette defined (0 'blue', 1 'red')\n");
    fprintf(commandsFile, "unset colorbox\n");
    fprintf(commandsFile, "unset key\n");
    fprintf(commandsFile, "plot 'combined_data.txt' using 1:($3 == 0 ? $2 : 1/0) with points pt 7 ps 1 lc rgb 'blue', \\\n");
    fprintf(commandsFile, "     'combined_data.txt' using 1:($3 == 1 ? $2 : 1/0) with points pt 7 ps 0.5 lc rgb 'red'\n");
    fclose(commandsFile);

    // Run Gnuplot commands and check the return value
    int systemResult = system("gnuplot commands.gplot");

    // Cleanup
    remove("combined_data.txt");
    remove("commands.gplot");

    // Check the return value
    if (systemResult == -1) {
        perror("Error running Gnuplot");
        exit(EXIT_FAILURE);
    } else if (WIFEXITED(systemResult) && WEXITSTATUS(systemResult) != 0) {
        fprintf(stderr, "Gnuplot command failed with exit status %d\n", WEXITSTATUS(systemResult));
        exit(EXIT_FAILURE);
    }
}

/**
 * save simulator to file
*/
void save_to_file_s(struct Simulator* simulator, char* directory) {
    char filename [256];
    snprintf(filename, sizeof(filename), "sim_seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf", *simulator->seed, *simulator->order, *simulator->snr, *simulator->max_amp);
    char full_file_path[512];
    snprintf(full_file_path, sizeof(full_file_path), "%s/%s.sconfig", directory, filename);
    char full_directory_path[512];
    snprintf(full_directory_path, sizeof(full_directory_path), "%s/%s", directory, filename);

    printf("%s\n", full_directory_path);

    //check if this simulator allready exists
    //create the directory where to store information about simulator
    if (check_file_path(full_file_path)) {
        log_file_already_exists(__FILE__, __LINE__);
    } else {
        create_directory(full_directory_path);
    }

    FILE *combinedDataFile = fopen(full_file_path, "w");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return;
    }

    fprintf(combinedDataFile, "%lf\n", *simulator->h_min);
    fprintf(combinedDataFile, "%lf\n", *simulator->h_max);
    fprintf(combinedDataFile, "%zu\n", *simulator->n_steps);
    save_to_file_a(simulator->h_range, full_directory_path, "h_range");
    fprintf(combinedDataFile, "%lf\n", *simulator->dec);
    fprintf(combinedDataFile, "%lf\n", *simulator->lat);
    fprintf(combinedDataFile, "%lf\n", *simulator->freq);
    fprintf(combinedDataFile, "%lf\n", *simulator->wave);
    int layout_type = *simulator->layout;
    fprintf(combinedDataFile, "%d\n", layout_type);
    fprintf(combinedDataFile, "%lf\n", *simulator->bas_len);
    fprintf(combinedDataFile, "%d\n", *simulator->order);
    save_to_file_a(simulator->ant, full_directory_path, "ant");
    fprintf(combinedDataFile, "%zu\n", *simulator->N);
    fprintf(combinedDataFile, "%zu\n", *simulator->B);
    fprintf(combinedDataFile, "%zu\n", *simulator->L);
    save_to_file_a(simulator->u_m, full_directory_path, "u_m");
    save_to_file_a(simulator->v_m, full_directory_path, "v_m");
    save_to_file_a(simulator->w_m, full_directory_path, "w_m");
    save_to_file_a(simulator->phi, full_directory_path, "phi");
    save_to_file_a(simulator->zeta, full_directory_path, "zeta");
    fprintf(combinedDataFile, "%lf\n", *simulator->snr);
    fprintf(combinedDataFile, "%lf\n", *simulator->max_amp);
    fprintf(combinedDataFile, "%u\n", *simulator->seed);
    save_to_file_a(simulator->point_sources, full_directory_path, "point_sources");
    save_to_file_a(simulator->antenna_gains, full_directory_path, "antenna_gains");
    save_to_file_a(simulator->M, full_directory_path, "M");
    save_to_file_a(simulator->D, full_directory_path, "D");

    fclose(combinedDataFile);
}

/**
 * Read simulator from file
*/
struct Simulator* read_from_file_s(char* filepath) {
    if(!ends_with_string(filepath, ".sconfig")) {
        log_invalid_file_extension(__FILE__,__LINE__);
    }

    FILE *combinedDataFile = fopen(filepath, "r");
    if (!combinedDataFile) {
        log_error_opening_file(__FILE__, __LINE__);
        return NULL;
    }

    char* sim_directory = remove_extension_from_filepath(filepath);
    if (!check_file_path(sim_directory)) {
        free(sim_directory);
        log_invalid_directory(__FILE__, __LINE__);
    }

    int sim_directory_length = strlen(sim_directory);

    double h_min = 0;
    double h_max = 0;
    size_t n_steps = 0;
    struct array* h_range = NULL;
    double dec = 0;
    double lat = 0;
    double freq = 0;
    double wave = 0;
    int layout_type_int = 0;
    double bas_len = 0;
    int order = 0;
    struct array* ant = NULL;
    size_t N = 0;
    size_t B = 0;
    size_t L = 0;
    struct array* u_m = NULL;
    struct array* v_m = NULL;
    struct array* w_m = NULL;
    struct array* phi = NULL;
    struct array* zeta = NULL;
    double snr = 0;
    double max_amp = 0;
    int seed = 0;
    struct array* point_sources = NULL;
    struct array* antenna_gains = NULL;
    struct array* M = NULL;
    struct array* D = NULL;

    if (fscanf(combinedDataFile, "%lf", &h_min) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &h_max) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    char h_range_file_path [sim_directory_length + 100];
    snprintf(h_range_file_path, sizeof(h_range_file_path), "%s/%s", sim_directory, "h_range.array");
    if (check_file_path(h_range_file_path)) {
        h_range = read_from_file_a(h_range_file_path);
    } else {
        // log_invalid_directory(__FILE__, __LINE__);
        printf("h_range not availible.\n");
    }
    if (fscanf(combinedDataFile, "%zu", &n_steps) != 1) {
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
    if (fscanf(combinedDataFile, "%lf", &wave) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%d", &layout_type_int) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%lf", &bas_len) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%d", &order) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    //read ant array
    char ant_file_path [sim_directory_length + 100];
    snprintf(ant_file_path, sizeof(ant_file_path), "%s/%s", sim_directory, "ant.array");
    if (check_file_path(ant_file_path)) {
        ant = read_from_file_a(ant_file_path);
    } else {
        // log_invalid_directory(__FILE__, __LINE__);
        printf("ant not availible.\n");
    }
    if (fscanf(combinedDataFile, "%zu", &N) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%zu", &B) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    if (fscanf(combinedDataFile, "%zu", &L) != 1) {
        log_error_fscan(__FILE__,__LINE__);
    }
    char u_m_file_path [sim_directory_length + 100];
    snprintf(u_m_file_path, sizeof(u_m_file_path), "%s/%s", sim_directory, "u_m.array");
    if (check_file_path(u_m_file_path)) {
        u_m = read_from_file_a(u_m_file_path);
    } else {
        // log_invalid_directory(__FILE__, __LINE__);
        printf("u_m not availible.\n");
    }

    char v_m_file_path [sim_directory_length + 100];
    snprintf(v_m_file_path, sizeof(v_m_file_path), "%s/%s", sim_directory, "v_m.array");
    if (check_file_path(v_m_file_path)) {
        v_m = read_from_file_a(v_m_file_path);
    } else {
        // log_invalid_directory(__FILE__, __LINE__);
        printf("v_m not availible.\n");
    }

    char w_m_file_path [sim_directory_length + 100];
    snprintf(w_m_file_path, sizeof(w_m_file_path), "%s/%s", sim_directory, "w_m.array");
    if (check_file_path(w_m_file_path)) {
        w_m = read_from_file_a(w_m_file_path);
    } else {
        // log_invalid_directory(__FILE__, __LINE__);
        printf("w_m not availible.\n");
    }

    char phi_file_path [sim_directory_length + 100];
    snprintf(phi_file_path, sizeof(phi_file_path), "%s/%s", sim_directory, "phi.array");
    if (check_file_path(phi_file_path)) {
        phi = read_from_file_a(phi_file_path);
    } else {
        // log_invalid_directory(__FILE__, __LINE__);
        // printf("phi not availible.\n");
    }

    char zeta_file_path [sim_directory_length + 100];
    snprintf(zeta_file_path, sizeof(zeta_file_path), "%s/%s", sim_directory, "zeta.array");
    if (check_file_path(zeta_file_path)) {
        zeta = read_from_file_a(zeta_file_path);
    } else {
        // log_invalid_directory(__FILE__, __LINE__);
        // printf("zeta not availible.\n");
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

    char point_sources_file_path [sim_directory_length + 100];
    snprintf(point_sources_file_path, sizeof(point_sources_file_path), "%s/%s", sim_directory, "point_sources.array");
    if (check_file_path(point_sources_file_path)) {
        point_sources = read_from_file_a(point_sources_file_path);
    } else {
        // log_invalid_directory(__FILE__, __LINE__);
        printf("point_sources not availible.\n");
    }

    char antenna_gains_file_path [sim_directory_length + 100];
    snprintf(antenna_gains_file_path, sizeof(antenna_gains_file_path), "%s/%s", sim_directory, "antenna_gains.array");
    if (check_file_path(antenna_gains_file_path)) {
        antenna_gains = read_from_file_a(antenna_gains_file_path);
    } else {
        // log_invalid_directory(__FILE__, __LINE__);
        printf("antenna_gains not availible.\n");
    }

    char M_file_path [sim_directory_length + 100];
    snprintf(M_file_path, sizeof(M_file_path), "%s/%s", sim_directory, "M.array");
    if (check_file_path(M_file_path)) {
        M = read_from_file_a(M_file_path);
    } else {
        // log_invalid_directory(__FILE__, __LINE__);
        printf("M not availible.\n");
    }

    char D_file_path [sim_directory_length + 100];
    snprintf(D_file_path, sizeof(D_file_path), "%s/%s", sim_directory, "D.array");
    if (check_file_path(D_file_path)) {
        D = read_from_file_a(D_file_path);
    } else {
        // log_invalid_directory(__FILE__, __LINE__);
        printf("D not availible.\n");
    }

    free(sim_directory);

    enum LayoutType layout = layout_type_int;
    struct Simulator* simulator = initialize_simulator(&h_min, &h_max, &dec, &lat, &freq, &layout, &n_steps, &bas_len, &order, &snr, &max_amp, &seed);

    free_a(simulator->h_range);
    simulator->h_range = h_range;
    *simulator->wave = wave;
    simulator->ant = ant;
    *simulator->N = N;
    *simulator->B = B;
    *simulator->L = L;
    simulator->u_m = u_m;
    simulator->v_m = v_m;
    simulator->w_m = w_m;
    simulator->phi = phi;
    simulator->zeta = zeta;
    simulator->point_sources = point_sources;
    simulator->antenna_gains = antenna_gains;
    simulator->M = M;
    simulator->D = D;

    fclose(combinedDataFile);

    return simulator;
}