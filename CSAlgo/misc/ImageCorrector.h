/****************************************************************************
  ImageCorrector.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

//=================================================================================================
/** \class ImageCorrector.
    \brief ImageCorrector. */
//=================================================================================================
class ImageCorrector
{
private:
  int       m_width;   //!< image width
  int       m_height;  //!< image height
  FloatArr  m_p;       //!< vector of quadratic approximation parameters
  DoubleArr m_b;       //!< temporal vector
  dMat      m_Q;       //!< inverted covariance matrix of quadratic approximation

private:
  bool Preprocess( int width, int height );

public:
  ImageCorrector();
  void DoCorrection( const Arr2f * pSource, QImage * pImage );
};

