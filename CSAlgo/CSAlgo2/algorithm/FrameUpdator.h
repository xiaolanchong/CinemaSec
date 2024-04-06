/****************************************************************************
  FrameUpdator.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csalgo2
{

//=================================================================================================
/** \class FrameUpdator.
    \brief FrameUpdator. */
//=================================================================================================
class FrameUpdator
{
public:
  enum { FIRST_FRAME_NUMBER = 40 };
  enum Method { UNKNOWN = 0, ROBUST_L1 = 1, ROBUST_L2 = 2, CONVENTIONAL = 3, KALMAN = 4 };

private:
  Arr2f  m_image;            //!< 
  Arr2f  m_covariance;       //!< 
  Arr2f  m_oldFrame;         //!< 
  float  m_noise;            //!< 
  float  m_measurementNoise; //!< deviation of measurement noise
  float  m_rate;             //!< 
  float  m_snRatio;          //!< 
  Method m_method;           //!< 
  int    m_firstCount;       //!< the number of several first frames that should be just accumulated

public:
  FrameUpdator();
  virtual ~FrameUpdator();

  void Clear();
  void Initialize( int width, int height, float signal2noise, Method method );
  void Update( const Arr2f & frame, float noise, std::ostream * pLog = 0 );

  Arr2f & Image() { return m_image; }
  const Arr2f & Image() const { return m_image; }
  const float Noise() const { return m_noise; }
};

} // csalgo2

