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
   *  - Array struct pointer
   * INPUTS:
   *  - Calibrator struct pointer
   *  - M_t - time step t of M
   *  - g - antenna gain array
  */
  struct array* compute_J(struct Calibrator* calibrator, struct array* M_t, struct array* g);
  
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
  struct array* compute_r(struct Calibrator* calibrator, struct array* M_t, struct array* D_t, struct array* g);
  
  /**
   * Computing Residual for complex domain for calibration problem
   * RETURNS:
   *  - Array struct pointer
   * INPUTS:
   *  - M_t - time step t of M
   *  - D_t - time step t of D
   *  - g - antenna gain array
  */
  struct array* compute_r_N(struct array* M_t, struct array* D_t, struct array* g);
  
  /**
   * Approximated Hessian
   * RETURNS:
   *  - NONE
   * INPUTS:
   *  - Calibtrator struct pointer
   *  - Hessian Array struct pointer
  */
  struct array* approx_hessian(struct Calibrator* calibrator, struct array* hessian);
  
  /**
   * Correcting visibilities
   * RETURNS:
   *  - NONE
   * INPUTS:
   *  - Calibtrator struct pointer
  */
  void correct_visibilities(struct Calibrator* calibrator);
  
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
  void solve_G_ADAM(struct Calibrator* calibrator, size_t* imax, double* tua, double* alpha, double* B1, double* B2);
  
  /**
   * Worker used to parallelize function solve_G_LM
  */
  void* worker_solve_G_ADAM(void* args);
  
  /**
   * ADAM (gradient computed directly from m, d, g — no explicit residual).
   * Same hyperparameters as solve_G_ADAM.
  */
  void solve_G_ADAM_without_r(struct Calibrator* calibrator, size_t* imax, double* tua, double* alpha, double* B1, double* B2);
  
  /**
   * Worker used to parallelize function solve_G_ADAM_without_r
  */
  void* worker_solve_G_ADAM_without_r(void* args);
  
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
   * Levenberg Marquardt Diagonal Hessian
   * RETURNS:
   *  - NONE
   * INPUT:
   *  - imax maximum amount of iterations.
   *  - tau stopping criteria.
   *  - l1 - 
   *  - l2 -
  */
  void solve_G_LM_Diag_Hessian_Lambda(struct Calibrator* calibrator, size_t* imax, double* tua, double* l1, double* l2);
  
  /**
   * Worker used to parallelize function solve_G_LM_Diag_Hessian_Lambda
  */
  void* worker_solve_G_LM_Diag_Hessian_Lambda(void* args);
  
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
  void solve_G_LM_Non_Adaptive_l1_l2(struct Calibrator* calibrator, size_t* imax, double* tua, double* l1, double* l2);
  
  /**
   * Worker used to parallelize function solve_G_LM_Diag_Hessian_Lambda
  */
  void* worker_solve_G_LM_Non_Adaptive_l1_l2(void* args);
  
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
   * Stochastic (mini-batch) StEFCal: same update as solve_G_StEFCal but each
   * per-antenna step sums over a random subset of b partner antennas instead of
   * all N-1, giving O(N*b) work per iteration instead of O(N^2).
   * RETURNS:
   *  - NONE
   * INPUT:
   *  - imax maximum amount of iterations.
   *  - tau stopping criteria.
   *  - b number of partner antennas sampled per antenna per iteration (batch size).
  */
  void solve_G_StEFCal_stochastic(struct Calibrator* calibrator, size_t* imax, double* tua, size_t* b);

  /**
   * Worker used to parallelize function solve_G_StEFCal_stochastic
  */
  void* worker_solve_G_StEFCal_stochastic(void* args);

  /**
   * NOS-fixed: NOn-diagonal StEFCal with rank-1 gauge regularisation so it
   * converges at lambda=0 (and the lambda_odd=0 schedule) where plain NOS
   * diverges. Same O(N^2)/iteration cost class as NOS/StEFCal.
   * INPUT: imax, tau, l1/l2 (alternating damping), eps (gauge reg, ~30).
  */
  void solve_G_NOS_fixed(struct Calibrator* calibrator, size_t* imax, double* tua,
                         double* l1, double* l2, double* eps);

  /**
   * Worker used to parallelize function solve_G_NOS_fixed
  */
  void* worker_solve_G_NOS_fixed(void* args);

  /**
   * Stochastic-adaptive StEFCal: Jacobi stochastic StEFCal with constant-lambda damping and
   * an adaptive per-antenna batch. The whole sweep updates from the OLD gains (antenna-parallel,
   * GPU-friendly); the sub-sample size starts at b0 and doubles when the relative step stalls.
   * O(N*b)/iteration. The two tunable hyperparameters are lam and b0.
   * INPUT: imax, tau, lam (constant damping, ~0.5), b0 (initial partners/antenna, ~2).
  */
  void solve_G_StEFCal_stochastic_adaptive(struct Calibrator* calibrator, size_t* imax, double* tua,
                                           double* lam, size_t* b0, int* use_work_equiv);

  /**
   * Worker used to parallelize function solve_G_StEFCal_stochastic_adaptive
  */
  void* worker_solve_G_StEFCal_stochastic_adaptive(void* args);

  /**
   * Stochastic-adaptive StEFCal with separate l1/l2 (odd/even) relaxation, on top of the Jacobi
   * stochastic adaptive-batch solver. Identical to solve_G_StEFCal_stochastic_adaptive when l1==l2.
   * Used for the (l1,l2) relaxation-justification study: off the l1=l2 diagonal the step oscillates,
   * the stall detector misfires, and b balloons -> high work-equivalent cost.
   * INPUT: imax, tau, l1, l2, b0 (initial partners/antenna).
  */
  void solve_G_StEFCal_stochastic_adaptive_l1l2(struct Calibrator* calibrator, size_t* imax, double* tua,
                                                double* l1, double* l2, size_t* b0, int* use_work_equiv);

  /**
   * Worker used to parallelize function solve_G_StEFCal_stochastic_adaptive_l1l2
  */
  void* worker_solve_G_StEFCal_stochastic_adaptive_l1l2(void* args);

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
  void solve_G_StEFCal_Lambda(struct Calibrator* calibrator, size_t* imax, double* tua, double* l1, double* l2);

  /**
   * StEFCal with alternating damping on flat buffers -- same algorithm and same numbers as
   * solve_G_StEFCal_Lambda, without the per-antenna struct-array allocation in the inner loop.
   * Use it when timing against SAS, so the comparison measures the algorithm and not the
   * memory-management style.
   */
  void solve_G_StEFCal_Lambda_v2(struct Calibrator* calibrator, size_t* imax, double* tua, double* l1, double* l2);

  /**
   * Worker used to parallelize function solve_G_StEFCal_Lambda_v2
   */
  void* worker_solve_G_StEFCal_Lambda_v2(void* args);
  
  /**
   * Worker used to parallelize function solve_G_StEFCal_Lambda
  */
  void* worker_solve_G_StEFCal_Lambda(void* args);
  
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
  void solve_G_StEFCal_improved(struct Calibrator* calibrator, size_t* imax, double* tua, double* l1, double* l2);
  
  /**
   * Worker used to parallelize function solve_G_StEFCal_improved
  */
  void* worker_solve_G_StEFCal_improved(void* args);
  
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
  void solve_G_LM_Non_Adaptive_l1_l2_REDUCED_TIMESTEPS(struct Calibrator* calibrator, size_t* imax, double* tua, double* l1, double* l2);
  
  /**
   * Worker used to parallelize function solve_G_LM_Diag_Hessian_Lambda REDUCED
  */
  void* worker_solve_G_LM_Non_Adaptive_l1_l2_REDUCED_TIMESTEPS(void* args);
  #endif