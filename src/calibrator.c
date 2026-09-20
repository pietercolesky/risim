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
  #include <string.h>

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
   *  - Array struct pointer
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
   *  - Array struct pointer
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
   *  - Array struct pointer
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
  
  /**
   * Approximated Hessian
   * RETURNS:
   *  - NONE
   * INPUTS:
   *  - Calibtrator struct pointer
   *  - Hessian Array struct pointer
  */
  struct array* approx_hessian(struct Calibrator* calibrator, struct array* hessian) {
  
      struct array* A = zeros_a(&(size_t){2}, (size_t[]){*calibrator->N, *calibrator->N}, &(enum DataType){COMPLEX});
      for (size_t i = 0; i < *calibrator->N; i++) {
          for (size_t j = 0; j < *calibrator->N; j++) {
              set_element_a(A, (size_t[]){i,j}, get_element_a(hessian, (size_t[]){i,j}));
          }
      }
  
      struct array* B = zeros_a(&(size_t){2}, (size_t[]){*calibrator->N, *calibrator->N}, &(enum DataType){COMPLEX});
      for (size_t i = 0; i < *calibrator->N; i++) {
          for (size_t j = 0; j < *calibrator->N; j++) {
              set_element_a(B, (size_t[]){i,j}, get_element_a(hessian, (size_t[]){i,j + *calibrator->N}));
          }
      }
  
      struct array* real_A = real_a(A);
      struct array* A_inv = diag_reciprocal_a(real_A);
      struct array* B_inv = dot_a(B, A_inv);
      struct array* B_inv_2 = dot_a(A_inv, B_inv);
      struct array* B_inv_3 = multiply_constant_a(B_inv_2, &(double){-1}, &(enum DataType){DOUBLE});
      struct array* B_inv_conj = conjugate_a(B_inv_3);
  
      struct array* H = zeros_a(&(size_t){2}, (size_t[]){2 * *calibrator->N, 2 * *calibrator->N}, &(enum DataType){COMPLEX});
  
      for (size_t i = 0; i < *calibrator->N; i++) {
          for (size_t j = 0; j < *calibrator->N; j++) {
              double a_inv_i_j = *(double*)get_element_a(A_inv, (size_t[]){i,j});
              double complex a_inv_i_j_complex = (double complex)((0.0 + 0.0 * I) + a_inv_i_j);
              set_element_a(H, (size_t[]){i,j}, &a_inv_i_j_complex);
              set_element_a(H, (size_t[]){i,j + *calibrator->N}, get_element_a(B_inv_3, (size_t[]){i,j}));
              set_element_a(H, (size_t[]){i + *calibrator->N,j}, get_element_a(B_inv_conj, (size_t[]){i,j}));
              set_element_a(H, (size_t[]){i + *calibrator->N,j + *calibrator->N}, &a_inv_i_j_complex);
          }
      }
      
      free_a(B_inv_conj);
      free_a(B_inv_3);
      free_a(B_inv_2);
      free_a(B_inv);
      free_a(B);
      free_a(A_inv);
      free_a(real_A);
      free_a(A);
  
      return H;
  }
  
  
  // /**
  //  *******************************************************************************************************************
  //  * First Order Algorithms
  //  *******************************************************************************************************************
  // */
  
  /**
   * ADAM
   * RETURNS:
   *  - NONE
   * INPUT:
   *  - imax maximum amount of iterations.
   *  - tau stopping criteria.
   *  - alpha - damping factor (Default = 0.001)
   *  - B1 - momentum decay hyperparameter (Default = 0.9)
   *  - B2 - scaling decay hyperparameter (Default = 0.999)
  */
  void solve_G_ADAM(struct Calibrator* calibrator, size_t* imax, double* tua, double* alpha, double* B1, double* B2) {
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
  
              struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
              struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});
  
              struct array* g_old = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              struct array* g_new = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              struct array* s = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              struct array* mv = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              
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
                  struct array* gradient = dot_a(J_conj_transpose, r);
  
                  // Calculating mv
                  struct array* mv_first = multiply_constant_a(mv, B1, &(enum DataType){DOUBLE});
                  struct array* mv_second = multiply_constant_a(gradient, &(double){1 - *B1}, &(enum DataType){DOUBLE});
                  free_a(mv);
                  mv = addition_a(mv_first, mv_second);
                  free_a(mv_first);
                  free_a(mv_second);
  
                  // Calculating s
                  struct array* s_first = multiply_constant_a(s, B2, &(enum DataType){DOUBLE});
                  struct array* absolute_gradient = abs_a(gradient);
                  struct array* absolute_gradient_square = multiply_a(absolute_gradient, absolute_gradient);
                  struct array* s_second = multiply_constant_a(absolute_gradient_square, &(double){1 - *B2}, &(enum DataType){DOUBLE});
                  free_a(s);
                  s = addition_a(s_first, s_second);
                  free_a(s_second);
                  free_a(absolute_gradient_square);
                  free_a(absolute_gradient);
                  free_a(s_first);
  
                  // Calculating mv_hat
                  double mv_hat_denominator = 1/(1 - pow(*B1, (double)(i+1)));
                  struct array* mv_hat = multiply_constant_a(mv, &(double){mv_hat_denominator}, &(enum DataType){DOUBLE});
  
                  // Calculating s_hat
                  double s_hat_denominator = 1/(1 - pow(*B2, (double)(i + 1)));
                  struct array* s_hat = multiply_constant_a(s, &(double){s_hat_denominator}, &(enum DataType){DOUBLE});
  
                  // Calculating update step
                  struct array* numerator = multiply_constant_a(mv_hat, alpha, &(enum DataType){DOUBLE});
                  struct array* sqrt_s_hat = sqrt_a(s_hat);
                  struct array* denominator = addition_constant_a(sqrt_s_hat, &(double){EPSILON_ADAM}, &(enum DataType){DOUBLE});
  
                  struct array* update_step = divide_a(numerator, denominator);
  
                  free_a(denominator);
                  free_a(sqrt_s_hat);
                  free_a(numerator);
                  free_a(mv_hat);
                  free_a(s_hat);
                  //setting g_new
                  free_a(g_new);
                  g_new = addition_a(g_old, update_step);
                  free_a(update_step);
  
  
                  if (ALL_DATA) {
                      if (check_random_timestep && i < MAX_ITERATIONS_CAPTURE) {
                          //saving damping_g
                          set_element_a(damping_g, (size_t[]){i}, &(double){0.0});
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
                      free_a(gradient);
                      free_a(J_conj_transpose);
                      free_a(J_conj);
                      free_a(r);
                      free_a(J);
                      break;
                  }
  
                  free_a(subtraction);
                  free(norm1);
                  free(norm2);
                  free_a(gradient);
                  free_a(J_conj_transpose);
                  free_a(J_conj);
                  free_a(r);
                  free_a(J);
              }
  
              free_a(mv);
              free_a(s);
  
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
          size_t num_args = 9;
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
              args[i]->args[6] = alpha;
              args[i]->args[7] = B1;
              args[i]->args[8] = B2;
  
              pthread_create(&threads[i], NULL, worker_solve_G_ADAM, args[i]);
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
          snprintf(message, sizeof(message), "ADAM: Antenna Size = %ld\n", *calibrator->N);
          stop_free_pb(progress_bar, message);
      }
  
  }
  
  /**
   * Worker used to parallelize function solve_G_ADAM
  */
  void* worker_solve_G_ADAM(void* args) {
      struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
      void** arguments = thread_args->args;
      struct Calibrator* calibrator = arguments[0];
      size_t* worker_queue = arguments[1];
      pthread_mutex_t* mutex_work_queue = arguments[2];
      struct ProgressBar* progress_bar = arguments[3];
      size_t* imax = arguments[4];
      double* tua = arguments[5];
      double* alpha = arguments[6];
      double* B1 = arguments[7];
      double* B2 = arguments[8];
  
      while (1) {
  
          pthread_mutex_lock(mutex_work_queue);
          size_t t = *worker_queue;
          (*worker_queue)++;
          pthread_mutex_unlock(mutex_work_queue);
  
          if (t < *calibrator->n_steps) {
              if (PROGRESSBAR) {
                  lock_pb(progress_bar);
                  update_pb(progress_bar, &(size_t){t+1});
                  unlock_pb(progress_bar);
              }
  
              struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
              struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});
  
              struct array* g_old = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              struct array* g_new = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              struct array* s = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              struct array* mv = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              
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
                  struct array* gradient = dot_a(J_conj_transpose, r);
  
                  // Calculating mv
                  struct array* mv_first = multiply_constant_a(mv, B1, &(enum DataType){DOUBLE});
                  struct array* mv_second = multiply_constant_a(gradient, &(double){1 - *B1}, &(enum DataType){DOUBLE});
                  free_a(mv);
                  mv = addition_a(mv_first, mv_second);
                  free_a(mv_first);
                  free_a(mv_second);
  
                  // Calculating s
                  struct array* s_first = multiply_constant_a(s, B2, &(enum DataType){DOUBLE});
                  struct array* absolute_gradient = abs_a(gradient);
                  struct array* absolute_gradient_square = multiply_a(absolute_gradient, absolute_gradient);
                  struct array* s_second = multiply_constant_a(absolute_gradient_square, &(double){1 - *B2}, &(enum DataType){DOUBLE});
                  free_a(s);
                  s = addition_a(s_first, s_second);
                  free_a(s_second);
                  free_a(absolute_gradient_square);
                  free_a(absolute_gradient);
                  free_a(s_first);
  
                  // Calculating mv_hat
                  double mv_hat_denominator = 1/(1 - pow(*B1, (double)(i+1)));
                  struct array* mv_hat = multiply_constant_a(mv, &(double){mv_hat_denominator}, &(enum DataType){DOUBLE});
  
                  // Calculating s_hat
                  double s_hat_denominator = 1/(1 - pow(*B2, (double)(i + 1)));
                  struct array* s_hat = multiply_constant_a(s, &(double){s_hat_denominator}, &(enum DataType){DOUBLE});
  
                  // Calculating update step
                  struct array* numerator = multiply_constant_a(mv_hat, alpha, &(enum DataType){DOUBLE});
                  struct array* sqrt_s_hat = sqrt_a(s_hat);
                  struct array* denominator = addition_constant_a(sqrt_s_hat, &(double){EPSILON_ADAM}, &(enum DataType){DOUBLE});
  
                  struct array* update_step = divide_a(numerator, denominator);
  
                  free_a(denominator);
                  free_a(sqrt_s_hat);
                  free_a(numerator);
                  free_a(mv_hat);
                  free_a(s_hat);
                  //setting g_new
                  free_a(g_new);
                  g_new = addition_a(g_old, update_step);
                  free_a(update_step);
  
  
                  if (ALL_DATA) {
                      if (check_random_timestep && i < MAX_ITERATIONS_CAPTURE) {
                          //saving damping_g
                          set_element_a(damping_g, (size_t[]){i}, &(double){0.0});
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
                      free_a(gradient);
                      free_a(J_conj_transpose);
                      free_a(J_conj);
                      free_a(r);
                      free_a(J);
                      break;
                  }
  
                  free_a(subtraction);
                  free(norm1);
                  free(norm2);
                  free_a(gradient);
                  free_a(J_conj_transpose);
                  free_a(J_conj);
                  free_a(r);
                  free_a(J);
              }
  
              free_a(mv);
              free_a(s);
  
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
  
  
  /**
   * Computes J^H @ r directly from M_t, D_t, and g without materialising J or r.
   * For each antenna p:
   *      N_p = sum_{q != p} conj(m_pq) * g_q * d_pq
   *      D_p = sum_{q != p} |m_pq|^2 * |g_q|^2
   *      grad[p]     = N_p - g_p * D_p
   *      grad[p + N] = conj(grad[p])
   * See python/gradient_without_r/calibrator.py (gradient_without_residual).
   */
  static struct array* compute_gradient_without_r(struct Calibrator* calibrator, struct array* M_t, struct array* D_t, struct array* g) {
      size_t N_ant = *calibrator->N;
      struct array* gradient = zeros_a(&(size_t){1}, (size_t[]){N_ant * 2}, &(enum DataType){COMPLEX});
  
      for (size_t p = 0; p < N_ant; p++) {
          double complex N_p = 0.0 + 0.0*I;
          double complex D_p = 0.0 + 0.0*I;
  
          for (size_t q = 0; q < N_ant; q++) {
              if (p == q) {
                  continue;
              }
              double complex m_pq = *(double complex*)get_element_a(M_t, (size_t[]){p, q});
              double complex d_pq = *(double complex*)get_element_a(D_t, (size_t[]){p, q});
              double complex g_q  = *(double complex*)get_element_a(g,   (size_t[]){q});
  
              N_p += conj(m_pq) * g_q * d_pq;
  
              double abs_m = cabs(m_pq);
              double abs_g = cabs(g_q);
              D_p += (abs_m * abs_m) * (abs_g * abs_g);
          }
  
          double complex g_p = *(double complex*)get_element_a(g, (size_t[]){p});
          double complex grad_p = N_p - g_p * D_p;
  
          set_element_a(gradient, (size_t[]){p}, &grad_p);
          double complex grad_p_conj = conj(grad_p);
          set_element_a(gradient, (size_t[]){p + N_ant}, &grad_p_conj);
      }
  
      return gradient;
  }
  
  
  /**
   * ADAM Without R
   * Same algorithm as solve_G_ADAM but computes the gradient directly from
   * m, d, g via compute_gradient_without_r instead of materialising J and r.
  */
  void solve_G_ADAM_without_r(struct Calibrator* calibrator, size_t* imax, double* tua, double* alpha, double* B1, double* B2) {
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
  
              struct array* g_old = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              struct array* g_new = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              struct array* s = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              struct array* mv = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
  
              //creating arrays to hold damping factors.
              struct array* damping_g = NULL;
              struct array* damping_g_old = NULL;
              int t_int = t;
              bool check_random_timestep = false;
              int index_random_timestep = 0;
  
              if (ALL_DATA) {
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
                  struct array* gradient = compute_gradient_without_r(calibrator, M_t, D_t, g_old);
  
                  // Calculating mv
                  struct array* mv_first = multiply_constant_a(mv, B1, &(enum DataType){DOUBLE});
                  struct array* mv_second = multiply_constant_a(gradient, &(double){1 - *B1}, &(enum DataType){DOUBLE});
                  free_a(mv);
                  mv = addition_a(mv_first, mv_second);
                  free_a(mv_first);
                  free_a(mv_second);            
  
                  // Calculating s
                  struct array* s_first = multiply_constant_a(s, B2, &(enum DataType){DOUBLE});
                  struct array* absolute_gradient = abs_a(gradient);
                  struct array* absolute_gradient_square = multiply_a(absolute_gradient, absolute_gradient);
                  struct array* s_second = multiply_constant_a(absolute_gradient_square, &(double){1 - *B2}, &(enum DataType){DOUBLE});
                  free_a(s);
                  s = addition_a(s_first, s_second);
                  free_a(s_second);
                  free_a(absolute_gradient_square);
                  free_a(absolute_gradient);
                  free_a(s_first);
  
                  // Calculating mv_hat
                  double mv_hat_denominator = 1/(1 - pow(*B1, (double)(i+1)));
                  struct array* mv_hat = multiply_constant_a(mv, &(double){mv_hat_denominator}, &(enum DataType){DOUBLE});
  
                  // Calculating s_hat
                  double s_hat_denominator = 1/(1 - pow(*B2, (double)(i + 1)));
                  struct array* s_hat = multiply_constant_a(s, &(double){s_hat_denominator}, &(enum DataType){DOUBLE});
  
                  // Calculating update step
                  struct array* numerator = multiply_constant_a(mv_hat, alpha, &(enum DataType){DOUBLE});
                  struct array* sqrt_s_hat = sqrt_a(s_hat);
                  struct array* denominator = addition_constant_a(sqrt_s_hat, &(double){EPSILON_ADAM}, &(enum DataType){DOUBLE});
  
                  struct array* update_step = divide_a(numerator, denominator);
  
                  free_a(denominator);
                  free_a(sqrt_s_hat);
                  free_a(numerator);
                  free_a(mv_hat);
                  free_a(s_hat);
                  //setting g_new
                  free_a(g_new);
                  g_new = addition_a(g_old, update_step);
                  free_a(update_step);
  
  
                  if (ALL_DATA) {
                      if (check_random_timestep && i < MAX_ITERATIONS_CAPTURE) {
                          set_element_a(damping_g, (size_t[]){i}, &(double){0.0});
                          set_element_a(damping_g_old, (size_t[]){i}, &(double){0.0});
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
                      free_a(gradient);
                      break;
                  }
  
                  free_a(subtraction);
                  free(norm1);
                  free(norm2);
                  free_a(gradient);
              }
  
              free_a(mv);
              free_a(s);
  
              if (ALL_DATA) {
                  if (check_random_timestep) {
                      if (i >= MAX_ITERATIONS_CAPTURE) {
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
          pthread_t threads[*calibrator->num_workers];
          struct ThreadArgs* args[*calibrator->num_workers];
          size_t work_queue = 0;
          pthread_mutex_t mutex_work_queue = PTHREAD_MUTEX_INITIALIZER;
          size_t num_args = 9;
          size_t i;
  
          for (i = 0; i < *calibrator->num_workers; i++) {
              args[i] = init_ta(&i, &num_args);
              args[i]->args[0] = calibrator;
              args[i]->args[1] = &work_queue;
              args[i]->args[2] = &mutex_work_queue;
              args[i]->args[3] = progress_bar;
              args[i]->args[4] = imax;
              args[i]->args[5] = tua;
              args[i]->args[6] = alpha;
              args[i]->args[7] = B1;
              args[i]->args[8] = B2;
  
              pthread_create(&threads[i], NULL, worker_solve_G_ADAM_without_r, args[i]);
          }
  
          for (i = 0; i < *calibrator->num_workers; i++) {
              pthread_join(threads[i], NULL);
          }
  
          for (i = 0; i < *calibrator->num_workers; i++) {
              free_ta(args[i]);
          }
  
          if (PRINT_DATA) {
              printf("All workers have joined.\n");
          }
      }
      if (PROGRESSBAR) {
          char message[100];
          snprintf(message, sizeof(message), "ADAM Without R: Antenna Size = %ld\n", *calibrator->N);
          stop_free_pb(progress_bar, message);
      }
  }
  
  /**
   * Worker used to parallelize function solve_G_ADAM_without_r
  */
  void* worker_solve_G_ADAM_without_r(void* args) {
      struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
      void** arguments = thread_args->args;
      struct Calibrator* calibrator = arguments[0];
      size_t* worker_queue = arguments[1];
      pthread_mutex_t* mutex_work_queue = arguments[2];
      struct ProgressBar* progress_bar = arguments[3];
      size_t* imax = arguments[4];
      double* tua = arguments[5];
      double* alpha = arguments[6];
      double* B1 = arguments[7];
      double* B2 = arguments[8];
  
      while (1) {
  
          pthread_mutex_lock(mutex_work_queue);
          size_t t = *worker_queue;
          (*worker_queue)++;
          pthread_mutex_unlock(mutex_work_queue);
  
          if (t < *calibrator->n_steps) {
              if (PROGRESSBAR) {
                  lock_pb(progress_bar);
                  update_pb(progress_bar, &(size_t){t+1});
                  unlock_pb(progress_bar);
              }
  
              struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
              struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});
  
              struct array* g_old = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              struct array* g_new = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              struct array* s = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
              struct array* mv = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){COMPLEX});
  
              struct array* damping_g = NULL;
              struct array* damping_g_old = NULL;
              int t_int = t;
              bool check_random_timestep = false;
              int index_random_timestep = 0;
  
              if (ALL_DATA) {
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
                  struct array* gradient = compute_gradient_without_r(calibrator, M_t, D_t, g_old);
  
                  // Calculating mv
                  struct array* mv_first = multiply_constant_a(mv, B1, &(enum DataType){DOUBLE});
                  struct array* mv_second = multiply_constant_a(gradient, &(double){1 - *B1}, &(enum DataType){DOUBLE});
                  free_a(mv);
                  mv = addition_a(mv_first, mv_second);
                  free_a(mv_first);
                  free_a(mv_second);
  
                  // Calculating s
                  struct array* s_first = multiply_constant_a(s, B2, &(enum DataType){DOUBLE});
                  struct array* absolute_gradient = abs_a(gradient);
                  struct array* absolute_gradient_square = multiply_a(absolute_gradient, absolute_gradient);
                  struct array* s_second = multiply_constant_a(absolute_gradient_square, &(double){1 - *B2}, &(enum DataType){DOUBLE});
                  free_a(s);
                  s = addition_a(s_first, s_second);
                  free_a(s_second);
                  free_a(absolute_gradient_square);
                  free_a(absolute_gradient);
                  free_a(s_first);
  
                  // Calculating mv_hat
                  double mv_hat_denominator = 1/(1 - pow(*B1, (double)(i+1)));
                  struct array* mv_hat = multiply_constant_a(mv, &(double){mv_hat_denominator}, &(enum DataType){DOUBLE});
  
                  // Calculating s_hat
                  double s_hat_denominator = 1/(1 - pow(*B2, (double)(i + 1)));
                  struct array* s_hat = multiply_constant_a(s, &(double){s_hat_denominator}, &(enum DataType){DOUBLE});
  
                  // Calculating update step
                  struct array* numerator = multiply_constant_a(mv_hat, alpha, &(enum DataType){DOUBLE});
                  struct array* sqrt_s_hat = sqrt_a(s_hat);
                  struct array* denominator = addition_constant_a(sqrt_s_hat, &(double){EPSILON_ADAM}, &(enum DataType){DOUBLE});
  
                  struct array* update_step = divide_a(numerator, denominator);
  
                  free_a(denominator);
                  free_a(sqrt_s_hat);
                  free_a(numerator);
                  free_a(mv_hat);
                  free_a(s_hat);
                  //setting g_new
                  free_a(g_new);
                  g_new = addition_a(g_old, update_step);
                  free_a(update_step);
  
  
                  if (ALL_DATA) {
                      if (check_random_timestep && i < MAX_ITERATIONS_CAPTURE) {
                          set_element_a(damping_g, (size_t[]){i}, &(double){0.0});
                          set_element_a(damping_g_old, (size_t[]){i}, &(double){0.0});
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
                      free_a(gradient);
                      break;
                  }
  
                  free_a(subtraction);
                  free(norm1);
                  free(norm2);
                  free_a(gradient);
              }
  
              free_a(mv);
              free_a(s);
  
              if (ALL_DATA) {
                  if (check_random_timestep) {
                      if (i >= MAX_ITERATIONS_CAPTURE) {
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
  
  /**
   * Levenberg Marquardt Diagonal Hessian
   * RETURNS:
   *  - NONE
   * INPUT:
   *  - imax maximum amount of iterations.
   *  - tau stopping criteria.
   *  - l1 - 
   *  - l2 -
  */
  void solve_G_LM_Diag_Hessian_Lambda(struct Calibrator* calibrator, size_t* imax, double* tua, double* l1, double* l2) {
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
              double l = 0;
  
              for (i = 0; i < *imax; i++) {
  
                  copy_over_a(g_old, g_new, (int[]){-1}, (int[]){-1});
                  struct array* J = compute_J(calibrator, M_t, g_old);
                  struct array* r = compute_r(calibrator, M_t, D_t, g_old);
  
                  if (i %2 == 0) {
                      l = *l2;
                  } else {
                      l = *l1;
                  }
  
                  struct array* J_conj = conjugate_a(J);
                  struct array* J_conj_transpose = transpose_a(J_conj);
                  struct array* Hessian = dot_a(J_conj_transpose, J);
                  struct array* ones_array = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){DOUBLE});
                  struct array* l_array = multiply_constant_a(ones_array, &(double){l}, &(enum DataType){DOUBLE});
                  struct array* diag_l = diag_a(l_array, (int[]){-1});
                  struct array* Hessian_diag_scaled = multiply_a(Hessian, diag_l);
                  struct array* Hessian_scaled = addition_a(Hessian, Hessian_diag_scaled);
                  struct array* hessian_diag_inverse = diag_hadamard_inverse_a(Hessian_scaled);
                  struct array* hessian_diag_inverse_dot_product = dot_a(hessian_diag_inverse, J_conj_transpose);
  
                  struct array* gradient = dot_a(hessian_diag_inverse_dot_product, r);
  
                  free_a(g_new);
                  g_new = addition_a(g_old, gradient);
  
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
                      free_a(gradient);
                      free_a(hessian_diag_inverse_dot_product);
                      free_a(hessian_diag_inverse);
                      free_a(Hessian_scaled);
                      free_a(Hessian_diag_scaled);
                      free_a(diag_l);
                      free_a(l_array);
                      free_a(ones_array);
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
                  free_a(gradient);
                  free_a(hessian_diag_inverse_dot_product);
                  free_a(hessian_diag_inverse);
                  free_a(Hessian_scaled);
                  free_a(Hessian_diag_scaled);
                  free_a(diag_l);
                  free_a(l_array);
                  free_a(ones_array);
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
              args[i]->args[6] = l1;
              args[i]->args[7] = l2;
  
              pthread_create(&threads[i], NULL, worker_solve_G_LM_Diag_Hessian_Lambda, args[i]);
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
          snprintf(message, sizeof(message), "Levenberg Marquardt Diagonal Hessian Lambda: Antenna Size = %ld\n", *calibrator->N);
          stop_free_pb(progress_bar, message);
      }
  
  }
  
  /**
   * Worker used to parallelize function solve_G_LM_Diag_Hessian_Lambda
  */
  void* worker_solve_G_LM_Diag_Hessian_Lambda(void* args) {
      struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
      void** arguments = thread_args->args;
      struct Calibrator* calibrator = arguments[0];
      size_t* worker_queue = arguments[1];
      pthread_mutex_t* mutex_work_queue = arguments[2];
      struct ProgressBar* progress_bar = arguments[3];
      size_t* imax = arguments[4];
      double* tua = arguments[5];
      double* l1 = arguments[6];
      double* l2 = arguments[7];
  
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
              double l = 0;
  
              for (i = 0; i < *imax; i++) {
  
                  copy_over_a(g_old, g_new, (int[]){-1}, (int[]){-1});
                  struct array* J = compute_J(calibrator, M_t, g_old);
                  struct array* r = compute_r(calibrator, M_t, D_t, g_old);
  
                  if (i %2 == 0) {
                      l = *l2;
                  } else {
                      l = *l1;
                  }
  
                  struct array* J_conj = conjugate_a(J);
                  struct array* J_conj_transpose = transpose_a(J_conj);
                  struct array* Hessian = dot_a(J_conj_transpose, J);
                  struct array* ones_array = ones_a(&(size_t){1}, (size_t[]){*calibrator->N * 2}, &(enum DataType){DOUBLE});
                  struct array* l_array = multiply_constant_a(ones_array, &(double){l}, &(enum DataType){DOUBLE});
                  struct array* diag_l = diag_a(l_array, (int[]){-1});
                  struct array* Hessian_diag_scaled = multiply_a(Hessian, diag_l);
                  struct array* Hessian_scaled = addition_a(Hessian, Hessian_diag_scaled);
                  struct array* hessian_diag_inverse = diag_hadamard_inverse_a(Hessian_scaled);
                  struct array* hessian_diag_inverse_dot_product = dot_a(hessian_diag_inverse, J_conj_transpose);
  
                  struct array* gradient = dot_a(hessian_diag_inverse_dot_product, r);
  
                  free_a(g_new);
                  g_new = addition_a(g_old, gradient);
  
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
                      free_a(gradient);
                      free_a(hessian_diag_inverse_dot_product);
                      free_a(hessian_diag_inverse);
                      free_a(Hessian_scaled);
                      free_a(Hessian_diag_scaled);
                      free_a(diag_l);
                      free_a(l_array);
                      free_a(ones_array);
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
                  free_a(gradient);
                  free_a(hessian_diag_inverse_dot_product);
                  free_a(hessian_diag_inverse);
                  free_a(Hessian_scaled);
                  free_a(Hessian_diag_scaled);
                  free_a(diag_l);
                  free_a(l_array);
                  free_a(ones_array);
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
  
  
  /**
   * Levenberg Marquardt Non Adaptive l1 l2
   * RETURNS:
   *  - NONE
   * INPUT:
   *  - imax maximum amount of iterations.
   *  - tau stopping criteria.
   *  - l1 - 
   *  - l2 -
  */
  void solve_G_LM_Non_Adaptive_l1_l2(struct Calibrator* calibrator, size_t* imax, double* tua, double* l1, double* l2) {
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
              double l = 0;
  
              for (i = 0; i < *imax; i++) {
                  copy_over_a(g_old, g_new, (int[]){-1}, (int[]){-1});
  
                  if (i % 2 == 0) {
                      l = *l2;
                  } else {
                      l = *l1;
                  }
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
                  free_a(g_new);
                  g_new = addition_a(g_old, update_step);
  
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
              args[i]->args[6] = l1;
              args[i]->args[7] = l2;
  
              pthread_create(&threads[i], NULL, worker_solve_G_LM_Non_Adaptive_l1_l2, args[i]);
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
          snprintf(message, sizeof(message), "Levenberg Marquardt Non Adaptive l1 l2: Antenna Size = %ld\n", *calibrator->N);
          stop_free_pb(progress_bar, message);
      }
  
  }
  
  /**
   * Worker used to parallelize function solve_G_LM_Diag_Hessian_Lambda
  */
  void* worker_solve_G_LM_Non_Adaptive_l1_l2(void* args) {
      struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
      void** arguments = thread_args->args;
      struct Calibrator* calibrator = arguments[0];
      size_t* worker_queue = arguments[1];
      pthread_mutex_t* mutex_work_queue = arguments[2];
      struct ProgressBar* progress_bar = arguments[3];
      size_t* imax = arguments[4];
      double* tua = arguments[5];
      double* l1 = arguments[6];
      double* l2 = arguments[7];
  
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
              double l = 0;
  
              for (i = 0; i < *imax; i++) {
                  copy_over_a(g_old, g_new, (int[]){-1}, (int[]){-1});
  
                  if (i % 2 == 0) {
                      l = *l2;
                  } else {
                      l = *l1;
                  }
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
                  free_a(g_new);
                  g_new = addition_a(g_old, update_step);
  
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
   * Stochastic (mini-batch) StEFCal. Identical to solve_G_StEFCal except that the
   * per-antenna update sums over a random subset of b partner antennas instead of
   * all N-1, giving O(N*b) work per iteration instead of O(N^2). The StEFCal
   * update is a self-normalised ratio (numerator and denominator share the same
   * per-term factor), so the (N-1)/b rescaling cancels and the estimate is
   * unbiased in the noise-free case. See extended-research/ for the analysis.
   * RETURNS:
   *  - NONE
   * INPUT:
   *  - imax maximum amount of iterations.
   *  - tau stopping criteria.
   *  - b number of partner antennas sampled per antenna per iteration (batch size).
  */
  void solve_G_StEFCal_stochastic(struct Calibrator* calibrator, size_t* imax, double* tua, size_t* b) {

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
                  index_random_timestep = index_of_element_a(calibrator->random_timesteps, &(int){t_int});
                  if (index_random_timestep >= 0) {
                      check_random_timestep = true;
                  }
                  if(check_random_timestep) {
                      damping_g = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                      damping_g_old = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                  }
              }

              //stochastic sub-sampling state: deterministic per timestep (seeded by t),
              //evolves across iterations, and is thread-safe (lives on the stack).
              unsigned long long rs = 88172645463325252ULL ^ ((unsigned long long)(t + 1));
              size_t bb = (*b < *calibrator->N) ? *b : (*calibrator->N - 1);

              size_t i = 0;

              for (i = 0; i < *imax; i++) {

                  struct array* g_p = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});
                  for (size_t p = 0; p < *calibrator->N; p++) {
                      double complex num = 0;
                      double den = 0;
                      for (size_t j = 0; j < bb; j++) {
                          rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17;          //xorshift
                          size_t q = (size_t)(rs % (unsigned long long)(*calibrator->N - 1));
                          if (q >= p) q++;                                          //sample q != p
                          double complex m_qp = *(double complex*)get_element_a(M_t, (size_t[]){q,p});
                          double complex d_qp = *(double complex*)get_element_a(D_t, (size_t[]){q,p});
                          double complex g_q  = *(double complex*)get_element_a(g, (size_t[]){q});
                          double complex z = g_q * m_qp;                            //z = g_q * m_qp
                          num += conj(d_qp) * z;                                     //matches dot(conj(d_p), z)
                          den += creal(conj(z) * z);                                //|z|^2
                      }
                      double complex gp = (den > 0) ? (num / den)
                                                    : *(double complex*)get_element_a(g, (size_t[]){p});
                      set_element_a(g_p, (size_t[]){p}, &gp);
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
                          set_element_a(damping_g, (size_t[]){i}, &(double){0.0});
                          set_element_a(damping_g_old, (size_t[]){i}, &(double){0.0});
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
                  if (check_random_timestep) {
                      if (i >= MAX_ITERATIONS_CAPTURE) {
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
          pthread_t threads[*calibrator->num_workers];
          struct ThreadArgs* args[*calibrator->num_workers];
          size_t work_queue = 0;
          pthread_mutex_t mutex_work_queue = PTHREAD_MUTEX_INITIALIZER;
          size_t num_args = 7;
          size_t i;

          for (i = 0; i < *calibrator->num_workers; i++) {
              args[i] = init_ta(&i, &num_args);
              args[i]->args[0] = calibrator;
              args[i]->args[1] = &work_queue;
              args[i]->args[2] = &mutex_work_queue;
              args[i]->args[3] = progress_bar;
              args[i]->args[4] = imax;
              args[i]->args[5] = tua;
              args[i]->args[6] = b;

              pthread_create(&threads[i], NULL, worker_solve_G_StEFCal_stochastic, args[i]);
          }

          for (i = 0; i < *calibrator->num_workers; i++) {
              pthread_join(threads[i], NULL);
          }

          for (i = 0; i < *calibrator->num_workers; i++) {
              free_ta(args[i]);
          }

          if (PRINT_DATA) {
              printf("All workers have joined.\n");
          }
      }

      if (PROGRESSBAR) {
          char message[100];
          snprintf(message, sizeof(message), "StEFCal Stochastic: Antenna Size = %ld\n", *calibrator->N);
          stop_free_pb(progress_bar, message);
      }
  }

  /**
   * Worker used to parallelize function solve_G_StEFCal_stochastic
  */
  void* worker_solve_G_StEFCal_stochastic(void* args) {
      struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
      void** arguments = thread_args->args;
      struct Calibrator* calibrator = arguments[0];
      size_t* worker_queue = arguments[1];
      pthread_mutex_t* mutex_work_queue = arguments[2];
      struct ProgressBar* progress_bar = arguments[3];
      size_t* imax = arguments[4];
      double* tua = arguments[5];
      size_t* b = arguments[6];

      while (1) {
          pthread_mutex_lock(mutex_work_queue);
          size_t t = *worker_queue;
          (*worker_queue)++;
          pthread_mutex_unlock(mutex_work_queue);

          if (t < *calibrator->n_steps) {

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

              struct array* damping_g = NULL;
              struct array* damping_g_old = NULL;
              int t_int = t;
              bool check_random_timestep = false;
              int index_random_timestep = 0;

              if (ALL_DATA) {
                  index_random_timestep = index_of_element_a(calibrator->random_timesteps, &(int){t_int});
                  if (index_random_timestep >= 0) {
                      check_random_timestep = true;
                  }
                  if(check_random_timestep) {
                      damping_g = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                      damping_g_old = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                  }
              }

              unsigned long long rs = 88172645463325252ULL ^ ((unsigned long long)(t + 1));
              size_t bb = (*b < *calibrator->N) ? *b : (*calibrator->N - 1);

              size_t i = 0;

              for (i = 0; i < *imax; i++) {

                  struct array* g_p = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});
                  for (size_t p = 0; p < *calibrator->N; p++) {
                      double complex num = 0;
                      double den = 0;
                      for (size_t j = 0; j < bb; j++) {
                          rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17;
                          size_t q = (size_t)(rs % (unsigned long long)(*calibrator->N - 1));
                          if (q >= p) q++;
                          double complex m_qp = *(double complex*)get_element_a(M_t, (size_t[]){q,p});
                          double complex d_qp = *(double complex*)get_element_a(D_t, (size_t[]){q,p});
                          double complex g_q  = *(double complex*)get_element_a(g, (size_t[]){q});
                          double complex z = g_q * m_qp;
                          num += conj(d_qp) * z;
                          den += creal(conj(z) * z);
                      }
                      double complex gp = (den > 0) ? (num / den)
                                                    : *(double complex*)get_element_a(g, (size_t[]){p});
                      set_element_a(g_p, (size_t[]){p}, &gp);
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
                          set_element_a(damping_g, (size_t[]){i}, &(double){0.0});
                          set_element_a(damping_g_old, (size_t[]){i}, &(double){0.0});
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
                  if (check_random_timestep) {
                      if (i >= MAX_ITERATIONS_CAPTURE) {
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
   * Core per-timestep math for NOS-fixed (selreg+NOS-1), in plain C arrays.
   * Mc, Dc are row-major N*N copies of the (diagonal-zeroed) model/data matrices.
   * On entry g holds the initial gains (ones); on exit the converged gains.
   * Implements, per iteration (lambda alternates l1 on odd, l2 on even):
   *   d_i = (1+lam) sum_{k!=i} |g_k|^2 |m_ik|^2
   *   c   = eps / (2 ||g||^2)
   *   a_i = sum_k conj(m_ik) g_k (d_ik - g_i m_ik conj(g_k))
   *   Ai(x) = x/d - (c/(1+c s))(g/d) sum_j conj(g_j) x_j / d_j   (Sherman-Morrison)
   *   Bsel(x)_i = g_i[ sum_{j!=i} g_j |m_ij|^2 x_j - c sum_j g_j x_j ]
   *   dg  = Ai(a) - Ai(Bsel(Ai(conj a)))
   * The rank-1 'c' terms regularise the unobservable gauge mode so the truncated
   * inverse is well defined at lambda=0. See extended-research/nos_lambda0_notes.md.
  */
  static void nos_fixed_timestep(size_t N, const double complex* Mc, const double complex* Dc,
                                 size_t imax, double tua, double l1, double l2, double eps,
                                 double complex* g, size_t* iters_out) {
      double* d     = malloc(N * sizeof(double));
      double* g2    = malloc(N * sizeof(double));
      double complex* a   = malloc(N * sizeof(double complex));
      double complex* x   = malloc(N * sizeof(double complex));   /* scratch */
      double complex* y   = malloc(N * sizeof(double complex));   /* scratch */
      double complex* dg  = malloc(N * sizeof(double complex));

      size_t it = 0;
      for (it = 0; it < imax; it++) {
          double lam = (it % 2 == 0) ? l1 : l2;     /* odd iters (it=0,2,..) use l1 */

          /* |g|^2 and ||g||^2 */
          double sumg2 = 0.0;
          for (size_t i = 0; i < N; i++) { g2[i] = creal(conj(g[i]) * g[i]); sumg2 += g2[i]; }

          /* d_i = (1+lam) sum_k |g_k|^2 |m_ik|^2   (diag of Mc is zero) */
          double sumd = 0.0;
          for (size_t i = 0; i < N; i++) {
              double acc = 0.0;
              const double complex* Mi = Mc + i * N;
              for (size_t k = 0; k < N; k++) acc += g2[k] * creal(conj(Mi[k]) * Mi[k]);
              d[i] = (1.0 + lam) * acc;
              if (d[i] == 0.0) d[i] = 1e-300;
              sumd += d[i];
          }
          /* scale-invariant gauge regularisation: bump gauge eigenvalue ~ eps*mean(d) */
          double mean_d = sumd / (double)N;
          double c = (sumg2 > 0) ? eps * mean_d / sumg2 : 0.0;

          /* a_i = sum_k conj(m_ik) g_k (d_ik - g_i m_ik conj(g_k)) */
          for (size_t i = 0; i < N; i++) {
              double complex acc = 0.0;
              const double complex* Mi = Mc + i * N;
              const double complex* Di = Dc + i * N;
              for (size_t k = 0; k < N; k++) {
                  double complex r = Di[k] - g[i] * Mi[k] * conj(g[k]);
                  acc += conj(Mi[k]) * g[k] * r;
              }
              a[i] = acc;
          }

          /* Sherman-Morrison constants: s = sum |g|^2/d ; denom = 1 + c s */
          double s = 0.0;
          for (size_t i = 0; i < N; i++) s += g2[i] / d[i];
          double denom = 1.0 + c * s;

          /* --- Ai applied to conj(a)  -> x --- */
          /* Ai(v)_i = v_i/d_i - (c/denom)(g_i/d_i) * sum_j conj(g_j) v_j / d_j */
          double complex gHDinv = 0.0;
          for (size_t j = 0; j < N; j++) gHDinv += conj(g[j]) * conj(a[j]) / d[j];
          for (size_t i = 0; i < N; i++)
              x[i] = conj(a[i]) / d[i] - (c / denom) * (g[i] / d[i]) * gHDinv;

          /* --- Bsel applied to x -> y ---  y_i = g_i[ sum_{j!=i} g_j |m_ij|^2 x_j - c sum_j g_j x_j ] */
          double complex gTx = 0.0;
          for (size_t j = 0; j < N; j++) gTx += g[j] * x[j];
          for (size_t i = 0; i < N; i++) {
              double complex bsum = 0.0;
              const double complex* Mi = Mc + i * N;
              for (size_t j = 0; j < N; j++) bsum += g[j] * creal(conj(Mi[j]) * Mi[j]) * x[j];
              y[i] = g[i] * (bsum - c * gTx);
          }

          /* --- Ai applied to y -> x --- */
          gHDinv = 0.0;
          for (size_t j = 0; j < N; j++) gHDinv += conj(g[j]) * y[j] / d[j];
          for (size_t i = 0; i < N; i++)
              x[i] = y[i] / d[i] - (c / denom) * (g[i] / d[i]) * gHDinv;

          /* --- Ai applied to a -> y --- */
          gHDinv = 0.0;
          for (size_t j = 0; j < N; j++) gHDinv += conj(g[j]) * a[j] / d[j];
          for (size_t i = 0; i < N; i++)
              y[i] = a[i] / d[i] - (c / denom) * (g[i] / d[i]) * gHDinv;

          /* dg = Ai(a) - Ai(Bsel(Ai(conj a))) = y - x ; update */
          double nrm_dg = 0.0, nrm_g = 0.0;
          for (size_t i = 0; i < N; i++) {
              dg[i] = y[i] - x[i];
              g[i] += dg[i];
              nrm_dg += creal(conj(dg[i]) * dg[i]);
              nrm_g  += creal(conj(g[i]) * g[i]);
          }
          if (nrm_g > 0 && sqrt(nrm_dg / nrm_g) <= tua) { it++; break; }
      }
      *iters_out = it;
      free(d); free(g2); free(a); free(x); free(y); free(dg);
  }

  /* Build the standard output (residual, datacollector, G) from converged C gains. */
  static void nos_fixed_store(struct Calibrator* calibrator, struct array* M_t, struct array* D_t,
                              const double complex* g, size_t N, size_t t, size_t iters, int locked) {
      struct array* g_struct = zeros_a(&(size_t){1}, (size_t[]){N}, &(enum DataType){COMPLEX});
      for (size_t i = 0; i < N; i++) set_element_a(g_struct, (size_t[]){i}, (void*)&g[i]);
      struct array* g_conj = conjugate_a(g_struct);
      struct array* g_outer = outer_a(g_struct, g_conj);
      struct array* g_full = zeros_a(&(size_t){1}, (size_t[]){N * 2}, &(enum DataType){COMPLEX});
      for (size_t i = 0; i < N; i++) {
          set_element_a(g_full, (size_t[]){i}, (double complex*)get_element_a(g_struct, (size_t[]){i}));
          set_element_a(g_full, (size_t[]){N + i}, (double complex*)get_element_a(g_conj, (size_t[]){i}));
      }
      struct array* residual = compute_r(calibrator, M_t, D_t, g_full);
      if (locked) lock_dc(calibrator->datacollector);
      add_last_residual_dc(calibrator->datacollector, &(size_t){t}, residual);
      if (locked) unlock_dc(calibrator->datacollector);
      free_a(residual);
      add_data_dc(calibrator, M_t, D_t, g_full, &t, &iters);
      free_a(g_full);
      if (locked) lock_a(calibrator->G);
      copy_over_a(calibrator->G, g_outer, (int[]){-1,-1,t}, (int[]){-1,-1});
      if (locked) unlock_a(calibrator->G);
      free_a(g_outer); free_a(g_conj); free_a(g_struct);
  }

  /* Stochastic-adaptive only: push the per-iteration traces captured by
     jacobi_stoch_adaptive_timestep (all_gains = g per iteration, damping_g = phi,
     damping_g_old = 1 - phi) into the datacollector for a random timestep. Kept separate from the
     shared nos_fixed_store so no other NOS-family solver is touched. */
  static void add_stoch_capture_dc(struct Calibrator* calibrator, size_t N, int locked,
                                   int index_random_timestep,
                                   const double* cap_damping, const double complex* cap_gains, size_t cap_count) {
      if (!(ALL_DATA && cap_count > 0)) return;
      struct array* damping_g = zeros_a(&(size_t){1}, (size_t[]){cap_count}, &(enum DataType){DOUBLE});
      struct array* damping_g_old = zeros_a(&(size_t){1}, (size_t[]){cap_count}, &(enum DataType){DOUBLE});
      if (locked) lock_dc(calibrator->datacollector);
      for (size_t k = 0; k < cap_count; k++) {
          set_element_a(damping_g, (size_t[]){k}, &(double){cap_damping[k]});
          set_element_a(damping_g_old, (size_t[]){k}, &(double){1.0 - cap_damping[k]});
          struct array* g_k = zeros_a(&(size_t){1}, (size_t[]){N}, &(enum DataType){COMPLEX});
          for (size_t i = 0; i < N; i++) set_element_a(g_k, (size_t[]){i}, (void*)&cap_gains[k*N + i]);
          add_all_gain_dc(calibrator->datacollector, &(size_t){index_random_timestep}, &(size_t){k}, g_k);
          free_a(g_k);
      }
      add_damping_g_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g);
      add_damping_g_old_dc(calibrator->datacollector, &(size_t){index_random_timestep}, damping_g_old);
      if (locked) unlock_dc(calibrator->datacollector);
      free_a(damping_g); free_a(damping_g_old);
  }

  /* Copy a (diagonal-zeroed) NxN struct-array slice into a row-major C buffer. */
  static void nos_fixed_copy_mat(struct array* X_t, double complex* C, size_t N) {
      for (size_t i = 0; i < N; i++)
          for (size_t j = 0; j < N; j++)
              C[i * N + j] = *(double complex*)get_element_a(X_t, (size_t[]){i, j});
  }

  /**
   * NOS-fixed: NOn-diagonal StEFCal with rank-1 gauge regularisation, which makes
   * it converge at lambda=0 (and the lambda_odd=0 schedule) where plain NOS
   * diverges. Same O(N^2)/iteration class as NOS/StEFCal.
   * INPUT: imax, tau, l1/l2 (alternating damping), eps (gauge-regularisation, ~30).
  */
  void solve_G_NOS_fixed(struct Calibrator* calibrator, size_t* imax, double* tua,
                         double* l1, double* l2, double* eps) {
      if (calibrator->G != NULL) free_a(calibrator->G);
      calibrator->G = zeros_a(calibrator->D->num_dimensions, calibrator->D->dimensions, calibrator->D->type);
      if (calibrator->datacollector != NULL) free_dc(calibrator->datacollector);
      calibrator->datacollector = new_dc(calibrator);

      struct ProgressBar* progress_bar = NULL;
      if (PROGRESSBAR) progress_bar = new_pb(&(size_t){50}, &calibrator->G->dimensions[2]);

      if (!*calibrator->parallel) {
          size_t N = *calibrator->N;
          double complex* Mc = malloc(N * N * sizeof(double complex));
          double complex* Dc = malloc(N * N * sizeof(double complex));
          double complex* g  = malloc(N * sizeof(double complex));
          for (size_t t = 0; t < *calibrator->n_steps; t++) {
              if (PROGRESSBAR) update_pb(progress_bar, &(size_t){t+1});
              struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
              struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});
              fill_diagonal_a(M_t, &(double complex){0 + 0*I});
              fill_diagonal_a(D_t, &(double complex){0 + 0*I});
              nos_fixed_copy_mat(M_t, Mc, N);
              nos_fixed_copy_mat(D_t, Dc, N);
              for (size_t i = 0; i < N; i++) g[i] = 1.0;
              size_t iters = 0;
              nos_fixed_timestep(N, Mc, Dc, *imax, *tua, *l1, *l2, *eps, g, &iters);
              nos_fixed_store(calibrator, M_t, D_t, g, N, t, iters, 0);
              free_a(M_t); free_a(D_t);
          }
          free(Mc); free(Dc); free(g);
      } else {
          pthread_t threads[*calibrator->num_workers];
          struct ThreadArgs* args[*calibrator->num_workers];
          size_t work_queue = 0;
          pthread_mutex_t mutex_work_queue = PTHREAD_MUTEX_INITIALIZER;
          size_t num_args = 9;
          size_t i;
          for (i = 0; i < *calibrator->num_workers; i++) {
              args[i] = init_ta(&i, &num_args);
              args[i]->args[0] = calibrator;
              args[i]->args[1] = &work_queue;
              args[i]->args[2] = &mutex_work_queue;
              args[i]->args[3] = progress_bar;
              args[i]->args[4] = imax;
              args[i]->args[5] = tua;
              args[i]->args[6] = l1;
              args[i]->args[7] = l2;
              args[i]->args[8] = eps;
              pthread_create(&threads[i], NULL, worker_solve_G_NOS_fixed, args[i]);
          }
          for (i = 0; i < *calibrator->num_workers; i++) pthread_join(threads[i], NULL);
          for (i = 0; i < *calibrator->num_workers; i++) free_ta(args[i]);
      }

      if (PROGRESSBAR) {
          char message[100];
          snprintf(message, sizeof(message), "NOS-fixed: Antenna Size = %ld\n", *calibrator->N);
          stop_free_pb(progress_bar, message);
      }
  }

  /**
   * Worker used to parallelize function solve_G_NOS_fixed
  */
  void* worker_solve_G_NOS_fixed(void* args) {
      struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
      void** arguments = thread_args->args;
      struct Calibrator* calibrator = arguments[0];
      size_t* worker_queue = arguments[1];
      pthread_mutex_t* mutex_work_queue = arguments[2];
      struct ProgressBar* progress_bar = arguments[3];
      size_t* imax = arguments[4];
      double* tua = arguments[5];
      double* l1 = arguments[6];
      double* l2 = arguments[7];
      double eps = *(double*)arguments[8];

      size_t N = *calibrator->N;
      double complex* Mc = malloc(N * N * sizeof(double complex));
      double complex* Dc = malloc(N * N * sizeof(double complex));
      double complex* g  = malloc(N * sizeof(double complex));
      while (1) {
          pthread_mutex_lock(mutex_work_queue);
          size_t t = *worker_queue;
          (*worker_queue)++;
          pthread_mutex_unlock(mutex_work_queue);
          if (t >= *calibrator->n_steps) break;
          if (PROGRESSBAR) { lock_pb(progress_bar); update_pb(progress_bar, &(size_t){t+1}); unlock_pb(progress_bar); }
          struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
          struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});
          fill_diagonal_a(M_t, &(double complex){0 + 0*I});
          fill_diagonal_a(D_t, &(double complex){0 + 0*I});
          nos_fixed_copy_mat(M_t, Mc, N);
          nos_fixed_copy_mat(D_t, Dc, N);
          for (size_t i = 0; i < N; i++) g[i] = 1.0;
          size_t iters = 0;
          nos_fixed_timestep(N, Mc, Dc, *imax, *tua, *l1, *l2, eps, g, &iters);
          nos_fixed_store(calibrator, M_t, D_t, g, N, t, iters, 1);
          free_a(M_t); free_a(D_t);
      }
      free(Mc); free(Dc); free(g);
      pthread_exit(NULL);
  }

  /* Generalised Jacobi stochastic adaptive timestep with StEFCal odd/even relaxation:
   *   phi = 1/(1+l2) on even sweeps, 1/(1+l1) on odd (as in solve_G_StEFCal_Lambda).
   * When l1==l2 this is the constant-lambda champion (phi constant). When l1!=l2 the step
   * oscillates, the single-step stall detector misfires, and the batch b balloons toward N-1 —
   * the exact behaviour the (l1,l2) relaxation-justification heatmap is meant to expose. */
  static void jacobi_stoch_adaptive_timestep(size_t N, const double complex* Mc, const double complex* Dc,
                                             size_t imax, double tua, double l1, double l2, size_t b0,
                                             unsigned long long seed,
                                             double complex* g, size_t* iters_out,
                                             double* cap_damping, double complex* cap_gains, size_t* cap_count,
                                             int use_work_equiv, int* cap_b) {
      double complex* gold = malloc(N * sizeof(double complex));
      double complex* gn   = malloc(N * sizeof(double complex));
      size_t bb = b0; if (bb < 2) bb = 2; if (bb > N - 1) bb = N - 1;
      unsigned long long rs = 88172645463325252ULL ^ seed;
      double best = INFINITY; size_t stall = 0; const size_t patience = 6;
      size_t total_b = 0;                                  /* Σ b over sweeps, for work-equiv metric */
      size_t it;
      for (it = 0; it < imax; it++) {
          total_b += bb;                                   /* partners/antenna used this sweep */
          double phi = (it % 2 == 0) ? 1.0/(1.0+l2) : 1.0/(1.0+l1);   /* l1==l2 => constant */
          for (size_t i = 0; i < N; i++) gold[i] = g[i];
          for (size_t p = 0; p < N; p++) {
              const double complex* Mp = Mc + p*N; const double complex* Dp = Dc + p*N;
              double complex num = 0; double den = 0;
              for (size_t j = 0; j < bb; j++) {
                  rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17;
                  size_t q = (size_t)(rs % (unsigned long long)(N - 1));
                  if (q >= p) q++;                                  /* sample q != p */
                  double complex z = Mp[q]*conj(gold[q]);           /* Jacobi: OLD gains */
                  num += Dp[q]*conj(z); den += creal(conj(z)*z);
              }
              gn[p] = (den > 0) ? num/den : gold[p];                /* (N-1)/b cancels in ratio */
          }
          for (size_t i = 0; i < N; i++) g[i] = phi*gn[i] + (1.0-phi)*gold[i];
          /* Per-iteration capture (random timesteps only): relaxation factor phi -> damping_g,
             current gains -> all_gains. Mirrors solve_G_StEFCal_Lambda's capture. */
          if (cap_damping && it < MAX_ITERATIONS_CAPTURE) {
              cap_damping[it] = phi;
              for (size_t i = 0; i < N; i++) cap_gains[it*N + i] = g[i];
          }
          /* Full-length per-iteration subsample-size capture (buffer sized imax by the caller,
             so no MAX_ITERATIONS_CAPTURE cap here — the summed b must be exact for the FLOPS scaling). */
          if (cap_b) cap_b[it] = (int)bb;
          double nd = 0, ng = 0;
          for (size_t i = 0; i < N; i++) { double complex dgi = g[i]-gold[i]; nd += creal(conj(dgi)*dgi); ng += creal(conj(g[i])*g[i]); }
          double step = (ng > 0) ? sqrt(nd/ng) : 0.0;
          if (step <= tua) { it++; break; }
          if (step < best*0.9) { best = step; stall = 0; }
          else {
              stall++;
              if (bb < N - 1) { bb = (2*bb < N - 1) ? 2*bb : N - 1; best = INFINITY; stall = 0; }
              else if (stall >= patience) { it++; break; }
          }
      }
      if (use_work_equiv) {
          *iters_out = (N > 1) ? (size_t)llround((double)total_b / (double)(N - 1)) : it;
      } else {
          *iters_out = it;
      }
      if (cap_count) *cap_count = (it < MAX_ITERATIONS_CAPTURE) ? it : MAX_ITERATIONS_CAPTURE;
      free(gold); free(gn);
  }

  /**
   * Stochastic-adaptive StEFCal (Jacobi stochastic with constant-lambda damping + adaptive batch).
   * O(N*b)/iteration. The two tunable hyperparameters are lam (constant damping, ~0.5) and
   * b0 (initial sub-sample size per antenna; grows automatically when the step stalls).
   * INPUT: imax, tau, lam, b0.
  */
  void solve_G_StEFCal_stochastic_adaptive(struct Calibrator* calibrator, size_t* imax, double* tua,
                                           double* lam, size_t* b0, int* use_work_equiv) {
      if (calibrator->G != NULL) free_a(calibrator->G);
      calibrator->G = zeros_a(calibrator->D->num_dimensions, calibrator->D->dimensions, calibrator->D->type);
      if (calibrator->datacollector != NULL) free_dc(calibrator->datacollector);
      calibrator->datacollector = new_dc(calibrator);

      struct ProgressBar* progress_bar = NULL;
      if (PROGRESSBAR) progress_bar = new_pb(&(size_t){50}, &calibrator->G->dimensions[2]);

      if (!*calibrator->parallel) {
          size_t N = *calibrator->N;
          double complex* Mc = malloc(N*N*sizeof(double complex));
          double complex* Dc = malloc(N*N*sizeof(double complex));
          double complex* g  = malloc(N*sizeof(double complex));
          for (size_t t = 0; t < *calibrator->n_steps; t++) {
              if (PROGRESSBAR) update_pb(progress_bar, &(size_t){t+1});
              struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
              struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});
              fill_diagonal_a(M_t, &(double complex){0 + 0*I});
              fill_diagonal_a(D_t, &(double complex){0 + 0*I});
              nos_fixed_copy_mat(M_t, Mc, N);
              nos_fixed_copy_mat(D_t, Dc, N);
              for (size_t i = 0; i < N; i++) g[i] = 1.0;
              size_t iters = 0;
              jacobi_stoch_adaptive_timestep(N, Mc, Dc, *imax, *tua, *lam, *lam, *b0, t+1, g, &iters, NULL, NULL, NULL, *use_work_equiv, NULL);
              nos_fixed_store(calibrator, M_t, D_t, g, N, t, iters, 0);
              free_a(M_t); free_a(D_t);
          }
          free(Mc); free(Dc); free(g);
      } else {
          pthread_t threads[*calibrator->num_workers];
          struct ThreadArgs* args[*calibrator->num_workers];
          size_t work_queue = 0;
          pthread_mutex_t mutex_work_queue = PTHREAD_MUTEX_INITIALIZER;
          size_t num_args = 9; size_t i;
          for (i = 0; i < *calibrator->num_workers; i++) {
              args[i] = init_ta(&i, &num_args);
              args[i]->args[0] = calibrator; args[i]->args[1] = &work_queue;
              args[i]->args[2] = &mutex_work_queue; args[i]->args[3] = progress_bar;
              args[i]->args[4] = imax; args[i]->args[5] = tua; args[i]->args[6] = lam; args[i]->args[7] = b0;
              args[i]->args[8] = use_work_equiv;
              pthread_create(&threads[i], NULL, worker_solve_G_StEFCal_stochastic_adaptive, args[i]);
          }
          for (i = 0; i < *calibrator->num_workers; i++) pthread_join(threads[i], NULL);
          for (i = 0; i < *calibrator->num_workers; i++) free_ta(args[i]);
      }
      if (PROGRESSBAR) {
          char message[100];
          snprintf(message, sizeof(message), "Stochastic-adaptive StEFCal: Antenna Size = %ld\n", *calibrator->N);
          stop_free_pb(progress_bar, message);
      }
  }

  /**
   * Worker used to parallelize function solve_G_StEFCal_stochastic_adaptive
  */
  void* worker_solve_G_StEFCal_stochastic_adaptive(void* args) {
      struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
      void** arguments = thread_args->args;
      struct Calibrator* calibrator = arguments[0];
      size_t* worker_queue = arguments[1];
      pthread_mutex_t* mutex_work_queue = arguments[2];
      struct ProgressBar* progress_bar = arguments[3];
      size_t* imax = arguments[4];
      double* tua = arguments[5];
      double* lam = arguments[6];
      size_t* b0 = arguments[7];
      int* use_work_equiv = arguments[8];
      size_t N = *calibrator->N;
      double complex* Mc = malloc(N*N*sizeof(double complex));
      double complex* Dc = malloc(N*N*sizeof(double complex));
      double complex* g  = malloc(N*sizeof(double complex));
      while (1) {
          pthread_mutex_lock(mutex_work_queue);
          size_t t = *worker_queue; (*worker_queue)++;
          pthread_mutex_unlock(mutex_work_queue);
          if (t >= *calibrator->n_steps) break;
          if (PROGRESSBAR) { lock_pb(progress_bar); update_pb(progress_bar, &(size_t){t+1}); unlock_pb(progress_bar); }
          struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
          struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});
          fill_diagonal_a(M_t, &(double complex){0 + 0*I});
          fill_diagonal_a(D_t, &(double complex){0 + 0*I});
          nos_fixed_copy_mat(M_t, Mc, N);
          nos_fixed_copy_mat(D_t, Dc, N);
          for (size_t i = 0; i < N; i++) g[i] = 1.0;
          size_t iters = 0;
          jacobi_stoch_adaptive_timestep(N, Mc, Dc, *imax, *tua, *lam, *lam, *b0, t+1, g, &iters, NULL, NULL, NULL, *use_work_equiv, NULL);
          nos_fixed_store(calibrator, M_t, D_t, g, N, t, iters, 1);
          free_a(M_t); free_a(D_t);
      }
      free(Mc); free(Dc); free(g);
      pthread_exit(NULL);
  }

  /**
   * Stochastic-adaptive StEFCal with separate l1/l2 relaxation (odd/even, as in
   * solve_G_StEFCal_Lambda) on top of the Jacobi stochastic adaptive-batch solver. Identical to
   * solve_G_StEFCal_stochastic_adaptive when l1==l2. Used for the (l1,l2) relaxation-justification
   * study: l1!=l2 makes the step oscillate, the stall detector misfires, and b balloons -> high
   * work-equivalent cost off the diagonal. INPUT: imax, tau, l1, l2, b0.
  */
  void solve_G_StEFCal_stochastic_adaptive_l1l2(struct Calibrator* calibrator, size_t* imax, double* tua,
                                                double* l1, double* l2, size_t* b0, int* use_work_equiv) {
      if (calibrator->G != NULL) free_a(calibrator->G);
      calibrator->G = zeros_a(calibrator->D->num_dimensions, calibrator->D->dimensions, calibrator->D->type);
      if (calibrator->datacollector != NULL) free_dc(calibrator->datacollector);
      calibrator->datacollector = new_dc(calibrator);

      struct ProgressBar* progress_bar = NULL;
      if (PROGRESSBAR) progress_bar = new_pb(&(size_t){50}, &calibrator->G->dimensions[2]);

      if (!*calibrator->parallel) {
          size_t N = *calibrator->N;
          double complex* Mc = malloc(N*N*sizeof(double complex));
          double complex* Dc = malloc(N*N*sizeof(double complex));
          double complex* g  = malloc(N*sizeof(double complex));
          for (size_t t = 0; t < *calibrator->n_steps; t++) {
              if (PROGRESSBAR) update_pb(progress_bar, &(size_t){t+1});
              struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
              struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});
              fill_diagonal_a(M_t, &(double complex){0 + 0*I});
              fill_diagonal_a(D_t, &(double complex){0 + 0*I});
              nos_fixed_copy_mat(M_t, Mc, N);
              nos_fixed_copy_mat(D_t, Dc, N);
              for (size_t i = 0; i < N; i++) g[i] = 1.0;
              size_t iters = 0;

              /* Capture per-iteration gains + damping only for the random timesteps (like Lambda). */
              int index_random_timestep = ALL_DATA ? index_of_element_a(calibrator->random_timesteps, &(int){(int)t}) : -1;
              int check_capture = (ALL_DATA && index_random_timestep >= 0);
              double* cap_damping = NULL; double complex* cap_gains = NULL; size_t cap_count = 0;
              if (check_capture) {
                  cap_damping = malloc(MAX_ITERATIONS_CAPTURE * sizeof(double));
                  cap_gains = malloc(MAX_ITERATIONS_CAPTURE * N * sizeof(double complex));
              }
              /* Raw-iteration (extraction) path only: capture the full per-iteration b sequence
                 (buffer sized *imax so the summed b is exact for the FLOPS scaling). */
              int* cap_b = (!*use_work_equiv) ? malloc(*imax * sizeof(int)) : NULL;
              jacobi_stoch_adaptive_timestep(N, Mc, Dc, *imax, *tua, *l1, *l2, *b0, t+1, g, &iters, cap_damping, cap_gains, &cap_count, *use_work_equiv, cap_b);
              if (check_capture) {
                  add_stoch_capture_dc(calibrator, N, 0, index_random_timestep, cap_damping, cap_gains, cap_count);
                  free(cap_damping); free(cap_gains);
              }
              if (cap_b) {
                  /* iters == raw sweep count == number of captured b values (use_work_equiv==0 here). */
                  struct array* b_row = zeros_a(&(size_t){1}, (size_t[]){iters}, &(enum DataType){INT});
                  for (size_t k = 0; k < iters; k++) set_element_a(b_row, (size_t[]){k}, &(int){cap_b[k]});
                  add_b_values_dc(calibrator->datacollector, &(size_t){t}, b_row);
                  free_a(b_row);
                  free(cap_b);
              }
              nos_fixed_store(calibrator, M_t, D_t, g, N, t, iters, 0);
              free_a(M_t); free_a(D_t);
          }
          free(Mc); free(Dc); free(g);
      } else {
          pthread_t threads[*calibrator->num_workers];
          struct ThreadArgs* args[*calibrator->num_workers];
          size_t work_queue = 0;
          pthread_mutex_t mutex_work_queue = PTHREAD_MUTEX_INITIALIZER;
          size_t num_args = 10; size_t i;
          for (i = 0; i < *calibrator->num_workers; i++) {
              args[i] = init_ta(&i, &num_args);
              args[i]->args[0] = calibrator; args[i]->args[1] = &work_queue;
              args[i]->args[2] = &mutex_work_queue; args[i]->args[3] = progress_bar;
              args[i]->args[4] = imax; args[i]->args[5] = tua; args[i]->args[6] = l1;
              args[i]->args[7] = l2; args[i]->args[8] = b0;
              args[i]->args[9] = use_work_equiv;
              pthread_create(&threads[i], NULL, worker_solve_G_StEFCal_stochastic_adaptive_l1l2, args[i]);
          }
          for (i = 0; i < *calibrator->num_workers; i++) pthread_join(threads[i], NULL);
          for (i = 0; i < *calibrator->num_workers; i++) free_ta(args[i]);
      }
      if (PROGRESSBAR) {
          char message[100];
          snprintf(message, sizeof(message), "Stochastic-adaptive l1/l2 StEFCal: Antenna Size = %ld\n", *calibrator->N);
          stop_free_pb(progress_bar, message);
      }
  }

  /**
   * Worker used to parallelize function solve_G_StEFCal_stochastic_adaptive_l1l2
  */
  void* worker_solve_G_StEFCal_stochastic_adaptive_l1l2(void* args) {
      struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
      void** arguments = thread_args->args;
      struct Calibrator* calibrator = arguments[0];
      size_t* worker_queue = arguments[1];
      pthread_mutex_t* mutex_work_queue = arguments[2];
      struct ProgressBar* progress_bar = arguments[3];
      size_t* imax = arguments[4];
      double* tua = arguments[5];
      double* l1 = arguments[6];
      double* l2 = arguments[7];
      size_t* b0 = arguments[8];
      int* use_work_equiv = arguments[9];
      size_t N = *calibrator->N;
      double complex* Mc = malloc(N*N*sizeof(double complex));
      double complex* Dc = malloc(N*N*sizeof(double complex));
      double complex* g  = malloc(N*sizeof(double complex));
      while (1) {
          pthread_mutex_lock(mutex_work_queue);
          size_t t = *worker_queue; (*worker_queue)++;
          pthread_mutex_unlock(mutex_work_queue);
          if (t >= *calibrator->n_steps) break;
          if (PROGRESSBAR) { lock_pb(progress_bar); update_pb(progress_bar, &(size_t){t+1}); unlock_pb(progress_bar); }
          struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
          struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});
          fill_diagonal_a(M_t, &(double complex){0 + 0*I});
          fill_diagonal_a(D_t, &(double complex){0 + 0*I});
          nos_fixed_copy_mat(M_t, Mc, N);
          nos_fixed_copy_mat(D_t, Dc, N);
          for (size_t i = 0; i < N; i++) g[i] = 1.0;
          size_t iters = 0;

          /* Capture per-iteration gains + damping only for the random timesteps (like Lambda). */
          int index_random_timestep = ALL_DATA ? index_of_element_a(calibrator->random_timesteps, &(int){(int)t}) : -1;
          int check_capture = (ALL_DATA && index_random_timestep >= 0);
          double* cap_damping = NULL; double complex* cap_gains = NULL; size_t cap_count = 0;
          if (check_capture) {
              cap_damping = malloc(MAX_ITERATIONS_CAPTURE * sizeof(double));
              cap_gains = malloc(MAX_ITERATIONS_CAPTURE * N * sizeof(double complex));
          }
          int* cap_b = (!*use_work_equiv) ? malloc(*imax * sizeof(int)) : NULL;
          jacobi_stoch_adaptive_timestep(N, Mc, Dc, *imax, *tua, *l1, *l2, *b0, t+1, g, &iters, cap_damping, cap_gains, &cap_count, *use_work_equiv, cap_b);
          if (check_capture) {
              add_stoch_capture_dc(calibrator, N, 1, index_random_timestep, cap_damping, cap_gains, cap_count);
              free(cap_damping); free(cap_gains);
          }
          if (cap_b) {
              /* iters == raw sweep count == number of captured b values (use_work_equiv==0 here). */
              struct array* b_row = zeros_a(&(size_t){1}, (size_t[]){iters}, &(enum DataType){INT});
              for (size_t k = 0; k < iters; k++) set_element_a(b_row, (size_t[]){k}, &(int){cap_b[k]});
              lock_dc(calibrator->datacollector);
              add_b_values_dc(calibrator->datacollector, &(size_t){t}, b_row);
              unlock_dc(calibrator->datacollector);
              free_a(b_row);
              free(cap_b);
          }
          nos_fixed_store(calibrator, M_t, D_t, g, N, t, iters, 1);
          free_a(M_t); free_a(D_t);
      }
      free(Mc); free(Dc); free(g);
      pthread_exit(NULL);
  }

  /**
   * This function finds argmin G ||R-GMG^H|| using StEFCal.
   * D is your observed visibilities matrx. This is saved when the object is made
   * M is your predicted visibilities. This is saved when the object is made
   * RETURNS:
   *  - NONE
   * INPUT:
   *  - imax maximum amount of iterations.
   *  - tau stopping criteria.
   *  - l1
   *  - l2
  */
  void solve_G_StEFCal_Lambda(struct Calibrator* calibrator, size_t* imax, double* tua, double* l1, double* l2) {
  
      //G
      if (calibrator->G != NULL) {
          free_a(calibrator->G);
      }
  
      double phi = 0;
  
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
                      phi = 1 / (1 + *l2);
                  } else {
                      phi = 1 / (1 + *l1);
                  }
  
                  struct array* scaled_g = multiply_constant_a(g, &(double){phi}, &(enum DataType){DOUBLE});
                  struct array* scaled_g_old = multiply_constant_a(g_old, &(double){1 - phi}, &(enum DataType){DOUBLE});
                  struct array* new_g = addition_a(scaled_g, scaled_g_old);
  
                  free_a(scaled_g);
                  free_a(scaled_g_old);
                  free_a(g);
                  g = new_g;
  
                  if (ALL_DATA) {
                      if (check_random_timestep && i < MAX_ITERATIONS_CAPTURE) {
                          //saving damping_g
                          set_element_a(damping_g, (size_t[]){i}, &(double){phi});
                          //saving damping_g_old
                          set_element_a(damping_g_old, (size_t[]){i}, &(double){1 - phi});
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
              for (size_t j = 0; j < *calibrator->N; j++) {
                  set_element_a(g_full, (size_t[]){j}, (double complex*)get_element_a(g, (size_t[]){j}));
                  set_element_a(g_full, (size_t[]){*calibrator->N + j}, (double complex*)get_element_a(g_conj, (size_t[]){j}));
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
              args[i]->args[6] = l1;
              args[i]->args[7] = l2;
  
              pthread_create(&threads[i], NULL, worker_solve_G_StEFCal_Lambda, args[i]);
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
          snprintf(message, sizeof(message), "StEFCal Lambda: Antenna Size = %ld\n", *calibrator->N);
          stop_free_pb(progress_bar, message);
      }
  }
  
  /**
   * Worker used to parallelize function solve_G_StEFCal_Lambda
  */
  /* One timestep of StEFCal with alternating damping, on flat row-major buffers.
   *
   * Numerically identical to the kernel inside worker_solve_G_StEFCal_Lambda: for each antenna p,
   *     g_p = sum_k conj(D[k][p]) * (g[k] * M[k][p])  /  sum_k |g[k] * M[k][p]|^2,
   * accumulated in the same k order, then damped by phi = 1/(1+l2) on even iterations and
   * 1/(1+l1) on odd ones. The diagonals of M and D are already zeroed by the caller.
   *
   * The difference is purely mechanical: the original allocates seven struct arrays per antenna
   * per iteration (m_p, d_p, z, two conjugates, two dots) via the array abstraction, so an N=100
   * solve costs on the order of 10^4 malloc/free pairs per timestep. Here nothing is allocated
   * inside the loop, which is what makes the wall-clock comparison against SAS a comparison of
   * algorithms rather than of memory-management styles. */
  static void stefcal_lambda_timestep(size_t N, const double complex* Mc, const double complex* Dc,
                                      size_t imax, double tua, double l1, double l2,
                                      double complex* g, size_t* iters_out,
                                      double* cap_damping, double complex* cap_gains, size_t* cap_count) {
      double complex* g_old = malloc(N * sizeof(double complex));
      double complex* g_p   = malloc(N * sizeof(double complex));
      size_t i = 0;
      if (cap_count) *cap_count = 0;

      for (i = 0; i < imax; i++) {
          for (size_t p = 0; p < N; p++) {
              double complex num = 0.0 + 0.0*I;
              double complex den = 0.0 + 0.0*I;
              for (size_t k = 0; k < N; k++) {
                  double complex z = g[k] * Mc[k*N + p];
                  num += conj(Dc[k*N + p]) * z;
                  den += conj(z) * z;
              }
              g_p[p] = num / den;
          }

          /* g_old <- previous g ; g <- damped combination, exactly as the original orders it. */
          for (size_t p = 0; p < N; p++) g_old[p] = g[p];
          double phi = (i % 2 == 0) ? 1.0 / (1.0 + l2) : 1.0 / (1.0 + l1);
          for (size_t p = 0; p < N; p++) g[p] = phi * g_p[p] + (1.0 - phi) * g_old[p];

          if (cap_gains && *cap_count < MAX_ITERATIONS_CAPTURE) {
              cap_damping[*cap_count] = phi;
              for (size_t p = 0; p < N; p++) cap_gains[(*cap_count)*N + p] = g[p];
              (*cap_count)++;
          }

          double n1 = 0.0, n2 = 0.0;
          for (size_t p = 0; p < N; p++) {
              double complex d = g[p] - g_old[p];
              n1 += creal(conj(d)*d);
              n2 += creal(conj(g[p])*g[p]);
          }
          /* The original records the loop index at break (add_data_dc(..., &i)), not i+1. */
          if (sqrt(n1) / sqrt(n2) <= tua) break;
      }

      *iters_out = i;
      free(g_old); free(g_p);
  }

  void* worker_solve_G_StEFCal_Lambda_v2(void* args) {
      struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
      void** arguments = thread_args->args;
      struct Calibrator* calibrator = arguments[0];
      size_t* worker_queue = arguments[1];
      pthread_mutex_t* mutex_work_queue = arguments[2];
      struct ProgressBar* progress_bar = arguments[3];
      size_t* imax = arguments[4];
      double* tua = arguments[5];
      double* l1 = arguments[6];
      double* l2 = arguments[7];
      size_t N = *calibrator->N;
      double complex* Mc = malloc(N*N*sizeof(double complex));
      double complex* Dc = malloc(N*N*sizeof(double complex));
      double complex* g  = malloc(N*sizeof(double complex));
      while (1) {
          pthread_mutex_lock(mutex_work_queue);
          size_t t = *worker_queue; (*worker_queue)++;
          pthread_mutex_unlock(mutex_work_queue);
          if (t >= *calibrator->n_steps) break;
          if (PROGRESSBAR) { lock_pb(progress_bar); update_pb(progress_bar, &(size_t){t+1}); unlock_pb(progress_bar); }

          struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
          struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});
          fill_diagonal_a(M_t, &(double complex){0 + 0*I});
          fill_diagonal_a(D_t, &(double complex){0 + 0*I});
          nos_fixed_copy_mat(M_t, Mc, N);
          nos_fixed_copy_mat(D_t, Dc, N);
          for (size_t i = 0; i < N; i++) g[i] = 1.0;
          size_t iters = 0;

          int index_random_timestep = ALL_DATA ? index_of_element_a(calibrator->random_timesteps, &(int){(int)t}) : -1;
          int check_capture = (ALL_DATA && index_random_timestep >= 0);
          double* cap_damping = NULL; double complex* cap_gains = NULL; size_t cap_count = 0;
          if (check_capture) {
              cap_damping = malloc(MAX_ITERATIONS_CAPTURE * sizeof(double));
              cap_gains = malloc(MAX_ITERATIONS_CAPTURE * N * sizeof(double complex));
          }

          stefcal_lambda_timestep(N, Mc, Dc, *imax, *tua, *l1, *l2, g, &iters,
                                  cap_damping, cap_gains, &cap_count);

          if (check_capture) {
              add_stoch_capture_dc(calibrator, N, 1, index_random_timestep, cap_damping, cap_gains, cap_count);
              free(cap_damping); free(cap_gains);
          }
          nos_fixed_store(calibrator, M_t, D_t, g, N, t, iters, 1);
          free_a(M_t); free_a(D_t);
      }
      free(Mc); free(Dc); free(g);
      pthread_exit(NULL);
  }

  /**
   * StEFCal with alternating damping (l1, l2) -- flat-buffer rewrite of solve_G_StEFCal_Lambda.
   *
   * Same algorithm, same numbers; only the inner loop's memory handling differs. Exists so that a
   * wall-clock comparison against SAS measures the O(Nb) subsampling saving rather than the cost of
   * the struct-array abstraction. Verify with compare_stefcal_v2(): iteration counts and residuals
   * should match solve_G_StEFCal_Lambda exactly.
   */
  void solve_G_StEFCal_Lambda_v2(struct Calibrator* calibrator, size_t* imax, double* tua,
                                 double* l1, double* l2) {
      if (calibrator->G != NULL) free_a(calibrator->G);
      calibrator->G = zeros_a(calibrator->D->num_dimensions, calibrator->D->dimensions, calibrator->D->type);
      if (calibrator->datacollector != NULL) free_dc(calibrator->datacollector);
      calibrator->datacollector = new_dc(calibrator);

      struct ProgressBar* progress_bar = NULL;
      if (PROGRESSBAR) progress_bar = new_pb(&(size_t){50}, &calibrator->G->dimensions[2]);

      if (!*calibrator->parallel) {
          size_t N = *calibrator->N;
          double complex* Mc = malloc(N*N*sizeof(double complex));
          double complex* Dc = malloc(N*N*sizeof(double complex));
          double complex* g  = malloc(N*sizeof(double complex));
          for (size_t t = 0; t < *calibrator->n_steps; t++) {
              if (PROGRESSBAR) update_pb(progress_bar, &(size_t){t+1});
              struct array* M_t = slice_a(calibrator->M, (int[]){-1,-1,t});
              struct array* D_t = slice_a(calibrator->D, (int[]){-1,-1,t});
              fill_diagonal_a(M_t, &(double complex){0 + 0*I});
              fill_diagonal_a(D_t, &(double complex){0 + 0*I});
              nos_fixed_copy_mat(M_t, Mc, N);
              nos_fixed_copy_mat(D_t, Dc, N);
              for (size_t i = 0; i < N; i++) g[i] = 1.0;
              size_t iters = 0;

              int index_random_timestep = ALL_DATA ? index_of_element_a(calibrator->random_timesteps, &(int){(int)t}) : -1;
              int check_capture = (ALL_DATA && index_random_timestep >= 0);
              double* cap_damping = NULL; double complex* cap_gains = NULL; size_t cap_count = 0;
              if (check_capture) {
                  cap_damping = malloc(MAX_ITERATIONS_CAPTURE * sizeof(double));
                  cap_gains = malloc(MAX_ITERATIONS_CAPTURE * N * sizeof(double complex));
              }

              stefcal_lambda_timestep(N, Mc, Dc, *imax, *tua, *l1, *l2, g, &iters,
                                      cap_damping, cap_gains, &cap_count);

              if (check_capture) {
                  add_stoch_capture_dc(calibrator, N, 0, index_random_timestep, cap_damping, cap_gains, cap_count);
                  free(cap_damping); free(cap_gains);
              }
              nos_fixed_store(calibrator, M_t, D_t, g, N, t, iters, 0);
              free_a(M_t); free_a(D_t);
          }
          free(Mc); free(Dc); free(g);
      } else {
          pthread_t threads[*calibrator->num_workers];
          struct ThreadArgs* args[*calibrator->num_workers];
          size_t work_queue = 0;
          pthread_mutex_t mutex_work_queue = PTHREAD_MUTEX_INITIALIZER;
          size_t num_args = 8;
          size_t i;
          for (i = 0; i < *calibrator->num_workers; i++) {
              args[i] = init_ta(&i, &num_args);
              args[i]->args[0] = calibrator;
              args[i]->args[1] = &work_queue;
              args[i]->args[2] = &mutex_work_queue;
              args[i]->args[3] = progress_bar;
              args[i]->args[4] = imax;
              args[i]->args[5] = tua;
              args[i]->args[6] = l1;
              args[i]->args[7] = l2;
              pthread_create(&threads[i], NULL, worker_solve_G_StEFCal_Lambda_v2, args[i]);
          }
          for (i = 0; i < *calibrator->num_workers; i++) pthread_join(threads[i], NULL);
          for (i = 0; i < *calibrator->num_workers; i++) free_ta(args[i]);
      }

      if (PROGRESSBAR) {
          char message[100];
          snprintf(message, sizeof(message), "StEFCal Lambda v2: Antenna Size = %ld\n", *calibrator->N);
          stop_free_pb(progress_bar, message);
      }
  }

  void* worker_solve_G_StEFCal_Lambda(void* args) {
      struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
      void** arguments = thread_args->args;
      struct Calibrator* calibrator = arguments[0];
      size_t* worker_queue = arguments[1];
      pthread_mutex_t* mutex_work_queue = arguments[2];
      struct ProgressBar* progress_bar = arguments[3];
      size_t* imax = arguments[4];
      double* tua = arguments[5];
      double* l1 = arguments[6];
      double* l2 = arguments[7];
      double phi = 0;
  
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
              
              //checking all_gains, damping_g and damping_g_old should be saved.
              int t_int = t;
              bool check_random_timestep = false;
              int index_random_timestep = 0;
  
              if (ALL_DATA) {
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
                      phi = 1 / (1 + *l2);
                  } else {
                      phi = 1 / (1 + *l1);
                  }
  
                  struct array* scaled_g = multiply_constant_a(g, &(double){phi}, &(enum DataType){DOUBLE});
                  struct array* scaled_g_old = multiply_constant_a(g_old, &(double){1 - phi}, &(enum DataType){DOUBLE});
                  struct array* new_g = addition_a(scaled_g, scaled_g_old);
  
                  free_a(scaled_g);
                  free_a(scaled_g_old);
                  free_a(g);
                  g = new_g;
  
                  if (ALL_DATA) {
                      if (check_random_timestep && i < MAX_ITERATIONS_CAPTURE) {
                          //saving damping_g
                          set_element_a(damping_g, (size_t[]){i}, &(double){phi});
                          //saving damping_g_old
                          set_element_a(damping_g_old, (size_t[]){i}, &(double){1 - phi});
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
              for (size_t j = 0; j < *calibrator->N; j++) {
                  set_element_a(g_full, (size_t[]){j}, (double complex*)get_element_a(g, (size_t[]){j}));
                  set_element_a(g_full, (size_t[]){*calibrator->N + j}, (double complex*)get_element_a(g_conj, (size_t[]){j}));
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
   * This function finds argmin G ||R-GMG^H|| using StEFCal Improved.
   * D is your observed visibilities matrx. This is saved when the object is made
   * M is your predicted visibilities. This is saved when the object is made
   * RETURNS:
   *  - NONE
   * INPUT:
   *  - imax maximum amount of iterations.
   *  - tau stopping criteria.
  */
  void solve_G_StEFCal_improved(struct Calibrator* calibrator, size_t* imax, double* tua, double* l1, double* l2) {
  
      double l = 0;
      // double phi = 0;
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
  
                  struct array* g_conjugate = conjugate_a(g);
                  struct array* alpha = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});
                  struct array* a = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});
                  struct array** z = malloc(*calibrator->N * sizeof(struct array*));
                  for (size_t p = 0; p < *calibrator->N; p++) {
                      struct array* m_p = slice_a(M_t, (int[]){p,-1});
                      struct array* d_p = slice_a(D_t, (int[]){p,-1});
  
                      //computing z_p
                      z[p] = multiply_a(m_p, g_conjugate);
                      //computing alpha_p
                      struct array* z_p_conjugate = conjugate_a(z[p]);
                      struct array* alpha_p_array = dot_a(z_p_conjugate, z[p]);
                      struct array* alpha_p_array_inverse = hadamard_inverse_a(alpha_p_array);
                      //setting alpha_p
                      set_element_a(alpha, (size_t[]){p}, (double complex*)get_element_a(alpha_p_array_inverse, (size_t[]){0}));
                      //calculating a_p
                      struct array* a_p_array = dot_a(d_p, z_p_conjugate);
                      //setting a_p
                      set_element_a(a, (size_t[]){p}, (double complex*)get_element_a(a_p_array, (size_t[]){0}));
  
                      free_a(a_p_array);
                      free_a(alpha_p_array_inverse);
                      free_a(alpha_p_array);
                      free_a(z_p_conjugate);
                      free_a(m_p);
                      free_a(d_p);
                  }
  
                  if (i % 2 == 0) {
                      l = *l2;
                  } else {
                      l = *l1;
                  }
                  
  
                  struct array* g_new = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});
                  struct array* a_conj = conjugate_a(a);
                  for (size_t p = 0; p < *calibrator->N; p++) {
                      double complex g_p_i_1 = *(double complex*)get_element_a(g, (size_t[]){p});
                      double complex a_p = *(double complex*)get_element_a(a, (size_t[]){p});
                      double complex alpha_p = *(double complex*)get_element_a(alpha, (size_t[]){p});
  
                      struct array* m_p = slice_a(M_t, (int[]){p,-1});
                      struct array* d_p = slice_a(D_t, (int[]){p,-1});
                      struct array* z_p_conjugate = conjugate_a(z[p]);
                      //calculation of x_p
                      struct array* z_conj_mul_m_p = multiply_a(z_p_conjugate, m_p);
                      struct array* t_p = multiply_a(z_conj_mul_m_p, alpha);
  
                      //calculation of g_p
                      struct array* a_conj_dot_t_p_array = dot_a(a_conj, t_p);
                      double complex a_conj_dot_t_p = *(double complex*)get_element_a(a_conj_dot_t_p_array, (size_t[]){0});
  
                      double complex g_p = ((1/(l + 1)) * a_p * alpha_p) + (-1*g_p_i_1*alpha_p)*(1/(l*l + 2*l + 1)) * a_conj_dot_t_p + (l*l + l + 1)/(l*l + 2*l + 1) * g_p_i_1;
                      set_element_a(g_new, (size_t[]){p}, &(double complex){g_p});
  
                      free_a(a_conj_dot_t_p_array);
                      free_a(t_p);
                      free_a(z_conj_mul_m_p);
                      free_a(z_p_conjugate);
                      free_a(m_p);
                      free_a(d_p);
                  }
                  free_a(a_conj);
                  free_a(g_conjugate);
                  //freeing z
                  for (size_t p = 0; p < *calibrator->N; p++) {
                      if (z[p] != NULL) {
                          free_a(z[p]);
                      }
                  }
                  free(z);
                  free_a(a);
                  free_a(alpha);
                  free_a(g_old);
                  g_old = g;
                  g = g_new;
  
                  if (ALL_DATA) {
                      double phi = 1;
                      if (check_random_timestep && i < MAX_ITERATIONS_CAPTURE) {
                          //saving damping_g
                          set_element_a(damping_g, (size_t[]){i}, &(double){phi});
                          //saving damping_g_old
                          set_element_a(damping_g_old, (size_t[]){i}, &(double){1 - phi});
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
              for (size_t j = 0; j < *calibrator->N; j++) {
                  set_element_a(g_full, (size_t[]){j}, (double complex*)get_element_a(g, (size_t[]){j}));
                  set_element_a(g_full, (size_t[]){*calibrator->N + j}, (double complex*)get_element_a(g_conj, (size_t[]){j}));
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
          // parallel
          // The parallelization is done with workers, where the workers work on first come first serve basis
          // The time steps are parallelized.
  
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
              args[i]->args[6] = l1;
              args[i]->args[7] = l2;
  
              pthread_create(&threads[i], NULL, worker_solve_G_StEFCal_improved, args[i]);
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
          snprintf(message, sizeof(message), "StEFCal Improved: Antenna Size = %ld\n", *calibrator->N);
          stop_free_pb(progress_bar, message);
      }
  }
  
  /**
   * Worker used to parallelize function solve_G_StEFCal_improved
  */
  void* worker_solve_G_StEFCal_improved(void* args) {
      struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
      void** arguments = thread_args->args;
      struct Calibrator* calibrator = arguments[0];
      size_t* worker_queue = arguments[1];
      pthread_mutex_t* mutex_work_queue = arguments[2];
      struct ProgressBar* progress_bar = arguments[3];
      size_t* imax = arguments[4];
      double* tua = arguments[5];
      double* l1 = arguments[6];
      double* l2 = arguments[7];
      double l = 0;
  
  
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
  
                  struct array* g_conjugate = conjugate_a(g);
                  struct array* alpha = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});
                  struct array* a = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});
                  struct array** z = malloc(*calibrator->N * sizeof(struct array*));
                  for (size_t p = 0; p < *calibrator->N; p++) {
                      struct array* m_p = slice_a(M_t, (int[]){p,-1});
                      struct array* d_p = slice_a(D_t, (int[]){p,-1});
  
                      //computing z_p
                      z[p] = multiply_a(m_p, g_conjugate);
                      //computing alpha_p
                      struct array* z_p_conjugate = conjugate_a(z[p]);
                      struct array* alpha_p_array = dot_a(z_p_conjugate, z[p]);
                      struct array* alpha_p_array_inverse = hadamard_inverse_a(alpha_p_array);
                      //setting alpha_p
                      set_element_a(alpha, (size_t[]){p}, (double complex*)get_element_a(alpha_p_array_inverse, (size_t[]){0}));
                      //calculating a_p
                      struct array* a_p_array = dot_a(d_p, z_p_conjugate);
                      //setting a_p
                      set_element_a(a, (size_t[]){p}, (double complex*)get_element_a(a_p_array, (size_t[]){0}));
  
                      free_a(a_p_array);
                      free_a(alpha_p_array_inverse);
                      free_a(alpha_p_array);
                      free_a(z_p_conjugate);
                      free_a(m_p);
                      free_a(d_p);
                  }
  
                  if (i % 2 == 0) {
                      l = *l2;
                  } else {
                      l = *l1;
                  }
                  
  
                  struct array* g_new = zeros_a(&(size_t){1}, (size_t[]){*calibrator->N}, &(enum DataType){COMPLEX});
                  struct array* a_conj = conjugate_a(a);
                  for (size_t p = 0; p < *calibrator->N; p++) {
                      double complex g_p_i_1 = *(double complex*)get_element_a(g, (size_t[]){p});
                      double complex a_p = *(double complex*)get_element_a(a, (size_t[]){p});
                      double complex alpha_p = *(double complex*)get_element_a(alpha, (size_t[]){p});
  
                      struct array* m_p = slice_a(M_t, (int[]){p,-1});
                      struct array* d_p = slice_a(D_t, (int[]){p,-1});
                      struct array* z_p_conjugate = conjugate_a(z[p]);
                      //calculation of x_p
                      struct array* z_conj_mul_m_p = multiply_a(z_p_conjugate, m_p);
                      struct array* t_p = multiply_a(z_conj_mul_m_p, alpha);
  
                      //calculation of g_p
                      struct array* a_conj_dot_t_p_array = dot_a(a_conj, t_p);
                      double complex a_conj_dot_t_p = *(double complex*)get_element_a(a_conj_dot_t_p_array, (size_t[]){0});
  
                      double complex g_p = ((1/(l + 1)) * a_p * alpha_p) + (-1*g_p_i_1*alpha_p)*(1/(l*l + 2*l + 1)) * a_conj_dot_t_p + (l*l + l + 1)/(l*l + 2*l + 1) * g_p_i_1;
                      set_element_a(g_new, (size_t[]){p}, &(double complex){g_p});
  
                      free_a(a_conj_dot_t_p_array);
                      free_a(t_p);
                      free_a(z_conj_mul_m_p);
                      free_a(z_p_conjugate);
                      free_a(m_p);
                      free_a(d_p);
                  }
                  free_a(a_conj);
                  free_a(g_conjugate);
                  //freeing z
                  for (size_t p = 0; p < *calibrator->N; p++) {
                      if (z[p] != NULL) {
                          free_a(z[p]);
                      }
                  }
                  free(z);
                  free_a(a);
                  free_a(alpha);
                  free_a(g_old);
                  g_old = g;
                  g = g_new;
  
                  if (ALL_DATA) {
                      double phi = 1;
                      if (check_random_timestep && i < MAX_ITERATIONS_CAPTURE) {
                          //saving damping_g
                          set_element_a(damping_g, (size_t[]){i}, &(double){phi});
                          //saving damping_g_old
                          set_element_a(damping_g_old, (size_t[]){i}, &(double){1 - phi});
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
              for (size_t j = 0; j < *calibrator->N; j++) {
                  set_element_a(g_full, (size_t[]){j}, (double complex*)get_element_a(g, (size_t[]){j}));
                  set_element_a(g_full, (size_t[]){*calibrator->N + j}, (double complex*)get_element_a(g_conj, (size_t[]){j}));
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
      fprintf(commandsFile, "set output './pictures/corrected_visibilities_baseline.png'\n");
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
  
  /**
   * Levenberg Marquardt Non Adaptive l1 l2 REDUCED
   * RETURNS:
   *  - NONE
   * INPUT:
   *  - imax maximum amount of iterations.
   *  - tau stopping criteria.
   *  - l1 - 
   *  - l2 -
  */
  void solve_G_LM_Non_Adaptive_l1_l2_REDUCED_TIMESTEPS(struct Calibrator* calibrator, size_t* imax, double* tua, double* l1, double* l2) {
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
          size_t t_reduced_count = 0;
          //Not in parallel
          for (size_t t = 0; t < *calibrator->n_steps; t++) {
              if (contains_element_a(calibrator->random_timesteps_reduced, &(int){t})) {
                  if (PROGRESSBAR) {
                      update_pb(progress_bar, &(size_t){t_reduced_count+1});
                  }
  
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
                  double l = 0;
  
                  for (i = 0; i < *imax; i++) {
                      copy_over_a(g_old, g_new, (int[]){-1}, (int[]){-1});
  
                      if (i % 2 == 0) {
                          l = *l2;
                      } else {
                          l = *l1;
                      }
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
                      free_a(g_new);
                      g_new = addition_a(g_old, update_step);
  
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
              args[i]->args[6] = l1;
              args[i]->args[7] = l2;
  
              pthread_create(&threads[i], NULL, worker_solve_G_LM_Non_Adaptive_l1_l2_REDUCED_TIMESTEPS, args[i]);
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
          snprintf(message, sizeof(message), "Levenberg Marquardt Non Adaptive l1 l2 REDUCED TIMESTEPS: Antenna Size = %ld\n", *calibrator->N);
          stop_free_pb(progress_bar, message);
      }
  
  }
  
  /**
   * Worker used to parallelize function solve_G_LM_Diag_Hessian_Lambda REDUCED
  */
  void* worker_solve_G_LM_Non_Adaptive_l1_l2_REDUCED_TIMESTEPS(void* args) {
      struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
      void** arguments = thread_args->args;
      struct Calibrator* calibrator = arguments[0];
      size_t* worker_queue = arguments[1];
      pthread_mutex_t* mutex_work_queue = arguments[2];
      struct ProgressBar* progress_bar = arguments[3];
      size_t* imax = arguments[4];
      double* tua = arguments[5];
      double* l1 = arguments[6];
      double* l2 = arguments[7];
  
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
                  //checking all_gains, damping_g and damping_g_old should be saved.
                  damping_g = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
                  damping_g_old = zeros_a(&(size_t){1}, (size_t[]){MAX_ITERATIONS_CAPTURE}, &(enum DataType){DOUBLE});
              }
  
              size_t i = 0;
              double l = 0;
  
              for (i = 0; i < *imax; i++) {
                  copy_over_a(g_old, g_new, (int[]){-1}, (int[]){-1});
  
                  if (i % 2 == 0) {
                      l = *l2;
                  } else {
                      l = *l1;
                  }
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
                  free_a(g_new);
                  g_new = addition_a(g_old, update_step);
  
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
  