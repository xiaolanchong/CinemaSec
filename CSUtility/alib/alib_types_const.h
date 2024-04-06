///////////////////////////////////////////////////////////////////////////////
//  alib_types_const.h
//  ---------------------
//  begin     : 1998
//  modified  : 25 Jul 2005
//  author(s) : Albert Akhriev
//  copyright : Albert Akhriev
//  email     : aaah@mail.ru, Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// Constants.
///////////////////////////////////////////////////////////////////////////////////////////////////

const int ALIB_HORZ_DIR = (1<<0);
const int ALIB_VERT_DIR = (1<<1);

const double ALIB_PI = 3.14159265358979323846;
const double ALIB_DBL_MIN = DBL_MIN/DBL_EPSILON;
const float  ALIB_FLT_MIN = FLT_MIN/FLT_EPSILON;

const bool ALIB_INVERT_Y      = true;      // invert Y for bitmaps
const int  ALIB_MAX_IMAGE_DIM = 1024;      // max. width or height of an image

const WCHAR ALIB_CURRENT_DIR[] = L".";
const WCHAR ALIB_PARENT_DIR[] = L"..";
const WCHAR ALIB_UNSUPPORTED_EXCEPTION[] = L"Unsupported exception";
const WCHAR ALIB_EXCEPTION[] = L"EXCEPTION:";
const WCHAR ALIB_ERROR[] = L"ERROR:";
const WCHAR ALIB_ERROR_IO_FILE[] = L"I/O operation failed on file:";
const WCHAR ALIB_EMPTY[] = L"";
const WCHAR ALIB_SPACE[] = L" ";

#ifdef WIN32
  const WCHAR ALIB_NEWLINE[] = L"\n";
  const WCHAR ALIB_BACKSLASH = L'\\';
#else
  const WCHAR ALIB_NEWLINE[] = L"\r\n";
  const WCHAR ALIB_BACKSLASH = L'/';
#endif

const WCHAR ALIB_WIN_BACKSLASH = L'\\';
const WCHAR ALIB_UNIX_BACKSLASH = L'/';

///////////////////////////////////////////////////////////////////////////////////////////////////
// Common Types.
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef  signed char                    sbyte;
typedef  unsigned char                  ubyte;
typedef  unsigned short                 ushort;
typedef  unsigned int                   uint;

typedef  std::vector<signed char>       SByteArr;
typedef  std::vector<unsigned char>     UByteArr;
typedef  std::vector<signed short>      ShortArr;
typedef  std::vector<unsigned short>    UShortArr;
typedef  std::vector<signed int>        IntArr;
typedef  std::vector<unsigned int>      UIntArr;
typedef  std::vector<float>             FloatArr;
typedef  std::vector<double>            DoubleArr;

typedef  std::list<signed char>         SByteLst;
typedef  std::list<unsigned char>       UByteLst;
typedef  std::list<signed short>        ShortLst;
typedef  std::list<unsigned short>      UShortLst;
typedef  std::list<signed int>          IntLst;
typedef  std::list<unsigned int>        UIntLst;
typedef  std::list<float>               FloatLst;
typedef  std::list<double>              DoubleLst;

typedef  std::string                    AStr;
typedef  std::stringstream              AStrStream;
typedef  std::wstring                   WStr;
typedef  std::wstringstream             WStrStream;

typedef  std::basic_string<WCHAR>       StdStr;
typedef  std::list<StdStr>              StdStrLst;
typedef  std::basic_stringstream<WCHAR> StdStrStream;
typedef  std::basic_iostream<WCHAR>     StdBaseStream;
typedef  std::auto_ptr<StdStrStream>    StdStrStrmPtr;

