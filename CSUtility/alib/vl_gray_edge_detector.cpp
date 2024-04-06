#include "stdafx.h"
#include "vl_gray_edge_detector.h"

#ifdef ALIB_CHECK_MEMORY_LEAKAGE
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace vislib
{

//-------------------------------------------------------------------------------------------------
/** \brief Contructor. */
//-------------------------------------------------------------------------------------------------
GrayEdgeDetector::GrayEdgeDetector()
{
  Clear();
  SetDefaultParameters();
  ASSERT( MAYBE_EDGE == 1 );
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
GrayEdgeDetector::~GrayEdgeDetector()
{
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object. */
//-------------------------------------------------------------------------------------------------
bool GrayEdgeDetector::Clear()
{
  m_dirs.clear();
  m_grads.clear();
  m_edgeMap.clear();
  m_loThr = (m_hiThr = (m_gradNoise = 0.0f));
  m_ok = false;
  return false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function sets default parameter values. */
//-------------------------------------------------------------------------------------------------
void GrayEdgeDetector::SetDefaultParameters()
{
  m_bBinarize = true;
  m_hiThrMult = 2.0f;
  m_loThrMult = 1.0f;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function fits parameter values to valid bounds. */
//-------------------------------------------------------------------------------------------------
void GrayEdgeDetector::ValidateParameters()
{
  m_loThrMult = alib::Limit( m_loThrMult, 0.5f, 10.0f );
  m_hiThrMult = alib::Limit( m_hiThrMult, 1.0f, 10.0f );
  
  if (m_hiThrMult < m_loThrMult)
    std::swap( m_loThrMult, m_hiThrMult );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes gradients and their directions at image points.

  Note, that scale factor 0.5 must be applied to gradient vectors, if you need the actual values.

  \param  image  the image to be processed.
  \param  dirs   out: the field of normalized gradient directions.
  \param  grads  out: the map of gradient modules.
  \param  pRect  pointer to the rectangle that bounds a region of interest (zero means entire image). */
//-------------------------------------------------------------------------------------------------
void GrayEdgeDetector::ComputeGradients( const Arr2f & image,
                                         Vec2fImg    & dirs,
                                         Arr2f       & grads,
                                         const ARect * pRect )
{
  int   W = image.width();
  int   H = image.height();
  ARect rect = (pRect != 0) ? (*pRect) : dirs.rect();

  dirs.resize( W, H, Vec2f() );
  grads.resize( W, H, 0.0f );
  if ((W < 3) || (H < 3))
    return;
  rect.inflate( 1, 1 );
  rect &= dirs.rect();
  rect.deflate( 1, 1 );

  for (int y = rect.y1; y < rect.y2; y++)
  {
    const float * pre = image.row_begin( y-1 );
    const float * cur = image.row_begin(  y  );
    const float * nxt = image.row_begin( y+1 );

    Vec2f * d = dirs.row_begin( y );
    float * g = grads.row_begin( y );

    for (int x = rect.x1; x < rect.x2; x++)
    {
      float gx = cur[x+1] - cur[x-1];
      float gy = nxt[ x ] - pre[ x ];
      float gr = (float)sqrt( gx*gx + gy*gy );

      if (gr > FLT_EPSILON)
      {
        (d[x]).x = gx/gr;
        (d[x]).y = gy/gr;
        g[x] = gr;
      }
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes gradients and their directions at image points.

  Note, that scale factor 0.5 must be applied to gradient vectors, if you need the actual values.

  \param  image  the image to be processed.
  \param  dirs   out: the field of normalized gradient directions.
  \param  grads  out: the map of gradient modules.
  \param  pRect  pointer to the rectangle that bounds a region of interest (zero means entire image). */
//-------------------------------------------------------------------------------------------------
/*
void GrayEdgeDetector::ComputeGradientsByDirectionalFilter( const Arr2f & image,
                                                        Vec2fImg    & dirs,
                                                        Arr2f       & grads,
                                                        const ARect * pRect )
{
  int    W = image.width();
  int    H = image.height();
  ARect  rect = (pRect != 0) ? (*pRect) : dirs.rect();
  Vec2f  dervDirs[8];
  double sum[8];

  dirs.resize( W, H, Vec2f() );
  grads.resize( W, H, 0.0f );
  if ((W < 9) || (H < 9))
    return;
  rect.inflate( 1, 1 );
  rect &= dirs.rect();
  rect.deflate( 1, 1 );

  for (int i = 0; i < ALIB_LengOf(dervDirs); i++)
  {
    (dervDirs[i]).set( (float)cos( i*(ALIB_PI/8.0) ),
                       (float)sin( i*(ALIB_PI/8.0) ) );
  }

  for (int y = rect.y1; y < rect.y2; y++)
  {
    for (int x = rect.x1; x < rect.x2; x++)
    {
      double maxSum = 0.0;
      int    bestNo = -1;

      std::fill( sum, sum+ALIB_LengOf(sum), 0.0 );

      for (int v = 0; v < 9; v++)
      {
        for (int u = 0; u < 9; u++)
        {
          float t = image.bounded_at( x+u-4, y+v-4 );
          sum[0] += t*GaussDirDerv0[v][u];
          sum[1] += t*GaussDirDerv1[v][u];
          sum[2] += t*GaussDirDerv2[v][u];
          sum[3] += t*GaussDirDerv3[v][u];
          sum[4] += t*GaussDirDerv4[v][u];
          sum[5] += t*GaussDirDerv5[v][u];
          sum[6] += t*GaussDirDerv6[v][u];
          sum[7] += t*GaussDirDerv7[v][u];
        }
      }

      for (int i = 0; i < ALIB_LengOf(sum); i++)
      {
        double a = fabs( sum[i] );
        if ((maxSum < a) && (a > FLT_EPSILON))
        {
          maxSum = a;
          bestNo = i;
        }
      }

      if (bestNo >= 0)
      {
        grads( x, y ) = (float)maxSum;
        dirs( x, y ) = dervDirs[bestNo];
      }
    }
  }
}
*/


//-------------------------------------------------------------------------------------------------
/** \brief Function estimates root-mean-square gradient noise and finds points where gradient
           reaches maximum in gradient direction and twice exceeds noise level (edge points).

  On output potential edge points are marked by '1' in resultant edge map.

  \param  dirs     the field of normalized gradient directions.
  \param  grads    the map of gradient modules.
  \param  edgeMap  out: resultant edge map (0 - no edge, 1 - maybe edge).
  \param  pRect    pointer to the rectangle that bounds a region of interest (zero means entire image).
  \return          Ok = estimated deviation of gradient noise, otherwise zero value. */
//-------------------------------------------------------------------------------------------------
float GrayEdgeDetector::EstimateGradNoiseAndFindRawEdgePoints( const Vec2fImg & dirs,
                                                           const Arr2f    & grads,
                                                           Arr2ub         & edgeMap,
                                                           const ARect    * pRect )
{
  int     W = dirs.width();
  int     H = dirs.height();
  double  minGrad = 0.0, noise = 0.0;
  Vec2f   fp, dir;
  Arr2f   diff;
  ARect   rect = (pRect != 0) ? (*pRect) : dirs.rect();

  ASSERT( alib::AreDimensionsEqual( dirs, grads ) );
  edgeMap.resize2( dirs );
  if ((W < 3) || (H < 3))
    return 0.0f;
  rect.inflate( 1, 1 );
  rect &= dirs.rect();
  rect.deflate( 1, 1 );
  diff.resize2( dirs );

  // Compute squared gradient noise over all potential edge points.
  {
    MeanAccum<double> accum;

    for (int y = rect.y1; y < rect.y2; y++)
    {
      for (int x = rect.x1; x < rect.x2; x++)
      {
        // Take a couple of gradient values in the gradient direction.
        fp.set( (float)x, (float)y );
        dir = dirs( x, y );

        float gr = grads( x, y );
        float g1 = grads.bilin_at( fp + dir );
        float g2 = grads.bilin_at( fp - dir );

        // Is edge point? If yes, then compute noise perpendicularly to gradient.
        if (((g1 < gr) && (gr >= g2)) || ((g1 <= gr) && (gr > g2)))
        {
          dir = dir.perpendicular();
          float t = (float)(fabs( grads.bilin_at( fp + dir ) - 2.0*gr + grads.bilin_at( fp - dir ) ));
          diff( x, y ) = t;
          accum += t;
          edgeMap( x, y ) = (ubyte)(MAYBE_EDGE);
        }
      }
    }

    // Minimal gradient at edge point should at least twice exceed the noise level.
    minGrad = (2.0 * sqrt( ALIB_PI/12.0 )) * accum.mean();
  }

  // Calculate gradient noise over refined set of raw edge points.
  {
    MeanAccum<double> accum;

    for (int y = rect.y1; y < rect.y2; y++)
    {
      for (int x = rect.x1; x < rect.x2; x++)
      {
        if (edgeMap( x, y ) != NO_EDGE)
        {
          if (grads( x, y ) > minGrad)
          {
            accum += diff( x, y );
          }
          else edgeMap( x, y ) = (ubyte)(NO_EDGE);
        }
      }
    }

    noise = sqrt( ALIB_PI/12.0 ) * accum.mean();
  }
  return (float)(std::max( minGrad/2.0, noise ));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function recursively follows a connected edge starting from the current point.

  \param  start  starting point of recursive search. */
//-------------------------------------------------------------------------------------------------
void GrayEdgeDetector::FollowEdge( Vec2i start )
{
  // Check the state of the current point and mark it as passed one.
  ubyte * s = m_edgeMap.get_if_in( start );
  if ((s == 0) || ((*s) != MAYBE_EDGE))
    return;
  (*s) = (ubyte)(YES_EDGE);

  float max = m_loThr;
  int   best = -1;
  Vec2i p;

  // Check immediately adjacent points to see whether they could be tracked.
  for (int k = 0; k < ALIB_LengOf(ALIB_NEIBR8); k++)
  {
    (p = start) += ALIB_NEIBR8[k];
    if ((m_edgeMap[p] == MAYBE_EDGE) && (m_grads[p] > max))
    {
      max = m_grads[p];
      best = k;
    }
  }

  // Recursively follow the best continuation.
  if (best >= 0)
    FollowEdge( start += ALIB_NEIBR8[best] );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function extracts edges from a raw edge map, using nonmaxima suppression algorithm. */
//-------------------------------------------------------------------------------------------------
void GrayEdgeDetector::EdgeTracking()
{
  ASSERT( alib::AreDimensionsEqual( m_edgeMap, m_grads ) );

  int   W = m_grads.width();
  int   H = m_grads.height();
  Vec2i start;

  for (int y = 0; y < H; y++)
  {
    const float * g = m_grads.row_begin( y );
    ubyte       * e = m_edgeMap.row_begin( y );

    for (int x = 0; x < W; x++)
    {
      if (e[x] == MAYBE_EDGE)
      {
        if (g[x] >= m_hiThr)
        {
          FollowEdge( start.set( x, y ) );
        }
        else e[x] = (ubyte)(NO_EDGE);
      }
    }
  }

  if (m_bBinarize)
  {
    for (int i = 0, n = m_edgeMap.size(); i < n; i++)
    {
      m_edgeMap[i] = (ubyte)((m_edgeMap[i] == YES_EDGE) ? UCHAR_MAX : 0);
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function performs edge detection.

  \param  image  the image to be processed.
  \param  pRect  pointer to the rectangle that bounds a region of interest (zero means entire image).
  \return        Ok = true. */
//-------------------------------------------------------------------------------------------------
bool GrayEdgeDetector::Run( const Arr2f & image, const ARect * pRect )
{
  try
  {
    if ((image.width() < 3) || (image.height() < 3))
      ALIB_THROW( _T("Too small image") );

    ARect rect = (pRect != 0) ? ((*pRect) & image.rect()) : image.rect();

    Clear();    
    ValidateParameters();

#if 1
    GrayEdgeDetector::ComputeGradients( image, m_dirs, m_grads, &rect );
#else
    GrayEdgeDetector::ComputeGradientsByDirectionalFilter( image, m_dirs, m_grads, &rect );
#endif

    m_gradNoise = GrayEdgeDetector::EstimateGradNoiseAndFindRawEdgePoints( m_dirs, m_grads, m_edgeMap, &rect );
    m_hiThr = m_hiThrMult * m_gradNoise;
    m_loThr = m_loThrMult * m_gradNoise;
    EdgeTracking();
    m_ok = true;
  }
  catch (std::runtime_error & e)
  {
    Clear();
    alib::ErrorMessage( CString( e.what() ) );
  }
  catch (...)
  {
    Clear();
    alib::ErrorMessage( ALIB_UNSUPPORTED_EXCEPTION );
  }
  return m_ok;
}

} // namespace vislib


