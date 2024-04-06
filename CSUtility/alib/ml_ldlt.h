#ifndef  A_MATRIX_LIB_LDLt_H
#define  A_MATRIX_LIB_LDLt_H

namespace matrixlib
{

//-------------------------------------------------------------------------------------------------
/** \brief Function carries out <b>A</b> = <b>L</b>*<b>D</b>*<b>L</b><sup>T</sup> decomposition
           of a symmetric matrix.

  On input, both the upper and the lower triangle of <b>A</b>, including diagonal, need be given.
  The factor <b>L</b> is returned in the lower triangle of <b>A</b> (elements below diagonal).
  It is assumed that diagonal elements of <b>L</b> are equal to 1, but they are not actually
  stored. Diagonal matrix <b>D</b> is stored in diagonal elements of <b>A</b>. The upper triangle
  submatrix of <b>A</b> (elements above diagonal) remains intact.<br>
  
  Following formulas provide some explanations for algorithm:
  \f[ L_{ii} = L_{jj} = 1
  \f]
  \f[ a_{ij} = a_{ji} = \sum_{k=0}^j d_k L_{jk} L_{ik} =
      \sum_{k=0}^{j-1} d_k L_{jk} L_{ik} + d_j L_{jj} L_{ij} \Longrightarrow
      L_{ij} = \frac{1}{d_j} \left( a_{ij} - \sum_{k=0}^{j-1} d_k L_{jk} L_{ik} \right)
  \f]
  \f[ (i = j) \Longrightarrow (L_{ii} = L_{jj} = 1) \Longrightarrow
      d_j = \left( a_{jj} - \sum_{k=0}^{j-1} d_k L_{jk}^2 \right)
  \f]
  \f[ vec[k] = d_k L_{jk} \hspace{10 mm} dia[j] = d_j
  \f]

  <OL> <LI> TYPE  - the type of matrix element (float, double, long double). </LI>
       <LI> PRECISE - the type of intermediate variables, PRECISE > TYPE. </LI>  </OL>
  Additional template parameter "PRECISE" serves to increase accuracy of intermediate
  computation results. Usually PRECISE == double.

  \param  A  symmetric matrix to be decomposed.
  \param  N  matrix dimension (N-by-N matrix).
  \param  z  parameter is only needed to identify the type "PRECISE".
  \param  F  begin pointer to the working space or 0 (then working space will be allocated).
  \param  L  end pointer to the working space or 0 (then working space will be allocated).
  \return    if (OK || A=0 || N=0) then required working space in bytes, otherwise 0. */
//-------------------------------------------------------------------------------------------------
template< class TYPE, class PRECISE >
int LDLt( TYPE * const * A, int N, PRECISE z, void * F = 0, void * L = 0 )
{
  if (N <= 0)
    return 0;

  z;
  ASSERT( A_MATRIX_LIB_IS_SUITABLE_NUMERICAL_TYPE( TYPE, PRECISE ) );

  const double MIN = (sizeof(TYPE) == sizeof(float)) ? (FLT_MIN/FLT_EPSILON)
                                                     : (DBL_MIN/DBL_EPSILON);
  int workSpace = 2*N*sizeof(PRECISE);
  if (A == 0)
    return workSpace;

  matrixlib::WorkSpace work( workSpace, &F, &L );

  int       i, j, k;
  PRECISE * vec = (PRECISE*)F;
  PRECISE * dia = vec + N;

  for (j = 0; j < N; ++j)
  {
    TYPE * aj = A[j];

    for (k = 0; k < j; ++k)
      vec[k] = aj[k] * dia[k];

    PRECISE d = aj[j];
    for (k = 0; k < j; ++k)
      d -= aj[k] * vec[k];

    aj[j] = (TYPE)( dia[j] = d );
    if (fabs( (double)d ) < MIN)
      return 0;

    for (i = j+1; i < N; ++i)
    {
      TYPE *  ai = A[i];
      PRECISE t = ai[j];

#if 1
      for (k = 0; k < j; ++k)
        t -= ai[k] * vec[k];
#else
      int j_mod_4 = j & ~3;
      for (k = 0; k < j_mod_4;)
      {
        t -= ai[k] * vec[k];  ++k;
        t -= ai[k] * vec[k];  ++k;
        t -= ai[k] * vec[k];  ++k;
        t -= ai[k] * vec[k];  ++k;
      }

      for (; k < j; ++k)
        t -= ai[k] * vec[k];
#endif

      ai[j] = (TYPE)(t/d);
    }
  }
  return workSpace;
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function carries out LDLt decomposition in place of <b>diagonal</b> symmetric matrix.

  The method implemented in this function is fully analogous to the usual LDLt decomposition,
  but exploits some advantages of LDLt. Namely, if the input matrix is a diagonal one (i.e.
  it consists of several sub- and super-diagonals) then "L" matrix of decomposition has
  special view. For example, for 10-by-10 pentadiagonal matrix:

         \f[ {\bf A} = \left( {\begin{array}{cccccccccc}
            a_0 & b_0 & c_0 & 0 & 0 & 0 & 0 & 0 & c_8 & b_9 \\
            b_0 & a_1 & b_1 & c_1 & 0 & 0 & 0 & 0 & 0 & c_9 \\
            c_0 & b_1 & a_2 & b_2 & c_2 & 0 & 0 & 0 & 0 & 0 \\
            0 & c_1 & b_2 & a_3 & b_3 & c_3 & 0 & 0 & 0 & 0 \\
            0 & 0 & c_2 & b_3 & a_4 & b_4 & c_4 & 0 & 0 & 0 \\
            0 & 0 & 0 & c_3 & b_4 & a_5 & b_5 & c_5 & 0 & 0 \\
            0 & 0 & 0 & 0 & c_4 & b_5 & a_6 & b_6 & c_6 & 0 \\
            0 & 0 & 0 & 0 & 0 & c_5 & b_6 & a_7 & b_7 & c_7 \\
            c_8 & 0 & 0 & 0 & 0 & 0 & c_6 & b_7 & a_8 & b_8 \\
            b_9 & c_9 & 0 & 0 & 0 & 0 & 0 & c_7 & b_8 & a_9
            \end{array}} \right) = {\bf L}{\bf D}{\bf L}^T
         \f]
         \f[ {\bf L} = \left( {\begin{array}{cccccccccc}
            1 & 0 & 0 & 0 & 0 & 0 & 0 & 0 & 0 & 0 \\
            e_0 & 1 & 0 & 0 & 0 & 0 & 0 & 0 & 0 & 0 \\
            f_0 & e_1 & 1 & 0 & 0 & 0 & 0 & 0 & 0 & 0 \\
            0 & f_1 & e_2 & 1 & 0 & 0 & 0 & 0 & 0 & 0 \\
            0 & 0 & f_2 & e_3 & 1 & 0 & 0 & 0 & 0 & 0 \\
            0 & 0 & 0 & f_3 & e_4 & 1 & 0 & 0 & 0 & 0 \\
            0 & 0 & 0 & 0 & f_4 & e_5 & 1 & 0 & 0 & 0 \\
            0 & 0 & 0 & 0 & 0 & f_5 & e_6 & 1 & 0 & 0 \\
            g_0 & g_1 & g_2 & g_3 & g_4 & g_5 & g_6 & g_7 & 1 & 0 \\
            h_0 & h_1 & h_2 & h_3 & h_4 & h_5 & h_6 & h_7 & h_8 & 1
            \end{array}} \right)
         \f]

  Note, that the input matrix maybe cyclical, i.e. sub- and super-diagonals are cyclically
  warped as in above example. The cost of decomposition is <i>O(N*nDiag<sup>2</sup>)</i>
  instead of <i>O(N<sup>3</sup>)</i> for the usual LDLt.

  <OL> <LI> TYPE  - the type of matrix element (float, double, long double). </LI>
       <LI> PRECISE - the type of intermediate variables, PRECISE > TYPE. </LI>  </OL>
  Additional template parameter "PRECISE" serves to increase accuracy of intermediate
  computation results. Usually PRECISE == double.

  TODO: The input and output matrices can be represented as several N-dimentional arrays,
        rather than N-by-N storage. Compact representation will improve performance.

  \param  A      symmetric matrix to be decomposed.
  \param  N      matrix dimension (N-by-N matrix).
  \param  nDiag  number of diagonals, should be odd.
  \param  z      parameter is only needed to identify the type "PRECISE".
  \param  F      begin pointer to the working space or 0 (then working space will be allocated).
  \param  L      end pointer to the working space or 0 (then working space will be allocated).
  \return        size of needed working space in bytes on success or when (A == 0), otherwise 0.
 */
//-------------------------------------------------------------------------------------------------
template< class TYPE, class PRECISE >
int NDiagonal_LDLt(TYPE * const * A, int N, int nDiag, PRECISE z, void * F = 0, void * L = 0)
{
  if (N <= 0 || (nDiag & 1) == 0 || (nDiag/2) > (N-1))
    return 0;

  z;
  ASSERT( A_MATRIX_LIB_IS_SUITABLE_NUMERICAL_TYPE( TYPE, PRECISE ) );

  const double MIN = (sizeof(TYPE) == sizeof(float)) ? (FLT_MIN/FLT_EPSILON)
                                                     : (DBL_MIN/DBL_EPSILON);
  int workSpace = 2*N*sizeof(PRECISE);
  if (A == 0)
    return workSpace;

  matrixlib::WorkSpace work( workSpace, &F, &L );

  const int HALF = nDiag/2;
  int       i, j, k;
  PRECISE * vec = (PRECISE*)F;
  PRECISE * dia = vec + N;

  for(j = 0; j < N; ++j)
  {
    TYPE * aj = A[j];
    int    k0 = (HALF < j && j < (N-HALF)) ? (j-HALF) : 0;

    for(k = k0; k < j; ++k)
      vec[k] = aj[k] * dia[k];

    PRECISE d = aj[j];
    for(k = k0; k < j; ++k)
      d -= aj[k] * vec[k];

    aj[j] = (TYPE)( dia[j] = d );
    if (fabs((double)d) < MIN)
      return 0;

    for(i = j+1; i < N; i = (i == (j+HALF) && i < (N-HALF)) ? (N-HALF) : (i+1))
    {
      TYPE *  ai = A[i];
      PRECISE t = ai[j];

      for(k = k0; k < j; ++k)
        t -= ai[k] * vec[k];
      ai[j] = (TYPE)(t/d);
    }
  }
  return workSpace;
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function carries out in-place Cholesky <b>A</b> = <b>L</b>*<b>L</b><sup>T</sup>
           decomposition of symmetric <b>positive</b> definite matrix.

  On input, only the upper triangle of <b>A</b>, including diagonal, need to be given.
  The Cholesky factor <b>L</b> is returned in the lower triangle of <b>A</b> including diagonal. 
  The upper triangle submatrix of <b>A</b> remains intact.<br>
  See for details: NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING (ISBN 0-521-43108-5)
                   Copyright (C) 1988-1992 by Cambridge University Press, Chapter 2.9, pp.96-97.

  <OL> <LI> TYPE  - the type of matrix element (float, double, long double). </LI>
       <LI> PRECISE - the type of intermediate variables, PRECISE > TYPE. </LI>  </OL>
  Additional template parameter "PRECISE" serves to increase accuracy of intermediate
  computation results. Usually PRECISE = double.

  TODO: Is temporal vector really needed?

  \param  A  symmetric matrix to be decomposed.
  \param  N  matrix dimension (N-by-N matrix).
  \param  s  any value needed to identify the type "PRECISE".
  \return    Ok == true, otherwise the matrix is not positive definite and returned destroyed. */
//-------------------------------------------------------------------------------------------------
template< class TYPE, class PRECISE >
bool Cholesky_LLt( TYPE * const * A, int N, PRECISE s )
{
  ASSERT( A_MATRIX_LIB_IS_SUITABLE_NUMERICAL_TYPE( TYPE, PRECISE ) );

  const TYPE MIN = (std::numeric_limits<TYPE>::min()/std::numeric_limits<TYPE>::epsilon());
  PRECISE *  dia = (PRECISE*)malloc( N*sizeof(PRECISE) );
  bool       ok = true;

  for (int i = 0; i < N; i++)
  {
    TYPE * ai = A[i];

    for (int j = i; j < N; j++)
    {
      TYPE * aj = A[j];

      s = ai[j];
      for (int k = 0; k < i; k++)
        s -= ai[k]*aj[k];

      if (i == j)
      {
        if (s <= MIN)
        {
          ok = false;
          i = N;
          break;
        }
        dia[i] = (PRECISE)sqrt( s );
      }
      else aj[i] = (TYPE)(s/dia[i]);
    }
  }

  for (int k = 0; k < N; k++)
    A[k][k] = (TYPE)(dia[k]);

  free( (void*)dia );
  return ok;
}

} // namespace matrixlib

#endif // A_MATRIX_LIB_LDLt_H

