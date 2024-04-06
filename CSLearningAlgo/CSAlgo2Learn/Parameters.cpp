/****************************************************************************
  Parameters.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "Parameters.h"

#define APSTUDIO_INVOKED
#undef  APSTUDIO_READONLY_SYMBOLS
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int DIALOG_TAB_WIDTH  = 320;     // the width of a property page in dialog units
const int DIALOG_TAB_HEIGHT = 200;     // the height of a property page in dialog units

const int FIRST_CONTROL_IDENTIFIER = _APS_NEXT_CONTROL_VALUE;

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
Parameters::Parameters( int control_identifier )
: s_acquirer( control_identifier )
, s_sequence( control_identifier )
{
  s_acquirer.init( _T("Acquire"), _T("image acquisition parameters"), this );
  s_sequence.init( _T("Sequence"), _T("sequence parameters"), this);
}

