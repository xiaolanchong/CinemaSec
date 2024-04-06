///////////////////////////////////////////////////////////////////////////////////////////////////
// triangle_storage.h
// ---------------------
// begin     : 1998
// modified  : 9 Aug 2005
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaah@mail.ru, aaahaaah@hotmail.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \class TriangleStorage.
/// \brief Triangle storage looks like symmetric matrix, but has twice as less the number of entries.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class TYPE, class PRECISE, bool bMALLOC >
class TriangleStorage
{
public:
  typedef  TYPE              value_type;      // type of element
  typedef  PRECISE           precise_type;    // type of intermediate result
  typedef  unsigned __int32  size_type;
  typedef  TYPE &            reference;
  typedef  const TYPE &      const_reference;
  typedef  TYPE *            iterator;
  typedef  const TYPE *      const_iterator;
  typedef  TYPE *            pointer;
  typedef  const TYPE *      const_pointer;

protected:
  value_type *     m_head; //!< pointer to the first element (content of this object)
  value_type *     m_tail; //!< pointer to the next after last element (STL convention)
  unsigned __int32 m_dim;  //!< width or height
  unsigned __int32 m_size; //!< size = (m_dim*m_dim - m_dim)/2 + m_dim

public:
//-------------------------------------------------------------------------------------------------
/** \brief Function returns plane "begin" iterator, i.e. object is considered as 1D-array. */
//-------------------------------------------------------------------------------------------------
iterator begin()
{
  return m_head;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns plane constant "begin" iterator. */
//-------------------------------------------------------------------------------------------------
const_iterator begin() const
{
  return m_head;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns plane "end" iterator. */
//-------------------------------------------------------------------------------------------------
iterator end()
{
  return m_tail;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns plane constant "end" iterator. */
//-------------------------------------------------------------------------------------------------
const_iterator end() const
{
  return m_tail;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns nonzero for an empty object. */
//-------------------------------------------------------------------------------------------------
bool empty() const
{
  return (m_size == 0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns storage's dimension (width or height). */
//-------------------------------------------------------------------------------------------------
int dim() const
{
  return m_dim;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns actual number of entries, that is approximately twice
           as small then the number of logical entries. */
//-------------------------------------------------------------------------------------------------
int size() const
{ 
  return m_size;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object and releases allocated memory. */
//-------------------------------------------------------------------------------------------------
void clear()
{ 
  if (bMALLOC)
    free( m_head );
  else
    delete [] m_head;
  m_dim = (m_size = 0);
  m_head = (m_tail = 0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function resizes this object.

  \param  dim    new width and height.
  \param  vfill  fill value.
  \param  bInit  if nonzero, then the object will be filled up by specified value. */
//-------------------------------------------------------------------------------------------------
void resize( int dim, value_type vfill = value_type(), bool bInit = true )
{
  if ((int)m_dim != dim)
  {
    clear();
    if (dim > 0)
    {
      m_dim = dim;
      m_size = (dim*dim - dim)/2 + dim;
      m_head = (bMALLOC ? reinterpret_cast<value_type*>( malloc( m_size*sizeof(value_type) ) )
                        : (new value_type [m_size]));
      m_tail = m_head + m_size;
      if (m_head == 0)
        clear();
      ASSERT( m_head != 0 );
    }
  }

  if (bInit)
    std::fill( begin(), end(), vfill );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns reference to the entry at positions (i,j) and (j,i). */
//-------------------------------------------------------------------------------------------------
reference operator()( int i, int j )
{
  ASSERT( ALIB_IS_RANGE( i, 0, (int)m_dim ) && ALIB_IS_RANGE( j, 0, (int)m_dim ) );

  return ((i <= j) ? m_head[(j*(j+1))/2 + i] : m_head[(i*(i+1))/2 + j]);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns constant reference to the entry at positions (i,j) and (j,i). */
//-------------------------------------------------------------------------------------------------
const_reference operator()( int i, int j ) const
{
  ASSERT( ALIB_IS_RANGE( i, 0, (int)m_dim ) && ALIB_IS_RANGE( j, 0, (int)m_dim ) );

  return ((i <= j) ? m_head[(j*(j+1))/2 + i] : m_head[(i*(i+1))/2 + j]);
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
TriangleStorage() : m_head(0)
{
  clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
~TriangleStorage()
{
  clear();
}

};

