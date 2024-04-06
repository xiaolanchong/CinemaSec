///////////////////////////////////////////////////////////////////////////////
// DayNightFileParser.cpp
// ---------------------
// begin     : Aug 2004
// modified  : 12 Sep 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DayNightFileParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csalgocommon
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function . */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GetDayNightParametersFromFile( LPCWSTR        dllName,
                                    const int      cameraNo,
                                    int          * pBrightnessThr,
                                    int          * pPercentageThr,
                                    IDebugOutput * pOutput )
{
  try
  {
    USES_CONVERSION;

    wchar_t      fname[_MAX_PATH+1];
    char         text[128];
    std::fstream file;
    int          id = 0;
    int          bThr = 0, pThr = 0;

    if (pBrightnessThr != 0) (*pBrightnessThr) = 0;
    if (pPercentageThr != 0) (*pPercentageThr) = 0;

    memset( fname, 0, sizeof(fname) );
    ALIB_ASSERT( dllName != 0 );
    ALIB_ASSERT( PathAppend( fname, dllName ) );
    ALIB_ASSERT( PathRemoveFileSpec( fname ) );
    ALIB_ASSERT( PathAppend( fname, DAY_NIGHT_FILE_NAME ) );

    if (!((CFileFind()).FindFile( fname )))
    {
      if (pOutput != 0)
        pOutput->PrintW( IDebugOutput::mt_warning, L"File of day-night setting does not exist" );
      return false;
    }

    file.open( W2CA(fname), std::ios::in );
    ALIB_ASSERT( file.good() );

    memset( text, 0, sizeof(text) );
    file.getline( text, sizeof(text)-1 );
    ALIB_VERIFY( (strcmp( text, DAY_NIGHT_FILE_ID ) == 0), L"Illegal file of day-night settings" );

    while (file.good() && !(file.eof()))
    {
      file.getline( text, sizeof(text)-1 );
      if ((text[0] == '#') || (text[0] == '\0'))
        continue;

      bool ok = ((strstr( text, "camera" ) == text) &&
                 (sscanf( text+6, "%d%d%d", &id, &bThr, &pThr ) == 3));

      ALIB_VERIFY( ok, L"Bad day-night file record" );
      if (id == cameraNo)
      {
        if (pBrightnessThr != 0) (*pBrightnessThr) = bThr;
        if (pPercentageThr != 0) (*pPercentageThr) = pThr;
        return true;
      }
    }
  }
  catch (std::runtime_error e)
  {
    if (pOutput != 0)
      pOutput->PrintA( IDebugOutput::mt_error, e.what() );
  }
  return false;
}

} // namespace csalgocommon

