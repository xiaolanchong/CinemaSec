#ifndef  ALIB_MACROS_H
#define  ALIB_MACROS_H

#ifndef  TRUE
#define  TRUE   1
#endif

#ifndef  FALSE
#define  FALSE  0
#endif

namespace alib
{
  extern "C"
  {
    void PrintStdException( LPCWSTR reason, const char * file, int line, AStr * text );
  }

  inline void ThrowStdException( LPCWSTR reason, const char * file, int line )
  {
    AStr s;
    PrintStdException( reason, file, line, &s );
    throw std::runtime_error( s.c_str() ); 
  }
}

#define  ALIB_THROW(cause)  alib::ThrowStdException( cause, __FILE__, __LINE__ )

#define  assertex(exp)  if (!(exp)) alib::ThrowStdException( #exp, __FILE__, __LINE__ )

#define  ALIB_VERIFY(exp,msg)  if (!(exp)) alib::ThrowStdException( msg, __FILE__, __LINE__ )

#ifdef _DEBUG
#define  ALIB_ASSERT(exp)  ASSERT(exp)
#else
#define  ALIB_ASSERT(exp)  if (!(exp)) alib::ThrowStdException( L#exp, __FILE__, __LINE__ )
#endif

#ifdef _DEBUG
#define  HRESULT_ASSERT(exp)  ASSERT(SUCCEEDED(exp))
#else
#define  HRESULT_ASSERT(exp)  if (FAILED(exp)) alib::ThrowStdException( L#exp, __FILE__, __LINE__ )
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// Some compilers don't expand inline functions inside template ones, so we need the macros below.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define  ALIB_MAX(a,b)               ( ((a) > (b)) ? (a) : (b) )

#define  ALIB_MIN(a,b)               ( ((a) < (b)) ? (a) : (b) )

#define  ALIB_LIMIT(v,min,max)       ( ((v) < (min)) ? (min) : (((v) > (max)) ? (max) : (v)) )

#define  ALIB_IS_LIMITED(v,min,max)  ( ((min) <= (v)) && ((v) <= (max)) )

#define  ALIB_IS_RANGE(v,min,max)    ( ((min) <= (v)) && ((v) < (max)) )

#define  ALIB_LengOf(array)          ( sizeof(array)/sizeof(array[0]) )

#define  ALIB_SIGN(v)                ( ((v) > 0) ? +1 : (((v) < 0) ? -1 : 0) )

#define  ALIB_SWAP(T,v1,v2)          { T t = (v1);  (v1) = (v2);  (v2) = t; }

#define  ALIB_ROUND(v)               ( (int)( ((v) >= 0.0) ? ((v)+0.5) : ((v)-0.5) ) )

#define  ALIB_SQR(v)                 ( (v)*(v) )

#define  ALIB_CUB(v)                 ( (v)*(v)*(v) )

#define  ALIB_INTERPOLATE(v1,v2,t)   ( (v1) + ((v2)-(v1))*(t) )

#define  ALIB_DELETE_OBJECT(p)       { delete   (p);  (p) = 0; }

#define  ALIB_DELETE_ARRAY(p)        { delete[] (p);  (p) = 0; }

#define  ALIB_ABS(x)                 ( ((x) >= 0) ? (x) : -(x) )

#define  ALIB_IS_FLOAT_TYPE(T)       ( std::numeric_limits<T>::is_specialized == true && \
                                       std::numeric_limits<T>::is_integer == false )

#define  ALIB_IS_INT_TYPE(T)         ( std::numeric_limits<T>::is_specialized == true && \
                                       std::numeric_limits<T>::is_integer == true )

#define  ALIB_DISTANCE(a, b, T, r)   { double dx = (double)((a).x) - (double)((b).x); \
                                       double dy = (double)((a).y) - (double)((b).y); \
                                       r = (T)sqrt( dx*dx + dy*dy ); }

// Use with integer types ONLY!
#define  ALIB_IS_NEIGHBOR2(x1,y1,x2,y2) \
  ( ((x1)==(x2) && (y1)==(y2)) ? -1 : ( (abs((x1)-(x2))<=1 && abs((y1)-(y2))<=1) ? +1 : 0 ) )

#endif // ALIB_MACROS_H


