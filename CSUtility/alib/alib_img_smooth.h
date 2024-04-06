#pragma once

#include "alib_small_sort.h"

////////////////////////////////////////////////////////////////////////////
// F U N C T I O N S:
////////////////////////////////////////////////////////////////////////////
//
// void ConvolveImageHorz(const Arr2D<T> & source, const float * const filter,
//                        int size, bool bSmooth, Arr2D<T> & dest)
//
// void ConvolveImageVert(const Arr2D<T> & source, const float * const filter,
//                        int size, bool bSmooth, Arr2D<T> & dest)
//
// void OrdinarySmoothImage(const Arr2D<T> & source, const float * fx, 
//                          const float * fy, int size, Arr2D<T> & image)
//
// void GaussSmoothImage(Arr2D<T> & image, float sigma)
//
// void RepeatedlySmoothImage(T * image, int W, int H, int nRepeat)
//
////////////////////////////////////////////////////////////////////////////

namespace alib
{

//--------------------------------------------------------------------------
// Function convolves image with 1-D filter along X-direction. Source
// image can coincide with destination one. Each 1-D filter should be
// represented by its half, including central sample: f[0],f[1],...f[n-1].
// The function proposes that f[-i] == f[+i] for smoothing filter, and
// f[-i] == -f[i] for differentiating one. While smoothing, if 1-D filter
// tries to address pixel outside of a scan line, then the first or last
// scan's pixel is taken instead of absent one.
//--------------------------------------------------------------------------
template<class T, class P>
void ConvolveImageHorz(
  const Arr2D<T,P>  & source,     // image to be convolved
  const float * const filter,     // filter
  int                 size,       // size of filter
  bool                bSmooth,    // smooth image, otherwise differentiate
  Arr2D<T,P>        & dest)       // out: result
{
  int  W = source.width();
  int  H = source.height();
  T ** pDst = 0;
  T    t, row[ALIB_MAX_IMAGE_DIM];
  
  assertex(W > size && H > size && W <= ALIB_MAX_IMAGE_DIM);
  if (!alib::AreDimensionsEqual(dest, source))
    dest.resize(W, H);

  pDst = dest.Data();
  for(int y = 0; y < H; y++)
  {
    const T * pRow = source.row_begin(y);
    int       x;

    for(x = 0; x < W; x++)
      row[x] = pRow[x];

    for(x = 0; x < W; x++)
    {
      int L = W-1;
      if (bSmooth)
      {
        t = row[x]*filter[0];
        for(int k = 1; k < size; k++)
        {
          int x1 = x-k;
          int x2 = x+k;
          if (x1 < 0) x1 = 0;
          if (x2 > L) x2 = L;
          t += (row[x2]+row[x1])*filter[k];
        }
      }
      else
      {
        t = T();
        for(int k = 1; k < size; k++)
        {
          int x1 = x-k;
          int x2 = x+k;
          if (x1 < 0) x1 = 0;
          if (x2 > L) x2 = L;
          t += (row[x2]-row[x1])*filter[k];
        }
      }
      pDst[y][x] = t;
    }
  } 
}


//--------------------------------------------------------------------------
// Function convolves image with 1-D filter along Y-direction. Source
// image can coincide with destination one. Each 1-D filter should be
// represented by its half, including central sample: f[0],f[1],...f[n-1].
// The function proposes that f[-i] == f[+i] for smoothing filter, and
// f[-i] == -f[i] for differentiating one. While smoothing, if 1-D filter
// tries to address pixel outside of a scan line, then the first or last
// scan's pixel is taken instead of absent one.
//--------------------------------------------------------------------------
template<class T, class P>
void ConvolveImageVert(
  const Arr2D<T,P>  & source,     // image to be convolved
  const float * const filter,     // filter
  int                 size,       // size of filter
  bool                bSmooth,    // smooth image, otherwise differentiate
  Arr2D<T,P>        & dest)       // out: result
{
  int  W = source.width();
  int  H = source.height();
  T ** pDst = 0;
  T    t, column[ALIB_MAX_IMAGE_DIM];
  
  assertex(W > size && H > size && H <= ALIB_MAX_IMAGE_DIM);
  if (!alib::AreDimensionsEqual(dest, source))
    dest.resize(W, H);

  pDst = dest.Data();
  for(int x = 0; x < W; x++)
  {
    const T * pCol = source.BeginColumn(x);
    int       y;

    for(y = 0; y < H; y++)
    {
      column[y] = (*pCol);
      pCol += W;
    }

    for(y = 0; y < H; y++)
    {
      int L = H-1;
      if (bSmooth)
      {
        t = column[y]*filter[0];
        for(int k = 1; k < size; k++)
        {
          int y1 = y-k;
          int y2 = y+k;
          if (y1 < 0) y1 = 0;
          if (y2 > L) y2 = L;
          t += (column[y2]+column[y1])*filter[k];
        }
      }
      else
      {
        t = T();
        for(int k = 1; k < size; k++)
        {
          int y1 = y-k;
          int y2 = y+k;
          if (y1 < 0) y1 = 0;
          if (y2 > L) y2 = L;
          t += (column[y2]-column[y1])*filter[k];
        }
      }
      pDst[y][x] = t;
    }
  }
}


//--------------------------------------------------------------------------
// Function smoothes image in ordinary way - convolving by smoothing filter.
// 2-D filter should be separable, i.e. it can be represented by independent
// 1-D filters acting along X and Y axes.
//--------------------------------------------------------------------------
template<class T, class P>
void OrdinarySmoothImage(
  const Arr2D<T,P> & source,        // image to be smoothed
  const float      * fx,            // x-filter
  const float      * fy,            // y-filter
  int                size,          // size of filters
  Arr2D<T,P>       & image)         // out: result of smoothing
{
  alib::ConvolveImageHorz(source, fx, size, true, image);
  alib::ConvolveImageVert(image, fy, size, true, image);
}


//--------------------------------------------------------------------------
// Function smoothes image using Gaussian filter. While smoothing, if filter
// tries to address pixel outside of a scan line, then the first or last
// scan's pixel is taken instead of absent one.
//--------------------------------------------------------------------------
template<class T, class P>
void GaussSmoothImage(
  Arr2D<T,P> & image,     // in: source image,  out: smoothed image
  float        sigma)     // sigma of smoothing
{
  if (image.size() == 0)
    return;
  sigma = ALIB_LIMIT(sigma, 1.0f, 50.0f);

  int     W = image.width();
  int     H = image.height();
  T **    pImg = image.Data();
  double  scale = 0.0;
  double  revSqSigma = (-1.0)/(2.0*ALIB_SQR(sigma));
  double  step = 0.01;
  double  halfStep = 0.5*step;
  int     filterSize = (int)(4.0*sigma);
  float * filter = new float [ filterSize ];
  T     * buffer = new T [ __max(W,H) + 2*filterSize ];
  T     * pScan = buffer + filterSize;
  T       t;
  int     k, x, y;
  
  // Compute filter.
  for(k = 0; k < filterSize; k++)
  {
    double x1 = k-0.5;
    double x2 = k+0.5-FLT_EPSILON;
    double sum = 0.0;

    for(double x = x1; x < x2; x += step)
    {
      double t = x + halfStep;
      sum += exp(t*t*revSqSigma);
    }
    filter[k] = (float)sum;
    scale += (k == 0) ? sum : (sum+sum);
  }
  scale = 1.0/scale;
  for(k = 0; k < filterSize; k++)
    filter[k] *= (float)scale;

  // Smooth along horizontal scans.
  for(y = 0; y < H; y++)
  {
    int L = W-1;
    for(x = 0; x < W; x++)
      pScan[x] = pImg[y][x];
    for(x = 1; x < filterSize; x++)
    {
      pScan[-x] = pScan[0];
      pScan[L+x] = pScan[L];
    }
    
    for(x = 0; x < W; x++)
    {
      t = pScan[x] * filter[0];
      for(k = 1; k < filterSize; k++)
      {
        t += (pScan[x+k] + pScan[x-k]) * filter[k];
      }
      pImg[y][x] = t;
    }
  }

  // Smooth along vertical scans.
  for(x = 0; x < W; x++)
  {
    int L = H-1;
    for(y = 0; y < H; y++)
      pScan[y] = pImg[y][x];
    for(y = 1; y < filterSize; y++)
    {
      pScan[-y] = pScan[0];
      pScan[L+y] = pScan[L];
    }

    for(y = 0; y < H; y++)
    {
      t = pScan[y] * filter[0];
      for(k = 1; k < filterSize; k++)
      {
        t += (pScan[y+k] + pScan[y-k]) * filter[k];
      }
      pImg[y][x] = t;
    }
  }
  
  delete [] filter;
  delete [] buffer;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function repeatedly smoothes an image by the mask (1,2,1),
           alternating vertical and horizontal directions.

  On each iteration, the 2D-mask has the following elements:
  \f[ M = \left( \begin{array}{ccc} 1 & 2 & 1 \\ 2 & 4 & 2 \\ 1 & 2 & 1 \end{array} \right) \f]
  Entire image will be smoothed, including border pixels.
  Variance of equivalent Guassian is roughly equal to:
  \f[ sigma \approx \sqrt{nRepeat/2} \f]

  \param  image    image to be smoothed.
  \param  nRepeat  number of repetitions. */
//-------------------------------------------------------------------------------------------------
template< class IMAGE >
void RepeatedlySmoothImage( IMAGE & image, int nRepeat )
{
  typedef  typename IMAGE::precise_type  PRECISE;
  typedef  typename IMAGE::value_type    VALTYPE;

  ASSERT( std::numeric_limits< PRECISE >::is_specialized );
  ASSERT( std::numeric_limits< VALTYPE >::is_specialized );

  int        W = image.width();
  int        H = image.height();
  const bool bInt = std::numeric_limits< VALTYPE >::is_integer;
  const bool bSignedInt = (bInt && std::numeric_limits< VALTYPE >::is_signed);

  if ((W < 2) || (H < 2))
    return;

  std::vector< PRECISE >  buffer( 3*W );

  // Repeat smoothing several times.
  for (; nRepeat > 0; nRepeat--)
  {
    VALTYPE * img = image.begin();
    VALTYPE * row = img;
    PRECISE * pre = &(*(buffer.begin()));
    PRECISE * cur = pre + W;
    PRECISE * nxt = cur + W;

    // Read and smooth the first row.
    for (int x = 1; x < (W-1); x++)
    {
      cur[x] = (pre[x] = row[x-1] + row[x] + row[x] + row[x+1]);
    }
    cur[ 0 ] = (pre[ 0 ] = row[ 0 ] + row[ 0 ] + row[ 0 ] + row[ 1 ]);
    cur[W-1] = (pre[W-1] = row[W-2] + row[W-1] + row[W-1] + row[W-1]);

    // Go along the image row by row...
    for (int y = 0; y < H; y++)
    {
      // Read and smooth the next row.
      if ((y+1) < H)
      {
        row = (img+W);
        for (int x = 1; x < (W-1); x++)
        {
          nxt[x] = row[x-1] + row[x] + row[x] + row[x+1];
        }
        nxt[ 0 ] = row[ 0 ] + row[ 0 ] + row[ 0 ] + row[ 1 ];
        nxt[W-1] = row[W-2] + row[W-1] + row[W-1] + row[W-1];
      }
      else nxt = cur;

      // Smooth the image in vertical direction scaling result appropriately.
      if (bSignedInt) // 'signed integer' pixels
      {
        for (int x = 0; x < W; x++)
        {
          PRECISE t = pre[x] + cur[x] + cur[x] + nxt[x];
          PRECISE e = (PRECISE)((t >= 0) ? (+8) : (-8));

          img[x] = (VALTYPE)((t + e)/16);
        }
      }
      else if (bInt) // 'unsigned integer' pixels
      {
        for (int x = 0; x < W; x++)
        {
          PRECISE t = pre[x] + cur[x] + cur[x] + nxt[x];

          img[x] = (VALTYPE)((t + 8)/16);
        }
      }
      else // 'float' pixels
      {
        for (int x = 0; x < W; x++)
        {
          img[x] = (VALTYPE)((pre[x] + cur[x] + cur[x] + nxt[x])*(1.0f/16.0f));
        }
      }

      PRECISE * tmp = pre;   pre = cur;   cur = nxt;   nxt = tmp;
      img += W;
    }

    ASSERT( img == image.end() );
  }
}


//=================================================================================================
/** \class RepetitiveColorImageSmoother.
    \brief Function-object repeatedly smoothes inplace a color image by the mask (1,2,1),
           alternating vertical and horizontal directions. */
//=================================================================================================
template< class IMAGE >
class RepetitiveColorImageSmoother
{
  typedef  typename IMAGE::value_type    PIXEL;
  typedef  typename PIXEL::precise_type  PRECISE;
  typedef  typename PIXEL::value_type    VALTYPE;

  struct Triple
  {
    PRECISE r, g, b;

    Triple & operator+=( const PIXEL & x )  { r += x.r;  g += x.g;  b += x.b;  return (*this); }

    Triple & operator+=( const Triple & x ) { r += x.r;  g += x.g;  b += x.b;  return (*this); }

    Triple & operator=( const PIXEL & x )   { r  = x.r;  g  = x.g;  b  = x.b;  return (*this); }

    void get_normalized_pixel( PIXEL & p ) const
    {
      if (std::numeric_limits<VALTYPE>::is_integer)
      {
        if (std::numeric_limits<VALTYPE>::is_signed)
        {
          p.r = (VALTYPE)((r + ((r >= 0) ? 8 : -8)) / 16);
          p.g = (VALTYPE)((g + ((g >= 0) ? 8 : -8)) / 16);
          p.b = (VALTYPE)((b + ((b >= 0) ? 8 : -8)) / 16);
        }
        else
        {
          p.r = (VALTYPE)((r+8) >> 4);
          p.g = (VALTYPE)((g+8) >> 4);
          p.b = (VALTYPE)((b+8) >> 4);
        }
      }
      else
      {
        p.r = (VALTYPE)(r * (1.0f/16.0f));
        p.g = (VALTYPE)(g * (1.0f/16.0f));
        p.b = (VALTYPE)(b * (1.0f/16.0f));
      }
    }
  };

private:
  std::vector<char> m_buffer; //!< temporal buffer

public:
  /** \brief Constructor. */
  RepetitiveColorImageSmoother()
  {
    ASSERT( std::numeric_limits<PRECISE>::is_specialized );
    ASSERT( std::numeric_limits<VALTYPE>::is_specialized );
    ASSERT( std::numeric_limits<PRECISE>::is_integer == std::numeric_limits<VALTYPE>::is_integer );
    ASSERT( std::numeric_limits<PRECISE>::is_signed == std::numeric_limits<VALTYPE>::is_signed );
    ASSERT( sizeof(PRECISE) >= sizeof(VALTYPE) );
    ASSERT( sizeof(char) == 1 );
  }

  /** \brief Function fulfils repetitive image smoothing. */
  void operator()( IMAGE & image, int nRepeat )
  {
    int    W = image.width();
    int    H = image.height();
    Triple t;

    if ((W < 2) || (H < 2))
      return;
    m_buffer.resize( 3*W*sizeof(Triple) );

    // Repeat smoothing several times.
    for (; nRepeat > 0; nRepeat--)
    {
      PIXEL  * img = image.begin();
      PIXEL  * row = img;
      Triple * pre = reinterpret_cast<Triple*>( &(*(m_buffer.begin())) );
      Triple * cur = pre + W;
      Triple * nxt = cur + W;

      // Read and smooth the first row.
      for (int x = 1; x < (W-1); x++)
      {
        cur[x] = ((((pre[x] = row[x-1]) += row[x]) += row[x]) += row[x+1]);
      }
      cur[ 0 ] = ((((pre[ 0 ] = row[ 0 ]) += row[ 0 ]) += row[ 0 ]) += row[ 1 ]);
      cur[W-1] = ((((pre[W-1] = row[W-2]) += row[W-1]) += row[W-1]) += row[W-1]);

      // Go along the image row by row...
      for (int y = 0; y < H; y++)
      {
        // Read and smooth the next row.
        if ((y+1) < H)
        {
          row = (img+W);
          for (int x = 1; x < (W-1); x++)
          {
            (((nxt[x] = row[x-1]) += row[x]) += row[x]) += row[x+1];
          }
          (((nxt[ 0 ] = row[ 0 ]) += row[ 0 ]) += row[ 0 ]) += row[ 1 ];
          (((nxt[W-1] = row[W-2]) += row[W-1]) += row[W-1]) += row[W-1];
        }
        else nxt = cur;

        // Smooth the image in vertical direction scaling result appropriately.
        for (int x = 0; x < W; x++)
        {
          ((((t = pre[x]) += cur[x]) += cur[x]) += nxt[x]).get_normalized_pixel( img[x] );
        }

        // Cyclically swap scan pointers.
        Triple * tmp = pre;   pre = cur;   cur = nxt;   nxt = tmp;
        img += W;
      }

      ASSERT( img == image.end() );
    }
  }
};


//-------------------------------------------------------------------------------------------------
/** \brief Function repeatedly applies 3x3 median filter to a color image.

  \param  image      the image to be processed.
  \param  nRepeat    the number of repetitions.
  \param  tmpBuffer  temporal buffer. */
//-------------------------------------------------------------------------------------------------
template< class IMAGE >
void RepeatedlyApply3x3MedianFilterToColorImage( IMAGE & image, int nRepeat, UByteArr & tmpBuffer )
{
  typedef  typename IMAGE::value_type       pixel_type;
  typedef  typename pixel_type::value_type  color_type;

  int W = image.width();
  int H = image.height();

  ASSERT( std::numeric_limits<color_type>::is_specialized );
  if ((W < 2) || (H < 2))
    return;

  tmpBuffer.resize( 3*(W+2)*sizeof(pixel_type) );

  pixel_type * buffer = reinterpret_cast<pixel_type*>( &(*(tmpBuffer)) );
  color_type   r[9], g[9], b[9];

  // Repeat smoothing several times.
  while (--nRepeat >= 0)
  {
    pixel_type * img = image.begin();
    pixel_type * row = img;
    pixel_type * pre = buffer + (W+2)*0 + 1;
    pixel_type * cur = buffer + (W+2)*1 + 1;
    pixel_type * nxt = buffer + (W+2)*2 + 1;

    // Read the first row.
    for (int x = 0; x < W; x++) { cur[x] = (pre[x] = row[x]); }
    cur[-1] = (pre[-1] = row[0]);
    cur[W] = (pre[W] = row[W-1]);

    // Go along the image row by row...
    for (int y = 0; y < H; y++)
    {
      // Read the next row.
      if ((y+1) < H)
      {
        row = (img+W);
        for (int x = 0; x < W; x++) { nxt[x] = row[x]; }
        nxt[-1] = row[0];
        nxt[W] = row[W-1];
      }
      else nxt = cur;

      for (int x = 0; x < W; x++)
      {
        int xm = x-1;
        int xp = x+1;

        r[0] = (pre[xm]).r;  g[0] = (pre[xm]).g;  b[0] = (pre[xm]).b;
        r[1] = (pre[x ]).r;  g[1] = (pre[x ]).g;  b[1] = (pre[x ]).b;
        r[2] = (pre[xp]).r;  g[2] = (pre[xp]).g;  b[2] = (pre[xp]).b;
                                                    
        r[3] = (cur[xm]).r;  g[3] = (cur[xm]).g;  b[3] = (cur[xm]).b;
        r[4] = (cur[x ]).r;  g[4] = (cur[x ]).g;  b[4] = (cur[x ]).b;
        r[5] = (cur[xp]).r;  g[5] = (cur[xp]).g;  b[5] = (cur[xp]).b;
                                                    
        r[6] = (nxt[xm]).r;  g[6] = (nxt[xm]).g;  b[6] = (nxt[xm]).b;
        r[7] = (nxt[x ]).r;  g[7] = (nxt[x ]).g;  b[7] = (nxt[x ]).b;
        r[8] = (nxt[xp]).r;  g[8] = (nxt[xp]).g;  b[8] = (nxt[xp]).b;

        AscendantSortNine( r, 9 );
        AscendantSortNine( g, 9 );
        AscendantSortNine( b, 9 );

        (img[x]).r = r[4];
        (img[x]).g = g[4];
        (img[x]).b = b[4];
      }

      pixel_type * tmp = pre;   pre = cur;   cur = nxt;   nxt = tmp;
      img += W;
    }

    ASSERT( img == image.end() );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function repeatedly applies 5x5 median filter to a color image.

  \param  image      the image to be processed.
  \param  nRepeat    the number of repetitions.
  \param  tmpBuffer  temporal buffer. */
//-------------------------------------------------------------------------------------------------
template< class IMAGE >
void RepeatedlyApply5x5MedianFilterToColorImage( IMAGE & image, int nRepeat, UByteArr & tmpBuffer )
{
  typedef  typename IMAGE::value_type       pixel_type;
  typedef  typename pixel_type::value_type  color_type;

  int W = image.width();
  int H = image.height();

  ASSERT( std::numeric_limits<color_type>::is_specialized );
  if ((W < 3) || (H < 3))
    return;

  tmpBuffer.resize( 5*(W+4)*sizeof(pixel_type) );

  pixel_type * buffer = reinterpret_cast<pixel_type*>( &(*(tmpBuffer)) );
  color_type   r[25], g[25], b[25];

  // Repeat smoothing several times.
  while (--nRepeat >= 0)
  {
    pixel_type * img = image.begin();
    pixel_type * row = img;
    pixel_type * p[5];
    int          i, j, x, y;

    // Init row pointers.
    for (i = 0; i < 5; i++)
      p[i] = (buffer + (W+4)*i + 2);

    // Read the first and the second rows.
    for (x = 0; x < W; x++)
    {
      p[0][x] = (p[1][x] = (p[2][x] = row[x]));
      p[3][x] = row[x+W];
    }

    // Expand ends.
    for (i = 0; i < 4; i++)
    {
      p[i][-2] = (p[i][-1] = p[i][0]);
      p[i][W] = (p[i][W+1] = p[i][W-1]);
    }

    // Go along the image row by row...
    for (y = 0; y < H; y++)
    {
      // Read the next row.
      if ((y+2) < H)
      {
        row = (img+2*W);
        for (x = 0; x < W; x++)
          p[4][x] = row[x];

        p[4][-2] = (p[4][-1] = p[4][0]);
        p[4][W] = (p[4][W+1] = p[4][W-1]);
      }
      else p[4] = p[3];

      for (x = 0; x < W; x++)
      {
        int xm2 = x-2;
        int xm1 = x-1;
        int xp1 = x+1;
        int xp2 = x+2;

        for (i = 0; i < 5; i++)
        {
          j = 5*i;
          r[j+0] = (p[i][xm2]).r;   g[j+0] = (p[i][xm2]).g;   b[j+0] = (p[i][xm2]).b;
          r[j+1] = (p[i][xm1]).r;   g[j+1] = (p[i][xm1]).g;   b[j+1] = (p[i][xm1]).b;
          r[j+2] = (p[i][ x ]).r;   g[j+2] = (p[i][ x ]).g;   b[j+2] = (p[i][ x ]).b;
          r[j+3] = (p[i][xp1]).r;   g[j+3] = (p[i][xp1]).g;   b[j+3] = (p[i][xp1]).b;
          r[j+4] = (p[i][xp2]).r;   g[j+4] = (p[i][xp2]).g;   b[j+4] = (p[i][xp2]).b;
        }

        AscendantSortTwentyFive( r, 25 );
        AscendantSortTwentyFive( g, 25 );
        AscendantSortTwentyFive( b, 25 );

        (img[x]).r = r[12];
        (img[x]).g = g[12];
        (img[x]).b = b[12];
      }

      pixel_type * tmp = p[0];
                         p[0] = p[1];
                                p[1] = p[2];
                                       p[2] = p[3];
                                              p[3] = p[4];
                                                     p[4] = tmp;
      img += W;
    }

    ASSERT( img == image.end() );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function repeatedly applies 5x5 median filter to a grayscaled image.

  \param  image      the image to be processed.
  \param  nRepeat    the number of repetitions.
  \param  tmpBuffer  temporal buffer. */
//-------------------------------------------------------------------------------------------------
template< class IMAGE >
void RepeatedlyApply5x5MedianFilterGrayImage( IMAGE & image, int nRepeat, UByteArr & tmpBuffer )
{
  typedef  typename IMAGE::value_type  pixel_type;

  int W = image.width();
  int H = image.height();
  if ((W < 3) || (H < 3))
    return;

  tmpBuffer.resize( 5*(W+4)*sizeof(pixel_type) );

  pixel_type * buffer = reinterpret_cast<pixel_type*>( &(*(tmpBuffer.begin())) );
  pixel_type   t[25];

  // Repeat smoothing several times.
  while (--nRepeat >= 0)
  {
    pixel_type * img = image.begin();
    pixel_type * row = img;
    pixel_type * p[5];
    int          i, j, x, y;

    // Init row pointers.
    for (i = 0; i < 5; i++)
      p[i] = (buffer + (W+4)*i + 2);

    // Read the first and the second rows.
    for (x = 0; x < W; x++)
    {
      p[0][x] = (p[1][x] = (p[2][x] = row[x]));
      p[3][x] = row[x+W];
    }

    // Expand ends.
    for (i = 0; i < 4; i++)
    {
      p[i][-2] = (p[i][-1] = p[i][0]);
      p[i][W] = (p[i][W+1] = p[i][W-1]);
    }

    // Go along the image row by row...
    for (y = 0; y < H; y++)
    {
      // Read the next row.
      if ((y+2) < H)
      {
        row = (img+2*W);
        for (x = 0; x < W; x++)
          p[4][x] = row[x];

        p[4][-2] = (p[4][-1] = p[4][0]);
        p[4][W] = (p[4][W+1] = p[4][W-1]);
      }
      else p[4] = p[3];

      for (x = 0; x < W; x++)
      {
        int xm2 = x-2;
        int xm1 = x-1;
        int xp1 = x+1;
        int xp2 = x+2;

        for (i = 0; i < 5; i++)
        {
          j = 5*i;
          t[j+0] = p[i][xm2];
          t[j+1] = p[i][xm1];
          t[j+2] = p[i][ x ];
          t[j+3] = p[i][xp1];
          t[j+4] = p[i][xp2];
        }

        AscendantSortTwentyFive( t, 25 );
        img[x] = t[12];
      }

      pixel_type * tmp = p[0];
                         p[0] = p[1];
                                p[1] = p[2];
                                       p[2] = p[3];
                                              p[3] = p[4];
                                                     p[4] = tmp;
      img += W;
    }

    ASSERT( img == image.end() );
  }
}

} // namespace alib

