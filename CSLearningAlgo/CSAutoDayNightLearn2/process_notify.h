#pragma once

namespace alib
{

struct IProcessNotify : public IDebugOutput
{
  virtual ~IProcessNotify() {}
  virtual void start( const TCHAR * text, __int32 value, const void * pData ) = 0;
  virtual void running( const TCHAR * text, __int32 value, const void * pData ) = 0;
  virtual void finished( const TCHAR * text, __int32 value, const void * pData ) = 0;
  virtual void print( const TCHAR * text ) = 0;
};

} // namespace alib

