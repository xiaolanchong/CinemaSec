/****************************************************************************
  frame_updator.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "FrameUpdator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  _CALC_MEAN_SIGMA_  1

namespace csalgo2
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
FrameUpdator::FrameUpdator()
{
  Clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Destructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
FrameUpdator::~FrameUpdator()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void FrameUpdator::Clear()
{
  m_image.clear();
  m_covariance.clear();
  m_oldFrame.clear();
  m_noise = 1.0f;
  m_measurementNoise = 1.0f;
  m_rate = 0.5f;
  m_snRatio = 10.0f;
  m_method = UNKNOWN;
  m_firstCount = FIRST_FRAME_NUMBER;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void FrameUpdator::Update( const Arr2f & frame, float noise, std::ostream * pLog )
{
  ALIB_ASSERT( !(frame.empty()) );
  ASSERT( alib::AreDimensionsEqual( frame, m_image ) );
  ASSERT( alib::AreDimensionsEqual( frame, m_covariance ) );
  noise = std::max( noise, MIN_NOISE_DEVIATION );

  // Print logging information.
  if (pLog != 0) (*pLog) << "FrameUpdator:" << std::endl;

  if (m_method == KALMAN)
  {
    if (m_oldFrame.empty())
    {
      m_noise = 1.0;
      m_image = frame;
      m_oldFrame = frame;
      m_covariance.resize( frame.width(), frame.height(), 1.0f, true );
      return;
    }

    ALIB_ASSERT( alib::AreDimensionsEqual( frame, m_oldFrame ) );
    ALIB_ASSERT( alib::AreDimensionsEqual( frame, m_covariance ) );

#if 1  // if 1, then use robust Kalman filter

    const float   SIGMA_SCALE = 3.0f;
    const float   sqImageNoise = alib::Sqr( m_noise );
    const float   sqMeasurementNoise = alib::Sqr( m_measurementNoise );
    const float   sm = m_measurementNoise * SIGMA_SCALE; sm;
    const float * F = frame.begin();
    float       * I = m_image.begin();
    float       * P = m_covariance.begin();
    float       * O = m_oldFrame.begin();
    int           N = frame.size();
    double        measurementNoise = 0.0, imageNoise = 0.0;

    for (int i = 0; i < N; i++)
    {
      float P_prior = P[i] + sqImageNoise;
      float K = (float)(P_prior/(P_prior + sqMeasurementNoise));
      float one_minus_K = 1.0f - K;
      float t = I[i];
#if 0
      float f = t + (F[i] - t)*(sm/(sm + fabs( F[i] - t )));
#else
      float f = F[i];
#endif

      I[i] = one_minus_K * t + K * f;
      P[i] = one_minus_K * P_prior;
      imageNoise += fabs( t - I[i] );
      measurementNoise += fabs( f - O[i] );
      O[i] = f;
    }
    m_noise = (float)(imageNoise/N);
    m_measurementNoise = (float)(measurementNoise/N);

#else // , otherwise conventional one

    const double SCALE = 2.5;
    const int    HISTO_SIZE = (4*MAX_INTENSITY_VALUE+1);
    int          histogram[HISTO_SIZE];
    const float  sqImageNoise = alib::Sqr( this->m_noise );
    const float  sqMeasurementNoise = alib::Sqr( noise );
    const int    W = frame.width();
    const int    H = frame.height();

    memset( histogram, 0, sizeof(histogram) );
    for (int y = 0; y < H; y++)
    {
      const float * F = frame.row_begin( y );
      float       * I = m_image.row_begin( y );
      float       * P = m_covariance.row_begin( y );

      for (int x = 0; x < W; x++)
      {
        float P_prior = P[x] + sqImageNoise;
        float K = P_prior/(P_prior + sqMeasurementNoise);
        float one_minus_K = 1.0f - K;

        I[x] = one_minus_K * I[x] + K * F[x];
        P[x] = one_minus_K * P_prior;

        if ((y > 0) && (x > 0))
        {
          int h = (int)fabs( I[x] + I[x-1-W] - I[x-1] - I[x-W] );
          if (h < HISTO_SIZE)
            histogram[h] += 1;
        }
      }
    }

    this->m_noise = csalgocommon::ProcessOnePolymaskNoiseHistogram( histogram, HISTO_SIZE, SCALE );

#endif

    // Print logging information.
    if (pLog != 0)
    {
      (*pLog) << "method=Kalman filter" << std::endl
              << "measurement noise=" << measurementNoise << std::endl
              << "residual noise deviation=" << m_noise << std::endl << std::endl;
    }
  }
  else
  {
    float  a = m_rate;
    float  b = (1.0f - a);
    double mean = 0.0;
    double var = 0.0;
    int    N = m_image.size();

    #if _CALC_MEAN_SIGMA_
    MeanAccum<double> saccum;
    #endif

    // Just accumulate several first frames.
    if (m_firstCount > 0)
    {
      if (m_firstCount == FIRST_FRAME_NUMBER)
        std::fill( m_covariance.begin(), m_covariance.end(), 0.0f );

      for (int k = 0; k < N; k++)
      {
        float f = frame[k];
        m_image[k] += f;
        m_covariance[k] += f*f;
        mean += f;
        var += f*f;
      }

      if (--m_firstCount == 0)
      {
        for (int k = 0; k < N; k++)
        {
          float   m = (m_image[k] /= (float)FIRST_FRAME_NUMBER);
          float & s = ((m_covariance[k] /= (float)FIRST_FRAME_NUMBER) -= m*m);

          if (s < 1.0f) s = 1.0f;
          if (m_method == ROBUST_L1) s = (float)sqrt( s );
        }
      }
    }
    else // ... otherwise update current frame and compute image deviation
    {
      if (m_method == ROBUST_L1)
      {
        const float SIGMA_SCALE = 2.5f;

        for (int k = 0; k < N; k++)
        {
          float & i = m_image[k];
          float & s = m_covariance[k];
          float   ss = s * SIGMA_SCALE;
          float   d = frame[k] - i;

          d = d*(ss/(ss+fabs(d)));
          s = a*s + b*fabs(d);
          i += b*d;
          if (s < 1.0f) s = 1.0f;
          mean += i;
          var += i*i;

          #if _CALC_MEAN_SIGMA_
          saccum += s;
          #endif
        }
      }
      else if (m_method == ROBUST_L2)
      {
        const float SIGMA_SCALE = alib::Sqr( 1.5f );

        for (int k = 0; k < N; k++)
        {
          float & i = m_image[k];
          float & s = m_covariance[k];
          float   ss = s * SIGMA_SCALE;
          float   d = frame[k] - i;

          d = d*sqrt( ss/(ss+d*d) );
          s = a*s + b*d*d;
          i += b*d;
          if (s < 1.0f) s = 1.0f;
          mean += i;
          var += i*i;

          #if _CALC_MEAN_SIGMA_
          saccum += sqrt(s);
          #endif
        }
      }
      else if (m_method == CONVENTIONAL)
      {
        for (int k = 0; k < N; k++)
        {
          float & i = m_image[k];
          i = a*i + b*frame[k];
          mean += i;
          var += i*i;
        }
      }
      else ALIB_THROW( L"Unknown update method" );
    }

    mean /= N;
    var = std::max<double>( (var/N - mean*mean), 1.0 );

    // Undate noise deviation.
    this->m_noise = sqrt( alib::Sqr( this->m_noise*a ) + alib::Sqr( noise*b ) );

    // Update the rate of updating.
    float r0r0 = m_snRatio * m_snRatio;
    float rr = (float)(var/(noise*noise));
    m_rate = std::max( (r0r0 - rr)/(r0r0 + rr), 0.5f );

    // Print logging information.
    if (pLog != 0)
    {
      (*pLog) << "method=" << ((m_method == ROBUST_L1)    ? "RobustL1"     :
                              ((m_method == ROBUST_L2)    ? "RobustL2"     :
                              ((m_method == CONVENTIONAL) ? "Conventional" : ""))) << std::endl
              << "residual noise deviation=" << this->m_noise << std::endl
              #if _CALC_MEAN_SIGMA_
              ; if (saccum.mean() > FLT_EPSILON) (*pLog) << "mean sigma=" << saccum.mean() << std::endl;
              (*pLog)
              #endif
              << "update rate=" << m_rate << std::endl << std::endl;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void FrameUpdator::Initialize( int width, int height, float signal2noise, FrameUpdator::Method method )
{
  Clear();
  ALIB_ASSERT( alib::IsLimited( method, ROBUST_L1, KALMAN ) );
  m_snRatio = std::max( signal2noise, 2.0f );
  m_method = method;
  m_image.resize( width, height );
  m_covariance.resize( width, height, 1.0f );
  m_oldFrame.clear();
}

} // csalgo2

