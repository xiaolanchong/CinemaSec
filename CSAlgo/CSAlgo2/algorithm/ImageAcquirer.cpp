/****************************************************************************
  ImageAcquirer.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "ImageAcquirer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csalgo2
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of ImageAcquireParams.
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
ImageAcquireParams::ImageAcquireParams( int & controlIdentifier )
{
  p_updateMethod.init(
    L"updateMethod",
    L"updating method: (1) - Robust L1, (2) - Robust L2, (3) - Conventional, (4) - Kalman filter ",
    L'1', L"1234", this, &controlIdentifier );

  p_signal2noise.init(
    L"signal2noise",
    L"min. signal to noise ratio ",
    50.0f, 2.0f, 100.0f, this, &controlIdentifier );

  p_meanDiffThr.init(
    L"meanDiffThr",
    L"threshold on relative difference of mean intensities of both half-frames ",
    0.1f, 0.0f, 1.0f, this, &controlIdentifier );

  p_minMeanIntensity.init(
    L"minMeanIntensity",
    L"threshold on minimal mean intensity of a frame, [0..255] ",
    30.0f, 1.0f, 255.0f, this, &controlIdentifier );

  p_nSmoothRepeat.init(
    L"nSmoothRepeat",
    L"the number of times acquired image must be repeatedly smoothed, zero means no smoothing ",
    0, 0, 10, this, &controlIdentifier );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of ImageAcquirer.
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
ImageAcquirer::ImageAcquirer()
{
  Clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Destructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
ImageAcquirer::~ImageAcquirer()
{
  Clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function clears this object. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void ImageAcquirer::Clear()
{
  m_frame.clear();
  m_property.clear();
  m_meanDiffThr = 0.0f;
  m_minMeanIntensity = 0.0f;
  m_droppedFrameNum = 0;
  m_frameNum = 0;
  m_updator.Clear();
  m_tempBuffer.clear();
  m_brightnessThr = 0;
  m_percentageThr = 0;
  m_dayNightState = -1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function initializes the process of image acquisition and measurement.

  \return  Ok = true. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void ImageAcquirer::Initialize( int width, int height, const csalgo2::ImageAcquireParams & params,
                                int brightnessThreshold, int percentageThreshold )
{
  Clear();
  m_frame.resize( width, height );
  m_meanDiffThr = params.p_meanDiffThr();
  m_minMeanIntensity = std::max<float>( params.p_minMeanIntensity(), 1.0f );
  m_updator.Initialize( width, height, params.p_signal2noise(),
                        (FrameUpdator::Method)(params.p_updateMethod() - L'1' + 1) );

  ALIB_ASSERT( alib::IsRange( brightnessThreshold, 0, (int)UCHAR_MAX ) );
  ALIB_ASSERT( alib::IsRange( percentageThreshold, 0, 100 ) );
  m_brightnessThr = brightnessThreshold;
  m_percentageThr = percentageThreshold;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function gets new frame and estimates whether
           it can be fed into the main algorithm or must be dropped.

  \param  pDib  pointer to the header of a DIB.
  \param  pImg  pointer to the image content.
  \return       nonzero if loaded frame is reliable, otherwise it must be dropped. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ImageAcquirer::Acquire( const BITMAPINFO * pDib, const ubyte * pImg, std::ostream * pLog )
{
  ALIB_ASSERT( (pDib != 0) && (pImg != 0) );
  ALIB_ASSERT( m_frame.width()  == abs( pDib->bmiHeader.biWidth  ) );
  ALIB_ASSERT( m_frame.height() == abs( pDib->bmiHeader.biHeight ) );

  // Measure image characteristics and convert DIB image into float one.
  ALIB_ASSERT( GaugeAndConvertImage( pDib, pImg ) );

  // Check that there is no abnormal mean intensity difference between half-frames of the same frame.
  double nomin = fabs( m_property.imgMean[0] - m_property.imgMean[1] );
  double denom = 0.5*( m_property.imgMean[0] + m_property.imgMean[1] );
  bool   bDropFrame = ((denom < m_minMeanIntensity) || ((nomin/denom) > m_meanDiffThr));

  if (bDropFrame)
    ++m_droppedFrameNum;

  // Print logging information.
  if (pLog != 0)
  {
    (*pLog) << A_LINE_SEPARATOR << std::endl
            << "frame: " << m_frameNum << std::endl
            << A_LINE_SEPARATOR << std::endl
            << "ImageAcquirer:" << std::endl
            << "noise: dev1=" << std::setprecision(4) << m_property.noiseDeviation[0]
            <<     ",  dev2=" << std::setprecision(4) << m_property.noiseDeviation[1]
            <<     ",  overall=" << std::setprecision(4) << m_property.noise << std::endl
            << "image: mean1=" << std::setprecision(4) << m_property.imgMean[0]
            <<     ",  mean2=" << std::setprecision(4) << m_property.imgMean[1]
            <<     ",  dev1=" << std::setprecision(4) << m_property.imgDeviation[0]
            <<     ",  dev2=" << std::setprecision(4) << m_property.imgDeviation[1] << std::endl
            << "dropped frames number=" << m_droppedFrameNum << std::endl
            << "frame state=" << (bDropFrame ? "dropped" : "passed") << std::endl
            << "day/night=" << m_dayNightState << std::endl << std::endl;
  }

  if (!bDropFrame)
    m_updator.Update( m_frame, m_property.noise, pLog );

  ++m_frameNum;
  return (!bDropFrame);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function computes noise deviation along with some characteristics of an input image
           by applying 2x2 polynomial mask, and also converts the input image into float format.

  There are two groups of image properties - one for each interlaced frame (half-frame),
  which are measured independently. Polynomial mask we have applied is as follows:
  \f[ \left( {\begin{array}{cc} {-1} & {+1} \\ {+1} & {-1} \\ \end{array}} \right) \f]

  \param  pDib  pointer to the header of an input image.
  \param  pImg  pointer to the contents of the input image.
  \return       Ok = true. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ImageAcquirer::GaugeAndConvertImage( const BITMAPINFO * pDib, const ubyte * pImg )
{
  #define  YOFF(y)  ((bInvertY ? (H-1-(y)) : (y)) * scanW)  // Y offset

  ASSERT( (pDib != 0) && (pImg != 0) );
  ASSERT( csutility::AreDimensionsEqual( &m_frame, &(pDib->bmiHeader) ) );

  const BITMAPINFOHEADER * pHeader = &(pDib->bmiHeader);
  if ((pHeader->biWidth < 2) || (abs( pHeader->biHeight ) & 1)) // height must be even!
    return false;
  if (!(csutility::IsGrayOrYUY2( pHeader )))
    return false;

  const int    bInvertY = csutility::DoInvertAxisY( pHeader );
  const double SCALE = 2.0;
  const int    HISTO_SIZE = (4*MAX_INTENSITY_VALUE+1);
  const int    W = pHeader->biWidth;
  const int    H = abs( pHeader->biHeight );
  const int    dx = (pHeader->biCompression == MY_BI_YUY2) ? 2 : 1;
  int          histogram1[HISTO_SIZE];
  int          histogram2[HISTO_SIZE];
  int          scanW = csutility::GetDibScanSizeInBytes( pHeader );
  double       mean[2] = {0,0}, var[2] = {0,0};
  int          N = 0, dayNightCount = 0, dayNightImageSize = (W*H - 2*W - (H-2));

  // Initialize histograms.
  memset( histogram1, 0, sizeof(histogram1) );
  memset( histogram2, 0, sizeof(histogram2) );

  // Copy first two scans of the input image to the output one.
  alib::Copy( pImg+YOFF(0), pImg+YOFF(0)+scanW, m_frame.row_begin(0), m_frame.row_end(0), dx, 1, float() );
  alib::Copy( pImg+YOFF(1), pImg+YOFF(1)+scanW, m_frame.row_begin(1), m_frame.row_end(1), dx, 1, float() );

  // Accumulate histograms applying 2x2 polynomial mask to the image.
  for (int y = 3; y < H; y += 2)
  {
    const ubyte * pre1 = pImg + YOFF( y-3 );
    const ubyte * pre2 = pImg + YOFF( y-2 );
    const ubyte * cur1 = pImg + YOFF( y-1 );
    const ubyte * cur2 = pImg + YOFF( y-0 );
    float       * dst1 = m_frame.row_begin( y-1 );
    float       * dst2 = m_frame.row_begin( y-0 );
    int           width = W*dx;

    float t1 = ((*dst1++) = (float)(cur1[0]));   mean[0] += t1;   var[0] += t1*t1;
    float t2 = ((*dst2++) = (float)(cur2[0]));   mean[1] += t2;   var[1] += t2*t2;

    // Process the current scan pair.
    for (int x = dx; x < width; x += dx)
    {
      int c1 = (int)(cur1[x]);
      int c2 = (int)(cur2[x]);
      int q = x - dx;
      int h1 = abs( c1 + (int)(pre1[q]) - (int)(cur1[q]) - (int)(pre1[x]) );
      int h2 = abs( c2 + (int)(pre2[q]) - (int)(cur2[q]) - (int)(pre2[x]) );

      ASSERT( (h1 < HISTO_SIZE) && (h2 < HISTO_SIZE) );

      histogram1[h1] += 1;
      histogram2[h2] += 1;

      (*dst1++) = (float)c1;   mean[0] += c1;   var[0] += c1*c1;
      (*dst2++) = (float)c2;   mean[1] += c2;   var[1] += c2*c2;

      if (c1 > m_brightnessThr) ++dayNightCount;
      if (c2 > m_brightnessThr) ++dayNightCount;
    }

    N += W;
    ASSERT( (dst1 == m_frame.row_end( y-1 )) && (dst2 == m_frame.row_end( y-0 )) );
  }

  if (N == 0)
    return false;

  m_property.noiseDeviation[0] = csalgocommon::ProcessOnePolymaskNoiseHistogram( histogram1, HISTO_SIZE, SCALE );
  m_property.noiseDeviation[1] = csalgocommon::ProcessOnePolymaskNoiseHistogram( histogram2, HISTO_SIZE, SCALE );

  m_property.imgMean[0] = (float)(mean[0] /= N);
  m_property.imgMean[1] = (float)(mean[1] /= N);

  m_property.imgDeviation[0] = (float)sqrt( var[0]/N - mean[0]*mean[0] );
  m_property.imgDeviation[1] = (float)sqrt( var[1]/N - mean[1]*mean[1] );

  m_property.noise = (float)sqrt( 0.5*(alib::Sqr( m_property.noiseDeviation[0] ) +
                                       alib::Sqr( m_property.noiseDeviation[1] )) );

  m_dayNightState = -1;
  if ((m_brightnessThr > 0) && (m_percentageThr > 0))
    m_dayNightState = (100*dayNightCount > dayNightImageSize*m_percentageThr) ? 1 : 0;

  return true;

  #undef  YOFF
}

} // csalgo2

