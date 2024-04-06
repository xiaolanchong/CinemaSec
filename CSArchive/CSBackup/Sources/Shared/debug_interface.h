/****************************************************************************
  debug_interface.h
  ---------------------
  begin     : Aug 2004
  author(s) : E.Gorbachev, Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \struct IDebugOutput.
    \brief  Interface class for any storage of debug information. */
//=================================================================================================
struct IDebugOutput
{
  enum MessageType
  {
    mt_debug_info = 0,
    mt_info,
    mt_warning,
    mt_error
  };

  virtual void PrintW( __int32 messageType, LPCWSTR szMessage ) = 0;
  virtual void PrintA( __int32 messageType, LPCSTR szMessage ) = 0;
};

