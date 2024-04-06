/******************************************************************************
  CalcRawDataThreshold.cpp
  ---------------------
  begin     : Aug 2004
  date      : 19 Jul 2005
  author(s) : Alexander Boltnev, Albert Akhriev
  email     : Alexander.Boltnev@biones.com, Albert.Akhriev@biones.com
******************************************************************************/

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Function finds the best threshold that separates two unimodal distributions.

  \param  arr1  array of samples of the first distribution.
  \param  arr2  array of samples of the second distribution.
  \return       optimal threshold. */
//-------------------------------------------------------------------------------------------------
float CalcRawDataThreshold( const FloatArr & arr1, const FloatArr & arr2 )
{
  ASSERT( !(arr1.empty()) && !(arr2.empty()) );

  MeanVarAccum<double> accum1, accum2;
  double               mean1, dev1, mean2, dev2;

  for (int i = 0; i < (int)(arr1.size()); i++) accum1 += (double)(arr1[i]);
  for (int i = 0; i < (int)(arr2.size()); i++) accum2 += (double)(arr2[i]);
  
  accum1.statistics( &mean1, 0, &dev1 );
  accum2.statistics( &mean2, 0, &dev2 );

  const FloatArr & leftArr  = (mean1 < mean2) ? arr1 : arr2;
  const FloatArr & rightArr = (mean1 < mean2) ? arr2 : arr1;

  double leftBound  = std::min( mean1, mean2 );
  double rightBound = std::max( mean1, mean2 );

  double step = std::min<double>( dev1, dev2 ) / 10.0;
  double candidate = leftBound;

  for (int iter = 0; iter < 3; iter++)
  {
    int err = INT_MAX;

    for (double x = leftBound; x <= rightBound; x += step)
    {
      int newErr = 0;

      for (int i = 0, n = (int)(leftArr.size()); i < n; i++)
      {
        if (leftArr[i] > x)
          ++newErr;
      }

      for (int i = 0, n = (int)(rightArr.size()); i < n; i++)
      {
        if (rightArr[i] < x)
          ++newErr;
      }

      if (newErr < err)
      {
        err = newErr;
        candidate = x;
      }
    }

    leftBound = candidate - 2.0 * step;
    rightBound = candidate + 2.0 * step;
    step = step / 10.0;
  }

  return ((float)candidate);
}

