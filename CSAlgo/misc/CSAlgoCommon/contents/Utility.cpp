/****************************************************************************
  Utility.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csalgocommon
{

//-------------------------------------------------------------------------------------------------
/** \brief Function draws intermediate chair result into external image.

  It is assumed that the input image has been already filled out by grayscaled picture.

  \param  chair         a chair to be drawn.
  \param  params        demo settings needed to known what to visualize.
  \param  demoImg       camera index + destination image that reflects the current result.
  \param  spectatorNum  the number of spectators currently in the control area or -1 (no info).
  \return               Ok = true. */
//-------------------------------------------------------------------------------------------------
/*
void DrawChairResult( const Chair      & chair,
                      const DemoParams & params,
                      QImage           & image,
                      GoAlongLine        gal )
{
  const HScanArr & scans = chair.region();
  int              W = image.width();
  int              H = image.height();
  int              W1 = W-1, H1 = H-1;
  Vec2i            prePt, curPt;
  char             text[64];

  // Draw a chair highlighted by different colors depending on category.
  for (HScanArr::const_iterator scanIt = scans.begin(); scanIt != scans.end(); ++scanIt)
  {
    int       y = ALIB_LIMIT( (*scanIt).y, 0, H1 );
    int       x1 = (int)((*scanIt).x1);
    int       x2 = (int)((*scanIt).x2);
    RGBQUAD * q = image.ptr( ALIB_LIMIT( x1, 0, W1 ), y );
    RGBQUAD * e = image.ptr( ALIB_LIMIT( x2, 0, W1 ), y );

    // Reflect chair states by different colors.
    if (chair.bOccupied)
    {
      if (chair.bMovement) // red chair - some activity takes place
      {
        for (; q < e; ++q)
        {
          uint c = q->rgbRed;
          q->rgbBlue  = (ubyte)0;
          q->rgbGreen = (ubyte)0;
          q->rgbRed   = (ubyte)c;
        }
      }
      else
      {
        for (; q < e; ++q) // green/yellow chair - the more yellow the more humanity
        {
          uint c = q->rgbRed;
          q->rgbBlue  = (ubyte)0;
          q->rgbGreen = (ubyte)c;
          q->rgbRed   = (ubyte)(c*chair.humanity+0.5);
        }
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
    if (!(chair.curve.empty()))
    {
      RGBQUAD color;
      bool    bDrawCurve = false;

      memset( &color, 0, sizeof(color) );
      if (chair.bOccupied && !(chair.bMovement) && chair.bHardMovement)
      {
        bDrawCurve = true;
        color.rgbRed = (ubyte)255; // red contour - no movement, but its "impression" still exists
      }
      else if (chair.bOccupied && chair.bHuman)
      {
        bDrawCurve = true;
        color.rgbRed = (ubyte)255;
        color.rgbGreen = (ubyte)255; // yellow contour - human
      }
      else if (!(chair.bOccupied) && chair.bHardOccupied)
      {
        bDrawCurve = true;
        color.rgbGreen = (ubyte)255; // green contour - no occupation, but its "impression" still exists
      }

      if (bDrawCurve)
      {
        prePt = chair.curve.back();
        prePt.x = ALIB_LIMIT( prePt.x, 0, W1 );
        prePt.y = ALIB_LIMIT( prePt.y, 0, H1 );
        for (Vec2fArr::const_iterator it = chair.curve.begin(); it != chair.curve.end(); ++it)
        {
          curPt = (*it);
          curPt.x = ALIB_LIMIT( curPt.x, 0, W1 );
          curPt.y = ALIB_LIMIT( curPt.y, 0, H1 );
          for (const Vec2i * p = gal.first( prePt, curPt ); p != 0; p = gal.next())
          {
            image[*p] = color;
          }
          prePt = curPt;
        }
      }
    }
  }

  // Draw values associated with a chair.
  switch (params.p_showValue())
  {
    case 'S':
      sprintf( text, "%04.2f", chair.statNCC );
      csutility::DrawDigits5x10( text, chair.center.x, chair.center.y,
                                 true, &image, RGB(0,255,255), MY_TRANSPARENT );
      break;

    case 'D':
      if (chair.bOccupied)
      {
        sprintf( text, "%04.2f", chair.dynaNCC );
        csutility::DrawDigits5x10( text, chair.center.x, chair.center.y,
                                   true, &image, RGB(0,255,255), MY_TRANSPARENT );
      }
      break;

    case 'I':
      sprintf( text, "%d", chair.index );
      csutility::DrawDigits5x10( text, chair.center.x, chair.center.y,
                                 true, &image, RGB(0,255,255), MY_TRANSPARENT );
      break;

    case 'H':
      sprintf( text, "%04.2f", chair.humanity );
      csutility::DrawDigits5x10( text, chair.center.x, chair.center.y,
                                 true, &image, RGB(0,255,255), MY_TRANSPARENT );
      break;
  }
}
*/

} // namespace csalgocommon

