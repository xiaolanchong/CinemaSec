/****************************************************************************
  ImageStandardizer.h
  ---------------------
  begin     : 25 Jul 2005
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

//=================================================================================================
/** \class ImageStandardizer.
    \brief ImageStandardizer. */
//=================================================================================================
class CS_ALGO_COMMON_API ImageStandardizer
{
private:
  bool   m_bMeanNormalize;
  int    m_resultWidth;
  int    m_resultHeight;
  Arr2f  m_intermediateImg;
  bool   m_bReady;

  Vec2iArr points;

private:
  void ProjectiveTransform( const float coefs[8], float & x, float & y );
  void RectToQuadrangleTransformCoefs( const Vec2f quadrangle[4], int width, int height, float coefs[8] );
  void QuadrangleToRectTransformCoefs( const Vec2f quadrangle[4], int width, int height, float coefs[8] );


public:
  ImageStandardizer();
  ~ImageStandardizer();

  void Clear();

  void Initialize( bool bMeanNormalize, int resultWidth, int resultHeight, int factor );

  static bool GetQuadrangleFromCurve( const Vec2fArr & curve, float inflation, Vec2f quadrangle[4] );

  bool GetStandardImage( const Arr2f & bigImage, const Vec2f quadrangle[4], Arr2ub & result );
  bool GetStandardImage( const Arr2f & bigImage, const Vec2fArr & curve, Arr2ub & result, float inflation = 0.0f);

};


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
inline void ImageStandardizer::ProjectiveTransform( const float coefs[8], float & x, float & y )
{
  float  tx = x;
  float  ty = y;
  double denominator = coefs[6]*tx + coefs[7]*ty + 1.0;

  x = (float)((coefs[0]*tx + coefs[1]*ty + coefs[2]) / denominator);
  y = (float)((coefs[3]*tx + coefs[4]*ty + coefs[5]) / denominator);
}

