#ifndef  ALIB_RANDOM_INCREMENT_ITERATOR_H
#define  ALIB_RANDOM_INCREMENT_ITERATOR_H

//=================================================================================================
/** Class provides implementation of iterator with arbitrary increment. <br>

  Template parameters have the following meanings:
  <ol> <li> T   - type of elements of any sequence where iterator points in. </li>
       <li> REF - type reference to an element. </li>
       <li> PTR - type of pointer to an element. </li> </ol> */
//=================================================================================================

template< class T, class REF = T&, class PTR = T* >
class RandIncIterator
{
 private:
  /** \brief Private constructor is used to create temporal variable only. */
  RandIncIterator() {}

 protected:
  PTR        m_ptr;          //!< pointer to an element of 2D-array
  ptrdiff_t  m_inc;          //!< increment of the pointer

  #ifdef _DEBUG
  PTR        m_head;         //!< bottom limit of pointer
  PTR        m_last;         //!< top limit of pointer
  #endif // _DEBUG

 public:
  typedef  RandIncIterator<T,REF,PTR>  this_type;     //!< type of this object


  RandIncIterator( const this_type & x ) : m_ptr( x.m_ptr ), m_inc( x.m_inc )
  {
    ASSERT( m_inc > 0 );
    #ifdef _DEBUG
    m_head = x.m_head;
    m_last = x.m_last;
    #endif
  }


  RandIncIterator( PTR pointer, ptrdiff_t increment, PTR pHead, PTR pLast )
  : m_ptr( pointer ), m_inc( increment )
  {
    ASSERT( m_inc > 0 );
    pHead;pLast;
    #ifdef _DEBUG
    m_head = pHead;
    m_last = pLast;
    #endif
  }


  this_type & set( PTR pointer, ptrdiff_t increment, PTR pHead, PTR pLast )
  {
    ASSERT( m_inc > 0 );
    pHead;pLast;
    #ifdef _DEBUG
    m_head = pHead;
    m_last = pLast;
    #endif

    m_ptr = pointer;
    m_inc = increment;
    return (*this);
  }


  PTR ptr() const
  {
    ASSERT( (m_head <= m_ptr) && (m_ptr < m_last) );

    return m_ptr;
  }


  REF operator*() const
  {
    ASSERT( (m_head <= m_ptr) && (m_ptr < m_last) );

    return (*m_ptr);
  }

/*
  PTR operator->() const
  {
    ASSERT( (m_head <= m_ptr) && (m_ptr < m_last) );

    return (m_ptr);
  }
*/

  REF operator[]( ptrdiff_t n ) const
  {
    PTR p = m_ptr + n*m_inc;
    ASSERT( (m_head <= p) && (p < m_last) );
    return (*p);
  }


  this_type & operator++()
  {
    m_ptr += m_inc;
    return (*this);
  }


  this_type operator++(int)
  {
    this_type t;
    t.m_ptr = m_ptr;
    t.m_inc = m_inc;
    m_ptr += m_inc;

    #ifdef _DEBUG
    t.m_head = m_head;
    t.m_last = m_last;
    #endif

    return t;
  }


  this_type & operator--()
  {
    m_ptr -= m_inc;
    return (*this);
  }


  this_type operator--(int)
  {
    this_type t;
    t.m_ptr = m_ptr;
    t.m_inc = m_inc;
    m_ptr -= m_inc;

    #ifdef _DEBUG
    t.m_head = m_head;
    t.m_last = m_last;
    #endif

    return t;
  }


  this_type & operator+=( ptrdiff_t n )
  {
    m_ptr += n*m_inc;
    return (*this);
  }


  this_type operator+( ptrdiff_t n ) const
  {
    this_type t;
    t.m_ptr = m_ptr + n*m_inc;
    t.m_inc = m_inc;

    #ifdef _DEBUG
    t.m_head = m_head;
    t.m_last = m_last;
    #endif

    return t;
  }


  this_type & operator-=( ptrdiff_t n )
  {
    m_ptr -= n*m_inc;
    return (*this);
  }


  this_type operator-( ptrdiff_t n ) const
  {
    this_type t;
    t.m_ptr = m_ptr - n*m_inc;
    t.m_inc = m_inc;

    #ifdef _DEBUG
    t.m_head = m_head;
    t.m_last = m_last;
    #endif

    return t;
  }


  this_type & operator=( const this_type & x )
  {
    #ifdef _DEBUG
    m_head = x.m_head;
    m_last = x.m_last;
    #endif

    m_ptr = x.m_ptr;
    m_inc = x.m_inc;
    return (*this);
  }

 
  bool operator==( const this_type & x ) const
  {
    ASSERT( (m_head == x.m_head) && (m_last == x.m_last) && (m_inc == x.m_inc) );

    return (m_ptr == x.m_ptr);
  }


  bool operator!=( const this_type & x ) const
  {
    ASSERT( (m_head == x.m_head) && (m_last == x.m_last) && (m_inc == x.m_inc) );

    return (m_ptr != x.m_ptr);
  }


  bool operator<( const this_type & x ) const
  {
    ASSERT( (m_head == x.m_head) && (m_last == x.m_last) && (m_inc == x.m_inc) );

    return (m_ptr < x.m_ptr);
  }


  bool operator>( const this_type & x ) const
  {
    ASSERT( (m_head == x.m_head) && (m_last == x.m_last) && (m_inc == x.m_inc) );

    return (m_ptr > x.m_ptr);
  }


  bool operator<=( const this_type & x ) const
  {
    ASSERT( (m_head == x.m_head) && (m_last == x.m_last) && (m_inc == x.m_inc) );

    return (m_ptr <= x.m_ptr);
  }


  bool operator>=( const this_type & x ) const
  {
    ASSERT( (m_head == x.m_head) && (m_last == x.m_last) && (m_inc == x.m_inc) );

    return (m_ptr >= x.m_ptr);
  }


  int distance( const this_type & x ) const
  {
    ASSERT( (m_head == x.m_head) && (m_last == x.m_last) && (m_inc == x.m_inc) );

    ptrdiff_t  dist = (m_ptr >= x.m_ptr) ? (m_ptr - x.m_ptr) : (x.m_ptr - m_ptr);
    return (dist/m_inc);
  }
};

#endif // ALIB_RANDOM_INCREMENT_ITERATOR_H
