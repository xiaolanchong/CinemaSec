/****************************************************************************
  ImageNormalizer.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csalgocommon
{

//=================================================================================================
/** \struct ImageNormalizer.
    \brief  Function-object fulfils normalization over a region of specified image. */
//=================================================================================================
template< class IMAGE, class POINT_ARRAY >
class ImageNormalizer
{
public:
  typedef  typename IMAGE::value_type  pixel_type;

  enum Method { UNKNOWN = 0, MEAN_NORMALIZATION, DEVIATION_NORMALIZATION };

private:
  Method m_method;      //!< method to be used
  double m_mean;        //!< image mean intensity value
  double m_dev;         //!< image mean intensity deviation
  double m_scale;       //!< scale factor used to convert in 'float' format
  double m_byteMean;    //!< image mean used to convert in 'byte' format
  double m_byteScale;   //!< scale factor used to convert in 'byte' format

public:
  /** \brief Function clears this object. */
  void Clear()
  {
    m_method = UNKNOWN;
    m_mean = 0.0;
    m_dev = 0.0;
    m_scale = 0.0;
    m_byteMean = 0.0;
    m_byteScale = 0.0;
  }

  /** \brief Constructor. */
  ImageNormalizer()
  {
    Clear();
  }

  /** \brief Function computes normalization parameters given an image and a region. */
  void Initialize( const IMAGE & image, const POINT_ARRAY & points, bool bMeanNormalization )
  {
    MeanVarAccum<double> accum;

    Clear();
    for (int i = 0, n = (int)(points.size()); i < n; i++)
      accum += (double)(image[ points[i] ]);
    accum.statistics( &m_mean, 0, &m_dev );

    if (bMeanNormalization)
    {
      m_method = MEAN_NORMALIZATION;
      m_scale = alib::Reciprocal( m_mean );
      m_byteScale = (UCHAR_MAX/2) * alib::Reciprocal( m_mean );
    }
    else
    {
      m_method = DEVIATION_NORMALIZATION;
      m_scale = alib::Reciprocal( m_dev );
      m_byteScale = (UCHAR_MAX/6.0)*m_scale;
      m_byteMean = (UCHAR_MAX/2.0) - m_byteScale*m_mean;
    }
  }


  /** \brief Function computes normalization parameters given an image and a region. */
  void Initialize( const IMAGE & image, bool bMeanNormalization )
  {
    MeanVarAccum<double> accum;

    Clear();
    for (int i = 0, n = image.size(); i < n; i++)
      accum += (double)(image[i]);
    accum.statistics( &m_mean, 0, &m_dev );

    if (bMeanNormalization)
    {
      m_method = MEAN_NORMALIZATION;
      m_scale = alib::Reciprocal( m_mean );
      m_byteScale = (UCHAR_MAX/2) * alib::Reciprocal( m_mean );
    }
    else
    {
      m_method = DEVIATION_NORMALIZATION;
      m_scale = alib::Reciprocal( m_dev );
      m_byteScale = (UCHAR_MAX/6.0)*m_scale;
      m_byteMean = (UCHAR_MAX/2.0) - m_byteScale*m_mean;
    }
  }

  /** \brief Function does normalization of specified intensity value into 'float' format. */
  float GetNormalizedValue( pixel_type x ) const
  {
    ASSERT( m_method != UNKNOWN );
    return ((m_method == MEAN_NORMALIZATION) ? (float)(x*m_scale) : (float)((x-m_mean)*m_scale));
  }

  /** \brief Function does normalization of specified intensity value into 'byte' format. */
  ubyte GetByteNormalizedValue( pixel_type x ) const
  {
    ASSERT( m_method != UNKNOWN );
    int t = (m_method == MEAN_NORMALIZATION) ? (int)(x*m_byteScale) : (int)(x*m_byteScale+m_byteMean);
    return ((ubyte)ALIB_LIMIT( t, 0, UCHAR_MAX ));
  }
};

} // csalgocommon

