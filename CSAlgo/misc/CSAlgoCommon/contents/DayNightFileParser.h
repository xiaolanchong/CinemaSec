///////////////////////////////////////////////////////////////////////////////
// DayNightFileParser.h
// ---------------------
// begin     : Aug 2004
// modified  : 12 Sep 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace csalgocommon
{

const wchar_t DAY_NIGHT_FILE_NAME[] = L"DayNightSettings.txt";
const char    DAY_NIGHT_FILE_ID[] = "{{{{{DAY-NIGHT-SETTINGS}}}}}";
const int     DAY_NIGHT_DEFAULT_BRIGHTNESS_THRESHOLD = 115;
const int     DAY_NIGHT_DEFAULT_PERCENTAGE_THRESHOLD = 65;

bool GetDayNightParametersFromFile( LPCWSTR        dllName,
                                    const int      cameraNo,
                                    int          * pBrightnessThr,
                                    int          * pPercentageThr,
                                    IDebugOutput * pOutput );

} // namespace csalgocommon

