///////////////////////////////////////////////////////////////////////////////
// ScheduleTestVideo.cpp
// ---------------------
// begin     : Aug 2005
// modified  : 1 Aug 2005
// author(s) : Albert Akhriev.
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csutility
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function parses INI-file and determines whether test video should be written.

  \param  dllName   full-path name of algorithm module.
  \param  cameraNo  unique identifier of a camera used to obtain test video.
  \return           nonzero means working scenario, negative value means failure. */
///////////////////////////////////////////////////////////////////////////////////////////////////
int ShouldTestVideoBeWritten( LPCWSTR dllName, int cameraNo )
{
  USES_CONVERSION;

  std::fstream file;
  std::string  text;
  int          camNo = -1;
  CTime        curTime = CTime::GetCurrentTime();
  int          month = curTime.GetMonth();
  int          day = curTime.GetDay();
  int          hour = curTime.GetHour();
  int          minute = curTime.GetMinute();
  wchar_t      name[_MAX_PATH+1];
  int          scenario = 0;

  if ((dllName == 0) || (cameraNo < 0))
    return (-1);

  memset( name, 0, sizeof(name) );
  if (!PathAppend( name, dllName ) || !PathRemoveFileSpec( name ) || !PathAppend( name, TESTVIDEO_INI ))
    return (-1);

  file.open( W2CA(name), std::ios::in );
  while (file.good() && !(file.eof()))
  {
    text.clear();
    file >> text;
    if (text.empty() || (text[0] == '#'))
    {
      while (file.good() && !(file.eof()) && (file.get() != '\n')); // skip comment line
      continue;
    }

    if (_stricmp( text.c_str(), "Scenario" ) == 0)
    {
      file >> scenario;
    }
    else if (_stricmp( text.c_str(), "CamId" ) == 0)
    {
      file >> camNo;
      camNo = (cameraNo == camNo) ? cameraNo : -1;
    }
    else if ((camNo >= 0) && (_stricmp( text.c_str(), "Time" ) == 0))
    {
      int  d, h1, m1, h2, m2;
      char c1, c2;

      file >> text >> d >> h1 >> c1 >> m1 >> h2 >> c2 >> m2 >> std::skipws;
      if ((c1 == ':') && (c2 == ':') && (_stricmp( text.c_str(), W2CA(MONTH[month]) ) == 0))
      {
        if (( (h1 < h2) && (h1 <= hour) && (hour <= h2) && (d == day) ) ||
            ( (h2 < h1) && ( ((h1 <= hour) && (d == day)) || ((hour <= h2) && (d+1 == day)) ) ))
        {
          if (h2 < h1)
          {
            if (hour <= h2) hour += 24;
            h2 += 24;
          }

          if (alib::IsLimited( 60*hour+minute, 60*h1+m1, 60*h2+m2 ))
            return scenario;
        }
      }
    }
    else
    {
      while (file.good() && !(file.eof()) && (file.get() != '\n')); // skip line
    }
  }
  return (-1);
}

} // namespace csutility

