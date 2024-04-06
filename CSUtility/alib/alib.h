#pragma once

#define  ALIB_API

#ifdef _MSC_VER // Visual C++
//#pragma warning( disable : 4710 )
//#pragma warning( disable : 4514 )
#pragma warning( disable : 4127 )
#endif

#define  ALIB_VERSION  "1.2.0"

#ifdef WIN32
  #pragma message( "Alik's Library, version: " ALIB_VERSION )

  #ifdef _DEBUG
    #define ALIB_CHECK_MEMORY_LEAKAGE
    #pragma message( "Memory leakage detection is enabled" )
  #endif // _DEBUG

  #undef ALIB_API
  #ifdef ALIB_USER_DLL
    #define ALIB_API __declspec(dllexport)
  #else
    #define ALIB_API __declspec(dllimport)
  #endif

  #ifdef  _M_IX86
  #define ALIB_X86 1
  #endif
#endif // WIN32

#ifndef ASSERT
#define ASSERT assert
#endif

/////////////////////////////////////////////////////////////////////
// System inclusions.
/////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <conio.h>
#include <direct.h>
#include <io.h>
#endif // WIN32

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>


/////////////////////////////////////////////////////////////////////
// STL inclusions.
/////////////////////////////////////////////////////////////////////

#include "alib_stl.h"

/////////////////////////////////////////////////////////////////////
// Alib inclusions.
/////////////////////////////////////////////////////////////////////

#include "alib_types_const.h"
#include "alib_macros.h"
//#include "alib_allocator.h"
#include "alib_rand_inc_iter.h"
//#include "alib_point2d.h"
#include "alib_vec2d.h"
#include "alib_rect.h"
#include "alib_arr2d.h"
//#include "alib_arr3d.h"
//#include "alib_b_rgb.h"
//#include "alib_f_rgb.h"
//#include "alib_i_rgb.h"
//#include "alib_small_sort.h"

#include "alib_file.h"
#include "alib_img_smooth.h"
//#include "alib_img_lib.h"

#include "alib_util.h"
#include "alib_algo.h"
//#include "alib_sort.h"
#include "alib_go_along_line.h"
#include "alib_timer.h"
//#include "alib_find_file.h"
//#include "alib_vicinity.h"
//#include "alib_h_scan.h"
//#include "alib_functions.h"
#include "alib_filter1d.h"
//#include "alib_graph.h"
//#include "alib_graph_ex.h"
//#include "alib_graph_copy.h"
//#include "alib_part_tree3d.h"
//#include "alib_mat2d.h"
//#include "alib_spline.h"
//#include "alib_alg_eq.h"
//#include "alib_random.h"
//#include "alib_result_html.h"
//#include "alib_histogram.h"
#include "alib_statistics.h"
#include "alib_kdim_tree_mt.h"
//#include "alib_uint_color_accum.h"
//#include "alib_partition_tree2d.h"
//#include "alib_allocator.h"
#include "k_harmonic_mean_clusterizer.h"
#include "alib_exception.h"

//-------------------------------------------------------------------------------------------------
// Declarations of the library functions.
//-------------------------------------------------------------------------------------------------
namespace alib
{

  // alib_util.cpp
  extern "C"
  {
    void Message( LPCWSTR text1, LPCWSTR text2 = 0, LPCWSTR text3 = 0, LPCWSTR text4 = 0 );
    void ErrorMessage( LPCWSTR text1, LPCWSTR text2 = 0, LPCWSTR text3 = 0, LPCWSTR text4 = 0 );
  };

} // namespace alib

namespace vislib
{

ALIB_API void EquateGradientPeaks(
  const Vec2fImg & dirs,
  const Arr2f    & grads,
  const Arr2ub   * pMask,
  const ARect    * pRect,
  float            minGradThr,
  float            pathStep,
  bool             bNoDirSign,
  bool             bSqGrad,
  Arr2f          & eqGrads );

} // namespace vislib

