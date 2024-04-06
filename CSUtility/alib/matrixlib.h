// Alaudin (Albert) Akhriev, Albert.Akhriev@biones.com, aaahaaah@hotmail.com, 1998-2003.

#ifndef  A_MATRIX_LIBRARY_H
#define  A_MATRIX_LIBRARY_H

#ifdef   _DEBUG
#define  A_MATRIX_LIB_TEST
#endif

#define  A_MATRIX_LIB_IS_SUITABLE_NUMERICAL_TYPE( TYPE, PRECISE ) ( \
  (sizeof(TYPE) <= sizeof(PRECISE)) \
  && (std::numeric_limits<TYPE>::is_specialized && std::numeric_limits<PRECISE>::is_specialized) \
  && (std::numeric_limits<TYPE>::is_signed      && std::numeric_limits<PRECISE>::is_signed) \
  && (std::numeric_limits<TYPE>::is_integer     == std::numeric_limits<PRECISE>::is_integer) )

#include "../alib/alib.h"
#include "ml_work_space.h"
#include "ml_util.h"
//#include "ml_multiply.h"
//#include "ml_multiply_new.h"
#include "ml_ldlt.h"
//#include "ml_norms.h"
//#include "ml_random.h"
//#include "ml_vector.h"
#include "ml_matrix.h"
//#include "ml_sparse_matrix.h"
//#include "ml_row_sparse_matrix.h"
//#include "ml_print.h"
//#include "ml_tridiag.h"
//#include "ml_pentadiag.h"
//#include "ml_solv_tri_sys.h"
#include "ml_solv_penta_sys.h"
#include "ml_util.h"
#include "ml_solv_linear_sys.h"
//#include "ml_invert.h"
//#include "ml_qdqt.h"
//#include "ml_eigen_solvers.h"
//#include "ml_svd.h"
//#include "ml_fix_dim_matrix.h"

namespace matrixlib
{

extern "C"
{

  bool Invert4x4( double * m, double * mEnd );
  bool Invert6x6( double * m, double * mEnd );

} // extern "C"


/*
bool ConjugateGradientSymLinSysSolver( const FltRowSparseMatrix  & A,
                                       const std::vector<double> & b,
                                             std::vector<double> & x );
*/

} // namespace matrixlib

#endif   // A_MATRIX_LIBRARY_H

