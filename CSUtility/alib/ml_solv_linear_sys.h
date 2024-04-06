#ifndef  A_MATRIX_LIB_SOLVE_SYMMETRIC_LINEAR_SYSTEM_H
#define  A_MATRIX_LIB_SOLVE_SYMMETRIC_LINEAR_SYSTEM_H

namespace matrixlib
{

//-------------------------------------------------------------------------------------------------
/*! \brief Function solves symmetric linear system A*x=b through LDLt decomposition.
  
  Computation is carried out in-place and only a small additional working space is required.
  <OL> <LI> TYPE  - the type of matrix element (float, double, long double). </LI>
       <LI> PRECISE - the type of intermediate variables, TYPE < PRECISE. </LI>  </OL>
  Additional template parameter "PRECISE" serves to increase accuracy of intermediate
  computation results. Usually PRECISE == double.

  \param  A          in: system's matrix,  out: LDLt decomposition of input matrix.
  \param  xb         in: right-side vector b,  out: solution of linear system.
  \param  N          system dimension (N-by-N matrix A and vector b of size N).
  \param  z          any value, parameter is only needed to identify the type "PRECISE".
  \param  bLDLtMade  true, if an input matrix is already decomposed into LDLt form.
  \param  F          begin pointer to the working space or 0 (then working space will be allocated).
  \param  L          end pointer to the working space or 0 (then working space will be allocated).
  \return            the size of required working space in bytes on success or when (A == 0),
                     otherwise 0. */
//-------------------------------------------------------------------------------------------------
template< class TYPE, class PRECISE >
int SolvSymLinSys( TYPE * const * A, TYPE * xb, int N, PRECISE z,
                   bool bLDLtMade = false, void * F = 0, void * L = 0 )
{
  z;
  ASSERT( A_MATRIX_LIB_IS_SUITABLE_NUMERICAL_TYPE( TYPE, PRECISE ) );
  ASSERT( std::numeric_limits<TYPE>::is_integer == false );

  if (N <= 0)
    return 0;

  int workSpace = N*sizeof(PRECISE);
  if (!bLDLtMade)
    workSpace = std::max<int>( workSpace, matrixlib::LDLt( (TYPE**)0, N, z, 0, 0 ) );

  if (A == 0)
    return workSpace;

  matrixlib::WorkSpace work( workSpace, &F, &L );

  if (!bLDLtMade && !matrixlib::LDLt( A, N, z, F, L ))
    return 0;

  int       i, j;
  PRECISE * y = (PRECISE*)F;

  for (i = 0; i < N; ++i)
  {
    PRECISE t = xb[i];

    for (j = 0; j < i; ++j) 
      t -= A[i][j] * y[j];
    y[i] = t;
  }
  
  for (i = 0; i < N; ++i)
    y[i] /= A[i][i];

  for (i = N-1; i >= 0; --i)
  {
    PRECISE t = y[i];

    for (j = i+1; j < N; ++j)
      t -= A[j][i] * xb[j];
    xb[i] = (TYPE)t;
  }
  return workSpace;
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function solves diagonal, symmetric linear system A*x=b via special version of
           LDLt decomposition.
  
  Computation is carried out in-place and only a small additional working space is required. Note,
  that the input matrix maybe cyclical, i.e. sub- and super-diagonals are cyclically warped.
  <OL> <LI> TYPE  - the type of matrix element (float, double, long double). </LI>
       <LI> PRECISE - the type of intermediate variables, TYPE < PRECISE. </LI>  </OL>
  Additional template parameter "PRECISE" serves to increase accuracy of intermediate
  computation results. Usually PRECISE == double.

  \param  A      in: system's matrix,  out: LDLt decomposition of input matrix.
  \param  xb     in: right-side vector b,  out: solution of linear system.
  \param  N      system dimension (N-by-N matrix A and vector b of size N).
  \param  nDiag  number of diagonals, should be odd.
  \param  z      any value, parameter is only needed to identify the type "PRECISE".
  \param  bLDLtMade  TRUE if input matrix is already decomposed into LDLt form.
  \param  F      begin pointer to the working space or 0 (then working space will be allocated).
  \param  L      end pointer to the working space or 0 (then working space will be allocated).
  \return        size of needed working space in bytes on success or when (A == 0), otherwise 0.
*/
//-------------------------------------------------------------------------------------------------
template< class TYPE, class PRECISE >
int SolvNDiagonalSymLinSys(TYPE * const * A, TYPE * xb, int N, int nDiag,
                           PRECISE z, bool bLDLtMade = false, void * F = 0, void * L = 0)
{
  z;
  ASSERT( A_MATRIX_LIB_IS_SUITABLE_NUMERICAL_TYPE( TYPE, PRECISE ) );
  ASSERT( std::numeric_limits<TYPE>::is_integer == false );

  if (N <= 0)
    return 0;

  int workSpace = N*sizeof(PRECISE);
  if (!bLDLtMade)
    workSpace = std::max<int>( workSpace, 
                               matrixlib::NDiagonal_LDLt( (TYPE**)0, N, nDiag, z, 0, 0 ) );
  if (A == 0)
    return workSpace;

  matrixlib::WorkSpace work( workSpace, &F, &L );

  if ( !bLDLtMade && !matrixlib::NDiagonal_LDLt( A, N, nDiag, z, F, L ) )
    return 0;

  const int HALF = nDiag/2;
  int       i, j;
  PRECISE * y = (PRECISE*)F;

  for(i = 0; i < N; ++i)
  {
    PRECISE t = xb[i];
    for(j = (HALF < i && i < (N-HALF)) ? (i-HALF) : 0; j < i; ++j)
    {
      t -= A[i][j] * y[j];
    }
    y[i] = t;
  }
  
  for(i = 0; i < N; ++i)
    y[i] /= A[i][i];

  for(i = N-1; i >= 0; --i)
  {
    PRECISE t = y[i];
    for(j = i+1; j < N; j = (j == (i+HALF) && j < (N-HALF)) ? (N-HALF) : (j+1))
    {
      t -= A[j][i] * xb[j];
    }
    xb[i] = (TYPE)t;
  }
  return workSpace;
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function solves symmetric linear system A*x=b by conjugate gradient method.

  <P> Iterations proceed until residual r=|b-A*x| is less then any fraction of |b|, or
  the number of passed iterations exceeds the system dimension (N). </P>

  <OL> <LI> TYPE - the type of matrix element (float, double, long double). </LI>
       <LI> PRECISE - the type of intermediate variables. </LI> </OL>

  <P> Additional template parameter "PRECISE" serves to increase accuracy of intermediate
  computation results. A variable of type "PRECISE" must have at least as many significant
  digits a variable of type "TYPE". </P>

  \param  A     dense system's matrix.
  \param  b     right-side vector.
  \param  x     solution of linear system.
  \param  N     system dimension (N-by-N matrix A and vector b of size N).
  \param  type  any value, parameter is only needed to identify the type "PRECISE".
  \param  F     begin pointer to the working space or 0 (then working space will be allocated).
  \param  L     end pointer to the working space or 0 (then working space will be allocated).
  \return       (1) the size of needed working space in bytes, when any of input valiable is zero,
                or (2) the number of fulfilled iterations on success, or (3) zero on failure. */
//-------------------------------------------------------------------------------------------------
template< class TYPE, class PRECISE >
int SolvSymLinSysByConjugateGradient( const TYPE * const * A, const TYPE * b, TYPE * x,
                                      unsigned int N, PRECISE type,
                                      void * F = 0, void * L = 0 )
{
  type;
  ASSERT( A_MATRIX_LIB_IS_SUITABLE_NUMERICAL_TYPE( TYPE, PRECISE ) );
  ASSERT( !(std::numeric_limits<TYPE>::is_integer) );

  unsigned int i = 0, nIter = 0, needSpace = 4*N*sizeof(PRECISE);
  if ((A == 0) || (b == 0) || (x == 0) || (N == 0))
    return needSpace;

  const double  PRECISE_MIN = std::numeric_limits<PRECISE>::min();
  const double  PRECISE_EPS = std::numeric_limits<PRECISE>::epsilon();
  const double  PRECISE_SMALL = PRECISE_MIN/(PRECISE_EPS*PRECISE_EPS);

  const double  TYPE_MIN = std::numeric_limits<TYPE>::min();
  const double  TYPE_EPS = std::numeric_limits<TYPE>::epsilon();
  const double  TYPE_SMALL = TYPE_MIN/TYPE_EPS;

  PRECISE               rr = (PRECISE)0, bb = (PRECISE)0;
  matrixlib::WorkSpace  work( needSpace, &F, &L );
  PRECISE *             p = (PRECISE*)F;
  PRECISE *             q = p + N;
  PRECISE *             r = q + N;
  PRECISE *             y = r + N;

  // Initialize.
  for (i = 0; i < N; i++)
  {
    PRECISE      sum = (PRECISE)0;
    const TYPE * row = A[i];

    for (unsigned int c = 0; c < N; c++)
      sum += row[c] * x[c];

    bb += b[i]*b[i];
    p[i] = (r[i] = (b[i] - sum));
    y[i] = x[i];
    rr += r[i]*r[i];
  }

  // Iterate.
  for (nIter = 0; nIter < N; nIter++)
  {
    PRECISE pAp = (PRECISE)0;

    // q = A*p.
    for (i = 0; i < N; i++)
    {
      PRECISE      sum = (PRECISE)0;
      const TYPE * row = A[i];

      for (unsigned int c = 0; c < N; c++)
        sum += row[c] * p[c];

      pAp += (q[i] = sum) * p[i];
    }

    if (pAp < rr*PRECISE_SMALL)
      return 0;
    double alpha = rr/pAp;
    double r1r1 = (PRECISE)0;

    for (i = 0; i < N; i++)
    {
      y[i] += p[i]*alpha;
      r[i] -= q[i]*alpha;

      r1r1 += r[i]*r[i];
    }

    if (rr < r1r1*PRECISE_SMALL)
      return 0;
    double beta = r1r1/rr;
    rr = r1r1;

    for (i = 0; i < N; i++)
    {
      p[i] = r[i] + p[i]*beta;
    }

    if ((rr < TYPE_SMALL) || (rr < bb*TYPE_EPS))
      break;
  }

  for (i = 0; i < N; i++)
    x[i] = (TYPE)(y[i]);

  return nIter;
}

} // namespace matrixlib

#endif // A_MATRIX_LIB_SOLVE_SYMMETRIC_LINEAR_SYSTEM_H


