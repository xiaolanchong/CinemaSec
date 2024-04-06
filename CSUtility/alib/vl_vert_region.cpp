#include "stdafx.h"
#include "vl_vert_region.h"

#ifdef ALIB_CHECK_MEMORY_LEAKAGE
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
VRegion::VRegion()
{ 
  clear(); 
}


//-------------------------------------------------------------------------------------------------
/** \brief Copy constructor.

  \param  rgn  the region to copy to this one. */
//-------------------------------------------------------------------------------------------------
VRegion::VRegion( const VRegion & rgn )
{ 
  copy( rgn ); 
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
VRegion::~VRegion()
{ 
  clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object.

  \param   bCompleteClear  if nonzero, then allocated memory will be totally deallocated,
                           otherwise allocated memory stills reserved (STL style).
  \return  always zero. */
//-------------------------------------------------------------------------------------------------
bool VRegion::clear( bool bCompleteClear )
{
  if (bCompleteClear)
    alib::CompleteClear( m_scans );
  else
    m_scans.clear();
  return false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function copies specified object to this one.

  \param  rgn  the region to copy to this one. */
//-------------------------------------------------------------------------------------------------
void VRegion::copy( const VRegion & rgn )
{
  clear();
  m_scans = rgn.m_scans;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function creates region from a curve.

  The curve must be long enough (>3) and must be enclosed. The curve may be self-intersected.

  \return  Ok == true. */
//-------------------------------------------------------------------------------------------------
bool VRegion::create( const Vec2iArr & curve )
{
  std::vector<Segment>  segments;      // auxiliary array of segments
  Segment               segm;
  VScan                 scan;
  int                   k, i, first;
  
  // Clear and verify.
  clear();

  int N = (int)(curve.size());
  if (N < 3)
    return false;
  if (!(vislib::IsEnclosedCurve( curve.begin(), curve.end() )))
    return false;
  if (!(vislib::IsValidCurve( curve.begin(), curve.end() )))
    return false;

  // Find a point that has x-coordinate other than the first one.
  for (first = 1; first < N; first++)
  {
    if ((curve[first]).x != (curve[0]).x)
      break;
  }

  // Entire curve consist of one vertical scan.
  if (first >= N)
  {
    ARect rect = vislib::GetBoundRect( curve.begin(), curve.end(), true );

    scan.y1 = (short)(rect.y1);
    scan.y2 = (short)(rect.y2);
    scan.x  = rect.x1;
    m_scans.resize( 1, scan );
    return true;
  }

  // Divide the curve into segments. Points of each segment have the same x-coordinates.
  segments.reserve( N );
  for (k = 0; k < N;)
  {
    // Extract sequence of curve's points (segment) which have the same x-coordinate.
    int index1 = (k+first)%N;
    segm.x = curve[index1].x;
    for (k++; (k < N) && ((curve[(k+first)%N]).x == segm.x); k++);
    int index2 = (k+first)%N;
    
    // Determine the type of curve's segment.
    int type = ((curve[(index1-1+N)%N]).x - segm.x) * ((curve[index2]).x - segm.x);
    //assert(type != 0);
    segm.type = (type < 0) ? CROSS : TOUCH;

    // Find y-coordinates of segment's ends.
    segm.y1 = (curve[index1]).y;
    segm.y2 = (curve[(index2-1+N)%N]).y;
    if (segm.y1 > segm.y2)
      std::swap( segm.y1, segm.y2 );
    segm.y2++;

    segments.push_back( segm );
  }

  // Sort segmentes by their x-coordinates.
  std::sort( segments.begin(), segments.end(), CmpX() );

  // Find vertical scans which cover the curve's area between appropriate segmentes.
  N = (int)(segments.size());
  m_scans.reserve( N );
  for (k = 0; k < N;)
  {
    // Among all segments, find those which have current x-coordinate. Thex
    // are all gathered together (after previous sorting), but not sorted bx y-coord.
    int x = segments[k].x;
    int index1 = k;
    for(++k; k < N && segments[k].x == x; k++);
    int index2 = k;

    // Sort segments in current scan-line bx y-coordinate.
    std::sort( segments.begin()+index1, segments.begin()+index2, CmpY() );

    // Find scans combining vertical curve segments. Segment with type 'CROSS' must meet
    // appropriate segment of the same type on the "other" side of the region. If the curve is
    // self-intersected, then some segments may intersect or one segment may be inside other or etc.
    // Following situation should be processed carefully. 'TOUCH' segment(s) occured between
    // two 'CROSS' ones. The resulting scan will start at 'y1' and finish an 'y2':
    //
    //         *                        *              ..........................
    //         *                        *              ..........................
    //  y1 -->  ***      *****      ****  <-- y2        ........................
    //             *    *     *    *                       ......     ......
    //             *    *     *    *                       ......     ......
    //
    for (i = index1; i < index2; i++)
    {
      scan.x = x;
      scan.y1 = (short)((segments[i]).y1);
      int y2 = (segments[i]).y2;
      if ((segments[i]).type == CROSS)
      {
        do                                       // find pair for this type
        {
          y2 = std::max( y2, (segments[i]).y2 ); // find the greatest second end
          i++;
        }
        while ((i < index2) && ((segments[i]).type != CROSS));

        if (i >= index2)
          return clear();
      }
      scan.y2 = (short)(std::max( y2, (segments[i]).y2 ));
      m_scans.push_back( scan );
    }
  }

  if (!remove_overlapping_of_scans())
    return clear();
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function reserves memory given the number of expected vertical scans. */
//-------------------------------------------------------------------------------------------------
void VRegion::reserve( unsigned int nScan )
{
  m_scans.reserve( nScan );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function removes ovelapping of scans that takes place for a self-intersecting curve.

  \return  Ok = true. */
//-------------------------------------------------------------------------------------------------
bool VRegion::remove_overlapping_of_scans()
{
  if (m_scans.empty())
    return false;

  VScanArr::iterator  pre = m_scans.begin();
  VScanArr::iterator  cur = m_scans.begin();

  for (cur++; cur != m_scans.end();)
  {
    if (((*pre).x != (*cur).x) || ((*pre).y2 < (*cur).y1))
    {
      pre = cur++;
      continue;
    }

    if ((*pre).y1 > (*cur).y1)
      return false;                                   // wrong order by y-coordinate

    (*pre).y2 = std::max( (*pre).y2, (*cur).y2 );     // combine overlapped scans into one
    cur = m_scans.erase( cur );
  }
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns non-zero, if the query point is inside this region.

  \param  pt  the query point.
  \return     non-zero for inside point. */
//-------------------------------------------------------------------------------------------------
bool VRegion::inside( Vec2i pt ) const
{
  for (int k = 0, n = (int)(m_scans.size()); k < n; k++)
  {
    VScan scan = m_scans[k];
    if ((scan.x == pt.x) && ((int)(scan.y1) <= pt.y) && (pt.y < (int)(scan.y2)))
      return true;
  }
  return false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes the center of mass of this region.

  \param  massCenter  reference to the receiver of mass center's value.
  \return             Ok = true. */
//-------------------------------------------------------------------------------------------------
bool VRegion::center( Vec2f & massCenter ) const
{
  Vec2i v, center(0,0);
  int   N = 0;

  for (int k = 0, size = (int)(m_scans.size()); k < size; k++)
  {
    VScan scan = m_scans[k];
    int   n = scan.y2 - scan.y1;

    center.y += scan.y1*n + (n*(n-1))/2;    // sum of y-coordinates
    center.x += scan.x*n;                   // sum of x-coordinates
    N += n;
  }
  massCenter = (N > 0) ? (Vec2f( center )*(float)(1.0/(double)N)) : Vec2f();
  return (N > 0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes the area of this region (the total number of internal points).

  \return  the area of this region. */
//-------------------------------------------------------------------------------------------------
int VRegion::area() const
{
  int area = 0;
  for (int k = 0, n = (int)(m_scans.size()); k < n; k++)
  {
    VScan scan = m_scans[k];
    area += (scan.y2 - scan.y1);
  }
  return area;
}


#ifdef _WINDOWS
//-------------------------------------------------------------------------------------------------
/** \brief Function paints this region. */
//-------------------------------------------------------------------------------------------------
void VRegion::draw( CDC * pDC, Vec2i offset, COLORREF color ) const
{
  for (int k = 0, n = (int)(m_scans.size()); k < n; k++)
  {
    VScan scan = m_scans[k];
    pDC->FillSolidRect( offset.x+scan.x, offset.y+scan.y1, scan.y2-scan.y1, 1, color );
  }
}
#endif // _WINDOWS

