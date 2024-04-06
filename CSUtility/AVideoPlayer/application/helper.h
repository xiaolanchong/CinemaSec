/******************************************************************************
  helper.h
  ---------------------
  begin     : Apr 2005
  modified  : 24 Jul 2005
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  email     : aaah@mail.ru
******************************************************************************/

#pragma once

//-------------------------------------------------------------------------------------------------
/** \brief Function draws a color image.

  \param  pImage  pointer to the image to be drawn.
  \param  hdc     device context.
  \param  rect    destination rectangle. */
//-------------------------------------------------------------------------------------------------
template< class IMAGE, bool DOWNWARD_AXIS_Y >
void DrawColorImage( const IMAGE * pImage, HDC hdc, CRect rect )
{
  if ((hdc == 0) || (pImage == 0) || pImage->empty())
    return;
  if (sizeof(typename IMAGE::value_type) != 4)
    return;

  int W = pImage->width();
  int H = pImage->height();

  BITMAPINFO bi;
  memset( &bi, 0, sizeof(bi) );
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = W;
  bi.bmiHeader.biHeight = DOWNWARD_AXIS_Y ? -H : +H;
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  bi.bmiHeader.biCompression = BI_RGB;
  bi.bmiHeader.biSizeImage = pImage->size() * sizeof(typename IMAGE::value_type);

  int   newMode = ((rect.Width() < W) || (rect.Height() < H)) ? HALFTONE : COLORONCOLOR;
  int   oldMode = ::SetStretchBltMode( hdc, newMode );
  POINT point;

  point.y = (point.x = 0);
  if (newMode == HALFTONE)
    ::SetBrushOrgEx( hdc, 0, 0, &point );

  ::StretchDIBits( hdc, rect.left, rect.top, rect.Width(), rect.Height(),
    0, 0, W, H, &(*(pImage->begin())), &bi, DIB_RGB_COLORS, SRCCOPY );

  if (newMode == HALFTONE)
    ::SetBrushOrgEx( hdc, point.x, point.y, 0 );

  ::SetStretchBltMode( hdc, oldMode );
}

