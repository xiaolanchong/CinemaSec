#ifndef  A_MATRIX_LIB_UTILITY_H
#define  A_MATRIX_LIB_UTILITY_H

namespace matrixlib
{

//-------------------------------------------------------------------------------------------------
/** \brief Function calculates reciprocal value of any numerical one.

  \param  x  numerical value to be inverted.
  \return    <i>1/x</i> if <i>x</i> is nonzero, otherwise 0. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline T Reciprocal( T x )
{
  ASSERT( std::numeric_limits<T>::is_specialized && !(std::numeric_limits<T>::is_integer) );

  const T min = (std::numeric_limits<T>::min()) / (std::numeric_limits<T>::epsilon());
  return ((fabs( x ) > min) ? (T)(1.0/x) : (T)(0.0));
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function checks whether specified matrices have equal dimensions or not.
    \param  A  first matrix.
    \param  B  second matrix.
    \return    TRUE if both matrices have equal dimensions, otherwise FALSE. */
//-------------------------------------------------------------------------------------------------
template< class AT, class BT >
bool AreDimensionsEqual(AT & A, BT & B)
{
  return ( A.nrow() == B.nrow() && A.ncol() == B.ncol() );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes: Sqrt(a<sup>2</sup> + b<sup>2</sup>) without destructive
           underflow or overflow. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline T pythag( T a, T b ) 
{
  T absa = (T)fabs( (T)a );
  T absb = (T)fabs( (T)b );

  if (absa > absb)
  {
    T t = absb/absa;
    return (T)( absa*sqrt( (T)(1.0+t*t) ) );
  }
  else
  {
    T t = absa/absb;
    return ( (absb == (T)0) ? (T)0 : (T)(absb*sqrt( (T)(1.0+t*t) )) );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns: |a| * Sign(b). */
//-------------------------------------------------------------------------------------------------
template< class T >
inline T abs_a_mul_sign_b( T a, T b )
{
  return (T)( (b >= (T)0) ? fabs( (T)a ) : -fabs( (T)a ) );
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function changes indexing from C-style to Fortran-style A[0,0] --> A[1,1] and so on.

  Dangerous function!!!

  \param  A     matrix to be transformed (only row pointers will be changed).
  \param  nRow  number of rows.
  \return       pointer to the modified array of row pointers. */
//-------------------------------------------------------------------------------------------------
template< class T >
T ** indexing_from_00_to_11(T ** A, unsigned int nRow)
{
  for(unsigned int i = 0; i < nRow; i++)
    --(A[i]);
  return (--A);
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function changes indexing from Fortran-style to C-style A[1,1] --> A[0,0] and so on.

  Dangerous function!!!

  \param  A     matrix to be transformed (only row pointers will be changed).
  \param  nRow  number of rows.
  \return       pointer to the modified array of row pointers. */
//-------------------------------------------------------------------------------------------------
template< class T >
T ** indexing_from_11_to_00(T ** A, unsigned int nRow)
{
  ++A;
  for(unsigned int i = 0; i < nRow; i++)
    ++(A[i]);
  return A;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function copies the source matrix <b>A</b> to the destination one <b>B</b>.

  \param  A  the source matrix.
  \param  B  the destination matrix.
  \return    reference to the destination matrix. */
//-------------------------------------------------------------------------------------------------
template< class AT, class BT >
BT & Copy( const AT & A, BT & B )
{
  typedef  typename BT::value_type  value_type;

  if (reinterpret_cast<const void*>( &A ) != reinterpret_cast<const void*>( &B ))
  {
    if (!matrixlib::AreDimensionsEqual( A, B ))
      B.resize( A.nrow(), A.ncol(), value_type(0), false );
    alib::copy( A.begin(), A.end(), B.begin(), B.end(), value_type(0) );
  }
  return B;
}



//-------------------------------------------------------------------------------------------------
/*! \brief Function copies lower-triangle submatrix of <b>A</b> to
           upper-triangle submatrix of <b>B</b>.

  \param  A                   source matrix.
  \param  B                   destination matrix.
  \param  bIncludingDiagonal  flag tells whether diagonal to be copied or not. */
//-------------------------------------------------------------------------------------------------
template< class AT, class BT >
void LowerToUpper( const AT & A, BT & B, bool bIncludingDiagonal = true )
{
  ASSERT( A.square() && AreDimensionsEqual( A, B ) );

  const AT::value_type * const * a = A.data();
  BT::value_type       * const * b = B.data();
  unsigned int                   nRow = B.nrow();
  unsigned int                   nCol = B.ncol();
  unsigned int                   inc = bIncludingDiagonal ? 0 : 1;

  for (unsigned int r = 0; r < nRow; r++)
  {
    for (unsigned int c = r+inc; c < nCol; c++)
    {
      b[r][c] = (BT::value_type)(a[c][r]);
    }
  }
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function copies upper-triangle submatrix of <b>A</b> to
           lower-triangle submatrix of <b>B</b>.
  \param  A                   source matrix.
  \param  B                   destination matrix.
  \param  bIncludingDiagonal  flag tells whether diagonal to be copied or not. */
//-------------------------------------------------------------------------------------------------
template< class AT, class BT >
void UpperToLower(const AT & A, BT & B, bool bIncludingDiagonal = true)
{
  ASSERT( A.square() && AreDimensionsEqual( A, B ) );

  const AT::value_type * const * a = A.data();
  BT::value_type       * const * b = B.data();
  unsigned int                   nRow = B.nrow();
  unsigned int                   nCol = B.ncol();
  unsigned int                   inc = bIncludingDiagonal ? 0 : 1;

  for(unsigned int r = 0; r < nRow; r++)
  {
    for(unsigned int c = r+inc; c < nCol; c++)
    {
      b[c][r] = (BT::value_type)(a[r][c]);
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates quadratic form: <b>a</b><sup>T</sup> <b>Q</b> <b>b</b>.

  \param  avec  left vector.
  \param  Q     matrix of quadratic form.
  \param  bvec  right vector.
  \return       value of quadratic form. */
//-------------------------------------------------------------------------------------------------
template< class AT, class MT, class BT >
double QuadForm( const AT & avec, const MT & Q, const BT & bvec )
{
  unsigned int                            nRow = (unsigned int)(avec.size());
  unsigned int                            nCol = (unsigned int)(bvec.size());
  const typename AT::value_type         * a = &(*(avec.begin()));
  const typename BT::value_type         * b = &(*(bvec.begin()));
  const typename MT::value_type * const * m = Q.data();
  typename MT::precise_type               total = (typename MT::precise_type)0;
  
  ASSERT( (Q.nrow() == nRow) && (Q.ncol() == nCol) );

  for (unsigned int r = 0; r < nRow; r++)
  {
    const typename MT::value_type * row = m[r];
    typename MT::precise_type       sum = (typename MT::precise_type)0;
   
    for (unsigned int c = 0; c < nCol; c++)
    {
      sum += (typename MT::precise_type)(row[c] * b[c]);
    }
    total += (typename MT::precise_type)(a[r] * sum);
  }
  return (double)total;
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function adds outer-product of two vectors to the specified matrix:
           <b>A</b> += coef <b>a</b> <b>b</b><sup>T</sup>.
    \param  avec    first vector.
    \param  bvec    second vector.
    \param  coef    multiplier.
    \param  A       destination matrix.
    \param  bUpper  if TRUE then only upper-triangle submatrix and diagonal will be altered. */
//-------------------------------------------------------------------------------------------------
template< class AT, class BT, class CT, class MT >
void AddOuterProductMatrix(const AT & avec, const BT & bvec, CT coef, MT & A, bool bUpper = false)
{
  unsigned int             nRow = avec.size();
  unsigned int             nCol = bvec.size();
  const AT::value_type   * a = avec.begin();
  const BT::value_type   * b = bvec.begin();
  MT::value_type * const * m = A.data();
  
  ASSERT( A.nrow() == nRow && A.ncol() == nCol );

  for(unsigned int r = 0; r < nRow; r++)
  {
    MT::precise_type  t = (MT::precise_type)(a[r] * coef);
    MT::value_type *  row = m[r];
   
    for(unsigned int c = (bUpper ? r : 0); c < nCol; c++)
    {
      row[c] += (MT::value_type)(t * b[c]);
    }
  }
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function sets all matrix elements to zero. */
//-------------------------------------------------------------------------------------------------
template< class MT >
void SetZero(MT & A)
{
  std::fill( A.begin(), A.end(), ((MT::value_type)0) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function sets all matrix's elements below diagonal to zero. */
//-------------------------------------------------------------------------------------------------
template< class MT >
void SetLowerZero( MT & A )
{
  ASSERT( A.square() );

  MT::value_type * const * m = A.data();
  unsigned int             nRow = A.nrow();

  for (unsigned int r = 0; r < nRow; r++)
  {
    MT::value_type * row = m[r];

    for (unsigned int c = 0; c < r; c++)
    {
      row[c] = (MT::value_type)0;
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function sets all matrix's elements above diagonal to zero. */
//-------------------------------------------------------------------------------------------------
template< class MT >
void SetUpperZero( MT & A )
{
  ASSERT( A.square() );

  MT::value_type * const * m = A.data();
  unsigned int             nRow = A.nrow();
  unsigned int             nCol = A.ncol();

  for (unsigned int r = 0; r < nRow; r++)
  {
    MT::value_type * row = m[r];

    for (unsigned int c = r+1; c < nCol; c++)
    {
      row[c] = (MT::value_type)0;
    }
  }
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function fills diagonal by specified value. */
//-------------------------------------------------------------------------------------------------
template< class MT, class CT >
void FillDiagonal(MT & A, CT vfill)
{
  std::fill( A.diagonal_begin(), A.diagonal_end(), ((MT::value_type)vfill) );
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function checks matrix for symmetry. */
//-------------------------------------------------------------------------------------------------
template< class MT >
bool IsSymmetric(const MT & A)
{
  if ( !A.square() )
    return false;

  const MT::value_type EPS = (MT::value_type)(3*std::numeric_limits<MT::value_type>::epsilon());

  const MT::value_type * const * m = A.data();
  unsigned int                   nRow = A.nrow();
  unsigned int                   nCol = A.ncol();
  
  for(unsigned int r = 0; r < nRow; r++)
  {
    for(unsigned int c = r+1; c < nCol; c++)
    {
      MT::value_type  m1 = m[r][c];
      MT::value_type  m2 = m[c][r];
      
      if ( fabs((double)(m2-m1)) > EPS*(fabs((double)m1)+fabs((double)m2)) )
        return false;
    }
  }
  return true;
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function repairs symmetry of this matrix. */
//-------------------------------------------------------------------------------------------------
template< class MT >
MT & Symmetrize(MT & A)
{
  MT::value_type * const * m = A.data();
  unsigned int             nRow = A.nrow();
  unsigned int             nCol = A.ncol();
  
  for(unsigned int r = 0; r < nRow; r++)
  {
    for(unsigned int c = r+1; c < nCol; c++)
    {
      MT::value_type & m1 = m[r][c];
      MT::value_type & m2 = m[c][r];
      m1 = ( m2 = (MT::value_type)(0.5*(m1+m2)) );
    }
  }
  return A;
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function sets this matrix to the identity one. */
//-------------------------------------------------------------------------------------------------
template< class MT >
MT & SetUnity(MT & A)
{
  ASSERT( A.square() );
  std::fill( A.begin(), A.end(), ((MT::value_type)0) );
  std::fill( A.diagonal_begin(), A.diagonal_end(), ((MT::value_type)1) );
  return A;
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function calculates trace of specified matrix. */
//-------------------------------------------------------------------------------------------------
template< class MT >
double Trace(const MT & A)
{
  ASSERT( A.square() );
  return (double)(std::accumulate( A.diagonal_begin(), A.diagonal_end(), ((MT::precise_type)0) ));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function copies submatrix of <b>A</b> to <b>B</b>.

  \param  A   source matrix.
  \param  ar  row index of the top-left element of <b>A</b> to begin coping from.
  \param  ac  column index of the top-left element of <b>A</b> to begin coping from.
  \param  B   destination matrix.
  \param  br  row index of the top-left element of <b>B</b> to begin coping to.
  \param  bc  column index of the top-left element of <b>B</b> to begin coping to.
  \param  nr  number of rows to copy.
  \param  nc  number of columns of copy. */
//-------------------------------------------------------------------------------------------------
template< class AT, class BT >
void CopySubMatrix( const AT & A, unsigned int ar, unsigned int ac,
                          BT & B, unsigned int br, unsigned int bc,
                                  unsigned int nr, unsigned int nc )
{
  ASSERT( (ar+nr) <= A.nrow() && (ac+nc) <= A.ncol() );
  ASSERT( (br+nr) <= B.nrow() && (bc+nc) <= B.ncol() );

  const typename AT::value_type * const * a = A.data();
        typename BT::value_type * const * b = B.data();

  for (unsigned int r = 0; r < nr; r++)
  {
    const typename AT::value_type * arow = a[r+ar];
          typename BT::value_type * brow = b[r+br];

    for (unsigned int c = 0; c < nc; c++)
    {
      brow[c+bc] = (typename BT::value_type)(arow[c+ac]);
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function multiplies matrix by the right-side vector: <b>b</b> = <b>A</b><b>a</b>. */
//-------------------------------------------------------------------------------------------------
template< class MT, class AT, class BT >
BT & MulRightVector( const MT & Q, const AT & avec, BT & bvec )
{
  ASSERT( (unsigned int)(avec.size()) == Q.ncol() );
  ASSERT( (unsigned int)(bvec.size()) == Q.nrow() );
  ASSERT( reinterpret_cast<const void*>( &avec ) != reinterpret_cast<const void*>( &bvec ) );

  unsigned int                   nRow = Q.nrow();
  unsigned int                   nCol = Q.ncol();
  const AT::value_type         * a = &(*(avec.begin()));
        BT::value_type         * b = &(*(bvec.begin()));
  const MT::value_type * const * m = Q.data();

  for (unsigned int r = 0; r < nRow; r++)
  {
    const MT::value_type * row = m[r];
    MT::precise_type       sum = (MT::precise_type)0;

    for (unsigned int c = 0; c < nCol; c++)
    {
      sum += (MT::precise_type)(row[c] * a[c]);
    }
    b[r] = (BT::value_type)sum;
  }
  return bvec;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function multiplies matrix by left-side vector:
           <b>b</b><sup>T</sup> = <b>a</b><sup>T</sup><b>A</b>. */
//-------------------------------------------------------------------------------------------------
template< class AT, class MT, class BT >
BT & MulLeftVector( const AT & avec, const MT & Q, BT & bvec )
{
  ASSERT( (unsigned int)(bvec.size()) == Q.ncol() );
  ASSERT( (unsigned int)(avec.size()) == Q.nrow() );
  ASSERT( &avec != &bvec );

  unsigned int                   nRow = Q.nrow();
  unsigned int                   nCol = Q.ncol();
  const AT::value_type         * a = &(*(avec.begin()));
        BT::value_type         * b = &(*(bvec.begin()));
  const MT::value_type * const * m = Q.data();

  for (unsigned int c = 0; c < nCol; c++)
  {
    MT::precise_type       sum = (MT::precise_type)0;
    const MT::value_type * col = m[0] + c;

    for (unsigned int r = 0; r < nRow; r++)
    {
      sum += (MT::precise_type)((*col) * a[r]);
      col += nCol;
    }
    b[c] = (BT::value_type)sum;

#ifdef A_MATRIX_LIB_TEST
    BT::value_type _b = (BT::value_type)(std::inner_product( avec.begin(), avec.end(),
                                           Q.column_begin(c), ((MT::precise_type)0) ));
    ASSERT( fabs(b[c]-_b) <= std::numeric_limits<BT::value_type>::epsilon()*(fabs(b[c])+fabs(_b)) );
#endif // A_MATRIX_LIB_TEST
  }
  return bvec;
}


//-------------------------------------------------------------------------------------------------
/** \brief <b>A</b> := <b>A</b><sup>T</sup> */
//-------------------------------------------------------------------------------------------------
template< class MT >
MT & Transpose( MT & A )
{
  unsigned int             nRow = A.nrow();
  unsigned int             nCol = A.ncol();
  MT::value_type * const * m = A.data();

  if (nRow != nCol)
  {
    MT                       tmp( nCol, nRow );
    MT::value_type * const * t = tmp.data();

    for (unsigned int r = 0; r < nRow; r++)
    {
      for (unsigned int c = 0; c < nCol; c++)
      {
        t[c][r] = m[r][c];
      }
    }
    A.swap( tmp );
  }
  else
  {
    for (unsigned int r = 0; r < nRow; r++)
    {
      for (unsigned int c = 0; c < r; c++)
      {
        MT::value_type t = m[r][c];
                           m[r][c] = m[c][r];
                                     m[c][r] = t;
      }
    }
  }
  return A;
}


//-------------------------------------------------------------------------------------------------
/*! Function creates matrix from outer-product of input vector <b>v</b>=[F,L) :
    <b>A</b> = <b>v</b><b>v</b><sup>T</sup>. */
//-------------------------------------------------------------------------------------------------
template< class MT, class TYPE >
MT & SetOuterProductMatrix(const TYPE * F, const TYPE * L, MT & A)
{
  unsigned int N = (unsigned int)(std::distance( F, L ));

  if ( A.nrow() != N || A.ncol() != N )
    A.resize( N, N, MT::value_type(), false );

  MT::value_type * const * m = A.data();

  for(unsigned int r = 0; r < N; r++)
  {
    for(unsigned int c = r; c < N; c++)
    {
      m[r][c] = ( m[c][r] = F[r] * F[c] );
    }
  }
  return A;
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function copies specified row to the destination vector. */
//-------------------------------------------------------------------------------------------------
template< class MT, class VT >
VT & GetRow(const MT & A, unsigned int i, VT & vec)
{
  ASSERT( i < A.nrow() && (unsigned int)(vec.size()) == A.ncol() );

  alib::copy( A.row_begin(i), A.row_end(i), vec.begin(), vec.end(), VT::value_type() );
  return vec;
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function copies source vector to specified row. */
//-------------------------------------------------------------------------------------------------
template< class MT, class VT >
void SetRow(MT & A, unsigned int i, const VT & vec)
{
  ASSERT( i < A.nrow() && (unsigned int)(vec.size()) == A.ncol() );

  alib::copy( vec.begin(), vec.end(), A.row_begin(i), A.row_end(i), MT::value_type() );
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function copies specified column to the destination vector. */
//-------------------------------------------------------------------------------------------------
template< class MT, class VT >
VT & GetColumn(const MT & A, unsigned int i, VT & vec)
{
  ASSERT( i < A.ncol() && (unsigned int)(vec.size()) == A.nrow() );

  alib::copy( A.column_begin(i), A.column_end(i), vec.begin(), vec.end(), VT::value_type() );
  return vec;
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function copies source vector to specified column. */
//-------------------------------------------------------------------------------------------------
template< class MT, class VT >
void SetColumn(MT & A, unsigned int i, const VT & vec)
{
  ASSERT( i < A.ncol() && (unsigned int)(vec.size()) == A.nrow() );

  alib::copy( vec.begin(), vec.end(), A.column_begin(i), A.column_end(i), MT::value_type() );
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function copies matrix diagonal to the destination vector. */
//-------------------------------------------------------------------------------------------------
template< class MT, class VT >
VT & GetDiagonal(const MT & A, VT & vec)
{
  ASSERT( A.square() && (unsigned int)(vec.size()) == A.nrow() );

  alib::copy( A.diagonal_begin(), A.diagonal_end(), vec.begin(), vec.end(), VT::value_type() );
  return vec;
}


//-------------------------------------------------------------------------------------------------
/*! \brief Function copies source vector to the matrix diagonal. */
//-------------------------------------------------------------------------------------------------
template< class MT, class VT >
void SetDiagonal(MT & A, const VT & vec)
{
  ASSERT( A.square() && (unsigned int)(vec.size()) == A.nrow() );

  alib::copy( vec.begin(), vec.end(), A.diagonal_begin(), A.diagonal_end(), MT::value_type() );
}

} // namespace matrixlib

#endif // A_MATRIX_LIB_UTILITY_H
