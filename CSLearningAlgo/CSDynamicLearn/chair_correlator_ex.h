/****************************************************************************
  chair_correlator.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \class ChairCorrelatorEx.
    \brief Class implements various methods of correlation between a chair and a background. */
//=================================================================================================
template< class FIRST_IMAGE, class SECOND_IMAGE, class CHAIR >
class ChairCorrelatorEx
{
  typedef  std::pair<float,float>  FFPair;
  typedef  std::vector<FFPair>     FFPairArr;

private:
  bool      m_bNCC;       //!< normalized cross-correlation measure, otherwise mean-normalized one
  bool      m_bNormL2;    //!< use L2 norm in correlation measure, otherwise L1 norm
  FFPairArr m_values;     //!< temporal storage of intensities
  Vec2i     m_correction; //!< correction of chair position

private:
  //-------------------------------------------------------------------------------------------------
  /** \brief Function computes normalized cross-correlation with L2 norm.

  \param  frame       the current frame.
  \param  background  the background (either static or dynamic) image.
  \param  chair       the chair of interest.
  \param  diff        out: correlation difference ([MIN_CORR_DIFF,...,MAX_CORR_DIFF]).
  \return             Ok = true. */
  //-------------------------------------------------------------------------------------------------
  bool Correlate_NCC_L2( const FIRST_IMAGE  & frame,
                         const SECOND_IMAGE & background,
                         const CHAIR        & chair,
                         const float          frameNoise,
                         float              & diff )
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
        typename FIRST_IMAGE::const_pointer  p1 = frame.ptr( u1, v );
        typename SECOND_IMAGE::const_pointer p2 = background.ptr( x1, y );
        int                                  n = std::min( x2-x1, u2-u1 );

        N += n;
        for (int q = 0; q < n; q++)
        {
          float f1 = (float)(p1[q]);
          float f2 = (float)(p2[q]);

          avr1 += f1;   dev1 += f1*f1;
          avr2 += f2;   dev2 += f2*f2;   cross += f1*f2;
        }
      }
    }

    if (N == 0)
      return false;

    avr1 /= N;
    avr2 /= N;

    dev1 = std::max<double>( sqrt( fabs( (dev1/N) - avr1*avr1 ) ), frameNoise );
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
  bool Correlate_NCC_L1( const FIRST_IMAGE  & frame,
                         const SECOND_IMAGE & background,
                         const CHAIR        & chair,
                         const float          frameNoise,
                         float              & diff )
  {
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
          typename FIRST_IMAGE::const_pointer  p1 = frame.ptr( u1, v );
          typename SECOND_IMAGE::const_pointer p2 = background.ptr( x1, y );
          int                                  n = std::min( x2-x1, u2-u1 );

          ASSERT( (N+n) <= bufSize );
          for (int q = 0; q < n; q++, N++)
          {
            avr1 += ((m_values[N]).first  = (float)(p1[q]));
            avr2 += ((m_values[N]).second = (float)(p2[q]));
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

      revDev1 = 1.0/std::max( dev1, (double)frameNoise );
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
  bool Correlate_Mean_L2( const FIRST_IMAGE  & frame,
                          const SECOND_IMAGE & background,
                          const CHAIR        & chair,
                          const float          frameNoise,
                          float              & diff )
  {
    const HScanArr & scans = chair.region();
    const int        W = frame.width();
    const int        H = frame.height();
    double           revMean1 = 0.0, revMean2 = 0.0, cross = 0.0;
    int              N = 0, bufSize = chair.rect.area();

    frameNoise;
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
          typename FIRST_IMAGE::const_pointer  p1 = frame.ptr( u1, v );
          typename SECOND_IMAGE::const_pointer p2 = background.ptr( x1, y );
          int                                  n = std::min( x2-x1, u2-u1 );

          ASSERT( (N+n) <= bufSize );
          for (int q = 0; q < n; q++, N++)
          {
            revMean1 += alib::Sqr( (m_values[N]).first  = (float)(p1[q]) );
            revMean2 += alib::Sqr( (m_values[N]).second = (float)(p2[q]) );
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
  bool Correlate_Mean_L1( const FIRST_IMAGE  & frame,
                          const SECOND_IMAGE & background,
                          const CHAIR        & chair,
                          const float          frameNoise,
                          float              & diff )
  {
    const HScanArr & scans = chair.region();
    const int        W = frame.width();
    const int        H = frame.height();
    double           revMean1 = 0.0, revMean2 = 0.0, cross = 0.0;
    int              N = 0, bufSize = chair.rect.area();

    frameNoise;
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
          typename FIRST_IMAGE::const_pointer  p1 = frame.ptr( u1, v );
          typename SECOND_IMAGE::const_pointer p2 = background.ptr( x1, y );
          int                                  n = std::min( x2-x1, u2-u1 );

          ASSERT( (N+n) <= bufSize );
          for (int q = 0; q < n; q++, N++)
          {
            revMean1 += alib::Sqr( (m_values[N]).first  = (float)(p1[q]) );
            revMean2 += alib::Sqr( (m_values[N]).second = (float)(p2[q]) );
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

public:
  //-------------------------------------------------------------------------------------------------
  /** \brief Constructor. */
  //-------------------------------------------------------------------------------------------------
  ChairCorrelatorEx()
  {
    Clear();
  }


  //-------------------------------------------------------------------------------------------------
  /** \brief Function clears this object. */
  //-------------------------------------------------------------------------------------------------
  void Clear()
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
  bool Initialize( const csalgo::AlgorithmParams & params )
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
  bool Correlate( const FIRST_IMAGE  & frame,
                  const SECOND_IMAGE & background,
                  const CHAIR        & chair,
                  const float          frameNoise,
                  bool                 bCorrectPos,
                  float              & diff )
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
        return Correlate_NCC_L2( frame, background, chair, frameNoise, diff );
      else
        return Correlate_NCC_L1( frame, background, chair, frameNoise, diff );
    }
    else
    {
      if (m_bNormL2)
        return Correlate_Mean_L2( frame, background, chair, frameNoise, diff );
      else
        return Correlate_Mean_L1( frame, background, chair, frameNoise, diff );
    }
  }

};

