/***************************************************************************
                          alib_vec2d.h - Two-dimensional point and vector objects.
                             -------------------
    begin                : 1998
    copyright            : (C) 1998-2004 by Alaudin (Albert) Akhriev
    email                : aaahaaah@hotmail.com
 ***************************************************************************/

#ifndef  ALIB_VECTOR_2D_H
#define  ALIB_VECTOR_2D_H

//=================================================================================================
/** \struct Point2D.
    \brief Two-dimensional point.

  'TYPE' - coordinate type, 'PRECISE' - superior type used to enhance computational precision. */
//=================================================================================================
template< class TYPE, class PRECISE >
struct Point2D
{
  typedef  TYPE     value_type;
  typedef  PRECISE  precise_type;

  value_type x;    //!< x-coordinate
  value_type y;    //!< y-coordinate
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Derived types.
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef  Point2D<short,int>      Pnt2s;
typedef  Point2D<int,int>        Pnt2i;
typedef  Point2D<float,double>   Pnt2f;
typedef  Point2D<double,double>  Pnt2d;

typedef  std::vector<Pnt2s>  Pnt2sArr;
typedef  std::vector<Pnt2i>  Pnt2iArr;
typedef  std::vector<Pnt2f>  Pnt2fArr;
typedef  std::vector<Pnt2d>  Pnt2dArr;

typedef  std::list<Pnt2s>  Pnt2sLst;
typedef  std::list<Pnt2i>  Pnt2iLst;
typedef  std::list<Pnt2f>  Pnt2fLst;
typedef  std::list<Pnt2d>  Pnt2dLst;


#define  ALIB_VECTOR2D_IS_VALID_TYPE  ( (std::numeric_limits< value_type >::is_specialized) && \
                                        (std::numeric_limits< precise_type >::is_specialized) && \
                                        (std::numeric_limits< value_type >::is_integer == \
                                         std::numeric_limits< precise_type >::is_integer) && \
                                        (std::numeric_limits< value_type >::is_signed == \
                                         std::numeric_limits< precise_type >::is_signed) && \
                                        (sizeof(value_type) <= sizeof(precise_type)) )


//=================================================================================================
/** \struct Vector2D.
    \brief Two-dimensional vector.

  'TYPE' - coordinate type, 'PRECISE' - superior type used to enhance computational precision. */
//=================================================================================================
template< class TYPE, class PRECISE >
struct Vector2D : public Point2D<TYPE,PRECISE>
{

typedef  Vector2D<TYPE,PRECISE>  this_type;

//-------------------------------------------------------------------------------------------------
/** \brief Contructor. */
//-------------------------------------------------------------------------------------------------
Vector2D()
{
  ASSERT( ALIB_VECTOR2D_IS_VALID_TYPE );
  x = (y = 0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Contructor.
  \param  nx  the new abscissa.
  \param  ny  the new ordinate. */
//-------------------------------------------------------------------------------------------------
Vector2D( value_type nx, value_type ny )
{
  ASSERT( ALIB_VECTOR2D_IS_VALID_TYPE );
  x = nx;
  y = ny;
}


//-------------------------------------------------------------------------------------------------
/** \brief Copy contructor that does NOT check the overflow of coordinate values.
  \param  v  the input vector with coordinates of any type. */
//-------------------------------------------------------------------------------------------------
template< class TYPE2, class PRECISE2 >
Vector2D( const Point2D< TYPE2, PRECISE2 > & v )
{
  ASSERT( ALIB_VECTOR2D_IS_VALID_TYPE );

  if (std::numeric_limits<TYPE>::is_integer && !(std::numeric_limits<TYPE2>::is_integer))
  {
    x = (value_type)(ALIB_ROUND( v.x ));
    y = (value_type)(ALIB_ROUND( v.y ));
  }
  else
  {
    x = (value_type)(v.x);
    y = (value_type)(v.y);
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Copy operator that does NOT check the overflow of coordinate values.
  \param  v  the input vector with coordinates of any type.
  \return    reference to this vector with newly set coordinates. */
//-------------------------------------------------------------------------------------------------
template< class TYPE2, class PRECISE2 >
this_type & operator=( const Point2D< TYPE2, PRECISE2 > & v )
{ 
  if (std::numeric_limits<TYPE>::is_integer && !(std::numeric_limits< TYPE2 >::is_integer))
  {
    x = (value_type)(ALIB_ROUND( v.x ));
    y = (value_type)(ALIB_ROUND( v.y ));
  }
  else
  {
    x = (value_type)(v.x);
    y = (value_type)(v.y);
  }
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns dimension of this vector.
  \return  dimension (=2). */
//-------------------------------------------------------------------------------------------------
int size() const
{ 
  return 2;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function sets coordinates of this vector.
  \param  nx  the new abscissa.
  \param  ny  the new ordinate.
  \return     reference to this vector. */
//-------------------------------------------------------------------------------------------------
this_type & set( value_type nx, value_type ny )
{ 
  x = nx;
  y = ny;
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes inner product of two vectors.
  \param  v  the second vector.
  \return    inner product. */
//-------------------------------------------------------------------------------------------------
template< class TYPE2 >
precise_type operator&( const Point2D< TYPE2, precise_type > & v ) const
{
  return (x*(precise_type)(v.x) +
          y*(precise_type)(v.y));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes outer product of two vectors.
  \param  v  the second vector.
  \return    outer product (z-coordinate in 3D space). */
//-------------------------------------------------------------------------------------------------
template< class TYPE2 >
precise_type operator*( const Point2D< TYPE2, precise_type > & v ) const
{
  return (x*(precise_type)(v.y) -
          y*(precise_type)(v.x));
}


//-------------------------------------------------------------------------------------------------
/** \brief Operator multiplies this vector by any scalar.
  \param  c  the scalar value.
  \return    reference to this vector. */
//-------------------------------------------------------------------------------------------------
this_type & operator*=( value_type c )
{ 
  x *= c;
  y *= c;
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief Operator produces this vector multiplied by any scalar.
  \param  c  the scalar value.
  \return    resultant vector. */
//-------------------------------------------------------------------------------------------------
this_type operator*( value_type c ) const
{ 
  return this_type( x * c,
                    y * c );
}


//-------------------------------------------------------------------------------------------------
/** \brief Operator adds specified vector to this one.
  \param  v  the vector to be added.
  \return    reference to this vector. */
//-------------------------------------------------------------------------------------------------
template< class VECTOR_TYPE >
this_type & operator+=( const VECTOR_TYPE & v )
{
  x = (value_type)(x + v.x);
  y = (value_type)(y + v.y);
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief Operator returns the sum of this vector and specified one.
  \param  v  the vector to be added.
  \return    the sum of this vector and specified one. */
//-------------------------------------------------------------------------------------------------
template< class VECTOR_TYPE >
this_type operator+( const VECTOR_TYPE & v ) const
{ 
  return this_type( (value_type)(x + v.x),
                    (value_type)(y + v.y) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Operator subtracts specified vector from this one.
  \param  v  the vector to be subtracted.
  \return    reference to this vector. */
//-------------------------------------------------------------------------------------------------
template< class VECTOR_TYPE >
this_type & operator-=( const VECTOR_TYPE & v )
{
  x = (value_type)(x - v.x);
  y = (value_type)(y - v.y);
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief Operator returns the vector of difference between this vector and specified one.
  \param  v  the vector to be subtracted.
  \return    the difference of this vector and specified one. */
//-------------------------------------------------------------------------------------------------
template< class VECTOR_TYPE >
this_type operator-( const VECTOR_TYPE & v ) const
{
  return this_type( (value_type)(x - v.x),
                    (value_type)(y - v.y) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes squared length of this vector.
  \return  squared length of this vector. */
//-------------------------------------------------------------------------------------------------
precise_type sq_length() const
{ 
  return ( x*(precise_type)x + y*(precise_type)y );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes the length of this vector.
  \return  the length of this vector. */
//-------------------------------------------------------------------------------------------------
double length() const
{ 
  return sqrt( (double)(x*(precise_type)x + y*(precise_type)y) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes squared length of a difference between this vector and specified one.
  \param  v  the vector the difference is counted off from.
  \return    squared length of the difference. */
//-------------------------------------------------------------------------------------------------
template< class VECTOR_TYPE >
precise_type sq_distance( const VECTOR_TYPE & v ) const
{
  precise_type dx = (precise_type)x - (precise_type)(v.x);
  precise_type dy = (precise_type)y - (precise_type)(v.y);
  return (dx*dx + dy*dy);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes the length of a difference between this vector and specified one.
  \param  v  the vector the difference is counted off from.
  \return    the length of the difference. */
//-------------------------------------------------------------------------------------------------
template< class VECTOR_TYPE >
double distance( const VECTOR_TYPE & p ) const
{
  double dx = (double)x - (double)(p.x);
  double dy = (double)y - (double)(p.y);
  return sqrt( dx*dx + dy*dy );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes the angle between this vector and X-axis.
  \param  b0_2PI  if true, then the range of the angle will be [0...2PI], otherwise [-PI...PI].
  \return         the angle. */
//-------------------------------------------------------------------------------------------------
double angle( bool b0_2PI = false ) const
{ 
  double a = atan2( (double)y, (double)x );
  if (b0_2PI && (a < (value_type)0)) a = fabs( a + (2.0*3.14159265358979323846) );
  return a;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function negates coordinates of this vector.
  \return  reference to this vector. */
//-------------------------------------------------------------------------------------------------
this_type & negate()
{ 
  x = -x;
  y = -y;
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns the vector with negative coordinates of this one.
  \return  the newly constructed vector with negative coordinates. */
//-------------------------------------------------------------------------------------------------
this_type operator-() const
{ 
  return this_type( -x, -y );
}


//-------------------------------------------------------------------------------------------------
/** \brief Operator addresses <i>x</i> or <i>y</i> coordinate.
  \return  the <i>x</i> (<i>n</i>=0) or <i>y</i> (<i>n</i>=1) coordinate. */
//-------------------------------------------------------------------------------------------------
value_type & operator[]( int n )
{ 
  ASSERT((n == 0) || (n == 1));
  return (n == 0) ? x : y;
}


//-------------------------------------------------------------------------------------------------
/** \brief Operator addresses <i>x</i> or <i>y</i> coordinate.
  \return  the <i>x</i> (<i>n</i>=0) or <i>y</i> (<i>n</i>=1) coordinate. */
//-------------------------------------------------------------------------------------------------
value_type operator[]( int n ) const
{ 
  ASSERT((n == 0) || (n == 1));
  return (n == 0) ? x : y;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns a vector that is perpendicular to this one.
  \return  the vector that is perpendicular to this one. */
//-------------------------------------------------------------------------------------------------
this_type perpendicular() const
{ 
  return this_type( -y, x );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function sets this vector perpendicular to its initial direction.
  \return  reference to this vector that is made perpendicular to the initial direction. */
//-------------------------------------------------------------------------------------------------
this_type & set_perpendicular()
{
  value_type t = x;  x = -y;  y = t;
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns <i>true</i> if the vector has integer coordinates.
  \return  <i>true</i> if the vector has integer coordinates. */
//-------------------------------------------------------------------------------------------------
bool integer() const
{ 
  return std::numeric_limits<value_type>::is_integer;
}


//-------------------------------------------------------------------------------------------------
/** \brief Operator divides this vector by any scalar.
  \param  c  the scalar value.
  \return    reference to this vector. */
//-------------------------------------------------------------------------------------------------
this_type & operator/=( value_type c )
{ 
  #ifdef _DEBUG
  bool   bin = std::numeric_limits<value_type>::is_integer;
  double min = std::numeric_limits<value_type>::min();
  double eps = std::numeric_limits<value_type>::epsilon();
  ASSERT( (bin && (c != (value_type)0)) || (!bin && (fabs( (double)c ) > (min/eps))) );
  #endif
  
  x /= c;
  y /= c;
  return (*this);
}


//-------------------------------------------------------------------------------------------------
/** \brief Operator produces this vector divides by any scalar.
  \param  c  the scalar value.
  \return    resultant vector. */
//-------------------------------------------------------------------------------------------------
this_type operator/( value_type c ) const
{ 
  #ifdef _DEBUG
  bool   bin = std::numeric_limits<value_type>::is_integer;
  double min = std::numeric_limits<value_type>::min();
  double eps = std::numeric_limits<value_type>::epsilon();
  ASSERT( (bin && (c != (value_type)0)) || (!bin && (fabs( (double)c ) > (min/eps))) );
  #endif

  return this_type( x / c,
                    y / c );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function compares two vectors to determine if they have the same coordinates.
  \param  v  the vector to compare with.
  \return    <i>true</i>, if two vectors are the same. */
//-------------------------------------------------------------------------------------------------
bool operator==( const this_type & v ) const
{
  if (std::numeric_limits<value_type>::is_integer)
  {
    return ( (x == v.x) &&
             (y == v.y) );
  }
  else
  {
    const double eps = 3.0*std::numeric_limits<value_type>::epsilon();

    return ( (fabs( (double)(x - v.x) ) <= eps) &&
             (fabs( (double)(y - v.y) ) <= eps) );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function compares two vectors to determine if they have different coordinates.
  \param  v  the vector to compare with.
  \return    <i>true</i>, if two vectors are different. */
//-------------------------------------------------------------------------------------------------
bool operator!=( const this_type & v ) const
{ 
  if (std::numeric_limits<value_type>::is_integer)
  {
    return ( (x != v.x) ||
             (y != v.y) );
  }
  else
  {
    double eps = 3.0*std::numeric_limits<value_type>::epsilon();

    return ( (fabs( (double)(x - v.x) ) > eps) ||
             (fabs( (double)(y - v.y) ) > eps) );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function compares lengthes of this vector and specified.
  \param  v  the vector to compare with.
  \return    <i>true</i>, if this vector is shorter than specified one. */
//-------------------------------------------------------------------------------------------------
bool operator<( const this_type & v ) const
{ 
  return ((x*(precise_type)x + y*(precise_type)y) < (v.x*(precise_type)v.x + v.y*(precise_type)v.y));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function compares lengthes of this vector and specified.
  \param  v  the vector to compare with.
  \return    <i>true</i>, if this vector is longer than specified one. */
//-------------------------------------------------------------------------------------------------
bool operator>( const this_type & v ) const
{ 
  return ((x*(precise_type)x + y*(precise_type)y) > (v.x*(precise_type)v.x + v.y*(precise_type)v.y));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function shifts to the left the values of vector's coordinates.
  \param  s  the shift.
  \return    reference to this vector. */
//-------------------------------------------------------------------------------------------------
this_type operator<<( int s ) const
{ 
  ASSERT( std::numeric_limits<value_type>::is_integer );
  return this_type( x << s,
                    y << s );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function shifts to the right the values of vector's coordinates.
  \param  s  the shift.
  \return    reference to this vector. */
//-------------------------------------------------------------------------------------------------
this_type operator>>( int s ) const
{
  ASSERT( std::numeric_limits<value_type>::is_integer );
  return this_type( x >> s,
                    y >> s );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function checks whether specified point is the neighbour of this one.
  \param  v  the vector to compare with.
  \return    -1 - points coincide, +1 - points are neighbours, 0 - points are not neighbours. */
//-------------------------------------------------------------------------------------------------
int is_neighbor( const this_type & v ) const
{
  ASSERT( std::numeric_limits<value_type>::is_integer );
  return ALIB_IS_NEIGHBOR2( x, y, v.x, v.y );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function normalizes this vector.
  \return  the length of initial vector. */
//-------------------------------------------------------------------------------------------------
double normalize()
{
  ASSERT( !(std::numeric_limits<value_type>::is_integer) );

  const value_type min = std::numeric_limits<value_type>::min() /
                         std::numeric_limits<value_type>::epsilon();
  double           len = sqrt( (double)(x*(precise_type)x + y*(precise_type)y) );
  double           rev = (len > min) ? (1.0/len) : (len = 0.0);

  x *= (value_type)rev;
  y *= (value_type)rev;
  return len;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns rotated vector.
  \param  angle  the angle of rotation.
  \return        rotated vector. */
//-------------------------------------------------------------------------------------------------
this_type get_rotated( double angle ) const
{
  ASSERT( !(std::numeric_limits<value_type>::is_integer) );
  double cs = cos( angle );
  double sn = sin( angle );
  return this_type( (value_type)(x*cs - y*sn),
                    (value_type)(x*sn + y*cs) );
}

#ifdef _WINDOWS

//-----------------------------------------------------------------------------------------------
/** \brief Contructor converts this vector to Windows POINT.
  \param  p  the object of POINT type. */
//-----------------------------------------------------------------------------------------------
Vector2D( const POINT & p )
{
  ASSERT( ALIB_VECTOR2D_IS_VALID_TYPE );
  x = (value_type)(p.x);
  y = (value_type)(p.y);
}


//-----------------------------------------------------------------------------------------------
/** \brief Operator converts this vector to Windows POINT.
  \return  the object of POINT type. */
//-----------------------------------------------------------------------------------------------
operator POINT() const
{ 
  POINT p;

  if (std::numeric_limits<value_type>::is_integer)
  {
    p.x = (LONG)x;
    p.y = (LONG)y;
  }
  else
  {
    p.x = (LONG)ALIB_ROUND( x );
    p.y = (LONG)ALIB_ROUND( y );
  }
  return p;
}


//-----------------------------------------------------------------------------------------------
/** \brief Operator converts Windows POINT to this vector.
  \return  reference to this vector. */
//-----------------------------------------------------------------------------------------------
this_type & operator=( const POINT & p )
{ 
  x = (value_type)(p.x);
  y = (value_type)(p.y);
  return (*this);
}

#endif // _WINDOWS

};

#undef  ALIB_VECTOR2D_IS_VALID_TYPE


///////////////////////////////////////////////////////////////////////////////////////////////////
// Relation of three points.
///////////////////////////////////////////////////////////////////////////////////////////////////

enum RelationOfThreePoints
{
  REL3PNT_LEFT    = (1<<0), 
  REL3PNT_RIGHT   = (1<<1), 
  REL3PNT_BEYOND  = (1<<2),
  REL3PNT_BEHIND  = (1<<3),
  REL3PNT_BETWEEN = (1<<4),
  REL3PNT_HEAD    = (1<<5),
  REL3PNT_TAIL    = (1<<6)
};


//-------------------------------------------------------------------------------------------------
/** Function classifies relative positions of three points.

  <tt><pre>

  p1 *----------* p2
     |
     |
     * p3

  </pre></tt>

  \param  p1  the first point.
  \param  p2  the second point.
  \param  p3  the third point.
  \return     three-point relation type. */
//-------------------------------------------------------------------------------------------------
template< class TYPE, class PRECISE >
RelationOfThreePoints ClassifyPosition(
  const Vector2D<TYPE,PRECISE> & p1,
  const Vector2D<TYPE,PRECISE> & p2,
  const Vector2D<TYPE,PRECISE> & p3)
{
  Vector2D<TYPE,PRECISE> a = p2 - p1;
  Vector2D<TYPE,PRECISE> b = p3 - p1;
  PRECISE                ab = a*b;

  if (ab > (PRECISE)0)
    return RelationOfThreePoints::REL3PNT_LEFT;

  if (ab < (PRECISE)0)
    return RelationOfThreePoints::REL3PNT_RIGHT;

  if (((PRECISE)(a.x)*(PRECISE)(b.x) < (PRECISE)0) ||
      ((PRECISE)(a.y)*(PRECISE)(b.y) < (PRECISE)0))
    return RelationOfThreePoints::REL3PNT_BEHIND;

  if (a.sq_length() < b.sq_length())
    return RelationOfThreePoints::REL3PNT_BEYOND;

  if (p1 == p3)
    return RelationOfThreePoints::REL3PNT_HEAD;

  if (p2 == p3)
    return RelationOfThreePoints::REL3PNT_TAIL;

  return RelationOfThreePoints::REL3PNT_BETWEEN;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Derived types.
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef  Vector2D<short,int>      Vec2s;
typedef  Vector2D<int,int>        Vec2i;
typedef  Vector2D<float,double>   Vec2f;
typedef  Vector2D<double,double>  Vec2d;

typedef  std::vector<Vec2s>  Vec2sArr;
typedef  std::vector<Vec2i>  Vec2iArr;
typedef  std::vector<Vec2f>  Vec2fArr;
typedef  std::vector<Vec2d>  Vec2dArr;

typedef  std::vector<Vec2s>::iterator  Vec2sArrIt;
typedef  std::vector<Vec2i>::iterator  Vec2iArrIt;
typedef  std::vector<Vec2f>::iterator  Vec2fArrIt;
typedef  std::vector<Vec2d>::iterator  Vec2dArrIt;

typedef  std::vector<Vec2s>::const_iterator  Vec2sArrCIt;
typedef  std::vector<Vec2i>::const_iterator  Vec2iArrCIt;
typedef  std::vector<Vec2f>::const_iterator  Vec2fArrCIt;
typedef  std::vector<Vec2d>::const_iterator  Vec2dArrCIt;

typedef  std::list<Vec2s>  Vec2sLst;
typedef  std::list<Vec2i>  Vec2iLst;
typedef  std::list<Vec2f>  Vec2fLst;
typedef  std::list<Vec2d>  Vec2dLst;

// Counter-clockwise neighbors of a pixel, 8-connectivity.
const Vec2i ALIB_NEIBR8[8] = { Vec2i( 1, 0),
                               Vec2i( 1, 1),
                               Vec2i( 0, 1),
                               Vec2i(-1, 1),
                               Vec2i(-1, 0),
                               Vec2i(-1,-1),
                               Vec2i( 0,-1),
                               Vec2i( 1,-1) };

// Counter-clockwise neighbors of a pixel, 4-connectivity.
const Vec2i ALIB_NEIBR4[4] = { Vec2i( 1, 0),
                               Vec2i( 0, 1),
                               Vec2i(-1, 0),
                               Vec2i( 0,-1) };

#endif // ALIB_VECTOR_2D_H

