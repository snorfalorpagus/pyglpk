/***********************************************************************
*  This code is part of GLPK (GNU Linear Programming Kit).
*
*  Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008,
*  2009, 2010, 2011, 2013 Andrew Makhorin, Department for Applied
*  Informatics, Moscow Aviation Institute, Moscow, Russia. All rights
*  reserved. E-mail: <mao@gnu.org>.
*
*  GLPK is free software: you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  GLPK is distributed in the hope that it will be useful, but WITHOUT
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
*  License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with GLPK. If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#ifndef _LPX_H
#define _LPX_H

#include <glpk.h>

/*#define LPX glp_prob*/
typedef glp_prob LPX;

#define xerror glp_error_(__FILE__, __LINE__)
#define xassert(expr) \
      ((void)((expr) || (glp_assert_(#expr, __FILE__, __LINE__), 1)))
#define xmalloc(size) glp_alloc(1, size)

/* problem class: */
#define LPX_LP          100   /* linear programming (LP) */
#define LPX_MIP         101   /* mixed integer programming (MIP) */

/* status of auxiliary/structural variable: */
#define LPX_BS          140   /* basic variable */
#define LPX_NL          141   /* non-basic variable on lower bound */
#define LPX_NU          142   /* non-basic variable on upper bound */
#define LPX_NF          143   /* non-basic free variable */
#define LPX_NS          144   /* non-basic fixed variable */

/* exit codes returned by solver routines: */
#define LPX_E_OK        200   /* success */
#define LPX_E_EMPTY     201   /* empty problem */
#define LPX_E_BADB      202   /* invalid initial basis */
#define LPX_E_INFEAS    203   /* infeasible initial solution */
#define LPX_E_FAULT     204   /* unable to start the search */
#define LPX_E_OBJLL     205   /* objective lower limit reached */
#define LPX_E_OBJUL     206   /* objective upper limit reached */
#define LPX_E_ITLIM     207   /* iterations limit exhausted */
#define LPX_E_TMLIM     208   /* time limit exhausted */
#define LPX_E_NOFEAS    209   /* no feasible solution */
#define LPX_E_INSTAB    210   /* numerical instability */
#define LPX_E_SING      211   /* problems with basis matrix */
#define LPX_E_NOCONV    212   /* no convergence (interior) */
#define LPX_E_NOPFS     213   /* no primal feas. sol. (LP presolver) */
#define LPX_E_NODFS     214   /* no dual feas. sol. (LP presolver) */
#define LPX_E_MIPGAP    215   /* relative mip gap tolerance reached */

/* control parameter identifiers: */
#define LPX_K_MSGLEV    300   /* lp->msg_lev */
#define LPX_K_SCALE     301   /* lp->scale */
#define LPX_K_DUAL      302   /* lp->dual */
#define LPX_K_PRICE     303   /* lp->price */
#define LPX_K_RELAX     304   /* lp->relax */
#define LPX_K_TOLBND    305   /* lp->tol_bnd */
#define LPX_K_TOLDJ     306   /* lp->tol_dj */
#define LPX_K_TOLPIV    307   /* lp->tol_piv */
#define LPX_K_ROUND     308   /* lp->round */
#define LPX_K_OBJLL     309   /* lp->obj_ll */
#define LPX_K_OBJUL     310   /* lp->obj_ul */
#define LPX_K_ITLIM     311   /* lp->it_lim */
#define LPX_K_ITCNT     312   /* lp->it_cnt */
#define LPX_K_TMLIM     313   /* lp->tm_lim */
#define LPX_K_OUTFRQ    314   /* lp->out_frq */
#define LPX_K_OUTDLY    315   /* lp->out_dly */
#define LPX_K_BRANCH    316   /* lp->branch */
#define LPX_K_BTRACK    317   /* lp->btrack */
#define LPX_K_TOLINT    318   /* lp->tol_int */
#define LPX_K_TOLOBJ    319   /* lp->tol_obj */
#define LPX_K_MPSINFO   320   /* lp->mps_info */
#define LPX_K_MPSOBJ    321   /* lp->mps_obj */
#define LPX_K_MPSORIG   322   /* lp->mps_orig */
#define LPX_K_MPSWIDE   323   /* lp->mps_wide */
#define LPX_K_MPSFREE   324   /* lp->mps_free */
#define LPX_K_MPSSKIP   325   /* lp->mps_skip */
#define LPX_K_LPTORIG   326   /* lp->lpt_orig */
#define LPX_K_PRESOL    327   /* lp->presol */
#define LPX_K_BINARIZE  328   /* lp->binarize */
#define LPX_K_USECUTS   329   /* lp->use_cuts */
#define LPX_K_BFTYPE    330   /* lp->bfcp->type */
#define LPX_K_MIPGAP    331   /* lp->mip_gap */

#define LPX_C_COVER     0x01  /* mixed cover cuts */
#define LPX_C_CLIQUE    0x02  /* clique cuts */
#define LPX_C_GOMORY    0x04  /* Gomory's mixed integer cuts */
#define LPX_C_MIR       0x08  /* mixed integer rounding cuts */
#define LPX_C_ALL       0xFF  /* all cuts */

/* this shit is so broken */
typedef struct DMP DMP;
typedef struct GLPROW GLPROW;
typedef struct GLPCOL GLPCOL;
typedef struct AVL AVL;
typedef struct BFD BFD;

struct glp_prob
{     /* LP/MIP problem object */
      unsigned magic;
      /* magic value used for debugging */
      DMP *pool;
      /* memory pool to store problem object components */
      glp_tree *tree;
      /* pointer to the search tree; set by the MIP solver when this
         object is used in the tree as a core MIP object */
      void *parms;
      /* reserved for backward compatibility */
      /*--------------------------------------------------------------*/
      /* LP/MIP data */
      char *name;
      /* problem name (1 to 255 chars); NULL means no name is assigned
         to the problem */
      char *obj;
      /* objective function name (1 to 255 chars); NULL means no name
         is assigned to the objective function */
      int dir;
      /* optimization direction flag (objective "sense"):
         GLP_MIN - minimization
         GLP_MAX - maximization */
      double c0;
      /* constant term of the objective function ("shift") */
      int m_max;
      /* length of the array of rows (enlarged automatically) */
      int n_max;
      /* length of the array of columns (enlarged automatically) */
      int m;
      /* number of rows, 0 <= m <= m_max */
      int n;
      /* number of columns, 0 <= n <= n_max */
      int nnz;
      /* number of non-zero constraint coefficients, nnz >= 0 */
      GLPROW **row; /* GLPROW *row[1+m_max]; */
      /* row[i], 1 <= i <= m, is a pointer to i-th row */
      GLPCOL **col; /* GLPCOL *col[1+n_max]; */
      /* col[j], 1 <= j <= n, is a pointer to j-th column */
      AVL *r_tree;
      /* row index to find rows by their names; NULL means this index
         does not exist */
      AVL *c_tree;
      /* column index to find columns by their names; NULL means this
         index does not exist */
      /*--------------------------------------------------------------*/
      /* basis factorization (LP) */
      int valid;
      /* the factorization is valid only if this flag is set */
      int *head; /* int head[1+m_max]; */
      /* basis header (valid only if the factorization is valid);
         head[i] = k is the ordinal number of auxiliary (1 <= k <= m)
         or structural (m+1 <= k <= m+n) variable which corresponds to
         i-th basic variable xB[i], 1 <= i <= m */
      glp_bfcp *bfcp;
      /* basis factorization control parameters; may be NULL */
      BFD *bfd; /* BFD bfd[1:m,1:m]; */
      /* basis factorization driver; may be NULL */
      /*--------------------------------------------------------------*/
      /* basic solution (LP) */
      int pbs_stat;
      /* primal basic solution status:
         GLP_UNDEF  - primal solution is undefined
         GLP_FEAS   - primal solution is feasible
         GLP_INFEAS - primal solution is infeasible
         GLP_NOFEAS - no primal feasible solution exists */
      int dbs_stat;
      /* dual basic solution status:
         GLP_UNDEF  - dual solution is undefined
         GLP_FEAS   - dual solution is feasible
         GLP_INFEAS - dual solution is infeasible
         GLP_NOFEAS - no dual feasible solution exists */
      double obj_val;
      /* objective function value */
      int it_cnt;
      /* simplex method iteration count; increased by one on performing
         one simplex iteration */
      int some;
      /* ordinal number of some auxiliary or structural variable having
         certain property, 0 <= some <= m+n */
      /*--------------------------------------------------------------*/
      /* interior-point solution (LP) */
      int ipt_stat;
      /* interior-point solution status:
         GLP_UNDEF  - interior solution is undefined
         GLP_OPT    - interior solution is optimal
         GLP_INFEAS - interior solution is infeasible
         GLP_NOFEAS - no feasible solution exists */
      double ipt_obj;
      /* objective function value */
      /*--------------------------------------------------------------*/
      /* integer solution (MIP) */
      int mip_stat;
      /* integer solution status:
         GLP_UNDEF  - integer solution is undefined
         GLP_OPT    - integer solution is optimal
         GLP_FEAS   - integer solution is feasible
         GLP_NOFEAS - no integer solution exists */
      double mip_obj;
      /* objective function value */
};

struct LPXCPS
{     /* control parameters and statistics */
      int msg_lev;
      /* level of messages output by the solver:
         0 - no output
         1 - error messages only
         2 - normal output
         3 - full output (includes informational messages) */
      int scale;
      /* scaling option:
         0 - no scaling
         1 - equilibration scaling
         2 - geometric mean scaling
         3 - geometric mean scaling, then equilibration scaling */
      int dual;
      /* dual simplex option:
         0 - use primal simplex
         1 - use dual simplex */
      int price;
      /* pricing option (for both primal and dual simplex):
         0 - textbook pricing
         1 - steepest edge pricing */
      double relax;
      /* relaxation parameter used in the ratio test; if it is zero,
         the textbook ratio test is used; if it is non-zero (should be
         positive), Harris' two-pass ratio test is used; in the latter
         case on the first pass basic variables (in the case of primal
         simplex) or reduced costs of non-basic variables (in the case
         of dual simplex) are allowed to slightly violate their bounds,
         but not more than (relax * tol_bnd) or (relax * tol_dj) (thus,
         relax is a percentage of tol_bnd or tol_dj) */
      double tol_bnd;
      /* relative tolerance used to check if the current basic solution
         is primal feasible */
      double tol_dj;
      /* absolute tolerance used to check if the current basic solution
         is dual feasible */
      double tol_piv;
      /* relative tolerance used to choose eligible pivotal elements of
         the simplex table in the ratio test */
      int round;
      /* solution rounding option:
         0 - report all computed values and reduced costs "as is"
         1 - if possible (allowed by the tolerances), replace computed
             values and reduced costs which are close to zero by exact
             zeros */
      double obj_ll;
      /* lower limit of the objective function; if on the phase II the
         objective function reaches this limit and continues decreasing,
         the solver stops the search */
      double obj_ul;
      /* upper limit of the objective function; if on the phase II the
         objective function reaches this limit and continues increasing,
         the solver stops the search */
      int it_lim;
      /* simplex iterations limit; if this value is positive, it is
         decreased by one each time when one simplex iteration has been
         performed, and reaching zero value signals the solver to stop
         the search; negative value means no iterations limit */
      double tm_lim;
      /* searching time limit, in seconds; if this value is positive,
         it is decreased each time when one simplex iteration has been
         performed by the amount of time spent for the iteration, and
         reaching zero value signals the solver to stop the search;
         negative value means no time limit */
      int out_frq;
      /* output frequency, in iterations; this parameter specifies how
         frequently the solver sends information about the solution to
         the standard output */
      double out_dly;
      /* output delay, in seconds; this parameter specifies how long
         the solver should delay sending information about the solution
         to the standard output; zero value means no delay */
      int branch; /* MIP */
      /* branching heuristic:
         0 - branch on first variable
         1 - branch on last variable
         2 - branch using heuristic by Driebeck and Tomlin
         3 - branch on most fractional variable */
      int btrack; /* MIP */
      /* backtracking heuristic:
         0 - select most recent node (depth first search)
         1 - select earliest node (breadth first search)
         2 - select node using the best projection heuristic
         3 - select node with best local bound */
      double tol_int; /* MIP */
      /* absolute tolerance used to check if the current basic solution
         is integer feasible */
      double tol_obj; /* MIP */
      /* relative tolerance used to check if the value of the objective
         function is not better than in the best known integer feasible
         solution */
      int mps_info; /* lpx_write_mps */
      /* if this flag is set, the routine lpx_write_mps outputs several
         comment cards that contains some information about the problem;
         otherwise the routine outputs no comment cards */
      int mps_obj; /* lpx_write_mps */
      /* this parameter tells the routine lpx_write_mps how to output
         the objective function row:
         0 - never output objective function row
         1 - always output objective function row
         2 - output objective function row if and only if the problem
             has no free rows */
      int mps_orig; /* lpx_write_mps */
      /* if this flag is set, the routine lpx_write_mps uses original
         row and column symbolic names; otherwise the routine generates
         plain names using ordinal numbers of rows and columns */
      int mps_wide; /* lpx_write_mps */
      /* if this flag is set, the routine lpx_write_mps uses all data
         fields; otherwise the routine keeps fields 5 and 6 empty */
      int mps_free; /* lpx_write_mps */
      /* if this flag is set, the routine lpx_write_mps omits column
         and vector names everytime if possible (free style); otherwise
         the routine never omits these names (pedantic style) */
      int mps_skip; /* lpx_write_mps */
      /* if this flag is set, the routine lpx_write_mps skips empty
         columns (i.e. which has no constraint coefficients); otherwise
         the routine outputs all columns */
      int lpt_orig; /* lpx_write_lpt */
      /* if this flag is set, the routine lpx_write_lpt uses original
         row and column symbolic names; otherwise the routine generates
         plain names using ordinal numbers of rows and columns */
      int presol; /* lpx_simplex */
      /* LP presolver option:
         0 - do not use LP presolver
         1 - use LP presolver */
      int binarize; /* lpx_intopt */
      /* if this flag is set, the routine lpx_intopt replaces integer
         columns by binary ones */
      int use_cuts; /* lpx_intopt */
      /* if this flag is set, the routine lpx_intopt tries generating
         cutting planes:
         LPX_C_COVER  - mixed cover cuts
         LPX_C_CLIQUE - clique cuts
         LPX_C_GOMORY - Gomory's mixed integer cuts
         LPX_C_ALL    - all cuts */
      double mip_gap; /* MIP */
      /* relative MIP gap tolerance */
};

typedef struct LPXKKT
{     /* this structure contains results reported by the routines which
         checks Karush-Kuhn-Tucker conditions (for details see comments
         to those routines) */
      /*--------------------------------------------------------------*/
      /* xR - A * xS = 0 (KKT.PE) */
      double pe_ae_max;
      /* largest absolute error */
      int    pe_ae_row;
      /* number of row with largest absolute error */
      double pe_re_max;
      /* largest relative error */
      int    pe_re_row;
      /* number of row with largest relative error */
      int    pe_quality;
      /* quality of primal solution:
         'H' - high
         'M' - medium
         'L' - low
         '?' - primal solution is wrong */
      /*--------------------------------------------------------------*/
      /* l[k] <= x[k] <= u[k] (KKT.PB) */
      double pb_ae_max;
      /* largest absolute error */
      int    pb_ae_ind;
      /* number of variable with largest absolute error */
      double pb_re_max;
      /* largest relative error */
      int    pb_re_ind;
      /* number of variable with largest relative error */
      int    pb_quality;
      /* quality of primal feasibility:
         'H' - high
         'M' - medium
         'L' - low
         '?' - primal solution is infeasible */
      /*--------------------------------------------------------------*/
      /* A' * (dR - cR) + (dS - cS) = 0 (KKT.DE) */
      double de_ae_max;
      /* largest absolute error */
      int    de_ae_col;
      /* number of column with largest absolute error */
      double de_re_max;
      /* largest relative error */
      int    de_re_col;
      /* number of column with largest relative error */
      int    de_quality;
      /* quality of dual solution:
         'H' - high
         'M' - medium
         'L' - low
         '?' - dual solution is wrong */
      /*--------------------------------------------------------------*/
      /* d[k] >= 0 or d[k] <= 0 (KKT.DB) */
      double db_ae_max;
      /* largest absolute error */
      int    db_ae_ind;
      /* number of variable with largest absolute error */
      double db_re_max;
      /* largest relative error */
      int    db_re_ind;
      /* number of variable with largest relative error */
      int    db_quality;
      /* quality of dual feasibility:
         'H' - high
         'M' - medium
         'L' - low
         '?' - dual solution is infeasible */
      /*--------------------------------------------------------------*/
      /* (x[k] - bound of x[k]) * d[k] = 0 (KKT.CS) */
      double cs_ae_max;
      /* largest absolute error */
      int    cs_ae_ind;
      /* number of variable with largest absolute error */
      double cs_re_max;
      /* largest relative error */
      int    cs_re_ind;
      /* number of variable with largest relative error */
      int    cs_quality;
      /* quality of complementary slackness:
         'H' - high
         'M' - medium
         'L' - low
         '?' - primal and dual solutions are not complementary */
} LPXKKT;

LPX *lpx_read_model(const char *model, const char *data, const char *output);
int lpx_read_bas(glp_prob *lp, const char *fname);
int lpx_exact(LPX *lp);
int lpx_interior(LPX *lp);
int lpx_intopt(LPX *lp);
void lpx_check_kkt(LPX *lp, int scaled, LPXKKT *kkt);
void lpx_check_int(LPX *lp, LPXKKT *kkt);
int lpx_get_ray_info(LPX *lp);
int lpx_write_mps(LPX *lp, const char *fname);
int lpx_write_bas(LPX *lp, const char *fname);
int lpx_write_freemps(LPX *lp, const char *fname);
int lpx_write_cpxlp(LPX *lp, const char *fname);
int lpx_print_prob(LPX *lp, const char *fname);
int lpx_print_sol(LPX *lp, const char *fname);
int lpx_print_sens_bnds(LPX *lp, const char *fname);
int lpx_print_ips(LPX *lp, const char *fname);
int lpx_print_mip(LPX *lp, const char *fname);
int lpx_get_int_parm(LPX *lp, int parm);
double lpx_get_real_parm(LPX *lp, int parm);
int lpx_get_num_rows(LPX *lp);
int lpx_get_num_cols(LPX *lp);
int lpx_get_class(LPX *lp);
void lpx_set_row_stat(LPX *lp, int i, int stat);
void lpx_set_col_stat(LPX *lp, int j, int stat);
void lpx_set_row_name(LPX *lp, int i, const char *name);
void lpx_set_col_name(LPX *lp, int j, const char *name);

#endif // # _LPX_H
