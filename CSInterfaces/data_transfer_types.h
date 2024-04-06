/****************************************************************************
  data_transfer_types.h
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "common_interface_settings.h"
#include "data_transfer_interface.h"

//>>>>>
#include "../CSChair/IChairContent.h"
//>>>>>

namespace csinterface
{

//=================================================================================================
/** \brief Types of chair's contents. */
//=================================================================================================
/*
enum ChairContents
{
	CHAIR_UNDEFINED = 0,
	CHAIR_EMPTY,
	CHAIR_HUMAN,
	CHAIR_CLOTHES,
	CHAIR_UNKNOWN
};
*/

// Demo image in 'QImage' format.
typedef  TDataWraper< QImage, DATAID_QIMAGE >  TQImage;

// Background image with 'float' entries.
typedef  TDataWraper< Arr2f, DATAID_BACKGROUND >  TBackground;

// Container of base chair structures.
typedef  TDataWraper< BaseChairArr, DATAID_BASE_CHAIR_SET >  TBaseChairSet;

// The binary image of parameters.
typedef  TDataWraper< Int8Arr, DATAID_PARAMETERS >  TParameters;

// The name of log file.
typedef  TDataWraper< std::wstring, DATAID_LOG_FILE_NAME >  TLogFileName;

// The text of the last error.
typedef  TDataWraper< std::wstring, DATAID_LAST_ERROR >  TLastError;

// The map between chair identifier and its contents.
typedef  TDataWraper< std::map<__int32, ::ChairContents>, DATAID_CHAIR_CONTENTS_MAP >  TChairContentsMap;

// base chair array actually used to init camera analyzer
typedef  TDataWraper< BaseChairArr, DATAID_EMPTYHALL_CHAIRSET > TEmptyHallArr;

// The state flag of an object.
typedef  TDataWraper< __int32, DATAID_STATE_FLAG >  TStateFlag;

// Index of a camera.
typedef  TDataWraper< __int32, DATAID_CAMERA_NO >  TCameraNo;

// Identifier of a working scenario.
typedef  TDataWraper< __int32, DATAID_WORKING_SCENARIO >  TWorkingScenario;

// Any flag that tells what to show in demo mode.
typedef  TDataWraper< __int32, DATAID_DEMO_FLAG >  TDemoFlag;

// Assumed duration of cinema show or any surveillance interval in milliseconds.
typedef  TDataWraper< std::pair<__int64,__int64>, DATAID_SURVEILLANCE_INTERVAL >  TSurveillanceInterval; 

// Data transfered from hall analyzer to camera analyzer.
typedef  TDataWraper< Int8Arr, DATAID_HALL_ANALYZER_TO_CAMERA >  THallAnalyzerToCamera;

typedef TDataWraper< bool, DATAID_MSG_IAMREADY >        TMsgIAmReady;
typedef TDataWraper< bool, DATAID_MSG_CLEAN_TEMP_DATA > TMsgCleanTempData;
typedef TDataWraper< bool, DATAID_MSG_LOAD_TEMP_DATA >  TMsgLoadTempData;
typedef TDataWraper< bool, DATAID_MSG_SAVE_TEMP_DATA >  TMsgSaveTempData;
typedef TDataWraper< bool, DATAID_MSG_FINALIZE >        TMsgFinalize;

typedef TDataWraper< bool, DATAID_MSG_SHOW_PARAM_DIALOG >       TMsgShowParamDialog;
typedef TDataWraper< std::wstring, DATAID_MSG_SAVE_PARAMETERS > TMsgSaveParameters;
typedef TDataWraper< std::wstring, DATAID_MSG_LOAD_PARAMETERS > TMsgLoadParameters;
typedef TDataWraper< bool, DATAID_MSG_SET_DEFAULT_PARAMS >      TMsgSetParametersToDefaults;

typedef TDataWraper< std::wstring, DATAID_MSG_SEQUENCE_BEGIN >  TMsgSequenceBegin;
typedef TDataWraper< std::wstring, DATAID_MSG_SEQUENCE_END >    TMsgSequenceEnd;

typedef TDataWraper< std::wstring, DATAID_RESULT_FILENAME >     TResultFilename;


} // namespace csinterface

