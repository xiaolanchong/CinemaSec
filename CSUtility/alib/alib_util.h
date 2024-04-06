#ifndef  ALIB_UTILITY_H
#define  ALIB_UTILITY_H

namespace alib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// T Max3(T a, T b, T c)
// int Round(double v) 
// int Sign(T v) 
// bool IsRange(T value, T min, T max)
// T Abs(T value)
// T Reciprocal(T x)
// T Limit(T value, T min, T max)
// struct Plus
// struct Minus
// struct MinusBack

// T    Sqr(T v)
// T    Interpolate<T,P>(T v1, T v2, P t);
// T    Interpolate2(T v1, T v2, double t1, double t, double t2)
//
// struct CmpPairByFirst
// struct CmpPairBySecond
// struct PointToPixel
// struct SquareRoot
//
// bool LoadArray(std::vector<T> & arr, FILE * f)
// bool SaveArray(const std::vector<T> & arr, FILE * f)
//
// void ClearPtrArr(std::vector<T*> & arr)
// void ClearPtrLst(std::list<T*> & lst)
//
// bool BilinearCoefs(int W, int H, Vec2f fp, Vec2i & ip, double * coef)
// bool BilinearInterpolation(const T * image, int W, int H, Vec2f p, T & value)
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
/** \brief Function completely clears and deallocates STL container
           by swapping with empty container of the same type.

  \param  container  the container to be cleared. */
//-------------------------------------------------------------------------------------------------
template< class CONTAINER >
void CompleteClear( CONTAINER & container )
{
  CONTAINER temp;
  container.swap( temp );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function checks that two containers have the same number of entries
           and their values are equal.

  \param  a  the first container to be checked.
  \param  b  the second container to be checked.
  \return    non-zero, if containers are equal. */
//-------------------------------------------------------------------------------------------------
template< class CONTAINER >
bool AreContainersSimilar( const CONTAINER & a, const CONTAINER & b )
{
  return ((a.size() == b.size()) &&
          ((std::mismatch( a.begin(), a.end(), b.begin() )).first == a.end()));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns maximal number among three specified ones. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline T Max3( T a, T b, T c )
{
  return ((a > b) ? ALIB_MAX(a,c) : ALIB_MAX(b,c));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function rounds any floating-point digit to the nearest integer.

  It is assumed that by default processor rounds towards zero,
  i.e. fractional part of either sign is truncated. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline int Round( T x )
{
  ASSERT( std::numeric_limits<T>::is_specialized && !(std::numeric_limits<T>::is_integer) );
  return (int)(x + ((x < ((T)(0.0))) ? ((T)(-0.5)) : ((T)(+0.5))));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function rounds 32-bit floating-point digit to the nearest integer.

  Function is optimized for x86 platforms with SSE instruction enabled. */
//-------------------------------------------------------------------------------------------------
inline int RoundFloat( float x )
{
#ifdef ALIB_X86
  __int32 h = (((__int32&)x) & 0x80000000) + 0x3f000000;
  return (int)(x + ((float&)h));
#else
  return (int)(x + ((x < 0.0f) ? (-0.5f) : (+0.5f)));
#endif
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns the sign of the specified value. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline int Sign( T v ) 
{ 
  return ALIB_SIGN( v );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function checks whether specified value lies within SEMI-OPEN interval [min,max).

  \param  value  the value to be checked.
  \param  min    the minimal margin.
  \param  max    the maximal margin. */
//-------------------------------------------------------------------------------------------------
template< class T > 
inline bool IsRange( T value, T min, T max )
{
  ASSERT( min < max );

  return ALIB_IS_RANGE( value, min, max );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns the value bounded by the minimal and maximal margins [min,max].

  \param  value  the value to be bounded.
  \param  min    the minimal margin.
  \param  max    the maximal margin. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline T Limit( T value, T min, T max )
{
  ASSERT( min <= max );

  return ALIB_LIMIT( value, min, max );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function checks whether specified value lies within CLOSED interval [min,max].

  \param  value  the value to be checked.
  \param  min    the minimal margin.
  \param  max    the maximal margin. */
//-------------------------------------------------------------------------------------------------
template< class T > 
inline bool IsLimited( T value, T min, T max )
{
  ASSERT( min <= max );

  return ALIB_IS_LIMITED( value, min, max );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns module of specified value. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline T Abs( T value )
{
  return ALIB_ABS( value );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates reciprocal value <i>1/x</i>, returns 0 when <i>x</i> = 0. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline T Reciprocal( T x )
{
  ASSERT( std::numeric_limits<T>::is_specialized && !(std::numeric_limits<T>::is_integer) );

  const T min = (std::numeric_limits<T>::min()) / (std::numeric_limits<T>::epsilon());
  return ((fabs( x ) > min) ? (T)(1.0/x) : (T)(0.0));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function tests an input character for backslash type. */
//-------------------------------------------------------------------------------------------------
inline bool IsBackSlash( int ch )
{
  return ((ch == _T('/')) || (ch == _T('\\')));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns the absolute value of difference between two digits.

  \param  a  the first operand.
  \param  b  the second operand.
  \return    |a-b|. */
//-------------------------------------------------------------------------------------------------
template< class TYPE >
TYPE ModuleOfDifference( TYPE a, TYPE b )
{
  return ((a >= b) ? (a-b) : (b-a));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function divides two positive integers rounding result (a/b) to the nearest integer. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline T RndPosDiv( T a, T b )
{
  ASSERT( std::numeric_limits<T>::is_specialized && std::numeric_limits<T>::is_integer );
  ASSERT( (a >= 0) && (b > 0) );

  return ((a+(b>>1))/b);
}


//=================================================================================================
/** \brief Result = Arg1 + Arg2 */
//=================================================================================================
template< class Arg1, class Arg2, class Result >
struct Plus : public std::binary_function< Arg1, Arg2, Result >
{
  Result operator()( Arg1 a, Arg2 b ) const
  {
    return (Result)(a+b);
  }
};


//=================================================================================================
/** \brief Result = Arg1 - Arg2 */
//=================================================================================================
template< class Arg1, class Arg2, class Result >
struct Minus : public std::binary_function< Arg1, Arg2, Result >
{
  Result operator()( Arg1 a, Arg2 b ) const
  {
    return (Result)(a-b);
  }
};


//=================================================================================================
/** \brief Result = -(Arg1 - Arg2) */
//=================================================================================================
template< class Arg1, class Arg2, class Result >
struct MinusBack : public std::binary_function< Arg1, Arg2, Result >
{
  Result operator()( Arg1 a, Arg2 b ) const
  {
    return (Result)(b-a);
  }
};


//=================================================================================================
/** \brief Result = Arg1 + Arg2*Scale2 */
//=================================================================================================
template< class Arg1, class Arg2, class Scale2, class Result >
struct PlusScaled2 : public std::binary_function< Arg1, Arg2, Result >
{
  Scale2  m_scale2;

  PlusScaled2( Scale2 s = second_scale_type() ) : m_scale2(s) {}

  Result operator()( Arg1 a, Arg2 b ) const
  {
    return (Result)(a + b*m_scale2);
  }
};


//=================================================================================================
/** \brief Function-object calculates squared distance between two 2D points. */
//=================================================================================================
template< class Arg1, class Arg2, class Result >
struct SqDist2D : public std::binary_function< Arg1, Arg2, Result >
{
  Result operator()( Arg1 & a, Arg2 & b ) const
  {
    Result dx = (Result)(a.x - b.x);
    Result dy = (Result)(a.y - b.y);
    return (dx*dx + dy*dy);
  }
};


//-------------------------------------------------------------------------------------------------
/** \brief Function compares points by their positions in (X,Y)-plane.

  (a < b) <---> ((a.y < b.y) OR ((a.y == b.y) AND (a.x < b.x)))  */
//-------------------------------------------------------------------------------------------------
template< class POINT_TYPE >
struct LessXY : public std::binary_function< POINT_TYPE, POINT_TYPE, bool >
{
  bool operator()( const POINT_TYPE & a, const POINT_TYPE & b ) const
  {
    return ((a.y < b.y) || ((a.y == b.y) && (a.x < b.x)));
  }
};


//-------------------------------------------------------------------------------------------------
/** \brief Function. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline T Sqr(T v)
{
  return ALIB_SQR(v);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline T SqDiff( T a, T b )
{
  T t = (a - b);
  return (t*t);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function. */
//-------------------------------------------------------------------------------------------------
template<class T,class P>
inline T Interpolate( const T & v1, const T & v2, P t )
{
  return ALIB_INTERPOLATE( v1, v2, t );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function. */
//-------------------------------------------------------------------------------------------------
template<class T>
inline T Interpolate2(const T & v1, const T & v2, double t1, double t, double t2)
{
  return (fabs(t2-t1) > (DBL_MIN/DBL_EPSILON)) ? (v1 + (v2-v1)*((t-t1)/(t2-t1))) : v1;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function checks that dimensions of two objects are equal.

  Objects must have methods 'width()' and 'height()'.

  \param  a  reference to the 1st object.
  \param  b  reference to the 2nd object. */
//-------------------------------------------------------------------------------------------------
template< class T1, class T2 >
bool AreDimensionsEqual( const T1 & a, const T2 & b )
{
  return ((a.width() == b.width()) && (a.height() == b.height()));
}


//=================================================================================================
/** \struct SquareRoot.
    \brief  Object. */
//=================================================================================================
template< class Arg, class Result >
struct SquareRoot : public std::unary_function< Arg, Result >
{
  Result operator()( Arg a ) const
  {
    return (Result)sqrt( a );
  }
};


//=================================================================================================
/** \struct CmpPtr.
    \brief  Object is used to find an element of a container by its pointer. */
//=================================================================================================
template< class T >
struct CmpPtr
{
  const T * m_pointer;       //!< the pointer to compare with

  /** \brief Constructor. */
  explicit CmpPtr( const T * p = 0 ) : m_pointer(p) {}

  /** \brief Operator returns 'true', if specified element is addressed by stored pointer. */
  bool operator()( const T & t )
  {
    return (&t == m_pointer);
  }
};


//=================================================================================================
/** \struct CmpPairByFirst.
    \brief  Object std::pair<>. */
//=================================================================================================
template< class T >
struct CmpPairByFirst
{
  bool operator()( const T & x, const T & y ) const
  {
    return (x.first < y.first);
  }
};


//=================================================================================================
/** \struct CmpPairBySecond.
    \brief  Object std::pair<>. */
//=================================================================================================
template< class T >
struct CmpPairBySecond
{
  bool operator()( const T & x, const T & y ) const
  {
    return (x.second < y.second);
  }
};


//-------------------------------------------------------------------------------------------------
/** \brief Function loads STL vector from a binary file.

  \param  f    source binary file.
  \param  arr  destination array.
  \return      Ok = true. */
//-------------------------------------------------------------------------------------------------
template< class T >
bool LoadArray( FILE * f, std::vector<T> & arr )
{
  typedef  typename std::vector<T>::value_type  value_type;

  int n = 0;
  arr.clear();
  if ((f != 0) && (fread( &n, sizeof(n), 1, f ) == 1) && (0 <= n) && (n < INT_MAX/2))
  {
    arr.resize( n );
    if (fread( &(*(arr.begin())), n*sizeof(value_type), 1, f ) == 1)
      return true;
  }
  arr.clear();
  return false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function loads STL vector from a binary file.

  \param  fname  file name.
  \param  arr    destination array.
  \return        Ok = true. */
//-------------------------------------------------------------------------------------------------
template< class T >
bool LoadArray( LPCWSTR fname, std::vector<T> & arr )
{
  FILE * f = _tfopen( fname, _T("rb") );
  bool   ok = LoadArray( f, arr );

  if (f != 0)
    fclose( f );
  return ok;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function saves STL vector in a binary file.

  \param  f    destination binary file.
  \param  arr  source array.
  \return      Ok = true. */
//-------------------------------------------------------------------------------------------------
template< class T >
bool SaveArray( FILE * f, const std::vector<T> & arr )
{
  typedef  typename std::vector<T>::value_type  value_type;

  int n = (int)(arr.size());
  return ((f != 0) &&
          (fwrite( &n, sizeof(n), 1, f ) == 1) &&
          (fwrite( &(*(arr.begin())), n*sizeof(value_type), 1, f ) == 1));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function saves STL vector in a binary file.

  \param  fname  file name.
  \param  arr    source array.
  \return        Ok = true. */
//-------------------------------------------------------------------------------------------------
template< class T >
bool SaveArray( LPCWSTR fname, const std::vector<T> & arr )
{
  FILE * f = _tfopen( fname, _T("wb") );
  bool   ok = alib::SaveArray( f, arr );

  if (f != 0)
    fclose( f );
  return ok;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears container of pointers.

  Function deallocates objects pointed by container's pointers and clears container itself. */
//-------------------------------------------------------------------------------------------------
template< class CONTAINER >
void ClearPtrContainer( CONTAINER & c )
{
  ASSERT( sizeof(typename CONTAINER::value_type) == sizeof(void*) ); // weak check for pointer

  for (typename CONTAINER::iterator it = c.begin(); it != c.end(); it++)
  {
    typename CONTAINER::value_type p = (*it);
    delete p;
  }
  c.clear();
}


//-------------------------------------------------------------------------------------------------
// Function computes coefficients of bilinear interpolation for any point between four neighbour
// pixels. Returns FALSE if the point falls out off image rectangle.
//
// Four pixels (and their bilinear coefficients) are located as follows:
// p3 p2         c3 c2
// p0 p1  <--->  c0 c1
// Bilinear interpolation: value = I0*C0 + I1*C1 + I2*C2 + I3*C3.
//-------------------------------------------------------------------------------------------------
inline bool BilinearCoefs(
  int     W,                           // image width
  int     H,                           // image height
  Vec2f   fp,                          // input point (with float coordinates)
  Vec2i * ip,                          // out: point with truncated coordinates of the input one
  float * coef )                       // out: 4 (!) found bilinear coefficients (coef[4])
{
  int x = (int)(fp.x + 1.0f) - 1;
  int y = (int)(fp.y + 1.0f) - 1;

  if (ip != 0)
  {
    ip->x = x;
    ip->y = y;
  }

  if ((x < 0) || (y < 0) || ((x+1) >= W) || ((y+1) >= H))
    return false;

  float dx = (float)(fp.x - x);
  float dy = (float)(fp.y - y);

  coef[2] = dx * dy;
  coef[1] = dx - coef[2];
  coef[3] = dy - coef[2];
  coef[0] = 1.0f - dy - coef[1];
  return true;
}


//-------------------------------------------------------------------------------------------------
// Function computes bilinear interpolation of a value of 2D-function at the point located between
// four neighbour pixels. Returns FALSE if the point falls out off image rectangle.
//-------------------------------------------------------------------------------------------------
/*
template<class T>
inline bool BilinearInterpolation(
  const T * image,                     // source image W x H
  int       W,                         // image width
  int       H,                         // image height
  Vec2f     p,                         // input point with float coordinates
  T &       value)                     // out: interpolated value
{
  int x = (int)(p.x + 1.0) - 1;
  int y = (int)(p.y + 1.0) - 1;

  if (x < 0 || y < 0 || (x+1) >= W || (y+1) >= H)
  {
    value = T();
    return false;
  }

  int   z = x + y*W;              // Pixels location:
  float dx = (float)(p.x - x);    // p3 p2
  float dy = (float)(p.y - y);    // p0 p1
  float C2 = dx * dy;
  float C1 = dx - C2;
  float C3 = dy - C2;
  float C0 = 1.0f - dy - C1;

  value = image[z]*C0 + image[z+1]*C1 + image[z+1+W]*C2 + image[z+W]*C3;
  return true;
}
*/


//-------------------------------------------------------------------------------------------------
/** \brief Function converts specified character to the uppercase one.

  \param  c  the character to be converted.
  \return    converted character. */
//-------------------------------------------------------------------------------------------------
inline char CharToUpperCase( char c )
{
  return ((('a' <= c) && (c <= 'z')) ? (char)((c-'a')+'A') : c);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function converts specified character to the lowercase one.

  \param  c  the character to be converted.
  \return    converted character. */
//-------------------------------------------------------------------------------------------------
inline char CharToLowerCase( char c )
{
  return ((('A' <= c) && (c <= 'Z')) ? (char)((c-'A')+'a') : c);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function deletes specified object and sets pointer to zero.

  \param  pObj  the pointer to the object to be deleted. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline void DeleteObject( T *& pObj )
{
  T * p = pObj;
  delete p;
  pObj = 0;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function deletes specified array and sets pointer to zero.

  \param  pArr  the pointer to the array to be deleted. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline void DeleteArray( T *& pArr )
{
  T * p = pArr;
  delete [] p;
  pArr = 0;
}


//=================================================================================================
/** \struct NotifyInterface.
    \brief  Function-object serves to change application's appearance during data processing. */
//=================================================================================================
struct NotifyInterface
{
  NotifyInterface() {}
  virtual ~NotifyInterface() {}
  virtual void UpdateInterface() {}
};



//-------------------------------------------------------------------------------------------------
/** \brief Function combines four characters into a single integer. */
//-------------------------------------------------------------------------------------------------
inline __int32 MakeFourCC( int a, int b, int c, int d )
{
  return ((__int32(a & 0x0FF))       |
          (__int32(b & 0x0FF) << 8)  |
          (__int32(c & 0x0FF) << 16) |
          (__int32(d & 0x0FF) << 24));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function converts integer value into a string of four characters. */
//-------------------------------------------------------------------------------------------------
inline LPCWSTR FourCC2Str( __int32 x, WCHAR s[5] )
{
  s[0] = (WCHAR)((x)       & 0x0FF);
  s[1] = (WCHAR)((x >>  8) & 0x0FF);
  s[2] = (WCHAR)((x >> 16) & 0x0FF);
  s[3] = (WCHAR)((x >> 24) & 0x0FF);
  s[4] = (WCHAR)(_T('\0'));
  return s;
}

} // namespace alib

#endif // ALIB_UTILITY_H

