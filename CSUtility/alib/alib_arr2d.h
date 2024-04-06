#ifndef  ALIB_ARR_2D_H
#define  ALIB_ARR_2D_H

#define  ALIB_ARR2D_INSIDE(x,y)  ((0 <= (x)) && ((x) < (int)m_W) && (0 <= (y)) && ((y) < (int)m_H))

//=================================================================================================
// 2D array of items of any type TYPE.
// <b>X</b>-axis is row-aligned, <b>Y</b>-axis is column-aligned.
//=================================================================================================
template< class TYPE, class PRECISE = TYPE >
class Arr2D
{
 public:
  typedef  TYPE                 value_type;      // type of element
  typedef  PRECISE              precise_type;    // type of intermediate result
  typedef  Arr2D<TYPE,PRECISE>  this_type;       // type of this class
  typedef  unsigned int         size_type;
  typedef  TYPE &               reference;
  typedef  const TYPE &         const_reference;
  typedef  TYPE *               iterator;
  typedef  const TYPE *         const_iterator;
  typedef  TYPE *               pointer;
  typedef  const TYPE *         const_pointer;

  typedef  TYPE *                                         x_iterator;          // row iterator
  typedef  const TYPE *                                   x_const_iterator;    // row iterator
  typedef  RandIncIterator<TYPE,TYPE&,TYPE*>              y_iterator;          // column iterator
  typedef  RandIncIterator<TYPE,const TYPE&,const TYPE*>  y_const_iterator;    // column iterator

 protected:
  value_type *  m_head;      //!< pointer to the first element (content of this object)
  value_type *  m_tail;      //!< pointer to the next after last element (STL convention)
  unsigned int  m_W;         //!< width
  unsigned int  m_H;         //!< height
  unsigned int  m_size;      //!< size = width*height
  unsigned int  m_capacity;  //!< maximal object's size that may be set without reallocation
  bool          m_bExternal; //!< if true then uses external memory buffer, otherwise allocates from the heap

 public:

//-------------------------------------------------------------------------------------------------
/** \brief Function returns plane "begin" iterator, i.e. object is considered as 1D-array. */
//-------------------------------------------------------------------------------------------------
iterator begin()
{
  return m_head;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns plane "begin" iterator, i.e. object is considered as 1D-array. */
//-------------------------------------------------------------------------------------------------
const_iterator begin() const
{
  return m_head;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns plane "end" iterator, i.e. object is considered as 1D-array. */
//-------------------------------------------------------------------------------------------------
iterator end()
{
  return m_tail;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns plane "end" iterator, i.e. object is considered as 1D-array. */
//-------------------------------------------------------------------------------------------------
const_iterator end() const
{
  return m_tail;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns true for an empty object. */
//-------------------------------------------------------------------------------------------------
bool empty() const
{
  return (m_size == 0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns true in the case of equal horizontal and vertical dimensions. */
//-------------------------------------------------------------------------------------------------
bool square() const
{
  return (m_W == m_H);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns horizontal dimension. */
//-------------------------------------------------------------------------------------------------
int width() const
{
  return m_W;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns vertical dimension. */
//-------------------------------------------------------------------------------------------------
int height() const
{
  return m_H;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns total number of entries of this 2D-array (width*height). */
//-------------------------------------------------------------------------------------------------
int size() const
{ 
  return m_size;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns bounding rectangle of this 2D-array. */
//-------------------------------------------------------------------------------------------------
ARect rect() const
{
  return ARect( 0, 0, m_W, m_H );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function checks whether specified point lies inside 2D-array.

  \param  x  x-coordinate of query point.
  \param  y  y-coordinate of query point.
  \return    TRUE if point lies inside array. */
//-------------------------------------------------------------------------------------------------
bool inside( int x, int y ) const
{
  return ALIB_ARR2D_INSIDE( x, y );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function checks whether specified point lies inside 2D-array.

  \param  p  query point.
  \return    TRUE if point lies inside array. */
//-------------------------------------------------------------------------------------------------
bool inside( Vec2i p ) const
{ 
  return ALIB_ARR2D_INSIDE( p.x, p.y );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object and releases allocated memory. */
//-------------------------------------------------------------------------------------------------
void clear()
{ 
  m_W = (m_H = (m_size = (m_capacity = 0)));
  if (!m_bExternal)
  {
    delete [] m_head;
  }
  m_head = (m_tail = 0);
  m_bExternal = false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Synonym (here) of clear() function. */
//-------------------------------------------------------------------------------------------------
void release()
{
  clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function resizes this object.

  \param  W      new width.
  \param  H      new height.
  \param  v      fill value.
  \param  bInit  if TRUE then the object will be filled by specified value. */
//-------------------------------------------------------------------------------------------------
void resize( unsigned int W, unsigned int H, value_type v = value_type(), bool bInit = true )
{
  if ((m_W != W) || (m_H != H))
  {
    if ((W == 0) || (H == 0))
    {
      clear();
    }
    else
    {
      if (W*H <= m_capacity)
      {
        m_size = (m_W = W)*(m_H = H);
      }
      else
      {
        clear();
        m_size = (m_capacity = (m_W = W)*(m_H = H));
        m_head = new value_type [ m_size ];
      }
      m_tail = m_head + m_size;

      ASSERT( m_head != 0 );
      if (m_head == 0)
        clear();
    }
  }

  if (bInit)
    std::fill( begin(), end(), v );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function resizes this object according to dimensions of specified one.

  \param  q      object that must have methods 'width()' and 'height()'.
  \param  bInit  if TRUE then the object will be filled by default value. */
//-------------------------------------------------------------------------------------------------
template< class Q >
void resize2( Q & q, bool bInit = true )
{
  resize( q.width(), q.height(), value_type(), bInit );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function attaches external buffer to this object instead of allocating
           memory form the heap.

  This object DOES NOT deallocate the attached external buffer.

  \param  width    the width of external buffer.
  \param  height   the height of external buffer.
  \param  content  pointer to the external buffer. */
//-------------------------------------------------------------------------------------------------
void attach_external( int width, int height, value_type * content )
{
  clear();
  m_capacity = 0;
  m_size = (m_W = width)*(m_H = height);
  m_head = content;
  m_tail = m_head + m_size;
  m_bExternal = true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function is similar to <i>attach_external(..)</i>.

  This object DOES NOT deallocate the attached external buffer.

  \param  width    the width of external buffer.
  \param  height   the height of external buffer.
  \param  content  pointer to the external buffer. */
//-------------------------------------------------------------------------------------------------
void wrap( int width, int height, value_type * content )
{
  clear();
  m_capacity = 0;
  m_size = (m_W = width)*(m_H = height);
  m_head = content;
  m_tail = m_head + m_size;
  m_bExternal = true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function copies source object into this one.

  \param  x  source object.
  \return    this object. */
//-------------------------------------------------------------------------------------------------
this_type & operator=( const this_type & x )
{
  if (&x != this)
  {
    resize( x.m_W, x.m_H, value_type(), false );
    std::copy( x.begin(), x.end(), begin() );
  }
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function swaps contents between this object and specified one.

  \param  x  source object.
  \return    this object. */
//-------------------------------------------------------------------------------------------------
this_type & swap( this_type & x )
{
  std::swap( m_head,      x.m_head );
  std::swap( m_tail,      x.m_tail );
  std::swap( m_W,         x.m_W );
  std::swap( m_H,         x.m_H );
  std::swap( m_size,      x.m_size );
  std::swap( m_capacity,  x.m_capacity );
  std::swap( m_bExternal, x.m_bExternal );
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns a reference to the element of this object at position (x,y).

  \param  x  x-coordinate of point of interest.
  \param  y  y-coordinate of point of interest.
  \return    reference to the element. */
//-------------------------------------------------------------------------------------------------
reference operator()( int x, int y )
{
  ASSERT( ALIB_ARR2D_INSIDE( x, y ) );

  return m_head[ x + y*m_W ];
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns a reference to the element of this object at position (x,y).

  \param  x  x-coordinate of point of interest.
  \param  y  y-coordinate of point of interest.
  \return    reference to the element. */
//-------------------------------------------------------------------------------------------------
const_reference operator()( int x, int y ) const
{
  ASSERT( ALIB_ARR2D_INSIDE( x, y ) );

  return m_head[ x + y*m_W ];
}


//-------------------------------------------------------------------------------------------------
/** \brief Functions return a pointer to the element of this object at position (x,y). */
//-------------------------------------------------------------------------------------------------
pointer ptr( int x, int y )
{
  ASSERT( ALIB_ARR2D_INSIDE( x, y ) );
  return (m_head + (x + y*m_W));
}

const_pointer ptr( int x, int y ) const
{
  ASSERT( ALIB_ARR2D_INSIDE( x, y ) );
  return (m_head + (x + y*m_W));
}

pointer ptr( const Vec2i & p )
{
  ASSERT( ALIB_ARR2D_INSIDE( p.x, p.y ) );
  return (m_head + (p.x + p.y*m_W));
}

const_pointer ptr( const Vec2i & p ) const
{
  ASSERT( ALIB_ARR2D_INSIDE( p.x, p.y ) );
  return (m_head + (p.x + p.y*m_W));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns a reference to the element of this object at position p.

  \param  p  point of interest.
  \return    reference to the element. */
//-------------------------------------------------------------------------------------------------
reference operator[]( const Vec2i & p )
{
  ASSERT( ALIB_ARR2D_INSIDE( p.x, p.y ) );

  return m_head[ p.x + p.y*m_W ];
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns a reference to the element of this object at position p.

  \param  p  point of interest.
  \return    reference to the element. */
//-------------------------------------------------------------------------------------------------
const_reference operator[]( const Vec2i & p ) const
{
  ASSERT( ALIB_ARR2D_INSIDE( p.x, p.y ) );

  return m_head[ p.x + p.y*m_W ];
}


//-------------------------------------------------------------------------------------------------
/** \brief Function will set new value to the element of this object if point (x,y)
           lies inside 2D-array.

  \param  x      x-coordinate of point of interest.
  \param  y      y-coordinate of point of interest.
  \param  value  value to be set. */
//-------------------------------------------------------------------------------------------------
void set_if_in( int x, int y, const_reference value )
{
  if (ALIB_ARR2D_INSIDE( x, y ))
  {
    m_head[ x + y*m_W ] = value;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function will set new value to the element of this object if point <b>p</b>
           lies inside 2D-array.

  \param  p      point of interest.
  \param  value  value to be set. */
//-------------------------------------------------------------------------------------------------
void set_if_in( const Vec2i & p, const_reference value )
{
  if (ALIB_ARR2D_INSIDE( p.x, p.y ))
  {
    m_head[ p.x + p.y*m_W ] = value;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function will return constant iterator of specified element of this object if
           point (x,y) lies inside 2D-array.

  \param  x  x-coordinate of point of interest.
  \param  y  y-coordinate of point of interest.
  \return    iterator of internal point or 0 for external one. */
//-------------------------------------------------------------------------------------------------
const_iterator get_if_in( int x, int y ) const
{
  return ((ALIB_ARR2D_INSIDE( x, y )) ? (m_head + (x + y*m_W)) : 0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function will return iterator of specified element of this object if
           point (x,y) lies inside 2D-array.

  \param  x  x-coordinate of point of interest.
  \param  y  y-coordinate of point of interest.
  \return    iterator of internal point or 0 for external one. */
//-------------------------------------------------------------------------------------------------
iterator get_if_in( int x, int y )
{
  return ((ALIB_ARR2D_INSIDE( x, y )) ? (m_head + (x + y*m_W)) : 0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function will return constant iterator of specified element of this object if
           point <b>p</b> lies inside 2D-array.

  \param  p  point of interest.
  \return    iterator of internal point or 0 for external one. */
//-------------------------------------------------------------------------------------------------
const_iterator get_if_in( const Vec2i & p ) const
{
  return ((ALIB_ARR2D_INSIDE( p.x, p.y )) ? (m_head + (p.x + p.y*m_W)) : 0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function will return iterator of specified element of this object if
           point <b>p</b> lies inside 2D-array.

  \param  p  point of interest.
  \return    iterator of internal point or 0 for external one. */
//-------------------------------------------------------------------------------------------------
iterator get_if_in( const Vec2i & p )
{
  return ((ALIB_ARR2D_INSIDE( p.x, p.y )) ? (m_head + (p.x + p.y*m_W)) : 0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns plane offset of the element of this object at point (x,y).

  \param  x  x-coordinate of point of interest.
  \param  y  y-coordinate of point of interest.
  \return    plane offset. */
//-------------------------------------------------------------------------------------------------
int offset( int x, int y ) const 
{
  ASSERT( ALIB_ARR2D_INSIDE( x, y ) );

  return (x + y*m_W);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns plane offset of the element of this object at point <b>p</b>.

  \param  p  point of interest.
  \return    plane offset. */
//-------------------------------------------------------------------------------------------------
int offset( const Vec2i & p ) const
{
  ASSERT( ALIB_ARR2D_INSIDE( p.x, p.y ) );

  return (p.x + p.y*m_W);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns offset of specified entry, if point (x,y) lies inside 2D-array.

  \param  x  abscissa of a point of interest.
  \param  y  ordinate of a point of interest.
  \return    offset of internal point or -1 for external one. */
//-------------------------------------------------------------------------------------------------
int offset_if_in( int x, int y ) const
{
  return ((ALIB_ARR2D_INSIDE( x, y )) ? (x + y*m_W) : -1);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns offset of specified entry, if point <b>p</b> lies inside 2D-array.

  \param  p  a point of interest.
  \return    offset of internal point or -1 for external one. */
//-------------------------------------------------------------------------------------------------
int offset_if_in( const Vec2i & p ) const
{
  return ((ALIB_ARR2D_INSIDE( p.x, p.y )) ? (p.x + p.y*m_W) : -1);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns a reference to the element of this object at plane index position n.

  \param  n  plane index (offset).
  \return    reference to the element. */
//-------------------------------------------------------------------------------------------------
reference operator[](unsigned int n)
{
  ASSERT( n < m_size );

  return m_head[n];
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns a reference to the element of this object at plane index position n.

  \param  n  plane index (offset).
  \return    reference to the element. */
//-------------------------------------------------------------------------------------------------
const_reference operator[](unsigned int n) const
{
  ASSERT( n < m_size );

  return m_head[n];
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns bilinearly-interpolated value at point (fx,fy).

  Elements of type TYPE must be supplied by:
  <ol><li> TYPE operator*(TYPE&, float); </li>
      <li> TYPE operator+(TYPE&, TYPE&); </li></ol>

  \param  fx  x-coordinate of point of interest.
  \param  fy  y-coordinate of point of interest.
  \return     interpolated value or TYPE(), if a point has occured outside of 2D-array. */
//-------------------------------------------------------------------------------------------------
value_type bilin_at( float fx, float fy ) const
{
  int x = (int)(fx + 1.0) - 1;    // will be rounded to -1 if -1 <= fx < 0
  int y = (int)(fy + 1.0) - 1;    // will be rounded to -1 if -1 <= fy < 0
  int x1 = 1;
  int y1 = m_W;
  int W1 = (int)m_W-1;
  int H1 = (int)m_H-1;

  // Border pixel requires additional processing.
  if ((x < 0) || (y < 0) || (x >= W1) || (y >= H1))
  {
    if ((m_size == 0) || (x < -1) || (y < -1) || (x >= (int)m_W) || (y >= (int)m_H))
      return value_type();

    if (x == -1) x1 = (x = 0); else if (x == W1) x1 = 0;
    if (y == -1) y1 = (y = 0); else if (y == H1) y1 = 0;
  }

  float dx = (float)(fx - x);          // p3 p2
  float dy = (float)(fy - y);          // p0 p1
  float C2 = dx * dy;
  float C1 = dx - C2;
  float C3 = dy - C2;
  float C0 = 1.0f - dy - C1;
  int   i = x + y*m_W;

  return (value_type)(m_head[i]*C0 + m_head[i+x1]*C1 + m_head[i+x1+y1]*C2 + m_head[i+y1]*C3);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns bilinearly-interpolated value at point <b>p</b>.

  Elements of type TYPE must be supplied by:
  <ol><li> TYPE operator*(TYPE&, float); </li>
      <li> TYPE operator+(TYPE&, TYPE&); </li></ol>

  \param  p  point of interest.
  \return    interpolated value or TYPE() if point occured outside of 2D-array. */
//-------------------------------------------------------------------------------------------------
value_type bilin_at( Vec2f p ) const
{
  return bilin_at( p.x, p.y );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns value of element at position p, adjusting position
           to be inside 2D-array.

  \param  p  point of interest (desired position).
  \return    value at adjusted (bounded) position. */
//-------------------------------------------------------------------------------------------------
const_reference bounded_at( Vec2i p ) const
{
  ASSERT( m_size > 0 );

  return m_head[ ALIB_LIMIT( p.x, 0, (int)m_W-1 ) + ALIB_LIMIT( p.y, 0, (int)m_H-1 ) * m_W ];
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns value of element at position (x,y), adjusting position
           to be inside 2D-array.

  \param  x  x-coordinate of point of interest (desired position).
  \param  y  y-coordinate of point of interest (desired position).
  \return    value at adjusted (bounded) position. */
//-------------------------------------------------------------------------------------------------
const_reference bounded_at( int x, int y ) const
{
  ASSERT( m_size > 0 );

  return m_head[ ALIB_LIMIT( x, 0, (int)m_W-1 ) + ALIB_LIMIT( y, 0, (int)m_H-1 ) * m_W ];
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "begin" iterator of specified row. */
//-------------------------------------------------------------------------------------------------
x_iterator row_begin( unsigned int r )
{
  ASSERT( r < m_H );

  return (m_head + r*m_W);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns constant "begin" iterator of specified row. */
//-------------------------------------------------------------------------------------------------
x_const_iterator row_begin( unsigned int r ) const
{
  ASSERT( r < m_H );

  return (m_head + r*m_W);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "end" iterator of specified row. */
//-------------------------------------------------------------------------------------------------
x_iterator row_end( unsigned int r )
{
  ASSERT( r < m_H );

  return (m_head + (r+1)*m_W);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns constant "end" iterator of specified row. */
//-------------------------------------------------------------------------------------------------
x_const_iterator row_end( unsigned int r ) const
{
  ASSERT( r < m_H );

  return (m_head + (r+1)*m_W);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "begin" iterator of specified column. */
//-------------------------------------------------------------------------------------------------
y_iterator column_begin( unsigned int c )
{
  ASSERT( c < m_W );

  return y_iterator( m_head+c, m_W, m_head+c, m_tail+c );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns constant "begin" iterator of specified column. */
//-------------------------------------------------------------------------------------------------
y_const_iterator column_begin( unsigned int c ) const
{
  ASSERT( c < m_W );

  return y_const_iterator( m_head+c, m_W, m_head+c, m_tail+c );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns "end" iterator of specified column. */
//-------------------------------------------------------------------------------------------------
y_iterator column_end( unsigned int c )
{
  ASSERT( c < m_W );

  return y_iterator( m_tail+c, m_W, m_head+c, m_tail+c );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns constant "end" iterator of specified column. */
//-------------------------------------------------------------------------------------------------
y_const_iterator column_end( unsigned int c ) const
{
  ASSERT( c < m_W );

  return y_const_iterator( m_tail+c, m_W, m_head+c, m_tail+c );
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
Arr2D() : m_head(0)
{
  clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
Arr2D( int W, int H, value_type v = value_type() ) : m_head(0)
{
  clear();
  resize( W, H, v );
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
Arr2D( const this_type & x ) : m_head(0)
{
  clear();
  (*this) = x;
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
~Arr2D()
{
  clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function quickly fills this object by zeros.
 
  D A N G E R O U S function! For simple types only! */
//-------------------------------------------------------------------------------------------------
void fast_zero()
{ 
  if (m_size > 0)
    memset( begin(), 0, sizeof(value_type)*m_size );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function quickly copies specified object to this one.
 
  D A N G E R O U S function! For simple types only! */
//-------------------------------------------------------------------------------------------------
void fast_copy( const this_type & x )
{
  resize( x.m_W, x.m_H, value_type(), false );
  if (m_size > 0)
    memcpy( begin(), x.begin(), sizeof(value_type)*m_size );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function copies a portion of the source 2D-array into this one.

  \param  dx   the destination start column.
  \param  dy   the destination start row.
  \param  src  the source 2D array (may coincide with this one).
  \param  sx   the source start column.
  \param  sy   the source start row.
  \param  cx   the number of columns to copy, 0 results in using of all columns from sx up to end.
  \param  cy   the number of rows to copy, 0 results in using of all rows from sy up to end.
  \return      this 2D-array. */
//-------------------------------------------------------------------------------------------------
this_type & copy( int dx, int dy, const this_type & src, int sx, int sy, int cx, int cy )
{
  if (cx == 0) cx = src.m_W - sx;
  if (cy == 0) cy = src.m_H - sy;

  if ((cx <= 0) || (cy <= 0))
    return (*this);

  ASSERT( (dx >= 0) && (dy >= 0) && (sx >= 0) && (sy >= 0) && (cx > 0) && (cy > 0) );
  ASSERT( ((dx+cx) <= (int)    (m_W)) && ((dy+cy) <= (int)    (m_H)) );
  ASSERT( ((sx+cx) <= (int)(src.m_W)) && ((sy+cy) <= (int)(src.m_H)) );

  if (dy > sy)
  {
    for(int j = cy-1; j >= 0; j--)
    {
      const_pointer s = src.row_begin( sy+j ) + sx;
      pointer       d = row_begin( dy+j ) + dx;

      for(int i = 0; i < cx; i++)
        d[i] = s[i];
    }
  }
  else if (dy == sy)
  {
    for(int j = 0; j < cy; j++)
    {
      const_pointer s = src.row_begin( sy+j ) + sx;
      pointer       d = row_begin( dy+j ) + dx;

      if (dx > sx)
      {
        for(int i = cx-1; i >= 0; i--)
          d[i] = s[i];
      }
      else if ((dx < sx) || (this != &src))
      {
        for(int i = 0; i < cx; i++)
          d[i] = s[i];
      }
    }
  }
  else // dy < sy
  {
    for(int j = 0; j < cy; j++)
    {
      const_pointer s = src.row_begin( sy+j ) + sx;
      pointer       d = row_begin( dy+j ) + dx;

      for(int i = 0; i < cx; i++)
        d[i] = s[i];
    }
  }
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function shrinks this array and copies the portion of old data in the new location.

  \param x   start column.
  \param y   start row.
  \param cx  number of columns to copy, 0 results in using of all columns form x up to end.
  \param cy  number of rows to copy, 0 results in using of all rows form y up to end. */
//-------------------------------------------------------------------------------------------------
this_type & reshape_down( int x, int y, int cx = 0, int cy = 0 )
{
  if (size == 0)
    return (*this);

  if (cx == 0) cx = m_W-x;
  if (cy == 0) cy = m_H-y;

  ASSERT( (x >= 0) && (y >= 0) && (cx > 0) && (cy > 0) );
  ASSERT( ((x+cx) <= (int)m_W) && ((y+cy) <= (int)m_H) );

  this_type tmp;
  tmp.resize( cx, cy, value_type(), false );
  tmp.copy( 0, 0, (*this), x, y, cx, cy );
  swap( tmp );
  return (*this);
}

};

#undef  ALIB_ARR2D_INSIDE

///////////////////////////////////////////////////////////////////////////////////////////////////
// Derived types.
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef  Arr2D<signed char,int>              Arr2sb;
typedef  Arr2D<unsigned char,unsigned int>   Arr2ub;

typedef  Arr2D<signed short,int>             Arr2s;
typedef  Arr2D<unsigned short,unsigned int>  Arr2us;

typedef  Arr2D<int,int>                      Arr2i;
typedef  Arr2D<unsigned int,unsigned int>    Arr2ui;

typedef  Arr2D<float,double>                 Arr2f;
typedef  Arr2D<double,double>                Arr2d;

typedef  Arr2D<Pnt2i,Pnt2i>                  Pnt2iImg;
typedef  Arr2D<Pnt2f,Pnt2f>                  Pnt2fImg;
typedef  Arr2D<Pnt2d,Pnt2d>                  Pnt2dImg;

typedef  Arr2D<Vec2i,Vec2i>                  Vec2iImg;
typedef  Arr2D<Vec2f,Vec2f>                  Vec2fImg;
typedef  Arr2D<Vec2d,Vec2d>                  Vec2dImg;

#endif // ALIB_ARR_2D_H

