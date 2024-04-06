/****************************************************************************
  AlgoParameters.cpp
  ---------------------
  begin     : Aug 2004
  modified  : 24 Aug 2005
  author(s) : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "AlgoParameters.h"

#define APSTUDIO_INVOKED
#undef  APSTUDIO_READONLY_SYMBOLS
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csalgo2
{

const int FIRST_CONTROL_IDENTIFIER = _APS_NEXT_CONTROL_VALUE;

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
Parameters::Parameters( int control_identifier )
: s_algorithm( control_identifier ),
  s_period( control_identifier ),
  s_acquirer( control_identifier ),
  s_demo( control_identifier )
{
  s_algorithm.init( _T("Algorithm"), _T("algorithm parameters"), this );
  s_period.init( _T("Periods"), _T("period parameters"), this );
  s_acquirer.init( _T("Acquire"), _T("image acquisition parameters"), this );
  s_demo.init( _T("Demo"), _T("demo parameters"), this );
}

} // namespace csalgo2

