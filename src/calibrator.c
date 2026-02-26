/**
  * @file calibrator.c
  * This is the implementation of the functions of calibrator.h
  */

#include <stdio.h>
#include <stdlib.h>
#include "calibrator.h"
#include "array.h"
#include "simulator.h"
#include "progress_bar.h"
#include "logger.h"
#include "thread_args.h"
#include "constants.h"
#include <sys/wait.h>
#include <math.h>

/**
 * The creates a new calibrator
 * RETUNS:
 *  - Calibrator struct pointer
 * INPUTS:
 *  - simulator - Simulator struct pointer
*/
struct Calibrator* new_c(struct Simulator* simulator) {
    struct Calibrator* calibrator = malloc(sizeof(struct Calibrator));

    if (calibrator == NULL) {
        return NULL;
    }
    calibrator->R_c = NULL;
    calibrator->D = zeros_a(simulator->D->num_dimensions, simulator->D->dimensions, simulator->D->type);
    calibrator->M = zeros_a(simulator->M->num_dimensions, simulator->M->dimensions, simulator->M->type);
    calibrator->N = malloc(sizeof(size_t));
    calibrator->B = malloc(sizeof(size_t));
    calibrator->n_steps = malloc(sizeof(size_t));
    calibrator->G = NULL;
    calibrator->datacollector = NULL;
    calibrator->parallel = malloc(sizeof(int));
    calibrator->num_workers = malloc(sizeof(int));

    *calibrator->N = simulator->D->dimensions[0];
    *calibrator->B = (*calibrator->N * (*calibrator->N -1) / 2);
    *calibrator->n_steps = simulator->D->dimensions[2];
    copy_over_a(calibrator->D, simulator->D, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});
    copy_over_a(calibrator->M, simulator->M, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});
    *calibrator->parallel = 0;
    *calibrator->num_workers = 0;

    calibrator->random_timesteps = new_set_a(&(size_t){1},
                                    (size_t[]){RANDOM_TIMESTEP_AMOUNT}, 
                                    &(enum DataType){INT}, 
                                    (int[RANDOM_TIMESTEP_AMOUNT]){
                                        24, 26, 29, 31, 43, 74, 84, 105, 116, 131, 170, 230, 237, 256, 274, 278, 279, 293, 296, 305, 319, 329, 332, 347, 434, 437, 454, 472, 558, 576
                                    });

    calibrator->random_timesteps_reduced_full = new_set_a(&(size_t){1},
                                                (size_t[]){600}, 
                                                &(enum DataType){INT}, 
                                                (int[600]){
                                                    177, 442, 53, 46, 435, 594, 380, 268, 153, 159, 384, 580, 502, 178, 305, 408, 147, 237, 54, 22, 227, 153, 63, 466, 342, 270, 179, 377, 582, 229, 582, 188, 216, 29, 303, 520, 41, 147, 285, 596, 213, 144, 573, 226, 503, 163, 149, 144, 363, 264, 442, 377, 542, 444, 128, 339, 413, 362, 5, 402, 99, 58, 97, 128, 368, 492, 479, 387, 447, 431, 367, 264, 178, 107, 155, 217, 263, 564, 218, 249, 442, 379, 590, 71, 301, 148, 362, 358, 269, 79, 105, 540, 313, 584, 309, 594, 176, 119, 335, 535, 542, 377, 313, 437, 580, 126, 322, 157, 148, 150, 377, 174, 121, 183, 57, 522, 416, 173, 345, 88, 346, 38, 411, 125, 336, 265, 66, 116, 587, 412, 451, 311, 366, 80, 572, 555, 538, 53, 364, 107, 306, 563, 7, 268, 254, 384, 28, 498, 34, 550, 389, 185, 117, 168, 438, 362, 431, 53, 488, 71, 220, 537, 174, 341, 373, 210, 400, 406, 365, 184, 423, 448, 442, 533, 304, 569, 228, 397, 378, 541, 165, 5, 579, 148, 391, 205, 262, 58, 472, 160, 535, 366, 482, 437, 296, 238, 298, 503, 172, 518, 309, 398, 196, 154, 421, 265, 568, 335, 444, 576, 300, 414, 95, 87, 289, 367, 389, 387, 382, 172, 578, 391, 428, 130, 550, 528, 64, 554, 251, 580, 280, 443, 488, 187, 83, 500, 318, 96, 341, 350, 289, 353, 497, 30, 506, 68, 175, 426, 327, 387, 269, 64, 118, 104, 116, 132, 152, 225, 165, 315, 546, 204, 238, 450, 571, 192, 193, 98, 309, 298, 472, 448, 495, 431, 180, 382, 563, 453, 58, 544, 547, 272, 163, 216, 265, 139, 493, 534, 560, 375, 502, 522, 339, 2, 507, 539, 594, 110, 104, 278, 352, 339, 22, 100, 105, 231, 230, 49, 442, 282, 43, 291, 44, 364, 298, 42, 60, 19, 539, 500, 31, 136, 367, 51, 320, 55, 513, 250, 596, 119, 57, 248, 162, 114, 456, 210, 300, 489, 307, 24, 23, 509, 424, 597, 256, 32, 167, 86, 218, 249, 446, 299, 567, 260, 438, 265, 322, 110, 317, 327, 27, 435, 455, 417, 289, 425, 442, 51, 526, 569, 150, 288, 265, 155, 6, 334, 86, 68, 507, 431, 296, 444, 212, 221, 376, 408, 140, 378, 203, 565, 181, 350, 193, 388, 498, 320, 576, 444, 128, 513, 577, 444, 165, 60, 168, 499, 353, 320, 315, 595, 353, 17, 442, 480, 424, 47, 310, 28, 422, 43, 423, 271, 344, 207, 327, 534, 94, 412, 422, 574, 64, 428, 555, 157, 247, 487, 61, 205, 488, 271, 242, 216, 196, 584, 138, 19, 157, 236, 312, 96, 481, 313, 311, 309, 376, 273, 485, 343, 187, 324, 62, 244, 86, 542, 359, 261, 430, 526, 396, 105, 274, 52, 121, 217, 337, 518, 539, 426, 546, 205, 539, 29, 269, 279, 304, 421, 136, 533, 279, 555, 538, 238, 451, 243, 378, 497, 595, 467, 378, 440, 124, 17, 406, 109, 418, 332, 447, 322, 412, 428, 39, 3, 421, 163, 55, 316, 402, 597, 350, 298, 197, 176, 545, 522, 117, 168, 490, 156, 385, 521, 430, 290, 480, 590, 597, 79, 212, 174, 150, 503, 234, 418, 336, 21, 276, 568, 597, 325, 402, 502, 172, 307, 328, 14, 351, 138, 503, 138, 561, 426, 178, 82, 530, 153, 397, 581, 353, 542, 431, 569, 474, 278, 356, 225, 317, 342, 38, 167, 426, 122, 339, 502, 94, 78, 538, 185, 589, 32, 520, 14, 162, 229, 495, 563, 308, 183, 242, 33, 298, 63
                                                });

    

    calibrator->random_timesteps_reduced = zeros_a(&(size_t){1}, (size_t[]){RANDOM_TIMESTEP_REDUCED_AMOUNT}, &(enum DataType){INT});

    for (size_t g = 0; g < RANDOM_TIMESTEP_REDUCED_AMOUNT; g++) {
        set_element_a(calibrator->random_timesteps_reduced, (size_t[]){g}, get_element_a(calibrator->random_timesteps_reduced_full, (size_t[]){g}));
    }

    return calibrator;
}

/**
 * Free Calibrator struct
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - Calibrator struct pointer
*/
void free_c(struct Calibrator* calibrator) {
    if (calibrator->datacollector != NULL) {
        free_dc(calibrator->datacollector);
    }
    if (calibrator->R_c != NULL) {
        free_a(calibrator->R_c);
    }
    if (calibrator->D != NULL) {
        free_a(calibrator->D);
    }
    if (calibrator->M != NULL) {
        free_a(calibrator->M);
    }
    if (calibrator->N != NULL) {
        free(calibrator->N);
    }
    if (calibrator->B != NULL) {
        free(calibrator->B);
    }
    if (calibrator->G != NULL) {
        free_a(calibrator->G);
    }
    if (calibrator->n_steps != NULL) {
        free(calibrator->n_steps);
    }
    if (calibrator->parallel != NULL) {
        free(calibrator->parallel);
    }
    if (calibrator->num_workers != NULL) {
        free(calibrator->num_workers);
    }
    if (calibrator->random_timesteps != NULL) {
        free_a(calibrator->random_timesteps);
    }
    if (calibrator->random_timesteps_reduced != NULL) {
        free_a(calibrator->random_timesteps_reduced);
    }
    if (calibrator->random_timesteps_reduced_full != NULL) {
        free_a(calibrator->random_timesteps_reduced_full);
    }
    free(calibrator);
}

/**
 * Lets Calibrator run in parallel where you specify how many workers to use
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - NUM_WORKERS
*/
void set_parallel_c(struct Calibrator* calibrator, int* num_workers) {
    *calibrator->parallel = 1;
    *calibrator->num_workers = *num_workers;
}

// /**
//  *******************************************************************************************************************
//  * Jacobian and Residual and other
//  *******************************************************************************************************************
// */

/**
 * Computing Jacobian for complex domain for calibration problem
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - Calibrator struct pointer
 *  - M_t - time step t of M
 *  - g - antenna gain array
*/
struct array* compute_J(struct Calibrator* calibrator, struct array* M_t, struct array* g) {
    struct array* J = zeros_a(&(size_t){2}, (size_t[]){(2 * (*calibrator->B)) , (2 * (*calibrator->N))}, &(enum DataType){COMPLEX});
    size_t row = 0;
    for (size_t p = 0; p < *calibrator->N; p++) {
        for (size_t q = p + 1; q < *calibrator->N; q++) {
            if (p != q) {
                for (size_t z = 0; z < *calibrator->N; z++) {
                    if (p == z) {
                        double complex m_pq = *(double complex*)get_element_a(M_t, (size_t[]){p,q});
                        double complex g_q = *(double complex*)get_element_a(g, (size_t[]){q});
                        //M
                        set_element_a(J, (size_t[]){row, z}, &(double complex){m_pq * conj(g_q)});
                        //M_bar
                        set_element_a(J, (size_t[]){row + *calibrator->B, z + *calibrator->N}, &(double complex){conj(m_pq * conj(g_q))});

                    }
                    if (q == z) {
                        double complex m_pq = *(double complex*)get_element_a(M_t, (size_t[]){p,q});
                        double complex g_p = *(double complex*)get_element_a(g, (size_t[]){p});
                        //N
                        set_element_a(J, (size_t[]){row, z + *calibrator->N}, &(double complex){g_p * m_pq});
                        //N_bar
                        set_element_a(J, (size_t[]){row + *calibrator->B, z}, &(double complex){conj(g_p * m_pq)});
                    }
                }
                row += 1;
            }
        }
    }

    return J;
}

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
struct array* compute_r(struct Calibrator* calibrator, struct array* M_t, struct array* D_t, struct array* g) {
    struct array* r = zeros_a(&(size_t){1}, (size_t[]){(2 * (*calibrator->B))}, &(enum DataType){COMPLEX});

    size_t count = 0;
    for (size_t p = 0; p < *calibrator->N; p++) {
        for (size_t q = p + 1; q < *calibrator->N; q++) {
            if (p != q) {
                double complex d_pq = *(double complex*)get_element_a(D_t, (size_t[]){p,q});
                double complex m_pq = *(double complex*)get_element_a(M_t, (size_t[]){p,q});
                double complex g_q = *(double complex*)get_element_a(g, (size_t[]){q});
                double complex g_p = *(double complex*)get_element_a(g, (size_t[]){p});
                
                double complex r_count = d_pq - g_p * m_pq * conj(g_q);
                set_element_a(r, (size_t[]){count}, &(double complex){ r_count});
                set_element_a(r, (size_t[]){count + *calibrator->B}, &(double complex){conj(r_count)});
                count += 1;
            }
        }
    }

    return r;
}

/**
 * Computing Residual for complex domain for calibration problem
 * RETURNS:
 *  - array struct pointer
 * INPUTS:
 *  - M_t - time step t of M
 *  - D_t - time step t of D
 *  - g - antenna gain array
*/
struct array* compute_r_N(struct array* M_t, struct array* D_t, struct array* g) {
    size_t N = M_t->dimensions[0];
    size_t B = (N * (N-1))/2;
    struct array* r = zeros_a(&(size_t){1}, (size_t[]){(2 * B)}, &(enum DataType){COMPLEX});

    size_t count = 0;
    for (size_t p = 0; p < N; p++) {
        for (size_t q = p + 1; q < N; q++) {
            if (p != q) {
                double complex d_pq = *(double complex*)get_element_a(D_t, (size_t[]){p,q});
                double complex m_pq = *(double complex*)get_element_a(M_t, (size_t[]){p,q});
                double complex g_q = *(double complex*)get_element_a(g, (size_t[]){q});
                double complex g_p = *(double complex*)get_element_a(g, (size_t[]){p});
                
                double complex r_count = d_pq - g_p * m_pq * conj(g_q);
                set_element_a(r, (size_t[]){count}, &(double complex){ r_count});
                set_element_a(r, (size_t[]){count + B}, &(double complex){conj(r_count)});
                count += 1;
            }
        }
    }

    return r;
}

/**
 * Correcting visibilities
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - Calibtrator struct pointer
*/
void correct_visibilities(struct Calibrator* calibrator) {
    if (calibrator->G == NULL) {
        log_null_array(__FILE__, __LINE__);
    } else {
        struct array* G_min_1 = hadamard_inverse_a(calibrator->G);
        if (calibrator->R_c != NULL) {
            free_a(calibrator->R_c);
        }
        calibrator->R_c = multiply_a(G_min_1, calibrator->D);
        free_a(G_min_1);
    }
}

// /**
//  *******************************************************************************************************************
//  * Second Order Algorithms
//  *******************************************************************************************************************
// */

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
void solve_G_LM(struct Calibrator* calibrator, size_t* imax, double* tua, double* l_init, double* v) {
    //G
    if (calibrator->G != NULL) {
        free_a(calibrator->G);
    }
    
    calibrator->G = zeros_a(calibrator->D->num_dimensions, calibrator->D->dimensions, calibrator->D->type);
    if (calibrator->datacollector != NULL) {
        free_dc(calibrator->datacollector);
    }

    calibrator->datacollector = new_dc(calibrator);

    struct ProgressBar* progress_bar = NULL;
    if (PROGRESSBAR) {
        progress_bar = new_pb(&(size_t){50}, &calibrator->G->dimensions[2]);
    }

    if (!*calibrator->parallel) {
        //Not in parallel
        for (size_t t = 0; t < *calibrator->n_steps; t++) {
            // printf("Timestep %zu\n", t);
            if (PROGRESSBAR) {
                update_pb(progress_bar, &(size_t){t+1});
            }

            double l = *l_init;

            struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
            struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});

            fill_diagonal_a(M_t, &(double complex){0 + 0*I});
            fill_diagonal_a(D_t, &(double complex){0 + 0*I});

            struct array* g_old = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
            struct array* g_new = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
            
            //creating arrays to hold damping factors.
            struct array* damping_g = NULL;
            struct array* damping_g_old = NULL;
            int t_int = t;
            bool check_random_timestep = false;
            int index_random_timestep = 0;
            
            if (ALL_DATA) {
                //checking all_gains, damping_g and damping_g_old should be saved.
                index_random_timestep = index_of_element_a(calibrator->random_timesteps, &(int){t_int});
                if (index_random_timestep >= 0) {
                    check_random_timestep = true;
                }
                if(check_random_timestep) {
                    damping_g = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                    damping_g_old = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                }
            }

            size_t i = 0;

            for (i = 0; i < *imax; i++) {
                copy_over_a(g_old, g_new, (int[]){-1}, (int[]){-1});
                struct array* J = compute_J(calibrator, M_t, g_old);
                struct array* r = compute_r(calibrator, M_t, D_t, g_old);
                struct array* J_conj = conjugate_a(J);
                struct array* J_conj_transpose = transpose_a(J_conj);
                struct array* Hessian = dot_a(J_conj_transpose, J);
                struct array* gradient = dot_a(J_conj_transpose, r);
                struct array* ones_array = ones_a(&(size_t){1}, (size_t[]){Hessian->dimensions[0]}, &(enum DataType){DOUBLE});
                struct array* identity = diag_a(ones_array, (int[]){-1});
                struct array* diag_only_hessian_array = multiply_a(Hessian, identity);
                struct array* Hessian_diag_scaled = multiply_constant_a(diag_only_hessian_array, &l, &(enum DataType){DOUBLE});
                struct array* Hessian_scaled = addition_a(Hessian, Hessian_diag_scaled);
                struct array* p_inv =  p_inv_a(Hessian_scaled);
                struct array* update_step = dot_a(p_inv, gradient);

                //setting g_new
                free_a(g_new);
                g_new = addition_a(g_old, update_step);

                struct array* new_r = compute_r(calibrator, M_t, D_t, g_new);
                double* norm_r = norm_a(r);
                double* norm_new_r = norm_a(new_r);

                if (*norm_r < * norm_new_r) {
                    size_t count = 0;
                    while(*norm_r < *norm_new_r && count < 20) {
                        count += 1;
                        l = l * *v;

                        free_a(update_step);
                        free_a(p_inv);
                        free_a(Hessian_scaled);
                        free_a(Hessian_diag_scaled);

                        Hessian_diag_scaled = multiply_constant_a(diag_only_hessian_array, &l, &(enum DataType){DOUBLE});
                        Hessian_scaled = addition_a(Hessian, Hessian_diag_scaled);
                        p_inv =  p_inv_a(Hessian_scaled);
                        update_step = dot_a(p_inv, gradient);

                        free_a(g_new);
                        g_new = addition_a(g_old, update_step);
                        free_a(new_r);
                        new_r = compute_r(calibrator, M_t, D_t, g_new);
                        free(norm_new_r);
                        norm_new_r = norm_a(new_r);
                    }
                } else {
                    l = l / *v;
                }

                free(norm_r);
                free(norm_new_r);
                free_a(new_r);

                if (ALL_DATA) {
                    if (check_random_timestep && i < MAX_ITERATIONS_CAPTURE) {
                        //saving damping_g
                        set_element_a(damping_g, (size_t[]){i}, &(double){l});
                        //saving damping_g_old
                        set_element_a(damping_g_old, (size_t[]){i}, &(double){0.0});
                        //saving all_gains
                        add_all_gain_dc(calibrator->datacollector, &(size_t){index_random_timestep}, &(size_t){i}, g_new);
                    }
                }

                struct array* subtraction = minus_a(g_new, g_old);
                double* norm1 = (double*)norm_a(subtraction);
                double* norm2 = (double*)norm_a(g_new);

                if (*norm1 / *norm2 <= *tua) {
                    free_a(subtraction);
                    free(norm1);
                    free(norm2);
                    free_a(update_step);
                    free_a(p_inv);
                    free_a(Hessian_scaled);
                    free_a(Hessian_diag_scaled);
                    free_a(diag_only_hessian_array);
                    free_a(identity);
                    free_a(ones_array);
                    free_a(gradient);
                    free_a(Hessian);
                    free_a(J_conj_transpose);
                    free_a(J_conj);
                    free_a(r);
                    free_a(J);
                    break;
                }

                free_a(subtraction);
                free(norm1);
                free(norm2);
                free_a(update_step);
                free_a(p_inv);
                free_a(Hessian_scaled);
                free_a(Hessian_diag_scaled);
                free_a(diag_only_hessian_array);
                free_a(identity);
                free_a(ones_array);
                free_a(gradient);
                free_a(Hessian);
                free_a(J_conj_transpose);
                free_a(J_conj);
                free_a(r);
                free_a(J);
            }

            if (ALL_DATA) {
                //truncating damping arrays to save them in datacollector
                if (check_random_timestep) {
                    if (i >= MAX_ITERATIONS_CAPTURE) {
                        //no need for truncation
                        add_damping_g_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g);
                        add_damping_g_old_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g_old);
                    } else {
                        struct array* damping_g_trunc = zeros_a(&(size_t){1}, (size_t[]){i + 1}, &(enum DataType){DOUBLE});
                        struct array* damping_g_old_trunc = zeros_a(&(size_t){1}, (size_t[]){i + 1}, &(enum DataType){DOUBLE});

                        for (int j = 0; j < i + 1; j++) {
                            set_element_a(damping_g_trunc, &(size_t){j}, &(double){*(double*)get_element_a(damping_g, &(size_t){j})});
                            set_element_a(damping_g_old_trunc, &(size_t){j}, &(double){*(double*)get_element_a(damping_g_old, &(size_t){j})});
                        }
                        add_damping_g_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g_trunc);
                        add_damping_g_old_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g_old_trunc);

                        free_a(damping_g_trunc);
                        free_a(damping_g_old_trunc);
                    }
                }
            }

            struct array* residual = compute_r(calibrator, M_t, D_t, g_new);
            add_last_residual_dc(calibrator->datacollector, &(size_t){t}, residual);
            free_a(residual);

            add_data_dc(calibrator, M_t, D_t, g_new, &t, &i);

            free_a(M_t);
            free_a(D_t);
            free_a(g_old);
            
            struct array* g_short = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});

            for (size_t i = 0; i < *calibrator->N; i++) {
                set_element_a(g_short, (size_t[]){i}, (double complex*)get_element_a(g_new, (size_t[]){i}));
            }

            struct array* g_conj = conjugate_a(g_short);
            struct array* g_outer = outer_a(g_short, g_conj);

            copy_over_a(calibrator->G, g_outer, (int[]){-1,-1,t}, (int[]){-1,-1});

            free_a(g_short);
            free_a(g_outer);
            free_a(g_conj);
            free_a(g_new);

            if (ALL_DATA) {
                if (check_random_timestep) {
                    free_a(damping_g);
                    free_a(damping_g_old);
                }
            }
        }
    } else {
        //parallelized
        //The parallelization is done with workers, where the workers work on first come first serve basis
        //The time steps are parallelized.

        pthread_t threads[*calibrator->num_workers];
        struct ThreadArgs* args[*calibrator->num_workers];
        size_t work_queue = 0;
        pthread_mutex_t mutex_work_queue = PTHREAD_MUTEX_INITIALIZER;
        size_t num_args = 8;
        size_t i;

        // Create worker threads
        for (i = 0; i < *calibrator->num_workers; i++) {
            args[i] = init_ta(&i, &num_args);
            args[i]->args[0] = calibrator;
            args[i]->args[1] = &work_queue;
            args[i]->args[2] = &mutex_work_queue;
            args[i]->args[3] = progress_bar;
            args[i]->args[4] = imax;
            args[i]->args[5] = tua;
            args[i]->args[6] = l_init;
            args[i]->args[7] = v;

            pthread_create(&threads[i], NULL, worker_solve_G_LM, args[i]);
        }

        // Main thread waits for all workers to finish
        for (i = 0; i < *calibrator->num_workers; i++) {
            pthread_join(threads[i], NULL);
        }

        //freeing ThreadArgs
        for (i = 0; i < *calibrator->num_workers; i++) {
            free_ta(args[i]);
        }

        if (PRINT_DATA) {
            printf("All workers have joined.\n");
        }

    }

    if (PROGRESSBAR) {
        char message[100];
        snprintf(message, sizeof(message), "Levenberg Marquardt: Antenna Size = %ld\n", *calibrator->N);
        stop_free_pb(progress_bar, message);
    }

}
/**
 * Worker used to parallelize function solve_G_LM
*/
void* worker_solve_G_LM(void* args) {
    struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
    void** arguments = thread_args->args;
    struct Calibrator* calibrator = arguments[0];
    size_t* worker_queue = arguments[1];
    pthread_mutex_t* mutex_work_queue = arguments[2];
    struct ProgressBar* progress_bar = arguments[3];
    size_t* imax = arguments[4];
    double* tua = arguments[5];
    double* l_init = arguments[6];
    double* v = arguments[7];

    // printf("Worker %ld\n",*thread_args->worker_id);

    while (1) {

        pthread_mutex_lock(mutex_work_queue);
        size_t t = *worker_queue;
        (*worker_queue)++;
        pthread_mutex_unlock(mutex_work_queue);

        if (t < *calibrator->n_steps) {

            //Need a lock
            if (PROGRESSBAR) {
                lock_pb(progress_bar);
                update_pb(progress_bar, &(size_t){t+1});
                unlock_pb(progress_bar);
            }

            double l = *l_init;

            struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
            struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});

            fill_diagonal_a(M_t, &(double complex){0 + 0*I});
            fill_diagonal_a(D_t, &(double complex){0 + 0*I});

            struct array* g_old = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
            struct array* g_new = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
            
            //creating arrays to hold damping factors.
            struct array* damping_g = NULL;
            struct array* damping_g_old = NULL;
            int t_int = t;
            bool check_random_timestep = false;
            int index_random_timestep = 0;
            
            if (ALL_DATA) {
                //checking all_gains, damping_g and damping_g_old should be saved.
                index_random_timestep = index_of_element_a(calibrator->random_timesteps, &(int){t_int});
                if (index_random_timestep >= 0) {
                    check_random_timestep = true;
                }
                if(check_random_timestep) {
                    damping_g = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                    damping_g_old = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                }
            }

            size_t i = 0;

            for (i = 0; i < *imax; i++) {
                copy_over_a(g_old, g_new, (int[]){-1}, (int[]){-1});
                struct array* J = compute_J(calibrator, M_t, g_old);
                struct array* r = compute_r(calibrator, M_t, D_t, g_old);
                struct array* J_conj = conjugate_a(J);
                struct array* J_conj_transpose = transpose_a(J_conj);
                struct array* Hessian = dot_a(J_conj_transpose, J);
                struct array* gradient = dot_a(J_conj_transpose, r);
                struct array* ones_array = ones_a(&(size_t){1}, (size_t[]){Hessian->dimensions[0]}, &(enum DataType){DOUBLE});
                struct array* identity = diag_a(ones_array, (int[]){-1});
                struct array* diag_only_hessian_array = multiply_a(Hessian, identity);
                struct array* Hessian_diag_scaled = multiply_constant_a(diag_only_hessian_array, &l, &(enum DataType){DOUBLE});
                struct array* Hessian_scaled = addition_a(Hessian, Hessian_diag_scaled);
                struct array* p_inv =  p_inv_a(Hessian_scaled);
                struct array* update_step = dot_a(p_inv, gradient);

                //setting g_new
                free_a(g_new);
                g_new = addition_a(g_old, update_step);

                struct array* new_r = compute_r(calibrator, M_t, D_t, g_new);
                double* norm_r = norm_a(r);
                double* norm_new_r = norm_a(new_r);

                if (*norm_r < * norm_new_r) {
                    size_t count = 0;
                    while(*norm_r < * norm_new_r && count < 20) {
                        count += 1;
                        l = l * *v;

                        free_a(update_step);
                        free_a(p_inv);
                        free_a(Hessian_scaled);
                        free_a(Hessian_diag_scaled);

                        Hessian_diag_scaled = multiply_constant_a(diag_only_hessian_array, &l, &(enum DataType){DOUBLE});
                        Hessian_scaled = addition_a(Hessian, Hessian_diag_scaled);
                        p_inv =  p_inv_a(Hessian_scaled);
                        update_step = dot_a(p_inv, gradient);

                        free_a(g_new);
                        g_new = addition_a(g_old, update_step);
                        free_a(new_r);
                        new_r = compute_r(calibrator, M_t, D_t, g_new);
                        free(norm_new_r);
                        norm_new_r = norm_a(new_r);
                    }
                } else {
                    l = l / *v;
                }

                free(norm_r);
                free(norm_new_r);

                free_a(new_r);

                if (ALL_DATA) {
                    if (check_random_timestep && i < MAX_ITERATIONS_CAPTURE) {
                        //saving damping_g
                        set_element_a(damping_g, (size_t[]){i}, &(double){l});
                        //saving damping_g_old
                        set_element_a(damping_g_old, (size_t[]){i}, &(double){0.0});
                        //saving all_gains
                        lock_dc(calibrator->datacollector);
                        add_all_gain_dc(calibrator->datacollector, &(size_t){index_random_timestep}, &(size_t){i}, g_new);
                        unlock_dc(calibrator->datacollector);
                    }
                }

                struct array* subtraction = minus_a(g_new, g_old);
                double* norm1 = (double*)norm_a(subtraction);
                double* norm2 = (double*)norm_a(g_new);

                if (*norm1 / *norm2 <= *tua) {
                    free_a(subtraction);
                    free(norm1);
                    free(norm2);
                    free_a(update_step);
                    free_a(p_inv);
                    free_a(Hessian_scaled);
                    free_a(Hessian_diag_scaled);
                    free_a(diag_only_hessian_array);
                    free_a(identity);
                    free_a(ones_array);
                    free_a(gradient);
                    free_a(Hessian);
                    free_a(J_conj_transpose);
                    free_a(J_conj);
                    free_a(r);
                    free_a(J);
                    break;
                }

                free_a(subtraction);
                free(norm1);
                free(norm2);
                free_a(update_step);
                free_a(p_inv);
                free_a(Hessian_scaled);
                free_a(Hessian_diag_scaled);
                free_a(diag_only_hessian_array);
                free_a(identity);
                free_a(ones_array);
                free_a(gradient);
                free_a(Hessian);
                free_a(J_conj_transpose);
                free_a(J_conj);
                free_a(r);
                free_a(J);
            }

            if (ALL_DATA) {
                //truncating damping arrays to save them in datacollector
                if (check_random_timestep) {
                    if (i >= MAX_ITERATIONS_CAPTURE) {
                        //no need for truncation
                        lock_dc(calibrator->datacollector);
                        add_damping_g_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g);
                        add_damping_g_old_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g_old);
                        unlock_dc(calibrator->datacollector);
                    } else {
                        struct array* damping_g_trunc = zeros_a(&(size_t){1}, (size_t[]){i + 1}, &(enum DataType){DOUBLE});
                        struct array* damping_g_old_trunc = zeros_a(&(size_t){1}, (size_t[]){i + 1}, &(enum DataType){DOUBLE});

                        for (int j = 0; j < i + 1; j++) {
                            set_element_a(damping_g_trunc, &(size_t){j}, &(double){*(double*)get_element_a(damping_g, &(size_t){j})});
                            set_element_a(damping_g_old_trunc, &(size_t){j}, &(double){*(double*)get_element_a(damping_g_old, &(size_t){j})});
                        }
                        lock_dc(calibrator->datacollector);
                        add_damping_g_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g_trunc);
                        add_damping_g_old_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g_old_trunc);
                        unlock_dc(calibrator->datacollector);

                        free_a(damping_g_trunc);
                        free_a(damping_g_old_trunc);
                    }
                }
            }

            struct array* residual = compute_r(calibrator, M_t, D_t, g_new);
            lock_dc(calibrator->datacollector);
            add_last_residual_dc(calibrator->datacollector, &(size_t){t}, residual);
            unlock_dc(calibrator->datacollector);
            free_a(residual);
            add_data_dc(calibrator, M_t, D_t, g_new, &t, &i);

            free_a(M_t);
            free_a(D_t);
            free_a(g_old);
            
            struct array* g_short = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});

            for (size_t i = 0; i < *calibrator->N; i++) {
                set_element_a(g_short, (size_t[]){i}, (double complex*)get_element_a(g_new, (size_t[]){i}));
            }

            struct array* g_conj = conjugate_a(g_short);
            struct array* g_outer = outer_a(g_short, g_conj);

            lock_a(calibrator->G);
            copy_over_a(calibrator->G, g_outer, (int[]){-1,-1,t}, (int[]){-1,-1});
            unlock_a(calibrator->G);

            free_a(g_short);
            free_a(g_outer);
            free_a(g_conj);
            free_a(g_new);


            if (ALL_DATA) {
                if (check_random_timestep) {
                    free_a(damping_g);
                    free_a(damping_g_old);
                }
            }

        } else {
            //All work is done.
            break;
        }
    }

    pthread_exit(NULL);
}

// /**
//  *******************************************************************************************************************
//  * Other Algorithms
//  *******************************************************************************************************************
// */


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
void solve_G_StEFCal(struct Calibrator* calibrator, size_t* imax, double* tua) {

    //G
    if (calibrator->G != NULL) {
        free_a(calibrator->G);
    }

    calibrator->G = zeros_a(calibrator->D->num_dimensions, calibrator->D->dimensions, calibrator->D->type);

    if (calibrator->datacollector != NULL) {
        free_dc(calibrator->datacollector);
    }
    calibrator->datacollector = new_dc(calibrator);

    struct ProgressBar* progress_bar = NULL;
    if (PROGRESSBAR) {
        progress_bar = new_pb(&(size_t){50}, &calibrator->G->dimensions[2]);
    }

    if (!*calibrator->parallel) {
        //Not in parallel
        for (size_t t = 0; t < *calibrator->n_steps; t++) {
            if (PROGRESSBAR) {
                update_pb(progress_bar, &(size_t){t+1});
            }

            struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
            struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});

            fill_diagonal_a(M_t, &(double complex){0 + 0*I});
            fill_diagonal_a(D_t, &(double complex){0 + 0*I});

            struct array* g_old = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});
            struct array* g = ones_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});

            //creating arrays to hold damping factors.
            struct array* damping_g = NULL;
            struct array* damping_g_old = NULL;
            int t_int = t;
            bool check_random_timestep = false;
            int index_random_timestep = 0;
            
            if (ALL_DATA) {
                //checking all_gains, damping_g and damping_g_old should be saved.
                index_random_timestep = index_of_element_a(calibrator->random_timesteps, &(int){t_int});
                if (index_random_timestep >= 0) {
                    check_random_timestep = true;
                }
                if(check_random_timestep) {
                    damping_g = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                    damping_g_old = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                }
            }

            size_t i = 0;

            for (i = 0; i < *imax; i++) {

                struct array* g_p = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});
                for (size_t p = 0; p < *calibrator->N; p++) {
                    struct array* m_p = slice_a(M_t, (int[]){-1,p});
                    struct array* d_p = slice_a(D_t, (int[]){-1,p});

                    struct array* z = multiply_a(g,m_p);
                    struct array* d_p_conjugate = conjugate_a(d_p);
                    struct array* z_conjugate = conjugate_a(z);
                    struct array* dot_1 = dot_a(d_p_conjugate, z);
                    struct array* dot_2 = dot_a(z_conjugate, z);
                    double complex dot_1_value = *(double complex*)get_element_a(dot_1, (size_t[]){0});
                    double complex dot_2_value = *(double complex*)get_element_a(dot_2, (size_t[]){0});

                    set_element_a(g_p, (size_t[]){p}, &(double complex){dot_1_value / dot_2_value});

                    free_a(m_p);
                    free_a(d_p);
                    free_a(z);
                    free_a(d_p_conjugate);
                    free_a(z_conjugate);
                    free_a(dot_1);
                    free_a(dot_2);
                }

                free_a(g_old);
                g_old = g;
                g = g_p;

                if (i % 2 == 0) {
                    struct array* sum = addition_a(g, g_old);
                    struct array* sum_divide = division_constant_a(sum, &(double){2}, &(enum DataType){DOUBLE});
                    free_a(sum);
                    free_a(g);
                    g = sum_divide;
                }

                if (ALL_DATA) {
                    if (check_random_timestep && i < MAX_ITERATIONS_CAPTURE) {
                        //saving damping_g
                        set_element_a(damping_g, (size_t[]){i}, &(double){0.0});
                        //saving damping_g_old
                        set_element_a(damping_g_old, (size_t[]){i}, &(double){0.0});
                        //saving all_gains
                        add_all_gain_dc(calibrator->datacollector, &(size_t){index_random_timestep}, &(size_t){i}, g);
                    }
                }

                struct array* subtraction = minus_a(g, g_old);
                double* norm1 = (double*)norm_a(subtraction);
                double* norm2 = (double*)norm_a(g);

                if (*norm1 / *norm2 <= *tua) {
                    free_a(subtraction);
                    free(norm1);
                    free(norm2);
                    break;
                }

                free_a(subtraction);
                free(norm1);
                free(norm2);
            }

            struct array* g_conj = conjugate_a(g);
            struct array* g_outer = outer_a(g, g_conj);

            struct array* g_full = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
            for (size_t i = 0; i < *calibrator->N; i++) {
                set_element_a(g_full, (size_t[]){i}, (double complex*)get_element_a(g, (size_t[]){i}));
                set_element_a(g_full, (size_t[]){*calibrator->N + i}, (double complex*)get_element_a(g_conj, (size_t[]){i}));
            }

            if (ALL_DATA) {
                //truncating damping arrays to save them in datacollector
                if (check_random_timestep) {
                    if (i >= MAX_ITERATIONS_CAPTURE) {
                        //no need for truncation
                        add_damping_g_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g);
                        add_damping_g_old_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g_old);
                    } else {
                        struct array* damping_g_trunc = zeros_a(&(size_t){1}, (size_t[]){i + 1}, &(enum DataType){DOUBLE});
                        struct array* damping_g_old_trunc = zeros_a(&(size_t){1}, (size_t[]){i + 1}, &(enum DataType){DOUBLE});

                        for (int j = 0; j < i + 1; j++) {
                            set_element_a(damping_g_trunc, &(size_t){j}, &(double){*(double*)get_element_a(damping_g, &(size_t){j})});
                            set_element_a(damping_g_old_trunc, &(size_t){j}, &(double){*(double*)get_element_a(damping_g_old, &(size_t){j})});
                        }
                        add_damping_g_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g_trunc);
                        add_damping_g_old_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g_old_trunc);

                        free_a(damping_g_trunc);
                        free_a(damping_g_old_trunc);
                    }
                }
            }

            struct array* residual = compute_r(calibrator, M_t, D_t, g_full);
            add_last_residual_dc(calibrator->datacollector, &(size_t){t}, residual);
            free_a(residual);


            add_data_dc(calibrator, M_t, D_t, g_full, &t, &i);
            free_a(g_full);

            free_a(M_t);
            free_a(D_t);
            free_a(g_old);

            copy_over_a(calibrator->G, g_outer, (int[]){-1,-1,t}, (int[]){-1,-1});

            free_a(g_outer);
            free_a(g_conj);
            free_a(g);

            if (ALL_DATA) {
                if (check_random_timestep) {
                    free_a(damping_g);
                    free_a(damping_g_old);
                }
            }
        }
    } else {
        //parallel
        //The parallelization is done with workers, where the workers work on first come first serve basis
        //The time steps are parallelized.

        pthread_t threads[*calibrator->num_workers];
        struct ThreadArgs* args[*calibrator->num_workers];
        size_t work_queue = 0;
        pthread_mutex_t mutex_work_queue = PTHREAD_MUTEX_INITIALIZER;
        size_t num_args = 6;
        size_t i;

        // Create worker threads
        for (i = 0; i < *calibrator->num_workers; i++) {
            args[i] = init_ta(&i, &num_args);
            args[i]->args[0] = calibrator;
            args[i]->args[1] = &work_queue;
            args[i]->args[2] = &mutex_work_queue;
            args[i]->args[3] = progress_bar;
            args[i]->args[4] = imax;
            args[i]->args[5] = tua;

            pthread_create(&threads[i], NULL, worker_solve_G_StEFCal, args[i]);
        }

        // Main thread waits for all workers to finish
        for (i = 0; i < *calibrator->num_workers; i++) {
            pthread_join(threads[i], NULL);
        }

        //freeing ThreadArgs
        for (i = 0; i < *calibrator->num_workers; i++) {
            free_ta(args[i]);
        }

        if (PRINT_DATA) {
            printf("All workers have joined.\n");
        }
    }

    if (PROGRESSBAR) {
        char message[100];
        snprintf(message, sizeof(message), "StEFCal: Antenna Size = %ld\n", *calibrator->N);
        stop_free_pb(progress_bar, message);
    }
}

/**
 * Worker used to parallelize function solve_G_StEFCal
*/
void* worker_solve_G_StEFCal(void* args) {
    struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
    void** arguments = thread_args->args;
    struct Calibrator* calibrator = arguments[0];
    size_t* worker_queue = arguments[1];
    pthread_mutex_t* mutex_work_queue = arguments[2];
    struct ProgressBar* progress_bar = arguments[3];
    size_t* imax = arguments[4];
    double* tua = arguments[5];

    while (1) {
        pthread_mutex_lock(mutex_work_queue);
        size_t t = *worker_queue;
        (*worker_queue)++;
        pthread_mutex_unlock(mutex_work_queue);

        if (t < *calibrator->n_steps) {

            //Need a lock
            if (PROGRESSBAR) {
                lock_pb(progress_bar);
                update_pb(progress_bar, &(size_t){t+1});
                unlock_pb(progress_bar);
            }

            struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
            struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});

            fill_diagonal_a(M_t, &(double complex){0 + 0*I});
            fill_diagonal_a(D_t, &(double complex){0 + 0*I});

            struct array* g_old = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});
            struct array* g = ones_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});

            //creating arrays to hold damping factors.
            struct array* damping_g = NULL;
            struct array* damping_g_old = NULL;
            int t_int = t;
            bool check_random_timestep = false;
            int index_random_timestep = 0;
            
            if (ALL_DATA) {
                //checking all_gains, damping_g and damping_g_old should be saved.
                index_random_timestep = index_of_element_a(calibrator->random_timesteps, &(int){t_int});
                if (index_random_timestep >= 0) {
                    check_random_timestep = true;
                }
                if(check_random_timestep) {
                    damping_g = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                    damping_g_old = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                }
            }

            size_t i = 0;

            for (i = 0; i < *imax; i++) {

                struct array* g_p = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});
                for (size_t p = 0; p < *calibrator->N; p++) {
                    struct array* m_p = slice_a(M_t, (int[]){-1,p});
                    struct array* d_p = slice_a(D_t, (int[]){-1,p});

                    struct array* z = multiply_a(g,m_p);
                    struct array* d_p_conjugate = conjugate_a(d_p);
                    struct array* z_conjugate = conjugate_a(z);
                    struct array* dot_1 = dot_a(d_p_conjugate, z);
                    struct array* dot_2 = dot_a(z_conjugate, z);
                    double complex dot_1_value = *(double complex*)get_element_a(dot_1, (size_t[]){0});
                    double complex dot_2_value = *(double complex*)get_element_a(dot_2, (size_t[]){0});

                    set_element_a(g_p, (size_t[]){p}, &(double complex){dot_1_value / dot_2_value});

                    free_a(m_p);
                    free_a(d_p);
                    free_a(z);
                    free_a(d_p_conjugate);
                    free_a(z_conjugate);
                    free_a(dot_1);
                    free_a(dot_2);
                }

                free_a(g_old);
                g_old = g;
                g = g_p;

                if (i % 2 == 0) {
                    struct array* sum = addition_a(g, g_old);
                    struct array* sum_divide = division_constant_a(sum, &(double){2}, &(enum DataType){DOUBLE});
                    free_a(sum);
                    free_a(g);
                    g = sum_divide;
                }

                if (ALL_DATA) {
                    if (check_random_timestep && i < MAX_ITERATIONS_CAPTURE) {
                        //saving damping_g
                        set_element_a(damping_g, (size_t[]){i}, &(double){0.0});
                        //saving damping_g_old
                        set_element_a(damping_g_old, (size_t[]){i}, &(double){0.0});
                        //saving all_gains
                        lock_dc(calibrator->datacollector);
                        add_all_gain_dc(calibrator->datacollector, &(size_t){index_random_timestep}, &(size_t){i}, g);
                        unlock_dc(calibrator->datacollector);
                    }
                }

                struct array* subtraction = minus_a(g, g_old);
                double* norm1 = (double*)norm_a(subtraction);
                double* norm2 = (double*)norm_a(g);

                if (*norm1 / *norm2 <= *tua) {
                    free_a(subtraction);
                    free(norm1);
                    free(norm2);
                    break;
                }

                free_a(subtraction);
                free(norm1);
                free(norm2);
            }

            struct array* g_conj = conjugate_a(g);
            struct array* g_outer = outer_a(g, g_conj);

            struct array* g_full = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
            for (size_t i = 0; i < *calibrator->N; i++) {
                set_element_a(g_full, (size_t[]){i}, (double complex*)get_element_a(g, (size_t[]){i}));
                set_element_a(g_full, (size_t[]){*calibrator->N + i}, (double complex*)get_element_a(g_conj, (size_t[]){i}));
            }

            if (ALL_DATA) {
                //truncating damping arrays to save them in datacollector
                if (check_random_timestep) {
                    if (i >= MAX_ITERATIONS_CAPTURE) {
                        //no need for truncation
                        lock_dc(calibrator->datacollector);
                        add_damping_g_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g);
                        add_damping_g_old_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g_old);
                        unlock_dc(calibrator->datacollector);
                    } else {
                        struct array* damping_g_trunc = zeros_a(&(size_t){1}, (size_t[]){i + 1}, &(enum DataType){DOUBLE});
                        struct array* damping_g_old_trunc = zeros_a(&(size_t){1}, (size_t[]){i + 1}, &(enum DataType){DOUBLE});

                        for (int j = 0; j < i + 1; j++) {
                            set_element_a(damping_g_trunc, &(size_t){j}, &(double){*(double*)get_element_a(damping_g, &(size_t){j})});
                            set_element_a(damping_g_old_trunc, &(size_t){j}, &(double){*(double*)get_element_a(damping_g_old, &(size_t){j})});
                        }
                        lock_dc(calibrator->datacollector);
                        add_damping_g_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g_trunc);
                        add_damping_g_old_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g_old_trunc);
                        unlock_dc(calibrator->datacollector);

                        free_a(damping_g_trunc);
                        free_a(damping_g_old_trunc);
                    }
                }
            }

            struct array* residual = compute_r(calibrator, M_t, D_t, g_full);
            lock_dc(calibrator->datacollector);
            add_last_residual_dc(calibrator->datacollector, &(size_t){t}, residual);
            unlock_dc(calibrator->datacollector);
            free_a(residual);
            add_data_dc(calibrator, M_t, D_t, g_full, &t, &i);
            
            free_a(g_full);

            free_a(M_t);
            free_a(D_t);
            free_a(g_old);

            lock_a(calibrator->G);
            copy_over_a(calibrator->G, g_outer, (int[]){-1,-1,t}, (int[]){-1,-1});
            unlock_a(calibrator->G);

            free_a(g_outer);
            free_a(g_conj);
            free_a(g);

            if (ALL_DATA) {
                if (check_random_timestep) {
                    free_a(damping_g);
                    free_a(damping_g_old);
                }
            }
        } else {
            //All work is done.
            break;
        }
    }

    pthread_exit(NULL);
}

/**
 * Plot the real corrected visibilities
 * RETURNS:
 *  - Makes .png image
 * INPUTS:
 *  - simulator - Simulator struct pointer
 *  - b_0 - first value of the baseline
 *  - b_1 - second value of the baseline
*/
void plot_corrected_visibilities_c(struct Calibrator* calibrator, size_t* b_0, size_t* b_1) {
    if (*calibrator->M->type != *calibrator->R_c->type) {
        log_incompatible_types(__FILE__, __LINE__);
    }
    if (*calibrator->M->num_dimensions != *calibrator->R_c->num_dimensions) {
        log_incompatible_size(__FILE__, __LINE__);
    }
    if (calibrator->M->dimensions[0] != calibrator->R_c->dimensions[0] || calibrator->M->dimensions[1] != calibrator->R_c->dimensions[1] || calibrator->M->dimensions[2] != calibrator->R_c->dimensions[2]) {
        log_incompatible_dimensions(__FILE__, __LINE__);
    }

    FILE *combinedDataFile = fopen("combined_data.txt", "w");

    for (int h = 0; h < calibrator->M->dimensions[2]; h++) {
        double complex complex_value_M = *(double complex*)get_element_a(calibrator->M, (size_t[]){*b_0,*b_1,h});
        double complex complex_value_R_c = *(double complex*)get_element_a(calibrator->R_c, (size_t[]){*b_0,*b_1,h});
        fprintf(combinedDataFile, "%d %lf %d\n", h, creal(complex_value_M), 0);
        fprintf(combinedDataFile, "%d %lf %d\n", h, creal(complex_value_R_c), 1);
    }

    fclose(combinedDataFile);

    char title[100];
    snprintf(title, sizeof(title), "Visibilities for baseline %ld to %ld",*b_0, *b_1);

        // Generate Gnuplot commands file
    FILE *commandsFile = fopen("commands.gplot", "w");
    fprintf(commandsFile, "set terminal png\n");
    fprintf(commandsFile, "set title '%s'\n", title);
    fprintf(commandsFile, "set output './figures/corrected_visibilities_baseline.png'\n");
    fprintf(commandsFile, "set offsets graph 0.01, graph 0.01, graph 0.01, graph 0.01\n");
    fprintf(commandsFile, "set yrange [%lf:%lf]\n", -50.0, 50.0);
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
 * This will add data to datacollector
*/
void add_data_dc(struct Calibrator* calibrator, struct array* M_t, struct array* D_t, struct array* g, size_t* time_step, size_t* iterations) {
    lock_dc(calibrator->datacollector);
    add_iteration_dc(calibrator->datacollector, time_step, &(int){*iterations});
    add_gain_dc(calibrator->datacollector, time_step, g);

    unlock_dc(calibrator->datacollector);
}

// /**
//  *******************************************************************************************************************
//  * Alogrithms with reduced amount of timesteps
//  *******************************************************************************************************************
// */

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
void solve_G_LM_REDUCED_TIMESTEPS(struct Calibrator* calibrator, size_t* imax, double* tua, double* l_init, double* v) {
    //G
    if (calibrator->G != NULL) {
        free_a(calibrator->G);
    }
    
    calibrator->G = zeros_a(calibrator->D->num_dimensions, calibrator->D->dimensions, calibrator->D->type);
    if (calibrator->datacollector != NULL) {
        free_dc(calibrator->datacollector);
    }

    calibrator->datacollector = new_reduced_dc(calibrator);

    struct ProgressBar* progress_bar = NULL;
    if (PROGRESSBAR) {
        progress_bar = new_pb(&(size_t){50}, &(size_t){RANDOM_TIMESTEP_REDUCED_AMOUNT});
    }

    if (!*calibrator->parallel) {
        //Not in parallel
        size_t t_reduced_count = 0;
        for (size_t t = 0; t < *calibrator->n_steps; t++) {
            if (contains_element_a(calibrator->random_timesteps_reduced, &(int){t})) {
                if (PROGRESSBAR) {
                    update_pb(progress_bar, &(size_t){t_reduced_count+1});
                }

                double l = *l_init;

                struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
                struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});

                fill_diagonal_a(M_t, &(double complex){0 + 0*I});
                fill_diagonal_a(D_t, &(double complex){0 + 0*I});

                struct array* g_old = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
                struct array* g_new = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
                
                //creating arrays to hold damping factors.
                struct array* damping_g = NULL;
                struct array* damping_g_old = NULL;
                
                if (ALL_DATA) {
                    damping_g = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                    damping_g_old = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                }

                size_t i = 0;
                for (i = 0; i < *imax; i++) {
                    copy_over_a(g_old, g_new, (int[]){-1}, (int[]){-1});
                    struct array* J = compute_J(calibrator, M_t, g_old);
                    struct array* r = compute_r(calibrator, M_t, D_t, g_old);
                    struct array* J_conj = conjugate_a(J);
                    struct array* J_conj_transpose = transpose_a(J_conj);
                    struct array* Hessian = dot_a(J_conj_transpose, J);
                    struct array* gradient = dot_a(J_conj_transpose, r);
                    struct array* ones_array = ones_a(&(size_t){1}, (size_t[]){Hessian->dimensions[0]}, &(enum DataType){DOUBLE});
                    struct array* identity = diag_a(ones_array, (int[]){-1});
                    struct array* diag_only_hessian_array = multiply_a(Hessian, identity);
                    struct array* Hessian_diag_scaled = multiply_constant_a(diag_only_hessian_array, &l, &(enum DataType){DOUBLE});
                    struct array* Hessian_scaled = addition_a(Hessian, Hessian_diag_scaled);
                    struct array* p_inv =  p_inv_a(Hessian_scaled);
                    struct array* update_step = dot_a(p_inv, gradient);

                    //setting g_new
                    free_a(g_new);
                    g_new = addition_a(g_old, update_step);

                    struct array* new_r = compute_r(calibrator, M_t, D_t, g_new);
                    double* norm_r = norm_a(r);
                    double* norm_new_r = norm_a(new_r);

                    if (*norm_r < * norm_new_r) {
                        size_t count = 0;
                        while(*norm_r < *norm_new_r && count < 20) {
                            count += 1;
                            l = l * *v;

                            free_a(update_step);
                            free_a(p_inv);
                            free_a(Hessian_scaled);
                            free_a(Hessian_diag_scaled);

                            Hessian_diag_scaled = multiply_constant_a(diag_only_hessian_array, &l, &(enum DataType){DOUBLE});
                            Hessian_scaled = addition_a(Hessian, Hessian_diag_scaled);
                            p_inv =  p_inv_a(Hessian_scaled);
                            update_step = dot_a(p_inv, gradient);

                            free_a(g_new);
                            g_new = addition_a(g_old, update_step);
                            free_a(new_r);
                            new_r = compute_r(calibrator, M_t, D_t, g_new);
                            free(norm_new_r);
                            norm_new_r = norm_a(new_r);
                        }
                    } else {
                        l = l / *v;
                    }

                    free(norm_r);
                    free(norm_new_r);
                    free_a(new_r);

                    if (ALL_DATA) {
                        if (i < MAX_ITERATIONS_CAPTURE) {
                            //saving damping_g
                            set_element_a(damping_g, (size_t[]){i}, &(double){l});
                            //saving damping_g_old
                            set_element_a(damping_g_old, (size_t[]){i}, &(double){0.0});
                            //saving all_gains
                            add_all_gain_dc(calibrator->datacollector, &(size_t){t_reduced_count}, &(size_t){i}, g_new);
                        }
                    }

                    struct array* subtraction = minus_a(g_new, g_old);
                    double* norm1 = (double*)norm_a(subtraction);
                    double* norm2 = (double*)norm_a(g_new);

                    if (*norm1 / *norm2 <= *tua) {
                        free_a(subtraction);
                        free(norm1);
                        free(norm2);
                        free_a(update_step);
                        free_a(p_inv);
                        free_a(Hessian_scaled);
                        free_a(Hessian_diag_scaled);
                        free_a(diag_only_hessian_array);
                        free_a(identity);
                        free_a(ones_array);
                        free_a(gradient);
                        free_a(Hessian);
                        free_a(J_conj_transpose);
                        free_a(J_conj);
                        free_a(r);
                        free_a(J);
                        break;
                    }

                    free_a(subtraction);
                    free(norm1);
                    free(norm2);
                    free_a(update_step);
                    free_a(p_inv);
                    free_a(Hessian_scaled);
                    free_a(Hessian_diag_scaled);
                    free_a(diag_only_hessian_array);
                    free_a(identity);
                    free_a(ones_array);
                    free_a(gradient);
                    free_a(Hessian);
                    free_a(J_conj_transpose);
                    free_a(J_conj);
                    free_a(r);
                    free_a(J);
                }

                if (ALL_DATA) {
                    //truncating damping arrays to save them in datacollector
                    if (i >= MAX_ITERATIONS_CAPTURE) {
                        //no need for truncation
                        add_damping_g_dc(calibrator->datacollector, &(size_t){t_reduced_count}, damping_g);
                        add_damping_g_old_dc(calibrator->datacollector, &(size_t){t_reduced_count}, damping_g_old);
                    } else {
                        struct array* damping_g_trunc = zeros_a(&(size_t){1}, (size_t[]){i + 1}, &(enum DataType){DOUBLE});
                        struct array* damping_g_old_trunc = zeros_a(&(size_t){1}, (size_t[]){i + 1}, &(enum DataType){DOUBLE});

                        for (int j = 0; j < i + 1; j++) {
                            set_element_a(damping_g_trunc, &(size_t){j}, &(double){*(double*)get_element_a(damping_g, &(size_t){j})});
                            set_element_a(damping_g_old_trunc, &(size_t){j}, &(double){*(double*)get_element_a(damping_g_old, &(size_t){j})});
                        }
                        add_damping_g_dc(calibrator->datacollector, &(size_t){t_reduced_count}, damping_g_trunc);
                        add_damping_g_old_dc(calibrator->datacollector, &(size_t){t_reduced_count}, damping_g_old_trunc);

                        free_a(damping_g_trunc);
                        free_a(damping_g_old_trunc);
                    }
                }

                struct array* residual = compute_r(calibrator, M_t, D_t, g_new);
                add_last_residual_dc(calibrator->datacollector, &(size_t){t_reduced_count}, residual);
                free_a(residual);

                add_data_dc(calibrator, M_t, D_t, g_new, &t_reduced_count, &i);

                free_a(M_t);
                free_a(D_t);
                free_a(g_old);
                
                struct array* g_short = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});

                for (size_t i = 0; i < *calibrator->N; i++) {
                    set_element_a(g_short, (size_t[]){i}, (double complex*)get_element_a(g_new, (size_t[]){i}));
                }

                struct array* g_conj = conjugate_a(g_short);
                struct array* g_outer = outer_a(g_short, g_conj);

                copy_over_a(calibrator->G, g_outer, (int[]){-1,-1,t}, (int[]){-1,-1});

                free_a(g_short);
                free_a(g_outer);
                free_a(g_conj);
                free_a(g_new);

                if (ALL_DATA) {
                    free_a(damping_g);
                    free_a(damping_g_old);
                }

                t_reduced_count++;
            }
        }
    } else {
        //parallelized
        //The parallelization is done with workers, where the workers work on first come first serve basis
        //The time steps are parallelized.

        pthread_t threads[*calibrator->num_workers];
        struct ThreadArgs* args[*calibrator->num_workers];
        size_t work_queue = 0;
        pthread_mutex_t mutex_work_queue = PTHREAD_MUTEX_INITIALIZER;
        size_t num_args = 8;
        size_t i;

        // Create worker threads
        for (i = 0; i < *calibrator->num_workers; i++) {
            args[i] = init_ta(&i, &num_args);
            args[i]->args[0] = calibrator;
            args[i]->args[1] = &work_queue;
            args[i]->args[2] = &mutex_work_queue;
            args[i]->args[3] = progress_bar;
            args[i]->args[4] = imax;
            args[i]->args[5] = tua;
            args[i]->args[6] = l_init;
            args[i]->args[7] = v;

            pthread_create(&threads[i], NULL, worker_solve_G_LM_REDUCED_TIMESTEPS, args[i]);
        }

        // Main thread waits for all workers to finish
        for (i = 0; i < *calibrator->num_workers; i++) {
            pthread_join(threads[i], NULL);
        }

        //freeing ThreadArgs
        for (i = 0; i < *calibrator->num_workers; i++) {
            free_ta(args[i]);
        }

        if (PRINT_DATA) {
            printf("All workers have joined.\n");
        }

    }

    if (PROGRESSBAR) {
        char message[100];
        snprintf(message, sizeof(message), "Levenberg Marquardt REDUCED_TIMESTEPS: Antenna Size = %ld\n", *calibrator->N);
        stop_free_pb(progress_bar, message);
    }

}
/**
 * Worker used to parallelize function solve_G_LM
*/
void* worker_solve_G_LM_REDUCED_TIMESTEPS(void* args) {
    struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
    void** arguments = thread_args->args;
    struct Calibrator* calibrator = arguments[0];
    size_t* worker_queue = arguments[1];
    pthread_mutex_t* mutex_work_queue = arguments[2];
    struct ProgressBar* progress_bar = arguments[3];
    size_t* imax = arguments[4];
    double* tua = arguments[5];
    double* l_init = arguments[6];
    double* v = arguments[7];

    // printf("Worker %ld\n",*thread_args->worker_id);

    while (1) {

        pthread_mutex_lock(mutex_work_queue);
        size_t t = *worker_queue;
        (*worker_queue)++;
        pthread_mutex_unlock(mutex_work_queue);
        if (t < RANDOM_TIMESTEP_REDUCED_AMOUNT) {
            int t_reduced_value = *(int*)get_element_a(calibrator->random_timesteps_reduced, (size_t[]){t});

            //Need a lock
            if (PROGRESSBAR) {
                lock_pb(progress_bar);
                update_pb(progress_bar, &(size_t){t+1});
                unlock_pb(progress_bar);
            }

            double l = *l_init;

            struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t_reduced_value});
            struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t_reduced_value});

            fill_diagonal_a(M_t, &(double complex){0 + 0*I});
            fill_diagonal_a(D_t, &(double complex){0 + 0*I});

            struct array* g_old = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
            struct array* g_new = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
            
            //creating arrays to hold damping factors.
            struct array* damping_g = NULL;
            struct array* damping_g_old = NULL;
            
            if (ALL_DATA) {
                damping_g = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                damping_g_old = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
            }

            size_t i = 0;

            for (i = 0; i < *imax; i++) {
                copy_over_a(g_old, g_new, (int[]){-1}, (int[]){-1});
                struct array* J = compute_J(calibrator, M_t, g_old);
                struct array* r = compute_r(calibrator, M_t, D_t, g_old);
                struct array* J_conj = conjugate_a(J);
                struct array* J_conj_transpose = transpose_a(J_conj);
                struct array* Hessian = dot_a(J_conj_transpose, J);
                struct array* gradient = dot_a(J_conj_transpose, r);
                struct array* ones_array = ones_a(&(size_t){1}, (size_t[]){Hessian->dimensions[0]}, &(enum DataType){DOUBLE});
                struct array* identity = diag_a(ones_array, (int[]){-1});
                struct array* diag_only_hessian_array = multiply_a(Hessian, identity);
                struct array* Hessian_diag_scaled = multiply_constant_a(diag_only_hessian_array, &l, &(enum DataType){DOUBLE});
                struct array* Hessian_scaled = addition_a(Hessian, Hessian_diag_scaled);
                struct array* p_inv =  p_inv_a(Hessian_scaled);
                struct array* update_step = dot_a(p_inv, gradient);

                //setting g_new
                free_a(g_new);
                g_new = addition_a(g_old, update_step);

                struct array* new_r = compute_r(calibrator, M_t, D_t, g_new);
                double* norm_r = norm_a(r);
                double* norm_new_r = norm_a(new_r);

                if (*norm_r < * norm_new_r) {
                    size_t count = 0;
                    while(*norm_r < * norm_new_r && count < 20) {
                        count += 1;
                        l = l * *v;

                        free_a(update_step);
                        free_a(p_inv);
                        free_a(Hessian_scaled);
                        free_a(Hessian_diag_scaled);

                        Hessian_diag_scaled = multiply_constant_a(diag_only_hessian_array, &l, &(enum DataType){DOUBLE});
                        Hessian_scaled = addition_a(Hessian, Hessian_diag_scaled);
                        p_inv =  p_inv_a(Hessian_scaled);
                        update_step = dot_a(p_inv, gradient);

                        free_a(g_new);
                        g_new = addition_a(g_old, update_step);
                        free_a(new_r);
                        new_r = compute_r(calibrator, M_t, D_t, g_new);
                        free(norm_new_r);
                        norm_new_r = norm_a(new_r);
                    }
                } else {
                    l = l / *v;
                }

                free(norm_r);
                free(norm_new_r);

                free_a(new_r);

                if (ALL_DATA) {
                    if (i < MAX_ITERATIONS_CAPTURE) {
                        //saving damping_g
                        set_element_a(damping_g, (size_t[]){i}, &(double){l});
                        //saving damping_g_old
                        set_element_a(damping_g_old, (size_t[]){i}, &(double){0.0});
                        //saving all_gains
                        lock_dc(calibrator->datacollector);
                        add_all_gain_dc(calibrator->datacollector, &(size_t){t}, &(size_t){i}, g_new);
                        unlock_dc(calibrator->datacollector);
                    }
                }

                struct array* subtraction = minus_a(g_new, g_old);
                double* norm1 = (double*)norm_a(subtraction);
                double* norm2 = (double*)norm_a(g_new);

                if (*norm1 / *norm2 <= *tua) {
                    free_a(subtraction);
                    free(norm1);
                    free(norm2);
                    free_a(update_step);
                    free_a(p_inv);
                    free_a(Hessian_scaled);
                    free_a(Hessian_diag_scaled);
                    free_a(diag_only_hessian_array);
                    free_a(identity);
                    free_a(ones_array);
                    free_a(gradient);
                    free_a(Hessian);
                    free_a(J_conj_transpose);
                    free_a(J_conj);
                    free_a(r);
                    free_a(J);
                    break;
                }

                free_a(subtraction);
                free(norm1);
                free(norm2);
                free_a(update_step);
                free_a(p_inv);
                free_a(Hessian_scaled);
                free_a(Hessian_diag_scaled);
                free_a(diag_only_hessian_array);
                free_a(identity);
                free_a(ones_array);
                free_a(gradient);
                free_a(Hessian);
                free_a(J_conj_transpose);
                free_a(J_conj);
                free_a(r);
                free_a(J);
            }

            if (ALL_DATA) {
                //truncating damping arrays to save them in datacollector
                if (i >= MAX_ITERATIONS_CAPTURE) {
                    //no need for truncation
                    lock_dc(calibrator->datacollector);
                    add_damping_g_dc(calibrator->datacollector, &(size_t){t}, damping_g);
                    add_damping_g_old_dc(calibrator->datacollector, &(size_t){t}, damping_g_old);
                    unlock_dc(calibrator->datacollector);
                } else {
                    struct array* damping_g_trunc = zeros_a(&(size_t){1}, (size_t[]){i + 1}, &(enum DataType){DOUBLE});
                    struct array* damping_g_old_trunc = zeros_a(&(size_t){1}, (size_t[]){i + 1}, &(enum DataType){DOUBLE});

                    for (int j = 0; j < i + 1; j++) {
                        set_element_a(damping_g_trunc, &(size_t){j}, &(double){*(double*)get_element_a(damping_g, &(size_t){j})});
                        set_element_a(damping_g_old_trunc, &(size_t){j}, &(double){*(double*)get_element_a(damping_g_old, &(size_t){j})});
                    }
                    lock_dc(calibrator->datacollector);
                    add_damping_g_dc(calibrator->datacollector, &(size_t){t}, damping_g_trunc);
                    add_damping_g_old_dc(calibrator->datacollector, &(size_t){t}, damping_g_old_trunc);
                    unlock_dc(calibrator->datacollector);

                    free_a(damping_g_trunc);
                    free_a(damping_g_old_trunc);
                }
            }

            struct array* residual = compute_r(calibrator, M_t, D_t, g_new);
            lock_dc(calibrator->datacollector);
            add_last_residual_dc(calibrator->datacollector, &(size_t){t}, residual);
            unlock_dc(calibrator->datacollector);
            free_a(residual);
            add_data_dc(calibrator, M_t, D_t, g_new, &t, &i);

            free_a(M_t);
            free_a(D_t);
            free_a(g_old);
            
            struct array* g_short = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});

            for (size_t i = 0; i < *calibrator->N; i++) {
                set_element_a(g_short, (size_t[]){i}, (double complex*)get_element_a(g_new, (size_t[]){i}));
            }

            struct array* g_conj = conjugate_a(g_short);
            struct array* g_outer = outer_a(g_short, g_conj);

            lock_a(calibrator->G);
            copy_over_a(calibrator->G, g_outer, (int[]){-1,-1,t_reduced_value}, (int[]){-1,-1});
            unlock_a(calibrator->G);

            free_a(g_short);
            free_a(g_outer);
            free_a(g_conj);
            free_a(g_new);


            if (ALL_DATA) {
                free_a(damping_g);
                free_a(damping_g_old);
            }
        } else {
            //All work is done.
            break;
        }
    }

    pthread_exit(NULL);
}