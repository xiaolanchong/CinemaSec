#include "stdafx.h"

#ifdef ALIB_CHECK_MEMORY_LEAKAGE
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace matrixlib
{

bool Invert6x6( double * m, double * mEnd );

//-------------------------------------------------------------------------------------------------
// Function solves linear penta-diagonal system: A*x = b. All calculations are carried out with
// double-precision accuracy. Temporary external buffer can be used to speed up computation.
// The size of the buffer should be equal or greater than [7*n] elements.
// See description of template function matrixlib::SolvPentaDiagLinSysInPlace<T>(...) for details.
//-------------------------------------------------------------------------------------------------
void SolvPentaDiagLinSys(
  int           n,      // dimensions of system's matrix (n-by-n)
  const float * e,      // sub-sub-diagonal
  const float * a,      // sub-diagonal
  const float * d,      // diagonal
  const float * c,      // super-diagonal
  const float * f,      // super-super-diagonal
  const float * b,      // right-side vector of linear system
  float       * x,      // out: solution vector
  double      * pf,     // pointer to the first element of external buffer (or 0)
  double      * pl)     // pointer to the next after last element of external buffer (or 0)
{
  int      i;
  double * pLoc = 0;
  double * pBuf = ((pf == 0) || (pl == 0) || ((pf+7*n) > pl)) ? (pLoc = new double[7*n]) : pf;
  double * ta = pBuf + 0*n;
  double * tb = pBuf + 1*n;
  double * tc = pBuf + 2*n;
  double * td = pBuf + 3*n;
  double * te = pBuf + 4*n;
  double * tf = pBuf + 5*n;
  double * tx = pBuf + 6*n;

  for(i = 0; i < n; i++)
  {
    ta[i] = a[i];
    tb[i] = b[i];
    tc[i] = c[i];
    td[i] = d[i];
    te[i] = e[i];
    tf[i] = f[i];
  }

  matrixlib::SolvPentaDiagLinSysInPlace<double>( n, te, ta, td, tc, tf, tb, tx );
  for(i = 0; i < n; i++)
    x[i] = (float)(tx[i]);

  if (pLoc != 0)
    delete [] pLoc;
}


//-------------------------------------------------------------------------------------------------
// Function solves cyclic linear penta-diagonal system: A*x = b. All calculations are carried out
// with double-precision accuracy. External buffer can be used to speed up computation. The size
// of buffer should be equal or greater than [19*n] elements. Matrix of linear system looks as
// follows,
//
// /                                                                               \
// | d[0]     c[0]     f[0]      0        0        .        0       e[n-2]  a[n-1] |
// | a[0]     d[1]     c[1]     f[1]      0        .        0        0      e[n-1] |
// | e[0]     a[1]     d[2]     c[2]      0        .        0        0       0     |
// |  0       e[1]     a[2]     d[3]     c[3]      .        0        0       0     |
// |  .        .        .        .        .        .        .        .       .     |
// |  .        .        .        .        .        .        .        .       .     |
// |  .        .        .        .        .        .        d[n-3]  c[n-3]  f[n-3] |
// | f[n-2]    0        0        0        0        .        a[n-3]  d[n-2]  c[n-2] |
// | c[n-1]   f[n-1]    0        0        0        .        e[n-3]  a[n-2]  d[n-1] |
// \                                                                               /
//-------------------------------------------------------------------------------------------------
// Auxiliary recursive function.
static void __SolvCyclicPentaDiagLinSys(
  int            iter,  // iteration number 0..3
  int            n,     // dimensions of system's matrix (n-by-n)
  const double * uv,    // non-zero coefficients of u1[], ... , v3[]
  const double * e,     // sub-sub-diagonal
  const double * a,     // sub-diagonal
  const double * d,     // diagonal
  const double * c,     // super-diagonal
  const double * f,     // super-super-diagonal
  const double * b,     // right-side vector of linear system
  double       * x,     // out: solution vector
  double       * pf,    // pointer to the first element of external buffer
  double       * pl)    // pointer to the next after last element of external buffer
{
  if (iter == 0)
  {
    ASSERT((pf+6*n) <= pl);
    double * ta = pf;   pf += n;   memcpy(ta, a, n*sizeof(double));
    double * tb = pf;   pf += n;   memcpy(tb, b, n*sizeof(double));
    double * tc = pf;   pf += n;   memcpy(tc, c, n*sizeof(double));
    double * td = pf;   pf += n;   memcpy(td, d, n*sizeof(double));
    double * te = pf;   pf += n;   memcpy(te, e, n*sizeof(double));
    double * tf = pf;   pf += n;   memcpy(tf, f, n*sizeof(double));
    matrixlib::SolvPentaDiagLinSysInPlace<double>(n, te, ta, td, tc, tf, tb, x);
  }
  else
  {
    int      i;
    double   coef = 0.0;
    double * u = pf;   pf += n;
    double * z = pf;   pf += n;
    ASSERT(pf <= pl);

    for(i = 0; i < n; i++)
      u[i] = 0.0;

    switch(iter)
    {
      case 1: u[0] = uv[0];  u[n-1] = uv[1];  break;
      case 2: u[1] = uv[3];  u[n-1] = uv[4];  break;
      case 3: u[0] = uv[6];  u[n-2] = uv[7];  break;
    }

    matrixlib::__SolvCyclicPentaDiagLinSys(iter-1, n, uv, e, a, d, c, f, b, x, pf, pl);
    matrixlib::__SolvCyclicPentaDiagLinSys(iter-1, n, uv, e, a, d, c, f, u, z, pf, pl);

    switch(iter)
    {
      case 1: coef = (x[0] + uv[2]*x[n-1])/(1.0 + z[0] + uv[2]*z[n-1]);  break;
      case 2: coef = (x[1] + uv[5]*x[n-1])/(1.0 + z[1] + uv[5]*z[n-1]);  break;
      case 3: coef = (uv[8]*x[0] + x[n-2])/(1.0 + uv[8]*z[0] + z[n-2]);  break;
    }

    for(i = 0; i < n; i++)
      x[i] -= coef*z[i];
  }
}


void SolvCyclicPentaDiagLinSys(
  int           n,      // dimensions of system's matrix (n-by-n)
  const float * e,      // sub-sub-diagonal
  const float * a,      // sub-diagonal
  const float * d,      // diagonal
  const float * c,      // super-diagonal
  const float * f,      // super-super-diagonal
  const float * b,      // right-side vector of linear system
  float       * x,      // out: solution vector
  double      * pf,     // pointer to the first element of external buffer (or 0)
  double      * pl)     // pointer to the next after last element of external buffer (or 0)
{
  ASSERT(n >= 5);
  ASSERT(sizeof(float) <= sizeof(double));

  int      i;
  double * pLocal = 0;
  pf = (pf == 0 || pl == 0 || (pf+19*n) > pl) ? (pLocal = new double[19*n]) : pf;
  pl = pf + 19*n;
  double * ta = pf;   pf += n;
  double * tb = pf;   pf += n;
  double * tc = pf;   pf += n;
  double * td = pf;   pf += n;
  double * te = pf;   pf += n;
  double * tf = pf;   pf += n;
  double * tx = pf;   pf += n;
  ASSERT(pf <= pl);

  double uv[9];
  uv[0] = -d[0];             // u1[0]
  uv[1] = c[n-1];            // u1[n-1]
  uv[2] = -a[n-1]/d[0];      // v1[n-1]

  uv[3] = -d[1];             // u2[1]
  uv[4] = f[n-1];            // u2[n-1]
  uv[5] = -e[n-1]/d[1];      // v2[n-1]

  uv[6] = e[n-2];            // u3[1]
  uv[7] = -d[n-2];           // u3[n-2]
  uv[8] = -f[n-2]/d[n-2];    // v3[0]

  for(i = 0; i < n; i++)
  {
    ta[i] = a[i];
    tb[i] = b[i];
    tc[i] = c[i];
    td[i] = d[i];
    te[i] = e[i];
    tf[i] = f[i];
  }
  td[0] += d[0] + e[n-2]*f[n-2]/d[n-2];
  td[1] += d[1];
  td[n-2] += d[n-2];
  td[n-1] += (c[n-1]*a[n-1]/d[0]) + (e[n-1]*f[n-1]/d[1]);
  te[n-2] = te[n-1] = ta[n-1] = tc[n-1] = tf[n-2] = tf[n-1] = 0.0;

  matrixlib::__SolvCyclicPentaDiagLinSys(3, n, uv, te, ta, td, tc, tf, tb, tx, pf, pl);
  for(i = 0; i < n; i++)
    x[i] = (float)(tx[i]);

  if (pLocal != 0)
    delete [] pLocal;
}


//-------------------------------------------------------------------------------------------------
// Function solves cyclic linear penta-diagonal system: A*x = b. All calculations are carried out
// with double-precision accuracy. External buffer can be used to speed up computation. The size
// of buffer should be equal or greater than [9*n] elements. This an implementation of my own 
// algorithm. 
//-------------------------------------------------------------------------------------------------
void ASolvCyclicPentaDiagLinSys(
  int           n,      // dimensions of system's matrix (n-by-n)
  const float * _e,     // sub-sub-diagonal
  const float * _a,     // sub-diagonal
  const float * _d,     // diagonal
  const float * _c,     // super-diagonal
  const float * _f,     // super-super-diagonal
  const float * _b,     // right-side vector of linear system
  float       * _x,     // out: solution vector
  double      * pf,     // pointer to the first element of external buffer (or 0)
  double      * pl)     // pointer to the next after last element of external buffer (or 0)
{
//>>>>>
  ASSERT(n >= 7);
//>>>>>

  const int NBUF = 9;
  int       k, dim = n, NEED = NBUF*n;
  double *  pLocal = 0;
  double    m, p[6], q[6], H[6][6];
  double *  ini[NBUF];

  // Allocate memory for temporary buffers.
  pf = (pf == 0 || pl == 0 || (pf+NEED) > pl) ? (pLocal = new double[NEED]) : pf;
  pl = pf + NEED;

  double * a = ini[0] = pf;  pf += n;
  double * b = ini[1] = pf;  pf += n;
  double * c = ini[2] = pf;  pf += n;
  double * d = ini[3] = pf;  pf += n;
  double * e = ini[4] = pf;  pf += n;
  double * f = ini[5] = pf;  pf += n;
  double * x = ini[6] = pf;  pf += n;
  double * y = ini[7] = pf;  pf += n;
  double * z = ini[8] = pf;  pf += n;
  ASSERT(pf <= pl);

  // Set initial matrix values.
  for(k = 0; k < n; k++)
  {
    a[k] = _a[k];
    b[k] = _b[k];
    c[k] = _c[k];
    d[k] = _d[k];
    e[k] = _e[k];
    f[k] = _f[k];
    y[k] = 0.0;
    z[k] = 0.0;
  }

  q[0] = f[n-2];      p[0] = e[n-2];
  q[1] = c[n-1];      p[1] = a[n-1];
  q[2] = 0.0;         p[2] = 0.0;
  q[3] = f[n-1];      p[3] = e[n-1];
  q[4] = 0.0;         p[4] = 0.0;
  q[5] = 0.0;         p[5] = 0.0;

  // Reduce initial matrix to 6x6 one.
  while(n > 6)
  {
    m = y[0] = -p[0]/d[0];
    p[2]   += m*a[0];
    p[4]   += m*e[0];
    d[n-2] += m*q[0];
    a[n-2] += m*q[1];

    m = z[0] = -p[1]/d[0];
    p[3]   += m*a[0];
    p[5]   += m*e[0];
    c[n-2] += m*q[0];
    d[n-1] += m*q[1];

    m = -q[1]/d[0];
    q[3] += m*c[0];
    q[5] += m*f[0];
    b[n-1] += m*b[0];

    m = -q[0]/d[0];
    q[2]   += m*c[0];
    q[4]   += m*f[0];
    b[n-2] += m*b[0];

    m = -a[0]/d[0];
    d[1] += m*c[0];
    c[1] += m*f[0];
    b[1] += m*b[0];

    m = -e[0]/d[0];
    a[1] += m*c[0];
    d[2] += m*f[0];
    b[2] += m*b[0];

    q[0] = q[2];      p[0] = p[2];
    q[1] = q[3];      p[1] = p[3];
    q[2] = q[4];      p[2] = p[4];
    q[3] = q[5];      p[3] = p[5];
    q[4] = 0.0;       p[4] = 0.0;
    q[5] = 0.0;       p[5] = 0.0;

    n--;  e++;  a++;  d++;  c++;  f++;  b++;  x++;  y++;  z++;
  }

  // Solve remained 6x6 linear system with dense matrix.
  H[0][0]=d[0];  H[0][1]=c[0];  H[0][2]=f[0];  H[0][3]=0.0;   H[0][4]=p[0];  H[0][5]=p[1];
  H[1][0]=a[0];  H[1][1]=d[1];  H[1][2]=c[1];  H[1][3]=f[1];  H[1][4]=p[2];  H[1][5]=p[3];
  H[2][0]=e[0];  H[2][1]=a[1];  H[2][2]=d[2];  H[2][3]=c[2];  H[2][4]=f[2];  H[2][5]=0.0;
  H[3][0]=0.0;   H[3][1]=e[1];  H[3][2]=a[2];  H[3][3]=d[3];  H[3][4]=c[3];  H[3][5]=f[3];
  H[4][0]=q[0];  H[4][1]=q[2];  H[4][2]=e[2];  H[4][3]=a[3];  H[4][4]=d[4];  H[4][5]=c[4];
  H[5][0]=q[1];  H[5][1]=q[3];  H[5][2]=0.0;   H[5][3]=e[3];  H[5][4]=a[4];  H[5][5]=d[5];

  matrixlib::Invert6x6( H[0], H[0]+(6*6) );
  for(int r = 0; r < 6; r++)
  {
    x[r] = 0.0;
    for(int c = 0; c < 6; c++)
    {
      x[r] += H[r][c]*b[c];
    }
  }

  // Restore initial dimension and pointers.
  n = dim;
  a = ini[0];
  b = ini[1];
  c = ini[2];
  d = ini[3];
  e = ini[4];
  f = ini[5];
  x = ini[6];
  y = ini[7];
  z = ini[8];

  // Obtain solution from recurrence.
  for(k = n-7; k >= 0; k--)
  {
    x[k] = ((b[k] - f[k]*x[k+2] - c[k]*x[k+1]) / d[k]) + (y[k]*x[n-2] + z[k]*x[n-1]);
  }

  for(k = 0; k < n; k++)
  {
    _x[k] = (float)(x[k]);
  }

  if (pLocal != 0)
    delete [] pLocal;
}

} // namespace matrixlib
