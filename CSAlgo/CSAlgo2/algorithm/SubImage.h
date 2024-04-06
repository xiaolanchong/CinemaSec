///////////////////////////////////////////////////////////////////////////////////////////////////
// SubImage.h
// ---------------------
// begin     : Aug 2004
// modified  : 28 Oct 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace csalgo2
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function extracts sub-image from the input frame.
///
/// \param  frame     the source frame.
/// \param  rect      bounding rectangle of a sub-image inside the frame.
/// \param  subImage  the destination sub-image.
///////////////////////////////////////////////////////////////////////////////////////////////////
inline void CopySubImage( const Arr2f & frame, const ARect & rect, Arr2f & subImage )
{
  ASSERT( !(rect.empty()) && (rect <= frame.rect()) );
  ALIB_ASSERT( alib::AreDimensionsEqual( subImage, rect ) );

  const int N = rect.area();
  const int W = rect.width();
  const int H = rect.height();
  int       i = 0;
  float *   s = subImage.begin();

  for (int y = 0; y < H; y++)
  {
    const float * f = frame.ptr( rect.x1, rect.y1+y );

    for (int x = 0; x < W; x++)
    {
      s[i++] = f[x];
    }
  }
  ASSERT( i == N );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function calculates mean and reciprocal deviation of an image.
///
/// Depending on specified norm, the mean and the deviation are computed as follows:
/// \f[
/// \begin{array}{rcl}
/// \{w_k \} &:& \sum \limits _k {w_k} = 1 \\
/// {\bar I} &=& \sum \limits _k {w_k I_k} \\ 
/// \sigma _{L_1} &=& \sum \limits _k {w_k \left| {I_k - \bar I} \right|} \\ 
/// \sigma _{L_2} &=& \sqrt{ \sum \limits _k {w_k \left( {I_k - \bar I} \right)^2}} \\ 
/// r\sigma &=& 1/\sigma
/// \end{array}
/// \f]
///
/// \param  image    the image of interest.
/// \param  weights  normalized weights of image points ().
/// \param  bNormL1  nonzero means L1 norm, otherwise L2 norm.
/// \return          mean brightness.
///////////////////////////////////////////////////////////////////////////////////////////////////
inline float NormalizeSubImage( Arr2f & image, const Arr2f & weights, const bool bNormL1 )
{
  ASSERT( !(image.empty()) && alib::AreDimensionsEqual( image, weights ) );

  float       *   i = image.begin();
  const float *   w = weights.begin();
  int             k, N = image.size();
  register double avr = 0.0, dev = 0.0;

  // Calculate mean and deviation.
  if (bNormL1)
  {
    for (k = 0; k < N; k++)
    {
      avr += w[k]*i[k];
    }

    for (k = 0; k < N; k++)
    {
      dev += w[k]*fabs( i[k] - avr );
    }
  }
  else // L2 norm
  {
    for (k = 0; k < N; k++)
    {
      float wi = w[k]*i[k];
      avr += wi;
      dev += wi*i[k];
    }
    dev = sqrt( fabs( dev - avr*avr ) );
  }

  // Calculate reciprocal deviation.
  dev = 1.0/std::max( dev, 1.0 );

  // Normalize image.
  for (k = 0; k < N; k++)
  {
    i[k] = (float)((i[k] - avr) * dev);
  }
  return ((float)avr);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function calculates difference between two normalized sub-images.
///
/// Depending on specified norm, the difference is computed as follows:
/// \f[ \delta _{L_1} = \sum \limits _k {w_k \left| {\left( {\frac{{I_{1,k} - \bar I_1}}
/// {{\sigma _1}}} \right) - \left( {\frac{{I_{2,k} - \bar I_2}}{{\sigma _2}}} \right)} \right|} \f]
/// \f[ \delta _{L_2} = \sqrt {\sum \limits _k {w_k \left( {\left( {\frac{{I_{1,k} - \bar I_1}}
/// {{\sigma _1}}} \right) - \left( {\frac{{I_{2,k} - \bar I_2}} {{\sigma _2}}} \right)} \right)^2}} \f]
///
/// \param  image1   the first normalized image of interest.
/// \param  image2   the second normalized image of interest.
/// \param  weights  normalized weights of image points (\f[ \sum \limits _k {w_k} = 1 \f]).
/// \param  bNormL1  nonzero means L1 norm, otherwise L2 norm.
///////////////////////////////////////////////////////////////////////////////////////////////////
inline float DifferenceBetweenNormalizedSubImages( const Arr2f & image1,
                                                   const Arr2f & image2,
                                                   const Arr2f & weights,
                                                   const bool    bNormL1 )
{
  ASSERT( !(image1.empty()) && alib::AreDimensionsEqual( image1, weights ) );
  ASSERT( !(image2.empty()) && alib::AreDimensionsEqual( image2, weights ) );

  const float *   i1 = image1.begin();
  const float *   i2 = image2.begin();
  const float *   w = weights.begin();
  register double diff = 0.0;

  if (bNormL1)
  {
    for (int k = 0, N = weights.size(); k < N; k++)
    {
      diff += w[k]*fabs( i1[k] - i2[k] );
    }
  }
  else // L2 norm
  {
    for (int k = 0, N = weights.size(); k < N; k++)
    {
      float t = (i1[k] - i2[k]);
      diff += w[k]*t*t;
    }
    ASSERT( ALIB_IS_LIMITED( diff, -0.01, 2.01 ) );
    diff = sqrt( fabs( diff ) );
  }
  return ((float)diff);
}

} // csalgo2

