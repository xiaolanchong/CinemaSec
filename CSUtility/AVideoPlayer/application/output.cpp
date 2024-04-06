/****************************************************************************
  output.cpp
  ---------------------
  begin     : Apr 2005
  modified  : 24 Jul 2005
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#include "stdafx.h"
#include "output.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Function prints a message.

  \param  messageType  the type of message (see IDebugOutput::MessageType).
  \param  szMessage    the message to print. */
//-------------------------------------------------------------------------------------------------
void DebugOutput::Print( __int32 messageType, const TCHAR * szMessage )
{
  if (sizeof(TCHAR) == sizeof(wchar_t))
    PrintW( messageType, reinterpret_cast<const wchar_t*>( szMessage ) );
  else
    PrintA( messageType, reinterpret_cast<const char*>( szMessage ) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function prints a wide-char message.

  \param  messageType  the type of message (see IDebugOutput::MessageType).
  \param  szMessage    the message to print. */
//-------------------------------------------------------------------------------------------------
void DebugOutput::PrintW( __int32 messageType, const wchar_t * szMessage )
{
  if (szMessage != 0)
  {
    switch (messageType)
    {
      case alib::IMessage::mt_debug_info:
        MessageBoxW( 0, szMessage, L"Debug info", MB_OK | MB_ICONINFORMATION );
        break;

      case alib::IMessage::mt_info:
        MessageBoxW( 0, szMessage, L"Info", MB_OK | MB_ICONINFORMATION );
        break;

      case alib::IMessage::mt_warning:
        MessageBoxW( 0, szMessage, L"Warning", MB_OK | MB_ICONWARNING );
        break;

      case alib::IMessage::mt_error:
        MessageBoxW( 0, szMessage, L"Error", MB_OK | MB_ICONERROR );
        break;

      default:
        MessageBoxW( 0, szMessage, L"Unknown message type", MB_OK | MB_ICONERROR );
        break;
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function prints an ANSI-char message.

  \param  messageType  the type of message (see IDebugOutput::MessageType).
  \param  szMessage    the message to print. */
//-------------------------------------------------------------------------------------------------
void DebugOutput::PrintA( __int32 messageType, const char * szMessage )
{
  if (szMessage != 0)
  {
    switch (messageType)
    {
      case alib::IMessage::mt_debug_info:
        MessageBoxA( 0, szMessage, "Debug info", MB_OK | MB_ICONINFORMATION );
        break;

      case alib::IMessage::mt_info:
        MessageBoxA( 0, szMessage, "Info", MB_OK | MB_ICONINFORMATION );
        break;

      case alib::IMessage::mt_warning:
        MessageBoxA( 0, szMessage, "Warning", MB_OK | MB_ICONWARNING );
        break;

      case alib::IMessage::mt_error:
        MessageBoxA( 0, szMessage, "Error", MB_OK | MB_ICONERROR );
        break;

      default:
        MessageBoxA( 0, szMessage, "Unknown message type", MB_OK | MB_ICONERROR );
        break;
    }
  }
}

