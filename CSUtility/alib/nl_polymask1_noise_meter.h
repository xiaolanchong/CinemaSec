/***************************************************************************
                          nl_polymask1_noise_meter.h - one polymask
                                                       noise meter.
                             -------------------
    begin                : Sun Sep 14 2003
    copyright            : (C) 2003 by Alaudin (Albert) Akhriev
    email                : aaahaaah@hotmail.com
 ***************************************************************************/

#ifndef NOISELIB_ONE_POLYNOMIAL_MASK_NOISE_METER
#define NOISELIB_ONE_POLYNOMIAL_MASK_NOISE_METER

namespace noiselib
{

//-------------------------------------------------------------------------------------------------
/** \brief Function estimates image noise convolving an image with extreme polynomial mask.

  \param  image  the input image.
  \param  dim    dimension of the mask (width or height), range: [2..29].
  \param  strm   file-stream that receives intermediate results (GNU-PLOT format) or 0.
  \return        Ok = standard deviation of image noise, otherwise default value. */
//-------------------------------------------------------------------------------------------------
template< class IMAGE >
float OnePolymaskNoiseMeter(
  IMAGE        & image,
  int            dim,
  std::fstream * strm = (std::fstream*)0 )
{
  typedef  typename IMAGE::value_type  VALUE_TYPE;

  ASSERT( std::numeric_limits<VALUE_TYPE>::is_specialized );

  dim = std::min( std::max( dim, 2 ), 29 );

  double              MULTIPLIER = 1.0, SCALE = 1.0;
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
    long double norm = 0, divisor = 0;
    int         i;

    // Compute mask's norm.
    for (i = 0; i < dim; i++)
    {
      long double t = noiselib::Binomial<long double>( dim-1, i );
      norm += t*t;
    }

    // Find maximal integer divisor within the range: [10..MAX_SCALE].
    divisor = norm;
    for (i = 10; (i <= MAX_SCALE) && (norm > i); i++)
    {
      if (fmod( norm, (long double)i ) < (10.0*DBL_EPSILON))
        divisor = i;
    }

    MULTIPLIER = (double)(divisor/norm);
    SCALE = (double)divisor;
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

    // Make last convolution with extreme mask and accumulate histogram.
    --W1;
    --H1;
    for (int y = 0; y < H1; y++)
    {
      float * p = plane.row_begin( y );

      for (int x = 0; x < W1; x++)
      {
        int h = (int)(fabs( (p[x] + p[x+1+W] - p[x+1] - p[x+W]) * MULTIPLIER ) + 0.5);
        if (h < HISTO_SIZE)
          histogram[h] += 1;
      }
    }
  }

  // Sometimes, images coded in block-based formats (MPEG, JPEG, YUV) have histogram with abnormal
  // value in the first entry. We fix situation by: histogram[0] = (histogram[1]+histogram[2]+1)/2.
  // But when histogram[0] is too large then the image consists of uniformly-colored blocks (like
  // animated cartoons). In the latter case we do nothing.
  {
    int max = *(std::max_element( histogram.begin()+1, histogram.end() ));
    if (histogram[0] < 10*max)
      histogram[0] = (histogram[1]+histogram[2]+1)/2;
  }

  // Obtain initial estimation of noise deviation.
  {
    double I1 = 0.0;
    double I2 = 0.0;

    for (int i = 0; i < HISTO_SIZE; i++)
    {
      double h = histogram[i];

      I1 += h;
      I2 += h*h;
    }
    dev = (I2 > FLT_EPSILON) ? ((I1*I1)/(I2*sqrt( ALIB_PI ))) : 0.0;
    dev = std::max( dev, (double)(SCALE*MIN_NOISE_DEVIATION) );
  }

  // Iteratively improve estimation using preconditioner.
  if (dev > ((SCALE+0.1)*MIN_NOISE_DEVIATION))
  {
    bool ok = false;
    int  N = std::min( HISTO_SIZE, (int)(4.0*dev+1.0) );

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
      dev = (SCALE*MIN_NOISE_DEVIATION);
  }

  // Print results in the form compatible with GNU-PLOT program.
  if (strm != 0)
  {
    double p = -0.5/(dev*dev), a = 0.0, he = 0.0, hh = 0.0;
    int    i, N = std::min( HISTO_SIZE, (int)(4.0*dev+1.0) );

    for (i = 0; i < N; i++)
    {
      double h = histogram[i];

      he += h*exp( p*i*i );
      hh += h*h;
    }
    a = (hh > FLT_EPSILON) ? (he/hh) : 0.0;

    for (i = 0; i < N; i++) { (*strm) << (i/SCALE) << "  " << (exp( p*i*i )) << std::endl; }
    (*strm) << std::endl << std::endl;
    for (i = 0; i < N; i++) { (*strm) << (i/SCALE) << "  " << (a*histogram[i]) << std::endl; }
  }

  return std::max( (float)(dev/SCALE), (float)MIN_NOISE_DEVIATION );
}

} // namespace noiselib

#endif // NOISELIB_ONE_POLYNOMIAL_MASK_NOISE_METER

