/****************************************************************************
  image_acquirer.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#include "stdafx.h"
#include "image_property.h"
#include "frame.h"
#include "algo_param.h"
#include "image_acquirer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
ImageAcquirer::ImageAcquirer()
{
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
ImageAcquirer::~ImageAcquirer()
{
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object. */
//-------------------------------------------------------------------------------------------------
void ImageAcquirer::Clear()
{
  m_frame.reset();
  m_property.clear();
  m_frameFlag = INVALID_CONTENT;
  m_meanIntensity = 0.0f;
  m_nDroppedFrames = 0;
  m_nPassedFrames = 0;
  m_bDropFrame = false;
  m_tempBuffer.clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes the process of image acquisition and measurement.

  \param  frameWidth   the width of sub-frame.
  \param  frameHeight  the height of sub-frame.
  \return              fOk = true. */
//-------------------------------------------------------------------------------------------------
bool ImageAcquirer::Init( int frameWidth, int frameHeight )
{
  Clear();
  if ((frameWidth > 0) && (frameHeight > 0))
    m_frame.resize( frameWidth, frameHeight );
  else
    m_frame.clear();
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function gets the current frame from the source device and estimates whether
           it can be fed into the main algorithm or must be dropped.

  \param  pDib  the header of DIB.
  \param  pImg  pointer to the image content.
  \return       if non-zero, then loaded frame is reliable, otherwise it must be dropped. */
//-------------------------------------------------------------------------------------------------
bool ImageAcquirer::Acquire( LPCDIB pDib, LPCUBYTE pImg, const csalgo::ImageAcquireParams & param )
{
  ALIB_ASSERT( pDib != 0 );

  m_frameFlag = INVALID_CONTENT;
  m_bDropFrame = false;

  // Resize frame if necessary.
  if (m_frame.empty() && (pDib->biWidth != 0) && (pDib->biHeight != 0))
    m_frame.resize( abs( pDib->biWidth ), abs( pDib->biHeight ) );

  // Measure image characteristics and convert DIB image into float one.
  ALIB_ASSERT( ImageAcquirer::GaugeAndConvertImage( pDib, pImg, m_frame, m_property ) );
  m_frameFlag = VALID_CONTENT;

  // Check that there is no abnormal mean intensity difference between half-frames of the
  // same frame. Also check that frame's intensity has not dramatically changed.
  if (m_nPassedFrames >= csalgo::MIN_PASSED_FRAMES_NUM)
  {
    float maxNoiseDev = std::max( m_property.noiseDeviation[0], m_property.noiseDeviation[1] );
    float meanDiffThr = std::max( 10.0f, param.p_maxMeanDiffThr() * maxNoiseDev );
    float outlierThr  = std::max( 10.0f, param.p_meanOutlierThr() * maxNoiseDev );

    bool ok1 = (m_property.noiseDeviation[0] < param.p_maxNoiseSignalRatio()*m_property.imgMean[0]);
    bool ok2 = (m_property.noiseDeviation[1] < param.p_maxNoiseSignalRatio()*m_property.imgMean[1]);
    bool ok3 = (fabs( m_property.imgMean[0] - m_property.imgMean[1] ) < meanDiffThr);
    bool ok4 = (fabs( m_property.imgMean[0] - m_meanIntensity ) < outlierThr);
    bool ok5 = (fabs( m_property.imgMean[1] - m_meanIntensity ) < outlierThr);

    m_bDropFrame = !(ok1 && ok2 && ok3 && ok4 && ok5);
  }

  // Increment dropped frame counter, if necessary.
  if (m_bDropFrame)
    ++m_nDroppedFrames;

  // Update or accumulate the mean intensity.
  {
    float a = m_bDropFrame ? (0.5f*param.p_meanUpdateRate()) : param.p_meanUpdateRate();
    float b = (1.0f - a);
    float average = 0.5f * (m_property.imgMean[0] + m_property.imgMean[1]);

    if (m_nPassedFrames >= csalgo::MIN_PASSED_FRAMES_NUM)
      m_meanIntensity = b * m_meanIntensity + a * average;
    else
      m_meanIntensity += average;
  }

  // Increment frame counter and normalize the mean intensity, if necessary.
  if (++m_nPassedFrames == csalgo::MIN_PASSED_FRAMES_NUM)
    m_meanIntensity /= (float)m_nPassedFrames;

  // Smooth input frame.
  if (!m_bDropFrame && (param.p_nSmoothRepeat() > 0))
    ImageAcquirer::RepeatedlySmoothImage( m_frame, param.p_nSmoothRepeat(), m_tempBuffer );

  return !m_bDropFrame;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns reference to the current frame.

  \return  reference to the current frame. */
//-------------------------------------------------------------------------------------------------
const Frame & ImageAcquirer::GetFrame() const
{
  ALIB_ASSERT( m_frameFlag == VALID_CONTENT );
  return m_frame;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function swaps content of the current frame with external storage.

  The function can be invoked only once after successful invocation of \sa Acquire(..).

  \param  exFrame  reference to the external storage. */
//-------------------------------------------------------------------------------------------------
void ImageAcquirer::SwapFrame( Frame & exFrame )
{
  ALIB_ASSERT( m_frameFlag == VALID_CONTENT );
  exFrame.swap( m_frame );
  m_frameFlag = INVALID_CONTENT;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function estimates noise deviation from a histogram that resembles
           the positive half of Gaussian.

  \param  histogram   the input histogram.
  \param  HISTO_SIZE  the size of histogram.
  \param  SCALE       the multiplier used during histogram accumulation.
  \return             estimated noise. */
//-------------------------------------------------------------------------------------------------
float ImageAcquirer::ProcessNoiseHistogram( int * histogram, const int HISTO_SIZE, const double SCALE )
{
  ASSERT( histogram != 0 );

  double dev = 0.0, roughDev = 0.0;
  int    histoSize = 0;

  // Compute the actual histogram's size and correct the first entry.
  {
    int max = 0;

    for (int i = 0; i < HISTO_SIZE; i++)
    {
      if (histogram[i] > 0)
        histoSize = i;
      if (histogram[i] > max)
        max = histogram[i];
    }
    histoSize = std::min( histoSize+1, HISTO_SIZE );

    // Sometimes, images coded in block-based formats (MPEG, JPEG, YUV) have histogram with abnormal
    // value in the first entry. We fix situation by: histogram[0] = (histogram[1]+histogram[2]+1)/2.
    // But when histogram[0] is too large then the image consists of uniformly-colored blocks (like
    // animated cartoons). In the latter case we do nothing.
    if (histogram[0] < 10*max)
      histogram[0] = (histogram[1]+histogram[2]+1)/2;
  }

  // Obtain initial estimation of noise deviation.
  {
    double I1 = 0.0;
    double I2 = 0.0;

    for (int i = 0; i < histoSize; i++)
    {
      double h = histogram[i];

      I1 += h;
      I2 += h*h;
    }
    dev = (I2 > FLT_EPSILON) ? ((I1*I1)/(I2*sqrt( ALIB_PI ))) : 0.0;
    dev = (roughDev = std::max( dev, (double)(SCALE*MIN_NOISE_DEVIATION) ));
  }

  // Iteratively improve estimation using preconditioner.
  if (dev > ((SCALE+0.1)*MIN_NOISE_DEVIATION))
  {
    bool ok = false;
    int  N = std::min( histoSize, (int)(4.0*dev+1.0) );

    for (int iter = 0; iter < 3; iter++)
    {
      double p = -0.5/(dev*dev), a = (0.5*histogram[0]), m = 0.0;

      for (int i = 1; i < N; i++)
      {
        double t = exp( p*i*i ) * histogram[i];

        m += i*t;
        a += t;
      }

      ok = false;
      if (a > FLT_EPSILON)
      {
        m /= a;
        p = (2.0*dev*dev - ALIB_PI*m*m);
        if (p > FLT_EPSILON)
        {
          dev = dev * m * sqrt( ALIB_PI / p );
          ok = true;
        }
      }
    }

    // On failure use simplified estimation.
    if (!ok || (dev < (SCALE*MIN_NOISE_DEVIATION)))
      dev = roughDev;
  }

  return std::max( (float)(dev/SCALE), (float)MIN_NOISE_DEVIATION );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes noise deviation along with some characteristics of an input image
           by applying 2x2 polynomial mask, and also converts the input image into float format.

  There are two groups of image properties - one for each interlaced frame (half-frame),
  which are measured independently. Polynomial mask we have applied is as follows:
  \f[
  \left( {\begin{array}{cc}
     { - 1} & { + 1}  \\
     { + 1} & { - 1}  \\
  \end{array}} \right)
  \f]

  \param  pDib      pointer to the header of an input image.
  \param  pImg      pointer to the contents of the input image.
  \param  frame     out: the image converted into float format and its characteristics.
  \param  property  out: some characteristics of the input image.
  \return           Ok = true. */
//-------------------------------------------------------------------------------------------------
bool ImageAcquirer::GaugeAndConvertImage( LPCDIB          pDib,
                                          LPCUBYTE        pImg,
                                          Frame         & frame,
                                          ImageProperty & property )
{
  #define  YOFF(y)  ((bInvertY ? (H-1-(y)) : (y)) * scanW)  // Y offset

  if ((pDib == 0) || (pImg == 0) || (abs( pDib->biHeight ) & 1)) // must be even!
    return false;
  if (!(csutility::IsGrayOrYUY2( pDib )))
    return false;
  if ((pDib->biWidth != frame.width()) || (abs( pDib->biHeight ) != frame.height()))
    return false;
  if (pDib->biWidth < 2)
    return false;

  const int    bInvertY = csutility::DoInvertAxisY( pDib );
  const double SCALE = 2.0;
  const int    HISTO_SIZE = (4*MAX_INTENSITY_VALUE+1);
  const int    W = pDib->biWidth;
  const int    H = abs( pDib->biHeight );
  const int    dx = (pDib->biCompression == MY_BI_YUY2) ? 2 : 1;
  int          histogram1[HISTO_SIZE];
  int          histogram2[HISTO_SIZE];
  int          scanW = csutility::GetDibScanSizeInBytes( pDib );
  double       mean[2] = {0,0}, var[2] = {0,0};
  int          N = 0;

  // Initialize histograms.
  memset( histogram1, 0, sizeof(histogram1) );
  memset( histogram2, 0, sizeof(histogram2) );

  // Copy first two scans of the input image to the output one.
  alib::Copy( pImg+YOFF(0), pImg+YOFF(0)+scanW, frame.row_begin(0), frame.row_end(0), dx, 1, float() );
  alib::Copy( pImg+YOFF(1), pImg+YOFF(1)+scanW, frame.row_begin(1), frame.row_end(1), dx, 1, float() );

  // Accumulate histograms applying 2x2 polynomial mask to the image.
  for (int y = 3; y < H; y += 2)
  {
    const ubyte * pre1 = pImg + YOFF( y-3 );
    const ubyte * pre2 = pImg + YOFF( y-2 );
    const ubyte * cur1 = pImg + YOFF( y-1 );
    const ubyte * cur2 = pImg + YOFF( y-0 );
    float       * dst1 = frame.row_begin( y-1 );
    float       * dst2 = frame.row_begin( y-0 );
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
    }

    N += W;
    ASSERT( (dst1 == frame.row_end( y-1 )) && (dst2 == frame.row_end( y-0 )) );
  }

  if (N == 0)
    return false;

  property.noiseDeviation[0] = ImageAcquirer::ProcessNoiseHistogram( histogram1, HISTO_SIZE, SCALE );
  property.noiseDeviation[1] = ImageAcquirer::ProcessNoiseHistogram( histogram2, HISTO_SIZE, SCALE );

  property.imgMean[0] = (float)(mean[0] /= N);
  property.imgMean[1] = (float)(mean[1] /= N);

  property.imgDeviation[0] = (float)sqrt( var[0]/N - mean[0]*mean[0] );
  property.imgDeviation[1] = (float)sqrt( var[1]/N - mean[1]*mean[1] );

  property.noise = (float)sqrt( 0.5*(alib::Sqr( property.noiseDeviation[0] ) +
                                     alib::Sqr( property.noiseDeviation[1] )) );
  frame.noise = property.noise;
  return true;

  #undef  YOFF
}


//-------------------------------------------------------------------------------------------------
/** \brief Function repeatedly smoothes in-place an image by the mask (1,2,1)
           alternating horizontal and vertical directions.

  \param  image       in: the source image, out: smoothed image.
  \param  nRepeat     the number of incremental smoothing repetitions.
  \param  tempBuffer  external storage of intermediate data. */
//-------------------------------------------------------------------------------------------------
void ImageAcquirer::RepeatedlySmoothImage( Arr2f & image, int nRepeat, UByteArr & tempBuffer )
{
  int W = image.width();
  int H = image.height();

  if ((W < 2) || (H < 2))
    return;

  if ((int)(tempBuffer.size()) < (3*W*sizeof(float)))
    tempBuffer.resize( 3*W*sizeof(float) );

  // Repeat smoothing several times.
  while (--nRepeat >= 0)
  {
    float * img = image.begin();
    float * row = img;
    float * pre = reinterpret_cast<float*>( &(*(tempBuffer.begin())) ) + W*0;
    float * cur = reinterpret_cast<float*>( &(*(tempBuffer.begin())) ) + W*1;
    float * nxt = reinterpret_cast<float*>( &(*(tempBuffer.begin())) ) + W*2;

    // Read and smooth the first row.
    for (int x = 1; x < (W-1); x++)
    {
      cur[x] = (pre[x] = row[x-1] + row[x] + row[x] + row[x+1]);
    }
    cur[ 0 ] = (pre[ 0 ] = row[ 0 ] + row[ 0 ] + row[ 0 ] + row[ 1 ]);
    cur[W-1] = (pre[W-1] = row[W-2] + row[W-1] + row[W-1] + row[W-1]);

    // Go along the image row by row...
    for (int y = 0; y < H; y++)
    {
      // Read and smooth the next row.
      if ((y+1) < H)
      {
        row = (img+W);
        for(int x = 1; x < (W-1); x++)
        {
          nxt[x] = row[x-1] + row[x] + row[x] + row[x+1];
        }
        nxt[ 0 ] = row[ 0 ] + row[ 0 ] + row[ 0 ] + row[ 1 ];
        nxt[W-1] = row[W-2] + row[W-1] + row[W-1] + row[W-1];
      }
      else nxt = cur;

      // Smooth the image in vertical direction scaling result appropriately.
      for (int x = 0; x < W; x++)
      {
        img[x] = (float)((pre[x] + cur[x] + cur[x] + nxt[x])*(1.0/16.0));
      }

      // Cyclically swap row pointers.
      float * tmp = pre;   pre = cur;   cur = nxt;   nxt = tmp;
      img += W;
    }

    ASSERT( img == image.end() );
  }
}

