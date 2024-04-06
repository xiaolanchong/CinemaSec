/****************************************************************************
  algo_param.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#include "stdafx.h"
#include "algo_param.h"

#define APSTUDIO_INVOKED
#undef  APSTUDIO_READONLY_SYMBOLS
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csalgo
{

const int FIRST_CONTROL_IDENTIFIER = _APS_NEXT_CONTROL_VALUE;

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
Parameters::Parameters( int control_identifier )
: s_algorithm( control_identifier ),
  s_acquirer( control_identifier ),
  s_statistics( control_identifier ),
  s_demo( control_identifier )
{
  s_algorithm.init( _T("Algorithm"), _T("algorithm parameters"), this );
  s_acquirer.init( _T("Acquire"), _T("image acquisition parameters"), this );
  s_statistics.init( _T("Statistics"), _T("statistics parameters"), this );
  s_demo.init( _T("Demo"), _T("demo parameters"), this );
}

} // namespace csalgo

