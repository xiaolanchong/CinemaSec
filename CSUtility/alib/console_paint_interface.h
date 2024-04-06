///////////////////////////////////////////////////////////////////////////////////////////////////
// console_paint_interface.h
// ---------------------
// begin     : Sep 2005
// modified  : 28 Sep 2005
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaahaaah@hotmail.com, aaah@mail.ru
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace alib
{

struct IConsolePainter
{
  virtual ~IConsolePainter() {}
  virtual void DrawDemoImage( const Arr2ub * pImage, const TCHAR * statusText ) throw(...) = 0;
  virtual void DrawDemoImage( const AImage * pImage, const TCHAR * statusText ) throw(...) = 0;
  virtual void SwapAndDrawDemoImage( AImage * pImage, const TCHAR * statusText  ) throw(...) = 0;
};

typedef  std::auto_ptr<alib::IConsolePainter>  IConsolePainterPtr;

#ifdef _WIN32
alib::IConsolePainter * CreateWindowsConsolePainter( const TCHAR * title, bool bResizable, bool bInvertAxisY );
#endif

} // namespace alib

