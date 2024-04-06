#include "stdafx.h"
#include "process_notify.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::wfstream global_log;

//=================================================================================================
/** \struct ConsoleNotifier.
    \brief  ConsoleNotifier. */
//=================================================================================================
struct ConsoleNotifier : public alib::IProcessNotify
{
  ConsoleNotifier()
  {
  }

  virtual ~ConsoleNotifier()
  {
  }

  virtual void start( const TCHAR * text, __int32 value, const void * pData )
  {
    text;value;pData;
    std::wcout << std::endl << ((text != 0) ? text : L"processing started: ") << std::flush;
    global_log << std::endl << ((text != 0) ? text : L"processing started: ") << std::flush;
  }

  virtual void running( const TCHAR * text, __int32 value, const void * pData )
  {
    text;value;pData;
    std::wcout << L'.' << std::flush;
    global_log << L'.' << std::flush;
  }

  virtual void finished( const TCHAR * text, __int32 value, const void * pData )
  {
    text;value;pData;
    std::wcout << ((text != 0) ? text : L"finished") << std::endl << std::flush;
    global_log << ((text != 0) ? text : L"finished") << std::endl << std::flush;
  }

  virtual void print( const TCHAR * text )
  {
    if (text != 0) std::wcout << text << std::endl;
    if (text != 0) global_log << text << std::endl;
  }

  virtual void PrintW( __int32 messageType, LPCWSTR szMessage )
  {
    if (szMessage != 0)
    {
      switch (messageType)
      {
        case IDebugOutput::mt_debug_info: std::wcout << L"Debug info";
                                          global_log << L"Debug info";
                                          break;
        case IDebugOutput::mt_info:       std::wcout << L"Info";
                                          global_log << L"Info";
                                          break;
        case IDebugOutput::mt_warning:    std::wcout << L"Warning";
                                          global_log << L"Warning";
                                          break;
        case IDebugOutput::mt_error:      std::wcout << L"Error";
                                          global_log << L"Error";
                                          break;
        default:                          std::wcout << L"Unknown message type";
                                          global_log << L"Unknown message type";
      }
      std::wcout << std::endl << szMessage << std::endl;
      global_log << std::endl << szMessage << std::endl;
    }
  }

  virtual void PrintA( __int32 messageType, LPCSTR szMessage )
  {
    USES_CONVERSION;
    if (szMessage != 0)
    {
      switch (messageType)
      {
        case IDebugOutput::mt_debug_info: std::cout  <<  "Debug info";
                                          global_log << L"Debug info";
                                          break;
        case IDebugOutput::mt_info:       std::cout  <<  "Info";
                                          global_log << L"Info";
                                          break;
        case IDebugOutput::mt_warning:    std::cout  <<  "Warning";
                                          global_log << L"Warning";
                                          break;
        case IDebugOutput::mt_error:      std::cout  <<  "Error";
                                          global_log << L"Error";
                                          break;
        default:                          std::cout  <<  "Unknown message type";
                                          global_log << L"Unknown message type";
      }
      std::cout  << std::endl <<      szMessage  << std::endl;
      global_log << std::endl << A2CW(szMessage) << std::endl;
    }
  }
};


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
alib::IProcessNotify * CreateConsoleProcessNotifier() throw(...)
{
  ConsoleNotifier * p = new ConsoleNotifier();
  if (p == 0)
    ALIB_THROW( L"Failed to create console process notifier" );
  return p;
}

