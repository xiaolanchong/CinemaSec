///////////////////////////////////////////////////////////////////////////////////////////////////
// DynamicAccumulator.cpp
// ---------------------
// begin     : 30 Jun 2005
// modified  : 31 Oct 2005
// author(s) : Alexander Boltnev, Albert Akhriev
// email     : Alexander.Boltnev@biones.com, Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DynamicAccumulator.h"
#include "SubImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
DynamicAccumulator::DynamicAccumulator()
{
  Reset();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Destructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
DynamicAccumulator::~DynamicAccumulator()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function initializes this accumulator.

  \param  rect  bounding rectangle of a chair of interest. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void DynamicAccumulator::Initialize( const ARect & rect )
{
  m_timeMeans.resize( rect.area() );
  Reset();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function resets this accumulator. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void DynamicAccumulator::Reset()
{
  m_frameCnt = 0;
  std::fill( m_timeMeans.begin(), m_timeMeans.end(), DDPair(0.0,0.0) );
  memset( m_histogram, 0, sizeof(m_histogram) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function updates this accumulator given new frame.

  The input image undergoes normalization, which makes subsequent algorithm insensitive to
  an arbitrary linear transformation of the image: <tt>I<sup>*</sup>=aI+b</tt>.
  Function simultaneously accumulates the histogram of brightness deviation from time means:
  \f[
  \begin{array}{l}
  {\mbox{\bf for all points}\,\,(x,y)\,\,\mbox{\bf do:}} \hfill \\
  \hfill \\
  {r = \left| {\left( {\frac{1}{T}\sum\limits_{t=1}^T {I_t (x,y)} } \right) - I_{T+1}(x,y)} \right|} \hfill \\
  \hfill \\
  {H[r] \,\,\, \leftarrow \,\,\, H[r] + 1} \hfill \\
  \end{array}
  \f]

  \param  frame    the current frame.
  \param  rect     bounding rectangle of a chair of interest.
  \param  weights  normalized mask of weights of chair points (sum of all weights = 1).
  \param  buffer   temporal buffer. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void DynamicAccumulator::Update( const Arr2f & frame,
                                 const ARect & rect,
                                 const Arr2f & weights,
                                 UByteArr    & buffer )
{
  const int N = rect.area();
  int       bufferSize = N*sizeof(float);
  Arr2f     subImage;

  ASSERT( rect <= frame.rect() );
  ASSERT( alib::AreDimensionsEqual( weights, rect ) );
  ASSERT( (int)(m_timeMeans.size()) == N );

  // Prepare normalized sub-image, which contains the chair of interest.
  if ((int)(buffer.size()) < bufferSize)
    buffer.resize( bufferSize );
  subImage.wrap( weights.width(), weights.height(), reinterpret_cast<float*>( &(buffer[0]) ) );
  csalgo2::CopySubImage( frame, rect, subImage );
  csalgo2::NormalizeSubImage( subImage, weights, true );

  // Accumulate time means and time squares.
  {
    DynamicAccumulator::DDPair * p = &(m_timeMeans[0]);
    const float                * s = subImage.begin();

    if (m_frameCnt > MIN_FRAME_NUMBER) // enough statistics?
    {
      const float revFrameCnt = (float)(1.0/(double)m_frameCnt);

      for (int i = 0; i < N; i++)
      {
        int n = alib::RoundFloat( (float)(HISTO_SCALE * fabs( (p[i]).first * revFrameCnt - s[i] )) );
        ++(m_histogram[ std::min( n, HISTO_SIZE-1 ) ]);

        (p[i]).first += s[i];
        (p[i]).second += s[i]*s[i];
      }
    }
    else
    {
      for (int i = 0; i < N; i++)
      {
        (p[i]).first += s[i];
        (p[i]).second += s[i]*s[i];
      }
    }
  }

  ++m_frameCnt;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function calculates dynamic characteristics of an object inside a chair.

  At each point (x,y) the function computes time variance of image brightness (first formula).
  Noise deviation is estimated as the median of deviation distribution (second formula).
  Resultant time deviation of image brightness is given as robust average of individual point
  deviations corrected by noise one (third formula).
  \f[
  \begin{array}{l}
  {s_T^2 (x,y) = \left( {\frac{1}{T} \sum \limits _{t=1}^T {I_t^2 (x,y)} } \right) -
                 \left( {\frac{1}{T} \sum \limits _{t=1}^T {I_t (x,y)} } \right)^2 } \hfill \\
  \hfill \\
  {\sigma _{noise}^2  = \left( {median\left( {H_T } \right)} \right)^2 } \hfill  \\
  \hfill \\
  {\sigma _T  = \sum \limits _{(x,y)} {w(x,y) \sqrt {\max \left( {\left( {s_T^2 (x,y) -
                                   \sigma _{noise}^2 } \right),0} \right)}}} \hfill  \\
  \end{array}
  \f]

  \param  weights  normalized mask of weights of chair points (sum of all weights = 1).
  \return          the mean time deviation of image brightness within a chair. */
///////////////////////////////////////////////////////////////////////////////////////////////////
float DynamicAccumulator::GetDynamicCharacteristic( const Arr2f & weights ) const
{
  double         deviation = 0.0;
  int            N = (int)(m_timeMeans.size());
  const DDPair * p = &(m_timeMeans[0]);
  const float  * w = weights.begin();

  if ((m_frameCnt < MIN_FRAME_NUMBER) || (N == 0))
    return 0.0f;
  ASSERT( weights.size() == N );

  const double  revFrameCnt = 1.0/(double)m_frameCnt;
  const __int64 area = std::accumulate( m_histogram, m_histogram+HISTO_SIZE, __int64(0) );
  const __int64 fraction = area/2;
  int           median = 0;
  float         sqNoise = 0;

  // Estimate noise deviation as the median of accumulated distibution.
  for (__int64 sum = 0; (median < HISTO_SIZE) && (sum < fraction); median++)
  {
    sum += m_histogram[median];
  }
  sqNoise = alib::Sqr( (float)median/(float)HISTO_SCALE );

  // Compute the overall time deviation.
  for (int i = 0; i < N; i++)
  {
    double avr = (p[i]).first * revFrameCnt;
    double var = (p[i]).second * revFrameCnt - avr * avr - sqNoise;
    deviation += w[i] * sqrt( std::max( var, DBL_EPSILON ) );
  }

  return ((float)deviation);
}

