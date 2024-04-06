///////////////////////////////////////////////////////////////////////////////////////////////////
// CorrelateChair.cpp
// ---------------------
// begin     : Aug 2004
// modified  : 2 Nov 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlgoTypes.h"
#include "SubImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csalgo2
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function computes normalized cross-correlation based on L1 or L2 norm.
///
/// \param  frame          the current frame.
/// \param  rect           rectangle of (shifted) chair location inside current frame.
/// \param  chair          the chair of interest.
/// \param  bNormL1        use robust, but expensive L1 norm if nonzero, otherwise L2 norm.
/// \param  brightnessThr  threshold on minimal brightness of input frame.
/// \param  buffer         temporal buffer.
/// \return                correlation difference.
///////////////////////////////////////////////////////////////////////////////////////////////////
float CorrelateChair( const Arr2f & frame,
                      const ARect & rect,
                      const Chair & chair,
                      const bool    bNormL1,
                      const float   brightnessThr,
                      UByteArr    & buffer ) throw(...)
{
  ASSERT( rect <= frame.rect() );
  ASSERT( !(chair.samples.empty()) );
  ASSERT( alib::AreDimensionsEqual( rect, chair.rect ) );
  ASSERT( alib::AreDimensionsEqual( rect, chair.weights ) );
  ASSERT( alib::AreDimensionsEqual( rect, chair.samples[0] ) );

  const int N = rect.area();
  float     result = FLT_MAX;
  int       bufferSize = N*sizeof(float);
  Arr2f     subImage;

  // Construct normalized sub-image extracted from the current frame according to chair's position.
  if ((int)(buffer.size()) < bufferSize)
    buffer.resize( bufferSize );
  subImage.wrap( rect.width(), rect.height(), reinterpret_cast<float*>( &(buffer[0]) ) );
  csalgo2::CopySubImage( frame, rect, subImage );
  if (csalgo2::NormalizeSubImage( subImage, chair.weights, bNormL1 ) < brightnessThr)
    return (chair.statEntity);  // return old value

  // Compute the minimal difference over the set of template images.
  for (csalgo2::Image2fSet::const_iterator i = chair.samples.begin(); i != chair.samples.end(); ++i)
  {
    float diff = DifferenceBetweenNormalizedSubImages( subImage, *i, chair.weights, bNormL1 );
    result = std::min( result, diff );
  }
  return result;
}

} // namespace csalgo2

