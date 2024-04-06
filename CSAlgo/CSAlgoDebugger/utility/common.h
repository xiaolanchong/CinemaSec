/****************************************************************************
  common.h - some common declarations, types and constants.
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

typedef csutility::AutoInterfacePtr<ICameraAnalyzer> ICamAnalyzerPtr;
typedef csutility::AutoInterfacePtr<IHallAnalyzer>   IHallAnalyzerPtr;

enum VideoSourceType
{
  VIDEO_UNDEFINED = 0,
  VIDEO_SEPARATE_FILES,      //!< videofiles were selected manually
  VIDEO_PLAYLIST_FILES,      //!< videofiles were selected through playlist
  VIDEO_CAMERAS              //!< 
};

enum GrabberType
{
  GRABBER_UNKNOWN = 0,
  GRABBER_AVI,
  GRABBER_AVD
};


#define  MYMESSAGE_UPDATE_STATUSBAR_TEXT    (WM_USER+0)
#define  MYMESSAGE_UPDATE_STATE_INDICATORS  (WM_USER+1)
#define  MYMESSAGE_UPDATE_FPS_INDICATOR     (WM_USER+2)
#define  MYMESSAGE_UPDATE_CURSOR_INDICATOR  (WM_USER+3)
#define  MYMESSAGE_UPDATE_WINDOW_TITLE      (WM_USER+4)

const wchar_t APPLICATION_PARAM_FILE_NAME[] = L"CSAlgoDebugger.param";
const wchar_t HALL_ANALYZER_LOG_FILE[] = L"HallAnalyzer.log";
const wchar_t CAMERA_FOLDER[] = L"camera";
const wchar_t HALL_FOLDER[] = L"hall";
const wchar_t CHAIRS_XML[] = L"chairs.xml";
const wchar_t BACKGROUND_BMP[] = L"background.bmp";

const int MAX_PARAM_FILE_SIZE = (1<<20);

const UINT MY_OFN_FLAGS = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

