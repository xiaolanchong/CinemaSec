#include "stdafx.h"
#include "vl_horz_region.h"

#ifdef ALIB_CHECK_MEMORY_LEAKAGE
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRegion::HRegion()
{ 
  clear(); 
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Copy constructor.

  \param  rgn  the region to copy to this one. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRegion::HRegion( const HRegion & rgn )
{ 
  copy( rgn ); 
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Destructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRegion::~HRegion()
{ 
  clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function clears this object.

  \param   bCompleteClear  if nonzero, then allocated memory will be totally deallocated,
                           otherwise allocated memory stills reserved (STL style).
  \return  always zero. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool HRegion::clear( bool bCompleteClear )
{
  if (bCompleteClear)
    alib::CompleteClear( m_scans );
  else
    m_scans.clear();
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function copies specified object to this one.

  \param  rgn  the region to copy to this one. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void HRegion::copy( const HRegion & rgn )
{
  clear();
  m_scans = rgn.m_scans;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function creates region from a curve.

  The curve must be long enough (>3) and must be enclosed. The curve may be self-intersected.

  \return  Ok == true. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool HRegion::create( const Vec2iArr & curve )
{
  std::vector<Segment>  segments;      // auxiliary array of segments
  Segment               segm;
  HScan                 scan;
  int                   k, i, first;
  
  // Clear and verify.
  clear();

  int N = (int)(curve.size());
  if (N < 3)
    return false;
  if (!vislib::IsEnclosedCurve( curve.begin(), curve.end() ))
    return false;
  if (!vislib::IsValidCurve( curve.begin(), curve.end() ))
    return false;

  // Find a point that has y-coordinate other than the first one.
  for (first = 1; first < N; first++)
  {
    if (curve[first].y != curve[0].y)
      break;
  }

  // Entire curve consist of one horizontal scan.
  if (first >= N)
  {
    ARect rect = vislib::GetBoundRect( curve.begin(), curve.end(), true );

    scan.x1 = (short)rect.x1;
    scan.x2 = (short)rect.x2;
    scan.y  = rect.y1;
    m_scans.resize(1, scan); 
    return true;
  }

  // Divide curve into segments. Points of each segment have the same y-coordinates.
  segments.reserve( N );
  for (k = 0; k < N;)
  {
    // Extract sequence of curve's points (segment) which have the same y-coord.
    int index1 = (k+first) % N;
    segm.y = curve[index1].y;
    for(k++; k < N && curve[(k+first)%N].y == segm.y; k++);
    int index2 = (k+first) % N;
    
    // Determine type of curve's segment.
    int type = (curve[(index1-1+N)%N].y - segm.y) * (curve[index2].y - segm.y);
    //assert(type != 0);
    segm.type = (type < 0) ? CROSS : TOUCH;

    // Find x-coordinates of segment's ends.
    segm.x1 = curve[index1].x;
    segm.x2 = curve[(index2-1+N)%N].x;
    if (segm.x1 > segm.x2)
      std::swap( segm.x1, segm.x2 );
    segm.x2++;

    segments.push_back( segm );
  }

  // Sort segmentes by their y-coordinates.
  std::sort( segments.begin(), segments.end(), CmpY() );

  // Find horizontal scans which cover the curve's area between appropriate segmentes.
  N = (int)(segments.size());
  m_scans.reserve(N);
  for (k = 0; k < N;)
  {
    // Among all segments, find those which have current y-coordinate. They
    // are all gathered together (after previous sorting), but not sorted by x-coord.
    int y = segments[k].y;
    int index1 = k;
    for(++k; k < N && segments[k].y == y; k++);
    int index2 = k;

    // Sort segments in current scan-line by x-coordinate.
    std::sort( segments.begin()+index1, segments.begin()+index2, CmpX() );

    // Find scans combining horizontal curve segments. Segment with type 'CROSS' must meet
    // appropiate segment of the same type on the "other" side of the region. If the curve is
    // self-intersected, then some segments may intersect or one segment may be inside other or etc.
    // Following situation should be processed carefully. 'TOUCH' segment(s) occured between
    // two 'CROSS' ones. The resulting scan will start at 'x1' and finish an 'x2':
    //
    //         *                        *              ..........................
    //         *                        *              ..........................
    //  x1 -->  ***      *****      ****  <-- x2        ........................
    //             *    *     *    *                       ......     ......
    //             *    *     *    *                       ......     ......
    //
    for (i = index1; i < index2; i++)
    {
      scan.y = y;
      scan.x1 = (short)( segments[i].x1 );
      int x2 = segments[i].x2;
      if (segments[i].type == CROSS)
      {
        do                                       // find pair for this type
        {
          x2 = std::max( x2, segments[i].x2 );   // find the greatest second end
          i++;
        }
        while (i < index2 && segments[i].type != CROSS);

        if (i >= index2)
          return clear();
      }
      scan.x2 = (short)(std::max( x2, segments[i].x2 ));
      m_scans.push_back( scan );
    }
  }

  if (!remove_overlapping_of_scans())
    return clear();
  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function reserves memory given the number of expected vertical scans. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void HRegion::reserve( unsigned int nScan )
{
  m_scans.reserve( nScan );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function removes ovelapping of scans that takes place for a self-intersecting curve.

  \return  Ok = true. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool HRegion::remove_overlapping_of_scans()
{
  if (m_scans.empty())
    return false;

  HScanArr::iterator  pre = m_scans.begin();
  HScanArr::iterator  cur = m_scans.begin();

  for (cur++; cur != m_scans.end();)
  {
    if (((*pre).y != (*cur).y) || ((*pre).x2 < (*cur).x1))
    {
      pre = cur++;
      continue;
    }

    if ((*pre).x1 > (*cur).x1)
      return false;                                   // wrong order by x-coordinate

    (*pre).x2 = std::max( (*pre).x2, (*cur).x2 );     // combine overlapped scans into one
    cur = m_scans.erase( cur );
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function returns non-zero, if the query point is inside this region.

  \param  pt  the query point.
  \return     non-zero for inside point. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool HRegion::inside( Vec2i pt ) const
{
  for (int k = 0, n = (int)(m_scans.size()); k < n; k++)
  {
    HScan scan = m_scans[k];
    if ((scan.y == pt.y) && ((int)(scan.x1) <= pt.x) && (pt.x < (int)(scan.x2)))
      return true;
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function computes the center of mass of this region.

  \param  massCenter  reference to the receiver of mass center's value.
  \return             Ok = true. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool HRegion::center( Vec2f & massCenter ) const
{
  Vec2i v, center(0,0);
  int   N = 0;

  for (int k = 0, size = (int)(m_scans.size()); k < size; k++)
  {
    HScan scan = m_scans[k];
    int   n = scan.x2 - scan.x1;

    center.x += scan.x1*n + (n*(n-1))/2;    // sum of x-coordinates
    center.y += scan.y*n;                   // sum of y-coordinates
    N += n;
  }
  massCenter = (N > 0) ? (Vec2f(center)*(float)(1.0/N)) : Vec2f();
  return (N > 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function computes the area of this region (the total number of internal points).

  \return  the area of this region. */
///////////////////////////////////////////////////////////////////////////////////////////////////
int HRegion::area() const
{
  int square = 0;
  for (int k = 0, n = (int)(m_scans.size()); k < n; k++)
  {
    HScan scan = m_scans[k];
    square += (scan.x2 - scan.x1);
  }
  return square;
}


#ifdef _WINDOWS
///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function paints this region. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void HRegion::draw( CDC * pDC, Vec2i offset, COLORREF color ) const
{
  for (int k = 0, n = (int)(m_scans.size()); k < n; k++)
  {
    HScan scan = m_scans[k];
    pDC->FillSolidRect( offset.x+scan.x1, offset.y+scan.y, scan.x2-scan.x1, 1, color );
  }
}
#endif // _WINDOWS

