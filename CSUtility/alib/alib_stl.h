#ifndef  ALIB_STL_H
#define  ALIB_STL_H

#undef min
#undef max

#ifdef WIN32
  #if _MSC_VER <= 1200
    #pragma warning( disable : 4786 )
    #pragma warning( push, 1 )
    #pragma warning( disable : 4702 )
    #pragma warning( disable : 4701 )
  #else
    #pragma warning( push, 1 )
    #pragma warning( disable : 4702 )
  #endif
#endif

//#define ALIB_SGI_STL
//#define ALIB_STL_PORT

#ifdef ALIB_SGI_STL

  #ifdef WIN32
  #pragma message("SGI standard template library")
  #error No inclusion
  #endif

#elif defined(ALIB_STL_PORT)

  #ifdef WIN32
  #pragma message("STLport standard template library")
  #endif

  #include "stlport/algorithm"
  #include <stlport/numeric>
  #include <stlport/exception>
  #include <stlport/iostream>
  #include <stlport/sstream>
  #include <stlport/fstream>
  #include <stlport/functional>
  #include <stlport/iterator>
  #include <stlport/list>
  #include <stlport/map>
  #include <stlport/set>
  #include <stlport/string>
  #include <stlport/utility>
  #include <stlport/valarray>
  #include <stlport/vector>
  #include <stlport/limits>
  #include <stlport/stack>
  #include <stlport/iomanip>

#else // DEF_STL

  #ifdef WIN32
  #pragma message("Default standard template library")
  #else
  #include <stdexcept>
  #endif

  #include <algorithm>
  #include <numeric>
  #include <exception>
  #include <iostream>
  #include <sstream>
  #include <fstream>
  #include <functional>
  #include <iterator>
  #include <list>
  #include <map>
  #include <set>
  #include <string>
  #include <utility>
  #include <valarray>
  #include <vector>
  #include <limits>
  #include <stack>
  #include <iomanip>

  #ifdef WIN32
  #if _MSC_VER <= 1200
  // Absent in VC++ 6.0 STL.
  namespace std
  {
    template<class T>
    inline T min( const T & x, const T & y )
    {
      return (x < y) ? x : y;
    }

    template<class T>
    inline T max( const T & x, const T & y )
    {
      return (x > y) ? x : y;
    }
  }
  #endif
  #endif // WIN32

#endif // STL inclusion

#ifdef WIN32
  #pragma warning( pop )
#endif

#endif // ALIB_STL_H
