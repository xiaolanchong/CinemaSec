#pragma once

#include "../algorithm/base_chair.h"
#include "../algorithm/base_chair_ex.h"
#include "../interfaces/camera_analyzer_interface.h"
#include "../interfaces/hall_analyzer_interface.h"
#include "../interfaces/background_accum_interface.h"
#include "../interfaces/parameter_dialog.h"

namespace csalgocommon
{

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


// noise_meter.cpp:
float ProcessOnePolymaskNoiseHistogram( int * histogram, const int HISTO_SIZE, const double SCALE );

} // namespace csalgocommon

