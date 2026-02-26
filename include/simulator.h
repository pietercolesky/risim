/**
  * @file simulator.h
  * @description A definition of functions offered by the simulator
  */

#include "array.h"
#include "utils.h"
#include <gsl/gsl_rng.h>
#include <stdbool.h>

#ifndef _SIMULATOR_H
#define _SIMULATOR_H

/**
 * Simulator Struct 
*/
struct Simulator {
    double* h_min;
    double* h_max;
    size_t* n_steps;
    struct array* h_range;
    double* dec;
    double* lat;
    double* freq;
    double* wave;
    enum LayoutType* layout;
    double* bas_len;
    int* order;
    struct array* ant;
    size_t* N;
    size_t* B;
    size_t* L;
    struct array* u_m;
    struct array* v_m;
    struct array* w_m;
    struct array* phi;
    struct array* zeta;
    double* snr;
    double* max_amp;
    unsigned int* seed;
    struct array* point_sources;
    gsl_rng* rng;
    struct array* antenna_gains;
    struct array* M;
    struct array* D;
};

/**
 * Create a simulator
 * RETURN: pointer to simulator struct that is empty
*/
struct Simulator* newSimulatorStructure();

/**
 * Initializing similator with own parameters +
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
struct Simulator* initialize_simulator(double* h_min, double* h_max, double* dec, double* lat, double* freq, enum LayoutType* layout, size_t* n_steps, double* bas_len, int* order, double* snr, double* max_amp, int* seed);

/**
 * This function initializes the simulator. +
 * Set parameter to Null, to set default value.
 *  RETURNS: None
 *  INPUTS: None
*/
struct Simulator* initialize_simulator_default();

/**
 * This function frees the simulator. +
 * This only frees the gsl vectors.
 *  RETURNS: None
 *  INPUTS: Simulator struct pointer
*/
void free_s(struct Simulator* simulator);

/**
 * Generates an antenna layout either from a file or with a standard grid depending on the value of simulator->layout +
 *  RETURNS: None
 *  INPUTS: None
*/
void generate_antenna_layout(struct Simulator* simulator);

/**
 * Generate an hexagonal layout +
 * RETURN: None
 * INPUT: Simulator struct pointer
*/
void hex_grid(struct Simulator* simulator);

/**
 * Generate an square layout +
 * RETURN: None
 * INPUT: Simulator struct pointer
*/
void square_grid(struct Simulator* simulator);

/**
 * Generate a regular EW layout +
 * RETURN: None
 * INPUT: Simulator struct pointer
*/
void line_grid(struct Simulator* simulator);

/**
 * Reads the antenna layout from a text file
 * RETURN: None
 * INPUT: Simulator struct pointer
*/
void read_antenna_layout(struct Simulator* simulator);

/**
 * Plot antenna layout  +
 * RETURN: PNG image
 * INPUT: Simulator struct pointer
*/
void plot_ant(struct Simulator* simulator); 

/**
 * Computes the uv-tracks of an array layout  +
 * RETURNS: 
 *  - None
 * INPUTS:
 *  - Simulator struct pointer
*/
void uv_tracks(struct Simulator* simulator);

/**
 * Computes the rotation matrix A needed to convert XYZ into uvw at a specific hour angle +
 * RETURNS:
 *  - A - rotation matrix capable of converting XYZ into uvw
 * INPUT:
 *  - h - a specific hour angel
 *  - delta - declinationof observation
*/

struct array* XYZ_to_uvw(double* h, double* delta);

/**
 * Converts baseline length, azimuth angle, elevation angle and latitude into XYZ coordinates +
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
struct array* DAE_to_XYZ(struct Simulator* simulator, double* d, double* az, double* el);

/**
 * Plots the uv-tracks of an array layout +
 * RETURN: PNG image
 * INPUT: Simulator struct pointer
*/
void plot_uv_coverage(struct Simulator* simulator);

/**
 * Creates a point sources array of dimension: num_sources x 3 +
 * RETURNS:
 *  - None
 * INPUT:
 *  - Simulator struct simulator
 *  - num_sources - number of sources to create
 *  - a - Pareto parameter
 *  - fov = fov of sources
*/
void create_point_sources(struct Simulator* simulator, size_t* num_sources, double* fov, double* a);

/**
 * Generate flux values of sources (power law distribution)
 * RETURNS:
 *  - y - an array of pareto distributed samples of size num_sources
 * INPUT:
 *  - a - pareto distribution (power law) shape parameter.
 *  - num_sources - the amount of values to draw from the pareto distribution.
*/
struct array* generate_flux(struct Simulator* simulator, double* a, size_t* num_sources);

/**
 * Generate position values of sources (uniform distribution)
 * RETURNS:
 *  - an array of uniformly distributed samples of size num_sources
 * INPUTS:
 *  - Simulator struct pointer
 *  - fov - the fov in which the sources reside (in degrees)
 *  - num_sources - the amount of values to draw from the uniform distribution.
*/
struct array* generate_pos(struct Simulator* simulator, double* fov, size_t* num_sources);

/**
 * Creates a random number generator with a specific seed set in the simulator.
 * RETURNS:
 *  - gsl_rng pointer
 * INPUTS:
 *  - Simulator struct pointer
 * NOTE:
 *  - Remember to free random number generator with gsl_rng_free(rng)
*/
gsl_rng* random_s(struct Simulator* simulator);

/**
 * Setting the rng
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct Simulator pointer
*/
void set_rng_s(struct Simulator* simulator);

/**
 * Generates the antenna gains via sinusoidal model
 * RETURNS:
 *  - None
 * INPUTS:
 *  - Simulator struct pointer
 *  - freq_scale - How many complete periods in timesteps
*/
void create_antenna_gains(struct Simulator* simulator, double* freq_scale);

/**
 * Plotting the antenna gains amplitude
 * RETURNS:
 *  - None
 * INPUTS:
 *  - Simulator struct pointer
*/
void plot_antenna_gains_amplitude(struct Simulator* simulator);

/**
 * Plotting the antenna gains phase
 * RETURNS:
 *  - None
 * INPUTS:
 *  - Simulator struct pointer
*/
void plot_antenna_gains_phase(struct Simulator* simulator);

/**
 *  Creates an observed visibility matrix
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - Simulator struct pointer
 *  - SNR - the signal to noise ratio in dB (gains are assumed to be signal)
 *  - w_m_include - boolean if it needs to be included for 3d simulation otherwise 2d simulation
*/
void create_vis_mat(struct Simulator* simulator, int* w_m_include, bool noise);

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
double* det_power_of_signal(struct array* D) ;

/**
 * How much power in the noise is needed to achieve SNR
 * RETURNS:
 *  - P_noise - Power in the noise
 * INPUTS:
 *  - P_signal - Power in the signal
 *  - SNR - SNR to achieve
*/
double* power_needed_for_SNR(double* P_signal, double* SNR);

/**
 * Function that generates noise at a certain power level
 * http://dsp.stackexchange.com/questions/16216/adding-white-noise-to-complex-signal-complex-envelope
 * RETURNS:
 *  - noise mat
 * INPUT:
 *  - D - used to get dimensions
 *  - power - power of the noise to generate
*/
struct array* generate_noise(struct Simulator* simulator, struct array* D, double* power);

/**
 * Plot the real visibilities
 * RETURNS:
 *  - Makes .png image
 * INPUTS:
 *  - simulator - Simulator struct pointer
 *  - b_0 - first value of the baseline
 *  - b_1 - second value of the baseline
*/
void plot_visibilities(struct Simulator* simulator, size_t* b_0, size_t* b_1);

/**
 * save simulator to file
*/
void save_to_file_s(struct Simulator* simulator, char* directory);

/**
 * Read simulator from file
*/
struct Simulator* read_from_file_s(char* filepath);

#endif