/******************************************************************************
  SampleDistance.h
  ---------------------
  begin     : Aug 2005
  modified  : 5 Aug 2005
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
******************************************************************************/

#pragma once

#include <emmintrin.h>		// MMX, SSE, SSE2 intrinsic support

//=================================================================================================
/** \struct SqSampleDistance.
    \brief  SqSampleDistance. */
//=================================================================================================
template< class VECTOR_TYPE1, class VECTOR_TYPE2, bool bNormL2 >
struct SqSampleDistance
{
  typedef  typename VECTOR_TYPE1::value_type  value_type1;
  typedef  typename VECTOR_TYPE2::value_type  value_type2;

#ifdef _DEBUG
  SqSampleDistance()
  {
    ASSERT( std::numeric_limits<value_type1>::is_specialized );
    ASSERT( std::numeric_limits<value_type2>::is_specialized );

    bool bInt1 = std::numeric_limits<value_type1>::is_integer;
    bool bInt2 = std::numeric_limits<value_type2>::is_integer;
    int  size1 = sizeof(value_type1);
    int  size2 = sizeof(value_type2);

    ASSERT( (bInt1 && (size1 <= sizeof(int))) || (!bInt1 && (size1 <= sizeof(double))) );
    ASSERT( (bInt2 && (size2 <= sizeof(int))) || (!bInt2 && (size2 <= sizeof(double))) );
  }
#endif // _DEBUG

  double operator()( const VECTOR_TYPE1 & a, const VECTOR_TYPE2 & b ) const
  {
    ASSERT( (int)(a.size()) == (int)(b.size()) );

    const value_type1 * pa = &(a[0]);
    const value_type2 * pb = &(b[0]);
    int                 N = (int)(a.size());

    if (N == 0)
      return double(0);

    if (std::numeric_limits<value_type1>::is_integer && std::numeric_limits<value_type2>::is_integer)
    {
      int sum = 0;

      for (int i = 0; i < N; i++)
      {
        int t = (int)(pa[i]) - (int)(pb[i]);
        sum += (bNormL2 ? (t*t) : ((t >= 0) ? t : -t));
      }
      return (bNormL2 ? ((double)sum/(double)N) : ((double)sum*(double)sum/(double)(N*N)));
    }
    else
    {
      double sum = 0.0;

      for (int i = 0; i < N; i++)
      {
        double t = (double)(pa[i]) - (double)(pb[i]);
        sum += (bNormL2 ? (t*t) : fabs( t ));
      }
      return (bNormL2 ? ((double)sum/(double)N) : ((double)sum*(double)sum/(double)(N*N)));
    }
  }
};


//=================================================================================================
/** \struct SqSampleDistance.
\brief  SqSampleDistance. */
//=================================================================================================
template< class VECTOR_TYPE1, class VECTOR_TYPE2 >
struct SqSampleDistanceEx
{
  typedef  typename VECTOR_TYPE1::value_type  value_type1;
  typedef  typename VECTOR_TYPE2::value_type  value_type2;

#ifdef _DEBUG
  SqSampleDistanceEx()
  {
    ASSERT( std::numeric_limits<value_type1>::is_specialized );
    ASSERT( std::numeric_limits<value_type2>::is_specialized );

    bool bInt1 = std::numeric_limits<value_type1>::is_integer;
    bool bInt2 = std::numeric_limits<value_type2>::is_integer;
    int  size1 = sizeof(value_type1);
    int  size2 = sizeof(value_type2);

    ASSERT( (bInt1 && (size1 <= sizeof(int))) || (!bInt1 && (size1 <= sizeof(double))) );
    ASSERT( (bInt2 && (size2 <= sizeof(int))) || (!bInt2 && (size2 <= sizeof(double))) );
  }
#endif // _DEBUG

  double operator()( const VECTOR_TYPE1 & a, const VECTOR_TYPE2 & b, const bool bNormL2 ) const
  {
    ASSERT( (int)(a.size()) == (int)(b.size()) );

    const value_type1 * pa = &(a[0]);
    const value_type2 * pb = &(b[0]);
    int                 N = (int)(a.size());

    if (N == 0)
      return double(0);

    if (std::numeric_limits<value_type1>::is_integer && std::numeric_limits<value_type2>::is_integer)
    {
      int sum = 0;

      if (bNormL2) // L2 norm
      {
        for (int i = 0; i < N; i++)
        {
          int t = (int)(pa[i]) - (int)(pb[i]);
          sum += (t*t);
        }
        return ((double)sum/(double)N);
      }
      else // L1 norm
      {
#if 0 // #ifdef _M_IX86
        if ((sizeof(value_type1) == sizeof(ubyte)) && (sizeof(value_type2) == sizeof(ubyte)))
        {
          const int       INC = sizeof(__m128i);
          const int       MASK = INC-1;
          const __m128i * pa = (const __m128i*)( &(a[0]) );
          const __m128i * pb = (const __m128i*)( &(b[0]) );

          if (!(N & MASK) && !((int)pa & MASK) && !((int)pb & MASK))
          {
            __int32 sum = 0;
            __m128i res;

            memset( &res, 0, sizeof(res) );
            for (int i = 0, n = N/INC; i < n; i++)
            {
              __m128i r = _mm_sad_epu8( pa[i], pb[i] );
              res = _mm_add_epi64( res, r );
            }

            sum += *((__int32*)(&res) + 0);
            sum += *((__int32*)(&res) + 2);
            return ((double)sum*(double)sum/(double)(N*N));
          }
        }
#endif // _M_IX86

        for (int i = 0; i < N; i++)
        {
          int t = (int)(pa[i]) - (int)(pb[i]);
          sum += ((t >= 0) ? t : -t);
        }
        return ((double)sum*(double)sum/(double)(N*N));

      }
    }
    else
    {
      double sum = 0.0;

      if (bNormL2) // L2 norm
      {
        for (int i = 0; i < N; i++)
        {
          double t = (double)(pa[i]) - (double)(pb[i]);
          sum += (t*t);
        }
        return ((double)sum/(double)N);
      }
      else // L1 norm
      {
        for (int i = 0; i < N; i++)
        {
          double t = (double)(pa[i]) - (double)(pb[i]);
          sum += fabs( t );
        }
        return ((double)sum*(double)sum/(double)(N*N));
      }
    }
  }
};


/*
template< class VECTOR_TYPE1, class VECTOR_TYPE2, class RESULT_TYPE, bool bNormL2 >
struct SqSampleDistance
{
  typedef  typename VECTOR_TYPE1::value_type  value_type1;
  typedef  typename VECTOR_TYPE2::value_type  value_type2;
  typedef  RESULT_TYPE                        result_type;

#ifdef _DEBUG
  SqSampleDistance()
  {
    ASSERT( std::numeric_limits<RESULT_TYPE>::is_specialized );
    ASSERT( std::numeric_limits<value_type1>::is_specialized );
    ASSERT( std::numeric_limits<value_type2>::is_specialized );

    bool bInt1 = std::numeric_limits<value_type1>::is_integer;
    bool bInt2 = std::numeric_limits<value_type2>::is_integer;
    int  size1 = sizeof(value_type1);
    int  size2 = sizeof(value_type2);

    ASSERT( (bInt1 && (size1 <= sizeof(int))) || (!bInt1 && (size1 <= sizeof(double))) );
    ASSERT( (bInt2 && (size2 <= sizeof(int))) || (!bInt2 && (size2 <= sizeof(double))) );
    ASSERT( !(std::numeric_limits<RESULT_TYPE>::is_integer) || (bInt1 && bInt2) );
  }
#endif // _DEBUG

  RESULT_TYPE operator()( const VECTOR_TYPE1 & a, const VECTOR_TYPE2 & b ) const
  {
    ASSERT( (int)(a.size()) == (int)(b.size()) );

    const value_type1 * pa = &(a[0]);
    const value_type2 * pb = &(b[0]);
    int                 N = (int)(a.size());

    if (std::numeric_limits<value_type1>::is_integer && std::numeric_limits<value_type2>::is_integer)
    {
      int sum = 0;

      for (int i = 0; i < N; i++)
      {
        int t = (int)(pa[i]) - (int)(pb[i]);
        sum += (bNormL2 ? (t*t) : ((t >= 0) ? t : -t));
      }
      return (RESULT_TYPE)sum;
    }
    else
    {
      double sum = 0.0;

      for (int i = 0; i < N; i++)
      {
        double t = (double)(pa[i]) - (double)(pb[i]);
        sum += (bNormL2 ? (t*t) : fabs( t ));
      }
      return (RESULT_TYPE)sum;
    }
  }
};
*/

