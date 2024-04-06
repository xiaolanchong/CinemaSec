/****************************************************************************
  ImageCorrector.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "ImageCorrector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
ImageCorrector::ImageCorrector()
{
  m_width = (m_height = 0);
  m_p.resize(6);
  m_b.resize(6);
  m_Q.resize(6,6);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function do some preprocessing that speeds up performance. */
//-------------------------------------------------------------------------------------------------
bool ImageCorrector::Preprocess( int width, int height )
{
  int    hW = width/2, hH = height/2;
  dMat   T(6,6);
  double p[6];

  if ((width <= 0) || (height <= 0))
    return false;

  m_width = width;
  m_height = height;
  std::fill( m_p.begin(), m_p.end(), float(0.0) );
  std::fill( m_b.begin(), m_b.end(), double(0.0) );
  std::fill( m_Q.begin(), m_Q.end(), double(0.0) );

  // Accumulate covariance matrix of quadratic approximation.
  for (int v = 0; v < height; v++)
  {
    for (int u = 0; u < width; u++)
    {
      double x = u - hW;
      double y = v - hH;

      p[0] = x*x;
      p[1] = x*y;
      p[2] = y*y;
      p[3] = x;
      p[4] = y;
      p[5] = 1;

      m_Q += matrixlib::SetOuterProductMatrix( p, p+6, T );
    }
  }

  // Compute inverted covariance matrix of quadratic approximation.
  alib::Multiply( m_Q.begin(), m_Q.end(), (1.0/(double)(width*height)) );
  return (matrixlib::Invert6x6( m_Q.begin(), m_Q.end() ));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function equalizes images brightness via quadratic approximation of intensity function.

  \param  pSource  pointer to the source grayscaled image.
  \param  pImage   pointer to the resultant color image. */
//-------------------------------------------------------------------------------------------------
void ImageCorrector::DoCorrection( const Arr2f * pSource, QImage * pImage )
{
  const double MULTIPLIER = 3.0;

  if ((pSource == 0) || (pImage == 0) || pSource->empty())
  {
    if (pImage != 0) pImage->fast_zero();
    return;
  }

  if (!(alib::AreDimensionsEqual( *pImage, *pSource )))
    pImage->resize2( *pSource, false );

  int   W = pSource->width();
  int   H = pSource->height();
  int   hW = W/2, hH = H/2;
  float scale = 0.0;

  if (((m_width != W) || (m_height != H)) && !Preprocess( W, H ))
  {
    if (pImage != 0) pImage->fast_zero();
    return;
  }

  // Compute vector of parameters of quadratic approximation.
  {
    double I2 = 0.0;

    std::fill( m_b.begin(), m_b.end(), double(0.0) );
    for (int v = 0; v < H; v++)
    {
      const float * row = pSource->row_begin( v );

      for (int u = 0; u < W; u++)
      {
        float i = row[u];
        int   x = u - hW;
        int   y = v - hH;
        float ix = (float)(i*x);
        float iy = (float)(i*y);

        m_b[0] += ix*x;
        m_b[1] += ix*y;
        m_b[2] += iy*y;
        m_b[3] += ix;
        m_b[4] += iy;
        m_b[5] += i;
        I2 += i*i;
      }
    }

    alib::Multiply( m_b.begin(), m_b.end(), (1.0/(double)(W*H)) );
    matrixlib::MulRightVector( m_Q, m_b, m_p );
    I2 /= (double)(W*H);
    double s = I2 - std::inner_product( m_p.begin(), m_p.end(), m_b.begin(), double(0.0) );
    scale = (float)((s > FLT_EPSILON) ? ((UCHAR_MAX/2.0)/(MULTIPLIER*sqrt( fabs( s ) ))) : 0.0);
  }

  // Fill color image. All pixels remain grayscaled.
  {
    for (int v = 0; v < H; v++)
    {
      const float * src = pSource->row_begin( v );
      RGBQUAD     * dst = pImage->row_begin( v );

      for (int u = 0; u < W; u++)
      {
        int       x = u - hW;
        int       y = v - hH;
        RGBQUAD & q = dst[u];
        float     m = (float)(x*(x*m_p[0] + y*m_p[1] + m_p[3]) + y*(y*m_p[2] + m_p[4]) + m_p[5]);
#if 0
        float     f = (float)((src[u] - m) * scale + (UCHAR_MAX/2.0));
#else
        float     f = (float)(src[u] * (UCHAR_MAX/2.0) / (m+1.0));
#endif
        int       v;

#ifdef _M_IX86
        __asm {
          fld   f
          fistp v
        }
#else
        v = (int)f;
#endif

        q.rgbBlue = (q.rgbGreen = (q.rgbRed = (ubyte)(ALIB_LIMIT( v, 0, UCHAR_MAX ))));
        q.rgbReserved = (ubyte)0;
      }
    }
  }
}

