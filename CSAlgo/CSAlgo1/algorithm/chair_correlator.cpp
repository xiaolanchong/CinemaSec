/****************************************************************************
  correlation.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#include "stdafx.h"
//#include "algo_types.h"
#include "algo_param.h"
#include "chair_state_data.h"
#include "chair.h"
#include "frame.h"
#include "image_property.h"
#include "image_acquirer.h"
#include "chair_correlator.h"
#include "utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
ChairCorrelator::ChairCorrelator()
{
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object. */
//-------------------------------------------------------------------------------------------------
void ChairCorrelator::Clear()
{
  m_bNCC = true;
  m_bNormL2 = true;
  m_values.clear();
  m_correction.set(0,0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes this object.

  \param  params  algorithm's parameters.
  \return         Ok = true. */
//-------------------------------------------------------------------------------------------------
bool ChairCorrelator::Initialize( const csalgo::AlgorithmParams & params )
{
  Clear();
  m_bNCC = params.p_NCC();
  m_bNormL2 = params.p_normL2();
  m_values.reserve( 1024 );
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes normalized correlation (NCC) between two images within specified chair.

  \param  frame        the current frame.
  \param  background   the background (either static or dynamic) image.
  \param  chair        the chair of interest.
  \param  bCorrectPos  if non-zero, then chair position should be corrected.
  \param  diff         out: correlation difference ([MIN_CORR_DIFF,...,MAX_CORR_DIFF]).
  \return              Ok = true. */
//-------------------------------------------------------------------------------------------------
bool ChairCorrelator::Correlate( const Frame & frame,
                                 const Arr2f & background,
                                 const Chair & chair,
                                 bool          bCorrectPos,
                                 float       & diff )
{
  if (bCorrectPos)
    m_correction = chair.correction;
  else
    m_correction.set(0,0);

  ASSERT( alib::AreDimensionsEqual( frame, background ) );
  diff = 0.0f;

  if (m_bNCC)
  {
    if (m_bNormL2)
      return Correlate_NCC_L2( frame, background, chair, diff );
    else
      return Correlate_NCC_L1( frame, background, chair, diff );
  }
  else
  {
    if (m_bNormL2)
      return Correlate_Mean_L2( frame, background, chair, diff );
    else
      return Correlate_Mean_L1( frame, background, chair, diff );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes normalized cross-correlation with L2 norm.

  \param  frame       the current frame.
  \param  background  the background (either static or dynamic) image.
  \param  chair       the chair of interest.
  \param  diff        out: correlation difference ([MIN_CORR_DIFF,...,MAX_CORR_DIFF]).
  \return             Ok = true. */
//-------------------------------------------------------------------------------------------------
bool ChairCorrelator::Correlate_NCC_L2( const Frame & frame,
                                        const Arr2f & background,
                                        const Chair & chair,
                                        float       & diff )
{
  const HScanArr & scans = chair.region();
  const int        W = frame.width();
  const int        H = frame.height();
  double           avr1 = 0.0, dev1 = 0.0, avr2 = 0.0, dev2 = 0.0, cross = 0.0;
  int              N = 0;

  for (HScanArr::const_iterator it = scans.begin(); it != scans.end(); ++it)
  {
    int x1 = (*it).x1;
    int x2 = (*it).x2;
    int u1 = x1 + m_correction.x;
    int u2 = x2 + m_correction.x;
    int y = (*it).y;
    int v = y + m_correction.y;

    ASSERT( (0 <= x1) && (x1 <= x2) && (x2 <= W) && ALIB_IS_RANGE( y, 0, H ) );

    if (u1 < 0) u1 = 0;
    if (u2 > W) u2 = W;

    if ((u2 > u1) && ALIB_IS_RANGE( v, 0, H ))
    {
      const float * i1 = frame.ptr( u1, v );
      const float * i2 = background.ptr( x1, y );
      int           n = std::min( x2-x1, u2-u1 );

      N += n;
      for (int q = 0; q < n; q++)
      {
        avr1 += (*i1);
        avr2 += (*i2);

        dev1 += (*i1)*(*i1);
        dev2 += (*i2)*(*i2);

        cross += (*i1)*(*i2);

        ++i1;
        ++i2;
      }
    }
  }

  if (N == 0)
    return false;

  avr1 /= N;
  avr2 /= N;

  dev1 = std::max<double>( sqrt( fabs( (dev1/N) - avr1*avr1 ) ), frame.noise );
  dev2 = std::max<double>( sqrt( fabs( (dev2/N) - avr2*avr2 ) ), MIN_NOISE_DEVIATION );

  diff = (float)(1.0 - ((cross/N)-(avr1*avr2))/(dev1*dev2));
  ASSERT( ALIB_IS_LIMITED( diff, -0.001f, 2.001f ) );
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes normalized cross-correlation with L1 norm.

  \param  frame       the current frame.
  \param  background  the background (either static or dynamic) image.
  \param  chair       the chair of interest.
  \param  diff        out: correlation difference ([MIN_CORR_DIFF,...,MAX_CORR_DIFF]).
  \return             Ok = true. */
//-------------------------------------------------------------------------------------------------
bool ChairCorrelator::Correlate_NCC_L1( const Frame & frame,
                                        const Arr2f & background,
                                        const Chair & chair,
                                        float       & diff )
{
  typedef  std::pair<float,float>  FltFltPair;

  const HScanArr & scans = chair.region();
  const int        W = frame.width();
  const int        H = frame.height();
  double           avr1 = 0.0, dev1 = 0.0, avr2 = 0.0, dev2 = 0.0, cross = 0.0;
  double           revDev1 = 0.0, revDev2 = 0.0;
  int              N = 0, bufSize = chair.rect.area();

  if ((int)(m_values.size()) < bufSize)
    m_values.resize( bufSize );

  // Compute mean m_values.
  {
    for (HScanArr::const_iterator it = scans.begin(); it != scans.end(); ++it)
    {
      int x1 = (*it).x1;
      int x2 = (*it).x2;
      int u1 = x1 + m_correction.x;
      int u2 = x2 + m_correction.x;
      int y = (*it).y;
      int v = y + m_correction.y;

      ASSERT( (0 <= x1) && (x1 <= x2) && (x2 <= W) && ALIB_IS_RANGE( y, 0, H ) );

      if (u1 < 0) u1 = 0;
      if (u2 > W) u2 = W;

      if ((u2 > u1) && ALIB_IS_RANGE( v, 0, H ))
      {
        const float * i1 = frame.ptr( u1, v );
        const float * i2 = background.ptr( x1, y );

        for (int q = std::min( x2-x1, u2-u1 ); q > 0; q--)
        {
          ASSERT( N < bufSize );
          avr1 += ((m_values[N]).first  = (*i1));
          avr2 += ((m_values[N]).second = (*i2));
          ++i1;
          ++i2;
          ++N;
        }
      }
    }

    if (N == 0)
      return false;

    avr1 /= N;
    avr2 /= N;
  }

  // Compute deviations.
  {
    for (int q = 0; q < N; q++)
    {
      dev1 += fabs( (m_values[q]).first  -= (float)avr1 );
      dev2 += fabs( (m_values[q]).second -= (float)avr2 );
    }

    dev1 /= N;
    dev2 /= N;

    revDev1 = 1.0/std::max( dev1, (double)frame.noise );
    revDev2 = 1.0/std::max( dev2, (double)MIN_NOISE_DEVIATION );
  }

  // Compute normalized cross-correlation.
  {
    for (int q = 0; q < N; q++)
    {
      cross += fabs( (m_values[q]).first * revDev1 - (m_values[q]).second * revDev2 );
    }
  }

  diff = (float)(cross/N);
  ASSERT( ALIB_IS_LIMITED( diff, -0.001f, 2.001f ) );
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes mean-normalized correlation with L2 norm.

  \param  frame       the current frame.
  \param  background  the background (either static or dynamic) image.
  \param  chair       the chair of interest.
  \param  diff        out: correlation difference ([MIN_CORR_DIFF,...,MAX_CORR_DIFF]).
  \return             Ok = true. */
//-------------------------------------------------------------------------------------------------
bool ChairCorrelator::Correlate_Mean_L2( const Frame & frame,
                                         const Arr2f & background,
                                         const Chair & chair,
                                         float       & diff )
{
  typedef  std::pair<float,float>  FltFltPair;

  const HScanArr & scans = chair.region();
  const int        W = frame.width();
  const int        H = frame.height();
  double           revMean1 = 0.0, revMean2 = 0.0, cross = 0.0;
  int              N = 0, bufSize = chair.rect.area();

  if ((int)(m_values.size()) < bufSize)
    m_values.resize( bufSize );

  // Compute mean m_values.
  {
    for (HScanArr::const_iterator it = scans.begin(); it != scans.end(); ++it)
    {
      int x1 = (*it).x1;
      int x2 = (*it).x2;
      int u1 = x1 + m_correction.x;
      int u2 = x2 + m_correction.x;
      int y = (*it).y;
      int v = y + m_correction.y;

      ASSERT( (0 <= x1) && (x1 <= x2) && (x2 <= W) && ALIB_IS_RANGE( y, 0, H ) );

      if (u1 < 0) u1 = 0;
      if (u2 > W) u2 = W;

      if ((u2 > u1) && ALIB_IS_RANGE( v, 0, H ))
      {
        const float * i1 = frame.ptr( u1, v );
        const float * i2 = background.ptr( x1, y );

        for (int q = std::min( x2-x1, u2-u1 ); q > 0; q--)
        {
          ASSERT( N < bufSize );
          revMean1 += alib::Sqr( (m_values[N]).first  = (*i1) );
          revMean2 += alib::Sqr( (m_values[N]).second = (*i2) );
          ++i1;
          ++i2;
          ++N;
        }
      }
    }

    if ((N == 0) || (revMean1 < FLT_EPSILON) || (revMean2 < FLT_EPSILON))
      return false;

    revMean1 = sqrt( fabs( N/revMean1 ) );
    revMean2 = sqrt( fabs( N/revMean2 ) );
  }

  // Compute normalize cross-correlation.
  {
    for (int q = 0; q < N; q++)
    {
      cross += alib::Sqr( (m_values[q]).first * revMean1 - (m_values[q]).second * revMean2 );
    }
  }

  diff = (float)sqrt( fabs( cross/N ) );
  ASSERT( ALIB_IS_LIMITED( diff, -0.001f, 2.001f ) );
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes mean-normalized correlation with L1 norm.

  \param  frame       the current frame.
  \param  background  the background (either static or dynamic) image.
  \param  chair       the chair of interest.
  \param  diff        out: correlation difference ([MIN_CORR_DIFF,...,MAX_CORR_DIFF]).
  \return             Ok = true. */
//-------------------------------------------------------------------------------------------------
bool ChairCorrelator::Correlate_Mean_L1( const Frame & frame,
                                         const Arr2f & background,
                                         const Chair & chair,
                                         float       & diff )
{
  typedef  std::pair<float,float>  FltFltPair;

  const HScanArr & scans = chair.region();
  const int        W = frame.width();
  const int        H = frame.height();
  double           revMean1 = 0.0, revMean2 = 0.0, cross = 0.0;
  int              N = 0, bufSize = chair.rect.area();

  if ((int)(m_values.size()) < bufSize)
    m_values.resize( bufSize );

  // Compute reversed mean values.
  {
    for (HScanArr::const_iterator it = scans.begin(); it != scans.end(); ++it)
    {
      int x1 = (*it).x1;
      int x2 = (*it).x2;
      int u1 = x1 + m_correction.x;
      int u2 = x2 + m_correction.x;
      int y = (*it).y;
      int v = y + m_correction.y;

      ASSERT( (0 <= x1) && (x1 <= x2) && (x2 <= W) && ALIB_IS_RANGE( y, 0, H ) );

      if (u1 < 0) u1 = 0;
      if (u2 > W) u2 = W;

      if ((u2 > u1) && ALIB_IS_RANGE( v, 0, H ))
      {
        const float * i1 = frame.ptr( u1, v );
        const float * i2 = background.ptr( x1, y );

        for (int q = std::min( x2-x1, u2-u1 ); q > 0; q--)
        {
          ASSERT( N < bufSize );
          revMean1 += fabs( (m_values[N]).first  = (*i1) );
          revMean2 += fabs( (m_values[N]).second = (*i2) );
          ++i1;
          ++i2;
          ++N;
        }
      }
    }

    if ((N == 0) || (revMean1 < FLT_EPSILON) || (revMean2 < FLT_EPSILON))
      return false;

    revMean1 = (N/revMean1);
    revMean2 = (N/revMean2);
  }

  // Compute normalize cross-correlation.
  {
    for (int q = 0; q < N; q++)
    {
      cross += fabs( (m_values[q]).first * revMean1 - (m_values[q]).second * revMean2 );
    }
  }

  diff = (float)(cross/N);
  ASSERT( ALIB_IS_LIMITED( diff, -0.001f, 2.001f ) );
  return true;
}

