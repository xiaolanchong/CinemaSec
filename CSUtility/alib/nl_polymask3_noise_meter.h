/***************************************************************************
                          nl_polymask3_noise_meter.h -  three polymasks
                                                        noise meter.
                             -------------------
    begin                : Sun Sep 14 2003
    copyright            : (C) 2003 by Alaudin (Albert) Akhriev
    email                : aaahaaah@hotmail.com
 ***************************************************************************/

#ifndef NOISELIB_THREE_POLYNOMIAL_MASKS_NOISE_METER
#define NOISELIB_THREE_POLYNOMIAL_MASKS_NOISE_METER

namespace noiselib
{

//-------------------------------------------------------------------------------------------------
/** \brief Function estimates image noise convolving the image with three polynomial masks.

  \param  image  the input image.
  \param  dim    dimension of the mask (width or height), range: [2..29].
  \param  strm   file-stream that receives intermediate results (GNU-PLOT format) or 0.
  \param  bHalf  use 50% median if true, otherwise 33.3% median.
  \return        Ok = standard deviation of image noise, otherwise default value. */
//-------------------------------------------------------------------------------------------------
template< class IMAGE >
float ThreePolymasksNoiseMeter(
  IMAGE        & image,
  int            dim,
  std::fstream * strm = (std::fstream*)0,
  bool           bHalf = false )
{
  ASSERT( std::numeric_limits< typename IMAGE::value_type >::is_specialized );

  dim = std::min( std::max( dim, 2 ), 29 );

  const float         FRACTION = bHalf ? (1.0f/2.0f) : (1.0f/3.0f);
  const float         MODE = bHalf ? 0.69485f : 0.70208f;
  double              MULTIPLIER_EX = 1.0, MULTIPLIER_AB = 1.0, SCALE = 1.0;
  const int           MAX_SCALE = 20;
  const int           HISTO_SIZE = MAX_SCALE*100;
  double              dev = 0.0;
  Arr2D<float,float>  plane;
  std::vector<int>    histogram( HISTO_SIZE, 0 );

  // Copy input image to the temporal one.
  {
    plane.resize2( image, false );
    alib::copy( image.begin(), image.end(), plane.begin(), plane.end(), float() );
  }

  // Calculate scale factors.
  {
    long double  s[2] = {0,2};
    long double  divisor[2] = {0,0};
    int          i;

    // Compute norm of extreme mask: sqrt(s[0]*s[0]).
    for (i = 0; i < dim; i++)
    {
      long double t = noiselib::Binomial<long double>( dim-1, i );
      s[0] += t*t;
    }

    // Compute norm of auxiliary masks: sqrt(s[0]*s[1]).
    for (i = 1; i < (dim-1); i++)
    {
      long double t = noiselib::Binomial<long double>( dim-2, i ) - 
                      noiselib::Binomial<long double>( dim-2, i-1 );
      s[1] += t*t;
    }

    // Find the maximal integer divisors within the range: [10..MAX_SCALE].
    for (int k = 0; k < 2; k++)
    {
      divisor[k] = s[k];
      for(i = 10; (i <= MAX_SCALE) && (s[k] > i); i++)
      {
        if (fmod( (double)(s[k]), (double)i ) < (10.0*DBL_EPSILON))
          divisor[k] = i;
      }
    }

    MULTIPLIER_EX = (double)((divisor[0]*divisor[1])/(s[0]*s[0]));
    MULTIPLIER_AB = (double)((divisor[0]*divisor[1])/(s[0]*s[1]));
    SCALE = sqrt( (double)(divisor[0]*divisor[1]) );
    ASSERT( (1.5 < SCALE) && (SCALE < (MAX_SCALE+1)) );
  }

  // Accumulate histogram.
  {
    int  W = image.width();
    int  H = image.height();
    int  W1 = W, H1 = H;

    if ((W < dim) || (H < dim))
      return (float)MIN_NOISE_DEVIATION;

    // Repeatedly convolve image with 2x2 extreme mask (dim-2) times.
    for (int i = 2; i < dim; i++)
    {
      --W1;
      --H1;
      for(int y = 0; y < H1; y++)
      {
        float * p = plane.row_begin( y );

        for(int x = 0; x < W1; x++)
        {
          p[x] = p[x] + p[x+1+W] - p[x+1] - p[x+W];
        }
      }
    }

    // Make the last convolution with 'E', 'A', 'B' masks and accumulate histogram.
    --W1;
    --H1;
    for (int y = 0; y < H1; y++)
    {
      float * p = plane.row_begin( y );

      for(int x = 0; x < W1; x++)
      {
        double e = p[x] + p[x+1+W] - p[x+1] - p[x+W];

        double a = p[x] + p[x+W] - p[x+1] - p[x+1+W];

        double b = p[x] + p[x+1] - p[x+W] - p[x+1+W];

        int h = (int)(sqrt( MULTIPLIER_AB*(a*a + b*b) + MULTIPLIER_EX*e*e ) + 0.5);
        if (h < HISTO_SIZE)
          histogram[h] += 1;
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

      N = std::min( HISTO_SIZE, 4*std::max( 1, (left+right)/2 ) );

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
        double t = (i*i * exp( p*i*i ) * histogram[i]);

        m += i*t;
        a += t;
      }

      if (a > FLT_EPSILON)
      {
        m /= a;
        p = 128.0*dev*dev - 9.0*ALIB_PI*m*m;
        if (p > FLT_EPSILON)
        {
          dev = m * dev * 3.0 * sqrt( ALIB_PI / p );
          continue;
        }
      }

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

      he += h*i*i*exp( p*i*i );
      hh += h*h;
    }
    a = (hh > FLT_EPSILON) ? (he/hh) : 0.0;

    for (i = 0; i < N; i++) { (*strm) << (i/SCALE) << "  " << (i*i*exp( p*i*i )) << std::endl; }
    (*strm) << std::endl << std::endl;
    for (i = 0; i < N; i++) { (*strm) << (i/SCALE) << "  " << (a*histogram[i]) << std::endl; }
  }

  return std::max( (float)(dev/SCALE), (float)MIN_NOISE_DEVIATION );
}

} // namespace noiselib

#endif // NOISELIB_THREE_POLYNOMIAL_MASKS_NOISE_METER

