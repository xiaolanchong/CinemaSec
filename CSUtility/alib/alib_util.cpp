#include "stdafx.h"

#ifdef ALIB_CHECK_MEMORY_LEAKAGE
#define new DEBUG_NEW
#endif

namespace alib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function displays an error message in the message box or prints it in console.

  \param  text1  the main message or 0.
  \param  text2  an auxiliary message or 0.
  \param  text3  an auxiliary message or 0.
  \param  text4  an auxiliary message or 0. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void ErrorMessage( LPCWSTR text1, LPCWSTR text2, LPCWSTR text3, LPCWSTR text4 )
{
  #ifdef WIN32
    #ifdef _WINDOWS
      std::basic_string<wchar_t> msg;
      if (text1 != 0) (msg = text1); else (msg = ALIB_ERROR) += ALIB_NEWLINE;
      if (text2 != 0) (msg += ALIB_NEWLINE) += text2;
      if (text3 != 0) (msg += ALIB_NEWLINE) += text3;
      if (text4 != 0) (msg += ALIB_NEWLINE) += text4;
      ::AfxMessageBox( msg.c_str(), MB_ICONSTOP | MB_OK );
    #else
      if (text1 != 0) std::wcout << text1 << std::endl; else std::wcout << ALIB_ERROR << std::endl;
      if (text2 != 0) std::wcout << text2 << std::endl;
      if (text3 != 0) std::wcout << text3 << std::endl;
      if (text4 != 0) std::wcout << text4 << std::endl;
    #endif
  #else
      if (text1 != 0) std::wcout << text1 << std::endl; else std::wcout << ALIB_ERROR << std::endl;
      if (text2 != 0) std::wcout << text2 << std::endl;
      if (text3 != 0) std::wcout << text3 << std::endl;
      if (text4 != 0) std::wcout << text4 << std::endl;
  #endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function displays a message in the message box or prints it in console.

  \param  text1  the main message or 0.
  \param  text2  an auxiliary message or 0.
  \param  text3  an auxiliary message or 0.
  \param  text4  an auxiliary message or 0. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void Message( LPCWSTR text1, LPCWSTR text2, LPCWSTR text3, LPCWSTR text4 )
{
  #ifdef WIN32
    #ifdef _WINDOWS
      std::basic_string<wchar_t> msg;
      if (text1 != 0) (msg = text1);
      if (text2 != 0) (msg += ALIB_NEWLINE) += text2;
      if (text3 != 0) (msg += ALIB_NEWLINE) += text3;
      if (text4 != 0) (msg += ALIB_NEWLINE) += text4;
      ::AfxMessageBox( msg.c_str(), MB_ICONINFORMATION | MB_OK );
    #else
      if (text1 != 0) std::wcout << text1 << std::endl;
      if (text2 != 0) std::wcout << text2 << std::endl;
      if (text3 != 0) std::wcout << text3 << std::endl;
      if (text4 != 0) std::wcout << text4 << std::endl;
    #endif
  #else
      if (text1 != 0) std::wcout << text1 << std::endl;
      if (text2 != 0) std::wcout << text2 << std::endl;
      if (text3 != 0) std::wcout << text3 << std::endl;
      if (text4 != 0) std::wcout << text4 << std::endl;
  #endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function prints exception text.

  \param  reason  explanation text or 0.
  \param  file    the name of source file where exception has occurred or 0.
  \param  line    the code line where exception has occurred.
  \param  text    pointer to the output string that contains exception message. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void PrintStdException( LPCWSTR reason, const char * file, int line, AStr * text )
{
  std::stringstream s;
  s << std::endl << "EXCEPTION:" << std::endl
    << "file: " << ((file != 0) ? file : "") << std::endl
    << "line: " << line << std::endl
    << "reason: ";

  if (reason != 0)
    for (int i = 0; (i < 512) && (reason[i] != _T('\0')); s << (char)(reason[i++]));
  else
    s << "...";

  s << std::endl << std::endl;
  if (text != 0)
    (*text) = s.str();
}

} // namespace alib


