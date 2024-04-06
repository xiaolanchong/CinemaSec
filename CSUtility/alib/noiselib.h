/***************************************************************************
                          noiselib.h - the main header file
                                       of the noise library.
                             -------------------
    begin                : Sun Sep 14 2003
    copyright            : (C) 2003 by Alaudin (Albert) Akhriev
    email                : aaahaaah@hotmail.com
 ***************************************************************************/

#ifndef  A_NOISE_LIBRARY_H
#define  A_NOISE_LIBRARY_H

namespace noiselib
{

// Minimal standard noise deviation returned by any noise estimator.
const double MIN_NOISE_DEVIATION = 0.1;


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates factorial.

  \param  n  the number which factorial to be computed, range: n=[0..20].
  \return    n!. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline T Factorial( int n )
{
  ASSERT( std::numeric_limits<T>::is_specialized );
  ASSERT( sizeof(T) >= 8 );
  ASSERT( (0 <= n) && (n <= 20) );

  T f = 1;

  for (int i = 2; i <= n; i++)
  {
    f *= i;
  }
  return f;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates binomial coefficient: n!/(r!(n-r)!).

  \param  n  range: n=[0..29].
  \param  r  range: r=[0..n].
  \return    n!/(r!(n-r)!). */
//-------------------------------------------------------------------------------------------------
template< class T >
inline T Binomial( int n, int r )
{
  ASSERT( std::numeric_limits<T>::is_specialized );
  ASSERT( sizeof(T) >= 8 );
  ASSERT( (0 <= r) && (r <= n) && (n <= 29) );

  T b = 1;

  if (r <= (n-r))
  {
    for (int i = (n-r+1); i <= n; i++)
    {
      b *= i;
    }
    b /= noiselib::Factorial<T>( r ); 
  }
  else
  {
    for (int i = (r+1); i <= n; i++)
    {
      b *= i;
    }
    b /= noiselib::Factorial<T>( n-r );
  }

  return b;
}

} // namespace noiselib

#include "nl_grad_noise_meter.h"
#include "nl_polymask1_noise_meter.h"
#include "nl_polymask3_noise_meter.h"

#endif // A_NOISE_LIBRARY_H


