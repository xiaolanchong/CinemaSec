/****************************************************************************
  CSAlgoCommon.h
  ---------------------
  begin     : Aug 2004
  modified  : 24 Aug 2005
  author(s) : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

#ifdef CS_ALGO_COMMON_USER_DLL
#define CS_ALGO_COMMON_API __declspec(dllexport)
#else
#define CS_ALGO_COMMON_API __declspec(dllimport)
#endif

#include "interfaces/IBackgroundAccum.h"
#include "interfaces/ICameraAnalyzer.h"
#include "interfaces/IHallAnalyzer.h"
#include "interfaces/IParameterDialog.h"

#include "contents/HResultCodes.h"
#include "contents/Utility.h"
#include "contents/ImageProperty.h"
#include "contents/BaseChair.h"
#include "contents/BaseChairEx.h"
#include "contents/ImageNormalizer.h"
#include "contents/DataTransferTypes.h"
#include "contents/ImageStandardizer.h"
#include "contents/DayNight.h"

namespace csalgocommon
{

// The minimal number of passed frames needed to obtain reliable measurements.
const __int32 MIN_PASSED_FRAMES_NUM = 25;

const wchar_t ALGORITHM[] = L"CSAlgorithm";

extern "C"
{

typedef HRESULT
(*PCreateBackgroundAccumulator)( IBackgroundAccumulator ** ppAccumulator );

typedef HRESULT
(*PCreateCameraAnalyzer)( ICameraAnalyzer ** ppAnalyzer, const Int8Arr * pParameters,
                          const BaseChairArr * pChairs, const Arr2f * pBackground,
                          __int32 cameraNo, IDebugOutput * pOutput );

typedef HRESULT
(*PCreateHallAnalyzer)( IHallAnalyzer ** ppAnalyzer, const Int8Arr * pParameters,
                        const BaseChairArr * pChairs, __int64 startTime, IDebugOutput * pOutput );

typedef HRESULT
(*PShowParameterDialog)( HWND hParentWnd, Int8Arr * pBinParams, IDebugOutput * pDebugOut );

typedef HRESULT
(*PSetDefaultParameters)( Int8Arr * pBinParams, IDebugOutput * pDebugOut );

typedef HRESULT
(*PShowParameterDialogEx)( HWND hParentWnd, Int8Arr * pBinParams, bool bSaveInFile, IDebugOutput * pDebugOut );

typedef HRESULT
(*PSetDefaultParametersEx)( Int8Arr * pBinParams, bool bSaveInFile, IDebugOutput * pDebugOut );

typedef HRESULT
(*PGetParameters)( Int8Arr * pBinParams, IDebugOutput * pOutput );

} // extern "C"

// NoiseMeter.cpp:
float ProcessOnePolymaskNoiseHistogram( int * histogram, const int HISTO_SIZE, const double SCALE );

} // namespace csalgocommon

