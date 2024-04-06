/******************************************************************************
  BilinearCoefs.h
  ---------------------
  begin     : 1998
  modified  : 22 Jul 2005
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  email     : aaah@mail.ru, aaahaaah@hotmail.com
******************************************************************************/

#pragma once

namespace csutility
{

//-------------------------------------------------------------------------------------------------
/** \brief Function computes coefficients of bilinear interpolation at a point (fx,fy).

  \param  fx      abscissa of a point of interest.
  \param  fy      ordinate of a point of interest.
  \param  width   image width.
  \param  height  image height.
  \param  c       array of size 4 that will receive calculated bilinear coefficients.
  \param  p       array of size 4 that will receive offsets of neighbour grid points.
  \return         Ok = nonzero, zero means that point (fx,fy) fell out of image border. */
//-------------------------------------------------------------------------------------------------
inline bool CalcBilinearCoefficients( float fx, float fy, int width, int height, float c[4], int p[4] )
{
  ASSERT( (width > 0) && (height > 0) );

  int x = (int)floor( fx );
  int y = (int)floor( fy );
  int x1 = 1;
  int y1 = width;
  int W1 = width-1;
  int H1 = height-1;

  // Border pixel requires additional processing.
  if ((x < 0) || (y < 0) || (x >= W1) || (y >= H1))
  {
    if ((width == 0) || (height == 0) || (x < -1) || (y < -1) || (x >= width) || (y >= height))
    {
      c[0] = (c[1] = (c[2] = (c[3] = 0)));
      p[0] = (p[1] = (p[2] = (p[3] = 0)));
      return false;
    }

    if (x == -1) x1 = (x = 0); else if (x == W1) x1 = 0;
    if (y == -1) y1 = (y = 0); else if (y == H1) y1 = 0;
  }

  int   i = x + y*width;
  float dx = (float)(fx - x);          // p3 p2
  float dy = (float)(fy - y);          // p0 p1

  c[2] = dx * dy;            p[0] = i;
  c[1] = dx - c[2];          p[1] = i+x1;
  c[3] = dy - c[2];          p[2] = i+x1+y1;
  c[0] = 1.0f - dy - c[1];   p[3] = i+y1;
  return true;
}

} // namespace csutility

