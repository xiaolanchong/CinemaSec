/****************************************************************************
  learn_algo_param.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#include "stdafx.h"
#include "learn_algo_param.h"

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
: s_algorithm( control_identifier ),
  s_acquirer( control_identifier ),
  s_statistics( control_identifier ),
  s_demo( control_identifier ),
  s_learn( control_identifier )
{
  s_algorithm.init( _T("Algorithm"), _T("algorithm parameters"), this );
  s_acquirer.init( _T("Acquire"), _T("image acquisition parameters"), this );
  s_statistics.init( _T("Statistics"), _T("statistics parameters"), this );
  s_demo.init( _T("Demo"), _T("demo parameters"), this );
  s_learn.init( _T("Learn"), _T("learning parameters"), this );
}


//-------------------------------------------------------------------------------------------------
/** \brief Показывает диалог редактирования параметров алгоритма.

  \param  hParentWnd  in: the parent window handle.
  \param  pBinParams  in: pointer to the binary image of input parameters, out: modified parameters.
  \param  pDebugOut   in: pointer to the object that prints a warning or an error message.
  \return             Ok = S_OK, cancel = S_FALSE, invalid parameters = E_INVALIDARG, otherwise E_FAIL. */
//-------------------------------------------------------------------------------------------------
HRESULT ShowParameterDialog( IN HWND           hParentWnd,
                             IN OUT Int8Arr  * pBinParams,
                             IN IDebugOutput * pDebugOut )
{
  HRESULT retValue = E_FAIL;

  try
  {
    ALIB_ASSERT( pBinParams != 0 );

    Parameters parameters( FIRST_CONTROL_IDENTIFIER );
    if (FAILED(csutility::LoadParameters( *pBinParams, parameters )))
    {
      retValue = E_INVALIDARG;
      throw std::runtime_error( "Invalid input parameters" );
    }

    CWnd * pParent = CWnd::FromHandle( hParentWnd );
    if ((CParameterDialog( parameters, pParent, DIALOG_TAB_WIDTH, DIALOG_TAB_HEIGHT )).DoModal() == IDOK)
    {
      StdStrStream strm;
      ALIB_ASSERT( parameters.save( strm ) );
      StdStr text = strm.str();
      const __int8 * first = reinterpret_cast<const __int8*>( text.c_str() );
      const __int8 * last = first + (int)((text.size()+1) * sizeof(StdStr::value_type));
      pBinParams->assign( first, last );
      retValue = S_OK;
    }
    retValue = S_OK;
  }
  catch (std::runtime_error & e)
  {
    CString msg = _T("Failed to change parameters.\n");
    msg += CString( e.what() );
    if (pDebugOut != 0)
      pDebugOut->PrintW( IDebugOutput::mt_error, (LPCWSTR)msg );
  }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function fills out the binary image of parameter structure by default settings.

  \param  pBinParams  in: pointer to the binary image of input parameters, out: modified parameters.
  \param  pDebugOut   in: pointer to the object that prints a warning or an error message.
  \return             Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT SetDefaultParameters( IN OUT Int8Arr  * pBinParams,
                              IN IDebugOutput * pDebugOut )
{
  HRESULT retValue = E_FAIL;

  try
  {
    Parameters   parameters( FIRST_CONTROL_IDENTIFIER );
    StdStrStream strm;
    StdStr       text;

    ALIB_ASSERT( pBinParams != 0 );
    parameters.set_default();
    ALIB_ASSERT( parameters.save( strm ) );
    text = strm.str();
    const __int8 * first = reinterpret_cast<const __int8*>( text.c_str() );
    const __int8 * last = first + (int)((text.size()+1) * sizeof(StdStr::value_type));
    pBinParams->assign( first, last );
    retValue = S_OK;
  }
  catch (std::runtime_error & e)
  {
    CString msg = _T("Failed to set default parameters.\n");
    msg += CString( e.what() );
    if (pDebugOut != 0)
      pDebugOut->PrintW( IDebugOutput::mt_error, (LPCWSTR)msg );
  }
  return retValue;
}


