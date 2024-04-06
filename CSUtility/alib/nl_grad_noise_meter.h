/***************************************************************************
                          nl_grad_noise_meter.h - Rosin's gradient
                                                  noise meter.
                             -------------------
    begin                : Sun Sep 14 2003
    copyright            : (C) 2003 by Alaudin (Albert) Akhriev
    email                : aaahaaah@hotmail.com
 ***************************************************************************/

#ifndef NOISELIB_GRADIENT_NOISE_METER
#define NOISELIB_GRADIENT_NOISE_METER

namespace noiselib
{

//-------------------------------------------------------------------------------------------------
/** \brief Function estimates image noise using Rosin's gradient method.

  \param  image  the input image.
  \param  strm   file-stream that receives intermediate results (GNU-PLOT format) or 0.
  \param  bHalf  use 50% median if true, otherwise 33.3% median.
  \return        Ok = standard deviation of image noise, otherwise default value. */
//-------------------------------------------------------------------------------------------------
template< class IMAGE >
float GradientNoiseMeter( IMAGE & image,
                          std::fstream * strm = (std::fstream*)0, bool bHalf = false )
{
  ASSERT( std::numeric_limits< typename IMAGE::value_type >::is_specialized );

  const float         FRACTION = bHalf ? (1.0f/2.0f) : (1.0f/3.0f);
  const float         MODE = bHalf ? 0.96777f : 0.98672f;
  const double        SCALE = 10.0;
  const double        SCALE_DIV_SQRT2 = SCALE/sqrt(2.0);
  const int           HISTO_SIZE = (int)(SCALE*100);
  double              dev = 0.0;
  Arr2D<float,float>  plane;
  std::vector<int>    histogram( HISTO_SIZE, 0 );

  // Subtract image and its smoothed version --> plane.
  {
    plane.resize2( image, false );
    alib::copy( image.begin(), image.end(), plane.begin(), plane.end(), float() );

#if 1

    alib::RepeatedlySmoothImage( plane, 3 );

    float                   * pF = plane.begin();
    float                   * pL = plane.end();
    const IMAGE::value_type * iF = image.begin();

    while (pF != pL) (*pF++) -= (float)(*iF++);

#endif
  }

  // Accumulate histogram.
  {
    int W = image.width();
    int H = image.height();

    if ((W < 2) || (H < 2))
      return (float)MIN_NOISE_DEVIATION;

    for (int y = 1; y < H; y++)
    {
      int offset = plane.offset( 1, y );

      for (int x = 1; x < W; x++, offset++)
      {
        float a = plane[ offset ] - plane[ offset-W-1 ];
        float b = plane[ offset-W ] - plane[ offset-1 ];
        int   d = (int)(sqrt( a*a + b*b ) * SCALE_DIV_SQRT2 + 0.5);

        if (d < HISTO_SIZE)
          histogram[d] += 1;
      }
    }
  }

  // Sometimes, images coded in block-based formats (MPEG, JPEG, YUV) have histogram with abnormal
  // value in the first entry. We fix situation by: histogram[0] = 0. But when histogram[0] is too
  // large then the image consists of uniformly-colored blocks (like animated cartoons). In the
  // latter case we do nothing.
  {
    int max = *(std::max_element( histogram.begin()+1, histogram.end() ));
    if (histogram[0] < 10*max)
      histogram[0] = 0;
  }

  // Iteratively find histogram's median.
  {
    int N = HISTO_SIZE;

    dev = 0.0;
    for (int i = 0; i < 3; i++)
    {
      int left, right;

      if (!(alib::HistoMedian( &(*(histogram.begin())), &(*(histogram.begin()))+N,
                               left, right, FRACTION )))
      {
        dev = 0.0;
        break;
      }

      N = std::min( HISTO_SIZE, 5*std::max( 1, (left+right)/2 ) );

      dev = std::max( (double)(MODE*0.5*(right-1+left)), (double)(SCALE*MIN_NOISE_DEVIATION) );
    }
  }

  // Iteratively improve estimation using preconditioner.
  if (dev > ((SCALE+0.1)*MIN_NOISE_DEVIATION))
  {
    int N = std::min( HISTO_SIZE, (int)(4.0*dev+1.0) );

    for (int iter = 0; iter < 2; iter++)
    {
      double p = -0.5/(dev*dev), m = 0.0, a = 0.0;

      for (int i = 0; i < N; i++)
      {
        double t = (i * exp( p*i*i ) * histogram[i]);

        m += i*t;
        a += t;
      }

      if (a > FLT_EPSILON)
      {
        m /= a;
        p = 8.0*dev*dev - ALIB_PI*m*m;
        if (p > FLT_EPSILON)
        {
          dev = m * dev * sqrt( ALIB_PI / p );
          continue;
        }
      }

      // On failure use simplified estimation.
      dev = (SCALE*MIN_NOISE_DEVIATION);
      break;
    }
  }
  dev = std::max( dev, (double)(SCALE*MIN_NOISE_DEVIATION) );

  // Print results in the form compatible with GNU-PLOT program.
  if (strm != 0)
  {
    double p = -0.5/(dev*dev), a = 0.0, he = 0.0, hh = 0.0;
    int    i, N = std::min( HISTO_SIZE, (int)(4.0*dev+1.0) );

    for (i = 0; i < N; i++)
    {
      double h = histogram[i];

      he += h*i*exp( p*i*i );
      hh += h*h;
    }
    a = (hh > FLT_EPSILON) ? (he/hh) : 0.0;

    for (i = 0; i < N; i++) { (*strm) << (i/SCALE) << "  " << (i*exp( p*i*i )) << std::endl; }
    (*strm) << std::endl << std::endl;
    for (i = 0; i < N; i++) { (*strm) << (i/SCALE) << "  " << (a*histogram[i]) << std::endl; }
  }

  return std::max( (float)(dev/SCALE), (float)MIN_NOISE_DEVIATION );
}

} // namespace noiselib

#endif // NOISELIB_GRADIENT_NOISE_METER

