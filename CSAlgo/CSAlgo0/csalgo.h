/****************************************************************************
  csalgo.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#ifdef CS_ALGO_USER_DLL
#define CS_ALGO_API __declspec(dllexport)
#else
#define CS_ALGO_API __declspec(dllimport)
#endif

#include "../interfaces/camera_analyzer_interface.h"
#include "../interfaces/hall_analyzer_interface.h"
#include "../interfaces/background_accum_interface.h"
#include "../interfaces/parameter_dialog.h"
#include "algorithm/csalgo_hresult_codes.h"
#include "../algorithm/base_chair.h"
#include "algorithm/algo_types.h"
#include "algorithm/algo_data_transfer_types.h"

namespace csalgo
{

// The minimal number of passed frames needed to obtain reliable measurements.
const __int32 MIN_PASSED_FRAMES_NUM = 25;

} // namespace csalgo


extern "C"
{

HRESULT CreateBackgroundAccumulator( IBackgroundAccumulator ** ppAccumulator );

HRESULT CreateCameraAnalyzer( ICameraAnalyzer   ** ppAnalyzer,
                              const Int8Arr      * pParameters,
                              const BaseChairArr * pChairs,
                              const Arr2f        * pBackground,
                              __int32              cameraNo,
                              IDebugOutput       * pDebugOut );

HRESULT CreateHallAnalyzer( IHallAnalyzer     ** ppAnalyzer,
                            const Int8Arr      * pParameters,
                            const BaseChairArr * pChairs,
                            __int64              startTime,
                            IDebugOutput       * pDebugOut );

} // extern "C"

