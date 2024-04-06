///////////////////////////////////////////////////////////////////////////////////////////////////
// DrawChairs.cpp
// ---------------------
// begin     : Aug 2004
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlgoTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csalgo2
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function draws intermediate chair results into an image.
///
/// It is assumed that the image has been already filled out by a grayscale picture.
///
/// \param  chairs        array of all chairs.
/// \param  whatToShow    tells what a value to show inside chair region.
/// \param  cameraNo      index of camera of interest.
/// \param  spectatorNum  the number of spectators currently in the control area or -1 (no info).
/// \param  image         destination image that reflects the current result.
/// \return               Ok = true.
///////////////////////////////////////////////////////////////////////////////////////////////////
void DrawChairResult( const csalgo2::ChairArr & chairs,
                      const wchar_t             whatToShow,
                      const int                 cameraNo,
                      const int                 spectatorNum,
                      QImage &                  image )
{
  
  int         W = image.width();
  int         H = image.height();
  int         W1 = W-1, H1 = H-1;
  Vec2i       prePt, curPt;
  GoAlongLine gal;
  char        text[64];

  for (csalgo2::ChairArr::const_iterator chrIt = chairs.begin(); chrIt != chairs.end(); ++chrIt)
  {
    if (chrIt->cameraNo != cameraNo)
      continue;
    const HScanArr & scans = chrIt->region();

    // Draw a chair highlighted by different colors depending on category.
    for (HScanArr::const_iterator scanIt = scans.begin(); scanIt != scans.end(); ++scanIt)
    {
      int       y = ALIB_LIMIT( (*scanIt).y, 0, H1 );
      int       x1 = (int)((*scanIt).x1);
      int       x2 = (int)((*scanIt).x2);
      RGBQUAD * q = image.ptr( ALIB_LIMIT( x1, 0, W1 ), y );
      RGBQUAD * e = image.ptr( ALIB_LIMIT( x2, 0, W1 ), y );

      // Reflect chair states by different colors.
      if (chrIt->bOccupied)
      {
        for (; q < e; ++q) // green/yellow chair - the more yellow the more humanity
        {
          uint c = q->rgbRed;
          q->rgbBlue  = (ubyte)0;
          q->rgbGreen = (ubyte)c;
          q->rgbRed   = (ubyte)(c*chrIt->humanity+0.5);
        }
      }
      else
      {
        for (; q < e; ++q) // blue chair - unoccupied one
        {
          uint c = q->rgbRed;
          q->rgbBlue  = (ubyte)c;
          q->rgbGreen = (ubyte)((2*c)/3);
          q->rgbRed   = (ubyte)((2*c)/3);
        }
      }

      // Draw chair's curve in different states, if necessary.
      if (!(chrIt->curve.empty()))
      {
        RGBQUAD color;
        bool    bDrawCurve = false;

        memset( &color, 0, sizeof(color) );
        if ((chrIt->bOccupied || chrIt->bHardOccupied) && chrIt->bHuman)
        {
          bDrawCurve = true;
          color.rgbRed = (ubyte)255;
          color.rgbGreen = (ubyte)255; // yellow contour - human
        }
        else if (!(chrIt->bOccupied) && chrIt->bHardOccupied)
        {
          bDrawCurve = true;
          color.rgbGreen = (ubyte)255; // green contour - no occupation, but its "impression" still exists
        }

        if (bDrawCurve)
        {
          prePt = chrIt->curve.back();
          for (Vec2fArr::const_iterator it = chrIt->curve.begin(); it != chrIt->curve.end(); ++it)
          {
            curPt = (*it);
            for (const Vec2i * p = gal.first( prePt, curPt ); p != 0; p = gal.next())
            {
              image.set_if_in( *p, color );
            }
            prePt = curPt;
          }
        }
      }
    }

    // Draw values associated with a chair.
    switch (whatToShow)
    {
      case L'S':
        sprintf( text, "%04.2f", chrIt->statEntity );
        csutility::DrawDigits5x10( text, chrIt->center.x, chrIt->center.y,
                                   true, &image, RGB(0,255,255), MY_TRANSPARENT );
        break;

      case L'D':
        sprintf( text, "%04.3f", chrIt->dynaEntity );
        csutility::DrawDigits5x10( text, chrIt->center.x, chrIt->center.y,
                                   true, &image, RGB(0,255,255), MY_TRANSPARENT );
        break;

      case L'I':
        sprintf( text, "%d", chrIt->index );
        csutility::DrawDigits5x10( text, chrIt->center.x, chrIt->center.y,
                                   true, &image, RGB(0,255,255), MY_TRANSPARENT );
        break;

      case L'H':
        sprintf( text, "%04.2f", chrIt->humanity );
        csutility::DrawDigits5x10( text, chrIt->center.x, chrIt->center.y,
                                   true, &image, RGB(0,255,255), MY_TRANSPARENT );
        break;
    }
  }

  // Draw spectator number in the top-left corner.
  if (spectatorNum >= 0)
  {
    sprintf( text, "%d", spectatorNum );
    csutility::DrawDigits5x10( text, 0, 0, false, &image, RGB(255,0,0), RGB(255,255,255) );
  }
}

} // namespace csalgo2

