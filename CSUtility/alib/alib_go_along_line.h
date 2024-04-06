#ifndef  ALIB_GO_ALONG_LINE_H
#define  ALIB_GO_ALONG_LINE_H

//=================================================================================================
// Class provides implementation of iterator that goes along a line according to
// Brezenham algorithm. See also constructor's comment. <br>
// <pre>
// Usage:
// GoAlongLine gal(bIncludeP2);
// for(const Vec2i * p = gal.first(p1,p2); p != 0; p = gal.next())
// {
//   .....
//   x = p->x;
//   y = p->y;
//   .....
// }
// </pre>
//=================================================================================================
class /*ALIB_API*/ GoAlongLine
{
private:
  int   D1, D2;         //!< distances between end-points along coordinate axes
  int   incX1, incY1;   //!< increments of x and y on each iteration step
  int   incX2, incY2;   //!< these increments correct x and y on some steps
  int   counter;        //!< counter of passed line points
  int   accum;          //!< accumulator spies on coordinates correction
  Vec2i vec;            //!< temporary vector
  int   x, y;           //!< coordinates of the current line point
  bool  bLastPt;        //!< include (or not) end point into process

public:
//-------------------------------------------------------------------------------------------------
/** \brief Constructor.

  If flag <i>bLastPt</i> is set on, then the second end point (p2) will be included in iteration
  process. So, the total number of passed points will be:<br>
  <center> max( |p2.x-p1.x|, |p2.y-p1.y| ) + 1 </center>.

  \param  bLastPoint  flag tells whether the last point must be included or not. */
//-------------------------------------------------------------------------------------------------
GoAlongLine( bool bLastPoint = true )
{ 
  memset( this, 0, sizeof(this) );
  bLastPt = bLastPoint;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns length of line in pixels.

  \param  p1  first end point of the line.
  \param  p2  second end point of the line.
  \return     line's length in pixels. */
//-------------------------------------------------------------------------------------------------
int length( Vec2i p1, Vec2i p2 ) const
{
  int Dx = abs( p2.x - p1.x );
  int Dy = abs( p2.y - p1.y );
  return ((bLastPt ? 1 : 0) + ((Dx >= Dy) ? Dx : Dy));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function is called at the beginning of iteration process.

  Function initializes internal variable using coordinates of specified end-points.

  \param  p1  first end point of the line.
  \param  p2  second end point of the line.
  \return     pointer to the temporal storage of the first end-point or 0 for empty line. */
//-------------------------------------------------------------------------------------------------
const Vec2i * first( Vec2i p1, Vec2i p2 )
{
  int Dx = abs( p2.x - p1.x );
  int Dy = abs( p2.y - p1.y );
  int Sx = (p2.x > p1.x) ? 1 : ((p2.x == p1.x) ? 0 : -1);
  int Sy = (p2.y > p1.y) ? 1 : ((p2.y == p1.y) ? 0 : -1);

  vec.x = (x = p1.x);
  vec.y = (y = p1.y);

  if (Dx >= Dy)
  {
    D1 = Dx;
    D2 = Dy;
    accum = Dx >> 1;
    counter = Dx;
    incX1 = Sx;
    incY1 = 0;
    incX2 = 0;
    incY2 = Sy;
  }
  else
  {
    D1 = Dy;
    D2 = Dx;
    accum = Dy >> 1;
    counter = Dy;
    incX1 = 0;
    incY1 = Sy;
    incX2 = Sx;
    incY2 = 0;
  }
  return (counter >= (bLastPt ? 0 : 1)) ? &vec : 0;
}


//-------------------------------------------------------------------------------------------------
/** Function proceeds iteration process calculating the next line's point.

  \return  pointer to the temporal storage of next point of the line or 0 if the end
           of the line is reached. */
//-------------------------------------------------------------------------------------------------
const Vec2i * next()
{
  counter--;
  if (counter >= (bLastPt ? 0 : 1))
  {
    if ((accum += D2) >= D1) 
    { 
      x += incX2;  
      y += incY2;  
      accum -= D1; 
    } 
    vec.x = (x += incX1);
    vec.y = (y += incY1);
    return &vec;
  }
  return 0;
}

};

#endif // ALIB_GO_ALONG_LINE_H
