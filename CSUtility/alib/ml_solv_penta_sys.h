///////////////////////////////////////////////////////////////////////////////////////////////////
// Adopted from:
// 1. Cheney-Kincaid, Numerical Mathematics and Computing, 4th ed.
//    Brooks/Cole Publ. Co. ISBN 0-534-20112-1,
//    ftp://ftp.ma.utexas.edu/pub/cheney-kincaid
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef  A_MATRIX_LIB_SOLVE_PENTA_DIAGONAL_LINEAR_SYSTEM_H
#define  A_MATRIX_LIB_SOLVE_PENTA_DIAGONAL_LINEAR_SYSTEM_H

namespace matrixlib
{

//-------------------------------------------------------------------------------------------------
// Function solves penta-diagonal linear system in place (i.e. input arrays will be changed during
// calculation). System: A*x = b. Matrix A is (n x n) penta-diagonal,
//
// /                                                                               \
// | d[0]     c[0]     f[0]      0        0        .        0        0       0     |
// | a[0]     d[1]     c[1]     f[1]      0        .        0        0       0     |
// | e[0]     a[1]     d[2]     c[2]     f[2]      .        0        0       0     |
// |  0       e[1]     a[2]     d[3]     c[3]      .        0        0       0     |
// |  .        .        .        .        .        .        .        .       .     |
// |  .        .        .        .        .        .        .        .       .     |
// |  .        .        .        .        .        .        d[n-3]  c[n-3]  f[n-3] |
// |  0        0        0        0        0        .        a[n-3]  d[n-2]  c[n-2] |
// |  0        0        0        0        0        .        e[n-3]  a[n-2]  d[n-1] |
// \                                                                               /
//-------------------------------------------------------------------------------------------------
template< class T >
void SolvPentaDiagLinSysInPlace( int n, T * e, T * a, T * d, T * c, T * f, T * b, T * x )
{
  int     i;
  double  mult;

  ASSERT( n >= 3 );
  ASSERT( std::numeric_limits<T>::is_specialized && !(std::numeric_limits<T>::is_integer) );

  for(i = 1; i < (n-1); i++)
  {
    mult = a[i-1] / d[i-1];
    d[i] -= (T)(mult * c[i-1]);
    c[i] -= (T)(mult * f[i-1]);
    b[i] -= (T)(mult * b[i-1]);
    mult = e[i-1] / d[i-1];
    a[i] -= (T)(mult * c[i-1]);
    d[i+1] -= (T)(mult * f[i-1]);
    b[i+1] -= (T)(mult * b[i-1]);
  }

  T bn1 = b[n-1];
  T bn2 = b[n-2];

  mult = a[n-2]*c[n-2] - d[n-2]*d[n-1];
  x[n-2] = (T)((c[n-2]*bn1 - bn2*d[n-1]) / mult);
  x[n-1] = (T)((a[n-2]*bn2 - bn1*d[n-2]) / mult);

  for(i = n-3; i >= 0; i--)
  {
    x[i] = (b[i] - c[i]*x[i+1] - f[i]*x[i+2]) / d[i];
  }
}


void SolvPentaDiagLinSys(
  int           n,           // dimensions of system's matrix (n-by-n)
  const float * e,           // sub-sub-diagonal
  const float * a,           // sub-diagonal
  const float * d,           // diagonal
  const float * c,           // super-diagonal
  const float * f,           // super-super-diagonal
  const float * b,           // right-side vector of linear system
  float       * x,           // out: solution vector
  double      * pf = 0,   // pointer to the first element of external buffer
  double      * pl = 0);  // pointer to the next after last element of external buffer


void SolvCyclicPentaDiagLinSys(
  int           n,           // dimensions of system's matrix (n-by-n)
  const float * e,           // sub-sub-diagonal
  const float * a,           // sub-diagonal
  const float * d,           // diagonal
  const float * c,           // super-diagonal
  const float * f,           // super-super-diagonal
  const float * b,           // right-side vector of linear system
  float       * x,           // out: solution vector
  double      * pf = 0,   // pointer to the first element of external buffer
  double      * pl = 0);  // pointer to the next after last element of external buffer


void ASolvCyclicPentaDiagLinSys(
  int           n,           // dimensions of system's matrix (n-by-n)
  const float * e,           // sub-sub-diagonal
  const float * a,           // sub-diagonal
  const float * d,           // diagonal
  const float * c,           // super-diagonal
  const float * f,           // super-super-diagonal
  const float * b,           // right-side vector of linear system
  float       * x,           // out: solution vector
  double      * pf = 0,   // pointer to the first element of external buffer
  double      * pl = 0);  // pointer to the next after last element of external buffer


//-------------------------------------------------------------------------------------------------
/** \brief Function solves linear system <b>Ax</b> = <b>w</b> with <i>cyclic</i> symmetric
           pentadiagonal matrix with constant coefficients.

  System matrix has the following view (e.g. n=10):
  \f[
      A := \left[ {\begin{array}{cccccccccc}
      {d_{0}} & {c_{0}} & {f_{0}} & 0 & 0 & 0 & 0 & 0 & {e_{8}} & {a_{9}} \\
      {a_{0}} & {d_{1}} & {c_{1}} & {f_{1}} & 0 & 0 & 0 & 0 & 0 & {e_{9}} \\
      {e_{0}} & {a_{1}} & {d_{2}} & {c_{2}} & {f_{2}} & 0 & 0 & 0 & 0 & 0 \\
      0 & {e_{1}} & {a_{2}} & {d_{3}} & {c_{3}} & {f_{3}} & 0 & 0 & 0 & 0 \\
      0 & 0 & {e_{2}} & {a_{3}} & {d_{4}} & {c_{4}} & {f_{4}} & 0 & 0 & 0 \\
      0 & 0 & 0 & {e_{3}} & {a_{4}} & {d_{5}} & {c_{5}} & {f_{5}} & 0 & 0 \\
      0 & 0 & 0 & 0 & {e_{4}} & {a_{5}} & {d_{6}} & {c_{6}} & {f_{6}} & 0 \\
      0 & 0 & 0 & 0 & 0 & {e_{5}} & {a_{6}} & {d_{7}} & {c_{7}} & {f_{7}} \\
      {f_{8}} & 0 & 0 & 0 & 0 & 0 & {e_{6}} & {a_{7}} & {d_{8}} & {c_{8}} \\
      {c_{9}} & {f_{9}} & 0 & 0 & 0 & 0 & 0 & {e_{7}} & {a_{8}} & {d_{9}}
      \end{array}} \right] 
  \f]

  I factorize pentadiagonal matrix into <i>almost</i> lower-triangle, cyclic, tridiagonal
  matrix <b>Q</b> and its transposion:
  <p align="center"><b>A</b> = <b>QQ</b><sup>T</sup></p>
  Then the linear system will be solved by two steps:
  <p align="center"><b>Qy</b> = <b>w</b></p>
  <p align="center"><b>Q</b><sup>T</sup><b>x</b> = <b>y</b></p>

  \param  n     system's size (number of unknowns).
  \param  a     the value of diagonal elements of system matrix <b>A</b>.
  \param  b     the value of sub-diagonal elements of system matrix <b>A</b>.
  \param  c     the value of sub-sub-diagonal elements of system matrix <b>A</b>.
  \param  w     right-side vector of linear system, length must be equal to <i>n</i>.
  \param  x     out: solution vector, length must be equal to <i>n</i>.
  \param  F     pointer to the beginning of temporal buffer (working space).
  \param  L     pointer to the end of temporal buffer (STL convention).
  \param  type  parameter only defines the type of intermediate results and isn't actually used.
  \return       if (Ok || w=0 || x=0 || F=0 || L=0) then required size of temporal buffer in bytes,
                otherwise negative value. */
//-------------------------------------------------------------------------------------------------
template< class T1, class T2, class T3, class T4 >
int SolvCyclicConstCoefSymmPentaDiagLinSys(
  int        n,
  T1         a,
  T1         b,
  T1         c,
  const T2 * w,
  T3       * x,
  void     * F,
  void     * L,
  T4         type)
{
  type;
  ASSERT( std::numeric_limits<T1>::is_specialized && !(std::numeric_limits<T1>::is_integer) );
  ASSERT( std::numeric_limits<T2>::is_specialized && !(std::numeric_limits<T2>::is_integer) );
  ASSERT( std::numeric_limits<T3>::is_specialized && !(std::numeric_limits<T3>::is_integer) );
  ASSERT( std::numeric_limits<T4>::is_specialized && !(std::numeric_limits<T4>::is_integer) );

  if (n < 5)
    return (-1);   // too small dimension

  if ((w == 0) || (x == 0) || (F == 0) || (L == 0))
    return (4*n*sizeof(T4));

  T4 * p = (T4*)F;
  T4 * q = p + n;
  T4 * r = q + n;
  T4 * y = r + n;

  if (((T4*)(y+n)) > ((T4*)L))
    return (-2);   // no enough working space

  // Calculate coefficients of factorization matrix Q (beta and gamma).

  T4  mult = (T4)1;

  if (a < (T1)0)
  {
    mult = (T4)(-1);   a = -a;   b = -b;   c = -c;
  }

  T4  d1 = (T4)(a+2.0*b+2.0*c);
  T4  d2 = (T4)(a-2.0*b+2.0*c);

  if ((a < FLT_EPSILON) || (d1 < (T4)(10.0*FLT_EPSILON)) || (d2 < (T4)(10.0*FLT_EPSILON)))
    return (-3);   // too small matrix coefficients

  T4  beta = (T4)(0.5*(sqrt( (double)d1 ) - sqrt( (double)d2 )));

  if (fabs( (double)beta ) < FLT_EPSILON)
    return (-4);   // beta is about zero

  d1 = (T4)(b/beta);
  d2 = (T4)(d1*d1 - 4.0*c);

  if (d2 < FLT_EPSILON)
    return (-5);   // discriminant negative? 
  d2 = (T4)sqrt( (double)d2 );

  T4  alpha = (T4)(0.5*(d1-d2));
  T4  gamma = (T4)(0.5*(d1+d2));

  if (fabs( (double)alpha ) < fabs( (double)gamma ))
    std::swap( alpha, gamma );

  if (fabs( (double)alpha ) < FLT_EPSILON)
    return (-6);   // alpha divisor is about zero

  beta  /= alpha;
  gamma /= alpha;
  mult = (T4)(1.0/(mult*alpha*alpha));

  // Solve first linear system Q*y = w.
  
  p[0] = w[0];             q[0] = -gamma;       r[0] = -beta;
  p[1] = w[1]-beta*w[0];   q[1] = beta*gamma;   r[1] = (beta*beta - gamma);

  int k;
  for(k = 2; k < n; k++)
  {
    p[k] = w[k] - gamma*p[k-2] - beta*p[k-1];
    q[k] = -gamma*q[k-2] - beta*q[k-1];
    r[k] = -gamma*r[k-2] - beta*r[k-1];
  }

  T4  det = (T4)(1.0 - q[n-1]*r[n-2] - q[n-2] + (q[n-2] - 1.0)*r[n-1]);

  if (fabs( (double)det ) < (FLT_MIN/FLT_EPSILON))
    return (-7);   // determinant is about zero
    
  T4  y_n_2 = (y[n-2] = (T4)(p[n-2]*(1.0 - r[n-1]) + p[n-1]*r[n-2]) / det);
  T4  y_n_1 = (y[n-1] = (T4)(q[n-1]*p[n-2] + (1.0 - q[n-2])*p[n-1]) / det);

  for(k = 0; k < (n-2); k++)
  {
    y[k] = p[k] + q[k]*y_n_2 + r[k]*y_n_1;
  }

  // Solve second linear system Qt*x = y.

  p[n-1] = y[n-1];                 q[n-1] = -beta;                r[n-1] = -gamma;
  p[n-2] = y[n-2] - beta*y[n-1];   q[n-2] = -gamma + beta*beta;   r[n-2] = beta*gamma;

  for(k = (n-3); k >= 0; k--)
  {
    p[k] = y[k] - beta*p[k+1] - gamma*p[k+2];
    q[k] = -beta*q[k+1] - gamma*q[k+2];
    r[k] = -beta*r[k+1] - gamma*r[k+2];
  }
  
  det = (T4)(1.0 - r[0]*q[1] - q[0] + r[1]*(q[0] - 1.0));

  if (fabs( (double)det ) < (FLT_MIN/FLT_EPSILON))
    return (-8);   // determinant is about zero

  T4  x0 = (T4)(p[0]*(1.0 - r[1]) + r[0]*p[1]) / det;
  T4  x1 = (T4)(p[1]*(1.0 - q[0]) + p[0]*q[1]) / det;

  for(k = 2; k < n; k++)
  {
    x[k] = (T3)((p[k] + q[k]*x0 + r[k]*x1)*mult);
  }
  x[0] = (T3)(x0*mult);
  x[1] = (T3)(x1*mult);

  return (4*n*sizeof(T4));
}

} // namespace matrixlib

#endif // A_MATRIX_LIB_SOLVE_PENTA_DIAGONAL_LINEAR_SYSTEM_H
