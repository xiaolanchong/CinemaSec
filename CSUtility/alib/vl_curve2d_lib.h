#ifndef  VISLIB_CURVE_LIBRARY_2_DIMENSIONS_H
#define  VISLIB_CURVE_LIBRARY_2_DIMENSIONS_H

namespace vislib
{

//=================================================================================================
// Function-object linearly interpolates between two 2D points.
//=================================================================================================
template< class T >
class Interpolate2D
{
 private:
  T  m_result;     //!< temporal variable is needed to avoid the call of contructor of 'T'

 public:
  //-----------------------------------------------------------------------------------------------
  /** \brief Function carries out linear interpolation.
    
    \f[ {\bf v} = {\bf v}_1  + ({\bf v}_2  - {\bf v}_1) \frac{{t - t_1}}{{t_2  - t_1}} \f]

    \param  v1  first vector.
    \param  v2  second vector.
    \param  t   parameter associated with interpolated point.
    \param  t1  parameter associated with v1.
    \param  t2  parameter associated with v2.
    \return     interpolated position. */
  //-----------------------------------------------------------------------------------------------
  T & operator()(const T & v1, const T & v2, double t, double t1, double t2)
  {
    double dt = t2-t1;
    t = (fabs(dt) > (FLT_MIN/FLT_EPSILON)) ? ((t-t1)/dt) : 0.0;
    m_result.x = (float)( v1.x + (v2.x - v1.x)*t );
    m_result.y = (float)( v1.y + (v2.y - v1.y)*t );
    return m_result;
  }
};


//=================================================================================================
// Function-object normalizes a vector given by iterator of type "ITER"
// with coordinates of any float type.
//=================================================================================================
template< class ITER >
struct Normalize2D
{
  void operator()(ITER it)
  {
    double  x = (*it).x;
    double  y = (*it).y;
    double  scale = x*x + y*y;

    scale = (scale > (FLT_MIN/FLT_EPSILON)) ? (1.0/sqrt(scale)) : 0.0;
    (*it).x *= (float)scale;
    (*it).y *= (float)scale;
  }
};


//-------------------------------------------------------------------------------------------------
/** \brief  Function splits coodinates of curve's points ((*InIt).x, (*InIt).y) into two separate
            containers of x-coordinates and y-coordinates respectively.

  \param  first  begin iterator of the input curve.
  \param  last   end iterator of the input curve.
  \param  xF     begin iterator of the output container of x-coordinates.
  \param  xL     end iterator of the output container of x-coordinates.
  \param  yF     begin iterator of the output container of y-coordinates.
  \param  yL     end iterator of the output container of y-coordinates. */
//-------------------------------------------------------------------------------------------------
template< class InIt, class OutIt >
void SplitCoordinates( InIt first, InIt last, OutIt xF, OutIt xL, OutIt yF, OutIt yL )
{
  xL;yL;
  ASSERT( std::distance( first, last ) == std::distance( xF, xL ) );
  ASSERT( std::distance( first, last ) == std::distance( yF, yL ) );

  for(; first != last; ++first)
  {
    (*xF) = (*first).x;   ++xF;
    (*yF) = (*first).y;   ++yF;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function repeatedly smoothes in-place an opened curve by (1 2 1) mask.

 Several successive repetitions act like Gauss filter with parameter equals to 'sigma'.
 Curve's end points stay fixed (!).

 */
//-------------------------------------------------------------------------------------------------
template< class InIt >
void SmoothOpenedCurveWithFixedEnds( InIt first, InIt last, float sigma )
{
  Vec2f  pre, cur, nxt;
  InIt   curIt, nxtIt;
  int    nRepeat = std::max( 1, (int)ceil((sigma*sigma)/(0.7413*0.7413)) );

  if (std::distance( first, last ) < 3)
    return;
  ASSERT( (*first).integer() == false );
  
  for (int r = 0; r < nRepeat; r++)
  {
    ++(nxtIt = ++(curIt = first));
    pre.x = (float)((*first).x);    cur.x = (float)((*curIt).x);
    pre.y = (float)((*first).y);    cur.y = (float)((*curIt).y);

    while (nxtIt != last)
    {
      nxt.x = (float)((*nxtIt).x);
      nxt.y = (float)((*nxtIt).y);

      (*curIt).x = (float)((pre.x + cur.x + cur.x + nxt.x) * 0.25);
      (*curIt).y = (float)((pre.y + cur.y + cur.y + nxt.y) * 0.25);

      pre = cur;
      cur = nxt;
      curIt = nxtIt++;
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function repeatedly smoothes curve by (1 2 1) mask.

  Several successive repetitions act like Gaussian with parameter roughly equals to 'sigma'.
  
  \param  F        begin iterator of the input curve.
  \param  L        end iterator of the input curve.
  \param  bClosed  non-zero for enclosed input curve.
  \param  nRepeat  number of repetitions or 0 ('sigma' will be considered instead).
  \param  sigma    if nRepeat=0 then variance of equivalent Gaussian, otherwise ignored. */
//-------------------------------------------------------------------------------------------------
template< class InIt >
void RepeatedlySmoothCurve( InIt F, InIt L, bool bClosed, unsigned int nRepeat, float sigma = 0.0f )
{
  if (F == L)
    return;

  std::pair<float,float>  pre, cur, nxt, headPoint;
  InIt                    curIt, nxtIt, preL;

  nRepeat = (nRepeat > 0) ? nRepeat : std::max( 1, (int)ceil((sigma*sigma)/(0.7413*0.7413)) );
  --(preL = L);

  for (unsigned int r = 0; r < nRepeat; r++)
  {
    curIt = F;
    ++(nxtIt = F);

    pre.first  = (float)((*(bClosed ? preL : F)).x);
    pre.second = (float)((*(bClosed ? preL : F)).y);

    cur.first  = (float)((*curIt).x);
    cur.second = (float)((*curIt).y);
    headPoint = cur;

    while (curIt != L)
    {
      if (nxtIt == L)
      {
        nxtIt = curIt;
        nxt = bClosed ? headPoint : cur;
      }
      else
      {
        nxt.first  = (float)((*nxtIt).x);
        nxt.second = (float)((*nxtIt).y);
      }

      (*curIt).x = (float)((pre.first  + cur.first  + cur.first  + nxt.first ) * 0.25);
      (*curIt).y = (float)((pre.second + cur.second + cur.second + nxt.second) * 0.25);

      pre = cur;
      cur = nxt;
      ++curIt;
      ++nxtIt;
    }
  }
}


//-------------------------------------------------------------------------------------------------
/**
  \brief Function convolves curve with symmetric filter. Filter is represented by its half:
         { filter[1],...,filter[filtSize-1] } and central element: filter[0].

  \param first       begin iterator of a curve.
  \param last        end iterator of a curve.
  \param bClosed     TRUE for enclosed curve.
  \param filter      filter to be applied.
  \param filtSize    length of filter array.
*/
//-------------------------------------------------------------------------------------------------
template<class InIt>
void SmoothCurve(
  InIt          first,
  InIt          last,
  bool          bClosed,
  const float * filter,
  int           filtSize)
{
  int N = std::distance( first, last );
  if (N == 0)
    return;
  ASSERT( (*first).integer() == false );
 
  int     L = N-1;
  int     nN = (2+filtSize/N)*N;
  InIt    it = first;
  void  * pMem = malloc( 2*N*sizeof(float) );
  float * x = reinterpret_cast<float*>( pMem );
  float * y = x + N;
  int     k;

  // Copy input curve (which could be represented by list, for example) into temporary arrays.
  for(k = 0; k < N; k++)
  {
    x[k] = (float)((*it).x);
    y[k] = (float)((*it).y);
    it++;
  }
  ASSERT( it == last );

  // Convolve temporary curve with filter and store result in the specified one.
  it = first;
  for(k = 0; k < N; k++)
  {
    double sx = x[k] * filter[0];
    double sy = y[k] * filter[0];

    if (bClosed)
    {
      for(int i = 1; i < filtSize; i++)
      {
        int p = (k-i+nN) % N;
        int q = (k+i) % N;
        sx += (x[q] + x[p]) * filter[i];
        sy += (y[q] + y[p]) * filter[i];
      }
    }
    else // !bClosed
    {
      for(int i = 1; i < filtSize; i++)
      {
        int p = (k-i < 0) ? 0 : k-i;
        int q = (k+i > L) ? L : k+i;
        sx += (x[q] + x[p]) * filter[i];
        sy += (y[q] + y[p]) * filter[i];
      }
    }

    (*it).x = (float)sx;
    (*it).y = (float)sy;
    it++;
  }
  free( pMem );
  ASSERT( it == last );
}


//-------------------------------------------------------------------------------------------------
/**
  \brief Function convolves curve with antisymmetric filter. Filter is represented by its half:
         { filter[1],...,filter[filtSize-1] } and central element: filter[0] == 0.

  \param first       begin iterator of a curve.
  \param last        end iterator of a curve.
  \param bClosed     TRUE for enclosed curve.
  \param filter      filter to be applied.
  \param filtSize    length of filter array.
*/
//-------------------------------------------------------------------------------------------------
template<class InIt>
void DifferentiateCurve(
  InIt          first,
  InIt          last,
  bool          bClosed,
  const float * filter,
  int           filtSize)
{
  int N = std::distance( first, last );
  if (N == 0)
    return;
  ASSERT( (*first).integer() == false );
 
  int     L = N-1;
  int     nN = (2+filtSize/N)*N;
  InIt    it = first;
  void  * pMem = malloc( 2*N*sizeof(float) );
  float * x = reinterpret_cast<float*>( pMem );
  float * y = x + N;
  int     k;

  // Copy input curve (which could be represented by list, for example) into temporary arrays.
  for(k = 0; k < N; k++)
  {
    x[k] = (float)((*it).x);
    y[k] = (float)((*it).y);
    it++;
  }
  ASSERT( it == last );

  // Convolve temporary curve with filter and store result in the specified one.
  it = first;
  for(k = 0; k < N; k++)
  {
    double sx = 0.0;
    double sy = 0.0;

    if (bClosed)
    {
      for(int i = 1; i < filtSize; i++)
      {
        int p = (k-i+nN) % N;
        int q = (k+i) % N;
        sx += (x[q] - x[p]) * filter[i];
        sy += (y[q] - y[p]) * filter[i];
      }
    }
    else // !bClosed
    {
      for(int i = 1; i < filtSize; i++)
      {
        int p = (k-i < 0) ? 0 : k-i;
        int q = (k+i > L) ? L : k+i;
        sx += (x[q] - x[p]) * filter[i];
        sy += (y[q] - y[p]) * filter[i];
      }
    }

    (*it).x = (float)sx;
    (*it).y = (float)sy;
    it++;
  }
  free( pMem );
  ASSERT( it == last );
}


//-------------------------------------------------------------------------------------------------
// Function resamples curve with specified step.
// 1. Input and output points have to have the fields (x,y).
// 2. Coordinates of output points should be of any float type.
//-------------------------------------------------------------------------------------------------
template<class CONTAINER>
void ResampleCurve(
  CONTAINER & curve,         // in: source curve,  out: resampled curve
  double      step,          // desired step of resample
  bool        bClosed)       // TRUE for enclosed input curve
{
  step = std::max(step, 0.01);

  int N = (int)(curve.size());
  if (N == 0 || N == 1)
    return;
  if (N == 2)
    bClosed = false;
  N = bClosed ? (N+1) : N;

  typename CONTAINER::iterator it, endIt;
  int                          i, k, n, nStep, nPoint;
  std::vector<Vec2f>           tmpCurveStorage( N );
  std::vector<double>          arcLengStorage( N );
  Vec2f *                      tmpCurve = &(*(tmpCurveStorage.begin()));
  double *                     arcLeng = &(*(arcLengStorage.begin()));
  double                       t, length, x, y, dx, dy;

  // Compute arc-length for each curve's point and store point coordinates into temporary curve.
  it = curve.begin();
  endIt = curve.end();
  x = (*it).x;
  y = (*it).y;
  length = 0.0;
  for(k = 0; it != endIt; it++)
  {
    dx = ( tmpCurve[k].x = (*it).x ) - x;
    dy = ( tmpCurve[k].y = (*it).y ) - y;
    arcLeng[k] = ( length += sqrt(dx*dx + dy*dy) );
    x = tmpCurve[k].x;
    y = tmpCurve[k].y;
    k++;
  }
  if (bClosed)
  {
    it = curve.begin();
    dx = ( tmpCurve[k].x = (*it).x ) - x;
    dy = ( tmpCurve[k].y = (*it).y ) - y;
    arcLeng[k] = ( length += sqrt(dx*dx + dy*dy) );
    k++;
  }
  ASSERT(k == N);

  // If curve is too short, then quit.
  if (length < step)
  {
    curve.resize(1);
    return;
  }

  // Adjust step value and resize curve.
  nStep = 1 + (int)(length/step);
  step = length/nStep;
  nPoint = bClosed ? nStep : (nStep+1);
  curve.clear();
  curve.resize(nPoint);

  // Perform resampling.
  it = curve.begin();
  endIt = curve.end();
  t = 0.0;
  ASSERT((*it).integer() == false);

  for(n = 0, k = 1; k < N; k++)
  {
    i = (k == N-1) ? nStep : ((int)floor(arcLeng[k]/step));

    for(; n <= i && n < nPoint; n++)
    {
      double s1 = arcLeng[k-1];
      double s2 = arcLeng[k];
      double coef = ((s2-s1) > (FLT_MIN/FLT_EPSILON)) ? ((t-s1)/(s2-s1)) : 0.0;

      ASSERT(it != endIt);
      (*it).x = (float)(tmpCurve[k-1].x + (tmpCurve[k].x - tmpCurve[k-1].x)*coef);
      (*it).y = (float)(tmpCurve[k-1].y + (tmpCurve[k].y - tmpCurve[k-1].y)*coef);
      it++;
      t += step;
    }
  }
  ASSERT(k == N && n == nPoint && it == endIt);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes in-place the tangents at curve's points.

  Optionally function smoothes resultant tangents and/or normalizes them.

  TODO: the last tangent (without normalization) could be shorter than others.

  \param  F              begin iterator of the input curve.
  \param  L              end iterator of the input curve.
  \param  bClosed        non-zero for enclosed input curve.
  \param  nSmoothRepeat  the number of smoothing repetitions.
  \param  bNormalize     if non-zero, then tangent vectors will be normalized. */
//-------------------------------------------------------------------------------------------------
template< class InIt >
void CalcTangentsInPlace(
  InIt  F,
  InIt  L,
  bool  bClosed,
  int   nSmoothRepeat = 1,
  bool  bNormalize = true )
{
  if (F == L)
    return;

  std::pair<float,float>  pre, cur, nxt, headPoint;
  InIt                    curIt, nxtIt, preL;

  --(preL = L);
  curIt = F;
  ++(nxtIt = F);

  pre.first  = (float)((*(bClosed ? preL : F)).x);
  pre.second = (float)((*(bClosed ? preL : F)).y);

  cur.first  = (float)((*F).x);
  cur.second = (float)((*F).y);
  headPoint = cur;

  // Compute tangent at each curve point.
  while (curIt != L)
  {
    if (nxtIt == L)
    {
      nxtIt = curIt;
      nxt = bClosed ? headPoint : cur;
    }
    else
    {
      nxt.first  = (float)((*nxtIt).x);
      nxt.second = (float)((*nxtIt).y);
    }

    (*curIt).x = (float)(nxt.first  - pre.first );
    (*curIt).y = (float)(nxt.second - pre.second);

    pre = cur;
    cur = nxt;
    ++curIt;
    ++nxtIt;
  }

  // Repeatedly smooth resultant tangents.
  if (nSmoothRepeat > 0)
  {
    RepeatedlySmoothCurve( F, L, bClosed, nSmoothRepeat );
  }

  // Normalize each tangent vector.
  if (bNormalize)
  {
    for (; F != L; ++F)
    {
      double x = (*F).x;
      double y = (*F).y;
      double scale = (x*x + y*y);

      scale = (scale > (FLT_MIN/FLT_EPSILON)) ? sqrt( 1.0/scale ) : 0.0;
      (*F).x = (float)(x * scale);
      (*F).y = (float)(y * scale);
    }
  }
}


//--------------------------------------------------------------------------
// Function checks curve's enclosure. Enclosed == true.
// N O T E: Each curve's point should have pair of coordinates (x,y) of
// any INTEGER type (i.e. valid cast to 'int' should be possible).
//--------------------------------------------------------------------------
template<class InIt>
inline bool IsEnclosedCurve(InIt first, InIt last)
{
  if (std::distance( first, last ) < 3)
    return true;
  ASSERT( (*first).integer() );

  --last;
  int nei = ALIB_IS_NEIGHBOR2((*first).x, (*first).y, (*last).x, (*last).y);
  ASSERT(nei >= 0);
  return (nei > 0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function finds nearest curve's point for the specified query one.

  \param  F  begin iterator of the curve.
  \param  L  end iterator of the curve.
  \param  x  x-coordinate of the query point.
  \param  y  y-coordinate of the query point.
  \return    iterator of the nearest point or "L" for empty sequence. */
//-------------------------------------------------------------------------------------------------
template< class InIt, class COORD >
InIt FindNearestPoint(InIt F, InIt L, COORD x, COORD y)
{
  double min = DBL_MAX;
  InIt   result = L;

  for(; F != L; ++F)
  {
    double dx = (double)((*F).x - x);
    double dy = (double)((*F).y - y);
    double d2 = dx*dx + dy*dy;

    if (min > d2)
    {
      min = d2;
      result = F;
    }
  }
  return result;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function finds the most distant curve's point from the specified query one.

  \param  F  begin iterator of the curve.
  \param  L  end iterator of the curve.
  \param  x  x-coordinate of the query point.
  \param  y  y-coordinate of the query point.
  \return    iterator of the most distant point or "L" for empty sequence. */
//-------------------------------------------------------------------------------------------------
template< class InIt, class COORD >
InIt FindDistantPoint(InIt F, InIt L, COORD x, COORD y)
{
  double max = 0.0;
  InIt   result = L;

  for(; F != L; ++F)
  {
    double dx = (double)((*F).x - x);
    double dy = (double)((*F).y - y);
    double d2 = dx*dx + dy*dy;

    if (max < d2)
    {
      max = d2;
      result = F;
    }
  }
  return result;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes rectangle that bounds specified point set.

  Resultant rectangle limits set's points either to semi-opened intervals
  (x1 <= x < x2) and (y1 <= y < y2) or closed ones (x1 <= x <= x2) and (y1 <= y <= y2),
  depending on the flag 'bSemiOpen', where (x1,y1) and (x2,y2) are rectangle's corners.

  \param  F          begin iterator of the point set.
  \param  L          end iterator of the point set.
  \param  coordType  any value that tells what is the type of point coordinates.
  \param  bSemiOpen  if 'true', then bounding intervals will be semi-opened.
  \return            the bounding rectangle of the point set. */
//-------------------------------------------------------------------------------------------------
template< class InIt, class COORDINATE >
ARect GetBoundRect( InIt F, InIt L, COORDINATE /*coordType*/, bool bSemiOpen = true )
{
  ASSERT( std::numeric_limits<COORDINATE>::is_specialized );

  ARect rect;
  InIt  ix1 = F, iy1 = F, ix2 = F, iy2 = F;
  int   a = bSemiOpen ? 1 : 0;

  if (F == L)
    return rect;

  for (++F; F != L; ++F)
  {
    if ((*ix1).x > (*F).x) ix1 = F;
    if ((*iy1).y > (*F).y) iy1 = F;

    if ((*ix2).x < (*F).x) ix2 = F;
    if ((*iy2).y < (*F).y) iy2 = F;
  }

  if (std::numeric_limits<COORDINATE>::is_integer)
  {
    rect.set(   (int)((*ix1).x),   (int)((*iy1).y),
              a+(int)((*ix2).x), a+(int)((*iy2).y) );
  }
  else
  {
    rect.set(  (int)floor( (double)((*ix1).x) ),  (int)floor( (double)((*iy1).y) ),
              a+(int)ceil( (double)((*ix2).x) ), a+(int)ceil( (double)((*iy2).y) ) );
  }
  return rect;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function roughly enforces points of a set to lie within bounding rectangle:
           [x1,x2] and [y1,y2].

  \param  F   begin iterator of the point set.
  \param  L   end iterator of the point set.
  \param  x1  minimal x-margin.
  \param  y1  minimal y-margin.
  \param  x2  maximal x-margin.
  \param  y2  maximal y-margin. */
//-------------------------------------------------------------------------------------------------
template< class InIt, class COORD >
void BoundLocation( InIt F, InIt L, COORD x1, COORD y1, COORD x2, COORD y2 )
{
  ASSERT( x1 <= x2 );
  ASSERT( y1 <= y2 );

  for (; F != L; ++F)
  {
    if ((*F).x < x1) (*F).x = x1; else if ((*F).x > x2) (*F).x = x2;
    if ((*F).y < y1) (*F).y = y1; else if ((*F).y > y2) (*F).y = y2;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function finds mean point of the specified point set.

  \param  F  begin iterator of the point set.
  \param  L  end iterator of the point set.
  \return    mean point. */
//-------------------------------------------------------------------------------------------------
template<class InIt>
Vec2d MeanPoint (InIt F, InIt L)
{
  Vec2d  meanPt(0.0, 0.0);
  int    N = 0;

  if (F != L)
  {
    while(F != L)
    {
      meanPt.x += (*F).x;
      meanPt.y += (*F).y;
      ++N;
      ++F;
    }
    meanPt.x /= N;
    meanPt.y /= N;
  }
  return meanPt;
}


//-------------------------------------------------------------------------------------------------
// Functions return index (iterator) of the point on the curve, or -1 (end iterator) if the point
// does not lay on the curve. Function is applicable only for the curves with integer coordinates.
// 1. Point (*InIt) has to have fields x and y.
// 2. Coordinates x and y should have integer, signed type (e.g. short, int).
//-------------------------------------------------------------------------------------------------
template<class InIt>
int GetPointIndex(InIt first, InIt last, int x, int y)
{
  ASSERT((*first).integer());

  int k = 0;
  for(InIt it = first; it != last; it++)
  {
    if ((int)((*it).x) == x && (int)((*it).y) == y)
      return k;
    k++;
  }
  return -1;
}


template<class InIt>
InIt GetPointIter(InIt first, InIt last, int x, int y)
{
  ASSERT((*first).integer());

  for(InIt it = first; it != last; it++)
  {
    if ((int)((*it).x) == x && (int)((*it).y) == y)
      return it;
  }
  return last;
}


//-------------------------------------------------------------------------------------------------
/** Function calculates the area of enclosed 2D curve.

  No precautions are made against self-intersecting curves. The sign of area depends on
  curve's orientation: the area is positive for counter-clockwise curve orientation and
  negative otherwise.

  \param  F  begin iterator of the curve.
  \param  L  end iterator of the curve.
  \return    curve's area. */
//-------------------------------------------------------------------------------------------------
template< class InIt >
double CurveArea( InIt F, InIt L )
{
  if (F == L)
    return 0.0;

  double area = 0.0;
  InIt   prev = L;

  for (--prev; F != L; ++F)
  {
    area += (double)((*prev).x) * (double)((*F).y) - (double)((*prev).y) * (double)((*F).x);
    prev = F;
  }
  return (0.5*area);
}


//-------------------------------------------------------------------------------------------------
/** Function ascertains relation between enclosed 2D curve and a point.

  Though the function does not implement the best optimality, it is quite universal.
  The curve can be of any orientation. It is always treated as enclosed curve.

  \param  F        begin iterator of the curve.
  \param  L        end iterator of the curve.
  \param  x        point's abscissa.
  \param  y        point's ordinate.
  \param  epsilon  the minimal distance between a pair of distinguishable points (must be non-zero).
  \return          +1 - a point lies inside internal curve's area,
                    0 - a point lies on the curve,
                   -1 - a point lies outside the curve. */
//-------------------------------------------------------------------------------------------------
template< class InIt, class COORD >
int CurveAndPointRelation( InIt F, InIt L, COORD x, COORD y, double epsilon = FLT_EPSILON )
{
  ASSERT( epsilon > (DBL_MIN/DBL_EPSILON) );
  if (F == L)
    return -1;

  const double PI = 3.1415926535897932384626433832795;
  double       angle = 0.0;
  InIt         prev = L;  --prev;
  double       preDx = (double)((*prev).x) - (double)x;
  double       preDy = (double)((*prev).y) - (double)y;
  double       preLeng = sqrt( preDx*preDx + preDy*preDy );

  if (preLeng < epsilon)
    return 0;

  // Accumulate the angle between successive vectors while moving along the curve.
  // Vectors are counted off from the query point.
  for (; F != L; ++F)
  {
    double curDx = (double)((*F).x) - (double)x;
    double curDy = (double)((*F).y) - (double)y;
    double curLeng = sqrt( curDx*curDx + curDy*curDy );

    if (curLeng < epsilon)
      return 0;

    double s = (preDx*curDy - preDy*curDx)/(preLeng*curLeng);
    angle += asin( ALIB_LIMIT( s, -1.0, +1.0 ) );

    preDx = curDx;
    preDy = curDy;
    preLeng = curLeng;
  }
  angle = fabs( angle );     // a curve could be a clockwise oriented one

  // angle = 2*PI - inside, angle = 0 - outside, angle = PI - on the curve.
  return (angle > (3.0*PI/2.0)) ? +1 : ((angle < (PI/2.0)) ? -1 : 0);
}


//-------------------------------------------------------------------------------------------------
/** Function checks orientation of enclosed 2D curve.

  It's assumed that thin curve (e.g. enclosed curve that occupies one-pixel width
  horizontal strip) has counter-clockwise orientation.

  \param  F  begin iterator of the curve.
  \param  L  end iterator of the curve.
  \return    counter-clockwise orientation: +1, clockwise orientation: -1, empty curve: 0. */
//-------------------------------------------------------------------------------------------------
template<class InIt>
int CurveOrientation(InIt F, InIt L)
{
  return ((F == L) ? 0 : ((CurveArea( F, L ) >= 0.0) ? +1 : -1));
}


//--------------------------------------------------------------------------
// Function checks the curve for self intersection.
// 1. Point (*InIt) has to have fields x and y.
// 2. Coordinates x and y should have integer, signed type (e.g. short, int).
//--------------------------------------------------------------------------
template<class InIt>
bool IsSelfIntersect(InIt it, InIt endIt)
{
  ASSERT((*it).integer());

  for(InIt p = it; p != endIt; p++)
  {
    for(InIt q = p+1; q != endIt; q++)
    {
      if ((*p).x == (*q).x && (*p).y == (*q).y)
        return true;
    }
  }
  return false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function shifts curve's point over specified vector.

  \param  F   begin iterator of the curve.
  \param  L   end iterator of the curve.
  \param  dx  x-movement.
  \param  dy  y-movement. */
//-------------------------------------------------------------------------------------------------
template< class InIt, class COORD >
void MoveCurve( InIt F, InIt L, COORD dx, COORD dy )
{
  for (; F != L; ++F)
  {
    (*F).x += dx;
    (*F).y += dy;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function checks validity of specified curve.

  <b>Definition:</b> valid curve doesn't contain gaps or coincided sequential points.<br>
  Coordinates (x,y) of curve points must be of signed, integer type.

  \param  F  begin iterator of the curve.
  \param  L  end iterator of the curve.
  \return    Ok = true. */
//-------------------------------------------------------------------------------------------------
template< class InIt >
bool IsValidCurve(InIt F, InIt L)
{
  if (F == L)
    return true;
  ASSERT( (*F).integer() );
  
  InIt  pre = L;  --pre;
  int   nei = ALIB_IS_NEIGHBOR2( (*F).x, (*F).y, (*pre).x, (*pre).y );

  if (nei < 0)
    return false;       // first and last points coincide, invalid curve
  else if (nei == 0)
    pre = F++;          // first and last points are not neighbours, open curve
  else
    pre;                // first and last points are neighbours, enclosed curve

  while(F != L)
  {
    if (ALIB_IS_NEIGHBOR2( (*F).x, (*F).y, (*pre).x, (*pre).y ) <= 0)
      return false;
    pre = F;
    ++F;
  }
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function processes input curve and removes coincided sequential points and fills
           gaps between points with straight lines.

  Note: <OL>
  <LI> While filling gaps function inserts points created by default constructor of curve's
       point. Only coordinates x and y will be initialized. </LI>
  <LI> Curve's point has to have fields x and y. </LI>
  <LI> Coordinates x and y should have integer, signed type (e.g. 'short', 'int'). </LI> </OL>

  \param  curve   in: sequence to be processed, out: validated curve.
  \param bClosed  flag is TRUE for enclosed curve and FALSE otherwise. */
//-------------------------------------------------------------------------------------------------
template< class CONTAINER >
void ValidateCurve( CONTAINER & curve, bool bClosed )
{
  typedef  typename CONTAINER::value_type  value_type;
  typedef  typename CONTAINER::iterator    iterator;

  if (curve.empty())
    return;

  iterator      pre = curve.end(), it = curve.begin();
  GoAlongLine   gal(false);
  Vec2i         a, b;
  const Vec2i * p;

  ASSERT( (*it).integer() == true );

  --pre;
  if (!bClosed && ALIB_IS_NEIGHBOR2((*it).x, (*it).y, (*pre).x, (*pre).y) == 0)
    pre = it++;    // unclosed curve

  while (it != curve.end())
  {
    int neighbourhood = ALIB_IS_NEIGHBOR2((*it).x, (*it).y, (*pre).x, (*pre).y);

    if (neighbourhood > 0)
    { // continue
      pre = it;
      ++it;
    }
    else if (neighbourhood < 0)
    { // remove overlapped points
      it = curve.erase(it);
    }
    else // neighbourhood == 0
    { // fill the gap with straight line
      a.set( (int)((*pre).x), (int)((*pre).y) );
      b.set( (int)( (*it).x), (int)( (*it).y) );
      p = gal.first(a, b);

      for(p = gal.next(); p != 0; p = gal.next())
      {
        it = curve.insert( it, value_type() );
        (*it).x = p->x;
        (*it).y = p->y;
        it++;
      }
      pre = it;
      ++it;
    }
  }

  ASSERT( vislib::IsValidCurve( curve.begin(), curve.end() ) );
}


//-------------------------------------------------------------------------------------------------
// Function returns absolute difference between index1 (i1) and index2 (i2).
// Result depends on curve's enclosure and path's direction.
//-------------------------------------------------------------------------------------------------
inline int IndexDistance(
  int  i1,              // first index
  int  i2,              // second index
  int  size,            // curve's size
  bool bPositiveDir,    // path direction: from i1 to i2 or vice versa
  bool bClosed)         // flag of curve's enclosure
{
  ASSERT(size > 0 && ALIB_IS_RANGE(i1, 0, size) && ALIB_IS_RANGE(i2, 0, size));

  if (bClosed)
  {
    return ( ( (bPositiveDir ? (i2-i1) : (i1-i2)) + size ) % size );
  }
  else
  {
    return abs(i2-i1);
  }
}


//-------------------------------------------------------------------------------------------------
// Function returns middle index between index1 (i1) and index2 (i2).
// Result depends on curve's enclosure and path's direction.
//-------------------------------------------------------------------------------------------------
inline int MiddleIndex(
  int  i1,              // first index
  int  i2,              // second index
  int  size,            // curve's size
  bool bPositiveDir,    // path direction: from i1 to i2 or vice versa
  bool bClosed)         // flag of curve's enclosure
{
  ASSERT(size > 0 && ALIB_IS_RANGE(i1, 0, size) && ALIB_IS_RANGE(i2, 0, size));

  if (bClosed)
  {
    if (bPositiveDir)
    {
      return (i2 >= i1) ? ((i1+i2)>>1) : (((i1+i2+size)>>1) % size);
    }
    else
    {
      return (i2 <= i1) ? ((i1+i2)>>1) : (((i1+i2+size)>>1) % size);
    }
  }
  else
  {
    return ((i1+i2)>>1);
  }
}


//-------------------------------------------------------------------------------------------------
// Function checks whether index lies inside semi-opened interval [index1,index2).
// Result depends on curve's enclosure.
//-------------------------------------------------------------------------------------------------
inline bool IsIndexInside(
  int  i,               // index to be checked
  int  i1,              // left bound [i1,....
  int  i2,              // right bound    ....,i2)
  int  size,            // curve's size
  bool bClosed)         // flag of curve's enclosure
{
  ASSERT(size > 0 && ALIB_IS_RANGE(i1, 0, size) && ALIB_IS_RANGE(i2, 0, size));

  if (bClosed)
  {
    if (i1 < i2)
    {
      return ALIB_IS_RANGE(i, i1, i2);
    }
    else
    {
      return (ALIB_IS_RANGE(i, i1, size) || ALIB_IS_RANGE(i, 0, i2));
    }
  }
  else
  {
    ASSERT(i1 < i2);
    return ALIB_IS_RANGE(i, i1, i2);
  }
}


//-------------------------------------------------------------------------------------------------
// Function polygonizes 2D input curve. Function recursively splits curve at the points where
// deviation, from the chord connecting arc's end points, reaches maximum. Those points are
// related as polygonal points. The input curve [first,last) should be defined as array of points
// which have at least two fields 'x' and 'y'. Returns: number of polygonal points.
//-------------------------------------------------------------------------------------------------
template<class InIt>
int SimplePolygonization(
  InIt               first,        //!< iterator of the first curve's point
  InIt               last,         //!< iterator of the next curve's point after last one
  bool               bClosed,      //!< TRUE for enclosed curve
  bool               bSplitAlways, //!< split chord even if deviation is small (at the middle point)
  double             minChord,     //!< threshold on minimal chord length
  double             deviation,    //!< threshold on maximal deviation from poly-line
  std::vector<int> & indices)      //!< out: array of indices of found polygonal points
{
  typedef  std::pair<int,int>   IIPair;
  typedef  std::vector<IIPair>  IIPairArr;

  indices.clear();
  int N = std::distance(first, last);
  if (N == 0)
    return 0;

  minChord = ALIB_LIMIT(minChord, 3.0, 30.0);
  deviation = ALIB_LIMIT(deviation, 1.0, 10.0);

  IIPairArr  arcStack;                           // stack of arcs
  int        iniIndex1 = 0;                      // first initial splitting index
  int        iniIndex2 = N-1;                    // second initial splitting index
  double     minSqChord = ALIB_SQR(minChord);
  double     sqDeviation = ALIB_SQR(deviation);

  arcStack.reserve( (int)(N/minChord+10.0) );
  indices.reserve( arcStack.capacity() );

  // Generate first couple of polygonal points.
  if (bClosed)
  {
    // Find mean point of the points set.
    Vec2d mean = vislib::MeanPoint(first, last);

    // Find point that is the most distant from the middle point.
    InIt it = vislib::FindDistantPoint(first, last, mean.x, mean.y);
    iniIndex1 = std::distance(first, it);

    // Find point that is the most distant from the first found point.
    it = vislib::FindDistantPoint( first, last, (*it).x, (*it).y );
    iniIndex2 = std::distance( first, it );

    arcStack.push_back( IIPair(iniIndex2, iniIndex1) );
  }
  arcStack.push_back( IIPair(iniIndex1, iniIndex2) );
  indices.push_back( iniIndex1 );
  indices.push_back( iniIndex2 );

  // Split the arcs between adjacent poly-points pairs.
  while( !arcStack.empty() )
  {
    IIPair ii = arcStack.back();
    arcStack.pop_back();

    int index1 = ii.first;
    int index2 = ii.second;
    ASSERT( ALIB_IS_RANGE(index1,0,N) && ALIB_IS_RANGE(index2,0,N) );

    InIt it = first + index1;
    double x1 = (*it).x;
    double y1 = (*it).y;

    it = first + index2;
    double chord_x = (*it).x - x1;
    double chord_y = (*it).y - y1;

    double sqChord = ALIB_SQR(chord_x) + ALIB_SQR(chord_y);
    int    newInd = -1;
    double max = 0.0;

    // If chord length is too small, then stop splitting of the current arc.
    if (sqChord < minSqChord)
      continue;

    // Find maximum deviation between arc's ends.
    ASSERT( bClosed || index1 <= index2 );
    for(int i = index1; i != index2; i = (i+1)%N)
    {
      it = first + i;
      double dx = (*it).x - x1;
      double dy = (*it).y - y1;
      double dist = fabs( dy*chord_x - dx*chord_y );

      if (max < dist)
      {
        max = dist;
        newInd = i;
      }
    }

    // If deviation is too small, then stop splitting or split at the middle point.
    if ( newInd < 0 || ALIB_SQR(max) < sqDeviation*sqChord )
    {
      if (bSplitAlways)
        newInd = vislib::MiddleIndex( index1, index2, N, true, bClosed );
      else
        continue;
    }

    // Newly found salient point splits this arc into two ones.
    ii.first  = index1;
    ii.second = newInd;
    arcStack.push_back(ii);
    indices.push_back(newInd);
    ii.first  = newInd;
    ii.second = index2;
    arcStack.push_back(ii);
  }

  // Sort indices and check index uniqueness.
  std::sort( indices.begin(), indices.end() );
  ASSERT( std::adjacent_find( indices.begin(), indices.end() ) == indices.end() );
  return (indices.size());
}


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates the length of a 2D-curve.

  \param  F        begin iterator of the curve.
  \param  L        end iterator of the curve.
  \param  bClosed  non-zero for enclosed curve.
  \return          curve's length. */
//-------------------------------------------------------------------------------------------------
template< class InIt >
double CurveLength( InIt F, InIt L, bool bClosed )
{
  if (F == L)
    return 0.0;

  InIt   pre;
  double length = 0.0;

  if (bClosed)
    --(pre = L);
  else
    pre = F++;

  while (F != L)
  {
    double dx = (double)((*F).x) - (double)((*pre).x);
    double dy = (double)((*F).y) - (double)((*pre).y);
    length += sqrt( dx*dx + dy*dy );
    pre = F++;
  }
  return length;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates arclength positions of curve's points.

  The size of array of arclength values must be equal to the number of curve points for an open
  curve, and must be greater by 1 for cyclically enclosed one. In the latter case the last entry
  containes the total arclength of enclosed curve, including the segment that connects end points.

  \param  first    begin iterator of the curve.
  \param  last     end iterator of the curve.
  \param  bClosed  non-zero for enclosed curve.
  \param  F        begin iterator of arclength values (values are of any 'float' type).
  \param  L        end iterator of arclength values (values are of any 'float' type).
  \return          curve's length. */
//-------------------------------------------------------------------------------------------------
template< class InIt, class OutIt >
double CalcArclengthPositions( InIt first, InIt last, bool bClosed, OutIt F, OutIt L )
{
  ASSERT( (std::distance( first, last ) + (bClosed ? 1 : 0)) == std::distance( F, L ) );

  if (first == last)
    return (bClosed ? (double)((*F) = (float)0.0) : (double)0.0);

  InIt   pre = first, cur = first;
  double length = 0.0;

  (*F++) = (float)0.0;   // the first point locates at zero arclength
  ++cur;
  while (F != L)
  {
    if (cur == last)
      cur = first;      // enclosed curve

    double dx = (double)((*cur).x) - (double)((*pre).x);
    double dy = (double)((*cur).y) - (double)((*pre).y);

    length += sqrt( dx*dx + dy*dy );
    (*F++) = (float)length;
    pre = cur++;
  }
  return length;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function produces uniformly sampled arclength positions.

  If (F == L) then function does nothing and returns the number of samples, consequently,
  one could query required space of output storage simply choosing F = L.

  \param  length   curve's length.
  \param  step     desired resampling step.
  \param  F        begin iterator of new arclength values (values are of any 'float' type).
  \param  L        end iterator of new arclength values (values are of any 'float' type).
  \return          the number of samples. */
//-------------------------------------------------------------------------------------------------
template< class OutIt >
int CalcUniformArclengthPositions( double length, double step, OutIt F, OutIt L )
{
  ASSERT( step > FLT_EPSILON );

  int     N = (int)floor( length/step+0.5 );
  int     S = N+1;
  double  sum = 0.0;

  if (F == L)
    return S;
  ASSERT( std::distance( F, L ) == S );

  step = (N > 0) ? (length/N) : 0.0;
  while (F != L)
  {
    (*F) = (float)sum;
    sum += step;
    ++F;
  }

  ASSERT( (F == L) && (fabs( length-sum ) < (10.0*FLT_EPSILON)*length) );
  (*(--F)) = (float)length;  // ensure that the last sample locates at the full-length arclength position
  return S;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function resamples sequence of values.

  <p> Function resamples a sequence of values [vF,vL), which are defined at the discrete arclength
  positions [aF,aL), into new samples [rF,rL) defined at the discrete arclength positions [bF,bL).
  </p><p>
  The arclengthes of both series [aF,aL) and [bF,bL) must coincide at the end positions:
  (*aF)==(*bF) and (*(aL-1))==(*(bL-1)).
  </p><p>
  The number of entries in the sets [aF,aL), [bF,bL) must be equal to the number of entries
  in the sets [vF,vL), [rF,rL) respectively for an open curve, and must be greater by 1 for
  cyclically enclosed one. In the latter case the last entry containes the total arclength of
  enclosed curve, including the segment that connects two end points.
  </p><p>
  The function carries out linear interpolation while resampling. Nevertheless, it can be easily
  extended for other types of iterpolation.</p>

  \param  vF            begin iterator of the input set of values to be resampled.
  \param  vL            end iterator of the input set of values to be resampled.
  \param  aF            begin iterator of arclength positions of the input set.
  \param  aL            end iterator of arclength positions of the input set.
  \param  rF            begin iterator of the output set of resampled values.
  \param  rL            end iterator of the output set of resampled values.
  \param  bF            begin iterator of desired arclength positions of the output set.
  \param  bL            end iterator of desired arclength positions of the output set.
  \param  bClosed       non-zero for enclosed.
  \param  interpolator  function-object that interpolates between two successive samples. */
//-------------------------------------------------------------------------------------------------
template< class InValIt, class InArcIt, class OutValIt, class OutArcIt, class PRED >
void Resample( InValIt  vF, InValIt  vL,
               InArcIt  aF, InArcIt  aL,
               OutValIt rF, OutValIt rL,
               OutArcIt bF, OutArcIt bL, bool bClosed, PRED interpolator )
{
  ASSERT( (std::distance( vF, vL ) + (bClosed ? 1 : 0)) == std::distance( aF, aL ) );
  ASSERT( (std::distance( rF, rL ) + (bClosed ? 1 : 0)) == std::distance( bF, bL ) );
  if (vF == vL)
    return;

  InValIt  v1 = vF, v2 = vF;
  InArcIt  s1 = aF, s2 = aF, aPreL = aL;  --aPreL;

  bL;bClosed;
  ASSERT( ((*aF) == (*bF)) && ((*aPreL) == (*(--bL))) );

  while (s2 != aL)
  {
    if (v2 == vL)
      v2 = vF;     // enclosed curve

    for (; (rF != rL) && ((s2 == aPreL) || ((*bF) <= (*s2))); ++bF)
    {
      (*rF++) = interpolator( (*v1), (*v2), (*bF), (*s1), (*s2) );
    }
    s1 = s2++;
    v1 = v2++;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function convolves curve with symmetric or antisymmetric filter.

  Filter is represented by its half: { filter[1],...,filter[filtSize-1] } and central element:
  filter[0]. <BR>
  If curve points are not equally spaced alongside the curve, then filtering maybe very inaccurate.
  To increase accuracy the following procedures are made: (1) resample curve with one-pixel step,
  (2) apply filter to the resampled curve, (3) back-resample curve.

  \param  first     begin iterator of a curve.
  \param  last      end iterator of a curve.
  \param  bClosed   TRUE for enclosed curve.
  \param  bSmooth   if TRUE then apply symmetric (smoothing) filter, otherwise antisymmetric one.
  \param  filter    filter to be applied.
  \param  filtSize  length of filter array. */
//-------------------------------------------------------------------------------------------------
template< class InIt >
void ConvolveCurveWithResampling(
  InIt          first,
  InIt          last,
  bool          bClosed,
  bool          bSmooth,
  const float * filter,
  int           filtSize)
{
  if (first == last || filtSize < 2)
    return;

  double length = CalcCurveLength( first, last, bClosed );
  int    P1 = std::distance( first, last );
  int    S1 = P1 + (bClosed ? 1 : 0);
  int    P2 = (int)(length+0.5) + 1;
  int    S2 = P2 + (bClosed ? 1 : 0);

  if (P2 < 2)
    return;

  void  * pMem = malloc( S1*sizeof(float) + S2*sizeof(float) + P2*sizeof(Vec2f) );
  float * arc1 = reinterpret_cast<float*>( pMem );
  float * arc2 = arc1 + S1;
  Vec2f * temp = reinterpret_cast<Vec2f*>( (arc2+S2) );
  double  sum = 0.0, step = length/(S2-1);

  Interpolate2D<Vec2f>  interpolator;

  CalcArclengthPositions( first, last, bClosed, arc1, arc1+S1 );
  ASSERT( arc1[0] == 0.0f && fabs(length-arc1[S1-1])/length < (10.0*FLT_EPSILON) );

  // Obtain equally spaced arclength positions.
  arc2[0] = (float)(sum = 0.0);
  for(int i = 1; i < S2; i++)
    arc2[i] = (float)(sum += step);
  ASSERT( fabs(length-arc2[S2-1])/length < (10.0*FLT_EPSILON) );

  // Ensure that end values coincide.
  arc1[0]    = ( arc2[0]    = 0.0f );
  arc1[S1-1] = ( arc2[S2-1] = (float)length );

  // Resample - Convolve - Back Resample.
  Resample( first, last, arc1, arc1+S1, temp, temp+P2, arc2, arc2+S2, bClosed, interpolator );
  if (bSmooth)
    SmoothCurve( temp, temp+P2, bClosed, filter, filtSize );
  else
    DifferentiateCurve( temp, temp+P2, bClosed, filter, filtSize );
  Resample( temp, temp+P2, arc2, arc2+S2, first, last, arc1, arc1+S1, bClosed, interpolator );

  free( pMem );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function linearly interpolates values at invalid points.

  Suppose any entity is defined at each curve's point, but at some points errors have occured.
  Thus, we should linearly interpolate the entity at invalid point. The implementation utilizes
  arclength as a parameter of interpolation between two successive valid points.
  <b>TODO:</b> only code for enclosed curves has been implemented.

  \param  first         begin iterator of the curve.
  \param  last          end iterator of the curve.
  \param  F             begin iterator of the entity defined at each curve's point.
  \param  L             end iterator of the entity defined at each curve's point.
  \param  bClosed       TRUE for enclosed curve.
  \param  isValid       unary function-object checks specified entity value for validity.
  \param  interpolator  function-object linearly interpolates between a couple of valid values.
  \return               Ok = true. */
//-------------------------------------------------------------------------------------------------
template< class PtIt, class EnIt, class CHECKER, class INTERPOLATOR  >
bool InterpolateInFailedPoints(PtIt first, PtIt last, EnIt F, EnIt L, bool bClosed,
                               CHECKER isValid, INTERPOLATOR interpolator)
{
  unsigned int N = std::distance( first, last );
  if (N < 2)
    return false;
  L;
  ASSERT( N == std::distance( F, L ) );

  std::vector<float>  arcLeng( N+2, 0.0f );

  if (bClosed)
  {
    PtIt  pCur, pNxt = first, start;
    EnIt  eCur, eNxt = F, e;
    int   iCur, iNxt = 0, count;

    // Find the first valid value. This value determines the start position.
    while(!isValid(*eNxt) && pNxt != last)
    {
      ++pNxt;
      ++eNxt;
    }
    start = pNxt;
    if (start == last)
      return false;

    // Go along the curve. At least one valid point is exist.
    arcLeng[iNxt] = 0.0f;
    do
    {
      pCur = pNxt;
      eCur = eNxt;
      iCur = iNxt;

      // Starting from the current, valid point try to find the next valid one.
      for(count = 0; count == 0 || !isValid(*eNxt);)
      {
        double dx = (double)((*pNxt).x);
        double dy = (double)((*pNxt).y);

        ++pNxt;
        ++eNxt;
        ++iNxt;
        ++count;

        if (pNxt == last) // cyclically warp iterators
        {
          ASSERT( eNxt == L );
          pNxt = first;
          eNxt = F;
        }

        dx -= (double)((*pNxt).x);
        dy -= (double)((*pNxt).y);
        arcLeng[iNxt] = (float)(arcLeng[iNxt-1] + sqrt( dx*dx + dy*dy ));
      }

      // Is there invalid point?
      if (count > 1)
      {
        double s1 = arcLeng[iCur];
        double s2 = arcLeng[iNxt];
        
        ++(e = eCur);
        ++iCur;
        
        // Linearly interpolate invalid points by the values at two successive, valid ones.
        while(e != eNxt)
        {
          (*e) = interpolator( (*eCur), (*eNxt), arcLeng[iCur++], s1, s2 );
          ++e;
        }
      }
    }
    while(pNxt != start);
  }
  else // !bClosed
  {
    // No Code.
    ASSERT(0);
  }

  #ifdef _DEBUG
  for(; F != L; ++F) ASSERT( isValid( *F ) );
  #endif // _DEBUG
}

} // namespace vislib

#endif // VISLIB_CURVE_LIBRARY_2_DIMENSIONS_H
