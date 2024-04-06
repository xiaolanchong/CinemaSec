/****************************************************************************
  algo_debug_output.h
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

//=================================================================================================
/** \struct AlgoDebugOutput.
    \brief  The class prints debug information. */
//=================================================================================================
struct AlgoDebugOutput : public IDebugOutput
{
  virtual void PrintW( __int32 messageType, LPCWSTR szMessage );
  virtual void PrintA( __int32 messageType, LPCSTR  szMessage );
};

