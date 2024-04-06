/****************************************************************************
  CSAlgo1.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

#include "../algorithm/hresult_codes.h"

//>>>>>
namespace csalgo {}
namespace csalgo1 = csalgo;
namespace csalgo1
{
  const __int32 MIN_PASSED_FRAMES_NUM = 25;
  const HRESULT HRESULT_DROP_FRAME = csalgocommon::HRESULT_DROP_FRAME;
  const HRESULT HRESULT_SKIP_FRAME = csalgocommon::HRESULT_SKIP_FRAME;
  const HRESULT HRESULT_UNKNOWN_DATA = csalgocommon::HRESULT_UNKNOWN_DATA;
}
//>>>>>


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

HRESULT ShowParameterDialog( HWND           hParentWnd,
                             Int8Arr      * pBinParams,
                             IDebugOutput * pDebugOut );

HRESULT SetDefaultParameters( Int8Arr      * pBinParams,
                              IDebugOutput * pDebugOut );

HRESULT ShowParameterDialogEx( HWND           hParentWnd,
                               Int8Arr      * pBinParams,
                               bool           bSaveInFile,
                               IDebugOutput * pDebugOut );

HRESULT SetDefaultParametersEx( Int8Arr      * pBinParams,
                                bool           bSaveInFile,
                                IDebugOutput * pDebugOut );

HRESULT GetParameters( Int8Arr      * pBinParams,
                       IDebugOutput * pOutput );

bool LoadParameters( LPCWSTR        filename,
                     IDebugOutput * pOutput );

bool SaveParameters( LPCWSTR        filename,
                     IDebugOutput * pOutput );

} // extern "C"

