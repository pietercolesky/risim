/**
  * @file calibrator.h
  * @description A definition of functions offered by the calibrator
  */

#include "array.h"
#include "utils.h"
#include "simulator.h"
#include "datacollector.h"
#include "thread_args.h"

#ifndef _CALIBRATOR_H
#define _CALIBRATOR_H

/**
 *Calibrator struct
 *This struct can be used to calibrate visibilities.
 *The visibilities can be calibrated with different methods.
*/
struct Calibrator {
    struct array* R_c;
    struct array* D;
    struct array* M;
    size_t* N;
    size_t* B;
    struct array* G;
    size_t* n_steps;
    struct DataCollector* datacollector;
    int* parallel;
    int* num_workers;
    struct array* random_timesteps;
    struct array* random_timesteps_reduced;
    struct array* random_timesteps_reduced_full;
};

/**
 * The creates a new calibrator
 * RETUNS:
 *  - Calibrator struct pointer
 * INPUTS:
 *  - simulator - Simulator struct pointer
*/
struct Calibrator* new_c(struct Simulator* simulator);

/**
 * Free Calibrator struct
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - Calibrator struct pointer
*/
void free_c(struct Calibrator* calibrator);

/**
 * Lets Calibrator run in parallel where you specify how many workers to use
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - NUM_WORKERS
*/
void set_parallel_c(struct Calibrator* calibrator, int* num_workers);

/**
 * Computing Jacobian for complex domain for calibration problem
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - Calibrator struct pointer
 *  - M_t - time step t of M
 *  - g - antenna gain array
*/
struct array* compute_J(struct Calibrator* calibrator, struct array* M_t, struct array* g);

/**
 * Computing Residual for complex domain for calibration problem
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - Calibrator struct pointer
 *  - M_t - time step t of M
 *  - D_t - time step t of D
 *  - g - antenna gain array
*/
struct array* compute_r(struct Calibrator* calibrator, struct array* M_t, struct array* D_t, struct array* g);

/**
 * Computing Residual for complex domain for calibration problem
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - M_t - time step t of M
 *  - D_t - time step t of D
 *  - g - antenna gain array
*/
struct array* compute_r_N(struct array* M_t, struct array* D_t, struct array* g);

/**
 * Correcting visibilities
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - Calibtrator struct pointer
*/
void correct_visibilities(struct Calibrator* calibrator);

/**
 * Levenberg Marquardt
 * RETURNS:
 *  - NONE
 * INPUT:
 *  - imax maximum amount of iterations.
 *  - tau stopping criteria.
 *  - l - 
 *  - v -
*/
void solve_G_LM(struct Calibrator* calibrator, size_t* imax, double* tua, double* l_init, double* v);

/**
 * Worker used to parallelize function solve_G_LM
*/
void* worker_solve_G_LM(void* args);

/**
 * This function finds argmin G ||R-GMG^H|| using StEFCal.
 * D is your observed visibilities matrx. This is saved when the object is made
 * M is your predicted visibilities. This is saved when the object is made
 * RETURNS:
 *  - NONE
 * INPUT:
 *  - imax maximum amount of iterations.
 *  - tau stopping criteria.
*/
void solve_G_StEFCal(struct Calibrator* calibrator, size_t* imax, double* tua);

/**
 * Worker used to parallelize function solve_G_StEFCal
*/
void* worker_solve_G_StEFCal(void* args);

/**
 * Plot the real corrected visibilities
 * RETURNS:
 *  - Makes .png image
 * INPUTS:
 *  - simulator - Simulator struct pointer
 *  - b_0 - first value of the baseline
 *  - b_1 - second value of the baseline
*/
void plot_corrected_visibilities_c(struct Calibrator* calibrator, size_t* b_0, size_t* b_1);

/**
 * This will add data to datacollector
*/
void add_data_dc(struct Calibrator* calibrator, struct array* M_t, struct array* D_t, struct array* g, size_t* time_step, size_t* iterations);

/**
 * Levenberg Marquardt REDUCED
 * RETURNS:
 *  - NONE
 * INPUT:
 *  - imax maximum amount of iterations.
 *  - tau stopping criteria.
 *  - l - 
 *  - v -
*/
void solve_G_LM_REDUCED_TIMESTEPS(struct Calibrator* calibrator, size_t* imax, double* tua, double* l_init, double* v);

/**
 * Worker used to parallelize function solve_G_LM
*/
void* worker_solve_G_LM_REDUCED_TIMESTEPS(void* args);

#endif