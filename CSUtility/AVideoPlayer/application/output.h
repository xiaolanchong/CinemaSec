/****************************************************************************
  output.h
  ---------------------
  begin     : Apr 2005
  modified  : 24 Jul 2005
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \struct DebugOutput.
    \brief  The class prints debug information. */
//=================================================================================================
struct DebugOutput : public alib::IMessage
{
  virtual void Print( __int32 messageType, const TCHAR * szMessage );
  virtual void PrintW( __int32 messageType, const wchar_t * szMessage );
  virtual void PrintA( __int32 messageType, const char * szMessage );
};

