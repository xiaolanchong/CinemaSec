/****************************************************************************
  algo_debug_output.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#include "stdafx.h"
#include "algo_debug_output.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Function prints a wide char message.

  \param  messageType  the type of message (see IDebugOutput::MessageType).
  \param  szMessage    the message to print. */
//-------------------------------------------------------------------------------------------------
void AlgoDebugOutput::PrintW( __int32 messageType, LPCWSTR szMessage )
{
  if (szMessage != 0)
  {
    switch (messageType)
    {
      case mt_debug_info:
        MessageBoxW( 0, szMessage, L"Debug info", MB_OK | MB_ICONINFORMATION );
        break;

      case mt_info:
        MessageBoxW( 0, szMessage, L"Info", MB_OK | MB_ICONINFORMATION );
        break;

      case mt_warning:
        MessageBoxW( 0, szMessage, L"Warning", MB_OK | MB_ICONWARNING );
        break;

      case mt_error:
        MessageBoxW( 0, szMessage, L"Error", MB_OK | MB_ICONERROR );
        break;

      default:
        MessageBoxW( 0, szMessage, L"Unknown message type", MB_OK | MB_ICONERROR );
        break;
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function prints an ANSI char message.

  \param  messageType  the type of message (see IDebugOutput::MessageType).
  \param  szMessage    the message to print. */
//-------------------------------------------------------------------------------------------------
void AlgoDebugOutput::PrintA( __int32 messageType, LPCSTR szMessage )
{
  if (szMessage != 0)
  {
    switch (messageType)
    {
      case mt_debug_info:
        MessageBoxA( 0, szMessage, "Debug info", MB_OK | MB_ICONINFORMATION );
        break;

      case mt_info:
        MessageBoxA( 0, szMessage, "Info", MB_OK | MB_ICONINFORMATION );
        break;

      case mt_warning:
        MessageBoxA( 0, szMessage, "Warning", MB_OK | MB_ICONWARNING );
        break;

      case mt_error:
        MessageBoxA( 0, szMessage, "Error", MB_OK | MB_ICONERROR );
        break;

      default:
        MessageBoxA( 0, szMessage, "Unknown message type", MB_OK | MB_ICONERROR );
        break;
    }
  }
}

