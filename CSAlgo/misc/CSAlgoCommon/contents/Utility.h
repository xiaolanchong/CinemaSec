///////////////////////////////////////////////////////////////////////////////////////////////////
/// Utility.h
/// ---------------------
/// begin     : Aug 2004
/// author(s) : Albert Akhriev
/// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BaseChair.h"

namespace csalgocommon
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function converts time interval from milliseconds to seconds.
///
/// \param  startTime    beginning of the interval in milliseconds.
/// \param  currentTime  the current time stamp in milliseconds.
/// \return              the interval given in seconds.
///////////////////////////////////////////////////////////////////////////////////////////////////
inline float MilliToSeconds( __int64 startTime, __int64 currentTime )
{
  ASSERT( startTime <= currentTime );
  return (float)(0.001*(currentTime - startTime));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function extract subset of chair from the input set according to camera index.
///
/// \param  chairs    array of input chairs.
/// \param  cameraNo  index of the camera of interest or -1 (all cameras).
/// \param  result    out: resultant subset of chairs inherited from BaseChair class.
/// \return           Ok = true. */
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class CHAIR_CONTAINER >
bool ExtractChairSubSet( const BaseChairArr & chairs,
                         int                  cameraNo,
                         CHAIR_CONTAINER    & result )
{
  result.clear();
  result.reserve( chairs.size() );
  for (int i = 0; i < (int)(chairs.size()); ++i)
  {
    if ((cameraNo < 0) || ((chairs[i]).cameraNo == cameraNo))
    {
      result.push_back( typename CHAIR_CONTAINER::value_type() );
      BaseChair & chair = result.back();
      chair = chairs[i];
    }
  }
  return !(result.empty());
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function initializes chairs, inherited from Chair class, given outline curve of each chear.
///
/// \param  pChair       pointer to the chair to be initialized.
/// \param  pCurve       pointer to the storage that will receive an outline curve with integer coordinates.
/// \param  pBoundRect   pointer to the bounding rectangle of an image or 0.
/// \param  cameraNo     index of a camera that observes specified chairs or -1.
/// \param  bInitPoints  if nonzero, then additional array of chair points will be installed.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class CHAIR_TYPE >
void InitializeSingleChair( CHAIR_TYPE * pChair,
                            Vec2iArr   * pCurve,
                            ARect      * pBoundRect = 0,
                            int          cameraNo = -1,
                            bool         bInitPoints = false )
{
  ALIB_ASSERT( (pChair != 0) && (pCurve != 0) && !(pChair->curve.empty()) );
  ALIB_ASSERT( (cameraNo < 0) || (pChair->cameraNo == cameraNo) );

  // Compute chair's center point.
  pChair->center = vislib::MeanPoint( pChair->curve.begin(), pChair->curve.end() );

  // Convert 'float' curve to validated 'integer' one.
  pCurve->resize( pChair->curve.size() );
  std::copy( pChair->curve.begin(), pChair->curve.end(), pCurve->begin() );
  if (pBoundRect != 0)
  {
    ALIB_ASSERT( !(pBoundRect->empty()) );
    vislib::BoundLocation( pCurve->begin(), pCurve->end(),
                           0, 0, pBoundRect->width()-1, pBoundRect->height()-1 );
  }
  vislib::ValidateCurve( *pCurve, true );

  // Generate internal region of the curve.
  ALIB_ASSERT( pChair->region.create( *pCurve ) );

  // Compute bounding rectangle.
  pChair->rect = vislib::GetBoundRect( pCurve->begin(), pCurve->end(), int() );
  pChair->correctedRect = pChair->rect;
  ALIB_ASSERT( (pBoundRect == 0) || (pChair->rect <= (*pBoundRect)) );

  // Create another representation of internal region: array of sorted points.
  if (bInitPoints)
  {
    const HScanArr & scans = pChair->region();
    int              count = 0;

    // Fill up the array of chair's internal points.
    pChair->points.resize( pChair->region.area() );
    ALIB_ASSERT( !(pChair->points.empty()) );
    for (HScanArr::const_iterator scanIt = scans.begin(); scanIt != scans.end(); ++scanIt)
    {
      for (int x = (int)((*scanIt).x1); x < (int)((*scanIt).x2); x++)
      {
        (pChair->points[count++]).set( x, (*scanIt).y );
      }
    }
    ALIB_ASSERT( count == (int)(pChair->points.size()) );

    // Sorting optimizes cache usage.
    std::sort( pChair->points.begin(), pChair->points.end(), alib::LessXY<Vec2i>() );
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function initializes chairs, inherited from 'BaseChairEx' or similar class,
///        given outline curve of each chair.
///
/// \param  chairs       the set of chairs to be initialized.
/// \param  pBoundRect   pointer to the bounding rectangle of an image or 0.
/// \param  cameraNo     index of a camera that observes specified chairs or -1.
/// \param  bInitPoints  if nonzero, then additional array of chair points will be installed.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class CHAIR_CONTAINER >
void InitializeChairs( CHAIR_CONTAINER & chairs,
                       ARect           * pBoundRect = 0,
                       int               cameraNo = -1,
                       bool              bInitPoints = false )
{
  Vec2iArr icurve;
  ALIB_ASSERT( !(chairs.empty()) );
  icurve.reserve( 3*(chairs[0]).curve.size() );
  for (typename CHAIR_CONTAINER::iterator it = chairs.begin(); it != chairs.end(); ++it)
  {
    csalgocommon::InitializeSingleChair( &(*it), &icurve, pBoundRect, cameraNo, bInitPoints );
  }
}

} // namespace csalgocommon

