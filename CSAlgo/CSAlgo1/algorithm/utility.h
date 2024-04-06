/****************************************************************************
  utility.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "chair.h"

namespace csalgo
{

//-------------------------------------------------------------------------------------------------
/** \brief Function converts time interval from milliseconds to seconds.

  \param  startTime    beginning of the interval in milliseconds.
  \param  currentTime  the current time stamp in milliseconds.
  \return              the interval given in seconds. */
//-------------------------------------------------------------------------------------------------
inline float MilliToSeconds( __int64 startTime, __int64 currentTime )
{
  ASSERT( startTime <= currentTime );
  return (float)(0.001*(currentTime - startTime));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function extract subset of chair from the input set according to camera index.

  \param  chairs    array of input chairs.
  \param  cameraNo  index of the camera of interest or -1 (all cameras).
  \param  result    out: resultant subset of chairs inherited from BaseChair class.
  \return           Ok = true. */
//-------------------------------------------------------------------------------------------------
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


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes chairs, inherited from Chair class, given outline curve of each chear.

  \param  chairArray   array of chairs to be initialized.
  \param  pBoundRect   pointer to the bounding rectangle of an image or 0.
  \param  cameraNo     index of a camera that observes specified chairs or -1.
  \param  bInitPoints  if nonzero, then additional array of chair points will be installed. */
//-------------------------------------------------------------------------------------------------
template< class CHAIR_CONTAINER >
void InitializeChairs( CHAIR_CONTAINER & chairSet,
                       ARect           * pBoundRect = 0,
                       int               cameraNo = -1,
                       bool              bInitPoints = false )
{
  ALIB_ASSERT( !(chairSet.empty()) );

  Vec2iArr icurve;

  // For each chair ...
  icurve.reserve( 3*(chairSet[0]).curve.size() );
  for (typename CHAIR_CONTAINER::iterator chrIt = chairSet.begin(); chrIt != chairSet.end(); ++chrIt)
  {
    Chair & chair = (*chrIt);

    ALIB_ASSERT( (cameraNo < 0) || (chair.cameraNo == cameraNo) );
    ALIB_ASSERT( !(chair.curve.empty()) );
    chair.center = vislib::MeanPoint( chair.curve.begin(), chair.curve.end() );

    // Convert 'float' curve to validated 'integer' one.
    icurve.resize( chair.curve.size() );
    std::copy( chair.curve.begin(), chair.curve.end(), icurve.begin() );
    if (pBoundRect != 0)
    {
      ALIB_ASSERT( !(pBoundRect->empty()) );
      vislib::BoundLocation( icurve.begin(), icurve.end(),
                             0, 0, pBoundRect->width()-1, pBoundRect->height()-1 );
    }
    vislib::ValidateCurve( icurve, true );

    // Generate internal region of the curve.
    ALIB_ASSERT( chair.region.create( icurve ) );

    // Compute bounding rectangle.
    chair.rect = vislib::GetBoundRect( icurve.begin(), icurve.end(), int() );
    ALIB_ASSERT( (pBoundRect == 0) || (chair.rect <= (*pBoundRect)) );

    // Create another representation of internal region: array of sorted points.
    if (bInitPoints)
    {
      const HScanArr & scans = chair.region();
      int              count = 0;

      // Fill up the array of internal chair's points and sort them to optimize cache usage.
      chair.points.resize( chair.region.area() );
      ALIB_ASSERT( !(chair.points.empty()) );
      for (HScanArr::const_iterator scanIt = scans.begin(); scanIt != scans.end(); ++scanIt)
      {
        for (int x = (int)((*scanIt).x1); x < (int)((*scanIt).x2); x++)
        {
          (chair.points[count++]).set( x, (*scanIt).y );
        }
      }
      ALIB_ASSERT( count == (int)(chair.points.size()) );
      std::sort( chair.points.begin(), chair.points.end(), alib::LessXY<Vec2i>() );
    }
  }
}


void DrawChairResult( const Chair      & chair,
                      const DemoParams & params,
                      QImage           & image,
                      GoAlongLine        gal );

//-------------------------------------------------------------------------------------------------
/** \brief Function draws intermediate result into external image.

  It is assumed that the input image has been already filled out by grayscaled picture.

  \param  chairSet      container of chairs inherited from Chair class.
  \param  params        demo settings needed to known what to visualize.
  \param  demoImg       camera index + destination image that reflects the current result.
  \param  spectatorNum  the number of spectators currently in the control area or -1 (no info).
  \return               Ok = true. */
//-------------------------------------------------------------------------------------------------
template< class CHAIR_CONTAINER >
void DrawResult( const CHAIR_CONTAINER & chairSet,
                 const DemoParams      & params,
                 TDemoImage            & demoImg,
                 const int               spectatorNum )
{
  GoAlongLine gal;
  char        text[64];

  for (typename CHAIR_CONTAINER::const_iterator it = chairSet.begin(); it != chairSet.end(); ++it)
  {
    if ((*it).cameraNo == (int)(demoImg.data.first))
      DrawChairResult( (*it), params, demoImg.data.second, gal );
  }

  if (spectatorNum >= 0)
  {
    sprintf( text, "%d", spectatorNum );
    csutility::DrawDigits5x10( text, 0, 0, false, &(demoImg.data.second), RGB(255,0,0), RGB(255,255,255) );
  }
}


HRESULT LoadParameters( const Int8Arr & binParams,
                        Parameters    & parameters );

} // namespace csalgo

