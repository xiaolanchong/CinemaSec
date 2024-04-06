///////////////////////////////////////////////////////////////////////////////
  CorrectBrightness.cpp
  ---------------------
  begin     : Aug 2004
  date      : 21 Jul 2005
  author(s) : Alexander Boltnev, Albert Akhriev
  email     : Alexander.Boltnev@biones.com, Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csutility
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function corrects brightness scaling image intensity by local mean value. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CorrectBrightness( const Arr2f * pSource, int winDim, QImage * pResult, UByteArr * pTempBuffer )
{
  const int MAX_WINDOW_DIM = 128;
  const int dim = 2*(winDim/2)+1;  // do 'dim' odd!
  const int half = dim/2;

  if ((pSource == 0) || (pResult == 0) || (pTempBuffer == 0))
    return false;
  if ((pSource->width() < dim) || (pSource->height() < dim) || (dim > MAX_WINDOW_DIM))
    return false;
  pResult->resize2( *pSource, false );

  float  *    scan[MAX_WINDOW_DIM];
  double *    sumScan = 0;
  const int   W = pSource->width();
  const int   H = pSource->height();
  const int   W_1 = (W - 1);
  const int   H_1 = (H - 1);
  const int   dim_1 = (dim - 1);
  const int   middle_offset = (half + W*half);
  const float revWinSize = (float)(1.0/(double)(dim*dim));

  // Initialize scan pointers that point inside temporal buffer.
  {
    int tempBufferSize = (int)(W*(dim*sizeof(float) + sizeof(double)));
    if ((int)(pTempBuffer->size()) < tempBufferSize)
      pTempBuffer->resize( tempBufferSize );

    scan[0] = reinterpret_cast<float*>( &(*(pTempBuffer->begin())) );
    for (int i = 1; i < dim; i++)
    {
      scan[i] = scan[i-1] + W;
    }
    sumScan = reinterpret_cast<double*>( scan[dim-1] + W );
    ASSERT( reinterpret_cast<ubyte*>( sumScan+W ) <= &(*(pTempBuffer->end())) );
  }

  // Fill up the scan of vertical sums with zeros.
  std::fill( sumScan, sumScan+W, double(0.0) );

  // Compute means and do convertion.
  for (int y = 0; y < H; y++)
  {
    RGBQUAD     * res = pResult->row_begin( y );
    const float * src = pSource->row_begin( y );
    float       * curScan = scan[y%dim];
    float       * preScan = scan[(y+1)%dim];
    float         horzSum = 0.0f;

    // Compute sums inside sliding window along the current scan, and also compute sums across horizontal scans.
    for (int x = 0; x < W; x++)
    {
      // Add advanced value to horizontal sum.
      horzSum += src[x];

      if (x >= dim_1)
      {
        // Store new horizontal sums in the current scanline and add them to the vertical sums.
        sumScan[x] += (curScan[x] = horzSum);

        // Subtract the oldest value from the horizontal sum.
        horzSum -= src[x - dim_1];

        if (y >= dim_1)
        {
          // Obtain local mean value of the source image.
          float m = (float)(sumScan[x] * revWinSize);

          // Subtract the oldest horizontal sum from the vertical sum.
          sumScan[x] -= preScan[x];

          // Do convertion.
          RGBQUAD & q = res[x - middle_offset];
          uint      i = (uint)((UCHAR_MAX/2)*src[x - middle_offset]/(m+1.0) + 0.5);

          q.rgbBlue = (q.rgbGreen = (q.rgbRed = (ubyte)(std::min<uint>( i, UCHAR_MAX ))));
          q.rgbReserved = 0;

          if (x == dim_1)
          {
            for (int cntx = 0; cntx < half; cntx++)
            {
              RGBQUAD & q = res[x - middle_offset - cntx];
              uint      i = (uint)((UCHAR_MAX/2)*src[x - middle_offset - cntx]/(m+1.0) + 0.5);

              q.rgbBlue = (q.rgbGreen = (q.rgbRed = (ubyte)(std::min<uint>( i, UCHAR_MAX ))));
              q.rgbReserved = 0;

              if (y == dim_1)
              {
                for (int cnty = 0; cnty < half; cnty++)
                {
                  // Do convertion.
                  RGBQUAD & q = res[x - middle_offset - cnty*W - cntx];
                  uint      i = (uint)((UCHAR_MAX/2)*src[x - middle_offset - cnty*W  - cntx]/(m+1.0) + 0.5);

                  q.rgbBlue = (q.rgbGreen = (q.rgbRed = (ubyte)(std::min<uint>( i, UCHAR_MAX ))));
                  q.rgbReserved = 0;
                }
              }
            }
          }

          if (x == W_1)
          {
            for (int cntx = 0; cntx < half; cntx++)
            {
              RGBQUAD & q = res[x - middle_offset + cntx];
              uint      i = (uint)((UCHAR_MAX/2)*src[x - middle_offset + cntx]/(m+1.0) + 0.5);

              q.rgbBlue = (q.rgbGreen = (q.rgbRed = (ubyte)(std::min<uint>( i, UCHAR_MAX ))));
              q.rgbReserved = 0;

              if (y == dim_1)
              {
                for (int cnty = 0; cnty < half; cnty++)
                {
                  RGBQUAD & q = res[x - middle_offset - cnty*W + cntx];
                  uint      i = (uint)((UCHAR_MAX/2)*src[x - middle_offset - cnty*W + cntx]/(m+1.0) + 0.5);

                  q.rgbBlue = (q.rgbGreen = (q.rgbRed = (ubyte)(std::min<uint>( i, UCHAR_MAX ))));
                  q.rgbReserved = 0;
                }
              }
            }
          }

          if (y == dim_1)
          {
            for (int cnty = 0; cnty < half; cnty++)
            {
              RGBQUAD & q = res[x - middle_offset - cnty*W];
              uint      i = (uint)((UCHAR_MAX/2)*src[ x - middle_offset - cnty*W]/(m+1.0) + 0.5);

              q.rgbBlue = (q.rgbGreen = (q.rgbRed = (ubyte)(std::min<uint>( i, UCHAR_MAX ))));
              q.rgbReserved = 0;
            }
          }

          if (y == H_1)
          {
            for (int cnty = 0; cnty < half; cnty++)
            {
              RGBQUAD & q = res[x - middle_offset + cnty*W];
              uint      i = (uint)((UCHAR_MAX/2)*src[x - middle_offset + cnty*W]/(m+1.0) + 0.5);

              q.rgbBlue = (q.rgbGreen = (q.rgbRed = (ubyte)(std::min<uint>( i, UCHAR_MAX ))));
              q.rgbReserved = 0;

              if (x == W_1)
              {
                for (int cntx = 0; cntx < half; cntx++)
                {
                  RGBQUAD & q = res[x - middle_offset + cntx + cnty*W];
                  uint      i = (uint)((UCHAR_MAX/2)*src[x - middle_offset + cntx + cnty*W]/(m+1.0) + 0.5);

                  q.rgbBlue = (q.rgbGreen = (q.rgbRed = (ubyte)(std::min<uint>( i, UCHAR_MAX ))));
                  q.rgbReserved = 0;
                }
              }

              if (x == dim_1)
              {
                for (int cntx = 0; cntx < half; cntx++)
                {
                  RGBQUAD & q = res[x - middle_offset - cntx + cnty*W];
                  uint      i = (uint)((UCHAR_MAX/2)*src[x - middle_offset - cntx + cnty*W]/(m+1.0) + 0.5);

                  q.rgbBlue = (q.rgbGreen = (q.rgbRed = (ubyte)(std::min<uint>( i, UCHAR_MAX ))));
                  q.rgbReserved = 0;
                }
              }
            }
          }
        }
      }
    }
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function corrects brightness scaling image intensity by local mean value. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CorrectByteBrightness( const Arr2ub * pSource, int winDim, Arr2ub * pResult, UByteArr * pTempBuffer )
{
  const int MAX_WINDOW_DIM = 128;
  const int dim = 2*(winDim/2)+1;  // do 'dim' odd!
  const int half = dim/2;

  if ((pSource == 0) || (pResult == 0) || (pTempBuffer == 0))
    return false;
  if ((pSource->width() < dim) || (pSource->height() < dim) || (dim > MAX_WINDOW_DIM))
    return false;
  pResult->resize2( *pSource, false );

  float  *    scan[MAX_WINDOW_DIM];
  double *    sumScan = 0;
  const int   W = pSource->width();
  const int   H = pSource->height();
  const int   W_1 = (W - 1);
  const int   H_1 = (H - 1);
  const int   dim_1 = (dim - 1);
  const int   middle_offset = (half + W*half);
  const float revWinSize = (float)(1.0/(double)(dim*dim));

  // Initialize scan pointers that point inside temporal buffer.
  {
    int tempBufferSize = (int)(W*(dim*sizeof(float) + sizeof(double)));
    if ((int)(pTempBuffer->size()) < tempBufferSize)
      pTempBuffer->resize( tempBufferSize );

    scan[0] = reinterpret_cast<float*>( &(*(pTempBuffer->begin())) );
    for (int i = 1; i < dim; i++)
    {
      scan[i] = scan[i-1] + W;
    }
    sumScan = reinterpret_cast<double*>( scan[dim-1] + W );
    ASSERT( reinterpret_cast<ubyte*>( sumScan+W ) <= &(*(pTempBuffer->end())) );
  }

  // Fill up the scan of vertical sums with zeros.
  std::fill( sumScan, sumScan+W, double(0.0) );

  // Compute means and do convertion.
  for (int y = 0; y < H; y++)
  {
    ubyte       * res = pResult->row_begin( y );
    const ubyte * src = pSource->row_begin( y );
    float       * curScan = scan[y%dim];
    float       * preScan = scan[(y+1)%dim];
    float         horzSum = 0.0f;

    // Compute sums inside sliding window along the current scan, and also compute sums across horizontal scans.
    for (int x = 0; x < W; x++)
    {
      // Add advanced value to horizontal sum.
      horzSum += src[x];

      if (x >= dim_1)
      {
        // Store new horizontal sums in the current scanline and add them to the vertical sums.
        sumScan[x] += (curScan[x] = horzSum);

        // Subtract the oldest value from the horizontal sum.
        horzSum -= src[x - dim_1];

        if (y >= dim_1)
        {
          // Obtain local mean value of the source image.
          float m = (float)(sumScan[x] * revWinSize);

          // Subtract the oldest horizontal sum from the vertical sum.
          sumScan[x] -= preScan[x];

          // Do convertion.
          ubyte & q = res[x - middle_offset];
          uint    i = (uint)((UCHAR_MAX/2)*src[x - middle_offset]/(m+1.0) + 0.5);

          q = (ubyte)(std::min<uint>( i, UCHAR_MAX ));

          if (x == dim_1)
          {
            for (int cntx = 0; cntx < half; cntx++)
            {
              ubyte & q = res[x - middle_offset - cntx];
              uint    i = (uint)((UCHAR_MAX/2)*src[x - middle_offset - cntx]/(m+1.0) + 0.5);

              q = (ubyte)(std::min<uint>( i, UCHAR_MAX ));

              if (y == dim_1)
              {
                for (int cnty = 0; cnty < half; cnty++)
                {
                  // Do convertion.
                  ubyte & q = res[x - middle_offset - cnty*W - cntx];
                  uint    i = (uint)((UCHAR_MAX/2)*src[x - middle_offset - cnty*W  - cntx]/(m+1.0) + 0.5);

                  q = (ubyte)(std::min<uint>( i, UCHAR_MAX ));
                }
              }
            }
          }

          if (x == W_1)
          {
            for (int cntx = 0; cntx < half; cntx++)
            {
              ubyte & q = res[x - middle_offset + cntx];
              uint    i = (uint)((UCHAR_MAX/2)*src[x - middle_offset + cntx]/(m+1.0) + 0.5);

              q = (ubyte)(std::min<uint>( i, UCHAR_MAX ));

              if (y == dim_1)
              {
                for (int cnty = 0; cnty < half; cnty++)
                {
                  ubyte & q = res[x - middle_offset - cnty*W + cntx];
                  uint    i = (uint)((UCHAR_MAX/2)*src[x - middle_offset - cnty*W + cntx]/(m+1.0) + 0.5);

                  q = (ubyte)(std::min<uint>( i, UCHAR_MAX ));
                }
              }
            }
          }

          if (y == dim_1)
          {
            for (int cnty = 0; cnty < half; cnty++)
            {
              ubyte & q = res[x - middle_offset - cnty*W];
              uint    i = (uint)((UCHAR_MAX/2)*src[ x - middle_offset - cnty*W]/(m+1.0) + 0.5);

              q = (ubyte)(std::min<uint>( i, UCHAR_MAX ));
            }
          }

          if (y == H_1)
          {
            for (int cnty = 0; cnty < half; cnty++)
            {
              ubyte & q = res[x - middle_offset + cnty*W];
              uint    i = (uint)((UCHAR_MAX/2)*src[x - middle_offset + cnty*W]/(m+1.0) + 0.5);

              q = (ubyte)(std::min<uint>( i, UCHAR_MAX ));

              if (x == W_1)
              {
                for (int cntx = 0; cntx < half; cntx++)
                {
                  ubyte & q = res[x - middle_offset + cntx + cnty*W];
                  uint    i = (uint)((UCHAR_MAX/2)*src[x - middle_offset + cntx + cnty*W]/(m+1.0) + 0.5);

                  q = (ubyte)(std::min<uint>( i, UCHAR_MAX ));
                }
              }

              if (x == dim_1)
              {
                for (int cntx = 0; cntx < half; cntx++)
                {
                  ubyte & q = res[x - middle_offset - cntx + cnty*W];
                  uint    i = (uint)((UCHAR_MAX/2)*src[x - middle_offset - cntx + cnty*W]/(m+1.0) + 0.5);

                  q = (ubyte)(std::min<uint>( i, UCHAR_MAX ));
                }
              }
            }
          }
        }
      }
    }
  }

  return true;
}

} // namespace csutility

