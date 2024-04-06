#ifndef  ALIB_ARECT_H
#define  ALIB_ARECT_H

///////////////////////////////////////////////////////////////////////////////////////////////////
// Rectangle.
// <p><pre><tt>
//
//         -----------* (x2,y2)
//         |          |
//         |          |
//         |          |
//         |          |
// (x1,y1) *-----------
//
// </tt></pre></p>
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ARect
{
  int x1, y1, x2, y2;   //!< coordinates of rectangle's points


  /** \brief Function normalizes rectangle so that both the height and width are positive. */
  ARect & normalize()
  {
    if (x1 > x2) std::swap( x1, x2 );
    if (y1 > y2) std::swap( y1, y2 );
    return (*this);
  }


  /** \brief Function checks whether rectangle is normalized. */
  bool is_normalized() const
  {
    return ((x1 <= x2) && (y1 <= y2));
  }


  /** \brief Constructor. */
  ARect() : x1(0), y1(0), x2(0), y2(0)
  {
  }


  /** \brief Constructor. */
  ARect( int nx1, int ny1, int nx2, int ny2 ) : x1(nx1), y1(ny1), x2(nx2), y2(ny2)
  {
  }


  /** \brief Constructor. */
  ARect( const ARect & rect )
  {
    x1 = rect.x1;
    y1 = rect.y1;
    x2 = rect.x2;
    y2 = rect.y2;
  }


  /** \brief Constructor. */
  template< class POINT_TYPE >
  ARect( const POINT_TYPE & p1, const POINT_TYPE & p2 )
  : x1( (int)(p1.x) ), y1( (int)(p1.y) ), x2( (int)(p2.x) ), y2( (int)(p2.y) )
  {
  }


  /** \brief Calculates the width by subtracting the x1 from the x2 (can be negative). */
  int width() const
  {
    return (x2-x1);
  }


  /** \brief Calculates the height by subtracting the y1 from the y2 (can be negative). */
  int height() const
  {
    return (y2-y1);
  }


  /** \brief Function returns (x1,y1). Example: CPoint pt = rect.get_point1( (CPoint*)0, int() ); */
  template< class POINT_TYPE, class COORD_TYPE >
  POINT_TYPE get_point1( POINT_TYPE *, COORD_TYPE ) const
  {
    POINT_TYPE pt;
    pt.x = static_cast<COORD_TYPE>( x1 );
    pt.y = static_cast<COORD_TYPE>( y1 );
    return pt;
  }


  /** \brief Function returns (x2,y2). Example: CPoint pt = rect.get_point2( (CPoint*)0, int() ); */
  template< class POINT_TYPE, class COORD_TYPE >
  POINT_TYPE get_point2( POINT_TYPE *, COORD_TYPE ) const
  {
    POINT_TYPE pt;
    pt.x = static_cast<COORD_TYPE>( x2 );
    pt.y = static_cast<COORD_TYPE>( y2 );
    return pt;
  }


  /** \brief Function returns the center point of rectangle. */
  template< class POINT_TYPE, class COORD_TYPE >
  void get_center( POINT_TYPE & pt, COORD_TYPE ) const   // example: CPoint pt;  rect.get_center( pt, int() );
  {
    pt.x = static_cast<COORD_TYPE>( (x1+x2)/2 );
    pt.y = static_cast<COORD_TYPE>( (y1+y2)/2 );
  }


  /** \brief Function determines whether rectangle is empty (zero area). */
  bool empty() const
  {
    return ((x1 == x2) || (y1 == y2));
  }


  /** \brief Function determines whether all coordinate are zero. */
  bool is_null() const
  { 
    return ((x1 == 0) && (y1 == 0) && (x2 == 0) && (y2 == 0));
  }


  /** \brief Function determines whether the specified point lies within the rectangle. */
  template< class POINT_TYPE >
  bool inside( const POINT_TYPE & point ) const
  { 
    return ((x1 <= (int)(point.x)) && ((int)(point.x) < x2) &&
            (y1 <= (int)(point.y)) && ((int)(point.y) < y2));
  }


  /** \brief Function determines whether the specified point lies within the rectangle. */
  bool inside( int x, int y ) const
  {
    return ((x1 <= x) && (x < x2) &&
            (y1 <= y) && (y < y2));
  }


  /** \brief Function initializes rectangle's coordinates. */
  ARect & set( int nx1, int ny1, int nx2, int ny2 )
  { 
    x1 = nx1;   x2 = nx2;
    y1 = ny1;   y2 = ny2;
    return (*this);
  }


  /** \brief Function initializes rectangle's coordinates. */
  template< class POINT_TYPE >
  ARect & set( const POINT_TYPE & p1, const POINT_TYPE & p2 )
  { 
    x1 = (int)(p1.x);   x2 = (int)(p2.x);
    y1 = (int)(p1.y);   y2 = (int)(p2.y);
    return (*this);
  }


  /** \brief Function sets rectangle's coordinates to zero. */
  ARect & set_null()
  {
    x1 = (y1 = (x2 = (y2 = 0)));
    return (*this);
  }


  /** \brief Function returns the area of rectangle. */
  int area() const
  {
    // ASSERT( is_normalized() );
    return (x2-x1)*(y2-y1);
  }


  /** \brief Function copies the coordinates of specified rectangle to this one. */
  ARect & operator=( const ARect & rect )
  {
    x1 = rect.x1;
    y1 = rect.y1;
    x2 = rect.x2;
    y2 = rect.y2;
    return (*this);
  }


  /** \brief Function determines whether this rectangle is equal to the specified one. */
  bool operator==( const ARect & rect ) const
  {
    return ((x1 == rect.x1) && (y1 == rect.y1) && (x2 == rect.x2) && (y2 == rect.y2));
  }


  /** \brief Function determines whether this rectangle is not equal to the specified one. */
  bool operator!=( const ARect & rect ) const
  {
    return ((x1 != rect.x1) || (y1 != rect.y1) || (x2 != rect.x2) || (y2 != rect.y2));
  }


  /** \brief Function returns TRUE if this rectangle lies inside or touches with specified one. */
  bool operator<=( const ARect & rect ) const
  {
    ASSERT( is_normalized() && rect.is_normalized() );

    return ((rect.x1 <= x1) && (x2 <= rect.x2) &&
            (rect.y1 <= y1) && (y2 <= rect.y2));
  }

  /** \brief Function returns TRUE if this rectangle lies inside specified one. */
  bool operator<( const ARect & rect ) const
  {
    ASSERT( is_normalized() && rect.is_normalized() );

    return ((rect.x1 < x1) && (x2 < rect.x2) &&
            (rect.y1 < y1) && (y2 < rect.y2));
  }


  /** \brief Function inflates rectangle by moving its sides away from its center. */
  ARect & inflate( int dx, int dy )
  {
    x1 -= dx;   x2 += dx;
    y1 -= dy;   y2 += dy;
    return (*this);
  }


  /** \brief Function inflates rectangle by moving its sides away from its center. */
  ARect & inflate( int dx1, int dy1, int dx2, int dy2 )
  {
    x1 -= dx1;   x2 += dx2;
    y1 -= dy1;   y2 += dy2;
    return (*this);
  }


  /** \brief Function deflates rectangle by moving its sides toward its center. */
  ARect & deflate( int dx, int dy )
  {
    x1 += dx;   x2 -= dx;
    y1 += dy;   y2 -= dy;
    return (*this);
  }


  /** \brief Function deflates rectangle by moving its sides toward its center. */
  ARect & deflate( int dx1, int dy1, int dx2, int dy2 )
  {
    x1 += dx1;   x2 -= dx2;
    y1 += dy1;   y2 -= dy2;
    return (*this);
  }


  /** \brief Function moves rectangle by the specified offsets. */
  ARect & move( int dx, int dy )
  {
    x1 += dx;   x2 += dx;
    y1 += dy;   y2 += dy;
    return (*this);
  }


  /** \brief Function moves rectangle by the specified offsets. */
  template< class POINT_TYPE >
  ARect & move( const POINT_TYPE & pt )
  {
    x1 += pt.x;   x2 += pt.x;
    y1 += pt.y;   y2 += pt.y;
    return (*this);
  }


  /** \brief Function returns rectangle of intersection of this one and specified one. */
  ARect operator&( const ARect & rect ) const
  {
    ASSERT( is_normalized() && rect.is_normalized() );

    ARect tmp( std::max( x1, rect.x1 ), std::max( y1, rect.y1 ),
               std::min( x2, rect.x2 ), std::min( y2, rect.y2 ) );

    if (!(tmp.is_normalized()))
      tmp.set_null();
    return tmp;
  }


  /** \brief Function sets this rectangle by intersection of this one and specified one. */
  ARect & operator&=( const ARect & rect )
  { 
    return ((*this) = ((*this) & rect));
  }


  /** \brief Function returns the smallest rectangle that contains this one and specified one. */
  ARect operator|( const ARect & rect ) const
  { 
    ASSERT( is_normalized() && rect.is_normalized() );
    
    return ARect( std::min( x1, rect.x1 ), std::min( y1, rect.y1 ),
                  std::max( x2, rect.x2 ), std::max( y2, rect.y2 ) );
  }


  /** \brief Function sets this rectangle equal to the union of this one and specified one. */
  ARect & operator|=( const ARect & rect )
  { 
    return ((*this) = ((*this) | rect));
  }


  /** \brief Function multiplies coordinates of this rectangle by specified coefficient. */
  ARect & operator*=( int coef )
  {
    x1 *= coef;
    y1 *= coef;
    x2 *= coef;
    y2 *= coef;
    return (*this);
  }


  /** \brief Function forces this rectangle to lie within specified one. */
  ARect & push_inside( const ARect & rect )
  {
    ASSERT( is_normalized() && rect.is_normalized() );
    int W = x2 - x1;
    int H = y2 - y1;
    if (x1 < rect.x1) x2 = (x1 = rect.x1) + W; else if (x2 > rect.x2) x1 = (x2 = rect.x2) - W;
    if (y1 < rect.y1) y2 = (y1 = rect.y1) + H; else if (y2 > rect.y2) y1 = (y2 = rect.y2) - H;
    (*this) &= rect;
    return (*this);
  }

#ifdef _WINDOWS

  /** \brief Constructor. */
  ARect( const RECT & rect )
  {
    x1 = rect.left;
    y1 = rect.top;
    x2 = rect.right;
    y2 = rect.bottom;
  }

  /** \brief Function converts this rectangle to RECT. */
  operator RECT() const
  {
    RECT rect;
    rect.left   = x1;
    rect.top    = y1;
    rect.right  = x2;
    rect.bottom = y2;
    return rect;
  }

  /** \brief Function copies the coordinates of specified rectangle of RECT type to this one. */
  ARect & operator=( const RECT & rect )
  {
    x1 = rect.left;
    y1 = rect.top;
    x2 = rect.right;
    y2 = rect.bottom;
    return (*this);
  }

#endif // _WINDOWS
};

typedef  std::vector<ARect>  ARectArr;
typedef  std::list<ARect>    ARectLst;

#endif // ALIB_ARECT_H
