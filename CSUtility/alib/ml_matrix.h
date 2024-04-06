#ifndef  A_MATRIX_LIB_MATRIX_H
#define  A_MATRIX_LIB_MATRIX_H

//=================================================================================================
// Object provides common functionality for the dense matrix with numerical entries. <BR>
// <OL> <LI> TYPE    - the type of matrix element (e.g. float, double, ...). </LI>
//      <LI> PRECISE - the type of intermediate variables, TYPE < PRECISE. </LI> </OL>
// Additional template parameter "PRECISE" serves to increase accuracy of intermediate
// computation results.
//=================================================================================================
template< class TYPE, class PRECISE >
class AMatrix
{
public:

typedef  AMatrix<TYPE,PRECISE>  this_type;            //!< type of this matrix
typedef  TYPE                   value_type;           //!< type of matrix elements
typedef  PRECISE                precise_type;         //!< type of intermediate variables
typedef  TYPE *                 iterator;             //!< type of iterator
typedef  const TYPE *           const_iterator;       //!< type of constant iterator

typedef  RandIncIterator<TYPE,TYPE&,TYPE*>              ri_iterator;         //!< auxiliary type
typedef  RandIncIterator<TYPE,const TYPE&,const TYPE*>  ri_const_iterator;   //!< auxiliary type

private:

value_type ** m_data;        //!< matrix datum
unsigned int  m_size;        //!< size of this matrix
unsigned int  m_nr;          //!< number of rows
unsigned int  m_nc;          //!< number of columns
bool          m_bExt;        //!< TRUE if matrix contents is placed in external storage

public:

//-------------------------------------------------------------------------------------------------
/** \brief Function returns total size in bytes occupied by content of this matrix. */
//-------------------------------------------------------------------------------------------------
unsigned int byte_size() const
{
  return (m_nr*sizeof(void*) + m_size*sizeof(value_type));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clear this object and deallocates occupied memory. */
//-------------------------------------------------------------------------------------------------
void clear()
{
  if (!m_bExt && (m_data != 0))
    free( (void*)m_data );
  m_data = 0;
  m_nr = m_nc = m_size = 0;
  m_bExt = false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function resizes matrix and places it either in external storage or in global memory.

  If pointers of external storage [F,L) are non-zero and there is enough space between them,
  then matrix will be placed without memory reallocation, otherwise malloc() will be called.

  \param  nRow   number of rows.
  \param  nCol   number of columns.
  \param  v      fill value.
  \param  bInit  flag tell whether to fill matrix with value 'v' or remain intact.
  \param  F      begin iterator of external storage or 0.
  \param  L      end iterator of external storage or 0.
  \return        (F+(size of content)) if matrix is placed in external storage, otherwise 0. */
//-------------------------------------------------------------------------------------------------
void * resize(unsigned int nRow, unsigned int nCol,
              value_type v = value_type(), bool bInit = true, void * F = 0, void * L = 0)
{
  ASSERT( sizeof(char) == 1 );

  void * newF = 0;
  if (m_nr != nRow || m_nc != nCol)
  {
    clear();
    m_nr = nRow;
    m_nc = nCol;
    m_size = nRow * nCol;
    if (m_size > 0)
    {
      m_bExt = (F != 0 && L != 0 && (((char*)F)+byte_size()) <= ((char*)L));
      m_data = m_bExt ? ((value_type**)F) : ((value_type**)malloc( byte_size() ));
      ASSERT( m_data != 0 );

      value_type * head = (value_type*)(m_data + nRow);
      value_type * last = head + m_size;

      // Initialize array of row pointers.
      for(unsigned int r = 0; r < nRow; r++)
      {
        m_data[r] = head;
        head += nCol;
      }
      ASSERT( head == last );
      newF = m_bExt ? ((void*)last) : 0;
    }
  }

  if (bInit)
    std::fill( begin(), end(), v );

  ASSERT( newF == 0 || newF <= L );
  return newF;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function swaps contents of this matrix and specified one. */
//-------------------------------------------------------------------------------------------------
void swap( this_type & A )
{
  std::swap( m_nr  , A.m_nr   );
  std::swap( m_nc  , A.m_nc   );
  std::swap( m_data, A.m_data );
  std::swap( m_size, A.m_size );
  std::swap( m_bExt, A.m_bExt );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function detaches content from the matrix <b>A</b> and attaches it to this one. */
//-------------------------------------------------------------------------------------------------
void attach(this_type & A)
{
  clear();
  m_nr   = A.m_nr;     A.m_nr   = 0;
  m_nc   = A.m_nc;     A.m_nc   = 0;
  m_data = A.m_data;   A.m_data = 0;
  m_size = A.m_size;   A.m_size = 0;
  m_bExt = A.m_bExt;   A.m_bExt = false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns number of rows. */
//-------------------------------------------------------------------------------------------------
unsigned int nrow() const
{ 
  return m_nr;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns number of columns. */
//-------------------------------------------------------------------------------------------------
unsigned int ncol() const
{
  return m_nc;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns total number of elements of this matrix. */
//-------------------------------------------------------------------------------------------------
unsigned int size() const
{
  return m_size;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function checks whether matrix is square. */
//-------------------------------------------------------------------------------------------------
bool square() const
{
  return (m_nr == m_nc);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function checks whether matrix is empty. */
//-------------------------------------------------------------------------------------------------
bool empty() const
{
  return (m_size == 0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function checks that specified pointer points within the matrix content. */
//-------------------------------------------------------------------------------------------------
bool valid_ptr(const value_type * p) const
{
  return ((m_data[0] <= p) && (p < (m_data[0]+m_size)) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function provides C-style access to this matrix. */
//-------------------------------------------------------------------------------------------------
const value_type * const * data() const
{
  return m_data;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function provides C-style access to this matrix. */
//-------------------------------------------------------------------------------------------------
value_type ** data()
{
  return m_data;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function fills array A[] of pointers so that (*this)[0][0] <--> A[1][1] and so on. */
//-------------------------------------------------------------------------------------------------
void data11(value_type ** A, int length)
{
  length;
  ASSERT( length == (int)(m_nr+1) );

  A[0] = 0;
  for(unsigned int r = 0; r < m_nr; r++)
  {
    A[r+1] = (m_data[r])-1;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "constant begin" iterator of this matrix. */
//-------------------------------------------------------------------------------------------------
const_iterator begin() const
{
  return m_data[0];
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "begin" iterator of this matrix. */
//-------------------------------------------------------------------------------------------------
iterator begin()
{
  return m_data[0];
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "constant end" iterator of this matrix. */
//-------------------------------------------------------------------------------------------------
const_iterator end() const
{
  return (m_data[0]+m_size);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "end" iterator of this matrix. */
//-------------------------------------------------------------------------------------------------
iterator end()
{
  return (m_data[0]+m_size);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function checks whether two matrices have the same dimensions. */
//-------------------------------------------------------------------------------------------------
bool has_equal_dim(const this_type & A) const
{
  return ((m_nr == A.m_nr) && (m_nc == A.m_nc));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function copies the source matrix to this one. */
//-------------------------------------------------------------------------------------------------
this_type & operator=( const this_type & A )
{
  if (this != &A)
  {
    if (!has_equal_dim( A ))
      resize( A.m_nr, A.m_nc, value_type(), false );
    std::copy( A.begin(), A.end(), begin() );
  }
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns specified element. */
//-------------------------------------------------------------------------------------------------
value_type & operator()(unsigned int r, unsigned int c)
{ 
  ASSERT( (r < m_nr) && (c < m_nc) );
  return m_data[r][c];
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns specified element. */
//-------------------------------------------------------------------------------------------------
value_type operator()(unsigned int r, unsigned int c) const
{ 
  ASSERT( (r < m_nr) && (c < m_nc) );
  return m_data[r][c];
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "begin" pointer of specified row. */
//-------------------------------------------------------------------------------------------------
const_iterator row_begin(unsigned int r) const
{ 
  ASSERT( r < m_nr );
  return m_data[r];
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "begin" pointer of specified row. */
//-------------------------------------------------------------------------------------------------
iterator row_begin(unsigned int r)
{ 
  ASSERT( r < m_nr );
  return m_data[r];
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "end" pointer of specified row. */
//-------------------------------------------------------------------------------------------------
const_iterator row_end(unsigned int r) const
{ 
  ASSERT( r < m_nr );
  return (m_data[r] + m_nc);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "end" pointer of specified row. */
//-------------------------------------------------------------------------------------------------
iterator row_end(unsigned int r)
{ 
  ASSERT( r < m_nr );
  return (m_data[r] + m_nc);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "begin" iterator of diagonal. */
//-------------------------------------------------------------------------------------------------
ri_iterator diagonal_begin()
{
  value_type * pLast = m_data[0]+(m_nc+m_size);
  return ri_iterator( m_data[0], m_nc+1, m_data[0], pLast );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "begin" iterator of diagonal. */
//-------------------------------------------------------------------------------------------------
ri_const_iterator diagonal_begin() const
{
  value_type * pLast = m_data[0]+(m_nc+m_size);
  return ri_const_iterator( m_data[0], m_nc+1, m_data[0], pLast );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "end" iterator of diagonal. */
//-------------------------------------------------------------------------------------------------
ri_iterator diagonal_end()
{
  value_type * pLast = m_data[0]+(m_nc+m_size);
  return ri_iterator( pLast, m_nc+1, m_data[0], pLast );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "end" iterator of diagonal. */
//-------------------------------------------------------------------------------------------------
ri_const_iterator diagonal_end() const
{
  value_type * pLast = m_data[0]+(m_nc+m_size);
  return ri_const_iterator( pLast, m_nc+1, m_data[0], pLast );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "begin" iterator of specified column. */
//-------------------------------------------------------------------------------------------------
ri_iterator column_begin(unsigned int c)
{
  ASSERT( c < m_nc );
  return ri_iterator( m_data[0]+c, m_nc, m_data[0]+c, m_data[0]+(c+m_size) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "begin" iterator of specified column. */
//-------------------------------------------------------------------------------------------------
ri_const_iterator column_begin(unsigned int c) const
{
  ASSERT( c < m_nc );
  return ri_const_iterator( m_data[0]+c, m_nc, m_data[0]+c, m_data[0]+(c+m_size) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "end" iterator of specified column. */
//-------------------------------------------------------------------------------------------------
ri_iterator column_end(unsigned int c)
{
  ASSERT( c < m_nc );
  return ri_iterator( m_data[0]+(c+m_size), m_nc, m_data[0]+c, m_data[0]+(c+m_size) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "end" iterator of specified column. */
//-------------------------------------------------------------------------------------------------
ri_const_iterator column_end(unsigned int c) const
{
  ASSERT( c < m_nc );
  return ri_const_iterator( m_data[0]+(c+m_size), m_nc, m_data[0]+c, m_data[0]+(c+m_size) );
}


//-------------------------------------------------------------------------------------------------
/** \brief (*this) += A */
//-------------------------------------------------------------------------------------------------
this_type & operator+=(const this_type & A)
{
  ASSERT( has_equal_dim(A) );

  const_iterator a = A.begin();
  iterator       i = begin();
  iterator       e = end();

  while(i != e)
  {
    (*i) += (*a);
    ++a;
    ++i;
  }
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief (*this) -= A */
//-------------------------------------------------------------------------------------------------
this_type & operator-=(const this_type & A)
{
  ASSERT( has_equal_dim(A) );

  const_iterator a = A.begin();
  iterator       i = begin();
  iterator       e = end();

  while(i != e)
  {
    (*i) -= (*a);
    ++a;
    ++i;
  }
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief (*this) *= m */
//-------------------------------------------------------------------------------------------------
this_type & operator*=(value_type m)
{
  for(iterator i = begin(), e = end(); i != e; ++i)
  {
    (*i) *= m;
  }
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief (*this) /= m. */
//-------------------------------------------------------------------------------------------------
this_type & operator/=( value_type m )
{
  return ((*this) *= alib::Reciprocal( m ));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function inverts the sign of matrix elements. */
//-------------------------------------------------------------------------------------------------
this_type & negate()
{
  for (iterator i = begin(), e = end(); i != e; ++i)
  {
    (*i) = -(*i);
  }
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief Contructor. */
//-------------------------------------------------------------------------------------------------
AMatrix()
: m_data(0), m_size(0), m_nr(0), m_nc(0), m_bExt(false)
{
  ASSERT( A_MATRIX_LIB_IS_SUITABLE_NUMERICAL_TYPE( value_type, precise_type ) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Contructor. */
//-------------------------------------------------------------------------------------------------
AMatrix(int nRow, int nCol, value_type v = value_type(), bool bInit = true)
: m_data(0), m_size(0), m_nr(0), m_nc(0), m_bExt(false)
{
  ASSERT( A_MATRIX_LIB_IS_SUITABLE_NUMERICAL_TYPE( value_type, precise_type ) );
  resize( nRow, nCol, v, bInit );
}


//-------------------------------------------------------------------------------------------------
/** \brief Contructor. */
//-------------------------------------------------------------------------------------------------
AMatrix(const this_type & A)
: m_data(0), m_size(0), m_nr(0), m_nc(0), m_bExt(false)
{
  ASSERT( A_MATRIX_LIB_IS_SUITABLE_NUMERICAL_TYPE( value_type, precise_type ) );
  (*this) = A;
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
~AMatrix()
{
  clear();
}

};

typedef  AMatrix<float,double>             fMat;
typedef  AMatrix<double,double>            dMat;
typedef  AMatrix<long double,long double>  lMat;

#endif // A_MATRIX_LIB_MATRIX_H
