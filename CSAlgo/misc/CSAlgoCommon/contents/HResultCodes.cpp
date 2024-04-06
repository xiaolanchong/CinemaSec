/****************************************************************************
  HResultCodes.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "HResultCodes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csalgocommon
{

//=================================================================================================
/** \class CodeDescriptor.
    \brief Descriptor of return code. */
//=================================================================================================
struct CodeDescriptor
{
  HRESULT         code;   //!< return code value
  const wchar_t * text;   //!< text descriptor
};


// Return code descriptors.
const CodeDescriptor Descriptors[] = 
{
  {E_FAIL,                   L"General failure"                                        },
  {HRESULT_UNKNOWN_DATA,     L"Data of unknown type have been encountered"             },
  {HRESULT_NULL_PROC_ADDR,   L"Pointer to a DLL function is NULL"                      },
  {HRESULT_NULL_PARAM_PTR,   L"NULL pointer has been passed as a function"             },
  {HRESULT_DLL_CRITICAL_ERR, L"Serious error has occurred while loading/freeing a DLL" }
};


//-------------------------------------------------------------------------------------------------
/** \brief Function returns the value of specofoed code and prints it description. */
//-------------------------------------------------------------------------------------------------
HRESULT PrintCode( HRESULT code, IDebugOutput * pOutput, const char * file, int line )
{
  if (pOutput != 0)
  {
    CString         msg;
    const wchar_t * type = (FAILED(code) ? L"ERROR" : L"WARNING");
    const wchar_t * text = L"no description";
    CString         wfile( (file != 0) ? file : "not specified" );

    for (int i = 0; i < ALIB_LengOf(Descriptors); i++)
    {
      if ((Descriptors[i]).code == code)
      {
        text = (Descriptors[i]).text;
        break;
      }
    }

    line = ((line < 0) || (file == 0)) ? (-1) : line;
    msg.Format( L"%s:\ncode: %d\nfile: %s\nline: %d\ntext: %s\n", type, code, (LPCWSTR)wfile, line, text );
    pOutput->PrintW( (FAILED(code) ? IDebugOutput::mt_error : IDebugOutput::mt_warning), (LPCWSTR)msg );
  }
  return code;
}

} // namespace csalgocommon

